#include <QDebug>

//#include <QRegExp>

#include <QRegularExpression>

#include <math.h>

#include "humandata.h"
/**
 * @brief HumanData::HumanData
 * @param clearUuid Use it when you ge HumanData from somewhere (database etc), it's like an indicator of result existence.
 */
HumanData::HumanData(bool clearUuid)
{    
    if(clearUuid)
        uuid=QUuid();
    else
        uuid=QUuid::createUuid();
    sex=humanSexUndefined;
    fatherListCount=-1;
    motherListCount=-1;
    birthYear=-1;
    deathYear=-1;
    father=QUuid();
    mother=QUuid();
    owner=QUuid();

    created=0;
    changed=0;
}

void HumanData::clear()
{
    fullName.clear();
    surName.clear();
    givenName.clear();
    patronymicName.clear();
    marName.clear();

    gedcomNumber=-1;
    children.clear();
    partners.clear();
    partnersCounts.clear();

    sex=humanSexUndefined;
    fatherListCount=-1;
    motherListCount=-1;
    birthYear=-1;
    deathYear=-1;

    uuid=QUuid::createUuid();

    father=QUuid();
    mother=QUuid();
    owner=QUuid();

    deathDate.clear();
    deathAge.clear();
    deathCause.clear();
    birthDate.clear();
    birthPlaceId.clear();

    occupation.clear();
    description.clear();
    note.clear();

    descendantsInfo.clear();
    created=0;
    changed=0;//а то ещё выскочит что не то...
}

void HumanData::clearUUID()
{
    uuid=QUuid();
}


QString HumanData::getHumanInfo()
{
    QString humanInfo;
    humanInfo.append(surName);
    if(!marName.isEmpty())
        humanInfo.append(" ("+marName+")");
    humanInfo.append(" "+givenName);
    humanInfo.append(" "+patronymicName);
    QRegularExpression regExpYear("[0-9]{4}");
    //QRegExp regExpYear("[0-9]{4}");
    QRegularExpressionMatch match=regExpYear.match(deathDate);
    QString deathyear;    
    if(match.hasMatch())
        deathyear=match.captured(0);
    else
        deathyear.clear();
    /*
    if(deathDate.contains(regExpYear))
        deathyear=regExpYear.cap(0);
    else
        deathyear.clear();
        */

    QString birthyear;
    match=regExpYear.match(birthDate);

    if(match.hasMatch())
        birthyear=match.captured(0);
    else
        birthyear.clear();
    /*
    if(birthDate.contains(regExpYear))
        birthyear=regExpYear.cap(0);
    else
        birthyear.clear();
        */
    if(!birthyear.isEmpty() || !deathyear.isEmpty())
    {
       humanInfo.append(" "+birthyear+"-"+deathyear);
    }
    //qDebug()<<humanInfo;
    return humanInfo;
}

int HumanData::getLevelPosition()
{
    //а можно считать, между прочим, во время создания
    int levelPosition=0;
    int recSize=recursyList.size();
    for(int i=0;i<recSize;i++)
        if(recursyList[i]==humanSexMale)
            levelPosition+=1<<(recSize-i-1);
    return levelPosition;
}

QString HumanData::getLevelPositionString()
{
    QString str;
    for(int i=0;i<recursyList.size();i++)
        if(recursyList[i]==humanSexMale)
            str.append('1');
        else
            str.append('0');

    return str;
}

bool HumanData::sortHumanLevelLessThan(const HumanData &h1, const HumanData &h2)
{
    return h1.recursyList.size()<h2.recursyList.size();
}

bool HumanData::sortHumanBirthYearLessThan(const HumanData &h1, const HumanData &h2)
{
    //qDebug()<<"sortsortsort!";
    /*
    QRegExp regExpYear("[0-9]{4}");
    int birth1=0;
    if(h1.birthDate.contains(regExpYear))
        birth1=regExpYear.cap(0).toInt();
    else
        birth1=0;
    int birth2=0;
    if(h2.birthDate.contains(regExpYear))
        birth1=regExpYear.cap(0).toInt();
    else
        birth2=0;
        */
    QRegularExpression regExpYear("[0-9]{4}");
    QRegularExpressionMatch match;
    int birth1=0;
    if(h1.birthDate.contains(regExpYear,&match))
        birth1=match.captured(0).toInt();
    else
        birth1=0;
    int birth2=0;
    if(h2.birthDate.contains(regExpYear,&match))
        birth1=match.captured(0).toInt();
    else
        birth2=0;
    return birth1<birth2;
}

bool HumanData::sortHumanNameLessThan(const HumanData &h1, const HumanData &h2)
{
    if(h1.surName!=h2.surName)
        return (h1.surName<h2.surName);
    if(h1.marName!=h2.marName)
        return (h1.marName<h2.marName);
    return (h1.givenName<h2.givenName);
}

FamilyData::FamilyData()
{    
    uuid=QUuid::createUuid();
    husbandListCount=-1;
    wifeListCount-=1;
}

void FamilyData::clear()
{
    uuid=QUuid::createUuid();
    children.clear();
    partners.clear();
    husband=QUuid();
    wife=QUuid();
    marriageDate.clear();
    marriagePlace.clear();
    owner=QUuid();
    note.clear();
    recordId.clear();
    husbandListCount=-1;
    wifeListCount=-1;
}


RecordData::RecordData()
{
    uuid=QUuid::createUuid().toString();
}

void RecordData::clear()
{
    record.clear();
    uuid=QUuid::createUuid().toString();
    date.clear();
    type.clear();
    sourceId.clear();
    location.clear();
    placeId.clear();
    note.clear();    
}


PlaceData::PlaceData()
{
    uuid=QUuid::createUuid().toString();
}

void PlaceData::clear()
{
    uuid=QUuid::createUuid().toString();
    name.clear();
    type.clear();
    anotherNames.clear();
    note.clear();
    coordinates.clear();
    owner.clear();
}

QString PlaceData::getPlaceInfo()
{
    QString placeInfo;
    placeInfo.append(type+" ");
    placeInfo.append(name);
    //qDebug()<<"ANOTHER NAMES COUNT IS "<<anotherNames.count();
    if(!anotherNames.isEmpty())
    {
     //   if(anotherNames[0].isEmpty())break;
        placeInfo.append("("+anotherNames.join(";")+")");
    }
    if(!coordinates.isEmpty())
        placeInfo.append(", "+coordinates);
    return placeInfo;
}

QStringList SourceData::types = QStringList() << "архив" << "текст" << "картинка" << "web-ресурс";

SourceData::SourceData()
{
    uuid=QUuid::createUuid().toString();
}

void SourceData::clear()
{
    uuid=QUuid::createUuid().toString();
    name.clear();
    note.clear();
}

QString SourceData::getSourceName(quint8 sourceType)
{
    if(sourceType<types.size()) return types.at(sourceType);
    return QString("тип не задан");
}


HumanPlaceData::HumanPlaceData()
{
    uuid=QUuid::createUuid().toString();
}

void HumanPlaceData::clear()
{
    uuid=QUuid::createUuid().toString();
    note.clear();
    placeId.clear();
    humanId.clear();
    date.clear();
    type.clear();
    owner.clear();
}


RecordSourceData::RecordSourceData()
{
    uuid=QUuid::createUuid().toString();
}

void RecordSourceData::clear()
{
    uuid=QUuid::createUuid().toString();
    sourceId.clear();
    recordId.clear();
    location.clear();
    note.clear();
}
