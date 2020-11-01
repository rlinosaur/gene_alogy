#ifndef VISUALIZEDIALOG_H
#define VISUALIZEDIALOG_H

#include <QDialog>
#include <QGraphicsScene>
#include "humandata.h"

#include "humannode.h"


namespace Ui {
class VisualizeDialog;
}

class VisualizeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VisualizeDialog(QWidget *parent = 0);
    ~VisualizeDialog();

    void visualizeTree(QList<HumanData> *list);

private:
    Ui::VisualizeDialog *ui;
};

#endif // VISUALIZEDIALOG_H
