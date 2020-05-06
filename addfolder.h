#ifndef ADDFOLDER_H
#define ADDFOLDER_H

#include <QDialog>

namespace Ui {
class AddFolder;
}

class AddFolder : public QDialog
{
    Q_OBJECT

    QString folder;

public:
    AddFolder(QWidget *parent = 0);
    virtual ~AddFolder();

    void setFolder(QString folder);
    QString getFolder();

private slots:
    void on_btnOK_clicked();
    void on_btnClose_clicked();

private:
    Ui::AddFolder *ui;
};

#endif // ADDFOLDER_H
