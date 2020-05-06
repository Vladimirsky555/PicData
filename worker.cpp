#include "worker.h"

#include <QFileDialog>
#include <QBuffer>
#include <QMessageBox>

Worker::Worker(QObject *parent) :
    QThread(parent)
{

}

void Worker::setFolder(QString folder)
{
    this->Folder = folder;
}

void Worker::setFilenamesList(QStringList filenames)
{
   this->filenameList = filenames;
}


void Worker::setModel(Model *model)
{
    this->model = model;
}

void Worker::addManyPictures()
{
    if(!filenameList.isEmpty() ){
        for (int i =0;i<filenameList.count();i++){
            emit(sendToCounter(i+1));
            QPixmap inixmap(filenameList.at(i));
            QByteArray inByteArray;
            QBuffer inBuffer(&inByteArray);
            inBuffer.open(QIODevice::WriteOnly);
            inixmap.save(&inBuffer, "JPG");
            model->insertIntoTable(Folder,
                                   Folder + "_" + QString::number(i),
                                   inByteArray);
//            model->selectFromTable(Folder);
        }
    }

    model->selectFromTable(Folder);
    emit workFinished();
}

void Worker::run()
{
    addManyPictures();
}
