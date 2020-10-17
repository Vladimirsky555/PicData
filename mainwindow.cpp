#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QBuffer>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QSqlQuery>

#include "imgwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Фотобиблиотека");

   setupModel("ScreenTable", QStringList() << trUtf8("id")
                                  << trUtf8("Папка")
                                  << trUtf8("Имя изображения")
                                  << trUtf8("изображение"));
   createUI();

   currentFolder = "";


    counter = new Counter();
    connect(counter, SIGNAL(sendValue(int)),
            ui->progressBar, SLOT(setValue(int)));


    worker = new Worker(this);
    worker->setModel(model);
    connect(worker, SIGNAL(workFinished(QString)),
            this, SLOT(finishWorker(QString)));
    connect(worker, SIGNAL(sendToCounter(int)),
            this, SLOT(acceptInfoForCounter(int)));

    refreshListFolders();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Обновляем список папок
void MainWindow::refreshListFolders()
{
    ui->listWidget->clear();
    int count = model->s.getCount();

    if (count > 0){
        for(int i = 0; i < count; i++){
            ui->listWidget->addItem(model->s.getFolderById(i)->getName());
        }
    }
}

void MainWindow::setupModel(const QString &tableName, const QStringList &headers)
{
        model = new Model(this);

        connect(this, SIGNAL(shutdown()),
                model, SLOT(acceptShutdown()));

        connect(this, SIGNAL(sendIndextoModel(int)),
                model, SLOT(acceptIndexfromView(int)));

        connect(this, SIGNAL(sendFolder(QString)),
                model, SLOT(acceptFolderName(QString)));

        connect(model, SIGNAL(add_folder()),
                this, SLOT(refresh_folders()));

        connect(model, SIGNAL(delete_folder()),
                this, SLOT(refresh_folders()));


        model->setTable(tableName);

        for(int i = 0, j = 0; i < model->columnCount(); i++, j++){
            model->setHeaderData(i,Qt::Horizontal,headers[j]);
        }
}

void MainWindow::createUI()
{
    ui->tableView->setModel(model);

    ui->tableView->addActions(model->allActions);
    ui->tableView->setContextMenuPolicy(Qt::ActionsContextMenu);

    ui->listWidget->addActions(model->folderActions);
    ui->listWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

    ui->tableView->setColumnHidden(0, true);
    ui->tableView->setColumnHidden(1, true);
    ui->tableView->setColumnHidden(3, true);

    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
}


//Добавляем одну фотографию в папку "Others"
void MainWindow::on_actionAddOne_triggered()
{
    if(currentFolder == ""){
        QMessageBox::information(this, "Информация", "Добавьте назвние папки!");
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),tr("Images (*.png *.xpm *.jpg)"));
    QFile f(filePath);
    QFileInfo fileInfo(f.fileName());
    QString fileName(fileInfo.fileName().remove(QRegularExpression(".(jpg|JPG|png|jpeg|bmp|ico)")));

    QPixmap inixmap(filePath);
    QByteArray inByteArray;
    QBuffer inBuffer(&inByteArray);
    inBuffer.open(QIODevice::WriteOnly);
    inixmap.save(&inBuffer, "JPG");

    model->insertIntoTable(currentFolder,fileName,inByteArray);
    model->selectFromTable(currentFolder);
}



void MainWindow::on_actionAddMany_triggered()
{
    //Поиск папок в корневой папке
    QStringList dirPathes;
    QStringList dirNames;
    QDir mDir(QFileDialog::getExistingDirectory(this, "Выбор папки", ""));

    for(QFileInfo tmp : mDir.entryInfoList())
    {
        if(tmp.fileName() != ".."){
            if(tmp.fileName() != "."){
                dirPathes.append(tmp.filePath());
                dirNames.append(tmp.fileName());
            }
        }
    }

    //Поиск фотографий в дочерних папках
    QStringList filePathes;
    QStringList fileNames;

    for(int i = 0; i < dirPathes.count(); i++){
        model->addFolder(dirNames.at(i));
        QDir fDir(dirPathes.at(i));
        fileNames.clear();
        filePathes.clear();

        for(QFileInfo tmp : fDir.entryInfoList())
        {
            if(tmp.fileName() != ".."){
                if(tmp.fileName() != "."){
                    filePathes.append(tmp.filePath());
                    fileNames.append(tmp.fileName().remove(QRegularExpression(".(jpg|JPG|png|jpeg|bmp|ico)")));
                }
            }
        }

        Worker *w = new Worker(this);
        w->setModel(model);
        connect(w, SIGNAL(workFinished(QString)),
                this, SLOT(finishWorker(QString)));

        if(w->isRunning()){
            w->terminate();
        } else {
            w->setFilePathList(filePathes);
            w->setFileNameList(fileNames);
            w->setFolder(dirNames.at(i));
            w->start();
        }
    }
}


//Отображаем фотографию в отдельном окошке
void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    Card *cd = new Card(model->data(model->index(index.row(), 3)).toByteArray());
    cd->show();
}

void MainWindow::refresh_folders()
{
    //Если нет папок, то не добавлять фотографии в базу
     if(model->s.getCount() == 0)
         currentFolder = "";

     refreshListFolders();
}

void MainWindow::finishWorker(QString dir)
{
    QMessageBox::information(this, "Информация", "Папка \""+ dir + "\" загружена");
    ui->progressBar->setValue(0);
}

void MainWindow::acceptInfoForCounter(int num)
{
    counter->setNum(num + 1);
}

void MainWindow::getInfoForCounterFromImgLoader(int num)
{
    ui->progressBar->setRange(0, num + 1);
}

void MainWindow::on_listWidget_clicked(const QModelIndex &)
{
    currentFolder = ui->listWidget->currentItem()->text();
    model->selectFromTable(currentFolder);
    emit sendFolder(currentFolder);
}

void MainWindow::on_actionAddManyByViewer_triggered()
{
    if(currentFolder == ""){
        QMessageBox::information(this, "Информация", "Добавьте название папки!");
        emit closeImgWidget();
        return;
    }

    ImgWidget *img = new ImgWidget();

    connect(this, SIGNAL(shutdown()),
            img, SLOT(shutdown()));

    connect(this, SIGNAL(closeImgWidget()),\
            img, SLOT(closeWidget()));

    connect(worker, SIGNAL(started()),
            img, SLOT(closeWidget()));

    connect(img, SIGNAL(infoForProgressBar(int)),
            this, SLOT(getInfoForCounterFromImgLoader(int)));


    img->setWorker(worker);
    img->setFolder(currentFolder);
    img->show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit shutdown();
    QWidget::closeEvent(event);
}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    currentIndex = index.row();
    currentFolder = model->data(model->index(index.row(), 1)).toString();
    emit sendIndextoModel(currentIndex);
}
