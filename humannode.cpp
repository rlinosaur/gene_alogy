#include <QStyle>
#include <QDebug>
#include <QPainter>
#include <QStyleOption>

#include "humannode.h"


HumanNode::HumanNode()
{    
    //qDebug()<<"constructor:";
    //humanRect.setRect(0,0,HUMANNODE_DEFAULT_WIDTH,HUMANNODE_DEFAULT_HEIGHT);
    humanBrush=Qt::green;
    humanPen=QPen(Qt::darkBlue, 0);
    humanRect.setRect(0,0,100,40);
    //qDebug()<<"constructor:"<<humanRect;

    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
}

QRectF HumanNode::boundingRect() const
{
    //qDebug()<<"painter "<<humanRect;
    return humanRect;
  //  return QRectF(0,0,HUMANNODE_DEFAULT_WIDTH,HUMANNODE_DEFAULT_HEIGHT);
    //qreal adjust = 2;
    //return QRectF( -10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust);

    //

}

QPainterPath HumanNode::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void HumanNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
Q_UNUSED(widget);
Q_UNUSED(option);
    //painter->setPen(Qt::NoPen);
    painter->setPen(humanPen);
    painter->setBrush(humanBrush);
    painter->drawRect(humanRect);
    painter->drawText(humanRect,humanText);
}

void HumanNode::setText(QString text)
{
    humanText=text;
    update();
}

void HumanNode::setRect(qreal x, qreal y, qreal width, qreal height)
{
    humanRect.setRect(x,y,width,height);
    update();
}

QVariant HumanNode::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
        case ItemPositionHasChanged:
           // qDebug()<<"Item position has been changed! Where? WHyYY&&?";
        break;
        default:

        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void HumanNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
   // qDebug()<<"Hho! You checked me! MEE!";
    update();
    QGraphicsItem::mousePressEvent(event);
}

void HumanNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  //  qDebug()<<"Почему-то не случается..неудачо!";
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

void HumanNode::setBrush(QBrush brush)
{
    humanBrush=brush;
}

void HumanNode::setBrushColor(QColor color)
{
    humanBrush.setColor(color);
}

void HumanNode::setPen(QPen pen)
{
    humanPen=pen;
}

void HumanNode::setPenColor(QColor color)
{
    humanPen.setColor(color);
}
