#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QClipboard>
#include <QMessageBox>

#include "ui_humansearchdialog.h"
#include "humansearchdialog.h"
#include "humaneditdialog.h"


HumanSearchDialog::HumanSearchDialog(HumansDatabase &humansDatabase, HumanSex sexData, QString searchString, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HumanSearchDialog),
    db(humansDatabase)
{

    if(!humansDatabase.isOpen())
    {
        QMessageBox::warning(this,"Внимание","База по-прежнему не открыта, а значит искать людей, увы, негде!");
        reject();
    }
    ui->setupUi(this);
    ui->lineEditSearch->setText(searchString);
    sex=sexData;      
    modelSort.setSourceModel(&model);
    ui->tableViewSearch->setModel(&modelSort);
    ui->tableViewSearch->setSortingEnabled(true);
    //ui->tableViewSearch->setModel(&model);
    ui->tableViewSearch->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(ui->tableViewSearch,SIGNAL(clicked(QModelIndex)),this,SLOT(tableSearchClickSlot(QModelIndex)));
    connect(ui->tableViewSearch,SIGNAL(activated(QModelIndex)),this,SLOT(tableSearchActivateSlot(QModelIndex)));//Для двойного клика.
    ui->comboBoxSex->addItem("женщина",humanSexFemale);
    ui->comboBoxSex->addItem("мужчина",humanSexMale);
    ui->comboBoxSex->addItem("неважно",humanSexUndefined);
    if(sexData==humanSexFemale)ui->comboBoxSex->setCurrentIndex(0);
    if(sexData==humanSexMale)ui->comboBoxSex->setCurrentIndex(1);
    if(sexData==humanSexUndefined)ui->comboBoxSex->setCurrentIndex(2);

    on_pushButtonSearch_clicked();
}

HumanSearchDialog::~HumanSearchDialog()
{
    delete ui;
}

void HumanSearchDialog::on_pushButtonSearch_clicked()
{   
    if(!ui->lineEditSearch->text().isEmpty()) QApplication::clipboard()->setText(ui->lineEditSearch->text());
    model.setQuery(HumansDatabase::getHumanSearchQuery(ui->lineEditSearch->text(),ui->comboBoxSex->itemData(ui->comboBoxSex->currentIndex())),db.getDb());
    ui->tableViewSearch->hideColumn(0);
    currentSelectionIndex=QModelIndex();
    ui->labelCount->setText("Найдено: "+db.getHumanSearchQueryCount(ui->lineEditSearch->text(),ui->comboBoxSex->itemData(ui->comboBoxSex->currentIndex())).toString());
}

void HumanSearchDialog::on_lineEditSearch_returnPressed()
{
    on_pushButtonSearch_clicked();
}


void HumanSearchDialog::on_pushButton_2_clicked()
{
    humanUuid.clear();
    reject();
}

void HumanSearchDialog::on_pushButton_clicked()
{
    if(!currentSelectionIndex.isValid())
    {
        QMessageBox::warning(this,"Внимание","Никто не выбран. Нажмите \"Отмена\", чтобы закрыть диалог.");
        return;
    }
    humanUuid=modelSort.data(modelSort.index(currentSelectionIndex.row(),0)).toString();
    accept();
}

void HumanSearchDialog::tableSearchClickSlot(QModelIndex index)
{    
    humanInfo.clear();
    humanInfo.append(modelSort.data(modelSort.index(index.row(),1)).toString());
    QString marname=modelSort.data(modelSort.index(index.row(),2)).toString();
    if(!marname.isEmpty())
        humanInfo.append(" ("+marname+")");
    humanInfo.append(" "+modelSort.data(modelSort.index(index.row(),3)).toString());
    humanInfo.append(" "+modelSort.data(modelSort.index(index.row(),4)).toString());
    QString birthyear=modelSort.data(modelSort.index(index.row(),5)).toString();
    QString deathyear=modelSort.data(modelSort.index(index.row(),6)).toString();
    if(!birthyear.isEmpty() || !deathyear.isEmpty())
    {
        humanInfo.append(" "+birthyear+"-"+deathyear);       
    }
    humanInfo.append(" "+modelSort.data(modelSort.index(index.row(),7)).toString());
    //qDebug()<<humanInfo;
    QString text;
    text.append("Выбранный человек:");
    text.append(humanInfo);
    ui->labelSelected->setText(text);
    currentSelectionIndex=index;
}

void HumanSearchDialog::tableSearchActivateSlot(QModelIndex index)
{
    HumanEditDialog dlg(db,modelSort.data(modelSort.index(index.row(),0)).toString());
    dlg.exec();
    on_pushButtonSearch_clicked();
    //Без последствий, просто редактирование. родителя там добавить, ещё чего.
}


void HumanSearchDialog::on_pushButtonNewHuman_clicked()
{
    HumanEditDialog dlg(db);
    dlg.exec();//дальше сам.
    on_pushButtonSearch_clicked();
}
