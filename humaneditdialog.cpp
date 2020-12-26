#include <QDebug>
#include <QMessageBox>
#include <QListWidget>

#include "humaneditdialog.h"
#include "ui_humaneditdialog.h"

#include "humanplacedialog.h"
#include "humansearchdialog.h"
#include "placesearchdialog.h"
#include "marriageeditdialog.h"
#include "recordsearchdialog.h"
#include "geneoptions.h"

HumanEditDialog::HumanEditDialog(HumansDatabase &humansDatabase, QString humanUuid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HumanEditDialog),
    db(humansDatabase)
{
    ui->setupUi(this);

    if(!humansDatabase.isOpen())
    {
        QMessageBox::warning(this,"Внимание","База не открыта!");
    }

    this->setWindowFlags(this->windowFlags() | Qt::WindowMaximizeButtonHint);

    //db=humansDatabase;
    hUid=humanUuid;
    humanData=db.getHuman(hUid);//хотя в пустоте даст пустоту.
    if(!hUid.isEmpty())
    {
        setWindowTitle("Редактирование человека "+humanData.getHumanInfo());
        ui->pushButtonOk->setText("Сохранить и закрыть");
        ui->pushButtonSave->setText("Сохранить");
    }
    else
    {
        setWindowTitle("Добавление нового человека");
        ui->pushButtonOk->setText("Добавить и закрыть");
        ui->pushButtonSave->setText("Добавить");
    }
    ui->lineEditFather->setReadOnly(true);
    ui->lineEditMother->setReadOnly(true);

    ui->labelDeathDate->setEnabled(false);
    ui->lineEditDeathDate->setEnabled(false);
    ui->labelDeathCause->setEnabled(false);
    ui->lineEditDeathCause->setEnabled(false);

    ui->comboBoxSex->addItem("женский");
    ui->comboBoxSex->addItem("мужской");
    ui->comboBoxSex->addItem("не указан");
    fillHumanData();    
    connect(ui->listWidgetChildren,SIGNAL(activated(QModelIndex)),this,SLOT(listChildrenActivate(QModelIndex)));
    connect(ui->listWidgetSpouses,SIGNAL(activated(QModelIndex)),this,SLOT(listSpousesActivate(QModelIndex)));
    connect(ui->listWidgetPlaces,SIGNAL(activated(QModelIndex)),this,SLOT(listPlacesActivate(QModelIndex)));
    connect(ui->listWidgetSiblings,SIGNAL(activated(QModelIndex)),this,SLOT(listSiblingsActivate(QModelIndex)));

}

HumanEditDialog::~HumanEditDialog()
{
    delete ui;
}

void HumanEditDialog::addFatherInfo(QString fatherUid)
{
    HumanData father=db.getHuman(fatherUid);
    humanData.father=father.uuid;
    ui->lineEditFather->setText(father.getHumanInfo());
    ui->lineEditFather->setToolTip(ui->lineEditFather->text());
    ui->lineEditFather->setCursorPosition(0);
}

void HumanEditDialog::addMotherInfo(QString motherUid)
{
    HumanData mother=db.getHuman(motherUid);
    humanData.mother=mother.uuid;
    ui->lineEditMother->setText(mother.getHumanInfo());
    ui->lineEditMother->setToolTip(ui->lineEditMother->text());
    ui->lineEditMother->setCursorPosition(0);
}

void HumanEditDialog::setSurname(QString surname)
{
    ui->lineEditSurName->setText(surname);
}

void HumanEditDialog::setPatrynomic(QString patrynomic)
{
    ui->lineEditPatronymic->setText(patrynomic);
}

