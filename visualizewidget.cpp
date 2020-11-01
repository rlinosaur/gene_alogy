#include <QWheelEvent>
#include <QMouseEvent>
#include <QDebug>

#include "humannode.h"

#include <math.h>

#include "visualizewidget.h"

VisualizeWidget::VisualizeWidget(QWidget *parent) :
    QGraphicsView(parent)
{
    setScene(&scene);


    for(int i=0;i<9;i++)
    {
        HumanNode *node = new HumanNode();
        node->setPos(i*10,i*10);
        node->setText("Вася №"+QString::number(i+1)+"\nДруг человека.");
        nodes.append(node);
        scene.addItem(node);
    }
    //connect(this,SIGNAL())
    //connect(&scene,SIGNAL())
}

void VisualizeWidget::visualizeTree(QList<HumanData> *list)
{

    //У нас есть список. Это хорошо? Ну...Это как минимум неплохо.
    //1. Сортировать список HumanData...как? Без понятия..но как-то надо.
/*
    for(int i=0;i<list->size();i++)
    {
        qDebug()<<"list level"<<list->at(i).recursyList.size()<<"item "<<list->at(i).fullName;
    }
    //Вот эта сортировка она нам что даст? Она нам даст то, что список будет отсортирован...
    //такие дела...ну и ладно, пусть будет...оно не лишнее.
    //Но надо учитывать этот факт на будущее. Лучше передать список целиком, а не сортировать имеющийся.
    std::sort(list->begin(),list->end(),HumanData::sortHumanLevelLessThan);
    //гм...ну прям ваще...вы чо творите-то?

    qDebug()<<"After";

    for(int i=0;i<list->size();i++)
    {
        qDebug()<<"list level"<<list->at(i).recursyList.size()<<"item "<<list->at(i).fullName;
    }

    qDebug()<<"Visualize TREEE";
    return;
    */
    //отбой, все уже отсортированы.
    scene.clear();
    //for(int i=0;i<nodes.size();i++) delete nodes[i];
    nodes.clear();

    //а теперь вот рисуем, ага.
    qint8 currentLevelXCount=0;
    qint8 level=0;
    qint8 maxlevel=list->last().recursyList.size()+1;
    for(int i=0;i<list->size();i++)
    {
        int defWidth=100;
        int defHeight=100;
        int defMarg=0;

        HumanData data;
        data=list->at(i);
        if (level<data.recursyList.size())
        {
            level=data.recursyList.size();
            currentLevelXCount=0;
        }
        int y=data.recursyList.size()*(defHeight+defMarg);// - для расчёта высоты
        int x=currentLevelXCount*(defWidth+defMarg);
        currentLevelXCount++;
        HumanNode *node = new HumanNode();
        //node->setColor(QColor("green"));
        //node->setPaintBrush(Qt::green);
        int colorC=(RAND_MAX/255)/(level+1)*maxlevel;
        int red=qrand()/colorC;
        int green=qrand()/colorC;
        int blue=qrand()/colorC;
        node->setBrushColor(QColor(red,green,blue));
        node->setPenColor(QColor(255-red,255-green,255-blue));
        node->setRect(0,0,(defWidth+defMarg),defHeight);
        node->setPos(x,y);
        QString str;
        //str.append(","+data.getLevelPositionString()+","+QString::number(data.getLevelPosition())+",");
        str.append("ш:"+QString::number((defWidth+defMarg))+",l:"+QString::number(level));
        str.append(data.getHumanInfo());
        node->setText(str);
        nodes.append(node);
        scene.addItem(node);
    }

    return;


    ///////////////////////////
    /// \brief supermaxlevel
    /// Old version of visualization
    ///////////////////////////
    int supermaxlevel=20;
    int maxLevel=0;
    for(int i=0;i<list->size();i++)
    {
        HumanData data;
        data=list->at(i);
        if(maxLevel<data.recursyList.size())maxLevel=data.recursyList.size();
    }
    maxLevel+=1;
    if(maxLevel>supermaxlevel)maxLevel=supermaxlevel;
    //ага..значит максимальный уровень тут нам известен....
qDebug()<<"maxlevel "<<maxLevel;
    for(int i=0;i<list->size();i++)
    {

         int defWidth=100;
         int defHeight=100;
         int defMarg=0;

        //node->setPos(i*10,i*10);
        //НЕЭФФЕКТИВНЫЙ КОД. ЧТО-ТО надо сделать. скорее всего список указателей а не указатель на список.
        HumanData data;
        data=list->at(i);
        int level=data.recursyList.size();
        if(level>supermaxlevel)continue;
        int coeff=1<<(maxLevel-level);
        int y=data.recursyList.size()*(defHeight+defMarg);// - для расчёта высоты
        //int x=data.getLevelPosition()*(2<<(maxLevel-level))*(HUMANNODE_DEFAULT_WIDTH+HUMANNODE_DEFAULT_MARGIN);// - для расчёта ширины

        int x=data.getLevelPosition()*(defWidth+defMarg)*coeff;// - для расчёта ширины

        HumanNode *node = new HumanNode();
        node->setRect(0,0,(defWidth+defMarg)*coeff,defHeight);
        node->setPos(x,y);
        QString str;
        //str.append(","+data.getLevelPositionString()+","+QString::number(data.getLevelPosition())+",");
        str.append("ш:"+QString::number((defWidth+defMarg)*coeff)+", m: "+QString::number(maxLevel)+",l:"+QString::number(level));
        str.append(data.getHumanInfo());
        node->setText(str);
        nodes.append(node);
        scene.addItem(node);
    }

}

