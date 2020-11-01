#ifndef HUMANSDATABASECALC_H
#define HUMANSDATABASECALC_H

#include "humansdatabase.h"

class HumansDatabaseCalc
{
public:
    HumansDatabaseCalc();

    static void recursiveRelativesSearch(HumansDatabase *allHumans, QList<HumanData> *list, unsigned int level, unsigned int humanListCount, unsigned int maxLevel=15, bool searchRelatives=false, bool directionIsDown=true);

    static void recursiveAncestrySearch(HumansDatabase *db, QList<HumanData> *list, unsigned int level, unsigned int humanListCount, unsigned int maxLevel=15);

    static QList<HumanData> removeHumanDuplicates(QList<HumanData> humans);
};

#endif // HUMANSDATABASECALC_H