QStringList HumanEditDialog::collectHumanData()
{
    QStringList error;

    humanData.givenName=ui->lineEditName->text();
    humanData.patronymicName=ui->lineEditPatronymic->text();
    humanData.surName= ui->lineEditSurName->text();
    humanData.marName=ui->lineEditMarName->text();

    humanData.birthDate=ui->lineEditBirthdate->text();
    //!!! ui->lineEditBirthPlace->setText(humanData.birthPlace);Место рождения пока не меняем, пусть как есть.
    humanData.deathDate=ui->lineEditDeathDate->text();
    if(ui->checkBoxLife->isChecked())
    {
        humanData.deathDate.clear();
        humanData.deathCause.clear();
    }
    else
    {
        humanData.deathCause=ui->lineEditDeathCause->text();
        humanData.deathDate=ui->lineEditDeathDate->text();
    }
    humanData.occupation=ui->lineEditOccupation->text();
    switch(ui->comboBoxSex->currentIndex())
    {
        case 0:
            humanData.sex=humanSexFemale;
        break;
        case 1:
            humanData.sex=humanSexMale;
        break;
        case 2:
            humanData.sex=humanSexUndefined;
        break;
    }
    if(humanData.sex==humanSexUndefined)
        error.append("пол не указан");
    humanData.note.clear();
    humanData.note.append(ui->textEditNote->toPlainText());
    return error;
}

void HumanEditDialog::on_pushButtonSearchFather_clicked()
{
    HumanSearchDialog dlg(db,humanSexMale);    
    if(dlg.exec()==QDialog::Rejected) return;
    if(dlg.getHumanUuid().isEmpty()) return;
    QString fatherUuid=dlg.getHumanUuid();
    humanData.father=HumansDatabase::getUuidFromString(fatherUuid);
    ui->lineEditFather->setText(dlg.getHumanInfo());
    ui->lineEditFather->setCursorPosition(0);
    ui->lineEditFather->setToolTip(ui->lineEditFather->text());
    updateSiblingsData();
}

void HumanEditDialog::on_pushButtonSearchMother_clicked()
{
    HumanSearchDialog dlg(db,humanSexFemale);
    if(dlg.exec()==QDialog::Rejected) return;
    if(dlg.getHumanUuid().isEmpty()) return;
    QString motherUuid=dlg.getHumanUuid();
    humanData.mother=HumansDatabase::getUuidFromString(motherUuid);
    ui->lineEditMother->setText(dlg.getHumanInfo());
    ui->lineEditMother->setCursorPosition(0);
    ui->lineEditMother->setToolTip(dlg.getHumanInfo());
    updateSiblingsData();

}

void HumanEditDialog::fillHumanData()
{
    //Заполнить данные из HumanData, если там чего есть, конечно...
    ui->lineEditName->setText(humanData.givenName);
    ui->lineEditPatronymic->setText(humanData.patronymicName);
    ui->lineEditSurName->setText(humanData.surName);
    ui->lineEditMarName->setText(humanData.marName);
    if(!humanData.father.isNull())
    {
        HumanData father=db.getHuman(humanData.father.toString());
        ui->lineEditFather->setText(father.getHumanInfo());
        ui->lineEditFather->setCursorPosition(0);
        ui->lineEditFather->setToolTip(ui->lineEditFather->text());
    }
    if(!humanData.mother.isNull())
    {
        HumanData mother=db.getHuman(humanData.mother.toString());
        ui->lineEditMother->setText(mother.getHumanInfo());
        ui->lineEditMother->setCursorPosition(0);
        ui->lineEditMother->setToolTip(ui->lineEditMother->text());
    }
    ui->lineEditBirthdate->setText(humanData.birthDate);

    //С местом рождения история особая, как всегда
    //ui->lineEditBirthPlace->setText(humanData.birthPlaceId);
    PlaceData birthPlace=db.getPlace(humanData.birthPlaceId);
    if(!birthPlace.name.isEmpty())
        ui->lineEditBirthPlace->setText(birthPlace.getPlaceInfo());
    else
        ui->lineEditBirthPlace->setText("[!!!]"+humanData.birthPlaceId);

    ui->lineEditBirthPlace->setToolTip(ui->lineEditBirthPlace->text());

    ui->lineEditDeathDate->setText(humanData.deathDate);
    if(!humanData.deathDate.isEmpty() || !humanData.deathCause.isEmpty())ui->checkBoxLife->setChecked(false);
    ui->lineEditDeathCause->setText(humanData.deathCause);
    ui->lineEditOccupation->setText(humanData.occupation);
    switch(humanData.sex)
    {
        case humanSexFemale:
        ui->comboBoxSex->setCurrentIndex(0);
        break;
        case humanSexMale:
        ui->comboBoxSex->setCurrentIndex(1);
        break;
        case humanSexUndefined:
        ui->comboBoxSex->setCurrentIndex(2);
        break;
    }
    ui->textEditNote->setText(humanData.note.join("\n"));
    updateChildrenData();
    updateSpousesData();
    updatePlacesData();
    updateSiblingsData();
    ///Ну и так далее, ага.
}

