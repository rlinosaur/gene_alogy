#include <QMessageBox>
#include <QFileDialog>

#include <QDesktopServices>

#include "options.h"
#include "sourceeditdialog.h"
#include "ui_sourceeditdialog.h"

SourceEditDialog::SourceEditDialog(HumansDatabase *humansDatabase, QString sourceUuid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SourceEditDialog)
{
    if(!humansDatabase->isOpen())
    {
        QMessageBox::warning(this,"Внимание","База не открыта!");
       reject();
        return;
    }
    ui->setupUi(this);
    ui->comboBoxTypes->addItems(sourceData.getTypes());//Before fillSourceData!

    db=humansDatabase;
    srcUid=sourceUuid;
    sourceData=db->getSource(srcUid);//хотя в пустоте даст пустоту.
    fillSourceData();

    if(!srcUid.isEmpty())
    {
        setWindowTitle("Редактирование источника \""+sourceData.name+"\"");
        ui->pushButtonOk->setText("Сохранить и закрыть");
    }
    else
    {
        setWindowTitle("Добавление источника");
        ui->pushButtonOk->setText("Добавить и закрыть");
    }



}

SourceEditDialog::~SourceEditDialog()
{
    delete ui;
}

void SourceEditDialog::fillSourceData()
{
    ui->lineEditName->setText(sourceData.name);        
    ui->comboBoxTypes->setCurrentIndex(sourceData.type);
    ui->lineEditLink->setText(sourceData.link);
    ui->textEditNote->setText(sourceData.note);
}

QStringList SourceEditDialog::collectSourceData()
{
    QStringList errors;
    sourceData.name=ui->lineEditName->text();
    sourceData.type=ui->comboBoxTypes->currentIndex();
    sourceData.link=ui->lineEditLink->text();
    sourceData.note=ui->textEditNote->toPlainText();
    if(sourceData.name.isEmpty()) errors.append("отсутствует название источника");

    return errors;
}

void SourceEditDialog::on_pushButtonDeleteSource_clicked()
{

    if(srcUid.isEmpty())
    {
        QMessageBox::warning(this,"Внимание","Источник не создан, удалять нечего!");
        return;
    }
    QMessageBox msgBox;
    msgBox.setText("Удаление важных данных");
    msgBox.setInformativeText("Вы действительно хотите удалить данные об источнике?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret)
    {
       case QMessageBox::Ok:
           db->deleteSource(srcUid);
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

void SourceEditDialog::on_pushButtonCancel_clicked()
{
    reject();
}

void SourceEditDialog::on_pushButtonOk_clicked()
{
    //в зависимости от srcUuid, конечно.е да-да-да.

    QStringList errors=collectSourceData();
    if(errors.count()>0)
    {
        QMessageBox::warning(this,"Внимание","Ошибки источника:"+errors.join(", ")+".");
        return;
    }
    if(srcUid.isEmpty())db->addSource(sourceData);
    else
    {
        db->editSource(sourceData);//должно уже работать, кстати.
    }
   accept();
}

void SourceEditDialog::on_pushButtonGetLink_clicked()
{

    QString dbFile = QFileDialog::getOpenFileName(this,"Выбор файла источника",optionsData.sourceDirectory,"All files (*.*)",0,QFileDialog::DontUseNativeDialog);
    if (dbFile.isEmpty()) return;
    QStringList parts;
    if(!optionsData.sourceDirectory.isEmpty())
        parts=dbFile.split(optionsData.sourceDirectory);
    if(parts.size()<2)
    {
        QMessageBox::warning(this,"Внимание!","Файл находится вне папки источников!");
    }
    else
        dbFile=parts.at(1);
    ui->lineEditLink->setText(dbFile);
    //путь должен быть  относительный
}

void SourceEditDialog::on_pushButtonShow_clicked()
{
    //@QDesktopServices::openUrl(QUrl("file:///X:/test/docs/myFile.html"), "iexplorer.exe" );@

    if(ui->lineEditLink->text().contains(":/"))
        QDesktopServices::openUrl(QUrl("file:///"+ui->lineEditLink->text()));
    else
        QDesktopServices::openUrl(QUrl("file:///"+optionsData.sourceDirectory+ui->lineEditLink->text()));

}
