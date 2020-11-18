#include <QQmlApplicationEngine>

#include "mapdialog.h"
#include "ui_mapdialog.h"

MapDialog::MapDialog(HumansDatabase *humansDatabase, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapDialog)
{
    ui->setupUi(this);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/map.qml")));


    mapView = new QQuickView();
    mapView->setResizeMode(QQuickView::SizeRootObjectToView);
    mapContainer = QWidget::createWindowContainer(mapView, this);
    mapContainer->setFocusPolicy(Qt::TabFocus);
    mapContainer->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    mapView->setSource(QUrl("qrc:/map.qml"));
    mapItem = mapView->rootObject();

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

    qDebug()<<"test existence of coordinates "<<humansDatabase->isFieldExists("places","coordinates");
    qDebug()<<"test existence of lan "<<humansDatabase->isFieldExists("places","lan");



}

MapDialog::~MapDialog()
{
    delete ui;
}

void MapDialog::on_pushButtonClose_clicked()
{
    reject();
}
