#include "worker.h"

#include <QFileDialog>
#include <QBuffer>
#include <QMessageBox>

Worker::Worker(QObject *parent) :
    QThread(parent)
{

}

Worker::~Worker()
{
}

void Worker::setFolder(QString folder)
{
    this->Folder = folder;
}

void Worker::setFilePathList(QStringList filePathes)
{
    this->filePathList = filePathes;
}

void Worker::setFileNameList(QStringList fileNames)
{
    this->fileNameList = fileNames;
}

void Worker::setModel(Model *model)
{
    this->model = model;
}

void Worker::addManyPictures()
{
    if(!filePathList.isEmpty() ){
        for (int i =0; i<filePathList.count(); i++){
            emit(sendToCounter(i+1));
            QPixmap inixmap(filePathList.at(i));
            QByteArray inByteArray;
            QBuffer inBuffer(&inByteArray);
            inBuffer.open(QIODevice::WriteOnly);
            inixmap.save(&inBuffer, "JPG");
            model->insertIntoTable(Folder,fileNameList.at(i),inByteArray);
        }
    }

    model->selectFromTable(Folder);
    emit workFinished(Folder);
}

void Worker::run()
{
    addManyPictures();
}
