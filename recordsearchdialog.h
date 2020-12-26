#ifndef RECORDSEARCHDIALOG_H
#define RECORDSEARCHDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>

#include "humansdatabase.h"

namespace Ui {
class RecordSearchDialog;
}

class RecordSearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecordSearchDialog(HumansDatabase &humansDatabase, QString humanId="", QString placeId="", QString searchString="", QWidget *parent = 0);
    ~RecordSearchDialog();

private slots:
    void on_pushButtonOk_clicked();

    //void tableSearchClickSlot(QModelIndex index);
    void tableSearchActivateSlot(QModelIndex index);

    void on_pushButtonSearch_clicked();

    void on_pushButtonSearchHuman_clicked();

    void on_pushButtonDeletHuman_clicked();

    void on_pushButtonSearchPlace_clicked();

    void on_pushButtonDeletePlace_clicked();

private:
    Ui::RecordSearchDialog *ui;
    HumansDatabase &db;
    QSqlQueryModel model;
    QSortFilterProxyModel modelSort;
    QString huId;
    QString plId;
};

#endif // RECORDSEARCHDIALOG_H
