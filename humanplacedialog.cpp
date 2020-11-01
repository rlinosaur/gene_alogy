#include <QMessageBox>
#include <QDebug>

#include "humanplacedialog.h"
#include "ui_humanplacedialog.h"

#include "placesearchdialog.h"
#include "geneoptions.h"

HumanPlaceDialog::HumanPlaceDialog(HumansDatabase *humansDatabase, QString humanUuid, QString humanPlaceUuid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HumanPlaceDialog)
{
    ui->setupUi(this);
    setLayout(ui->verticalLayoutMain);

    if(!humansDatabase->isOpen())
    {
        QMessageBox::warning(this,"Внимание","База не открыта!");
        reject();
        return;
    }

    db=humansDatabase;

    if(humanUuid.isEmpty())
    {
        QMessageBox::warning(this,"Внимание","Не переданы данные человека!");
        reject();
        return;
    }
    humanData = db->getHuman(humanUuid);
    if(humanData.uuid.isNull())
    {
        QMessageBox::warning(this,"Внимание","Не удалось получить из базы данных данные человека. Увы!");
        reject();
        return;
    }    
    hpUuid=humanPlaceUuid;
    if(!hpUuid.isEmpty())
    {
        setWindowTitle("Редактирование человекоместа");       
        ui->pushButtonSaveAndClose->setText("Сохранить и закрыть");
        ui->pushButtonPlaceSearch->setDisabled(true);
    }
    else
    {
        setWindowTitle("Добавление человекоместа");        
        ui->pushButtonSaveAndClose->setText("Добавить и закрыть");
    }
    if(!hpUuid.isEmpty())
        humanPlaceData=db->getHumanPlace(hpUuid);
    else
        humanPlaceData.humanId=humanUuid;//А если да, то нет, т.е. из БД.
    fillHumanPlaceData();

}

HumanPlaceDialog::~HumanPlaceDialog()
{
    delete ui;
}

void HumanPlaceDialog::fillHumanPlaceData()
{
    if(!humanPlaceData.placeId.isEmpty())
    {
        placeData=db->getPlace(humanPlaceData.placeId);
        ui->lineEditPlace->setText(placeData.getPlaceInfo());
    }
    ui->lineEditHuman->setText(humanData.getHumanInfo());

    ui->lineEditDate->setText(humanPlaceData.date);
    ui->comboBoxType->setEditText(humanPlaceData.type);
    ui->textEditNote->setText(humanPlaceData.note);
}

QStringList HumanPlaceDialog::collectHumanPlaceData()
{
    QStringList errors;
    humanPlaceData.date=ui->lineEditDate->text();
    humanPlaceData.type=ui->comboBoxType->currentText();
    humanPlaceData.note=ui->textEditNote->toPlainText();
    //А остальное вроде уже само записалось.
    return errors;
}


void HumanPlaceDialog::on_pushButtonPlaceSearch_clicked()
{
    PlaceSearchDialog dlg(db);
    if(dlg.exec()==QDialog::Rejected) return;

    QString placeUuid=dlg.getPlaceUuid();
    if(!placeUuid.isEmpty())
    {
        humanPlaceData.placeId=placeUuid;
        ui->lineEditPlace->setText(dlg.getPlaceInfo());
    }
}

void HumanPlaceDialog::on_pushButtonSaveAndClose_clicked()
{
    QStringList errors=collectHumanPlaceData();
    if(errors.count()>0)
    {
        QMessageBox::warning(this,"Внимание","Ошибки записей:"+errors.join(", ")+".");
        return;
    }
    if(hpUuid.isEmpty())
    {                
        if(humanPlaceData.uuid.isEmpty()) humanPlaceData.uuid=QUuid::createUuid().toString();
        qDebug()<<"our humanid 1:"<<humanPlaceData.humanId;
        db->addHumanPlace(humanPlaceData);
        qDebug()<<"HumanPlace added.";
        hpUuid=humanPlaceData.uuid;
        geneOptions.lastHumanPlace=humanPlaceData;

    }
    else
    {
        db->editHumanPlace(humanPlaceData);
        geneOptions.lastHumanPlace=humanPlaceData;
    }
    accept();
}

void HumanPlaceDialog::on_pushButtonCancel_clicked()
{
    reject();
}
