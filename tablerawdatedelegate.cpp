

#include <QLabel>
#include <QDebug>
#include <QComboBox>
#include <QDateTime>
#include <QItemDelegate>

#include "tablerawdatedelegate.h"

TableRawDateDelegate::TableRawDateDelegate(QObject *parent = 0) :
    QStyledItemDelegate(parent)
{
    qDebug()<<"Create delegate";
}
QString TableRawDateDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    Q_UNUSED(locale);
    QDateTime t;
    t.setTime_t(value.toInt());
    return t.toString("yyyy.MM.dd hh::mm:ss");
}


