#include <QFile>
#include <QUuid>
#include <QDebug>
#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>
#include <QSqlDatabase>
#include <QRegularExpression>
#include "humansdatabase.h"

//QRegularExpression regExpYear("[0-9]{4}");
QRegExp regExpYear("[0-9]{4}");
QRegExp regNonHex("[^0-9A-F]");

HumansDatabase::HumansDatabase(QObject *parent) : QObject(parent)
{

}

void HumansDatabase::init(QString fileName)
{
    QStringList drivers = QSqlDatabase::drivers();
    mess("Database initialization...");
    mess("Available drivers list:"+drivers.join(","));
    db = QSqlDatabase::addDatabase("QSQLITE", QUuid::createUuid().toRfc4122().toHex());
    db.setDatabaseName(fileName);
    db.setHostName("localhost");

    if(db.open())
    {
        fName=fileName;
    }
    else
    {
        mess("Last error"+db.lastError().databaseText()+",driver:"+db.lastError().driverText());
    }

    //Патч базы данных, если необходимо
    if(db.isOpen())
    {
        patchDatabase();
    }
}

bool HumansDatabase::isOpen() const
{
    return db.isOpen();//Зачем повторял? Не знаю.
}

bool HumansDatabase::isConnected()
{
    if(!db.isOpen()) return false;
    QSqlQuery q(db);
    q.exec("select 1;");
    if(q.first()) return true;
    return false;

}

bool HumansDatabase::close()
{
    db.close();
    return true;
}


/**
 * @brief HumansDatabase::deleteDatabase Deleting database
 * @return
 */
bool HumansDatabase::deleteDatabase(QString fileName="")
{
    if(this->isOpen())
    {
        this->close();
        return QFile::remove(fName);
    }
    //Практически просто удаление файла, будь осторожен с этим.
    return QFile::remove(fileName);
}

bool HumansDatabase::createTables()
{
    if(!this->isOpen())return false;

    //Simpliest table structure existense check.
    if(isTableExists("records")) return false;//Could be temporary disabled sometimes.
    QSqlQuery q(db);    
    //Таблица человеков. Самая главная, собственно.
    q.prepare("CREATE TABLE IF NOT EXISTS humans (id CHAR(38) PRIMARY KEY NOT NULL, name TEXT, patronymic TEXT, surname TEXT, marname TEXT, sex INTEGER, father CHAR(38), mother CHAR(38), birthdate TEXT, birthplace CHAR(38), deathdate TEXT, deathcause TEXT, occupation TEXT, deathyear INTEGER,birthyear INTEGER,note TEXT, created INTEGER, changed INTEGER, owner CHAR(38));");
    q.exec();
    //Таблица записей. Откуда, собственно, берутся генеалогические данные.
    q.prepare("CREATE TABLE IF NOT EXISTS records (id CHAR(38) PRIMARY KEY NOT NULL, record TEXT, date TEXT, year INTEGER, type TEXT, sourceid CHAR(38), location TEXT, placeid CHAR(38), note TEXT, created INTEGER, changed INTEGER, owner CHAR(38));");
    q.exec();

    //Таблица соответствия записей и человеков
    q.prepare("CREATE TABLE IF NOT EXISTS recordhumans (id CHAR(38) PRIMARY KEY NOT NULL, recordid CHAR(38), humanid CHAR(38), unique (recordid,humanid));");
    q.exec();

    //Таблица соответствия записей и мест
    q.prepare("CREATE TABLE IF NOT EXISTS recordplaces (id CHAR(38) PRIMARY KEY NOT NULL, recordid CHAR(38), placeid CHAR(38),unique (recordid,placeid));");
    q.exec();

    q.prepare("CREATE TABLE IF NOT EXISTS recordsources (id CHAR(38) PRIMARY KEY NOT NULL, recordid CHAR(38), sourceid CHAR(38), location TEXT, note TEXT, UNIQUE (recordid, sourceid))");
    q.exec();//С уникальностью я может и переборщил, но пусть будет пока.

    //Таблица мест, которые упоминаются в записях (полезно, между прочим).    
    q.prepare("CREATE TABLE IF NOT EXISTS places (id CHAR(38) PRIMARY KEY NOT NULL, name TEXT, type TEXT, anothernames TEXT, lan REAL, lot REAL,coordacc INTEGER, note TEXT, owner CHAR(38));");
    q.exec();

    //Таблица связи людей с местами
    q.prepare("CREATE TABLE IF NOT EXISTS humanplaces (id CHAR(38) PRIMARY KEY NOT NULL, humanid CHAR(38), placeid CHAR(38), date TEXT, type INTEGER, note TEXT, owner CHAR(38));");
    q.exec();

    //Таблица браков между людьми
    q.prepare("CREATE TABLE IF NOT EXISTS marriages(id CHAR(38) PRIMARY KEY NOT NULL, husband CHAR(38), wife CHAR(38), date TEXT, type INTEGER, placeid CHAR(38),recordid CHAR(38), note TEXT, owner CHAR(38));");
    q.exec();

    q.prepare("CREATE TABLE IF NOT EXISTS sources (id CHAR(38) PRIMARY KEY NOT NULL, name TEXT, type INTEGER, link TEXT, note TEXT);");
    q.exec();

    //Вроде бы ничего больше знать о них не надо, так ведь?
    //Инфу по датам, если что, уберу из дат..хотя...год-то добавлю человеку, добавлю.
    return true;
}

bool HumansDatabase::createIndices()
{
    if(!this->isOpen())return false;
    //Simpliest table structure existense check.
    if(isTableExists("records")) return false;
    QSqlQuery q(db);

    q.exec("CREATE INDEX humans_father_index ON humans(father);");
    q.exec("CREATE INDEX humans_mother_index ON humans(mother);");
    q.exec("CREATE INDEX humans_birthplace_index ON humans(birthplace);");

    q.exec("CREATE INDEX records_place_index ON records(placeid);");

    q.exec("CREATE INDEX humanplaces_humanid_index ON humanplaces(humanid);");
    q.exec("CREATE INDEX humanplaces_placeid_index ON humanplaces(placeid);");

    q.exec("CREATE INDEX marriages_husband_index ON marriages(husband);");
    q.exec("CREATE INDEX marriages_wife_index ON marriages(wife);");

    return true;
}

bool HumansDatabase::isTableExists(QString tableName)
{
    QSqlQuery q(db);
    q.prepare("SELECT COUNT(*) from sqlite_master WHERE type='table' AND name='"+tableName+"';");
    q.exec();
    if(!q.first()) return false;
    return q.record().value(0).toBool();
}

bool HumansDatabase::isFieldExists(QString tableName, QString fieldName)
{
    QSqlQuery q(db);
    bool res=q.exec("PRAGMA table_info('"+tableName+"')");
    if(!res)
    {
        mess("isFieldExists error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
        return false;
    }
    if(!q.first()) return false;
    do
    {
        QSqlRecord rec=q.record();
        if(rec.value(rec.indexOf("name")).toString()==fieldName) return true;
    }while(q.next());
    return false;
}

bool HumansDatabase::clearTable(QString tableName)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);
    q.prepare("delete from "+tableName+";");
    return q.exec();
}

bool HumansDatabase::transactionStart()
{
    if(!this->isOpen())return false;
    return db.transaction();
}

bool HumansDatabase::transactionEnd()
{
    if(!this->isOpen())return false;
    return db.commit();
}

bool HumansDatabase::addHuman(HumanData human)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);
    q.prepare ("INSERT OR IGNORE INTO humans (id,name,patronymic,surname,marname,sex,father,mother,birthdate,birthplace,deathdate,deathcause,occupation,deathyear,birthyear,note,created,changed,owner) VALUES(:id,:name,:patronymic,:surname,:marname,:sex,:father,:mother,:birthdate,:birthplace,:deathdate,:deathcause,:occupation,:deathyear,:birthyear,:note,:created,:changed,:owner);");
    q.bindValue(":id",human.uuid.toString());
    q.bindValue(":name",human.givenName);
    q.bindValue(":patronymic",human.patronymicName);
    q.bindValue(":surname",human.surName);
    q.bindValue(":marname",human.marName);
    q.bindValue(":sex",human.sex);
    if(!human.father.isNull())
        q.bindValue(":father",human.father.toString());
    else
        q.bindValue(":father",QVariant(QVariant::String));
    if(!human.mother.isNull())
        q.bindValue(":mother",human.mother.toString());
    else
        q.bindValue(":mother",QVariant(QVariant::String));
    q.bindValue(":birthdate",human.birthDate);
    q.bindValue(":birthplace",human.birthPlaceId);
    q.bindValue(":deathdate",human.deathDate);
    q.bindValue(":deathcause",human.deathCause);
    q.bindValue(":occupation",human.occupation);

    if(human.deathDate.contains(regExpYear))
        q.bindValue(":deathyear",regExpYear.cap(0).toInt());
    else
        q.bindValue(":deathyear",QVariant(QVariant::Int));

    if(human.birthDate.contains(regExpYear))
        q.bindValue(":birthyear",regExpYear.cap(0).toInt());
    else
        q.bindValue(":birthyear",QVariant(QVariant::Int));

    q.bindValue(":note",human.note.join(";"));
    q.bindValue(":created",QDateTime::currentMSecsSinceEpoch()/1000);
    q.bindValue(":changed",QDateTime::currentMSecsSinceEpoch()/1000);
    if(!human.owner.isNull())
       q.bindValue(":owner",human.owner.toString());
    else
        q.bindValue(":owner",QVariant(QVariant::String));

    bool res=q.exec();
    if(!res)
        mess("Add human error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
    return res;
}

