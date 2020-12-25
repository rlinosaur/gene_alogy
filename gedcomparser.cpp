#include <QFile>
#include <QRegularExpression>
#include <QRegExp>
#include <QTextCodec>
#include <QTextStream>
#include <QStringList>

#include <QDebug>


#include "gedcomparser.h"

//#define MAXGEDCOMLEVEL 10

//QRegExp regexpNotNumbers("[^0-9]");
QRegularExpression regexpNotNumbers("[^0-9]");

using namespace gedcom;

GedcomParser::GedcomParser()
{

}

void GedcomParser::clear()
{
    humanList.clear();
    familyList.clear();

}

unsigned int GedcomParser::getIndiNumber(QString hm)
{
   return hm.remove(QRegularExpression("[^0-9]")).toInt();
}

//Я думаю, что парсер надо бы сделать в отдельной функции.
ParseGedcom GedcomParser::parseGedcomFile(QString fileName)
{
    ParseGedcom stat;

    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly))
    {
        stat.setError("Ошибка открытия файла "+fileName);
        return stat;
    }
    humanList.clear();
    HumanData human;
    FamilyData family;
    QTextStream inStream(&f);
    inStream.setCodec(QTextCodec::codecForName("UTF-8"));

    unsigned int stringCounter=0;
    unsigned int humanCounter=0;
    unsigned int unknownCounter=0;    

    //Это вот убрать...не надо нам...
    //Уровень ноль - меняем текущий тэг...
    //если текущий был семья - заканчиваем семью.
    //если текущий был человек - заканчиваем человек...
    //А вообще-то...Надо ли нам что-то заканчивать? Я щитаю, что нет...
    //И сейчас вполне себе информативненько...
    //Но, конечно, считывание всех тэгов надо убрать, не надо это.
    bool humanFlag=false;//признак того, что мы щас обрабатываем человека.
    unsigned int humanLevel=0;//на самом деле всегда 0, но почему бы и нет. Уровень, на котором встретился человек.
    GedcomIndividualTag humanTag=tagIndUnknown;
    bool familyFlag=false;//определённо надо вводить режим какой-то вместо флаго.
    GedcomFamilyTag familyTag=tagFamilyUnknown;
    unsigned int familyLevel=0;



    while(!inStream.atEnd())
    {
        QString str=inStream.readLine();
        stringCounter++;
        QStringList strList=str.split(' ');
        unsigned int level=strList.at(0).toInt();
        QString tagName=strList.at(1);
        QString value;
        if(strList.count()==3)value=strList[2];
        else if (strList.count()>3)
        {
            for(int i=2;i<strList.count();i++)
            {
                value.append(" "+strList[i]);
            }
        }
        //Итак, у нас есть, текущий тэг, текущий уровень, текущее значение...пробуем.



        //Вычисляем конец данных о человеке
        if(humanFlag==true && level<=humanLevel)
        {
            humanList.append(human);
            humanFlag=false;
            humanCounter++;
        }

        //Вычисляем конец данных о семье
        if(familyFlag==true && level<=familyLevel)
        {
            familyList.append(family);
            familyFlag=false;
        }

        if(level<=humanLevel)
        {
             GedcomMainTag tagMain=stat.getMainTagByName(tagName);
            //Вычисляем начало данных о человеке
            if(tagMain==tagMainIdentity && value=="INDI")
            {
                humanFlag=true;
                humanLevel=level;
                humanTag=tagIndUnknown;
                human.clear();
                human.gedcomNumber=strList[1].remove(regexpNotNumbers).toInt();

            }
            //Вычисляем начало данных о семье
            if(tagMain==tagMainIdentity && value=="FAM")
            {
                familyFlag=true;
                familyLevel=level;
                familyTag=tagFamilyUnknown;
                family.clear();
            }

            switch(tagMain)
            {
                case tagMainUnknown:
                    unknownCounter++;
                break;
            default:
                break;
            }
        }


        //вычисляем данные о человеке
        if(humanFlag==true && level==humanLevel+1)
        {
            //проверка на имя фамилиё и всё такое прочее.
            GedcomIndividualTag iTag;
            iTag = ParseGedcom::getIndividualTagByName(tagName);
            humanTag=iTag;
            switch(iTag)
            {
                case tagIndName:
                    human.fullName=value;
                break;               
                case tagIndSex:
                if(value=="M")human.sex=humanSexMale;
                else if (value=="F") human.sex=humanSexFemale;
                else human.sex=humanSexUndefined;
                break;
            case tagIndNote:
                human.note.append(value);
                break;
            case tagIndOccupation:
                human.occupation=value;
                break;
            case tagIndDescription:
                human.description=value;
                break;
            default:
                break;
            }
        }

        if(humanFlag==true && level==humanLevel+2)
        {
            //qDebug()<<"Найдена информация человека второго уровня, "<<tagName<<", "<<humanTag;
            if(humanTag==tagIndBirth)
            {
                GedcomCommonTag iTag;
                iTag=ParseGedcom::getCommonTagByName(tagName);
              //  qDebug()<<"Найдена информация о рождении "<<tagName<<","<<value;
                switch(iTag)
                {
                    case tagCommonDate:
                    human.birthDate=value;
                    break;
                case tagCommonPlace:
                    human.birthPlaceId=value;
                    break;
                default:
                    break;
                }
            }
            if (humanTag==tagIndName)
            {
                GedcomNameTag iTag;
                iTag=ParseGedcom::getNameTagByName(tagName);
                switch(iTag)
                {
                    case tagGivenName:
                    human.givenName=value;
                    break;
                case tagSurName:
                    human.surName=value;
                    break;
                case tagMarName:
                    human.marName=value;
                    break;
                default:
                    break;
                }
            }
            if(humanTag==tagIndDeath)
            {
                GedcomCommonTag iTag;
                iTag=ParseGedcom::getCommonTagByName(tagName);

                switch(iTag)
                {
                    case tagCommonDate:
                    human.deathDate=value;                    
                    break;
                case tagCommonCause:
                    human.deathCause=value;
                    break;
                case tagCommonAge:
                    human.deathAge=value;
                    break;
                default:
                    break;
                }
            }
            if(humanTag==tagIndNote)
            {
                GedcomCommonTag iTag;
                iTag=ParseGedcom::getCommonTagByName(tagName);
                if(iTag==tagCommonConc)
                {
                    qDebug()<<"ДОБАВКА! ДОЛЖНА БЫТЬ ХОТЬ ОДНА!";
                    human.note.append(value);
                }
            }
        }

        //Вычисляем данные о семье (уровень 1)
        if(familyFlag==true && level==familyLevel+1)
        {
            //qDebug()<<"Внутренний семейный тэг "<<strList.at(1);
            GedcomFamilyTag fTag;
            fTag=ParseGedcom::getFamilyTagByName(tagName);
            familyTag=fTag;
            switch(fTag)
            {
                case tagFamilyChild:
                    family.children.append(value.remove(regexpNotNumbers).toInt());                   
                    break;
                case tagFamilyHusband:
                    family.partners.append(value.remove(regexpNotNumbers).toInt());
                    break;
                case tagFamilyWife:
                    family.partners.append(value.remove(regexpNotNumbers).toInt());
                    break;                
                default:
                    break;
            }
        }

        if(familyFlag==true && level==familyLevel+2)
        {
            if(familyTag==tagFamilyMarriageData)
            {
                GedcomCommonTag cTag=ParseGedcom::getCommonTagByName(tagName);
                switch(cTag)
                {
                case tagCommonDate:
                    family.marriageDate=value;
                    break;
                case tagCommonPlace:
                    family.marriagePlace=value;
                    break;
                case tagCommonNote:
                    family.note.append(value);
                    break;
                default:
                    break;
                }
            }
        }
        if(familyFlag==true && level>familyLevel+2)
        {
            //Из известных это только conc для Note.
            GedcomCommonTag cTag=ParseGedcom::getCommonTagByName(tagName);
            if(cTag==tagCommonConc) family.note.append(value);
            //А остальные не рассматриваем. Так-то.
        }
        //if(currentLevel<MAXGEDCOMLEVEL) currentTag[currentLevel]=tag;
        //далее очень просто..Смотрим уровень вложенности...и всё такое...
        //заголовок пропускаем...
    }
    f.close();


    for(int i=0;i<familyList.count();i++)
    {
        //Добавляем детей каждому из партнёров в класс...Ну а как иначе-то...Никак иначе-то...
        for (int partnersCount=0;partnersCount<familyList[i].partners.count();partnersCount++)
        {
            for(int j=0;j<humanList.count();j++)
            {
                if(humanList[j].gedcomNumber==familyList[i].partners[partnersCount])
                {
                    for (int k=0;k<familyList[i].children.count();k++)
                        //ВНИМАНИЕ!ВНИМАНИЕ!ИЩИ И ВСТАВЛЯЙ СЮДА СРАЗУ ИДЕНТИФИКАТОРЫ ДЕТЕЙ! Хотя...не надо, детям всё равно родители нужны, а родителям дети - вряд ли..разве что в отдельной таблице.
                        humanList[j].children.append(familyList[i].children[k]);

                    //А мог бы прям тут искать детям идентификаторы и вставлять уже их, к примеру...
                    break;//вроде как не надо больше искать.
                }
            }
        }

        //В семью для каждого партнёра ищем его репрезентацию в реальности и указываем на неё. Да...

        //для каждого партнёра ищем его репрезентацию. как-то так.
        //Добавляем мужей всяких там и жён...а как иначе
        familyList[i].partnersCounts.clear();
        for (int j=0;j<familyList[i].partners.count();j++)
        {
            int count=-1;
            for(int k=0;k<humanList.count();k++)
            {
                if(humanList[k].gedcomNumber==familyList[i].partners[j])
                {
                    //нашли одного из партнёров...добавляем ему всех партнеров...
                    count=k;
                    break;
                }               
            }
            //Добавляем uuid к информации о родителях, что уж прям сразу уж...ага!
            if(count>=0)
            {
                if(humanList[count].sex==humanSexMale)
                    familyList[i].husband=humanList[count].uuid;
                else
                    familyList[i].wife=humanList[count].uuid;
            }
            familyList[i].partnersCounts.append(count);//на каждого партнёра должно быть по штуке. Думаю так.
            //а тут мы в спец.лист закидываем то, что закидываем.
         }
    }
    //Вот. Добавили всех детей. Так-то. Точнее их цифры...Теперь надо добавить детям линки на родителей...
    for(int i=0;i<humanList.count();i++)
    {
        for(int j=0;j<humanList[i].children.count();j++)
        {
            //для каждого ребёнка ищем его класс и вставляем линк на родителя..
            for(int k=0;k<humanList.count();k++)
            {
                if(humanList[k].gedcomNumber==humanList[i].children[j])
                {
                    //Значит это наше дитё.
                    if(humanList[i].sex==humanSexMale)
                    {
                        humanList[k].fatherListCount=i;//сомнения есть в неоходимости этих данных, впрочем..ладно.
                        humanList[k].father=humanList[i].uuid;
                     }
                    else if (humanList[i].sex==humanSexFemale)
                    {
                        humanList[k].motherListCount=i;//Просто не хочу хранить указатель на список. Да и счётчик тож как-то не так.
                        //uuid понадёжнее будет, хотя и гораздо медленнее...
                        humanList[k].mother=humanList[i].uuid;
                    }
                }
            }
        }
    }
    stat.stringCount=stringCounter;
    stat.humanCount=humanCounter;
    stat.unknownCount=unknownCounter;
    return stat;
}

