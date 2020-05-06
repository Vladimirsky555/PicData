#ifndef IMGLOADER_H
#define IMGLOADER_H

#include <QObject>
#include <QPixmap>

class ImgLoader : public QObject
{
    Q_OBJECT

    QStringList m_fileNameList;
    int m_width;
    QList<QByteArray> dataList;

public:
    bool flag;

public:
    explicit ImgLoader(QObject *parent = 0);

    void setFileNameList(QStringList fileNameList);
    void setWidth(int width);
    void setDataList(QList<QByteArray> arr);

signals:
    void finished();
    void sendPixmap(QPixmap pix);

private slots:
    void process();


};

#endif // IMGLOADER_H
