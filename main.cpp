#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(".");
    paths.append("platforms");
    paths.append("sqldrivers");
    QCoreApplication::setLibraryPaths(paths);

    QApplication a(argc, argv);
    //a.addLibraryPath(a.applicationDirPath()+"/plugins");
    //QFileInfo(argv[0]).dir().path()
   // QTextCodec *codec=QTextCodec::codecForName("UTF-8");
#if QT_VERSION >= 0x050000
    // Qt5 code
#else
    // Qt4 code
    QTextCodec *codec=QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);
#endif
    //QTextCodec::setCodecForLocale(codec);//Оказывается это даёт там в консоле кракозяблы. Ну надо же.

    MainWindow w;
    w.show();

    return a.exec();
}
