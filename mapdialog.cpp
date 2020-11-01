#include <QLocation>
#include <QtPositioning>

#include "mapdialog.h"
#include "ui_mapdialog.h"

MapDialog::MapDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapDialog)
{
    ui->setupUi(this);
    QLocation location;
}

MapDialog::~MapDialog()
{
    delete ui;
}

void MapDialog::on_pushButtonClose_clicked()
{
    reject();
}
