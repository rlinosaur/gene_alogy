#ifndef PARSEGEDCOMSTAT_H
#define PARSEGEDCOMSTAT_H

#include <QString>
#include <QStringList>

/**
 * @brief The ParseGedcomStat class Содержит информацию о том, как прошла обработка файла Gedcom
 */
namespace gedcom
{

enum GedcomFamilyTag
{
    //а ведь могут же и повторяться они...гм...
    tagFamilyHusband,
    tagFamilyWife,
    tagFamilyChild,
    tagFamilyMarriageData,
    tagFamilyRin,
    tagFamilyUid,    
    tagFamilyUnknown
};
//Тэги, которые связаны с человеком.
enum GedcomIndividualTag
{
    tagIndName,
    tagIndSex,
    tagIndRin,
    tagIndUid,
    tagIndBirth,
    tagIndDeath,
    tagIndFamChild,//link to a family where the person is a child
    tagIndFamSpouse,//link to a family where the person is a spouse
    tagIndResidence,
    tagIndNote,
    tagIndOccupation,//чем занимался
    tagIndDescription,//Редкий тэг. Описание какое-то.
    tagIndUnknown
};
//Тэги с информацией заголовка (нам не надо).
enum GedcomHeadTag
{
    tagHeadGEDC,
    tagHeadVersion,
    tagHeadCharset,
    tagHeadLanguage,
    tagHeadForm,
    tagHeadUnknown
};
//Тэги, которые бывают в корне
enum GedcomMainTag
{
    tagMainIdentity,
    tagMainHead,    
    tagMainUnknown
};
//Общие тэги, например дата, место, возраст и так далее.
enum GedcomCommonTag
{
    tagCommonDate,
    tagCommonPlace,
    tagCommonAge,
    tagCommonCause,
    tagCommonAddress,
    tagCommonPhone,
    tagCommonNote,//Заметка
    tagCommonConc,//Это когда заметка длинная.Обычно на уровень ниже Note.
    tagCommonUnknown
};
//Тэги, которые встречаются в имени (может потом объединю с общими, а может нет.
enum GedcomNameTag
{
    tagGivenName,
    tagSurName,
    tagMarName,
    tagUnknownName
};

enum gedcomAddressTag
{
    tagAddressString1,
    tagAddressCity,
    tagAddressCountry,
    tagAddressPost,
    tagAddressState,
    tagAddressUnknown
};
}

class ParseGedcom
{
public:
    ParseGedcom();

    void setError(QString errorText);    
    QString error;///текст ошибки

    unsigned int stringCount;///Количество обработанных строк
    unsigned int humanCount;
    unsigned int unknownCount;

    QStringList messages;//важные и не очень сообщения (а пусть будут).

    static gedcom::GedcomFamilyTag getFamilyTagByName(QString gedcomTagName);
    static gedcom::GedcomIndividualTag getIndividualTagByName(QString gedcomTagName);

    static gedcom::GedcomMainTag getMainTagByName(QString gedcomTagName);

    static gedcom::GedcomCommonTag getCommonTagByName(QString gedcomTagName);
    static gedcom::GedcomNameTag getNameTagByName(QString gedcomTagName);
    static gedcom::gedcomAddressTag getAddressTagByName(QString gedcomTagName);
};




#endif // PARSEGEDCOMSTAT_H
