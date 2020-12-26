#ifndef MARRIAGEEDITDIALOG_H
#define MARRIAGEEDITDIALOG_H

#include <QDialog>

#include "humandata.h"
#include "humansdatabase.h"

namespace Ui {
class MarriageEditDialog;
}

class MarriageEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MarriageEditDialog(HumansDatabase &humansDatabase, QString familyUid="", QWidget *parent = 0);
    ~MarriageEditDialog();

    void addHusbandInfo(QString husbandId);
    void addWifeInfo(QString wifeId);

private slots:
    void on_pushButtonSearchHusband_clicked();

    void on_pushButtonSearchWife_clicked();

    void on_pushButtonSearchPlace_clicked();

    void on_pushButtonCancel_clicked();

    void on_pushButtonDeleteMarriage_clicked();

    void on_pushButtonOk_clicked();

private:
    Ui::MarriageEditDialog *ui;
    HumansDatabase &db;
    QString fUid;
    FamilyData familyData;
    HumanData husbandData;
    HumanData wifeData;

    void fillMarriageData();
    QStringList collectMarriageData();
};

#endif // MARRIAGEEDITDIALOG_H
