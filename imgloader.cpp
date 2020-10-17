#include "imgloader.h"


ImgLoader::ImgLoader(QObject *parent) :
    QObject(parent)
{
   this->flag = false;
}

void ImgLoader::setFilePathList(QStringList filePathList)
{
    m_filePathList = filePathList;
}

void ImgLoader::setWidth(int width)
{
    m_width = width;
}

void ImgLoader::setDataList(QList<Data_pic*> pix)
{
    this->dataList = pix;
}

void ImgLoader::process()
{
    if(!flag){
        for (QString fileName : m_filePathList)
        {
            QPixmap pix(fileName);
            emit sendPixmap(pix.scaledToWidth(m_width));
        }
    } else {
        for(int i = 0; i < dataList.count(); i++){
            QPixmap p;
            if(p.loadFromData(dataList.at(i)->Pic(),"JPG")){
                emit sendPixmap(p.scaledToWidth(m_width));
            }
        }
    }

    emit finished();
}


