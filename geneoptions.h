#ifndef GENEOPTIONS_H
#define GENEOPTIONS_H

#include <QFont>
#include <QObject>

#include "humandata.h"

class GeneOptions : public QObject
{
    Q_OBJECT
public:
    explicit GeneOptions(QObject *parent = 0);

    HumanPlaceData lastHumanPlace;
    QFont recordEditFont;
signals:

public slots:

};
//Не хочу передавать его всем классам редактирования/поиска...Или хочу?...Не, не хочу.
extern GeneOptions geneOptions;
#endif // GENEOPTIONS_H
