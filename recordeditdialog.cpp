#include <QDebug>
#include <QMessageBox>

#include "recordeditdialog.h"
#include "ui_recordeditdialog.h"
#include "recordsourceeditdialog.h"
#include "sourcesearchdialog.h"
#include "humansearchdialog.h"
#include "placesearchdialog.h"
#include "placeeditdialog.h"
#include "humaneditdialog.h"
#include "geneoptions.h"


RecordEditDialog::RecordEditDialog(HumansDatabase *humansDatabase, QString recordUuid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RecordEditDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() | Qt::WindowMaximizeButtonHint);
    setLayout(ui->verticalLayoutMain);
    ui->tabHumans->setLayout(ui->verticalLayoutHumans);
    ui->tabPlaces->setLayout(ui->verticalLayoutPlaces);
    ui->tabSources->setLayout(ui->verticalLayoutSources);
    ui->textEditRecord->setMinimumWidth(549);
    ui->webViewRecord->setMinimumWidth(549);    

    ui->textEditRecord->setSizePolicy(QSizePolicy::QSizePolicy::Maximum,QSizePolicy::QSizePolicy::Maximum);
    ui->webViewRecord->setSizePolicy(QSizePolicy::QSizePolicy::Maximum,QSizePolicy::QSizePolicy::Maximum);
    ui->tabWidget->setMaximumWidth(350);
    ui->tabWidget->setMinimumWidth(350);
    if(!humansDatabase->isOpen())
    {
        QMessageBox::warning(this,"Внимание","База не открыта!");
        reject();
        return;
    }
    db=humansDatabase;
    recUid=recordUuid;
    recordData=db->getRecord(recUid);
    fillRecordData();
    if(!recordUuid.isEmpty())
    {
        setWindowTitle("Редактирование записи");
        ui->pushButtonOk->setText("Сохранить и закрыть");
        ui->pushButtonSave->setText("Сохранить");
    }
    else
    {
        setWindowTitle("Добавление новой записи");
        ui->pushButtonOk->setText("Добавить и закрыть");
        ui->pushButtonSave->setText("Добавить");
    }


    ui->textEditRecord->setVisible(true);
    ui->webViewRecord->setVisible(false);

    ui->textEditRecord->setAcceptRichText(false);

    connect(ui->listWidgetHumans,SIGNAL(activated(QModelIndex)),this,SLOT(listHumansActivated(QModelIndex)));
    connect(ui->listWidgetPlaces,SIGNAL(activated(QModelIndex)),this,SLOT(listPlacesActivated(QModelIndex)));
    connect(ui->listWidgetSources,SIGNAL(activated(QModelIndex)),this,SLOT(listSourcesActivated(QModelIndex)));

    //qDebug()<<"START EDIT record uid "<<recordData.uuid<<", but "<<recUid<<", but "<< recordUuid;
    updateHumansData();
    updatePlacesData();
    updateRecordSourcesData();

    ui->tabWidget->setCurrentIndex(0);

    ui->textEditRecord->setFont(geneOptions.recordEditFont);
    ui->webViewRecord->setFont(geneOptions.recordEditFont);
}

RecordEditDialog::~RecordEditDialog()
{
    delete ui;
}

