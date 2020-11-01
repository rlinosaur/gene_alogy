#ifndef SOURCESEARCHDIALOG_H
#define SOURCESEARCHDIALOG_H

#include <QDialog>
#include <QModelIndex>

#include "humansdatabase.h"

namespace Ui {
class SourceSearchDialog;
}

class SourceSearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SourceSearchDialog(HumansDatabase *humansDatabase, QWidget *parent = 0);
    ~SourceSearchDialog();
    QString getSourceId();
    QString getSourceInfo();
private slots:
    void on_pushButtonCancel_clicked();

    void on_pushButtonNewSource_clicked();

    void on_pushButtonOk_clicked();

    void editSource(QModelIndex ind);

    void on_pushButtonDelete_clicked();

    void on_pushButtonSearch_clicked();

    void on_comboBoxTypes_activated(int index);

private:
    Ui::SourceSearchDialog *ui;
    QString sourceIdSelected;
    QString sourceInfoSelected;
    QList<QStringList> sources;
    HumansDatabase *db;

    void updateSources();

};

#endif // SOURCESEARCHDIALOG_H
