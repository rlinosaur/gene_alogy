#include <QGraphicsItem>

#include "visualizedialog.h"
#include "ui_visualizedialog.h"

VisualizeDialog::VisualizeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VisualizeDialog)
{
    ui->setupUi(this);

    setLayout(ui->verticalLayoutMain);
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
    setWindowTitle("Глядим себе, рисуем.");



}

VisualizeDialog::~VisualizeDialog()
{
    delete ui;
}

void VisualizeDialog::visualizeTree(QList<HumanData> *list)
{
    //на каждый элемент создать квадрат. отрисовать взаимодействия, наблюдать за результатом. как-то так.

    ui->graphicsView->visualizeTree(list);
}