bool HumansDatabase::editHuman(HumanData human)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);
    //q.prepare ("INSERT OR IGNORE INTO humans (id,name,patronymic,surname,marname,sex,father,mother,birthdate,birthplace,deathdate,deathcause,occupation,deathyear,birthyear,note,created,owner) values(:id,:name,:patronymic,:surname,:marname,:sex,:father,:mother,:birthdate,:birthplace,:deathdate,:deathcause,:occupation,:deathyear,:birthyear,:note,:created,:owner);");
    q.prepare ("UPDATE OR IGNORE humans SET name=:name,patronymic=:patronymic,surname=:surname,marname=:marname,sex=:sex,father=:father,mother=:mother,birthdate=:birthdate,birthplace=:birthplace,deathdate=:deathdate,deathcause=:deathcause,occupation=:occupation,deathyear=:deathyear,birthyear=:birthyear,note=:note,changed=:changed WHERE id=:id;");
    q.bindValue(":id",human.uuid.toString());
    q.bindValue(":name",human.givenName);
    q.bindValue(":patronymic",human.patronymicName);
    q.bindValue(":surname",human.surName);
    q.bindValue(":marname",human.marName);
    q.bindValue(":sex",human.sex);
    if(!human.father.isNull())
        q.bindValue(":father",human.father.toString());
    else
        q.bindValue(":father",QVariant(QVariant::String));
    if(!human.mother.isNull())
        q.bindValue(":mother",human.mother.toString());
    else
        q.bindValue(":mother",QVariant(QVariant::String));
    q.bindValue(":birthdate",human.birthDate);
    q.bindValue(":birthplace",human.birthPlaceId);
    q.bindValue(":deathdate",human.deathDate);
    q.bindValue(":deathcause",human.deathCause);
    q.bindValue(":occupation",human.occupation);
    if(human.deathDate.contains(regExpYear))
        q.bindValue(":deathyear",regExpYear.cap(0).toInt());
    else
        q.bindValue(":deathyear",QVariant(QVariant::Int));
    if(human.birthDate.contains(regExpYear))
        q.bindValue(":birthyear",regExpYear.cap(0).toInt());
    else
        q.bindValue(":birthyear",QVariant(QVariant::Int));
    q.bindValue(":note",human.note.join(";"));
    q.bindValue(":changed",QDateTime::currentMSecsSinceEpoch()/1000);
    bool res=q.exec();
    if(!res)
        mess("Edit human error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
    return res;
}

HumanData HumansDatabase::getHuman(QString uuid)
{    
    if(!this->isOpen() || uuid.isEmpty())
    {        
        return HumanData(true);
    }
    QSqlQuery q(db);

    q.prepare("SELECT * FROM humans WHERE id=:id LIMIT 1;");
    q.bindValue(":id",uuid);
    q.exec();
    if(!q.first())
    {
        mess("Get human with uuid "+uuid+" error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());        
        return HumanData(true);
    }
    return this->getHumanDataFromRecord(q.record());
}

bool HumansDatabase::deleteHuman(QString humanId)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);
    q.prepare("DELETE FROM humans where id=:id;");
    q.bindValue(":id",humanId);
    bool res=q.exec();
    if(res)
    {
        this->transactionStart();
        q.prepare("UPDATE humans SET father=NULL WHERE father=:id");
        q.bindValue(":id",humanId);
        q.exec();
        q.prepare("UPDATE humans SET mother=NULL WHERE mother=:id");
        q.bindValue(":id",humanId);
        q.exec();
        q.prepare("DELETE FROM recordhumans WHERE humanid=:id;");
        q.bindValue(":id",humanId);
        q.exec();
        q.prepare("DELETE FROM humanplaces WHERE humanid=:id;");
        q.bindValue(":id",humanId);
        q.exec();
        q.prepare("DELETE FROM marriages WHERE husband=:husbandid OR wife=:wifeid;");
        q.bindValue(":husbandid",humanId);
        q.bindValue(":wifeid",humanId);
        q.exec();
        this->transactionEnd();
    }

    return res;
}

