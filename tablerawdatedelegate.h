#ifndef TABLERAWDATEDELEGATE_H
#define TABLERAWDATEDELEGATE_H

#include <QStyledItemDelegate>

class TableRawDateDelegate : public QStyledItemDelegate
{
public:
    TableRawDateDelegate(QObject *parent);
    virtual QString displayText(const QVariant &value, const QLocale &locale) const;
};

#endif // TABLERAWDATEDELEGATE_H
