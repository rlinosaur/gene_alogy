#include <QDebug>
#include <QMessageBox>

#include "marriageeditdialog.h"
#include "ui_marriageeditdialog.h"

#include "humansearchdialog.h"
#include "placesearchdialog.h"

MarriageEditDialog::MarriageEditDialog(HumansDatabase *humansDatabase, QString familyUid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MarriageEditDialog)
{
    if(!humansDatabase->isOpen())
    {
        QMessageBox::warning(this,"Внимание","База не открыта!");
        reject();
        return;
    }
    ui->setupUi(this);
    db=humansDatabase;
    fUid=familyUid;
    familyData=db->getMarriage(fUid);
    qDebug()<<"Debug about family:"<<fUid;
    qDebug()<<familyUid;
    //qDebug()<<familyData;
    qDebug()<<familyData.husband;
    qDebug()<<"Wife "<<familyData.wife;
    if(!fUid.isEmpty())
    {
        setWindowTitle("Редактирование семьи, можно сказать и так");
        ui->pushButtonOk->setText("Сохранить");
    }
    else
    {
        setWindowTitle("Добавление новой семьи");
        ui->pushButtonOk->setText("Добавить");
    }

    fillMarriageData();
}

MarriageEditDialog::~MarriageEditDialog()
{
    delete ui;
}

void MarriageEditDialog::addHusbandInfo(QString husbandId)
{
    husbandData=db->getHuman(husbandId);
    familyData.husband=husbandData.uuid;
    ui->lineEditHusband->setText(husbandData.getHumanInfo());
    ui->lineEditHusband->setCursorPosition(0);
}

void MarriageEditDialog::addWifeInfo(QString wifeId)
{
    wifeData=db->getHuman(wifeId);
    familyData.wife=wifeData.uuid;
    ui->lineEditWife->setText(wifeData.getHumanInfo());
    ui->lineEditWife->setCursorPosition(0);
}

void MarriageEditDialog::on_pushButtonSearchHusband_clicked()
{
    HumanSearchDialog dlg(db,humanSexMale);
    if(dlg.exec()==QDialog::Rejected) return;
    if(dlg.getHumanInfo().isEmpty()) return;
    familyData.husband=HumansDatabase::getUuidFromString(dlg.getHumanUuid());
    ui->lineEditHusband->setText(dlg.getHumanInfo());
    ui->lineEditHusband->setCursorPosition(0);
}

void MarriageEditDialog::on_pushButtonSearchWife_clicked()
{
    HumanSearchDialog dlg(db,humanSexFemale);
    if(dlg.exec()==QDialog::Rejected) return;
    if(dlg.getHumanInfo().isEmpty()) return;
    familyData.wife=HumansDatabase::getUuidFromString(dlg.getHumanUuid());
    ui->lineEditWife->setText(dlg.getHumanInfo());
    ui->lineEditWife->setCursorPosition(0);
}

void MarriageEditDialog::on_pushButtonSearchPlace_clicked()
{
    PlaceSearchDialog dlg(db);
    if(dlg.exec()==QDialog::Rejected) return;
    if(dlg.getPlaceInfo().isEmpty()) return;
    familyData.marriagePlace=dlg.getPlaceUuid();
    ui->lineEditPlace->setText(dlg.getPlaceInfo());
    ui->lineEditPlace->setCursorPosition(0);
}

void MarriageEditDialog::on_pushButtonCancel_clicked()
{
    reject();
}

void MarriageEditDialog::on_pushButtonDeleteMarriage_clicked()
{

    if(fUid.isEmpty())
    {
        QMessageBox::warning(this,"Внимание","Брак ещё не добавлен в базу, так что удалить его нельзя.");
        return;
    }

    QMessageBox msgBox;
    msgBox.setText("Удаление важных данных");
    msgBox.setInformativeText("Вы действительно хотите удалить данные об этом браке?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret)
    {
       case QMessageBox::Ok:
           db->deleteMarriage(fUid);
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

void MarriageEditDialog::on_pushButtonOk_clicked()
{
    QStringList errors=collectMarriageData();
    if(errors.count()>0)
    {
        QMessageBox::warning(this,"Внимание","Ошибки записей:"+errors.join(", ")+".");
        return;
    }
    if(fUid.isEmpty())
    {
        qDebug()<<"add new marriage"<<familyData.uuid.toString();
        qDebug()<<"add new marriage"<<familyData.husband.toString();
        qDebug()<<"add new marriage"<<familyData.wife.toString();
        bool res=db->addMarriage(familyData);
        if(res)
            fUid=familyData.uuid.toString();
    }
    else
    {
        db->editMarriage(familyData);//должно уже работать, кстати.
    }
    accept();
}

void MarriageEditDialog::fillMarriageData()
{
    ui->lineEditDate->setText(familyData.marriageDate);
    ui->textEdit->setText(familyData.note.join(";"));
    ui->lineEditPlace->setText(db->getPlace(familyData.marriagePlace).getPlaceInfo());
    ui->lineEditWife->setText(db->getHuman(familyData.wife.toString()).getHumanInfo());
    ui->lineEditHusband->setText(db->getHuman(familyData.husband.toString()).getHumanInfo());
}

QStringList MarriageEditDialog::collectMarriageData()
{
    QStringList error;

    if(ui->lineEditHusband->text().isEmpty())
    {
        error.append("муж не указан");
    }
    if(ui->lineEditWife->text().isEmpty())
    {
        error.append("жена не указана");
    }

    familyData.marriageDate=ui->lineEditDate->text();
    familyData.note=ui->textEdit->toPlainText().split(" ");
    return error;
}


