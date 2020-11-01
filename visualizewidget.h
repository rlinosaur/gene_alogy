#ifndef VISUALIZEWIDGET_H
#define VISUALIZEWIDGET_H

#include <QGraphicsView>

#include "humannode.h"
#include "humandata.h"

#define ME_NO_MOUSEEVENTS_FOR_GRAPHICS

class VisualizeWidget : public QGraphicsView
{
    Q_OBJECT
public:
    explicit VisualizeWidget(QWidget *parent = 0);


    void visualizeTree(QList<HumanData> *list);

protected:

    void mouseDoubleClickEvent(QMouseEvent *event);

#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *event);
#endif

#ifndef ME_NO_MOUSEEVENTS_FOR_GRAPHICS
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
#endif
    void drawBackground(QPainter *painter, const QRectF &rect);
    void scaleView(qreal scaleFactor);
signals:

public slots:

private:

    int dXMouse;
    int dYMouse;

    QGraphicsScene scene;
    QList<QGraphicsItem *> nodes;

};

#endif // VISUALIZEWIDGET_H
