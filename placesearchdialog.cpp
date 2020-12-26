#include <QDebug>
#include <QClipboard>
#include <QMessageBox>

#include "placesearchdialog.h"
#include "ui_placesearchdialog.h"


#include "placeeditdialog.h"

PlaceSearchDialog::PlaceSearchDialog(HumansDatabase &humansDatabase, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlaceSearchDialog),
    db(humansDatabase)
{
    if(!humansDatabase.isOpen())
    {
        QMessageBox::warning(this,"Внимание","База по-прежнему не открыта, а значит искать людей, увы, негде!");
        reject();
        return;
    }
    ui->setupUi(this);

    //db=humansDatabase;
    modelSort.setSourceModel(&model);
    ui->tableViewSearch->setModel(&modelSort);
    ui->tableViewSearch->setSortingEnabled(true);
    //ui->tableViewSearch->setModel(&model);
    ui->tableViewSearch->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(ui->tableViewSearch,SIGNAL(clicked(QModelIndex)),this,SLOT(tableSearchClickSlot(QModelIndex)));
    connect(ui->tableViewSearch,SIGNAL(activated(QModelIndex)),this,SLOT(tableSearchActivateSlot(QModelIndex)));//Для двойного клика.
    on_pushButtonSearch_clicked();
}

PlaceSearchDialog::~PlaceSearchDialog()
{
    delete ui;
}

void PlaceSearchDialog::on_pushButtonSearch_clicked()
{
    //if(!ui->lineEditSearch->text().isEmpty()) QApplication::clipboard()->setText(ui->lineEditSearch->text());
    model.setQuery(HumansDatabase::getPlaceSearchQuery(ui->lineEditSearch->text()),db.getDb());
    ui->tableViewSearch->hideColumn(0);
    currentSelectionIndex=QModelIndex();    
    ui->labelCount->setText("Найдено:"+db.getPlaceSearchQueryCount(ui->lineEditSearch->text(),COMMONSEARCHFLAG_COUNTQUERY).toString());
}

void PlaceSearchDialog::tableSearchClickSlot(QModelIndex index)
{
    placeInfo.clear();
    placeInfo.append(modelSort.data(modelSort.index(index.row(),1)).toString());//Type
    placeInfo.append(" "+modelSort.data(modelSort.index(index.row(),2)).toString());//Name

    QString anothernames=modelSort.data(modelSort.index(index.row(),3)).toString();
    if(!anothernames.isEmpty())
        placeInfo.append(" ("+anothernames+")");
    QString note=modelSort.data(modelSort.index(index.row(),4)).toString();
    if(!note.isEmpty())placeInfo.append(note);

    //qDebug()<<"Place Info"<<placeInfo;
    QString text;
    text.append("Выбранное место:");
    text.append(placeInfo);
    ui->labelSelected->setText(text);
    currentSelectionIndex=index;
}

void PlaceSearchDialog::tableSearchActivateSlot(QModelIndex index)
{
    PlaceEditDialog dlg(db,modelSort.data(modelSort.index(index.row(),0)).toString());
    dlg.exec();
    on_pushButtonSearch_clicked();
}

void PlaceSearchDialog::on_lineEditSearch_returnPressed()
{
    on_pushButtonSearch_clicked();
}

void PlaceSearchDialog::on_pushButtonCancel_clicked()
{
    placeUuid.clear();
    reject();
}

void PlaceSearchDialog::on_pushButtonOk_clicked()
{
    if(!currentSelectionIndex.isValid())
    {
        QMessageBox::warning(this,"Внимание","Никто не выбран. Нажмите \"Отмена\", чтобы закрыть диалог.");
        return;
    }
    placeUuid=modelSort.data(modelSort.index(currentSelectionIndex.row(),0)).toString();
    accept();
}

void PlaceSearchDialog::on_pushButtonNewPlace_clicked()
{
    PlaceEditDialog dlg(db);
    dlg.exec();
    on_pushButtonSearch_clicked();
}
