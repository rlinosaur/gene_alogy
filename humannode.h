#ifndef HUMANNODE_H
#define HUMANNODE_H

#include <QPen>
#include <QBrush>
#include <QObject>
#include <QGraphicsItem>

#define HUMANNODE_DEFAULT_WIDTH 100
#define HUMANNODE_DEFAULT_HEIGHT 40
#define HUMANNODE_DEFAULT_MARGIN 5

////////////////////////////////////
/// \brief The HumanNode class для визуализации
///
////////////////////////////////////
class HumanNode : public QGraphicsItem //, QObject //works, but no need right now.
{        
public:
    HumanNode();

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setText(QString text);
    void setRect(qreal x, qreal y, qreal width, qreal height);
    void setBrush(QBrush brush);
    void setBrushColor(QColor color);
    void setPen(QPen pen);
    void setPenColor(QColor color);
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    QPointF coor;
    QRectF humanRect;
    QString humanText;
    QBrush humanBrush;
    QPen humanPen;
};

#endif // HUMANNODE_H
