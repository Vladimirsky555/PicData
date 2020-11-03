#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>
#include <QFileSystemModel>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QMutex>

#include "model.h"

class Worker : public QObject, public QRunnable
{
    Q_OBJECT

    QString Folder;
    QStringList filePathList;
    QStringList fileNameList;
    Model *model;

public:
    QMutex *mutex;

public:
    Worker(QObject *parent, QMutex* mutex = nullptr);
    virtual ~Worker(){}

    void setFolder(QString folder);
    void setFilePathList(QStringList filePathes);
    void setFileNameList(QStringList fileNames);
    void setModel(Model *model);
    void addManyPictures();

signals:
    void workFinished(QString);
    void sendToCounter(int);
    void closeWidget();

    // QRunnable interface
public:
    virtual void run();
};

#endif // WORKER_H
