#ifndef SOURCEEDITDIALOG_H
#define SOURCEEDITDIALOG_H

#include <QDialog>

#include "humansdatabase.h"

namespace Ui {
class SourceEditDialog;
}

class SourceEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SourceEditDialog(HumansDatabase *humansDatabase, QString sourceUuid = "", QWidget *parent = 0);
    ~SourceEditDialog();    

private slots:
    void on_pushButtonDeleteSource_clicked();

    void on_pushButtonCancel_clicked();

    void on_pushButtonOk_clicked();

    void on_pushButtonGetLink_clicked();

    void on_pushButtonShow_clicked();

private:
    Ui::SourceEditDialog *ui;
    HumansDatabase *db;
    QString srcUid;
    SourceData sourceData;
    void fillSourceData();
    QStringList collectSourceData();
};

#endif // SOURCEEDITDIALOG_H
