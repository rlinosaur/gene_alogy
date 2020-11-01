#include <QMessageBox>


#include "sourcesearchdialog.h"
#include "recordsourceeditdialog.h"
#include "ui_recordsourceeditdialog.h"

RecordSourceEditDialog::RecordSourceEditDialog(HumansDatabase *humansDatabase, QString recordUuid, QString recordSourceUuid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RecordSourceEditDialog)
{
    ui->setupUi(this);

    if(!humansDatabase->isOpen())
    {
        QMessageBox::warning(this,"Внимание","База не открыта!");
        reject();
        return;
    }

    db=humansDatabase;    
    srcRecUid=recordSourceUuid;

    if(!srcRecUid.isEmpty())
    {
        //доступ к БД только тут, там это незачем.
        recordSourceData=db->getRecordSource(srcRecUid);
        fillRecordSourceData();
        setWindowTitle("Редактирование источника для записи");
        ui->pushButtonOk->setText("Сохранить и закрыть");
    }
    else
    {
        setWindowTitle("Добавление источника для записи");
        ui->pushButtonOk->setText("Добавить и закрыть");
        recordSourceData.recordId=recordUuid;
    }
    setLayout(ui->verticalLayoutMain);
}

RecordSourceEditDialog::~RecordSourceEditDialog()
{
    delete ui;
}

void RecordSourceEditDialog::on_pushButtonCancel_clicked()
{
    reject();
}

void RecordSourceEditDialog::on_pushButtonOk_clicked()
{
    //
    //Цель - сохранить инфу об источнике, как-то так. В базу данных там, whatever.
    //а как её сохраниь?

    QStringList errors=collectRecordSourceData();
    if(errors.count()>0)
    {
        QMessageBox::warning(this,"Внимание","Ошибки какие-то:"+errors.join(", ")+".");
        return;
    }

    if(srcRecUid.isEmpty())
    {
        bool res=db->addRecordSource(recordSourceData);
        if(res)
            srcRecUid=recordSourceData.uuid;//не понадобится, ну да ладно. может и понадобится.
    }
    else
    {
        db->editRecordSource(recordSourceData);
    }
    accept();
}

QStringList RecordSourceEditDialog::collectRecordSourceData()
{
    QStringList error;

    if(ui->lineEditSourceInfo->text().isEmpty())
    {
        error.append("не выбран источник");
    }
    recordSourceData.sourceId=sourceData.uuid;
    recordSourceData.location=ui->lineEditLocation->text();
    recordSourceData.note=ui->textEditNote->toPlainText();
    return error;
}

void RecordSourceEditDialog::fillRecordSourceData()
{
    sourceData=db->getSource(recordSourceData.sourceId);
    //Исходя из результата отображаем его в неизменном виде, ага.
    fillSourceInfo(sourceData);


    ui->lineEditLocation->setText(recordSourceData.location);
    ui->textEditNote->setText(recordSourceData.note);
}

void RecordSourceEditDialog::fillSourceInfo(SourceData srcDat)
{
    ui->lineEditSourceInfo->setText(srcDat.name+", "+srcDat.getSourceName(sourceData.type));
}

void RecordSourceEditDialog::on_pushButtonSearchSource_clicked()
{
    //Выбор источника...значит к поиску источников.
    SourceSearchDialog dlg(db);
    if(dlg.exec()==QDialog::Rejected) return;
    //qDebug()<<"something "<<dlg.setSourceId();
    sourceData=db->getSource(dlg.getSourceId());
    fillSourceInfo(sourceData);
}
