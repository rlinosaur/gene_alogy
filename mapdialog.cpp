#include <QQmlApplicationEngine>
#include <QClipboard>

#include "mapdialog.h"
#include "ui_mapdialog.h"

#include "placeeditdialog.h"

MapDialog::MapDialog(HumansDatabase *humansDatabase, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapDialog)
{
    ui->setupUi(this);

    db = humansDatabase;

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/map.qml")));


    mapView = new QQuickView();
    mapView->setResizeMode(QQuickView::SizeRootObjectToView);
    mapContainer = QWidget::createWindowContainer(mapView, this);
    mapContainer->setFocusPolicy(Qt::TabFocus);
    mapContainer->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    mapView->setSource(QUrl("qrc:/map.qml"));
    mapItem = mapView->rootObject();
    connect(mapItem,SIGNAL(placeClick(QString)),this,SLOT(placeClick(QString)));
    connect(mapItem,SIGNAL(mapClick(qreal,qreal)),this,SLOT(mapClick(qreal,qreal)));

    setLayout((ui->verticalLayout));
    this->layout()->addWidget(mapContainer);

    bottomWidget = new QWidget;
    //ui->horizontalLayout->setParent(bottomWidget);
    bottomWidget->setLayout(ui->horizontalLayout);

    mainVerticalSplitter = new QSplitter;
    mainVerticalSplitter->setOrientation(Qt::Vertical);
    //mainVerticalSplitter->addWidget(ui->pushButtonPressMe);
    mainVerticalSplitter->addWidget(mapContainer);
    mainVerticalSplitter->addWidget(bottomWidget);

    //mainVerticalSplitter->addWidget(ui->pushButtonClose);

    //mainVerticalSplitter->addWidget(ui->textEditLog);

    ui->verticalLayout->addWidget(mainVerticalSplitter);


    mapContainer->resize(mapContainer->width(),this->height());



    /*
    qDebug()<<"test existence of coordinates "<<humansDatabase->isFieldExists("places","coordinates");
    qDebug()<<"test existence of lan "<<humansDatabase->isFieldExists("places","lan");
    QList<PlaceData> places = humansDatabase->getPlaces();
    for(int i=0;i<places.size();i++)
    {
        qDebug()<<"Place found name "<<places[i].name;
    }
    */
    //Fill map objects here! a-ha-ha-ha-ha!
    QList<PlaceData> places = humansDatabase->getPlaces();
    addPlacesToMap(places);
}

MapDialog::~MapDialog()
{
    delete ui;
}

void MapDialog::on_pushButtonClose_clicked()
{
    reject();
}

void MapDialog::placeClick(QString uuid)
{
    qDebug()<<"Device with uuid "<<uuid<<" click";
    PlaceEditDialog dlg(db,uuid);
    dlg.exec();

    //PlaceData place=db->getPlace(uuid);
    //changePlaceOntoMap(place);
    //PlaceData place=db->getPlace(uuid);
    changePlaceOntoMap(db->getPlace(uuid));

}

void MapDialog::mapClick(qreal lat, qreal lon)
{
    qDebug()<<"click "<<lat<<", "<<lon;
    ui->lineEditCoordinates->setText(QString::number(lat)+", "+QString::number(lon));

}

void MapDialog::addPlacesToMap(const QList<PlaceData> &places)
{
    clearPlaces();
    for (int i=0;i<places.size();i++)
    {
        addPlaceToMap(places[i]);
    }
}

void MapDialog::addPlaceToMap(const PlaceData &place)
{
    if(place.latitude.isEmpty() || place.longitude.isEmpty()) return;

    QMetaObject::invokeMethod(mapItem, "addPlace",
                              Q_ARG(QVariant,place.uuid)
                              ,Q_ARG(QVariant,place.latitude)
                              ,Q_ARG(QVariant,place.longitude)
                              ,Q_ARG(QVariant,place.name)
                              ,Q_ARG(QVariant,place.anotherNames.join(','))
                              );
}

void MapDialog::changePlaceOntoMap(const PlaceData &place)
{
    QMetaObject::invokeMethod(mapItem, "changePlace",
                              Q_ARG(QVariant,place.uuid)
                              ,Q_ARG(QVariant,place.latitude)
                              ,Q_ARG(QVariant,place.longitude)
                              ,Q_ARG(QVariant,place.name)
                              ,Q_ARG(QVariant,place.anotherNames.join(','))
                              );

}

void MapDialog::clearPlaces()
{
    QMetaObject::invokeMethod(mapItem, "clearAllPlaces");
}

void MapDialog::on_pushButtonCopyCoordinates_clicked()
{
    QClipboard* c = QApplication::clipboard();
    c->setText(ui->lineEditCoordinates->text());

}
