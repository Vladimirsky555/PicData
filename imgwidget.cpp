#include "imgwidget.h"
#include "ui_imgwidget.h"

#include <QFileDialog>
#include <QDir>
#include <QStandardPaths>
#include <QScrollBar>
#include <QLabel>
#include <QDesktopWidget>
#include <QMessageBox>



ImgWidget::ImgWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImgWidget)
{
    ui->setupUi(this);
    setWindowTitle("Предварительный просмотр");

    ui->btnAddToDB->setEnabled(false);//блокируем по умолчанию кнопку доавления в базу

    imgLoader = new ImgLoader();

    connect(imgLoader, SIGNAL(finished()),
            &m_thread, SLOT(terminate()));


    connect(&m_thread, SIGNAL(started()),
            imgLoader, SLOT(process()));

    connect(&m_thread, SIGNAL(finished()),
            imgLoader, SLOT(deleteLater()));

    //Очищаем список, после завершения просмотра
    connect(&m_thread, SIGNAL(finished()),
            this, SLOT(clearList()));

    connect(imgLoader, SIGNAL(sendPixmap(QPixmap)),
            this, SLOT(addImageLabel(QPixmap)));


    const auto screenSize = size();
    move(screenSize.width()/2 - 200, screenSize.height()/2 - size().height()/2);

}

ImgWidget::~ImgWidget()
{
    for(int i = 0; i < widgets.count(); i++){
        delete widgets.at(i);
    }
    widgets.clear();
    delete ui;
}

void ImgWidget::setWorker(Worker *worker)
{
    ui->btnAddFromTable->setEnabled(false);//запрещаем при добавлении просмотр
    this->worker = worker;
}

void ImgWidget::setFolder(QString folder)
{
    this->folder = folder;
}

void ImgWidget::acceptDataForShow(QList<Data_pic*> pix)
{
    ui->btnAddList->setEnabled(false);
    this->dataForShow = pix;
}



void ImgWidget::addImageLabel(QPixmap pixmap)
{
    QLabel* imageLabel = new QLabel(ui->scrollArea);
    widgets.append(imageLabel);
    imageLabel->setPixmap(pixmap);
    ui->verticalLayout->addWidget(imageLabel);
}

void ImgWidget::clearList()
{
    dataForShow.clear();
}

//Закрываем виджет, перед началом добавления в базу данных
void ImgWidget::closeWidget()
{
    close();
}

//Просматриваем из файловой системы
void ImgWidget::on_btnAddList_clicked()
{
    filePathList.clear();
    //При просмотре из файловой системы, открываем возможность добавлять в базу
    ui->btnAddToDB->setEnabled(true);


    imgLoader->flag = false;

    QDir mDir(QFileDialog::getExistingDirectory(this, "Выбор папки", ""));

    for(QFileInfo tmp : mDir.entryInfoList())
    {
        if(tmp.fileName() != ".."){
            if(tmp.fileName() != "."){
                filePathList.append(tmp.filePath());
                fileNameList.append(tmp.fileName().remove(QRegularExpression(".(jpg|JPG|png|jpeg|bmp|ico)")));
            }
        }
    }

    int imageWidth = ui->scrollArea->width() - 30;

    imgLoader->setFilePathList(filePathList);
    imgLoader->setWidth(imageWidth);
    imgLoader->moveToThread(&m_thread);

    m_thread.start();
}

//Добавляем в базу данных
void ImgWidget::on_btnAddToDB_clicked()
{
    if(worker->isRunning()){
        worker->terminate();
    } else {
        emit infoForProgressBar(filePathList.count());
        worker->setFilePathList(filePathList);
        worker->setFileNameList(fileNameList);
        worker->setFolder(folder);
        worker->start();
    }
}

void ImgWidget::shutdown()
{
    close();
}



//Просматриваем из таблицы
void ImgWidget::on_btnAddFromTable_clicked()
{
    ui->btnAddFromTable->setEnabled(false);
    ui->btnAddList->setEnabled(false);
    ui->btnAddList->setEnabled(false);
    if(dataForShow.count() < 1)
        QMessageBox::information(this, "Сообщение",
                                 "Вы не добавили ни одной фотографии для просмотра. "
                                 "Для выбора фотографии нужно выбрать одну из папок"
                                 " в левом столбце основного окна программы или выбрать"
                                 " папку для просмотра в меню!");

    //когда просто смотрим, то кнопки файловой системы блокируем
    ui->btnAddList->setEnabled(false);
    ui->btnAddToDB->setEnabled(false);

    imgLoader->flag = true;
    int imageWidth = ui->scrollArea->width() - 30;

    imgLoader->setDataList(dataForShow);
    imgLoader->setWidth(imageWidth);
    imgLoader->moveToThread(&m_thread);

    m_thread.start();
}







