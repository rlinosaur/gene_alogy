#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();

private slots:
    void on_pushButtonSaveAndClose_clicked();

    void on_pushButtonClose_clicked();

    void on_pushButtonChooseDbFile_clicked();

    void on_pushButtonChooseSourceDir_clicked();

private:
    Ui::OptionsDialog *ui;
    QSettings *settings;
};

#endif // OPTIONSDIALOG_H
