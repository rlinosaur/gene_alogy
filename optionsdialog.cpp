#include <QDebug>
#include <QFileDialog>

#include "options.h"
#include "optionsdialog.h"
#include "ui_optionsdialog.h"

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);


    settings = new QSettings(DEFAULT_CONFIG_NAME,QSettings::IniFormat);

    settings->beginGroup(CONFIG_GROUP_DATABASE);
    ui->lineEditDbFile->setText(settings->value(CONFIG_DATABASE_FILENAME).toString());
    settings->endGroup();
    settings->beginGroup(CONFIG_GROUP_SOURCES);
    ui->lineEditSourceDir->setText(settings->value(CONFIG_SOURCES_PATH).toString());
    settings->endGroup();
}

OptionsDialog::~OptionsDialog()
{
    delete ui;    
    delete settings;
}

void OptionsDialog::on_pushButtonSaveAndClose_clicked()
{

    //Just a sample
    settings->beginGroup("Database");
    settings->setValue("FileName",ui->lineEditDbFile->text());
    settings->endGroup();

    settings->beginGroup("Sources");
    settings->setValue("Path",ui->lineEditSourceDir->text());    
    settings->endGroup();
    optionsData.sourceDirectory=ui->lineEditSourceDir->text();
    accept();
}

void OptionsDialog::on_pushButtonClose_clicked()
{
    reject();
}

void OptionsDialog::on_pushButtonChooseDbFile_clicked()
{
    //Open Choose Directory Dialog
    QString dbFile = QFileDialog::getOpenFileName(this,"Выбор файла базы данных...",ui->lineEditDbFile->text(),"Database (*.db)",0,QFileDialog::DontUseNativeDialog);
    if (dbFile.isEmpty()) return;
    //ui->labelInfo->setText("ВНИМАНИЕ! Перезапустите программу для открытия нового файла базы данных.");
    ui->lineEditDbFile->setText(dbFile);

}

void OptionsDialog::on_pushButtonChooseSourceDir_clicked()
{
    QString sourceDir = QFileDialog::getExistingDirectory(this,"Выбор папки источников...",ui->lineEditSourceDir->text());
    if (sourceDir.isEmpty()) return;
    ui->lineEditSourceDir->setText(sourceDir);
}
