#include <QMessageBox>

#include <QQuickItem>

#include "placeeditdialog.h"
#include "ui_placeeditdialog.h"

#include "placesearchdialog.h"
#include "recordsearchdialog.h"

PlaceEditDialog::PlaceEditDialog(HumansDatabase &humansDatabase, QString placeUuid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlaceEditDialog),
    db(humansDatabase)
{
    if(!db.isOpen())
    {
        QMessageBox::warning(this,"Внимание","База не открыта!");
       reject();
        return;
    }
    ui->setupUi(this);

    //db=humansDatabase;
    if(!placeUuid.isEmpty())
    {
        setWindowTitle("Редактирование места");
        ui->pushButtonOk->setText("Изменить");
    }
    else
    {
        setWindowTitle("Добавление нового места");
        ui->pushButtonOk->setText("Добавить");

    }
    pUid=placeUuid;
    placeData=db.getPlace(pUid);
    fillPlaceData();    
    ui->labelMap->setVisible(false);//labelMap нужно для того, чтобы оставить пустое место в gridLayout
    mapView = new QQuickView();
    mapView->setResizeMode(QQuickView::SizeRootObjectToView);//Эта штука помогает ресайзить маркеры. ВНЕЗАПНО!!!Почему - не знаю.
    mapContainer = QWidget::createWindowContainer(mapView, this);
    mapContainer->setFocusPolicy(Qt::TabFocus);
    mapView->setSource(QUrl("qrc:/setupmap.qml"));
    ui->gridLayout->addWidget(mapContainer,4,1);
    mapContainer->setVisible(false);
    mapContainer->setMinimumHeight(450);
    connect(mapView->rootObject(), SIGNAL(clickSignal(qreal,qreal)),this, SLOT(getCoordinates(qreal,qreal)));
}

PlaceEditDialog::~PlaceEditDialog()
{
    delete ui;
}

void PlaceEditDialog::on_pushButtonCancel_clicked()
{
    reject();
}

void PlaceEditDialog::on_pushButtonDelete_clicked()
{
    if(pUid.isEmpty())
    {
        QMessageBox::warning(this,"Внимание","Место ещё не добавлено в базу, так что удалить его нельзя.");
        return;
    }
    QMessageBox msgBox;
    msgBox.setText("Удаление важных данных");
    msgBox.setInformativeText("Вы действительно хотите удалить данные об этом месте?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret)
    {
       case QMessageBox::Ok:
           db.deletePlace(pUid);
           accept();
           break;
       case QMessageBox::Cancel:
           //Тоже в общем без последствий.
           break;
       default:
           // Ну так, пусть будет.
           break;
     }
}

void PlaceEditDialog::on_pushButtonOk_clicked()
{

    QStringList errors=collectPlaceData();
    if(errors.count()>0)
    {
        QMessageBox::warning(this,"Внимание","Ошибки какие-то:"+errors.join(", ")+".");
        return;
    }
    if(pUid.isEmpty())
    {
        bool res=db.addPlace(placeData);
        if(res)
            pUid=placeData.uuid;
    }
    else
    {
        db.editPlace(placeData);//должно уже работать, кстати.
    }
    accept();
}

void PlaceEditDialog::fillPlaceData()
{
    ui->lineEditName->setText(placeData.name);
    //ui->comboBoxType->setCurrentText(placeData.type);
    ui->comboBoxType->setEditText(placeData.type);
    ui->textEditAnotherNames->setText(placeData.anotherNames.join("\n"));    
    ui->lineEditLatitude->setText(placeData.latitude);
    ui->lineEditLongitude->setText(placeData.longitude);
    ui->lineEditCoordAccuracy->setText(placeData.coordAccuracy);
    ui->textEditNote->setText(placeData.note);
}

QStringList PlaceEditDialog::collectPlaceData()
{
    QStringList error;

    if(ui->lineEditName->text().isEmpty())
    {
        error.append("название не указано");
    }
    placeData.name=ui->lineEditName->text();
    placeData.type=ui->comboBoxType->currentText();
    placeData.anotherNames=ui->textEditAnotherNames->toPlainText().split("\n");    
    //Проверку координат сделать бы тоже хорошо. Но пока не будем.
    placeData.latitude=ui->lineEditLatitude->text();
    placeData.longitude=ui->lineEditLongitude->text();
    placeData.coordAccuracy=ui->lineEditCoordAccuracy->text();
    placeData.note=ui->textEditNote->toPlainText();
    return error;
}

void PlaceEditDialog::getCoordinates(qreal lat, qreal lon)
{
    ui->lineEditLatitude->setText(QString::number(lat));
    ui->lineEditLongitude->setText(QString::number(lon));
}

void PlaceEditDialog::on_pushButtonUnionPlaces_clicked()
{
    if(pUid.isEmpty())
    {
        QMessageBox::warning(this,"Внимание","Место только создаётся и пока его нельзя ни с каким другим местом объединить!");
        return;
    }

    PlaceSearchDialog dlg(db);
    if(dlg.exec()==QDialog::Rejected) return;
    if(dlg.getPlaceUuid().isEmpty()) return;

    QMessageBox msgBox;
    msgBox.setText("Внимание! Объединение!");
    msgBox.setInformativeText("Вы действительно хотите объединить данные о \""+placeData.getPlaceInfo()+"\" c найденным \""+dlg.getPlaceInfo()+"\"? Приоритет у найденных данных о месте.");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret)
    {
       case QMessageBox::Ok:
            db.unitePlaces(pUid,dlg.getPlaceUuid());
           //db->uniteHumans(hUid,dlg.getHumanUuid());
           reject();
           break;
       case QMessageBox::Cancel:
           //Тоже в общем без последствий.
           break;
       default:
           // Ну так, пусть будет.
           break;
     }
}

void PlaceEditDialog::on_pushButtonRecordSearch_clicked()
{
    RecordSearchDialog dlg(db,"",placeData.uuid);
    dlg.exec();
}

void PlaceEditDialog::on_toolButtonShowMap_clicked()
{
    //включить/выключить карту
    mapContainer->setVisible(mapContainer->isHidden());
    //запись координат на карту из полей ввода
    if(mapContainer->isVisible())
    {
        QString latitude=ui->lineEditLatitude->text();
        QString longitude=ui->lineEditLongitude->text();
        QMetaObject::invokeMethod(mapView->rootObject(), "setCenter",
                              Q_ARG(QVariant,latitude)
                              ,Q_ARG(QVariant,longitude)
                              );
        QMetaObject::invokeMethod(mapView->rootObject(), "setMarkerCoords",
                              Q_ARG(QVariant,latitude)
                              ,Q_ARG(QVariant,longitude)
                              );
    }
}
