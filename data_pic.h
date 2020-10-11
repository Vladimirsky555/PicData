#ifndef DATA_PIC_H
#define DATA_PIC_H

#include <QObject>
#include <QVariant>

class QSqlQuery;

class Data_pic : public QObject
{
    Q_OBJECT

    QVariant id;
    QByteArray pic;
    QString name;
    QString folder;

public:
    Data_pic(QObject *parent = 0);
    Data_pic(QObject *parent, QSqlQuery &qry);

    QVariant Id();

    QByteArray Pic();
    void setPic(QByteArray pic);

    QString Name();
    void setName(QString name);

    QString Folder();
    void setFolder(QString folder);

signals:

public slots:
};

#endif // DATA_PIC_H
