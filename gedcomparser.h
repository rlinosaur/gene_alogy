#ifndef GEDCOMPARSER_H
#define GEDCOMPARSER_H

#include "parsegedcom.h"
#include "humandata.h"
class GedcomParser
{
public:
    GedcomParser();
    void clear();
    ParseGedcom parseGedcomFile(QString fileName);

    static unsigned int getIndiNumber(QString hm);
    QList <HumanData> humanList;
    QList <FamilyData> familyList;
};

#endif // GEDCOMPARSER_H
