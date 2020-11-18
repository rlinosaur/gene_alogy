#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextCodec>
#include <QDateTime>
#include <QSettings>

#include "options.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "humansdatabasecalc.h"
#include "gedcomimportdialog.h"
#include "humansearchdialog.h"
#include "placesearchdialog.h"
#include "recordeditdialog.h"
#include "humaneditdialog.h"
#include "placeeditdialog.h"
#include "visualizedialog.h"
#include "optionsdialog.h"
#include "mapdialog.h"

#include "tablerawdatedelegate.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),delegateTimeToString(new TableRawDateDelegate(this))
{
    ui->setupUi(this);
   // db.deleteDatabase(DEFAULT_DATABASE_NAME);

    QSettings settings(DEFAULT_CONFIG_NAME,QSettings::IniFormat);
    settings.beginGroup(CONFIG_GROUP_SOURCES);
    optionsData.sourceDirectory=settings.value(CONFIG_SOURCES_PATH).toString();
    settings.endGroup();
    settings.beginGroup(CONFIG_GROUP_DATABASE);
    QString dbName=settings.value(CONFIG_DATABASE_FILENAME).toString();
    settings.endGroup();
    if (dbName.isEmpty()) dbName=DEFAULT_DATABASE_NAME;
    db.init(dbName);

    db.createTables();
    db.createIndices();


    //Инициализация внешнего вида.    
    ui->tabWidget->setCurrentIndex(0);//выбраны человеки.
    modelHumansSort.setSourceModel(&modelHumans);
    ui->tableViewHumans->setModel(&modelHumansSort);
    ui->tableViewHumans->setSortingEnabled(true);
    //ui->tableViewHumans->setModel(&modelHumans);
    ui->tableViewHumans->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewHumans->setItemDelegateForColumn(8,&delegateTimeToString);
    ui->tableViewHumans->setItemDelegateForColumn(9,&delegateTimeToString);
    //ui->tableViewHumans->verticalHeader()->setVisible(false);
    connect(ui->tableViewHumans,SIGNAL(activated(QModelIndex)),this,SLOT(tableHumansActivate(QModelIndex)));
    initModelHumans();

    modelRecordsSort.setSourceModel(&modelRecords);
    ui->tableViewRecords->setModel(&modelRecordsSort);
    ui->tableViewRecords->setSortingEnabled(true);
    //ui->tableViewRecords->setModel(&modelRecords);
    ui->tableViewRecords->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewRecords->setItemDelegateForColumn(6,&delegateTimeToString);
    ui->tableViewRecords->setItemDelegateForColumn(7,&delegateTimeToString);
    connect(ui->tableViewRecords,SIGNAL(activated(QModelIndex)),this,SLOT(tableRecordsActivate(QModelIndex)));

    modelPlacesSort.setSourceModel(&modelPlaces);
    ui->tableViewPlaces->setModel(&modelPlacesSort);
    ui->tableViewPlaces->setSortingEnabled(true);
    //ui->tableViewPlaces->setModel(&modelPlaces);
    ui->tableViewPlaces->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(ui->tableViewPlaces,SIGNAL(activated(QModelIndex)),this,SLOT(tablePlacesActivate(QModelIndex)));

    ui->tabHumans->setLayout(ui->verticalLayoutHumans);
    ui->tabPlaces->setLayout(ui->verticalLayoutPlaces);
    ui->tabRecords->setLayout(ui->verticalLayoutRecords);
    ui->tabMisc->setLayout(ui->verticalLayoutMisc);



    //Michellaneous
    connect(ui->listWidgetMisc,SIGNAL(activated(QModelIndex)),this,SLOT(mischListActivated(QModelIndex)));


    //Translations
    QLocale localio;
    if(localio.language()==QLocale::Russian)
        on_actionRussian_triggered();
    else
        on_actionEnglish_triggered();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_GEDCOM_triggered()
{
    if(!db.isOpen())
    {
        QMessageBox::warning(this,"Внимание","Увы, теперь надо сначала открыть базу, чтобы туда чего-нибудь экспортироват...хотя..Ну пусть пока спрашивает.");
        return;
    }
    GedcomImportDialog gimport(&db);
    gimport.exec();
    on_tabWidget_currentChanged(ui->tabWidget->currentIndex());
    //смотрим текущую вкладку...
}


void MainWindow::on_action_close_db_triggered()
{
    if(db.isOpen())db.close();
    modelHumans.clear();
    modelRecords.clear();
    modelPlaces.clear();
    //И какие-нибудь действия с интерфейсом.
}

void MainWindow::on_action_open_db_triggered()
{
    //диалог открытия файла.
    //открытие файла. если неудача - сообщение.
    //Если удача - передача там куда-то я не знаю...модель там, таблица и всё такое.
    if(db.isOpen())
    QMessageBox::warning(this,"Внимание","База данных уже открыта. Она будет закрыта после выбора файла.");
    QString dbFile = QFileDialog::getOpenFileName(this,"Открыть базу данных человеков...","","Database (*.db)",0,QFileDialog::DontUseNativeDialog);
    if (dbFile.isEmpty()) return;
    if(db.isOpen())db.close();
    db.init(dbFile);
    if(!db.isOpen())
    {
        QMessageBox::warning(this,"Внимание!","Файл с базой открыть не удалось. Даже не знаю, как такое могло случиться.");
        return;
    }

    on_tabWidget_currentChanged(ui->tabWidget->currentIndex());
}

void MainWindow::on_action_create_db_triggered()
{
    //При создании надо делать следующее...И я это сейчас сделаю.
    if(db.isOpen())
        QMessageBox::warning(this,"Внимание","База данных уже открыта. Она будет закрыта после выбора файла.");
    QString dbFile = QFileDialog::getSaveFileName(this,"Создать базу данных человеков...","","Database (*.db)");
    if (dbFile.isEmpty()) return;
    if(db.isOpen())db.close();
    db.init(dbFile);
    if(!db.isOpen())
    {
        QMessageBox::warning(this,"Внимание!","Создать базу данных почему-то не удалось!");
        return;
    }
    //Создаём таблицы и индексы. А ты как думал!
    db.createTables();
    db.createIndices();
}

void MainWindow::on_action_exit_triggered()
{
    this->close();
}

void MainWindow::on_pushButtonCreateRecord_clicked()
{
    if(!db.isOpen())
    {
        messageDatabaseClosed();
        return;
    }
    RecordEditDialog dlg(&db);
    dlg.exec();
    initModelRecords();
}

void MainWindow::on_pushButtonSearchHuman_clicked()
{
    if(!db.isOpen())
    {
        messageDatabaseClosed();
        return;
    }
    HumanSearchDialog dlg(&db,humanSexFemale);
    if(dlg.exec()==QDialog::Rejected) return;
    QString uuidd=dlg.getHumanUuid();
    HumanData datte=db.getHuman(uuidd);
    //qDebug()<<"Победа!"<<datte.getHumanInfo();
    //qDebug()<<"Победа была бы не полной без краткого содержания "<<dlg.getHumanInfo();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    switch(index)
    {
        case 0://Humans
            initModelHumans();
            break;
        case 1://records
            initModelRecords();
            break;
        case 2://Places
            initModelPlaces();
            break;
    }
}

void MainWindow::tableHumansActivate(QModelIndex index)
{
    if(!db.isOpen())
    {
        messageDatabaseClosed();
        return;
    }
    HumanEditDialog dlg(&db,modelHumansSort.data(modelHumansSort.index(index.row(),0)).toString());
    dlg.exec();
    initModelHumans();
}

void MainWindow::tableRecordsActivate(QModelIndex index)
{
    if(!db.isOpen())
    {
        messageDatabaseClosed();
        return;
    }
    RecordEditDialog dlg(&db,modelRecordsSort.data(modelRecordsSort.index(index.row(),0)).toString());
    dlg.exec();
    initModelRecords();
}

void MainWindow::tablePlacesActivate(QModelIndex index)
{
    if(!db.isOpen())
    {
        messageDatabaseClosed();
        return;
    }
    PlaceEditDialog dlg(&db,modelPlacesSort.data(modelPlacesSort.index(index.row(),0)).toString());
    dlg.exec();
    initModelPlaces();

}

void MainWindow::initModelHumans()
{
    if(!db.isOpen())return;
    unsigned int flags=0;
    if(ui->checkBoxNoRecordsOfHuman->isChecked()) flags|=COMMONSEARCHFLAG_NORECORDS;
    if(ui->checkBoxNoPlacesOfHuman->isChecked())flags|=COMMONSEARCHFLAG_NOPLACES;
    modelHumans.setQuery(db.getHumanSearchQuery(ui->lineEditHumanSearch->text(),humanSexUndefined,placeIdForSearchHumans,flags),db.getDb());
    ui->tableViewHumans->hideColumn(0);
    ui->tableViewHumans->resizeColumnsToContents();
    ui->labelHumansCount->setText(tr("Человеков:")+db.getHumanSearchQueryCount(ui->lineEditHumanSearch->text(),humanSexUndefined,placeIdForSearchHumans,flags).toString());
}

void MainWindow::initModelRecords()
{
    if(!db.isOpen())return;
    unsigned int flags=0;
    if(ui->checkBoxNoHumansInRecord->isChecked())flags|=COMMONSEARCHFLAG_NOHUMANS;
    if(ui->checkBoxNoPlacesInRecord->isChecked())flags|=COMMONSEARCHFLAG_NOPLACES;
    modelRecords.setQuery(db.getRecordSearchQuery(ui->lineEditRecordSearch->text(),"",placeIdForSearchRecords,flags),db.getDb());
    ui->tableViewRecords->hideColumn(0);    
    ui->tableViewRecords->resizeColumnToContents(1);
    ui->tableViewRecords->resizeColumnToContents(4);
    ui->tableViewRecords->resizeColumnToContents(5);
    ui->tableViewRecords->resizeColumnToContents(6);
    ui->tableViewRecords->resizeColumnToContents(7);
    //ui->tableViewRecords->resizeColumnsToContents();
    //ui->tableViewRecords->resizeRowsToContents();
    ui->labelRecordsCount->setText("Записей:"+db.getRecordSearchQueryCount(ui->lineEditRecordSearch->text(),"",placeIdForSearchRecords,flags).toString());
}

void MainWindow::initModelPlaces()
{
    if(!db.isOpen())return;
    modelPlaces.setQuery(db.getPlaceSearchQuery(ui->lineEditPlaceSearch->text()),db.getDb());
    ui->tableViewPlaces->hideColumn(0);
    ui->tableViewPlaces->resizeColumnToContents(1);
    ui->tableViewPlaces->resizeColumnToContents(2);
    //ui->tableViewPlaces->resizeColumnsToContents();
    //ui->tableViewPlaces->resizeRowsToContents();
    ui->labelPlacesCount->setText("Мест:"+db.getPlaceSearchQueryCount(ui->lineEditPlaceSearch->text()).toString());
}

bool MainWindow::changeTranslator(QTranslator &translationClass, QString translationFileName)
{
    qDebug()<<"translation file name "<<translationFileName;
    qApp->removeTranslator(&translationClass);
    if(translationClass.load(translationFileName)) //could be done through QString("genealogy_trans_%1).arg("language_ISO639_name);
        qApp->installTranslator(&translationClass);
    else
    {
        qDebug()<<"Load translation file "<<translationFileName<<" error.";
        return false;
    }
    return true;
}

void MainWindow::on_pushButtonHumanSearch_clicked()
{
    initModelHumans();
}

void MainWindow::on_lineEditHumanSearch_returnPressed()
{
    initModelHumans();
}

void MainWindow::on_pushButtonCreateIndividual_clicked()
{
    if(!db.isOpen())
    {
        messageDatabaseClosed();
        return;
    }
    HumanEditDialog dlg(&db);
    dlg.exec();
    initModelHumans();
}

void MainWindow::on_pushButtonEditHuman_clicked()
{
    if(!db.isOpen())
    {
        messageDatabaseClosed();
        return;
    }
    QItemSelectionModel *selModel=ui->tableViewHumans->selectionModel();
    QModelIndex index=selModel->currentIndex();
    if(index.row()<0)
    {
        QMessageBox::warning(this,"Внимание","Человек для редактирования не выбран!");
        return;
    }
    HumanEditDialog dlg(&db,modelHumansSort.data(modelHumansSort.index(index.row(),0)).toString());
    dlg.exec();
    initModelHumans();
}

void MainWindow::on_pushButtonEditRecord_clicked()
{
    if(!db.isOpen())
    {
        messageDatabaseClosed();
        return;
    }
    QItemSelectionModel *selModel=ui->tableViewRecords->selectionModel();
    QModelIndex index=selModel->currentIndex();
    if(index.row()<0)
    {
        QMessageBox::warning(this,"Внимание","Запись для редактирования не выбрана!");
        return;
    }
    RecordEditDialog dlg(&db,modelRecordsSort.data(modelRecordsSort.index(index.row(),0)).toString());
    dlg.exec();
    initModelRecords();
}

void MainWindow::on_pushButtonEditPlace_clicked()
{
    if(!db.isOpen())
    {
        messageDatabaseClosed();
        return;
    }
    QItemSelectionModel *selModel=ui->tableViewPlaces->selectionModel();
    QModelIndex index=selModel->currentIndex();
    if(index.row()<0)
    {
        QMessageBox::warning(this,"Внимание","Место для редактирования не выбрано!");
        return;
    }
    PlaceEditDialog dlg(&db,modelPlacesSort.data(modelPlacesSort.index(index.row(),0)).toString());
    dlg.exec();
    initModelPlaces();
}

void MainWindow::on_pushButtonPlaceAdd_clicked()
{
    if(!db.isOpen())
    {
        messageDatabaseClosed();
        return;
    }
    PlaceEditDialog dlg(&db);
    dlg.exec();
    initModelPlaces();
}

void MainWindow::messageDatabaseClosed()
{
    QMessageBox::warning(this,"Внимание","База данных не открыта!");
}

void MainWindow::on_pushButtonDeleteHuman_clicked()
{
    if(!db.isOpen())
    {
        messageDatabaseClosed();
        return;
    }

    QItemSelectionModel *selModel=ui->tableViewHumans->selectionModel();
    QModelIndex index=selModel->currentIndex();
    if(index.row()<0)
    {
        QMessageBox::warning(this,"Внимание","Человек для удаления не выбран!");
        return;
    }
    QString humanId=modelHumansSort.data(modelHumansSort.index(index.row(),0)).toString();
    //qDebug()<<"Searching for humanId "<<humanId;
    QString humanInfo;
    humanInfo.append(modelHumansSort.data(modelHumansSort.index(index.row(),1)).toString());
    QString marname=modelHumansSort.data(modelHumansSort.index(index.row(),2)).toString();
    if(!marname.isEmpty())
        humanInfo.append(" ("+marname+")");
    humanInfo.append(" "+modelHumansSort.data(modelHumansSort.index(index.row(),3)).toString());
    humanInfo.append(" "+modelHumansSort.data(modelHumansSort.index(index.row(),4)).toString());
    QString birthyear=modelHumansSort.data(modelHumansSort.index(index.row(),5)).toString();
    QString deathyear=modelHumansSort.data(modelHumansSort.index(index.row(),6)).toString();
    if(!birthyear.isEmpty() || !deathyear.isEmpty())
    {
        humanInfo.append(" "+birthyear+"-"+deathyear);
    }
    humanInfo.append(" "+modelHumansSort.data(modelHumansSort.index(index.row(),7)).toString());

    QMessageBox msgBox;
    msgBox.setText("Удаление важных данных o "+humanInfo);
    msgBox.setInformativeText("Вы действительно хотите удалить данные человеке?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret)
    {
       case QMessageBox::Ok:
           db.deleteHuman(humanId);
            //qDebug()<<"result of deleting is "<<res<<" for id "<<humanId;
            initModelHumans();
           break;
       case QMessageBox::Cancel:
           //Тоже в общем без последствий.
           break;
       default:
           // Ну так, пусть будет.
           break;
     }

}

void MainWindow::on_pushButtonRecordDelete_clicked()
{
    if(!db.isOpen())
    {
        messageDatabaseClosed();
        return;
    }

    QItemSelectionModel *selModel=ui->tableViewRecords->selectionModel();
    QModelIndex index=selModel->currentIndex();
    if(index.row()<0)
    {
        QMessageBox::warning(this,"Внимание","Запись для удаления не выбрана!");
        return;
    }
    QString recordId=modelPlacesSort.data(modelRecordsSort.index(index.row(),0)).toString();
    //qDebug()<<"Searching for humanId "<<humanId;
    QString recordInfo=modelPlacesSort.data(modelPlacesSort.index(index.row(),1)).toString();
    QMessageBox msgBox;
    msgBox.setText("Удаление важных данных, а именно ценной записи \""+recordInfo+"\"?");
    msgBox.setInformativeText("Вы действительно хотите удалить данные о человеке?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret)
    {
       case QMessageBox::Ok:
           db.deleteRecord(recordId);
            //qDebug()<<"result of deleting is "<<res<<" for id "<<humanId;
            initModelRecords();
           break;
       case QMessageBox::Cancel:
           //Тоже в общем без последствий.
           break;
       default:
           // Ну так, пусть будет.
           break;
     }
}

void MainWindow::on_pushButtonPlaceDelete_clicked()
{
    if(!db.isOpen())
    {
        messageDatabaseClosed();
        return;
    }

    QItemSelectionModel *selModel=ui->tableViewPlaces->selectionModel();
    QModelIndex index=selModel->currentIndex();
    if(index.row()<0)
    {
        QMessageBox::warning(this,"Внимание","Место для удаления не выбрано!");
        return;
    }
    QString placeId=modelPlacesSort.data(modelPlacesSort.index(index.row(),0)).toString();
    //qDebug()<<"Searching for humanId "<<humanId;
    QString placeInfo;
    placeInfo.append(modelPlacesSort.data(modelPlacesSort.index(index.row(),1)).toString());
    placeInfo.append(" "+modelPlacesSort.data(modelPlacesSort.index(index.row(),2)).toString());

    QMessageBox msgBox;
    msgBox.setText("Удаление важных данных o "+placeInfo);
    msgBox.setInformativeText("Вы действительно хотите удалить данные о месте?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret)
    {
       case QMessageBox::Ok:
           db.deletePlace(placeId);
            //qDebug()<<"result of deleting is "<<res<<" for id "<<humanId;
            initModelPlaces();
           break;
       case QMessageBox::Cancel:
           //Тоже в общем без последствий.
           break;
       default:
           // Ну так, пусть будет.
           break;
     }
}

void MainWindow::on_pushButtonRecordSearch_clicked()
{
    //Искать и искать.
    initModelRecords();
}

void MainWindow::on_lineEditRecordSearch_returnPressed()
{
    initModelRecords();
}

void MainWindow::on_pushButtonPlaceSearch_clicked()
{
    initModelPlaces();
}

void MainWindow::on_lineEditPlaceSearch_returnPressed()
{
    initModelPlaces();
}

void MainWindow::on_pushButtonMiscGetHuman_clicked()
{
    HumanSearchDialog dlg(&db);
    dlg.exec();
    miscHumanId=dlg.getHumanUuid();
    ui->lineEditMiscGetHuman->setText(dlg.getHumanInfo());
}


void MainWindow::on_pushButton_clicked()
{
    //Делаем следующее...Берём человека и ищем его предков.
    if(miscHumanId.isEmpty())
    {
        return;
    }
    //
    miscHumanList.clear();
    ui->textEditMisc->clear();
    HumanData core=db.getHuman(miscHumanId);
    miscHumanList.append(core);
    HumansDatabaseCalc::recursiveAncestrySearch(&db,&miscHumanList,0,miscHumanList.size()-1);

    //qSort(miscHumanList.begin(),miscHumanList.end(),HumanData::sortHumanLevelLessThan);
    std::sort(miscHumanList.begin(),miscHumanList.end(),HumanData::sortHumanLevelLessThan);
    ui->textEditMisc->append("Всего человеков после рекурсивной функции у нас тут: "+QString::number(miscHumanList.size()));
    ui->listWidgetMisc->clear();    
    int maxLevel=9000;
    if(!ui->lineEditLevel->text().isEmpty())
    {
        maxLevel=ui->lineEditLevel->text().toInt();
        ui->textEditMisc->append("Показаны только предки до поколения "+ui->lineEditLevel->text());
    }
    int humCount=0;
    for(int i=0;i<miscHumanList.size();i++)
    {
        QStringList problems;
        if(miscHumanList[i].recursyList.size()>maxLevel) continue;
        humCount++;
        if(miscHumanList[i].father.isNull())problems.append("нет отца");
        if(miscHumanList[i].mother.isNull())problems.append("нет матери");
        ui->textEditMisc->append(miscHumanList[i].getHumanInfo()+", поколение "+QString::number(miscHumanList[i].recursyList.size())+", "+problems.join(", "));

        QListWidgetItem *item = new QListWidgetItem();
        item->setText(miscHumanList[i].getHumanInfo());
        item->setData(Qt::UserRole,miscHumanList[i].uuid.toString());
        ui->listWidgetMisc->addItem(item);
    }

    ui->textEditMisc->append("Всего людей найдено и показано: "+QString::number(humCount));
}

void MainWindow::on_pushButtonMiscGetCousin_clicked()
{
    HumanSearchDialog dlg(&db);
    dlg.exec();
    miscCousinId=dlg.getHumanUuid();
    ui->lineEditMiscGetCousin->setText(dlg.getHumanInfo());
}

void MainWindow::on_pushButtonSearchCommon_clicked()
{
    if(miscHumanList.isEmpty())
    {
        QMessageBox::warning(this,"Внимание!","Искать пока нечего, надо организовать дерево.");
        return;
    }

    int level=ui->lineEditLevel->text().toInt();

    ui->textEditMisc->clear();
    ui->textEditMisc->append("Список предков, для которых не хватает информации на "+QString::number(level)+" поколений в прошлое.");
    ui->listWidgetMisc->clear();
    int problemsCount=0;

    //qSort(miscHumanList.begin(),miscHumanList.end(),HumanData::sortHumanLevelLessThan);
    std::sort(miscHumanList.begin(),miscHumanList.end(),HumanData::sortHumanLevelLessThan);
    for(int i=0;i<miscHumanList.size();i++)
    {
        if(miscHumanList[i].recursyList.size()>level)continue;
        QStringList problems;
        if(miscHumanList[i].father.isNull()) problems.append("нет отца");
        if(miscHumanList[i].mother.isNull()) problems.append("нет матери");
        if(problems.size()>0)
        {
            QString txt=miscHumanList[i].getHumanInfo()+", поколение "+QString::number(miscHumanList[i].recursyList.size())+" : "+problems.join(", ");
            ui->textEditMisc->append(txt);
            QListWidgetItem *item = new QListWidgetItem();
            item->setText(txt);
            item->setData(Qt::UserRole,miscHumanList[i].uuid.toString());
            ui->listWidgetMisc->addItem(item);
            problemsCount++;
        }
    }

    ui->textEditMisc->append("Итого искать надо: "+QString::number(problemsCount)+" человек.");


}

void MainWindow::on_pushButtonMiscSearchAllCommon_clicked()
{
    if(miscHumanId.isEmpty() || miscCousinId.isEmpty())
    {
        return;
    }

    //Теперь ищем, раз оба существуют.
    miscHumanList.clear();
    HumanData core=db.getHuman(miscHumanId);
    miscHumanList.append(core);
    HumansDatabaseCalc::recursiveAncestrySearch(&db,&miscHumanList,0,miscHumanList.size()-1);

    miscCousinList.clear();
    HumanData cousinCore=db.getHuman(miscCousinId);
    miscCousinList.append(cousinCore);
    HumansDatabaseCalc::recursiveAncestrySearch(&db,&miscCousinList,0,miscCousinList.size()-1);

    //Ну а далее просто поиск общих предков...отдельный список, например.

    QList<HumanData> commonAncestry;
    int minWomanAncestryCount=-1;
    int minManAncestryCount=-1;
    int minWomanAncestry=miscHumanList.size()+miscCousinList.size();
    int minManAncestry=miscHumanList.size()+miscCousinList.size();

    qDebug()<<"Щасвернуся!! Начинаемс читать!";
    for(int i=0;i<miscHumanList.size();i++)
    {
        for(int j=0;j<miscCousinList.size();j++)
        {
            if(miscHumanList[i].uuid==miscCousinList[j].uuid)
            {
                commonAncestry.append(miscHumanList[i]);
                int ancestrySum=miscHumanList[i].recursyList.size()+miscCousinList[j].recursyList.size();
                qDebug()<<"ancestry Sum"<<ancestrySum<<"min1 "<<minWomanAncestry<<", min2 "<<minManAncestry;
                if(miscHumanList[i].sex==humanSexFemale && ancestrySum<minWomanAncestry && ancestrySum>0)
                {
                    minWomanAncestry=ancestrySum;
                    minWomanAncestryCount=commonAncestry.size()-1;
                }
                if(miscHumanList[i].sex==humanSexMale && ancestrySum<minManAncestry && ancestrySum>0)
                {
                    minManAncestry=ancestrySum;
                    minManAncestryCount=commonAncestry.size()-1;
                }

            }
        }
    }
    qDebug()<<"Посчитали, будем отобращать:"<<minWomanAncestry<<", "<<minManAncestry;
    qDebug()<<"Посчитали, будем отобращать:"<<minWomanAncestryCount<<", "<<minManAncestryCount;
    ui->textEditMisc->append("Всего общих родственников: "+QString::number(commonAncestry.size()));
    if(minManAncestryCount>=0)
    ui->textEditMisc->append("Ближайший родственник мужчина (поколений "+QString::number(commonAncestry[minManAncestryCount].recursyList.size())+"): "+commonAncestry[minManAncestryCount].getHumanInfo());
    if(minWomanAncestryCount>=0)
    ui->textEditMisc->append("Ближайший родственник женщина (поколений "+QString::number(commonAncestry[minWomanAncestryCount].recursyList.size())+"): "+commonAncestry[minWomanAncestryCount].getHumanInfo());
    ui->listWidgetMisc->clear();
    for(int i=0;i<commonAncestry.size();i++)
    {
        ui->textEditMisc->append(commonAncestry[i].getHumanInfo());

        QListWidgetItem *item = new QListWidgetItem();
        item->setText(commonAncestry[i].getHumanInfo());
        item->setData(Qt::UserRole,commonAncestry[i].uuid.toString());
        ui->listWidgetMisc->addItem(item);
    }
}

void MainWindow::on_pushButtonMiscTree_clicked()
{

    if(miscHumanList.isEmpty())
    {
        QMessageBox::warning(this,"Внимание!","Визуализировать нечего, надо организовать дерево.");
        return;
    }
    VisualizeDialog dlg;
    dlg.visualizeTree(&miscHumanList);
    dlg.exec();
}

void MainWindow::mischListActivated(QModelIndex index)
{
    QString selectedHumanUid=ui->listWidgetMisc->item(index.row())->data(Qt::UserRole).toString();
    HumanEditDialog dlg(&db,selectedHumanUid);
    dlg.exec();
}

void MainWindow::on_pushButtonSearchDuplicates_clicked()
{
    if(miscHumanList.isEmpty())
    {
        QMessageBox::warning(this,"Внимание!","Искать пока нечего, надо организовать дерево (выбрать человеков и поиск предков).");
        return;
    }
    QList<HumanData> dups;
    QSet<QUuid> set;
    QSet<QUuid> bublicats;
    //QSetIterator<QWidget *> j(set);
    //iterator vasia;
    int setSizeOld=0;
    int setSizeNew=0;
    for(int i=0;i<miscHumanList.size();i++)
    {

        set.insert(miscHumanList[i].uuid);
        setSizeNew=set.size();
        if(setSizeNew==setSizeOld)
        {
            //не добавился идентификатор. бубликат.
            //dups.append(miscHumanList[i]);
            bublicats.insert(miscHumanList[i].uuid);
        }
        setSizeOld=setSizeNew;

    }
    ui->textEditMisc->clear();
    ui->listWidgetMisc->clear();
    ui->textEditMisc->append("Бубликатов уникальных:"+QString::number(bublicats.size()));

    QList<QUuid> bublist=bublicats.values();//Так как с итераторами я пока и вообще не очень.
    for(int i=0;i<miscHumanList.size();i++)
    {
        for(int j=0;j<bublicats.size();j++)
        {
            if(bublist[j]==miscHumanList[i].uuid)
            {
                dups.append(miscHumanList[i]);
                break;
            }
        }
    }


    std::sort(dups.begin(),dups.end(),HumanData::sortHumanNameLessThan);
    for(int i=0;i<dups.size();i++)
    {
        ui->textEditMisc->append(dups[i].getHumanInfo());
        ui->textEditMisc->append("И его потомки:");

        for(int j=dups[i].descendantsInfo.size()-1;j>=0;j--)
        {
          ui->textEditMisc->append(dups[i].descendantsInfo[j]);
        }
        //ui->textEditMisc->append(dups[i].descendantsInfo.join("\n"));
        ui->textEditMisc->append("\n The End \n");

        QListWidgetItem *item = new QListWidgetItem();
        item->setText(dups[i].getHumanInfo());
        item->setData(Qt::UserRole,dups[i].uuid.toString());
        ui->listWidgetMisc->addItem(item);
    }
    ui->textEditMisc->append("Бубликатов всего:"+QString::number(dups.size()));



    /*
    QSet
    iterator QSet::insert(const T & value)
    Inserts item value into the set, if value isn't already in the set, and returns an iterator pointing at the inserted item.
    */
}

void MainWindow::on_pushButtonGedcomExport_clicked()
{

    //unsigned int exportFlags=0;//добавлять/не добавлять комментарии и всё такое прочее...
    if(miscHumanList.isEmpty())
    {
        QMessageBox::warning(this,"Внимание!","Сначала надо найти предков. А уж потом и экспортировать дерево в Gedcom.");
        return;
    }
    QString gedFile = QFileDialog::getSaveFileName(this,"Создать файл Gedcom...","","Gedcom file (*.ged)");
    if (gedFile.isEmpty()) return;

    QFile f(gedFile);
    if (!f.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this,"Внимание!","Ошибка открытия файла "+gedFile+" для записи.");
        return;
    }
    QTextStream outStream(&f);
    outStream.setCodec(QTextCodec::codecForName("UTF-8"));

    outStream<<"0 HEAD\n1 GEDC\n2 VERS 5.5\n2 FORM LINEAGE-LINKED\n1 CHAR UTF-8\n1 LANG Russian\n1 SOUR GENE_ALOGY\n2 NAME rLin's genealogy tool\n2 VERS 1.0\n2 _RTLSAVE RTL\n2 CORP None\n";
    outStream<<"1 DEST GENE_ALOGY\n1 DATE "<<QDateTime::currentDateTime().toString()<<"\n1 FILE Exported by rlin's genealogy tool\n1 _PROJECT_GUID None\n1 EXPORTED_FROM_SITE_ID 000000000";

    QDateTime opTime;
    /*
    QSet<QUuid> set;
    int setSizeOld=0;
    int setSizeNew=0;
    QList<HumanData> exportList;
    //Убираем дубликатов/бубликатов. Кстати, я рад, что нашёл нескольких
    qDebug()<<"Remove bublicates:";
    for(int i=0;i<miscHumanList.size();i++)
    {
        set.insert(miscHumanList[i].uuid);
        setSizeNew=set.size();
        if(setSizeNew==setSizeOld)
            continue;//не экспортируем этого человека, так как он уже есть в списке у нас. Заодно в конце множество UUID получим (хотя лучше б строк c UUID;
        setSizeOld=setSizeNew;
        exportList.append(miscHumanList[i]);
    }
    */
    QList<HumanData> exportList=HumansDatabaseCalc::removeHumanDuplicates(miscHumanList);
    //qDebug()<<"List size:"<<exportList.size();
    //qDebug()<<"Find parents:";
    //Находим всех родителей (точнее их номера в списке, этого в принципе достаточно, ага).
    //А заодно указываем места рождения и жительства...
    for(int i=0;i<exportList.size();i++)
    {        
        bool fatherFound=false;
        if(exportList[i].father.isNull())fatherFound=true;
        bool motherFound=false;
        if(exportList[i].mother.isNull())motherFound=true;
        //qDebug()<<"Searching for pair "<<exportList[i].father<<", "<<exportList[i].mother;
        for(int j=0;j<exportList.size();j++)
        {
            if(fatherFound && motherFound)
            {
                j=exportList.size();
                continue;
            }
            if(exportList[j].uuid==exportList[i].father)
            {
                exportList[i].fatherListCount=j;
                fatherFound=true;
                continue;
            }
            if(exportList[j].uuid==exportList[i].mother)
            {
                exportList[i].motherListCount=j;
                motherFound=true;
                continue;
            }
        }        
        //Сначала место рождения.
        //qDebug()<<"search birthplace for "<<exportList[i].birthPlaceId;
        if(!exportList[i].birthPlaceId.isEmpty())
        {
            exportList[i].birthPlaceId=db.getPlace(exportList[i].birthPlaceId).getPlaceInfo();
        }
        //qDebug()<<"search occupations";
        exportList[i].occupations=db.getHumanPlacesInfoForExport(exportList[i].uuid.toString());
        //qDebug()<<"end search occupations";
        //а куда их запихнуть?? Гм...вот не знаю.
    }

    QList<FamilyData> familyList;

    //Создаём фамилии, однако.
    //qDebug()<<"Prepare families:";
    for(int i=0;i<exportList.size();i++)
    {
        //проходимся по каждому, чтобы создать семью.
        bool familyConnection=false;
        //qDebug()<<"Prepare to find family for a human "<<exportList[i].father<<",";
        for( int j=0;j<familyList.size();j++)
        {
            if(exportList[i].father==familyList[j].husband && exportList[i].mother==familyList[j].wife)
            {
                //add child to family...
                familyList[j].children.append(i);
                familyConnection=true;
                j=familyList.size();
                continue;
            }
        }
        //qDebug()<<"Iteration "<<i<<"conflag "<<familyConnection;
        if(familyConnection==false)
        {
            int newFamilyCount=familyList.size();
            //Создаём новую семью.
  //          qDebug()<<"New family is coming!";
            FamilyData newFamily;
            int fatherCount=exportList[i].fatherListCount;
            newFamily.husbandListCount=fatherCount;
            newFamily.husband=exportList[i].father;
            int motherCount=exportList[i].motherListCount;
            newFamily.wifeListCount=motherCount;
            newFamily.wife=exportList[i].mother;
            //Теперь самим родителям в класс надо добавить информацию, что они партнеры в семье.
          //  qDebug()<<"append fatherCount "<<fatherCount;
//            qDebug()<<"append motherCount "<<motherCount;
            if(fatherCount>=0)exportList[fatherCount].partners.append(newFamilyCount);
            if(motherCount>=0)exportList[motherCount].partners.append(newFamilyCount);
            //Ну и, наконец, добавляем в семью ребёнка.
            newFamily.children.append(i);//Вроде всё.
            exportList[i].children.append(newFamilyCount);
            familyList.append(newFamily);
            //qDebug()<<"end of append new family";
        }
    }

    //К этому моменту мы: избавились от дубликатов, создали класс семьями. Молодцы. Далее создаём экспортный файл.
    //Добавляем человеков
    //qDebug()<<"Humans:";
    for(int i=0;i<exportList.size();i++)
    {
        outStream<<"\n0 @I"<<QString::number(i)<<"@ INDI";
        opTime.setTime_t(exportList[i].changed);
        outStream<<"\n1 _UPD "<<opTime.toString();
        outStream<<"\n1 NAME "<<exportList[i].givenName<<" /"<<exportList[i].surName<<"/";
        outStream<<"\n2 GIVN "<<exportList[i].givenName;
        if(!exportList[i].surName.isEmpty())
            outStream<<"\n2 SURN "<<exportList[i].surName;
        if(!exportList[i].marName.isEmpty())
            outStream<<"\n2 _MARNM "<<exportList[i].marName;
        if(exportList[i].sex==humanSexFemale)
            outStream<<"\n1 SEX F";
        else
            outStream<<"\n1 SEX M";
        if(!exportList[i].birthDate.isEmpty())
        {
            outStream<<"\n1 BIRT\n2 DATE "<<exportList[i].birthDate;
            if(!exportList[i].birthPlaceId.isEmpty()) outStream<<"\n2 PLAC "<<exportList[i].birthPlaceId;
        }
        if(!exportList[i].deathDate.isEmpty())
            outStream<<"\n1 DEAT Y\n2 DATE "<<exportList[i].deathDate;
        else
        {
            if((QDateTime::currentDateTime().date().year()-exportList[i].birthYear)>100)
                outStream<<"\n1 DEAT Y";
        }

        for(int j=0;j<exportList[i].occupations.size();j++)
        {
            outStream<<"\n1 RESI";
            outStream<<"\n2 DATE "<<exportList[i].occupations[j][1];
            outStream<<"\n2 ADDR";
            outStream<<"\n3 CITY "<<exportList[i].occupations[j][0];
        }
        if(!exportList[i].partners.isEmpty())
        {
            for(int j=0;j<exportList[i].partners.size();j++)
                outStream<<"\n1 FAMS @F"+QString::number(exportList[i].partners[j])+"@";

        }
        if(!exportList[i].children.isEmpty())
        {
            for(int j=0;j<exportList[i].children.size();j++)
                outStream<<"\n1 FAMC @F"+QString::number(exportList[i].children[j])+"@";
        }
        outStream<<"\n1 RIN MH:I"<<QString::number(i);
        outStream<<"\n1 _UID "<<exportList[i].uuid.toString().remove(QRegExp("[^0-9a-fA-F]"));
    }
    //Добавляем семьи (новинка, кстати).
    //qDebug()<<"Families:";
    QString updateTime=QDateTime::currentDateTime().toString();
    for(int i=0;i<familyList.size();i++)
    {
        outStream<<"\n0 @F"<<QString::number(i)<<"@ FAM";
        outStream<<"\n1 _UPD "<<updateTime;
        if(familyList[i].husbandListCount!=-1)
            outStream<<"\n1 HUSB @I"<<QString::number(familyList[i].husbandListCount)<<"@";
        if(familyList[i].wifeListCount!=-1)
            outStream<<"\n1 WIFE @I"<<QString::number(familyList[i].wifeListCount)<<"@";
        for(int j=0;j<familyList[i].children.size();j++)
        {
            //хотя по нашей схеме детей всегда один, если это дерево, ну да пофиг, ладно. На будущее.
            outStream<<"\n1 CHIL @"<<QString::number(familyList[i].children[j])<<"@";
        }
        outStream<<"\n1 RIN MH:F"<<QString::number(i);
        outStream<<"\n1 _UID "<<familyList[i].uuid.toString().remove(QRegExp("[^0-9a-fA-F]"));
    }

    outStream<<"\n";
    outStream<<"0 TRLR";
    f.close();

    QMessageBox::warning(this,"Внимание!","Экспорт завершён.");
}

void MainWindow::on_pushButtonPlaceForRecord_clicked()
{
    PlaceSearchDialog dlg(&db);
    if(dlg.exec()==QDialog::Rejected) return;
    ui->lineEditPlaceForRecord->setText(dlg.getPlaceInfo());
    placeIdForSearchRecords=dlg.getPlaceUuid();
}

void MainWindow::on_pushButtonPlaceForRecordClear_clicked()
{
    ui->lineEditPlaceForRecord->clear();
    placeIdForSearchRecords.clear();
}

void MainWindow::on_pushButtonPlaceForHumansSearch_clicked()
{
    PlaceSearchDialog dlg(&db);
    if(dlg.exec()==QDialog::Rejected) return;
    ui->lineEditPlaceForHuman->setText(dlg.getPlaceInfo());
    placeIdForSearchHumans=dlg.getPlaceUuid();
}

void MainWindow::on_pushButtonPlaceForHumansClear_clicked()
{
    ui->lineEditPlaceForHuman->clear();
    placeIdForSearchHumans.clear();
}

void MainWindow::on_pushButtonFindRelatives_clicked()
{
    //Делаем следующее...Берём человека и ищем его предков.
    if(miscHumanId.isEmpty())
    {
        return;
    }
    miscHumanList.clear();
    ui->textEditMisc->clear();
    HumanData core=db.getHuman(miscHumanId);
    miscHumanList.append(core);
    HumansDatabaseCalc::recursiveRelativesSearch(&db,&miscHumanList,0,miscHumanList.size()-1,15*2,true,true);
    //Убираем бубликатов. Несколько странным способом, но что делать...Кстати, его надо бы, наверное как-то в функцию загнать что ли...
    miscHumanList=HumansDatabaseCalc::removeHumanDuplicates(miscHumanList);
    //qSort(miscHumanList.begin(),miscHumanList.end(),HumanData::sortHumanLevelLessThan);
    std::sort(miscHumanList.begin(),miscHumanList.end(),HumanData::sortHumanLevelLessThan);
    ui->textEditMisc->append("Всего человеков после рекурсивной функции у нас тут: "+QString::number(miscHumanList.size()));
    ui->listWidgetMisc->clear();
    int maxLevel=9000;
    if(!ui->lineEditLevel->text().isEmpty())
    {
        maxLevel=ui->lineEditLevel->text().toInt();
        ui->textEditMisc->append("Показаны только предки до поколения "+ui->lineEditLevel->text());
    }
    int humCount=0;
    for(int i=0;i<miscHumanList.size();i++)
    {
        QStringList problems;
        if(miscHumanList[i].recursyList.size()>maxLevel) continue;
        humCount++;
        if(miscHumanList[i].father.isNull())problems.append("нет отца");
        if(miscHumanList[i].mother.isNull())problems.append("нет матери");
        ui->textEditMisc->append(miscHumanList[i].getHumanInfo()+", поколение "+QString::number(miscHumanList[i].recursyList.size())+", "+problems.join(", "));

        QListWidgetItem *item = new QListWidgetItem();
        item->setText(miscHumanList[i].getHumanInfo());
        item->setData(Qt::UserRole,miscHumanList[i].uuid.toString());
        ui->listWidgetMisc->addItem(item);
    }

    ui->textEditMisc->append("Всего людей найдено и показано: "+QString::number(humCount));
}

void MainWindow::on_actionEnglish_triggered()
{
    ui->actionEnglish->setChecked(true);
    ui->actionRussian->setChecked(false);
    //retranslate to English...How...Let's try to figure out.
   // QLocale localio;
    //qDebug()<<"QLanguage firsttt! "<<localio.language();

    QLocale locale=QLocale(QLocale::English);
    QLocale::setDefault(locale);

    changeTranslator(translator,QString("languages/gene_alogy_translation_en.qm"));
    //changeTranslator(translatorQt,QString("languages/qt_en.qm"));
    ui->retranslateUi(this);



}

void MainWindow::on_actionRussian_triggered()
{
    ui->actionEnglish->setChecked(false);
    ui->actionRussian->setChecked(true);
    //retranslate to Russian...
    //Yes, you need something universal for every language...
    QLocale locale=QLocale(QLocale::Russian);
    QLocale::setDefault(locale);

    changeTranslator(translator,QString("languages/gene_alogy_translation_ru.qm"));
    //changeTranslator(translatorQt,QString("languages/qt_ru.qm"));
     ui->retranslateUi(this);

}

void MainWindow::on_action_about_triggered()
{
    QMessageBox::warning(this,tr("О программе"),"Gene_alogy.\nThe software helps you to manage your genealogy records.\n rLinosaur.2016. ");
}

void MainWindow::on_action_triggered()
{
    //Открыть окно настроек. Как бы. щас вспомним, как я это делаю обычно.
    OptionsDialog dlg;
    if(dlg.exec()==QDialog::Rejected) return;

    if(db.isOpen())db.close();
    modelHumans.clear();
    modelRecords.clear();
    modelPlaces.clear();
    //И какие-нибудь действия с интерфейсом.
    QSettings settings("config.ini",QSettings::IniFormat);

    settings.beginGroup("Database");
    QString dbFile=settings.value("FileName").toString();
    settings.endGroup();

    if (dbFile.isEmpty())
    {
        QMessageBox::warning(this,"Внимание!","Имя файла базы отсутствует. Открыть его не удалось.");
        return;
    }
    if(db.isOpen())db.close();
    db.init(dbFile);
    if(!db.isOpen())
    {
        QMessageBox::warning(this,"Внимание!","Файл с базой открыть не удалось. Даже не знаю, ка такое могло случиться.");
        return;
    }

    on_tabWidget_currentChanged(ui->tabWidget->currentIndex());
}

void MainWindow::on_pushButtonMap_clicked()
{
    MapDialog dlg(&db);
    //хотя, как ты понимаешь, мы можем использовать бд по умолчанию и вообще ничего не передавать..ну да ладно.
    //Тут бы надо бы передать список объектов откуда-нибудь. Напишем спец.функцию.
    dlg.exec();
}
