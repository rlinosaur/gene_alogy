#ifndef GEDCOMIMPORTDIALOG_H
#define GEDCOMIMPORTDIALOG_H

//#include <QtGui/QDialog>
#include <QDialog>

#include "humansdatabase.h"
#include "gedcomparser.h"

namespace Ui {
class GedcomImportDialog;
}

class GedcomImportDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit GedcomImportDialog(HumansDatabase *humansDataBase, QWidget *parent = 0);
    ~GedcomImportDialog();
    
private slots:
    void on_pushButtonOpenGedcom_clicked();

    void on_pushButtonAutoImport_clicked();

private:
    Ui::GedcomImportDialog *ui;
//    ParseGedcomStat parseGedcomFile(QString fileName);
     GedcomParser parser;
     HumansDatabase *db;

};

#endif // GEDCOMIMPORTDIALOG_H