void HumanEditDialog::updateChildrenData()
{
    //Находим всех детей QList<QStringList> пойдёт, идентификатор и информация...
    //qDebug()<<"Ищем детей, однако";
    childrenData=db.getChildrenInfo(humanData.uuid.toString());
    //qDebug()<<childrenData;
    //теперь этот лист надо экспортировать в listView.
   // QListWidget w;
    ui->listWidgetChildren->clear();
    for(int i=0;i<childrenData.count();i++)
    {
        ui->listWidgetChildren->addItem(childrenData[i][1]);
    }
}

void HumanEditDialog::updateSpousesData()
{
    //qDebug()<<"Updating spouses";
    spousesData=db.getSpousesInfo(humanData.uuid.toString(),humanData.sex);
    //qDebug()<<spousesData;
    ui->listWidgetSpouses->clear();
    for(int i=0;i<spousesData.count();i++)
    {
        ui->listWidgetSpouses->addItem(spousesData[i][1]);
    }
}

void HumanEditDialog::updatePlacesData()
{
    placesData=db.getHumanPlacesInfo(humanData.uuid.toString());
    ui->listWidgetPlaces->clear();
    for(int i=0;i<placesData.count();i++)
    {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(placesData[i][1]);
        item->setData(Qt::UserRole,placesData[i][0]);
        ui->listWidgetPlaces->addItem(item);
    }
}

void HumanEditDialog::updateSiblingsData()
{
    siblingsData=db.getHumanSiblingsInfo(humanData.father.toString(),humanData.mother.toString(),hUid);
    ui->listWidgetSiblings->clear();
    for(int i=0;i<siblingsData.count();i++)
    {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(siblingsData[i][1]);
        item->setData(Qt::UserRole,siblingsData[i][0]);
        ui->listWidgetSiblings->addItem(item);
    }
}


