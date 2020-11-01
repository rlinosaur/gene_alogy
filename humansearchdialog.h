#ifndef HUMANSEARCHDIALOG_H
#define HUMANSEARCHDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>

#include "humandata.h"
#include "humansdatabase.h"

namespace Ui {
class HumanSearchDialog;
}

class HumanSearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HumanSearchDialog(HumansDatabase *humansDatabase, HumanSex sexData=humanSexUndefined, QString searchString="", QWidget *parent = 0);
    ~HumanSearchDialog();    
    QString getHumanUuid(){return humanUuid;}
    QString getHumanInfo(){return humanInfo;}
private slots:
    void on_pushButtonSearch_clicked();

    void on_lineEditSearch_returnPressed();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void tableSearchClickSlot(QModelIndex index);
    void tableSearchActivateSlot(QModelIndex index);

    void on_pushButtonNewHuman_clicked();

private:
    Ui::HumanSearchDialog *ui;
    HumanSex sex;
    HumansDatabase *db;
    QSqlQueryModel model;
    QSortFilterProxyModel modelSort;
    QModelIndex currentSelectionIndex;
    QString humanUuid;
    QString humanInfo;
};

#endif // HUMANSEARCHDIALOG_H
