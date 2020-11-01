#include <QDebug>

#include "humansdatabasecalc.h"
/**
 * @brief HumansDatabaseCalc::HumansDatabaseCalc
 * Конструктор класса HumansDatabaseCalc. По сути не нужен, так как скорее всего класс будет представлять собой лишь набор статических функций.
 * Может быть класс вообще не нужен? Может быть, но пусть будет.
 */
HumansDatabaseCalc::HumansDatabaseCalc()
{
}

void HumansDatabaseCalc::recursiveRelativesSearch(HumansDatabase *db, QList<HumanData> *list, unsigned int level, unsigned int humanListCount, unsigned int maxLevel, bool searchRelatives, bool directionIsDown)
{
    HumanData child=list->at(humanListCount);

    if(directionIsDown==false)
    {
        //значит мы попали сюда
        //тут мы ищем мужа/жену пациента, необязательно добавлять её в список. И детей пациента.
        //qDebug()<<"UP DIRECTION for "<<child.getHumanInfo()<<", level "<<level;
        QList<HumanData> spouses;
        spouses=db->getSpouses(child.uuid.toString());
/*
        for(int i=0;i<spouses.size();i++)
        {
            for(int j=0;j<list->size();j++)
            if(spouses.at(i).uuid==list->at(j).uuid)
            {
                HumanData dat=list->at(j);
                qDebug()<<"Повтор, однако, есть такой СУПРУГ у нас: "<<dat.getHumanInfo();
            }
        }
*/
        for(int i=0;i<spouses.size();i++)
        {
            //HumanData spouse=spouses[i];
           //list->append(spouse);
           list->append(spouses[i]);

        }
        //теперь детей давай.

        QList<HumanData> children;
        children=db->getChildren(child.uuid.toString());
        /*
        for(int i=0;i<children.size();i++)
        {
            for(int j=0;j<list->size();j++)
            if(children.at(i).uuid==list->at(j).uuid)
            {
                HumanData dat=list->at(j);
                qDebug()<<"Повтор, однако, есть такой ДИТЁ у нас: "<<dat.getHumanInfo();
            }
        }
        */
        for(int i=0;i<children.size();i++)
        {
            //HumanData child=children[i];
            //list->append(child);
            list->append(children[i]);
            recursiveRelativesSearch(db,list,level+1,list->size()-1,maxLevel,searchRelatives,false);//ищем детей детей
        }
        return;
    }

    if(searchRelatives)
    {
        //Значит надо найти братьев и сестров этого человека, который child. Окей? Окей.
        //но тогда рушится схема с childListCount...А может и не рушится.
        //Короче, ищем всех, у кого родители совпадают полностью или частично, но при этом childListCount другой. Это будут братья и сестры.
        //добавляем их в список, между прочим, а потом ищем вверх (т.е. их мужей, их детей).
        QList<HumanData> siblings;
        //qDebug()<<"Search siblings of "<<child.getHumanInfo();
        siblings=db->getSiblings(child.uuid.toString(),child.father.toString(),child.mother.toString());
        /*
        for(int i=0;i<siblings.size();i++)
        {
            for(int j=0;j<list->size();j++)
            if(siblings.at(i).uuid==list->at(j).uuid)
            {
                HumanData dat=list->at(j);
                qDebug()<<"Повтор, однако, есть такой человек у нас: "<<dat.getHumanInfo();
            }
        }
        */
        for(int i=0;i<siblings.size();i++)
        {
            //HumanData sibling=siblings[i];//мало ли, а то начнёт их из списка дёргать...
            //list->append(sibling);
            list->append(siblings[i]);

            if(level<maxLevel)
                recursiveRelativesSearch(db,list,level+1,list->size()-1,maxLevel,searchRelatives,false);//ищем детей их и супругов. Но это уже не здесь.
        }
    }

    //Direction Is Down, в смысле ищем предков.
    //Ищем данные отца и уходим в рекурсию по его предкам.
    if(!list->at(humanListCount).father.isNull())
    {
        HumanData fatherHuman=db->getHuman(list->at(humanListCount).father.toString());
        fatherHuman.descendantsInfo=child.descendantsInfo;
        fatherHuman.descendantsInfo.append(child.getHumanInfo());
        if(!fatherHuman.uuid.isNull())
        {
            fatherHuman.recursyList=list->at(humanListCount).recursyList;
            fatherHuman.recursyList.append(humanSexMale);
            list->append(fatherHuman);
            if(level<maxLevel)
                recursiveRelativesSearch(db,list,level+1,list->size()-1,maxLevel,searchRelatives,directionIsDown);
        }
    }
    //Ищем данные матери и уходим в рекурсию по его предкам.
    if(!list->at(humanListCount).mother.isNull())
    {
        HumanData motherHuman=db->getHuman(list->at(humanListCount).mother.toString());
        if(!motherHuman.uuid.isNull())
        {
            motherHuman.descendantsInfo=child.descendantsInfo;
            motherHuman.descendantsInfo.append(child.getHumanInfo());
            motherHuman.recursyList=list->at(humanListCount).recursyList;
            motherHuman.recursyList.append(humanSexFemale);
            list->append(motherHuman);
            if(level<maxLevel)
                recursiveRelativesSearch(db,list,level+1,list->size()-1,maxLevel,searchRelatives,directionIsDown);
        }
    }

}

void HumansDatabaseCalc::recursiveAncestrySearch(HumansDatabase *db, QList<HumanData> *list, unsigned int level, unsigned int humanListCount, unsigned int maxLevel)
{    
    recursiveRelativesSearch(db,list,level,humanListCount,maxLevel);
    /*
    HumanData child=list->at(humanListCount);
    //Ищем данные отца и уходим в рекурсию по его предкам.
    if(!list->at(humanListCount).father.isNull())
    {
        HumanData fatherHuman=db->getHuman(list->at(humanListCount).father.toString());
        fatherHuman.descendantsInfo=child.descendantsInfo;
        fatherHuman.descendantsInfo.append(child.getHumanInfo());
        if(!fatherHuman.uuid.isNull())
        {
            fatherHuman.recursyList=list->at(humanListCount).recursyList;
            fatherHuman.recursyList.append(humanSexMale);
            list->append(fatherHuman);
            if(level<maxLevel)
                recursiveAncestrySearch(db,list,level+1,list->size()-1,maxLevel);
        }
    }
    //Ищем данные матери и уходим в рекурсию по его предкам.
    if(!list->at(humanListCount).mother.isNull())
    {
        HumanData motherHuman=db->getHuman(list->at(humanListCount).mother.toString());
        if(!motherHuman.uuid.isNull())
        {
            motherHuman.descendantsInfo=child.descendantsInfo;
            motherHuman.descendantsInfo.append(child.getHumanInfo());
            motherHuman.recursyList=list->at(humanListCount).recursyList;
            motherHuman.recursyList.append(humanSexFemale);
            list->append(motherHuman);
            if(level<maxLevel)
                recursiveAncestrySearch(db,list,level+1,list->size()-1,maxLevel);
        }
    }
    */
}

QList<HumanData> HumansDatabaseCalc::removeHumanDuplicates(QList<HumanData> humans)
{
    QList<HumanData> newList;
    QSet<QUuid> set;
    int setSizeOld=0;
    int setSizeNew=0;
    for(int i=0;i<humans.size();i++)
    {

        set.insert(humans[i].uuid);
        setSizeNew=set.size();
        if(setSizeNew>setSizeOld)
        {
            newList.append(humans[i]);
        }
        setSizeOld=setSizeNew;
    }
    return newList;
}