void HumanEditDialog::on_pushButtonFatherDelete_clicked()
{   
    QMessageBox msgBox;
    msgBox.setText("Удаление важных данных");
    msgBox.setInformativeText("Вы действительно хотите удалить данные о родителе?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret)
    {
       case QMessageBox::Ok:
           humanData.father=NULL;
           ui->lineEditFather->setText("");
           ui->lineEditFather->setToolTip("");
           updateSiblingsData();
           break;
       case QMessageBox::Cancel:
           //Тоже в общем без последствий.
           break;
       default:
           // Ну так, пусть будет.
           break;
     }
}

void HumanEditDialog::on_pushButtonMotherDelete_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Удаление важных данных");
    msgBox.setInformativeText("Вы действительно хотите удалить данные о родителе?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret)
    {
       case QMessageBox::Ok:
           humanData.mother=NULL;
           ui->lineEditMother->setText("");
           ui->lineEditMother->setToolTip("");
           updateSiblingsData();
           break;
       case QMessageBox::Cancel:
           //Тоже в общем без последствий.
           break;
       default:
           // Ну так, пусть будет.
           break;
     }
}

void HumanEditDialog::on_pushButtonCancel_clicked()
{    
    reject();
}

void HumanEditDialog::on_pushButtonOk_clicked()
{
    //Нам в общем-то надо что? Просто сохранить данные...В общем-то а как...
    QStringList errors=collectHumanData();
    if(errors.count()>0)
    {
        QMessageBox::warning(this,"Внимание","Ошибки записей:"+errors.join(", ")+".");
        return;
    }
    if(hUid.isEmpty())
    {
        //qDebug()<<"AddHuman: "<<humanData.uuid.toString();
        if(humanData.uuid.isNull()) humanData.uuid=QUuid::createUuid();
        db.addHuman(humanData);
        hUid=humanData.uuid.toString();
        //qDebug()<<"Uuid in the end of redo and close"<<hUid;
        //qDebug()<<"AddHuman: "<<hUid;
        /*
        bool res=db.addHuman(humanData);
        if(res)
            hUid=humanData.uuid.toString();
            */
    }
    else
    {
        db.editHuman(humanData);//должно уже работать, кстати.
    }
    accept();
}

void HumanEditDialog::on_pushButtonAddSpouse_clicked()
{
    //Как ни странно, надо открыть такой же диалог, но передать в него данные о родителях
    if(hUid.isEmpty())
    {
        QMessageBox::warning(this,"Внимание","Добавление супругов возможно только после сохранения данных человека!");
        return;
    }
    MarriageEditDialog dlg(db);
    if(humanData.sex==humanSexFemale)
    {
        qDebug()<<"Female";
        dlg.addWifeInfo(humanData.uuid.toString());
    }
    if(humanData.sex==humanSexMale)
        dlg.addHusbandInfo(humanData.uuid.toString());
    dlg.exec();
    updateSpousesData();
}

void HumanEditDialog::on_pushButtonChild_clicked()
{
    if(hUid.isEmpty())
    {
        QMessageBox::warning(this,"Внимание","Добавление детей возможно только после сохранения данных человека!");
        return;
    }
    HumanEditDialog dlg(db);
    if(humanData.sex==humanSexFemale)
        dlg.addMotherInfo(humanData.uuid.toString());
    if(humanData.sex==humanSexMale)
    {
        dlg.addFatherInfo(humanData.uuid.toString());
        dlg.setPatrynomic(ui->lineEditName->text());
        dlg.setSurname(ui->lineEditSurName->text());
    }

    dlg.exec();
    //Обновить список детей, однако.
    updateChildrenData();
}

void HumanEditDialog::on_pushButtonChildSearch_clicked()
{
    if(hUid.isEmpty())
    {
        QMessageBox::warning(this,"Внимание","Добавление детей возможно только после сохранения данных человека!");
    }
    HumanSearchDialog dlg(db,humanSexUndefined,humanData.surName);
    dlg.setWindowTitle("Поиск ребёнка (можно добавить ему в родители кого-нибудь)");
    if(dlg.exec()==QDialog::Rejected) return;
    if(hUid.isEmpty())
    {
        return;
    }
    db.addParent(dlg.getHumanUuid(),hUid,humanData.sex);
    updateChildrenData();
    //только найти и там добавить ему родителя..без сложностей.
}

void HumanEditDialog::listChildrenActivate(QModelIndex index)
{
    //получить индекс
    HumanEditDialog dlg(db,childrenData[index.row()][0]);
    dlg.exec();
    updateChildrenData();
    //И снова ничего не делаем. Так-то.
}

void HumanEditDialog::listSpousesActivate(QModelIndex index)
{
    HumanEditDialog dlg(db,spousesData[index.row()][0]);
    dlg.exec();
    updateSpousesData();
    //Опять ничего?
}

void HumanEditDialog::listPlacesActivate(QModelIndex index)
{
    HumanPlaceDialog dlg(db,hUid,ui->listWidgetPlaces->item(index.row())->data(Qt::UserRole).toString());
    dlg.exec();
    updatePlacesData();
}

void HumanEditDialog::listSiblingsActivate(QModelIndex index)
{
    HumanEditDialog dlg(db,ui->listWidgetSiblings->item(index.row())->data(Qt::UserRole).toString());
    dlg.exec();
    updateSiblingsData();
}

void HumanEditDialog::on_checkBoxLife_toggled(bool checked)
{
    if(checked)
    {
        ui->labelDeathDate->setEnabled(false);
        ui->lineEditDeathDate->setEnabled(false);
        ui->labelDeathCause->setEnabled(false);
        ui->lineEditDeathCause->setEnabled(false);
    }
    else
    {
        ui->labelDeathDate->setEnabled(true);
        ui->lineEditDeathDate->setEnabled(true);
        ui->labelDeathCause->setEnabled(true);
        ui->lineEditDeathCause->setEnabled(true);
    }
}

void HumanEditDialog::on_pushButtonSave_clicked()
{
    //Тут всё, как в там, только не закрываем диалог и обновляем содержимое.
    QStringList errors=collectHumanData();
    if(errors.count()>0)
    {
        QMessageBox::warning(this,"Внимание","Ошибки записей:"+errors.join(", ")+".");
        return;
    }
    if(hUid.isEmpty())
    {
        if(humanData.uuid.isNull()) humanData.uuid=QUuid::createUuid();
        db.addHuman(humanData);
        hUid=humanData.uuid.toString();
        qDebug()<<"Uuid in the end of redo and stay"<<hUid;
        setWindowTitle("Редактирование человека "+humanData.getHumanInfo());
        ui->pushButtonOk->setText("Сохранить и закрыть");
        ui->pushButtonSave->setText("Сохранить");
    }
    else
    {
        db.editHuman(humanData);//должно уже работать, кстати.
    }
    fillHumanData();//Пусть побудет тут для теста (хотя скорее всего останётся надолго...Но для проверки - ок.
}

void HumanEditDialog::on_pushButtonDelete_clicked()
{
    if(hUid.isEmpty())
    {
        QMessageBox::warning(this,"Внимание","Человек ещё не добавлен в базу, так что удалить его нельзя.");
        return;
    }

    QMessageBox msgBox;
    msgBox.setText("Удаление важных данных");
    msgBox.setInformativeText("Вы действительно хотите удалить данные об этом человеке?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret)
    {
       case QMessageBox::Ok:
           db.deleteHuman(hUid);
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

void HumanEditDialog::on_pushButtonSearchPlace_clicked()
{
    PlaceSearchDialog dlg(db);
    if(dlg.exec()==QDialog::Rejected) return;
    ui->lineEditBirthPlace->setText(dlg.getPlaceInfo());
    ui->lineEditBirthPlace->setToolTip(ui->lineEditBirthPlace->text());
    ui->lineEditBirthPlace->setCursorPosition(0);
    humanData.birthPlaceId=dlg.getPlaceUuid();
}

void HumanEditDialog::on_pushButtonDeleteMarriage_clicked()
{
    QModelIndex index;
    index=ui->listWidgetSpouses->currentIndex();
    if(!index.isValid()) return;

    QMessageBox msgBox;
    msgBox.setText("Удаление важных данных");
    msgBox.setInformativeText("Вы действительно хотите удалить данные об браке с этим человеке?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret)
    {
       case QMessageBox::Ok:
           //index=ui->listWidgetSpouses->currentIndex();
           db.deleteMarriageBySpouses(hUid,spousesData[ui->listWidgetSpouses->currentIndex().row()][0]);
           updateSpousesData();
           break;
       case QMessageBox::Cancel:
           //Тоже в общем без последствий.
           break;
       default:
           // Ну так, пусть будет.
           break;
     }


}


void HumanEditDialog::on_pushButton_clicked()
{
    QModelIndex index;
    index=ui->listWidgetSpouses->currentIndex();
    if(!index.isValid()) return;
    FamilyData family = db.getMarriageBySpouses(hUid,spousesData[index.row()][0]);
    MarriageEditDialog dlg(db,family.uuid.toString());
    dlg.exec();
    updateSpousesData();
}

void HumanEditDialog::on_pushButtonSearchRecords_clicked()
{
    if(hUid.isEmpty())
    {
        QMessageBox::warning(this,"Внимание","Человек только создаётся и пока нигде не может быть упомянут!");
        return;
    }
    RecordSearchDialog dlg(db,hUid,"","");
    dlg.exec();
}

void HumanEditDialog::on_pushButtonEditMother_clicked()
{
    if(humanData.mother.isNull()) return;
    HumanEditDialog dlg(db,humanData.mother.toString());
    if(dlg.exec()==QDialog::Rejected) return;
    if(dlg.getHumanUuid().isEmpty()) return;
    QString motherUuid=dlg.getHumanUuid();
    humanData.father=HumansDatabase::getUuidFromString(motherUuid);
    ui->lineEditMother->setText(dlg.getHumanInfo());
    ui->lineEditMother->setCursorPosition(0);
    ui->lineEditMother->setToolTip(dlg.getHumanInfo());
}

void HumanEditDialog::on_pushButtonEditFather_clicked()
{
    if(humanData.father.isNull())return;
    HumanEditDialog dlg(db,humanData.father.toString());
    if(dlg.exec()==QDialog::Rejected) return;
    if(dlg.getHumanUuid().isEmpty()) return;
    QString fatherUuid=dlg.getHumanUuid();
    humanData.father=HumansDatabase::getUuidFromString(fatherUuid);
    ui->lineEditFather->setText(dlg.getHumanInfo());
    ui->lineEditFather->setCursorPosition(0);
    ui->lineEditFather->setToolTip(ui->lineEditFather->text());
}



void HumanEditDialog::on_pushButtonUnionHumans_clicked()
{

    if(hUid.isEmpty())
    {
        QMessageBox::warning(this,"Внимание","Человек только создаётся и пока его нельзя ни с кем объединить!");
        return;
    }

    HumanSearchDialog dlg(db,humanData.sex,humanData.surName+" "+humanData.givenName+" "+humanData.marName);
    //dlg.exec();

    if(dlg.exec()==QDialog::Rejected) return;
    if(dlg.getHumanUuid().isEmpty()) return;


    QMessageBox msgBox;
    msgBox.setText("Внимание! Объединение!");
    msgBox.setInformativeText("Вы действительно хотите объединить данные \""+humanData.getHumanInfo()+"\" c найденным \""+dlg.getHumanInfo()+"\"? Приоритет у данных найденного человека.");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret)
    {
       case QMessageBox::Ok:
           db.uniteHumans(hUid,dlg.getHumanUuid());
           reject();
           break;
       case QMessageBox::Cancel:
           //Тоже в общем без последствий.
           break;
       default:
           // Ну так, пусть будет.
           break;
     }
}

void HumanEditDialog::on_pushButtonPlaceAdd_clicked()
{
    if(hUid.isEmpty())
    {
        QMessageBox::warning(this,"Внимание","Человек только создаётся и пока с ним нельзя связать какое-либо место.!");
        return;
    }

    HumanPlaceDialog dlg(db,hUid);    
    if(dlg.exec()==QDialog::Rejected) return;
    updatePlacesData();
}

void HumanEditDialog::on_pushButtonPlaceDelete_clicked()
{
    QModelIndex index;
    index=ui->listWidgetPlaces->currentIndex();
    if(!index.isValid()) return;

    QMessageBox msgBox;
    msgBox.setText("Удаление важных данных");
    msgBox.setInformativeText("Вы действительно хотите удалить данные о связи человека с этим местом?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret)
    {
       case QMessageBox::Ok:
           db.deleteHumanPlace(ui->listWidgetPlaces->item(index.row())->data(Qt::UserRole).toString());
           updatePlacesData();
           break;
       case QMessageBox::Cancel:
           //Тоже в общем без последствий.
           break;
       default:
           // Ну так, пусть будет.
           break;
     }
}

void HumanEditDialog::on_pushButtonPlaceEdit_clicked()
{
    QModelIndex index;
    index=ui->listWidgetPlaces->currentIndex();
    if(!index.isValid()) return;
    listPlacesActivate(index);
}

void HumanEditDialog::on_pushButtonAddSibling_clicked()
{
    if(hUid.isEmpty())
    {
        QMessageBox::warning(this,"Внимание","Человек только создаётся и пока нельзя добавить ему родственника! Сохранитесь!");
        return;
    }

    HumanEditDialog dlg(db);
    dlg.addFatherInfo(humanData.father.toString());
    dlg.addMotherInfo(humanData.mother.toString());
    dlg.setPatrynomic(ui->lineEditPatronymic->text());
    dlg.setSurname(ui->lineEditSurName->text());
    dlg.exec();
    updateSiblingsData();
}

void HumanEditDialog::on_pushButtonPlaceAddLast_clicked()
{
    if(hUid.isEmpty())
    {
        QMessageBox::warning(this,"Внимание","Человек только создаётся и пока нельзя связать его с местом! Сохранитесь!");
        return;
    }
    HumanPlaceData hPlace=geneOptions.lastHumanPlace;
    if(hPlace.placeId.isEmpty())return;
    hPlace.uuid=QUuid::createUuid().toString();
    hPlace.humanId=hUid;
    db.addHumanPlace(hPlace);
    updatePlacesData();
}
