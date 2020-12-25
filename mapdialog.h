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
    explicit MapDialog(const HumansDatabase &humansDatabase, QWidget *parent = 0);
    ~MapDialog();

private slots:
    void on_pushButtonClose_clicked();

    void placeClick(QString uuid);
    void mapClick(qreal lat, qreal lon);

    void on_pushButtonCopyCoordinates_clicked();

private:
    Ui::MapDialog *ui;

     QSplitter *mainVerticalSplitter;

    QQuickItem *mapItem;
    QQuickView *mapView;
    QWidget *mapContainer;

    QWidget *bottomWidget = new QWidget;

    const HumansDatabase &db;

    void addPlacesToMap(const QList<PlaceData> &places);
    void addPlaceToMap(const PlaceData &place);
    void changePlaceOntoMap(const PlaceData &place);
    void clearPlaces();
};

#endif // MAPDIALOG_H