void VisualizeWidget::mouseDoubleClickEvent(QMouseEvent *event)
{

    //    if ( e->button() == Qt::LeftButton )
      qDebug()<<"Doubleclick is happening...";
      //SOMETHING WRONG! But doubleclick must survive!
      //HumanNode *noddy=(HumanNode *)scene.focusItem();
      // HumanNode *noddy=reinterpret_cast<HumanNode*>(scene.focusItem());
      //noddy->setBrushColor(QColor(15,15,255));
}

#ifndef QT_NO_WHEELEVENT
void VisualizeWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
    //shear(1.5,1.2);//сжатие, не надо.
    translate(1500,1500);
}

void VisualizeWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    // Shadow
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
        painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
        painter->fillRect(bottomShadow, Qt::darkGray);

    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersected(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);

    // Text
    QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
                    sceneRect.width() - 4, sceneRect.height() - 4);
    QString message("Просто фоновый текст");

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(14);
    painter->setFont(font);
    painter->setPen(Qt::lightGray);
    painter->drawText(textRect.translated(2, 2), message);
    painter->setPen(Qt::black);
    painter->drawText(textRect, message);
}
#endif

#ifndef ME_NO_MOUSEEVENTS_FOR_GRAPHICS
void VisualizeWidget::mousePressEvent(QMouseEvent *event)
{
    dXMouse=event->x();
    dYMouse=event->y();
    qDebug()<<"Start: "<<dXMouse<<", "<<dYMouse;

}

void VisualizeWidget::mouseReleaseEvent(QMouseEvent *event)
{
    /*
    qDebug()<<"End: "<<event->x()<<", "<<event->y();
    dXMouse=event->x()-dXMouse;
    dYMouse=event->y()-dYMouse;

    qDebug()<<"DX: "<<dXMouse<<", "<<dYMouse;

    for(int i=0;i<nodes.size();i++)
    {
        QPointF pos=nodes.at(i)->pos();
        nodes.at(i)->setPos(pos.x()+dXMouse,pos.y()+dYMouse);
    }
    */
}

void VisualizeWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(!event->buttons()&Qt::LeftButton) return;
    int pdx=event->x()-dXMouse;
    int pdy=event->y()-dYMouse;
    if(pdx>5 || pdx<-5 || pdy>5 || pdy<-5)
    {
        for(int i=0;i<nodes.size();i++)
        {
            QPointF pos=nodes.at(i)->pos();
            nodes.at(i)->setPos(pos.x()+pdx,pos.y()+pdy);
        }
        dXMouse=event->x();
        dYMouse=event->y();
    }
}
#endif

void VisualizeWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
    //в принципе перерисовывать действительно легче тут, чем там...по событиям и всему такому.
}
