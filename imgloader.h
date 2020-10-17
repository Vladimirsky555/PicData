#ifndef IMGLOADER_H
#define IMGLOADER_H

#include <QObject>
#include <QPixmap>

#include "data_pic.h"

class ImgLoader : public QObject
{
    Q_OBJECT

    QStringList m_filePathList;
    int m_width;
    QList<Data_pic*> dataList;

public:
    bool flag;

public:
    explicit ImgLoader(QObject *parent = 0);

    void setFilePathList(QStringList filePathList);
    void setWidth(int width);
    void setDataList(QList<Data_pic *> pix);

signals:
    void finished();
    void sendPixmap(QPixmap pix);

private slots:
    void process();


};

#endif // IMGLOADER_H