bool HumansDatabase::uniteHumans(QString fromId, QString toId)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);

    transactionStart();
    //всё очень просто, кстати...
    q.prepare("DELETE FROM humans WHERE id=:fromid;");
    q.bindValue(":fromid",fromId);
    bool res=q.exec();
    if(!res) return false;

    q.prepare("UPDATE humans SET father=:toid WHERE father=:fromid;");
    q.bindValue(":toid",toId);
    q.bindValue(":fromid",fromId);
    res=q.exec();
    if(!res)
        mess("Unite humans fathers recordhumans error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());

    q.prepare("UPDATE humans SET mother=:toid WHERE mother=:fromid;");
    q.bindValue(":toid",toId);
    q.bindValue(":fromid",fromId);
    res=q.exec();
    if(!res)
        mess("Unite humans mothers recordhumans error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());


    q.prepare("UPDATE recordhumans SET humanid=:toid WHERE humanid=:fromid;");
    q.bindValue(":toid",toId);
    q.bindValue(":fromid",fromId);
    res=q.exec();
    if(!res)
        mess("Unite humans recordhumans error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());

    q.prepare("UPDATE humanplaces SET humanid=:toid WHERE humanid=:fromid;");
    q.bindValue(":toid",toId);
    q.bindValue(":fromid",fromId);
    res=q.exec();
    if(!res)
        mess("Unite humans humanplaces error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());

    //Потенциальная проблема..За транзакцию её, может быть? Повторная запись может быть..нехорошо.
    q.prepare("update marriages SET husband=:toid WHERE husband=:fromid;");
    q.bindValue(":toid",toId);
    q.bindValue(":fromid",fromId);
    q.exec();
    if(!res)
        mess("Unite humans marriages error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());

    q.prepare("UPDATE marriages SET wife=:toid WHERE wife=:fromid;");
    q.bindValue(":toid",toId);
    q.bindValue(":fromid",fromId);
    q.exec();
    if(!res)
        mess("Unite humans marriages 2 error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
    transactionEnd();
    return true;
}

QList<HumanData> HumansDatabase::getSiblings(QString uuid, QString fatherId, QString motherId)
{
    QList <HumanData> list;
    if(!this->isOpen() || uuid.isEmpty())
    {
        return list;
    }
    QSqlQuery q(db);

    q.prepare("SELECT * FROM humans WHERE father=:father AND mother=:mother AND id<>:id;");
    q.bindValue(":father",fatherId);
    q.bindValue(":mother",motherId);
    q.bindValue(":id",uuid);
    q.exec();
    if(!q.first())return list;
    do
    {
        list.append(this->getHumanDataFromRecord(q.record()));
    }while(q.next());
    return list;
}

QList<HumanData> HumansDatabase::getSpouses(QString uuid)
{
    QList <HumanData> list;
    if(!this->isOpen() || uuid.isEmpty())
    {
        return list;
    }
    QSqlQuery q(db);

    q.prepare("SELECT * FROM humans WHERE id<>:id AND (id in (select husband from marriages where wife=:wid) OR id in (select wife from marriages where husband=:hid));");
    q.bindValue(":id",uuid);
    q.bindValue(":wid",uuid);
    q.bindValue(":hid",uuid);

    q.exec();
    if(!q.first())return list;
    do
    {
        list.append(this->getHumanDataFromRecord(q.record()));
    }while(q.next());
    return list;
}

QList<HumanData> HumansDatabase::getChildren(QString uuid)
{
    QList <HumanData> list;
    if(!this->isOpen() || uuid.isEmpty())
    {
        return list;
    }
    QSqlQuery q(db);

    q.prepare("SELECT * FROM humans WHERE father=:fid OR mother=:mid;");
    q.bindValue(":fid",uuid);
    q.bindValue(":mid",uuid);
    q.exec();
    if(!q.first())return list;
    do
    {
        list.append(this->getHumanDataFromRecord(q.record()));
    }while(q.next());
    return list;
}

bool HumansDatabase::addMarriage(FamilyData family)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);

    q.prepare ("INSERT OR IGNORE INTO marriages (id,husband,wife,date,placeid,recordid,note,owner) VALUES(:id,:husband,:wife,:date,:placeid,:recordid,:note,:owner);");
    q.bindValue(":id",family.uuid.toString());

    if(!family.husband.isNull())
        q.bindValue(":husband",family.husband.toString());
    else
        q.bindValue(":husband",QVariant(QVariant::String));
    if(!family.wife.isNull())
        q.bindValue(":wife",family.wife.toString());
    else
        q.bindValue(":wife",QVariant(QVariant::String));   
    q.bindValue(":date",family.marriageDate);
    q.bindValue(":placeid",family.marriagePlace);
    q.bindValue(":placeid",family.recordId);
    q.bindValue(":note",family.note.join(";"));

    if(!family.owner.isNull())
       q.bindValue(":owner",family.owner.toString());
    else
        q.bindValue(":owner",QVariant(QVariant::String));
    //q.bindValue(":owner",family.owner.toString());
    bool res=q.exec();
    if(!res)
        mess("Add marriage error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
    return res;
}

bool HumansDatabase::editMarriage(FamilyData family)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);
    q.prepare ("UPDATE OR IGNORE marriages SET husband=:husband,wife=:wife,date=:date,placeid=:placeid,recordid=:recordid,note=:note WHERE id=:id;");
    q.bindValue(":id",family.uuid.toString());
    if(!family.husband.isNull())
        q.bindValue(":husband",family.husband.toString());
    else
        q.bindValue(":husband",QVariant(QVariant::String));
    if(!family.wife.isNull())
        q.bindValue(":wife",family.wife.toString());
    else
        q.bindValue(":wife",QVariant(QVariant::String));   
    q.bindValue(":date",family.marriageDate);
    q.bindValue(":placeid",family.marriagePlace);
    q.bindValue(":recordid",family.recordId);
    q.bindValue(":note",family.note.join(";"));
    bool res=q.exec();
    if(!res)
        mess("Edit marriage error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
    return res;
}

bool HumansDatabase::deleteMarriage(QString uuid)
{
    if(!this->isOpen() || uuid.isEmpty())return false;
    QSqlQuery q(db);
    q.prepare("DELETE FROM marriages WHERE id=:id;");
    q.bindValue(":id",uuid);
    return q.exec();
}

bool HumansDatabase::deleteMarriageBySpouses(QString spouse1, QString spouse2)
{
    if(!this->isOpen()) return false;
    QSqlQuery q(db);
    q.prepare("DELETE FROM marriages WHERE (husband=:h1 AND wife=:w1) OR (husband=:h2 AND wife=:w2);");
    q.bindValue(":h1",spouse1);
    q.bindValue(":w1",spouse2);
    q.bindValue(":h2",spouse2);
    q.bindValue(":w2",spouse1);
    return q.exec();
}

FamilyData HumansDatabase::getMarriage(QString uuid)
{
    FamilyData familyData;
    if(!this->isOpen() || uuid.isEmpty())
    {        
        return familyData;
    }
    QSqlQuery q(db);  
    q.prepare("SELECT id,husband,wife,date,placeid,note,recordid FROM marriages WHERE id=:id limit 1;");
    q.bindValue(":id",uuid);
    q.exec();
    if(!q.first())
    {
        mess("Get marriage info error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());        
        return familyData;
    }
    QSqlRecord rec=q.record();

    familyData.uuid=rec.value(rec.indexOf("id")).toString();

    QString husband=rec.value(rec.indexOf("husband")).toString();
    if(husband.isEmpty())familyData.husband=NULL;
    else
        familyData.husband=HumansDatabase::getUuidFromString(husband);
    QString wife=rec.value(rec.indexOf("wife")).toString();
    if(wife.isEmpty())familyData.wife=NULL;
    else
        familyData.wife=HumansDatabase::getUuidFromString(wife);

    familyData.marriageDate=rec.value(rec.indexOf("date")).toString();
    familyData.marriagePlace=rec.value(rec.indexOf("placeid")).toString();
    familyData.note=rec.value(rec.indexOf("note")).toString().split(";");
    familyData.recordId=rec.value(rec.indexOf("recordid")).toString();
    return familyData;
}

FamilyData HumansDatabase::getMarriageBySpouses(QString spouse1, QString spouse2)
{
    FamilyData familyData;
    if(!this->isOpen() || spouse1.isEmpty() || spouse2.isEmpty())
    {        
        return familyData;
    }
    QSqlQuery q(db);  
    q.prepare("SELECT id,husband,wife,date,placeid,note,recordid FROM marriages WHERE (husband=:h1 AND wife=:w1) OR (husband=:h2 OR wife=:w2) limit 1;");
    q.bindValue(":h1",spouse1);
    q.bindValue(":w1",spouse2);
    q.bindValue(":h2",spouse2);
    q.bindValue(":w2",spouse1);
    q.exec();
    if(!q.first())
    {
        mess("Get marriage info error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());      
        return familyData;
    }
    QSqlRecord rec=q.record();

    familyData.uuid=rec.value(rec.indexOf("id")).toString();
    //familyData.uuid.fromString(rec.value(rec.indexOf("id")).toString());

    QString husband=rec.value(rec.indexOf("husband")).toString();
    if(husband.isEmpty())familyData.husband=QUuid();
    else
        familyData.husband=HumansDatabase::getUuidFromString(husband);
    QString wife=rec.value(rec.indexOf("wife")).toString();
    if(wife.isEmpty())familyData.wife=QUuid();
    else
        familyData.wife=HumansDatabase::getUuidFromString(wife);

    familyData.marriageDate=rec.value(rec.indexOf("date")).toString();
    familyData.marriagePlace=rec.value(rec.indexOf("placeid")).toString();
    familyData.note=rec.value(rec.indexOf("note")).toString().split(";");
    familyData.recordId=rec.value(rec.indexOf("recordid")).toString();
    return familyData;
}

bool HumansDatabase::addRecord(RecordData record)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);

    q.prepare ("INSERT OR IGNORE INTO records (id,record,date,year,type,sourceid,location,placeid,note,created,changed,owner) VALUES(:id,:record,:date,:year,:type,:sourceid,:location,:placeid,:note,:created,:changed,:owner);");
    q.bindValue(":id",record.uuid);
    q.bindValue(":record",record.record);
    q.bindValue(":date",record.date);

    if(record.date.contains(regExpYear))
        q.bindValue(":year",regExpYear.cap(0).toInt());
    else
        q.bindValue(":year",QVariant(QVariant::Int));

    q.bindValue(":type",record.type);
    q.bindValue(":sourceid",record.sourceId);
    q.bindValue(":location",record.location);
    q.bindValue(":placeid",record.placeId);
    q.bindValue(":note",record.note);
    q.bindValue(":created",QDateTime::currentMSecsSinceEpoch()/1000);
    q.bindValue(":changed",QDateTime::currentMSecsSinceEpoch()/1000);
    if(!record.owner.isEmpty())
       q.bindValue(":owner",record.owner);
    else
        q.bindValue(":owner",QVariant(QVariant::String));
    bool res=q.exec();
    if(!res)
        mess("Add record error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
    return res;
}


bool HumansDatabase::editRecord(RecordData record)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);    
    q.prepare("UPDATE OR IGNORE records set record=:record,date=:date,year=:year,type=:type,sourceid=:sourceid,location=:location,placeid=:placeid,note=:note,changed=:changed WHERE id=:id;");
    q.bindValue(":id",record.uuid);
    q.bindValue(":record",record.record);
    q.bindValue(":date",record.date);

    if(record.date.contains(regExpYear))
        q.bindValue(":year",regExpYear.cap(0).toInt());
    else
        q.bindValue(":year",QVariant(QVariant::Int));

    q.bindValue(":type",record.type);
    q.bindValue(":sourceid",record.sourceId);
    q.bindValue(":location",record.location);
    q.bindValue(":placeid",record.placeId);
    q.bindValue(":note",record.note);
    q.bindValue(":changed",QDateTime::currentMSecsSinceEpoch()/1000);
    bool res=q.exec();
    if(!res)
        mess("Edit record error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
    return res;
}

RecordData HumansDatabase::getRecord(QString uuid)
{
    RecordData recordData;
    if(!this->isOpen() || uuid.isEmpty())
    {        
        return recordData;
    }
    QSqlQuery q(db);

    q.prepare("SELECT * FROM records WHERE id=:id LIMIT 1;");
    q.bindValue(":id",uuid);
    q.exec();
    if(!q.first())
    {
        mess("Get record error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());        
        return recordData;
    }
    QSqlRecord rec=q.record();    
    recordData.uuid=rec.value(rec.indexOf("id")).toString();
    recordData.record=rec.value(rec.indexOf("record")).toString();
    recordData.date=rec.value(rec.indexOf("date")).toString();
    recordData.type=rec.value(rec.indexOf("type")).toString();
    recordData.sourceId=rec.value(rec.indexOf("sourceid")).toString();
    recordData.location=rec.value(rec.indexOf("location")).toString();
    recordData.placeId=rec.value(rec.indexOf("placeid")).toString();
    recordData.note=rec.value(rec.indexOf("note")).toString();
    return recordData;
}

bool HumansDatabase::deleteRecord(QString uuid)
{
    if(!this->isOpen() || uuid.isEmpty())return false;
    QSqlQuery q(db);
    q.prepare("DELETE FROM records WHERE id=:id;");
    q.bindValue(":id",uuid);

    bool res=q.exec();
    if(res)
    {//можно было бы на триггер повесить, но неохота.
        this->transactionStart();
        q.prepare("DELETE FROM recordhumans WHERE recordid=:id");
        q.bindValue(":id",uuid);
        q.exec();
        q.prepare("DELETE FROM recordplaces WHERE recordid=:id");
        q.bindValue(":id",uuid);
        q.exec();
        q.prepare("DELETE FROM marriages WHERE recordid=:id");
        q.bindValue(":id",uuid);
        q.exec();
        this->transactionEnd();
    }

    return res;
}

bool HumansDatabase::addPlace(PlaceData place)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);
    q.prepare ("INSERT OR IGNORE INTO places (id,name,type,anothernames,lat,lon,coordacc,note,owner) VALUES(:id,:name,:type,:anothernames,:lat,:lon,:coordacc,:note,:owner);");
    q.bindValue(":id",place.uuid);
    q.bindValue(":name",place.name);
    q.bindValue(":type",place.type);
    q.bindValue(":anothernames",place.anotherNames.join(";"));
    q.bindValue(":lat",place.latitude);
    q.bindValue(":lon",place.longitude);
    q.bindValue(":coordacc",place.coordAccuracy);
    q.bindValue(":note",place.note);
    q.bindValue(":owner",place.owner);
    bool res=q.exec();
    if(!res)
        mess("Add place error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
    return res;
}

bool HumansDatabase::editPlace(PlaceData place)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);
    q.prepare("UPDATE OR IGNORE places set name=:name,type=:type,anothernames=:anothernames,lat=:lat,lon=:lon, coordacc=:coordacc, note=:note,owner=:owner WHERE id=:id;");
    q.bindValue(":id",place.uuid);
    q.bindValue(":name",place.name);
    q.bindValue(":type",place.type);
    q.bindValue(":anothernames",place.anotherNames.join(";"));
    q.bindValue(":lat",place.latitude);
    q.bindValue(":lon",place.longitude);
    q.bindValue(":coordacc",place.coordAccuracy);
    q.bindValue(":note",place.note);
    q.bindValue(":owner",place.owner);
    bool res=q.exec();
    if(!res)
        mess("Edit place error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
    return res;
}

PlaceData HumansDatabase::getPlace(QString uuid) const
{
    PlaceData placeData;
    if(!this->isOpen() || uuid.isEmpty())
    {
        return placeData;
    }
    QSqlQuery q(db);
    //qDebug()<<"Getting place for "<<uuid;
    q.prepare("SELECT id,name,type,anothernames,lat,lon,coordacc,note,owner FROM places WHERE id=:id LIMIT 1;");
    q.bindValue(":id",uuid);
    //qDebug()<<"Query validation:"<<q.isValid();
    q.exec();
    if(!q.first())
    {
        mess("Get place error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
        //Потому что места может и не быть, например. Так-то.
        return placeData;
    }

    QSqlRecord rec=q.record();
    return getPlaceDataFromRecord(rec);
}

QList<PlaceData> HumansDatabase::getPlaces() const
{

    QList<PlaceData> list;
    if(!this->isOpen())return list;
    QSqlQuery q(db);
    q.prepare("SELECT id,name,type,anothernames,lat,lon,coordacc FROM places;");
    bool res=q.exec();
    if(!res)
    {
        mess("getPlaces error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
        return list;
    }
    if(!q.first()) return list;
    do
    {
        QSqlRecord rec=q.record();
        list.append(getPlaceDataFromRecord(rec));
    }while(q.next());
    return list;
}

bool HumansDatabase::deletePlace(QString uuid)
{
    if(!this->isOpen() || uuid.isEmpty()) return false;
    QSqlQuery q(db);
    q.prepare("delete from places where id=:id;");
    q.bindValue(":id",uuid);

    bool res=q.exec();
    if(res)
    {//можно было бы на триггер повесить, но неохота.
        this->transactionStart();
        q.prepare("UPDATE humans SET birthplace=NULL WHERE birthplace=:id;");
        q.bindValue(":id",uuid);
        q.exec();

        q.prepare("UPDATE record SET placeid=NULL WHERE placeid=:id;");
        q.bindValue(":id",uuid);
        q.exec();
        q.prepare("DELETE FROM recordplaces WHERE placeid=:id");
        q.bindValue(":id",uuid);
        q.exec();
        q.prepare("DELETE FROM humanplaces WHERE placeid=:id");
        q.bindValue(":id",uuid);
        q.exec();
        this->transactionEnd();
    }
    return res;
}

bool HumansDatabase::unitePlaces(QString fromId, QString toId)
{
    //Требуется проверка, да доработка...
    //хотя можно и сейчас организовать, чего уж.

    if(!this->isOpen())return false;
    QSqlQuery q(db);

    transactionStart();
    //всё очень просто, кстати...
    q.prepare("DELETE FROM places WHERE id=:fromId;");
    q.bindValue(":fromId",fromId);
    bool res=q.exec();
    if(!res)
        mess("Unite places fail error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());

    if(!res) return false;


    q.prepare("UPDATE humans SET birthplace=:toid WHERE birthplace=:fromid;");
    q.bindValue(":toid",toId);
    q.bindValue(":fromid",fromId);
    res=q.exec();
    if(!res)
        mess("Unite places humans birthplace error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());

    q.prepare("UPDATE records SET placeid=:toid WHERE placeid=:fromid;");
    q.bindValue(":toid",toId);
    q.bindValue(":fromid",fromId);
    res=q.exec();
    if(!res)
        mess("Unite places records place error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());


    q.prepare("UPDATE recordplaces SET placeid=:toid WHERE placeid=:fromid;");
    q.bindValue(":toid",toId);
    q.bindValue(":fromid",fromId);
    res=q.exec();
    if(!res)
        mess("Unite places records placessss error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());

    q.prepare("UPDATE humanplaces SET placeid=:toid WHERE placeid=:fromid;");
    q.bindValue(":toid",toId);
    q.bindValue(":fromid",fromId);
    res=q.exec();
    if(!res)
        mess("Unite places humans in places error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());

    q.prepare("UPDATE marriages SET placeid=:toid WHERE placeid=:fromid;");
    q.bindValue(":toid",toId);
    q.bindValue(":fromid",fromId);
    res=q.exec();
    if(!res)
        mess("Unite places marriages in places error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
    transactionEnd();
    return true;



}

bool HumansDatabase::addSource(SourceData source)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);
    q.prepare ("INSERT OR IGNORE INTO sources (id,name,type,link,note) VALUES(:id,:name,:type,:link,:note);");
    q.bindValue(":id",source.uuid);
    q.bindValue(":name",source.name);
    q.bindValue(":type",source.type);    
    q.bindValue(":link",source.link);
    q.bindValue(":note",source.note);
    bool res=q.exec();
    if(!res)
        mess("Add source error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
    return res;
}

bool HumansDatabase::editSource(SourceData source)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);
    q.prepare("UPDATE OR IGNORE sources SET name=:name,type=:type, link=:link, note=:note  WHERE id=:id;");
    q.bindValue(":id",source.uuid);
    q.bindValue(":name",source.name);
    q.bindValue(":type",source.type);
    q.bindValue(":link",source.link);
    q.bindValue(":note",source.note);
    bool res=q.exec();
    if(!res)
        mess("Edit place error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
    return res;
}

SourceData HumansDatabase::getSource(QString uuid)
{
    SourceData sourceData;
    if(!this->isOpen() || uuid.isEmpty())return sourceData;
    QSqlQuery q(db);

    q.prepare("SELECT * FROM sources WHERE id=:id LIMIT 1;");
    q.bindValue(":id",uuid);
    q.exec();
    if(!q.first())
    {
        mess("Get source error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());

        return sourceData;
    }
    QSqlRecord rec=q.record();

    sourceData.uuid=rec.value(rec.indexOf("id")).toString();
    sourceData.name=rec.value(rec.indexOf("name")).toString();
    sourceData.note=rec.value(rec.indexOf("note")).toString();    
    sourceData.type=rec.value(rec.indexOf("type")).toInt();
    sourceData.link=rec.value(rec.indexOf("link")).toString();
    return sourceData;
}

bool HumansDatabase::deleteSource(QString uuid)
{
    if(!this->isOpen() || uuid.isEmpty())return false;
    QSqlQuery q(db);
    q.prepare("DELETE FROM sources WHERE id=:id;");
    q.bindValue(":id",uuid);

    bool res=q.exec();
    if(res)
    {//можно было бы на триггер повесить, но неохота.
        this->transactionStart();
        q.prepare("UPDATE record SET sourceid=NULL WHERE sourceid=:id;");
        q.bindValue(":id",uuid);
        q.exec();

        q.prepare("DELETE FROM recordsources where sourceid=:id;");
        q.bindValue(":id", uuid);
        q.exec();
        this->transactionEnd();
    }
    return res;
}

QList<QStringList> HumansDatabase::getSources(QString searchString, bool searchForType, qint8 sourceType)
{
    QList<QStringList> list;
    if(!this->isOpen())return list;
    QSqlQuery q(db);
    QString query("SELECT id,name,type, link, note FROM sources");
    QStringList whereAndList;
    if(!searchString.isEmpty())
    {
        whereAndList.append(" name like '%"+searchString+"%' or link like '%"+searchString+"%' or note like'%"+searchString+"%'");
    }
    if(searchForType)
    {
        whereAndList.append("type = "+QString::number(sourceType,10));
    }
    if(whereAndList.count()>0)
    {
        query.append(" WHERE ");
        query.append("("+whereAndList.join(") AND (")+")");//ну надеюсь получится...
    }
    query.append(";");
    qDebug()<<"Query string final is "<<query;
    q.prepare(query);
    bool res=q.exec();
    if(!res)
    {
        mess("Get sources error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
        return list;
    }
    if(!q.first()) return list;
    do
    {
        QStringList sourceData;
        //на основе каждой записи получаем информацию...
        QSqlRecord record=q.record();        
        sourceData.append(record.value(record.indexOf("id")).toString());
        sourceData.append(record.value(record.indexOf("name")).toString());
        sourceData.append(record.value(record.indexOf("note")).toString());
        sourceData.append(record.value(record.indexOf("link")).toString());
        sourceData.append(SourceData::getSourceName(record.value(record.indexOf("type")).toInt()));
        list.append(sourceData);
    }while(q.next());
    //Похоже что-то получили...Так что поехали.
    return list;
}

bool HumansDatabase::addRecordSource(RecordSourceData recordSource)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);
    q.prepare ("INSERT OR IGNORE INTO recordsources (id,recordid,sourceid,location,note) VALUES(:id,:recordid,:sourceid,:location,:note);");
    q.bindValue(":id",recordSource.uuid);
    q.bindValue(":recordid",recordSource.recordId);
    q.bindValue(":sourceid",recordSource.sourceId);
    q.bindValue(":location",recordSource.location);
    q.bindValue(":note",recordSource.note);
    bool res=q.exec();
    if(!res)
        mess("Add source error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
    return res;
}

bool HumansDatabase::editRecordSource(RecordSourceData recordSource)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);
    q.prepare("UPDATE OR IGNORE recordsources SET recordid=:recordid,sourceid=:sourceid,location=:location,note=:note WHERE id=:id;");
    q.bindValue(":id",recordSource.uuid);
    q.bindValue(":recordid",recordSource.recordId);
    q.bindValue(":sourceid",recordSource.sourceId);
    q.bindValue(":location",recordSource.location);
    q.bindValue(":note",recordSource.note);
    bool res=q.exec();
    if(!res)
        mess("Edit place error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
    return res;
}

RecordSourceData HumansDatabase::getRecordSource(QString uuid)
{
    RecordSourceData sourceRecordData;
    if(!this->isOpen() || uuid.isEmpty())return sourceRecordData;
    QSqlQuery q(db);

    q.prepare("SELECT * FROM recordsources WHERE id=:id LIMIT 1;");
    q.bindValue(":id",uuid);
    q.exec();
    if(!q.first())
    {
        mess("Get recordSource error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());

        return sourceRecordData;
    }
    QSqlRecord rec=q.record();

    sourceRecordData.uuid=rec.value(rec.indexOf("id")).toString();
    sourceRecordData.recordId=rec.value(rec.indexOf("recordid")).toString();
    sourceRecordData.sourceId=rec.value(rec.indexOf("sourceid")).toString();
    sourceRecordData.location=rec.value(rec.indexOf("location")).toString();
    sourceRecordData.note=rec.value(rec.indexOf("note")).toString();
    return sourceRecordData;
}

bool HumansDatabase::deleteRecordSource(QString uuid)
{
    if(!this->isOpen() || uuid.isEmpty())return false;
    QSqlQuery q(db);
    q.prepare("DELETE FROM recordsources WHERE id=:id;");
    q.bindValue(":id",uuid);

    bool res=q.exec();
    return res;
}

bool HumansDatabase::addHumanPlace(HumanPlaceData humanPlace)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);

    //qDebug()<<"Add humanid :"<<humanPlace.humanId;
    q.prepare ("INSERT OR IGNORE INTO humanplaces (id,placeid,humanid,date,type,note,owner) VALUES(:id,:placeid,:humanid,:date,:type,:note,:owner);");
    q.bindValue(":id",humanPlace.uuid);
    q.bindValue(":placeid",humanPlace.placeId);
    q.bindValue(":humanid",humanPlace.humanId);
    q.bindValue(":date",humanPlace.date);
    q.bindValue(":type",humanPlace.type);
    q.bindValue(":note",humanPlace.note);
    q.bindValue(":owner",humanPlace.owner);
    bool res=q.exec();
    if(!res)
        mess("Add HumanPlace error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
    return res;

}

bool HumansDatabase::editHumanPlace(HumanPlaceData humanPlace)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);
    q.prepare("UPDATE OR IGNORE humanplaces SET date=:date,type=:type,note=:note WHERE id=:id;");
    q.bindValue(":id",humanPlace.uuid);
    q.bindValue(":date",humanPlace.date);
    q.bindValue(":type",humanPlace.type);
    q.bindValue(":note",humanPlace.note);
    bool res=q.exec();
    if(!res)
        mess("Edit HumanPlace error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
    return res;
}

bool HumansDatabase::deleteHumanPlace(QString uuid)
{
    if(!this->isOpen() || uuid.isEmpty())return false;
    QSqlQuery q(db);
    q.prepare("DELETE FROM humanplaces WHERE id=:id;");
    q.bindValue(":id",uuid);
    bool res=q.exec();
    if(!res)
         mess("Delete Human place error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
    return res;
}

HumanPlaceData HumansDatabase::getHumanPlace(QString humanPlaceId)
{
    HumanPlaceData humanPlaceData;
    if(!this->isOpen())return humanPlaceData;
    QSqlQuery q(db);
    q.prepare("SELECT id,humanid,placeid,date,type,note,owner FROM humanplaces WHERE id=:id LIMIT 1;");
    q.bindValue(":id",humanPlaceId);
    bool res=q.exec();

    if(!res)
    {
        mess("Get HumanPlace error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
        return humanPlaceData;
    }
    if(!q.first()) return humanPlaceData;
    humanPlaceData.uuid=q.record().value(q.record().indexOf("id")).toString();
    humanPlaceData.humanId=q.record().value(q.record().indexOf("humanid")).toString();
    humanPlaceData.placeId=q.record().value(q.record().indexOf("placeid")).toString();
    humanPlaceData.date=q.record().value(q.record().indexOf("date")).toString();
    humanPlaceData.type=q.record().value(q.record().indexOf("type")).toString();
    humanPlaceData.note=q.record().value(q.record().indexOf("note")).toString();
    humanPlaceData.owner=q.record().value(q.record().indexOf("owner")).toString();
    return humanPlaceData;

}

bool HumansDatabase::addParent(QString childId, QString parentId, HumanSex parentSex)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);
    QString parent;
    if(parentSex==humanSexFemale) parent="mother";
    else if(parentSex==humanSexMale) parent="father";
    else return false;
    q.prepare("UPDATE humans SET "+parent+"=:parentid WHERE id=:childid;");
    q.bindValue(":parentid",parentId);
    q.bindValue(":childid",childId);
    return q.exec();
}

void HumansDatabase::cleanDatabase()
{
    if(!this->isOpen())return;
    QSqlQuery q(db);
    q.prepare("");
    q.exec();
}

bool HumansDatabase::addHumanToRecord(QString recordId, QString humanId)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);
    q.prepare("INSERT OR IGNORE INTO recordhumans (id,recordid,humanid) VALUES (:id,:recordid,:humanid);");
    q.bindValue(":id",QUuid::createUuid().toString());
    q.bindValue(":recordid",recordId);
    q.bindValue(":humanid",humanId);
    return q.exec();

}

bool HumansDatabase::addPlaceToRecord(QString recordId, QString placeId)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);
    q.prepare("INSERT OR IGNORE INTO recordplaces (id,recordid,placeid) VALUES (:id,:recordid,:placeid);");
    q.bindValue(":id",QUuid::createUuid().toString());
    q.bindValue(":recordid",recordId);
    q.bindValue(":placeid",placeId);
    return q.exec();
}

bool HumansDatabase::deleteHumanFromRecord(QString recordId, QString humanId)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);
    q.prepare("DELETE FROM recordhumans WHERE recordid=:recordid AND humanid=:humanid;");
    q.bindValue(":recordid",recordId);
    q.bindValue(":humanid",humanId);
    return q.exec();
}

bool HumansDatabase::deletePlaceFromRecord(QString recordId, QString placeId)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);
    q.prepare("DELETE FROM recordplaces WHERE recordid=:recordid AND placeid=:placeid;");
    q.bindValue(":recordid",recordId);
    q.bindValue(":placeid",placeId);
    return q.exec();
}

