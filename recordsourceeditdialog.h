#ifndef RECORDSOURCEEDITDIALOG_H
#define RECORDSOURCEEDITDIALOG_H

#include <QDialog>

#include "humansdatabase.h"

namespace Ui {
class RecordSourceEditDialog;
}

class RecordSourceEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecordSourceEditDialog(HumansDatabase *humansDatabase, QString recordUuid, QString recordSourceUuid="", QWidget *parent = 0);
    ~RecordSourceEditDialog();

private slots:
    void on_pushButtonCancel_clicked();

    void on_pushButtonOk_clicked();

    void on_pushButtonSearchSource_clicked();

private:
    Ui::RecordSourceEditDialog *ui;    
    HumansDatabase *db;
    QString recUid;
    QString srcRecUid;
    RecordSourceData recordSourceData;
    SourceData sourceData;

    QStringList collectRecordSourceData();
    void fillRecordSourceData();
    void fillSourceInfo(SourceData srcDat);
};

#endif // RECORDSOURCEEDITDIALOG_H
