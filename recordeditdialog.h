#ifndef RECORDEDITDIALOG_H
#define RECORDEDITDIALOG_H

#include <QDialog>
#include <QModelIndex>

#include "humansdatabase.h"

namespace Ui {
class RecordEditDialog;
}

class RecordEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecordEditDialog(HumansDatabase *humansDatabase, QString recordUuid = "", QWidget *parent = 0);
    ~RecordEditDialog();
    RecordData getRecordData(){return recordData;}
    static QString getTabTable(QString text);
private slots:
    void on_pushButtonEdit_clicked();

    void on_pushButtonApply_clicked();

    void on_pushButtonCancel_clicked();

    void on_pushButtonOk_clicked();

    void on_pushButtonSearchSource_clicked();

    void on_pushButtonNewHuman_clicked();

    void on_pushButtonSearchHuman_clicked();


    void listHumansActivated(QModelIndex index);
    void listPlacesActivated(QModelIndex index);
    void listSourcesActivated(QModelIndex index);

    void on_pushButtonRemoveHuman_clicked();

    void on_pushButtonNewPlace_clicked();

    void on_pushButtonSearchPlace_clicked();

    void on_pushButtonSave_clicked();

    void on_pushButtonRemovePlace_clicked();

    void on_pushButtonDelete_clicked();

    void on_pushButtonRecordPlaceSearch_clicked();

    void on_pushButtonPlusFont_clicked();

    void on_pushButtonMinusFont_clicked();

    void on_pushButtonNewRecordSource_clicked();

    void on_pushButtonDeleteRecordSource_clicked();

private:
    Ui::RecordEditDialog *ui;
    RecordData recordData;
    HumansDatabase *db;
    QString recUid;

    QList<QStringList> humansData;
    QList<QStringList> placesData;
    QList<QStringList> sourcesData;

    void fillRecordData();
    QStringList collectRecordData();

    void updateHumansData();
    void updatePlacesData();
    void updateRecordSourcesData();
};
#endif // RECORDEDITDIALOG_H
