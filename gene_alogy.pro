#-------------------------------------------------
#
# Project created by QtCreator 2016-04-04T10:10:27
#
#-------------------------------------------------

QT       += core gui sql qml quick quickwidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5) QT += core5compat

TARGET = gene_alogy
TEMPLATE = app

#INCLUDEPATH += C:\Qt\4.8.6\include
#INCLUDEPATH += C:\Qt\4.8.6\include\Qt


SOURCES += main.cpp\
        mainwindow.cpp \
    humandata.cpp \
    gedcomimportdialog.cpp \
    gedcomparser.cpp \
    parsegedcom.cpp \
    humansdatabase.cpp \
    humansearchdialog.cpp \
    humaneditdialog.cpp \
    recordeditdialog.cpp \
    placeeditdialog.cpp \
    marriageeditdialog.cpp \
    sourceeditdialog.cpp \
    sourcesearchdialog.cpp \
    placesearchdialog.cpp \
    recordsearchdialog.cpp \
    humansdatabasecalc.cpp \
    visualizedialog.cpp \
    humannode.cpp \
    visualizewidget.cpp \
    tablerawdatedelegate.cpp \
    humanplacedialog.cpp \
    geneoptions.cpp \
    optionsdialog.cpp \
    options.cpp \
    recordsourceeditdialog.cpp \
    mapdialog.cpp

HEADERS  += mainwindow.h \
    humandata.h \
    gedcomimportdialog.h \
    gedcomparser.h \
    parsegedcom.h \
    humansdatabase.h \
    defines.h \
    humansearchdialog.h \
    humaneditdialog.h \
    recordeditdialog.h \
    placeeditdialog.h \
    marriageeditdialog.h \
    sourceeditdialog.h \
    sourcesearchdialog.h \
    placesearchdialog.h \
    recordsearchdialog.h \
    humansdatabasecalc.h \
    visualizedialog.h \
    humannode.h \
    visualizewidget.h \
    tablerawdatedelegate.h \
    humanplacedialog.h \
    geneoptions.h \
    optionsdialog.h \
    options.h \
    recordsourceeditdialog.h \
    mapdialog.h

FORMS    += mainwindow.ui \
    gedcomimportdialog.ui \
    humansearchdialog.ui \
    humaneditdialog.ui \
    recordeditdialog.ui \
    placeeditdialog.ui \
    marriageeditdialog.ui \
    sourceeditdialog.ui \
    sourcesearchdialog.ui \
    placesearchdialog.ui \
    recordsearchdialog.ui \
    visualizedialog.ui \
    humanplacedialog.ui \
    optionsdialog.ui \
    recordsourceeditdialog.ui \
    mapdialog.ui

RESOURCES += resources.qrc


OTHER_FILES += \
    database.txt \
    find_tasks.txt

DISTFILES += \
    todo.txt

TRANSLATIONS = languages/gene_alogy_translation_en.ts \
               languages/gene_alogy_translation_es.ts \
                languages/gene_alogy_translation_ru.ts

