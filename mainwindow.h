#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include <QtGui/QMainWindow>
#include <QMainWindow>
#include <QTranslator>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>


#include "humansdatabase.h"
#include "tablerawdatedelegate.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();    

private slots:
    void on_action_GEDCOM_triggered();
    void on_action_close_db_triggered();

    void on_action_open_db_triggered();

    void on_action_create_db_triggered();

    void on_action_exit_triggered();

    void on_pushButtonCreateRecord_clicked();

    void on_pushButtonSearchHuman_clicked();

    void on_tabWidget_currentChanged(int index);

    void tableHumansActivate(QModelIndex index);
    void tableRecordsActivate(QModelIndex index);
    void tablePlacesActivate(QModelIndex index);

    void on_pushButtonHumanSearch_clicked();

    void on_lineEditHumanSearch_returnPressed();

    void on_pushButtonCreateIndividual_clicked();

    void on_pushButtonEditHuman_clicked();

    void on_pushButtonEditRecord_clicked();

    void on_pushButtonEditPlace_clicked();

    void on_pushButtonPlaceAdd_clicked();

    void on_pushButtonDeleteHuman_clicked();

    void on_pushButtonRecordDelete_clicked();

    void on_pushButtonPlaceDelete_clicked();

    void on_pushButtonRecordSearch_clicked();

    void on_lineEditRecordSearch_returnPressed();

    void on_pushButtonPlaceSearch_clicked();

    void on_lineEditPlaceSearch_returnPressed();

    void on_pushButtonMiscGetHuman_clicked();

    void on_pushButton_clicked();

    void on_pushButtonMiscGetCousin_clicked();

    void on_pushButtonSearchCommon_clicked();

    void on_pushButtonMiscSearchAllCommon_clicked();

    void on_pushButtonMiscTree_clicked();



    void mischListActivated(QModelIndex index);
    void on_pushButtonSearchDuplicates_clicked();

    void on_pushButtonGedcomExport_clicked();

    void on_pushButtonPlaceForRecord_clicked();

    void on_pushButtonPlaceForRecordClear_clicked();

    void on_pushButtonPlaceForHumansSearch_clicked();

    void on_pushButtonPlaceForHumansClear_clicked();

    void on_pushButtonFindRelatives_clicked();

    void on_actionEnglish_triggered();

    void on_actionRussian_triggered();

    void on_action_about_triggered();

    void on_action_triggered();
    
    void on_pushButtonMap_clicked();

private:
    Ui::MainWindow *ui;
    QTranslator translator; // contains the translations for this application
    QTranslator translatorQt; // contains the translations for qt

    HumansDatabase db;

    void messageDatabaseClosed();
    void initModelHumans();
    void initModelRecords();
    void initModelPlaces();

    QSqlQueryModel modelHumans;
    QSqlQueryModel modelRecords;
    QSqlQueryModel modelPlaces;

    QSortFilterProxyModel modelHumansSort;
    QSortFilterProxyModel modelRecordsSort;
    QSortFilterProxyModel modelPlacesSort;


    //Мiscellaneous, то бишь разное.
    QString miscHumanId;
    QString miscCousinId;
    QList<HumanData> miscHumanList;
    QList<HumanData> miscCousinList;

    TableRawDateDelegate delegateTimeToString;
    TableRawDateDelegate *hu;

    //Records
    QString placeIdForSearchRecords;
    //Humans
    QString placeIdForSearchHumans;

    //Translation
    bool changeTranslator(QTranslator &translationClass, QString translationFileName);
};

#endif // MAINWINDOW_H
