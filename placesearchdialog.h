#ifndef PLACESEARCHDIALOG_H
#define PLACESEARCHDIALOG_H

#include <QDialog>
#include <QModelIndex>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>

#include "humansdatabase.h"

namespace Ui {
class PlaceSearchDialog;
}

class PlaceSearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlaceSearchDialog(HumansDatabase &humansDatabase, QWidget *parent = 0);
    ~PlaceSearchDialog();
    QString getPlaceUuid(){return placeUuid;}
    QString getPlaceInfo(){return placeInfo;}
private slots:
    void on_pushButtonSearch_clicked();

    void tableSearchClickSlot(QModelIndex index);
    void tableSearchActivateSlot(QModelIndex index);

    void on_lineEditSearch_returnPressed();

    void on_pushButtonCancel_clicked();

    void on_pushButtonOk_clicked();

    void on_pushButtonNewPlace_clicked();

private:
    Ui::PlaceSearchDialog *ui;
    QSortFilterProxyModel modelSort;
    QSqlQueryModel model;
    QString placeUuid;
    QString placeInfo;
    HumansDatabase &db;
    QModelIndex currentSelectionIndex;
};

#endif // PLACESEARCHDIALOG_H
