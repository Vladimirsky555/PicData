#ifndef IMGWIDGET_H
#define IMGWIDGET_H

#include <QWidget>
#include <QThread>
#include <QImage>
#include <QPixmap>

#include "imgloader.h"
#include "worker.h"
#include "model.h"
#include "counter.h"
#include "data_pic.h"

namespace Ui {
class ImgWidget;
}

class ImgWidget : public QWidget
{
    Q_OBJECT

    QThread m_thread;
    ImgLoader *imgLoader;
    QList<Data_pic*> dataForShow;
    QList<QWidget*> widgets;
    QStringList filePathList;
    QStringList fileNameList;
    Worker *worker;
    QString folder;

public:
    ImgWidget(QWidget *parent = 0);
    virtual ~ImgWidget();

    void setWorker(Worker *);
    void setFolder(QString folder);
    void acceptDataForShow(QList<Data_pic*> pix);

private slots:
    void on_btnAddList_clicked();//просмотр из файловой системы
    void on_btnAddFromTable_clicked();//просмотр из таблицы
    void addImageLabel(QPixmap pixmap);
    void clearList();
    void closeWidget();
    void on_btnAddToDB_clicked();
    void shutdown();

signals:
    void infoForProgressBar(int);

private:
    Ui::ImgWidget *ui;
};

#endif // IMGWIDGET_H