QString RecordEditDialog::getTabTable(QString text)
{
    //Разделим на строки.
    QList<QStringList> table;
    QStringList strings=text.split("\n");
    int maxColumnCount=0;
    for(int i=0;i<strings.count();i++)
    {
        //QStringList heho=strings[i].split("\t");
     //   qDebug()<<heho.count();
        table.append(strings[i].split("\t"));
        if(table[i].count()>maxColumnCount) maxColumnCount=table[i].count();
    }

   // return text;

    bool tableFlag=false;
    QString html;
    html.append("<html><body>");
    //html.append("<table cellpadding=1 cellspacing=1 border = \"thick solid black\"><thead></thead><tbody>");

    //!!!html.append("<table cellpadding=\"0\" cellspacing=\"0\" border = \"1\"><thead></thead><tbody>");
    for(int i=0;i<table.count();i++)
    {
        if(table[i].count()>1)
        {
            if(tableFlag==false)
            {
                //html.append("<table cellpadding=1 cellspacing=1 border = \"thick solid black\"><thead></thead><tbody>");
                html.append("<table cellpadding=\"0\" cellspacing=\"0\" border = \"1\"><thead></thead><tbody>");
                tableFlag=true;
            }
            html.append("<tr >");
            for(int j=0;j<table[i].count();j++)
            {
                html.append("<td>"+table[i][j]+"</td>");
            }
            for(int j=table[i].count();j<maxColumnCount;j++)
                html.append("<td></td>");
            html.append("</tr>");
        }
        else
        {
            QString fullString=table[i].join("");
            if(fullString.trimmed().isEmpty())continue;
            if(tableFlag==true)
            {
                tableFlag=false;
                html.append("</tbody></table>");
            }
            html.append("<br>"+fullString);
        }
        /*
        if(table[i].count()<2)
        {
            html.append(table[i].join(" "));
            continue;
        }
        html.append("<tr >");
        for(int j=0;j<table[i].count();j++)
        {
            html.append("<td>"+table[i][j]+"</td>");
        }
        for(int j=table[i].count();j<maxColumnCount;j++)
            html.append("<td></td>");
        html.append("</tr>");
        */
    }
    //html.append("</tbody></table>");
    if(tableFlag==true)html.append("</tbody></table>");

    //qDebug()<<"Найдено строк "<<strings.count();
    //qDebug()<<html;
    html.append("</body></html>");
    return html;
}

void RecordEditDialog::on_pushButtonEdit_clicked()
{
    ui->textEditRecord->setVisible(true);
    ui->webViewRecord->setVisible(false);
    //!QRect rect=ui->webViewRecord->geometry();
    //!ui->textEditRecord->setGeometry(rect.x(),rect.y(),rect.width(),rect.height());
}

void RecordEditDialog::on_pushButtonApply_clicked()
{
    ui->textEditRecord->setVisible(false);
    ui->webViewRecord->setVisible(true);
    //!QRect rect=ui->textEditRecord->geometry();
    //!ui->webViewRecord->setGeometry(rect.x(),rect.y(),rect.width(),rect.height());
    //Далее спец.обработка текста
    QString hoho=getTabTable(ui->textEditRecord->toPlainText());
    //qDebug()<<"Result of table realization:"<<hoho;
    ui->webViewRecord->setHtml(hoho);
    //QTextBrowser herrr;
   // herrr.toHtml()
}

void RecordEditDialog::on_pushButtonCancel_clicked()
{
    reject();
}

void RecordEditDialog::on_pushButtonOk_clicked()
{
    //Собрать данные о записи.
    //Добавить запись или сохранить запись, в зависимости от.
    QStringList errors=collectRecordData();
    if(errors.count()>0)
    {
        QMessageBox::warning(this,"Внимание","Ошибка записи:"+errors.join(",")+".");
        return;
    }
    if(recUid.isEmpty())
    {
        qDebug()<<"Empty record uid "<<recordData.uuid;
        db->addRecord(recordData);
    }
    else
    {
        qDebug()<<"Edit record uid "<<recordData.uuid;
        db->editRecord(recordData);
    }
    accept();
}

void RecordEditDialog::on_pushButtonSave_clicked()
{
    //Собрать данные о записи.
    //Добавить запись или сохранить запись, в зависимости от.
    QStringList errors=collectRecordData();
    if(errors.count()>0)
    {
        QMessageBox::warning(this,"Внимание","Ошибка записи:"+errors.join(",")+".");
        return;
    }
    if(recUid.isEmpty())
    {
        db->addRecord(recordData);
        recUid=recordData.uuid;
    }
    else
    {
        db->editRecord(recordData);
    }

    setWindowTitle("Редактирование записи");
    ui->pushButtonOk->setText("Сохранить и закрыть");
    ui->pushButtonSave->setText("Сохранить");
}

void RecordEditDialog::fillRecordData()
{
    ui->textEditRecord->setText(recordData.record);\
    ui->lineEditDate->setText(recordData.date);
    //ui->comboBoxType->setCurrentText(recordData.type);
    ui->comboBoxType->setEditText(recordData.type);
    SourceData source=db->getSource(recordData.sourceId);
    ui->lineEditSource->setText(source.name);
    ui->lineEditLocation->setText(recordData.location);
    PlaceData place=db->getPlace(recordData.placeId);
    ui->lineEditPlace->setText(place.type+" "+place.name);
    ui->lineEditPlace->setCursorPosition(0);
    ui->textEditNote->setText(recordData.note);
}

