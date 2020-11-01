#ifndef HUMANPLACEDIALOG_H
#define HUMANPLACEDIALOG_H

#include <QDialog>

#include "humansdatabase.h"

namespace Ui {
class HumanPlaceDialog;
}

class HumanPlaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HumanPlaceDialog(HumansDatabase *humansDatabase, QString humanUuid, QString humanPlaceUuid="",QWidget *parent = 0);
    ~HumanPlaceDialog();

private slots:
    void on_pushButtonPlaceSearch_clicked();

    void on_pushButtonSaveAndClose_clicked();

    void on_pushButtonCancel_clicked();

private:
    Ui::HumanPlaceDialog *ui;

    HumansDatabase *db;

    HumanPlaceData humanPlaceData;
    HumanData humanData;
    PlaceData placeData;

    QString hpUuid;

    void fillHumanPlaceData();
    QStringList collectHumanPlaceData();
};

#endif // HUMANPLACEDIALOG_H
