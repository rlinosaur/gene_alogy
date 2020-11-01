#include "gedcomimportdialog.h"
#include "ui_gedcomimportdialog.h"


#include <QFile>
#include <QDebug>
#include <QString>
#include <QTextCodec>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>


#include "gedcomparser.h"

GedcomImportDialog::GedcomImportDialog(HumansDatabase *humansDataBase, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GedcomImportDialog)
{
    ui->setupUi(this);

    if(!humansDataBase->isOpen())
    {
        QMessageBox::warning(this,"Внимание","База по-прежнему не открыта, а значит импортировать-то некуда!");
       //db=NULL;//как бы проверить можно а может и нельзя..давай-как это...не будем открывать это окно, если бд не открыта.
        reject();
    }
    db=humansDataBase;
}

GedcomImportDialog::~GedcomImportDialog()
{
    delete ui;
}

void GedcomImportDialog::on_pushButtonOpenGedcom_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Открытие файла", "", "Файлы GEDCOM(*.ged)");
    //отлично...теперь надо бы открыть, да попробовать распарсить.    
    ParseGedcom stat;
    parser.clear();
    stat=parser.parseGedcomFile(fileName);
    ui->textBrowser->append("Получено строк "+QString::number(stat.stringCount));
    ui->textBrowser->append("Человеков "+QString::number(stat.humanCount));
    ui->textBrowser->append("Неизвестных тэгов "+QString::number(stat.unknownCount));
    ui->textBrowser->append(stat.messages.join("\r\n"));

    for(int i=0;i<parser.humanList.count();i++)
    {
        ui->textBrowser->append("Найден человек: "+parser.humanList[i].getHumanInfo());
        if(parser.humanList[i].fatherListCount>=0) ui->textBrowser->append("\tОтец:"+parser.humanList[parser.humanList[i].fatherListCount].getHumanInfo());
        if(parser.humanList[i].motherListCount>=0) ui->textBrowser->append("\tМать:"+parser.humanList[parser.humanList[i].motherListCount].getHumanInfo());
    }

    for(int i=0;i<parser.familyList.count();i++)
    {
        ui->textBrowser->append("Найдена семья, в ней детей : "+QString::number(parser.familyList[i].children.count())+" и партнёров "+QString::number(parser.familyList[i].partners.count()));
        for(int j=0;j<parser.familyList[i].partnersCounts.count();j++)
        {
            //ui->textBrowser->append("\tПартнёр "+parser.humanList[parser.familyList[i].partnersCounts[j]].getInfo());

        }
        ui->textBrowser->append("Коммент: "+parser.familyList[i].note.join(";"));
    }
    ui->textBrowser->append("Всего людей "+QString::number(parser.humanList.count()));
    ui->textBrowser->append("Всего семей "+QString::number(parser.familyList.count()));
    //добавить людям детей из списка...и супругов из списка...ишь..ну в принципе почему б и нет...обработать семьи, да добавить.
}



void GedcomImportDialog::on_pushButtonAutoImport_clicked()
{
    //Берём парсер, берём БД и парсим и бдсим!
    QList<HumanData> *hlist;
    hlist=&parser.humanList;
    db->transactionStart();
    for(int i=0;i<hlist->count();i++)
    //for(int i=0;i<5;i++)
    {
        qDebug()<<"Добавляем человека "<<hlist->at(i).fullName;
        db->addHuman(hlist->at(i));
        //Проходим по всем человекам и добавляем в базу данные...
        //Может и не стоило, конечно, а может и стоило...Ладно, поехали.
    }
    db->transactionEnd();
    ui->textBrowser->append("Импорт человеков закончен. Человеки удалены.");

    //импорт семей нужен? вообще говоря да, нужен.
    //так что двай сначала этим, а потом уже сообщения, редактирование и отображение табличек (которое с горем пополам есть в других программах).
    QList<FamilyData> *flist;
    flist=&parser.familyList;
    db->transactionStart();
    for(int i=0;i<flist->count();i++)
    //for (int i=0;i<5;i++)
    {
        //таблица marriage, не иначе.
        qDebug()<<"Добавляем данные о браке "<<i;
        db->addMarriage(flist->at(i));
    }
    db->transactionEnd();
    hlist->clear();

}