QStringList RecordEditDialog::collectRecordData()
{
    QStringList errors;
    recordData.record=ui->textEditRecord->toPlainText();
    recordData.date=ui->lineEditDate->text();
    recordData.type=ui->comboBoxType->currentText();
    //recordData.sourceId//не меняется, выставляется другими способами.
    recordData.location=ui->lineEditLocation->text();
    //recordData.placeId//не меняется тут, выставляется другим способом.
    recordData.note=ui->textEditNote->toPlainText();

    if(recordData.record.isEmpty())
        errors.append("запись пуста");
    return errors;
}

void RecordEditDialog::updateHumansData()
{
    humansData=db->getHumansOfRecordInfo(recordData.uuid);
    ui->listWidgetHumans->clear();
    for(int i=0;i<humansData.count();i++)
    {
        ui->listWidgetHumans->addItem(humansData[i][1]);
    }
}

void RecordEditDialog::updatePlacesData()
{
    placesData=db->getPlacesOfRecordInfo(recordData.uuid);
    ui->listWidgetPlaces->clear();
    for(int i=0;i<placesData.count();i++)
    {
        ui->listWidgetPlaces->addItem(placesData[i][1]);
    }
}

void RecordEditDialog::updateRecordSourcesData()
{  
    sourcesData=db->getSourcesOfRecordInfo(recordData.uuid);   
    ui->listWidgetSources->clear();
    for(int i=0;i<sourcesData.count();i++)
    {
        ui->listWidgetSources->addItem(sourcesData[i][1]);
    }   
}

void RecordEditDialog::on_pushButtonSearchSource_clicked()
{
    SourceSearchDialog dlg(db);
    if(dlg.exec()==QDialog::Rejected) return;
    recordData.sourceId=dlg.getSourceId();
    ui->lineEditSource->setText(dlg.getSourceInfo());
}

void RecordEditDialog::on_pushButtonNewHuman_clicked()
{
    if(recUid.isEmpty())
    {
        QMessageBox::warning(this,"Внимание!","Запись не сохранена, фактически не создана, поэтому привязать к ней людей нельзя.");
        return;
    }
    HumanEditDialog dlg(db);
    if(dlg.exec()==QDialog::Rejected) return;
    QString hUuid=dlg.getHumanUuid();
    if(!hUuid.isEmpty())
    {
        db->addHumanToRecord(recordData.uuid,hUuid);
        updateHumansData();
    }
}

void RecordEditDialog::on_pushButtonSearchHuman_clicked()
{
    //Добавление нового человека к месту...
    if(recUid.isEmpty())
    {
        QMessageBox::warning(this,"Внимание!","Запись не сохранена, фактически не создана, поэтому привязать к ней людей нельзя.");
        return;
    }
    HumanSearchDialog dlg(db);
    if(dlg.exec()==QDialog::Rejected) return;
    QString hUuid=dlg.getHumanUuid();
    if(!hUuid.isEmpty())
    {
        db->addHumanToRecord(recordData.uuid,hUuid);
        updateHumansData();
    }
}

void RecordEditDialog::listHumansActivated(QModelIndex index)
{
    HumanEditDialog dlg(db,humansData[index.row()][0]);
    dlg.exec();
    updateHumansData();
}

void RecordEditDialog::listPlacesActivated(QModelIndex index)
{
    PlaceEditDialog dlg(db,placesData[index.row()][0]);
    dlg.exec();
    updatePlacesData();
}

void RecordEditDialog::listSourcesActivated(QModelIndex index)
{
    RecordSourceEditDialog dlg(db,recordData.uuid,sourcesData[index.row()][0]);
    dlg.exec();
    updateRecordSourcesData();
}

void RecordEditDialog::on_pushButtonRemoveHuman_clicked()
{
    QModelIndex index=ui->listWidgetHumans->currentIndex();
    if(!index.isValid())
    {
        QMessageBox::warning(this,"Внимание","Не выбран человек!");
        return;
    }
    db->deleteHumanFromRecord(recordData.uuid,humansData[index.row()][0]);
    updateHumansData();
}

