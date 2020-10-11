#include "data_pic.h"

#include <QtSql>

Data_pic::Data_pic(QObject *parent) : QObject(parent){}

Data_pic::Data_pic(QObject *parent, QSqlQuery &qry)
{
    Q_UNUSED (parent)
    this->id = qry.value("id").toInt();
    this->pic = qry.value("pic").toByteArray();
    this->name = qry.value("name").toString();
    this->folder = qry.value("folder").toString();
}

QVariant Data_pic::Id()
{
    return this->id;
}

QByteArray Data_pic::Pic()
{
    return this->pic;
}

void Data_pic::setPic(QByteArray pic)
{
    this->pic = pic;
}

QString Data_pic::Name()
{
    return this->name;
}

void Data_pic::setName(QString name)
{
    this->name = name;
}

QString Data_pic::Folder()
{
    return this->folder;
}

void Data_pic::setFolder(QString folder)
{
    this->folder = folder;
}
