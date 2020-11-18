#ifndef HUMANDATA_H
#define HUMANDATA_H

#include <QList>
#include <QUuid>
#include <QStringList>

typedef int humanIdType;

enum HumanSex
{
    humanSexFemale=0,
    humanSexMale=1,
    humanSexUndefined=2
};

class HumanData
{
public:
    HumanData(bool clearUuid=false);
    void clear();
    void clearUUID();
    QString getHumanInfo();

    int getLevelPosition();
    QString getLevelPositionString();

    //Sorting function
    static bool sortHumanLevelLessThan(const HumanData &h1,const HumanData &h2);
    static bool sortHumanBirthYearLessThan(const HumanData &h1,const HumanData &h2);
    static bool sortHumanNameLessThan(const HumanData &h1,const HumanData &h2);

    QUuid uuid;//Идентификатор, собственно.

    //Имена
    QString fullName;//Зачем - не знаю, просто такк.    
    QString givenName;
    QString surName;
    QString patronymicName;//Отчество, если что.
    QString marName;//Девичья фамилия (или первая фамилия)

    //Связи и родственники
    humanIdType gedcomNumber;//Номер в файле gedcom, по крайней мере так в myheritage, надо будет сравнить с тем, что выдаёт familyTreeDna.


    HumanSex sex;    
    QUuid father;
    QUuid mother;
    QUuid owner;//Кто добавил.
    //Разное
    QString occupation;
    QString description;//Not a note! o_O
    QStringList note;



    //Даты
    QString deathDate;
    QString deathAge;
    QString deathCause;
    QString birthDate;
    QString birthPlaceId;
    unsigned int created;//когда создали. иногда пригождается.
    unsigned int changed;

    //GEDCOM import/export variables
    QList<humanIdType> children;
    QList<humanIdType> partners;
    QList<int>partnersCounts;
    int motherListCount;
    int fatherListCount;
    int birthYear;
    int deathYear;

    QList<QStringList> occupations;//для хранения информации о нескольких местах (место + дата).


    //Recursive search variables
    QList<HumanSex> recursyList;//да, из такого выжимать данные будет полегче...но всё же может быть unsigned char?
    QStringList descendantsInfo;//Информация о потомках. Пока просто список с ФИО.

};


class FamilyData
{
public:
    FamilyData();
    void clear();
    QUuid uuid;
    QList<humanIdType> children;
    //QList<humanId> husband;//список на случай, если там нет мужа или жены...как-то так.
    //QList<humanId> wife;
    QList<humanIdType> partners;
    QList<int>partnersCounts;
    QUuid husband;
    QUuid wife;
    QString marriageDate;
    QString marriagePlace;
    QUuid owner;//Кто добавил
    QStringList note;
    QString recordId;//Идентификатор записи, в которой найдена информация.
    //GEDCOM export data
    int husbandListCount;
    int wifeListCount;
};

struct RecordData
{
    RecordData();
    void clear();

    QString record;//Сама по себе запись.
    QString uuid;
    QString date;
    QString type;//Не используется пока.
    QString sourceId;
    QString location;//где точно..типа: Основание.
    QString placeId;
    QString note;//заметка.
    QString owner;    
};

struct PlaceData
{
    PlaceData();
    void clear();
    QString getPlaceInfo();

    QString uuid;
    QString name;
    QString type;
    QStringList anotherNames;
    QString note;
    QString coordinates;//В свободной форме пока, я думаю это ок.
    QString owner;
    QString latitude;
    QString longitude;
    QString coordAccuracy;
};

struct SourceData
{
    SourceData();
    void clear();
    QString uuid;
    QString name;
    qint8 type;
    QString link;
    QString note;
    static QString getSourceName(quint8 sourceType);
    static QStringList getTypes(){return types;}
private:
    static QStringList types;
};

struct HumanPlaceData
{
    HumanPlaceData();
    void clear();

    QString uuid;
    QString placeId;
    QString humanId;
    QString note;
    QString date;
    QString type;
    QString owner;
};


struct RecordSourceData
{
    RecordSourceData();
    void clear();

    QString uuid;
    QString recordId;
    QString sourceId;
    QString location;
    QString note;
};

#endif // HUMANDATA_H
