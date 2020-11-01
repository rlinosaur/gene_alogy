#include "parsegedcom.h"

using namespace gedcom;
ParseGedcom::ParseGedcom()
{    
    stringCount=0;
    humanCount=0;
    unknownCount=0;
}

void ParseGedcom::setError(QString errorText)
{    
    error=errorText;
}


GedcomFamilyTag ParseGedcom::getFamilyTagByName(QString gedcomTagName)
{
    if(gedcomTagName=="CHIL") return tagFamilyChild;
    if(gedcomTagName=="HUSB") return tagFamilyHusband;
    if(gedcomTagName=="WIFE") return tagFamilyWife;
    if(gedcomTagName=="MARR") return tagFamilyMarriageData;
    if(gedcomTagName=="RIN") return tagFamilyRin;
    if(gedcomTagName=="_UID") return tagFamilyUid;    
    return tagFamilyUnknown;
}

GedcomIndividualTag ParseGedcom::getIndividualTagByName(QString gedcomTagName)
{
    if(gedcomTagName=="NAME") return tagIndName;    
    if(gedcomTagName=="SEX") return tagIndSex;
    if(gedcomTagName=="BIRT") return tagIndBirth;
    if(gedcomTagName=="DEAT") return tagIndDeath;
    if(gedcomTagName=="FAMC") return tagIndFamChild;
    if(gedcomTagName=="FAMS") return tagIndFamSpouse;
    if(gedcomTagName=="RIN") return tagIndRin;
    if(gedcomTagName=="_UID") return tagIndUid;
    if(gedcomTagName=="OCCU") return tagIndOccupation;
    if(gedcomTagName=="DSCR") return tagIndDescription;
    if(gedcomTagName=="RESI") return tagIndResidence;
    if(gedcomTagName=="NOTE") return tagIndNote;

    return tagIndUnknown;
}

GedcomMainTag ParseGedcom::getMainTagByName(QString gedcomTagName)
{
    if(gedcomTagName.at(0)=='@') return tagMainIdentity;
    if(gedcomTagName=="HEAD") return tagMainHead;
    return tagMainUnknown;
}

GedcomCommonTag ParseGedcom::getCommonTagByName(QString gedcomTagName)
{
    if(gedcomTagName=="DATE") return tagCommonDate;
    if(gedcomTagName=="PLAC") return tagCommonPlace;
    if(gedcomTagName=="CAUS") return tagCommonCause;
    if(gedcomTagName=="AGE") return tagCommonAge;
    if(gedcomTagName=="ADDR") return tagCommonAddress;
    if(gedcomTagName=="PHON") return tagCommonPhone;
    if(gedcomTagName=="NOTE") return tagCommonNote;
    if(gedcomTagName=="CONC") return tagCommonConc;
    return tagCommonUnknown;
}

GedcomNameTag ParseGedcom::getNameTagByName(QString gedcomTagName)
{
    if(gedcomTagName=="GIVN") return tagGivenName;
    if(gedcomTagName=="SURN") return tagSurName;
    if(gedcomTagName=="_MARNM") return tagMarName;
    return tagUnknownName;

}

gedcomAddressTag ParseGedcom::getAddressTagByName(QString gedcomTagName)
{
       if(gedcomTagName=="ADR1") return tagAddressString1;
       if(gedcomTagName=="CITY") return tagAddressCity;
       if(gedcomTagName=="CTRY") return tagAddressCountry;
       if(gedcomTagName=="POST") return tagAddressPost;
       if(gedcomTagName=="STAE") return tagAddressState;
       return tagAddressUnknown;
}

