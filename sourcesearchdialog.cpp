#include <QMessageBox>

#include "sourcesearchdialog.h"
#include "ui_sourcesearchdialog.h"

#include "sourceeditdialog.h"

SourceSearchDialog::SourceSearchDialog(HumansDatabase *humansDatabase, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SourceSearchDialog)
{
    if(!humansDatabase->isOpen())
    {
        QMessageBox::warning(this,"Внимание","База по-прежнему не открыта, а значит искать источники, увы, негде!");
        reject();
        return;
    }
    ui->setupUi(this);    
    setWindowTitle("Поиск источника");

    ui->comboBoxTypes->addItems(SourceData::getTypes());
    db=humansDatabase;
    connect(ui->listWidgetSources,SIGNAL(activated(QModelIndex)),this,SLOT(editSource(QModelIndex)));
    updateSources();

}

SourceSearchDialog::~SourceSearchDialog()
{
    delete ui;
}

QString SourceSearchDialog::getSourceId()
{
    return sourceIdSelected;
}

QString SourceSearchDialog::getSourceInfo()
{
    return sourceInfoSelected;
}


void SourceSearchDialog::on_pushButtonCancel_clicked()
{
    reject();
}

void SourceSearchDialog::on_pushButtonNewSource_clicked()
{
    SourceEditDialog dlg(db);
    dlg.exec();
    updateSources();
}

void SourceSearchDialog::updateSources()
{    
    sources=db->getSources(ui->lineEditSearch->text(),ui->checkBox->isChecked(),ui->comboBoxTypes->currentIndex());
    ui->listWidgetSources->clear();
    for(int i=0;i<sources.count();i++)
    {
        ui->listWidgetSources->addItem(sources[i][1]+","+sources[i][4]);
    }
}

void SourceSearchDialog::on_pushButtonOk_clicked()
{
    QModelIndex ind;
    ind=ui->listWidgetSources->currentIndex();
    if(!ind.isValid())
    {
        QMessageBox::warning(this,"Внимание","Источник не выбран!");
        return;
    }
    sourceIdSelected=sources[ind.row()][0];
    sourceInfoSelected=sources[ind.row()][1];
    //qDebug()<<currentRow
    accept();
}

void SourceSearchDialog::editSource(QModelIndex ind)
{
    SourceEditDialog dlg(db,sources[ind.row()][0]);
    dlg.exec();
    updateSources();
}

void SourceSearchDialog::on_pushButtonDelete_clicked()
{
    QModelIndex ind;
    ind=ui->listWidgetSources->currentIndex();
    if(!ind.isValid())
    {
        QMessageBox::warning(this,"Внимание","Источник не выбран!");
        return;
    }
    if(QMessageBox::warning(this,"Подтверждение удаления","Вы и правда хотите удалить источник?",QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Ok)
    {
        if(QMessageBox::warning(this,"Подтверждение удаления","Точно-точно хотите удалить источник?",QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Cancel)
            return;
    }
    sourceIdSelected=sources[ind.row()][0];
    db->deleteSource(sourceIdSelected);
    updateSources();
}

void SourceSearchDialog::on_pushButtonSearch_clicked()
{
    updateSources();
}


void SourceSearchDialog::on_comboBoxTypes_activated(int index)
{
    ui->comboBoxTypes->setCurrentIndex(index);//Just to be used.
    ui->checkBox->setChecked(true);
}