void RecordEditDialog::on_pushButtonNewPlace_clicked()
{
    if(recUid.isEmpty())
    {
        QMessageBox::warning(this,"Внимание!","Запись не сохранена, фактически не создана, поэтому привязать к ней людей нельзя.");
        return;
    }
    PlaceEditDialog dlg(db);
    if(dlg.exec()==QDialog::Rejected) return;
    QString pUuid=dlg.getPlaceUuid();
    if(!pUuid.isEmpty())
    {
        db->addPlaceToRecord(recordData.uuid,pUuid);
        updatePlacesData();
    }
}

void RecordEditDialog::on_pushButtonSearchPlace_clicked()
{
    if(recUid.isEmpty())
    {
        QMessageBox::warning(this,"Внимание!","Запись не сохранена, фактически не создана, поэтому привязать к ней места нельзя.");
        return;
    }
    PlaceSearchDialog dlg(db);
    if(dlg.exec()==QDialog::Rejected) return;
    QString pUuid=dlg.getPlaceUuid();
    if(!pUuid.isEmpty())
    {
        //qDebug()<<"Before add";
        db->addPlaceToRecord(recordData.uuid,pUuid);
        //qDebug()<<"After add";
        updatePlacesData();
    }
}



void RecordEditDialog::on_pushButtonRemovePlace_clicked()
{
    QModelIndex index=ui->listWidgetPlaces->currentIndex();
    if(!index.isValid())
    {
        QMessageBox::warning(this,"Внимание","Не выбрано место!");
        return;
    }

    if(QMessageBox::question(this,"Подтверждение удаления места","Вы уверены, что хотите удалить место?",QMessageBox::Yes,QMessageBox::No)
            == QMessageBox::No) return;


    db->deletePlaceFromRecord(recordData.uuid,placesData[index.row()][0]);
    updatePlacesData();
}

void RecordEditDialog::on_pushButtonDelete_clicked()
{
    if(recUid.isEmpty())
    {
        QMessageBox::warning(this,"Внимание","Запись ещё не добавлена в базу, так что удалить её нельзя.");
        return;
    }
    QMessageBox msgBox;
    msgBox.setText("Удаление важных данных");
    msgBox.setInformativeText("Вы действительно хотите удалить данные об этой записи?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret)
    {
       case QMessageBox::Ok:
           db->deleteRecord(recUid);
           accept();
           break;
       case QMessageBox::Cancel:
           //Тоже в общем без последствий.
           break;
       default:
           // Ну так, пусть будет.
           break;
     }
}

void RecordEditDialog::on_pushButtonRecordPlaceSearch_clicked()
{
    PlaceSearchDialog dlg(db);
    if(dlg.exec()==QDialog::Rejected) return;
    ui->lineEditPlace->setText(dlg.getPlaceInfo());
    ui->lineEditPlace->setCursorPosition(0);
    recordData.placeId=dlg.getPlaceUuid();
}

void RecordEditDialog::on_pushButtonPlusFont_clicked()
{
    QFont font=ui->textEditRecord->font();
    font.setPointSize(font.pointSize()+1);
    ui->textEditRecord->setFont(font);
    ui->webViewRecord->setFont(font);
    geneOptions.recordEditFont=font;
}

void RecordEditDialog::on_pushButtonMinusFont_clicked()
{
    QFont font=ui->textEditRecord->font();
    if(font.pointSize()>2)
        font.setPointSize(font.pointSize()-1);
    ui->textEditRecord->setFont(font);
    ui->webViewRecord->setFont(font);
    geneOptions.recordEditFont=font;
}

void RecordEditDialog::on_pushButtonNewRecordSource_clicked()
{
    RecordSourceEditDialog dlg(db,recordData.uuid);
    dlg.exec();
    updateRecordSourcesData();
}

void RecordEditDialog::on_pushButtonDeleteRecordSource_clicked()
{
    QModelIndex index=ui->listWidgetSources->currentIndex();
    if(!index.isValid())
    {
        QMessageBox::warning(this,"Внимание","Не выбран источник!");
        return;
    }

    if(QMessageBox::question(this,"Подтверждение удаления источника","Вы уверены, что хотите удалить источник?",QMessageBox::Yes,QMessageBox::No)
            == QMessageBox::No) return;


    db->deleteSourceFromRecord(sourcesData[index.row()][0]);
    updateRecordSourcesData();
}
