#ifndef HUMANSDATABASE_H
#define HUMANSDATABASE_H

#include <QUuid>
#include <QObject>
#include <QString>
#include <QSqlDatabase>

#include "defines.h"
#include "humandata.h"

#define DEFAULT_DATABASE_NAME "sample_genebase.db"


#define COMMONSEARCHFLAG_NOPLACES   0b0000000000000001
#define COMMONSEARCHFLAG_NOHUMANS   0b0000000000000010
#define COMMONSEARCHFLAG_NORECORDS  0b0000000000000100
#define COMMONSEARCHFLAG_COUNTQUERY 0b0000000000001000




class HumansDatabase : public QObject
{
    Q_OBJECT
public:
    explicit HumansDatabase(QObject *parent = 0);
    void init(QString fileName=DEFAULT_DATABASE_NAME);
    bool isOpen();
    bool isConnected();//Для MySQL актуально, в случае потери соединения, но и тут пригодится.
    bool close();
    bool deleteDatabase(QString fileName);

    bool createTables();
    bool createIndices();

    bool isTableExists(QString tableName);

    bool clearTable(QString tableName);
    bool transactionStart();
    bool transactionEnd();
    QSqlDatabase getDb(){return db;}

    //Работа непосредственно с БД
    bool addHuman(HumanData human);//Добавить человека в базу.
    bool editHuman(HumanData human);
    HumanData getHuman(QString uuid);
    bool deleteHuman(QString humanId);
    bool uniteHumans(QString fromId, QString toId);
    QList<HumanData> getSiblings(QString uuid, QString fatherId, QString motherId);
    QList<HumanData> getSpouses(QString uuid);
    QList<HumanData> getChildren(QString uuid);
    bool addMarriage(FamilyData family);//Добавить сведения о браке в базу.
    bool editMarriage(FamilyData family);
    bool deleteMarriage(QString uuid);
    bool deleteMarriageBySpouses(QString spouse1,QString spouse2);
    FamilyData getMarriage(QString uuid);
    FamilyData getMarriageBySpouses(QString spouse1,QString spouse2);
    bool addRecord(RecordData record);
    bool editRecord(RecordData record);
    RecordData getRecord(QString uuid);
    bool deleteRecord(QString uuid);
    bool addPlace(PlaceData place);
    bool editPlace(PlaceData place);
    PlaceData getPlace(QString uuid);
    bool deletePlace(QString uuid);
    bool unitePlaces(QString fromId, QString toId);
    bool addSource(SourceData source);
    bool editSource(SourceData source);
    SourceData getSource(QString uuid);
    bool deleteSource(QString uuid);
    QList<QStringList> getSources(QString searchString, bool searchForType=false, qint8 sourceType=0);

    bool addRecordSource(RecordSourceData recordSource);
    bool editRecordSource(RecordSourceData recordSource);
    RecordSourceData getRecordSource(QString uuid);
    bool deleteRecordSource(QString uuid);

    bool addHumanPlace(HumanPlaceData humanPlace);
    bool editHumanPlace(HumanPlaceData humanPlace);
    bool deleteHumanPlace(QString uuid);
    HumanPlaceData getHumanPlace(QString humanPlaceId);    

    bool addParent(QString childId,QString parentId, HumanSex parentSex);

    void cleanDatabase();//Почистить от ошибочных записей. Пока не знаю даже точно, что туда добавлять.


    bool addHumanToRecord(QString recordId,QString humanId);
    bool addPlaceToRecord(QString recordId, QString placeId);
    bool deleteHumanFromRecord(QString recordId,QString humanId);
    bool deletePlaceFromRecord(QString recordId,QString placeId);
    bool deleteSourceFromRecord(QString uuid);

    QList<QStringList> getChildrenInfo(QString humanId);//получить всех детей (uid) и их описания.
    QList<QStringList> getSpousesInfo(QString humanId, HumanSex sex);//получить всех супругов(uid) и описания.
    QList<QStringList> getHumanPlacesInfo(QString humanId);
    QList<QStringList> getHumanPlacesInfoForExport(QString humanId);
    QList<QStringList> getHumansOfRecordInfo(QString recordId);
    QList<QStringList> getPlacesOfRecordInfo(QString recordId);
    QList<QStringList> getSourcesOfRecordInfo(QString recordId);
    QList<QStringList> getHumanSiblingsInfo(QString fatherId,QString motherId,QString humanId="");

    QVariant getHumanSearchQueryCount(QString searchString, QVariant searchSex,QString searchPlaceId="",unsigned int flags=0);
    QVariant getRecordSearchQueryCount(QString searchString,QString searchHumanId="",QString searchPlaceId="",unsigned int flags=0);
    QVariant getPlaceSearchQueryCount(QString searchString, unsigned int flags=0);

    static QUuid getUuidFromString(QString uuidString);
    static QString getHumanSearchQuery(QString searchString, QVariant searchSex,QString searchPlaceId="", unsigned int flags=0);
    static QString getRecordSearchQuery(QString searchString,QString searchHumanId="",QString searchPlaceId="",unsigned int flags=0);
    static QString getPlaceSearchQuery(QString searchString, unsigned int flags=0);

    static QString getHumanInfoFromRecord(QSqlRecord record);
    static QString getHumanPlaceInfoFromRecord(QSqlRecord record);
    static QString getPlaceInfoFromRecord(QSqlRecord record);
    static QString getSourceInfoFromRecord(QSqlRecord record);
    static HumanData getHumanDataFromRecord(QSqlRecord record);

private:
    QSqlDatabase db;
    QString fName;
    void mess(QString message);
signals:
    void sendMessage(QString message, MessageType messtype);
public slots:
};

#endif // HUMANSDATABASE_H
