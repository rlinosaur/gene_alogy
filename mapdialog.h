#ifndef MAPDIALOG_H
#define MAPDIALOG_H

#include <QtQuick/QQuickItem>
#include <QQuickView>
#include <QSplitter>

#include <QDialog>

#include "humansdatabase.h"


namespace Ui {
class MapDialog;
}

class MapDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MapDialog(HumansDatabase *humansDatabase, QWidget *parent = 0);
    ~MapDialog();

private slots:
    void on_pushButtonClose_clicked();

private:
    Ui::MapDialog *ui;

     QSplitter *mainVerticalSplitter;

    QQuickItem *mapItem;
    QQuickView *mapView;
    QWidget *mapContainer;

    QWidget *bottomWidget = new QWidget;
};

#endif // MAPDIALOG_H
