#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>
#include <QFileSystemModel>
#include <QDir>
#include <QDateTime>
#include <QDebug>

#include "model.h"

class Worker : public QThread
{
    Q_OBJECT

    QString Folder;
    QStringList filePathList;
    QStringList fileNameList;
    Model *model;


public:
    Worker(QObject *parent = 0);
    virtual ~Worker(){}

    void setFolder(QString folder);
    void setFilePathList(QStringList filePathes);
    void setFileNameList(QStringList fileNames);
    void setModel(Model *model);

    void addManyPictures();


signals:
    void workFinished();
    void sendToCounter(int);

    // QThread interface
protected:
    virtual void run();
};

#endif // WORKER_H