bool HumansDatabase::deleteSourceFromRecord(QString uuid)
{
    if(!this->isOpen())return false;
    QSqlQuery q(db);
    q.prepare("DELETE FROM recordsources WHERE id=:uuid;");
    q.bindValue(":uuid",uuid);
    return q.exec();
}

QList<QStringList> HumansDatabase::getChildrenInfo(QString humanId)
{
    QList<QStringList> list;
    //Я, конечно, мог бы использовать существующие функции типа getHuman, но это медленнее, так что пока так вот.
    //Дальше будет хуже, когда нужно будет менять все места рождения на placeId...Почему-то я это откладываю пока...странно.
    if(!this->isOpen())return list;
    QSqlQuery q(db);
    q.prepare("SELECT id,surname,marname,name,patronymic,birthyear, deathyear, birthplace FROM humans WHERE mother=:motherid OR father=:fatherid;");
    q.bindValue(":motherid", humanId);
    q.bindValue(":fatherid", humanId);
    bool res=q.exec();
    if(!res)
    {
        mess("Get children error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
        return list;
    }
    if(!q.first()) return list;
    do
    {
        QStringList childData;
        //на основе каждой записи получаем информацию...
        QSqlRecord record=q.record();
        //qDebug()<<"record about child "<<record;
        childData.append(record.value(record.indexOf("id")).toString());
        childData.append(HumansDatabase::getHumanInfoFromRecord(record));
        list.append(childData);
    }while(q.next());
    //Похоже что-то получили...Так что поехали.
    return list;
}

QList<QStringList> HumansDatabase::getSpousesInfo(QString humanId,HumanSex humanSex)
{
    QList<QStringList> list;
    if(!this->isOpen())return list;
    QSqlQuery q(db);
    switch(humanSex)
    {
    case humanSexMale:
         q.prepare("SELECT id,surname,marname,name,patronymic,birthyear, deathyear, birthplace FROM humans WHERE id IN (SELECT wife FROM marriages WHERE husband=:id);");
          q.bindValue(":id", humanId);
         //qDebug()<<"SeaRCH For husband";
        break;
    case humanSexFemale:
        q.prepare("SELECT id,surname,marname,name,patronymic,birthyear, deathyear, birthplace FROM humans WHERE id IN (SELECT husband FROM marriages WHERE wife=:id);");
         q.bindValue(":id", humanId);
        //qDebug()<<"SeaRCH For wife";
        break;
    case humanSexUndefined:
        q.prepare("SELECT DISTINCT id,surname,marname,name,patronymic,birthyear, deathyear, birthplace FROM humans WHERE id IN (SELECT husband FROM marriages WHERE wife=:husbandid) OR id IN (SELECT wife FROM marriages WHERE husband=:wifeid);");
         q.bindValue(":husbandid", humanId);
         q.bindValue(":wifeid", humanId);
        //qDebug()<<"SeaRCH For everyone";
        break;
    }

    bool res=q.exec();
    if(!res)
    {
        mess("Get spouses error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
        return list;
    }
    if(!q.first()) return list;
    do
    {
        QStringList childData;
        //на основе каждой записи получаем информацию...
        QSqlRecord record=q.record();
        //qDebug()<<"Record of spouse:"<<record;
        childData.append(record.value(record.indexOf("id")).toString());
        childData.append(HumansDatabase::getHumanInfoFromRecord(record));
        list.append(childData);
    }while(q.next());
    //Похоже что-то получили...Так что поехали.
    return list;
}

QList<QStringList> HumansDatabase::getHumanPlacesInfo(QString humanId)
{
    QList<QStringList> list;
    if(!this->isOpen())return list;
    QSqlQuery q(db);


    q.prepare("SELECT humanplaces.id, humanplaces.date,humanplaces.note,humanplaces.type, places.type || ' ' || places.name as place FROM humanplaces LEFT JOIN places ON humanplaces.placeid=places.id where humanplaces.humanid=:humanid order by humanplaces.date desc;");
    q.bindValue(":humanid",humanId);
    bool res = q.exec();

    if(!res)
    {
        mess("Get All HumanPlaces error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
        return list;
    }
    if(!q.first()) return list;
    do
    {
        QStringList humanPlacesData;
        QSqlRecord record=q.record();
        humanPlacesData.append(record.value(record.indexOf("id")).toString());
        humanPlacesData.append(HumansDatabase::getHumanPlaceInfoFromRecord(record));
        list.append(humanPlacesData);
    }while(q.next());
    //Похоже что-то получили...Так что поехали.
    return list;
}

QList<QStringList> HumansDatabase::getHumanPlacesInfoForExport(QString humanId)
{
    QList<QStringList> list;
    if(!this->isOpen())return list;
    QSqlQuery q(db);


    q.prepare("SELECT humanplaces.id, humanplaces.date,humanplaces.note,humanplaces.type, places.type || ' ' || places.name as place FROM humanplaces LEFT JOIN places ON humanplaces.placeid=places.id where humanplaces.humanid=:humanid order by humanplaces.date desc;");
    q.bindValue(":humanid",humanId);
    bool res = q.exec();

    if(!res)
    {
        mess("Get All HumanPlaces error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
        return list;
    }
    if(!q.first()) return list;
    do
    {
        QStringList humanPlacesData;
        QSqlRecord record=q.record();
        humanPlacesData.append(record.value(record.indexOf("place")).toString());
        humanPlacesData.append(record.value(record.indexOf("date")).toString());
        list.append(humanPlacesData);
    }while(q.next());
    //Похоже что-то получили...Так что поехали.
    return list;
}

QList<QStringList> HumansDatabase::getHumansOfRecordInfo(QString recordId)
{
    QList<QStringList> list;
    if(!this->isOpen())return list;
    QSqlQuery q(db);
    q.prepare("SELECT id,surname,marname,name,patronymic,birthyear, deathyear, birthplace FROM humans WHERE id IN (SELECT humanid FROM recordhumans WHERE recordid=:recordid) order by birthyear asc;");
    q.bindValue(":recordid", recordId);
    bool res=q.exec();
    if(!res)
    {
        mess("Get HumansOfRecord error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
        return list;
    }
    if(!q.first()) return list;
    do
    {
        QStringList humData;
        QSqlRecord record=q.record();
        humData.append(record.value(record.indexOf("id")).toString());
        humData.append(HumansDatabase::getHumanInfoFromRecord(record));
        list.append(humData);
    }while(q.next());
    return list;
}

QList<QStringList> HumansDatabase::getPlacesOfRecordInfo(QString recordId)
{

    QList<QStringList> list;
    if(!this->isOpen())return list;
    QSqlQuery q(db);
    q.prepare("SELECT id,name,type,anothernames,lat,lon,coordacc FROM places WHERE id IN (SELECT placeid FROM recordplaces WHERE recordid=:recordid);");
    q.bindValue(":recordid", recordId);
    bool res=q.exec();
    if(!res)
    {
        mess("Get PlacesOfRecord error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
        return list;
    }
    if(!q.first()) return list;
    do
    {
        QStringList humData;
        QSqlRecord record=q.record();
        humData.append(record.value(record.indexOf("id")).toString());
        humData.append(HumansDatabase::getPlaceInfoFromRecord(record));
        //всё забыл, балбес...вспоминай, чо.
        list.append(humData);
    }while(q.next());
    return list;
}

QList<QStringList> HumansDatabase::getSourcesOfRecordInfo(QString recordId)
{
    QList<QStringList> list;
    if(!this->isOpen())return list;
    QSqlQuery q(db);

    q.prepare("SELECT recordsources.id as id, recordsources.location as location, recordsources.note as note, sources.name as name FROM sources INNER JOIN recordsources on sources.id=recordsources.sourceid where sources.id in (select sourceid from recordsources where recordid=:recordid);");

    q.bindValue(":recordid", recordId);
    bool res=q.exec();
    if(!res)
    {
        mess("Get PlacesOfRecord error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
        return list;
    }
    if(!q.first()) return list;
    do
    {
        QStringList humData;
        QSqlRecord record=q.record();
        humData.append(record.value(record.indexOf("id")).toString());
        humData.append(HumansDatabase::getSourceInfoFromRecord(record));
        list.append(humData);
    }while(q.next());
    return list;


}

QList<QStringList> HumansDatabase::getHumanSiblingsInfo(QString fatherId, QString motherId, QString humanId)
{
    QList<QStringList> list;
    if(!this->isOpen())return list;
    QSqlQuery q(db);
    q.prepare("SELECT id,surname,marname,name,patronymic,birthyear, deathyear, birthplace, father,mother FROM humans WHERE (father=:fatherid OR mother=:motherid) AND id<>:humanid;");
    q.bindValue(":fatherid", fatherId);
    q.bindValue(":motherid", motherId);
    q.bindValue(":humanid", humanId);
    bool res=q.exec();
    if(!res)
    {
        mess("Get Human Siblings error: "+q.lastError().databaseText()+",driver:"+q.lastError().driverText());
        return list;
    }
    if(!q.first()) return list;
    do
    {
        QStringList humData;
        QSqlRecord record=q.record();
        humData.append(record.value(record.indexOf("id")).toString());
        int siblingFlag=0;
        if(record.value((record.indexOf("father"))).toString()==fatherId)siblingFlag|=1;
        if(record.value((record.indexOf("mother"))).toString()==motherId)siblingFlag|=2;
        QString siblingAddon;
        switch(siblingFlag)
        {
            case 1:
                siblingAddon=", по отцу";
                break;
            case 2:
                siblingAddon=", по матери";
                break;
            default:
                break;
        }
        humData.append(HumansDatabase::getHumanInfoFromRecord(record)+siblingAddon);
        list.append(humData);
    }while(q.next());
    return list;
}

QVariant HumansDatabase::getHumanSearchQueryCount(QString searchString, QVariant searchSex, QString searchPlaceId, unsigned int flags)
{
    if(!this->isOpen())return -1;
    QString s=HumansDatabase::getHumanSearchQuery(searchString,searchSex,searchPlaceId,flags | COMMONSEARCHFLAG_COUNTQUERY);
    QSqlQuery q(db);
    q.prepare(s);
    q.exec();
    if(!q.first())return -1;
    return q.record().value(0).toInt();
}

QVariant HumansDatabase::getRecordSearchQueryCount(QString searchString, QString searchHumanId, QString searchPlaceId, unsigned int flags)
{
    if(!this->isOpen())return -1;
    QString s=HumansDatabase::getRecordSearchQuery(searchString,searchHumanId,searchPlaceId,flags | COMMONSEARCHFLAG_COUNTQUERY);
    QSqlQuery q(db);
    q.prepare(s);
    q.exec();
    if(!q.first())return -1;
    return q.record().value(0).toInt();
}

QVariant HumansDatabase::getPlaceSearchQueryCount(QString searchString, unsigned int flags)
{
    if(!this->isOpen())return -1;
    QString s=HumansDatabase::getPlaceSearchQuery(searchString,flags | COMMONSEARCHFLAG_COUNTQUERY);
    QSqlQuery q(db);
    q.prepare(s);
    q.exec();
    if(!q.first())return -1;
    return q.record().value(0).toInt();
}

void HumansDatabase::mess(QString message) const
{
    qDebug()<<message;
    Q_EMIT sendMessage(message,messageText);
}

QUuid HumansDatabase::getUuidFromString(QString uuidString)
{
    //return QUuid::fromRfc4122(QByteArray::fromHex( uuidString.remove(QRegExp("[^0-9a-fA-F]")).toUtf8()));
    return QUuid::fromRfc4122(QByteArray::fromHex( uuidString.toUtf8()));//fromHex игнорирует не hex-символы, поэтому работает как надо.
}

QString HumansDatabase::getHumanSearchQuery(QString searchString, QVariant searchSex, QString searchPlaceId, unsigned int flags)
{
    QStringList strList;
    if(!searchString.isEmpty())
        strList=searchString.split(" ");
    QStringList searchColumns;
    searchColumns<<"humans.surname"<<"humans.marname"<<"humans.name"<<"humans.patronymic"<<"humans.birthyear"<<"humans.deathyear"<<"places.name";
    QString whereStr;
    QStringList whereAndList;
    if(!strList.isEmpty())
    {
        for(int i=0;i<strList.count();i++)
        {
            QStringList whereOrList;
            for(int j=0;j<searchColumns.count();j++)
                whereOrList.append(searchColumns[j]+" like '%"+strList[i]+"%'");
            whereAndList.append(whereOrList.join(" OR "));
        }
    }
    if(searchSex==humanSexMale)whereAndList.append("sex=1");
    if(searchSex==humanSexFemale)whereAndList.append("sex=0");
    if(!searchPlaceId.isEmpty())
    {
        whereAndList.append("humans.id IN (SELECT humanid from humanplaces where placeid='"+searchPlaceId+"')");
    }
    if(flags&COMMONSEARCHFLAG_NORECORDS)
    {
        whereAndList.append("humans.id not in (select humanid from recordhumans)");
    }
    if(flags&COMMONSEARCHFLAG_NOPLACES)
    {
        whereAndList.append("humans.id NOT IN (SELECT humanid FROM humanplaces)");
    }
    if(whereAndList.count()>0)
    {
        whereStr.append(" WHERE ");
        whereStr.append("("+whereAndList.join(") AND (")+")");
    }    
    //QString mainRequest("SELECT humans.id,humans.surname,humans.marname,humans.name,humans.patronymic,humans.birthyear, humans.deathyear, humans.birthplace FROM humans");
    //QString mainRequest("SELECT humans.id,humans.surname,humans.marname,humans.name,humans.patronymic,humans.birthyear, humans.deathyear, places.type || ' ' || places.name as birthplace, humans.created FROM humans LEFT JOIN places on humans.birthplace=places.id");
    QString mainRequest;
    if(flags&COMMONSEARCHFLAG_COUNTQUERY)
        mainRequest="SELECT count(humans.id) FROM humans LEFT JOIN places ON humans.birthplace=places.id";
    else
        mainRequest="SELECT humans.id, CASE WHEN LENGTH(humans.surname)>0 THEN humans.surname ELSE humans.marname END AS family, humans.marname,humans.name,humans.patronymic,humans.birthyear, humans.deathyear, places.type || ' ' || places.name AS birthplace, humans.created, humans.changed FROM humans LEFT JOIN places ON humans.birthplace=places.id";
    if(!whereStr.isEmpty())
        mainRequest.append(whereStr);    
    mainRequest.append(" ORDER BY humans.created ASC");
    //qDebug()<<"HUMANS MAIN REQUEST : "<<mainRequest;
    return mainRequest;
}

QString HumansDatabase::getRecordSearchQuery(QString searchString, QString searchHumanId, QString searchPlaceId, unsigned int flags)
{
    QStringList strList;
    if(!searchString.isEmpty())
        strList=searchString.split(" ");
    QStringList searchColumns;
    searchColumns<<"record"<<"note"<<"date"<<"year"<<"location";
    QString whereStr;
    QStringList whereAndList;
    if(!strList.isEmpty())
    {
        for(int i=0;i<strList.count();i++)
        {
            QStringList whereOrList;
            for(int j=0;j<searchColumns.count();j++)
                whereOrList.append(searchColumns[j]+" like '%"+strList[i]+"%'");
            whereAndList.append(whereOrList.join(" OR "));
        }
    }
    if(!searchHumanId.isEmpty())
    {
        whereAndList.append("id IN (SELECT recordid FROM recordhumans WHERE humanid='"+searchHumanId+"')");
    }
    if(!searchPlaceId.isEmpty())
    {
        whereAndList.append("id IN (SELECT recordid from recordplaces where placeid='"+searchPlaceId+"')");
    }
    if(flags&COMMONSEARCHFLAG_NOPLACES)
    {
        whereAndList.append("id NOT IN (SELECT recordid FROM recordplaces)");
    }

    if(flags&COMMONSEARCHFLAG_NOHUMANS)
    {
        whereAndList.append("id NOT IN (SELECT recordid FROM recordhumans)");
    }
    if(whereAndList.count()>0)
    {
        whereStr.append(" WHERE ");
        whereStr.append("("+whereAndList.join(") AND (")+")");
    }
    //QString mainRequest("SELECT id,year,substr(record,1,50) as rec,note,date,location,created FROM records");
    QString mainRequest;
    if(flags&COMMONSEARCHFLAG_COUNTQUERY)
        mainRequest="SELECT count(id) FROM records";
    else
        mainRequest="SELECT id,year,record,note,date,location,created,changed FROM records";
    if(!whereStr.isEmpty())
        mainRequest.append(whereStr);
    mainRequest.append(" ORDER BY created ASC");
    //qDebug()<<"Full record search request "<<mainRequest;
    return mainRequest;
}

QString HumansDatabase::getPlaceSearchQuery(QString searchString,unsigned int flags)
{
    QStringList strList;
    if(!searchString.isEmpty())
        strList=searchString.split(" ");
    QStringList searchColumns;
    searchColumns<<"name"<<"type"<<"anothernames"<<"note";
    QString whereStr;
    QStringList whereAndList;
    if(!strList.isEmpty())
    {
        for(int i=0;i<strList.count();i++)
        {
            QStringList whereOrList;
            for(int j=0;j<searchColumns.count();j++)
                whereOrList.append(searchColumns[j]+" like '%"+strList[i]+"%'");
            whereAndList.append(whereOrList.join(" OR "));
        }
    }
    if(whereAndList.count()>0)
    {
        whereStr.append(" WHERE ");
        whereStr.append("("+whereAndList.join(") AND (")+")");
    }    

    QString mainRequest;
    if(flags&COMMONSEARCHFLAG_COUNTQUERY)
        mainRequest = "SELECT count(id) FROM places";
    else
        mainRequest = "SELECT id,type,name,anothernames,lat,lon,coordacc,note FROM places";
    if(!whereStr.isEmpty())
        mainRequest.append(whereStr);
    mainRequest.append(" ORDER BY name ASC");
    return mainRequest;
}

QString HumansDatabase::getHumanInfoFromRecord(QSqlRecord record)
{
    QString humanInfo;
    humanInfo.append(record.value(record.indexOf("surname")).toString());
    QString marName=record.value(record.indexOf("marname")).toString();    
    if(!marName.isEmpty())
        humanInfo.append(" ("+marName+")");
    humanInfo.append(" "+record.value(record.indexOf("name")).toString());    
    humanInfo.append(" "+record.value(record.indexOf("patronymic")).toString());    
    QString deathyear=record.value(record.indexOf("deathyear")).toString();
    QString birthyear=record.value(record.indexOf("birthyear")).toString();
    if(!birthyear.isEmpty() || !deathyear.isEmpty())
    {
       humanInfo.append(" "+birthyear+"-"+deathyear);
    }    
    return humanInfo;
}

QString HumansDatabase::getHumanPlaceInfoFromRecord(QSqlRecord record)
{
    QString humanPlaceInfo;
    humanPlaceInfo.append(record.value(record.indexOf("place")).toString());
    QString type=record.value(record.indexOf("type")).toString();
    if(!type.isEmpty())
        humanPlaceInfo.append(", "+type);
    QString date=record.value(record.indexOf("date")).toString();
    if(!date.isEmpty())
        humanPlaceInfo.append(", "+date);
    QString note=record.value(record.indexOf("note")).toString();
    if(!note.isEmpty())
        humanPlaceInfo.append(", "+note);
    return humanPlaceInfo;
}

QString HumansDatabase::getPlaceInfoFromRecord(QSqlRecord record)
{
    QString placeInfo;
    placeInfo.append(record.value(record.indexOf("type")).toString());
    placeInfo.append(" "+record.value(record.indexOf("name")).toString());
    QString anothernames=record.value(record.indexOf("anothernames")).toString();
    if(!anothernames.isEmpty())
        placeInfo.append("("+anothernames+")");
    QString lat=record.value(record.indexOf("lat")).toString();
    QString lon=record.value(record.indexOf("lon")).toString();
    QString coordacc=record.value(record.indexOf("coordacc")).toString();
    QStringList coordinates;
    if(!lat.isEmpty()) coordinates.append(lat);
    if(!lon.isEmpty()) coordinates.append(lon);
    if(!coordacc.isEmpty())coordinates.append(coordacc);
    if(coordinates.size()>1)
        placeInfo.append(", coord:"+coordinates.join(", "));
    return placeInfo;
}

QString HumansDatabase::getSourceInfoFromRecord(QSqlRecord record)
{
    QString sourceRecordInfo;

    sourceRecordInfo.append( record.value( record.indexOf("name") ).toString()  );
    sourceRecordInfo.append(", ");
    sourceRecordInfo.append( record.value( record.indexOf("location") ).toString()  );
    sourceRecordInfo.append(", ");
    sourceRecordInfo.append(SourceData::getSourceName(record.value(record.indexOf("type")).toInt()));
    //жёстко-то как...
    return sourceRecordInfo;
}

HumanData HumansDatabase::getHumanDataFromRecord(QSqlRecord record)
{
    HumanData humanData(true);
    //Пошли гнать данные сюду.
    humanData.uuid=HumansDatabase::getUuidFromString(record.value(record.indexOf("id")).toString());
    humanData.givenName=record.value(record.indexOf("name")).toString();
    humanData.patronymicName=record.value(record.indexOf("patronymic")).toString();
    humanData.surName=record.value(record.indexOf("surname")).toString();
    humanData.marName=record.value(record.indexOf("marname")).toString();
    int sex=record.value(record.indexOf("sex")).toInt();
    if(sex==0) humanData.sex=humanSexFemale;
    else if (sex==1) humanData.sex=humanSexMale;
    else humanData.sex=humanSexUndefined;
    QString father=record.value(record.indexOf("father")).toString();
    if(father.isEmpty())humanData.father=QUuid();
    else
        humanData.father=HumansDatabase::getUuidFromString(father);
    QString mother=record.value(record.indexOf("mother")).toString();
    if(mother.isEmpty())humanData.mother=QUuid();
    else
        humanData.mother=HumansDatabase::getUuidFromString(mother);
    humanData.birthDate=record.value(record.indexOf("birthdate")).toString();
    if(!record.value(record.indexOf("birthyear")).isNull())
        humanData.birthYear=record.value(record.indexOf("birthyear")).toInt();
    else
        humanData.deathYear=-1;
    humanData.birthPlaceId=record.value(record.indexOf("birthplace")).toString();

    humanData.deathDate=record.value(record.indexOf("deathdate")).toString();
    if(!record.value(record.indexOf("deathyear")).isNull())
        humanData.deathYear=record.value(record.indexOf("deathyear")).toInt();
    else
        humanData.deathYear=-1;
    humanData.deathCause=record.value(record.indexOf("deathcause")).toString();
    humanData.occupation=record.value(record.indexOf("occupation")).toString();
    humanData.note=record.value(record.indexOf("note")).toString().split(";");
    humanData.created=record.value(record.indexOf("created")).toInt();
    humanData.changed=record.value(record.indexOf("changed")).toInt();

    QString owner=record.value(record.indexOf("owner")).toString();
    if(owner.isEmpty())humanData.owner=QUuid();
    else
        humanData.owner=HumansDatabase::getUuidFromString(owner);

    return humanData;
}

PlaceData HumansDatabase::getPlaceDataFromRecord(const QSqlRecord &record)
{
    PlaceData placeData;
    placeData.uuid=record.value(record.indexOf("id")).toString();
    placeData.name=record.value(record.indexOf("name")).toString();
    placeData.type=record.value(record.indexOf("type")).toString();
    placeData.anotherNames=record.value(record.indexOf("anothernames")).toString().split(";");
    if(placeData.anotherNames.count()==1 && placeData.anotherNames[0].isEmpty())placeData.anotherNames.clear();
    placeData.latitude=record.value(record.indexOf("lat")).toString();
    placeData.longitude=record.value(record.indexOf("lon")).toString();
    placeData.coordAccuracy=record.value(record.indexOf("coordacc")).toString();
    placeData.note=record.value(record.indexOf("note")).toString();
    placeData.owner=record.value(record.indexOf("owner")).toString();
    return placeData;
}

void HumansDatabase::patchDatabase()
{
    if(!isFieldExists("places","lat"))
    {
        QSqlQuery q(db);
        bool res;
        res=q.exec("ALTER TABLE places ADD COLUMN lat REAL;");//широта
        if(!res)
            mess("Last error"+db.lastError().databaseText()+",driver:"+db.lastError().driverText());
        res=q.exec("ALTER TABLE places ADD COLUMN lon REAL;");//долгота
        if(!res)
            mess("Last error"+db.lastError().databaseText()+",driver:"+db.lastError().driverText());
        res=q.exec("ALTER TABLE places ADD COLUMN coordacc INTEGER;");//точность в метрах
        if(!res)
            mess("Last error"+db.lastError().databaseText()+",driver:"+db.lastError().driverText());
        //Это не особо работает, кстати...Ограничения sqlite. Оставим для совместимости. Пока.
        //res=q.exec("ALTER TABLE places DROP COLUMN coordinates;");
    }

}
