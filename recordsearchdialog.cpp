#include <QDebug>
#include <QMessageBox>

#include "recordsearchdialog.h"
#include "ui_recordsearchdialog.h"

#include "recordeditdialog.h"
#include "humansearchdialog.h"
#include "placesearchdialog.h"

RecordSearchDialog::RecordSearchDialog(HumansDatabase &humansDatabase, QString humanId, QString placeId, QString searchString,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RecordSearchDialog),
    db(humansDatabase)
{
    if(!humansDatabase.isOpen())
    {
        QMessageBox::warning(this,"Внимание","База по-прежнему не открыта, а значит искать людей, увы, негде!");
        reject();
        return;
    }
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() | Qt::WindowMaximizeButtonHint);
    setWindowTitle("Поиск записей");   
    huId=humanId;

    if(!huId.isEmpty())
    {
        HumanData huData=db.getHuman(huId);
        ui->lineEditHuman->setText(huData.getHumanInfo());
    }
    plId=placeId;
    if(!plId.isEmpty())
    {
        PlaceData plData=db.getPlace(plId);
        ui->lineEditPlace->setText(plData.getPlaceInfo());
    }

    ui->lineEditSearch->setText(searchString);
    modelSort.setSourceModel(&model);
    ui->tableViewSearch->setModel(&modelSort);
    ui->tableViewSearch->setSortingEnabled(true);
    //ui->tableViewSearch->setModel(&model);
    ui->tableViewSearch->setSelectionBehavior(QAbstractItemView::SelectRows);
    //connect(ui->tableViewSearch,SIGNAL(clicked(QModelIndex)),this,SLOT(tableSearchClickSlot(QModelIndex)));
    connect(ui->tableViewSearch,SIGNAL(activated(QModelIndex)),this,SLOT(tableSearchActivateSlot(QModelIndex)));//Для двойного клика.
    on_pushButtonSearch_clicked();
}

RecordSearchDialog::~RecordSearchDialog()
{
    delete ui;
}

void RecordSearchDialog::on_pushButtonOk_clicked()
{
    accept();
}

void RecordSearchDialog::tableSearchActivateSlot(QModelIndex index)
{
    RecordEditDialog dlg(db,modelSort.data(modelSort.index(index.row(),0)).toString());
    dlg.exec();
    on_pushButtonSearch_clicked();
}

void RecordSearchDialog::on_pushButtonSearch_clicked()
{
    model.setQuery(HumansDatabase::getRecordSearchQuery(ui->lineEditSearch->text(),huId,plId),db.getDb());
    ui->tableViewSearch->hideColumn(0);
    ui->tableViewSearch->resizeColumnsToContents();
    ui->labelCount->setText("Найдено: "+db.getRecordSearchQueryCount(ui->lineEditSearch->text(),huId,plId,COMMONSEARCHFLAG_COUNTQUERY).toString());
}

void RecordSearchDialog::on_pushButtonSearchHuman_clicked()
{
    HumanSearchDialog dlg(db,humanSexUndefined,"");
    if(dlg.exec()==QDialog::Rejected) return;
    ui->lineEditHuman->setText(dlg.getHumanInfo());
    huId=dlg.getHumanUuid();
}

void RecordSearchDialog::on_pushButtonDeletHuman_clicked()
{
    huId.clear();
    ui->lineEditHuman->clear();
    on_pushButtonSearch_clicked();
}

void RecordSearchDialog::on_pushButtonSearchPlace_clicked()
{
    PlaceSearchDialog dlg(db);
    if(dlg.exec()==QDialog::Rejected) return;
    ui->lineEditPlace->setText(dlg.getPlaceInfo());
    plId=dlg.getPlaceUuid();
}

void RecordSearchDialog::on_pushButtonDeletePlace_clicked()
{
    plId.clear();
    ui->lineEditPlace->clear();
    on_pushButtonSearch_clicked();
}
