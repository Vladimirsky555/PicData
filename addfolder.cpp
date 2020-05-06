#include "addfolder.h"
#include "ui_addfolder.h"

#include <QMessageBox>

AddFolder::AddFolder(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFolder)
{
    ui->setupUi(this);
    setWindowTitle("Добавить папку");
}


AddFolder::~AddFolder()
{
    delete ui;
}

void AddFolder::setFolder(QString folder)
{
    this->folder = folder;
    ui->edtFolder->setText(folder);
}

QString AddFolder::getFolder()
{

    return this->folder;
}



void AddFolder::on_btnOK_clicked()
{
    folder = ui->edtFolder->text();

    if(folder == ""){
        QMessageBox::information(this, "Информация",
                                        "Вы не ввели имя новой папки!");
        return;
    }

    close();
}

void AddFolder::on_btnClose_clicked()
{
    close();
}
