#ifndef HUMANEDITDIALOG_H
#define HUMANEDITDIALOG_H

#include <QDialog>
#include <QModelIndex>

#include "humansdatabase.h"

namespace Ui {
class HumanEditDialog;
}

class HumanEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HumanEditDialog(HumansDatabase &humansDatabase, QString humanUuid = "", QWidget *parent = 0);
    ~HumanEditDialog();

    void addFatherInfo(QString fatherUid);
    void addMotherInfo(QString motherUid);
    void setSurname(QString surname);
    void setPatrynomic(QString patrynomic);

    QString getHumanUuid(){return hUid;}
    QString getHumanInfo(){return humanData.getHumanInfo();}

private slots:   

    void on_pushButtonSearchFather_clicked();

    void on_pushButtonSearchMother_clicked();

    void on_pushButtonFatherDelete_clicked();

    void on_pushButtonMotherDelete_clicked();

    void on_pushButtonCancel_clicked();

    void on_pushButtonOk_clicked();

    void on_pushButtonAddSpouse_clicked();

    void on_pushButtonChild_clicked();

    void on_pushButtonChildSearch_clicked();

    void listChildrenActivate(QModelIndex index);
    void listSpousesActivate(QModelIndex index);
    void listPlacesActivate(QModelIndex index);
    void listSiblingsActivate(QModelIndex index);

    void on_checkBoxLife_toggled(bool checked);

    void on_pushButtonSave_clicked();

    void on_pushButtonDelete_clicked();

    void on_pushButtonSearchPlace_clicked();

    void on_pushButtonDeleteMarriage_clicked();

    void on_pushButton_clicked();

    void on_pushButtonSearchRecords_clicked();

    void on_pushButtonEditMother_clicked();

    void on_pushButtonEditFather_clicked();

    void on_pushButtonUnionHumans_clicked();

    void on_pushButtonPlaceAdd_clicked();

    void on_pushButtonPlaceDelete_clicked();

    void on_pushButtonPlaceEdit_clicked();

    void on_pushButtonAddSibling_clicked();

    void on_pushButtonPlaceAddLast_clicked();

private:
    Ui::HumanEditDialog *ui;
    HumansDatabase &db;
    QString hUid;
    QString humanInfo;
    HumanData humanData;
    QList<QStringList> childrenData;
    QList<QStringList> spousesData;
    QList<QStringList> placesData;
    QList<QStringList> siblingsData;

    void fillHumanData();
    QStringList collectHumanData();

    void updateChildrenData();
    void updateSpousesData();
    void updatePlacesData();
    void updateSiblingsData();

};

#endif // HUMANEDITDIALOG_H
