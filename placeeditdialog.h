#ifndef PLACEEDITDIALOG_H
#define PLACEEDITDIALOG_H

#include <QDialog>

#include "humansdatabase.h"

namespace Ui {
class PlaceEditDialog;
}

class PlaceEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlaceEditDialog(HumansDatabase *humansDatabase, QString placeUuid = "", QWidget *parent = 0);
    ~PlaceEditDialog();

    QString getPlaceUuid(){return pUid;}
    QString getPlaceInfo(){return placeData.getPlaceInfo();}

private slots:
    void on_pushButtonCancel_clicked();

    void on_pushButtonDelete_clicked();

    void on_pushButtonOk_clicked();

    void on_pushButtonUnionPlaces_clicked();

    void on_pushButtonRecordSearch_clicked();

private:
    Ui::PlaceEditDialog *ui;
    HumansDatabase *db;
    QString pUid;    
    PlaceData placeData;


    void fillPlaceData();
    QStringList collectPlaceData();
};

#endif // PLACEEDITDIALOG_H
