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
    connect(worker, SIGNAL(workFinished()),
            this, SLOT(finishWorker()));

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
    /* Производим инициализацию модели представления данных
         * с установкой имени таблицы в базе данных, по которому
         * будет производится обращение в таблице
         * */
        model = new Model(this);

        connect(this, SIGNAL(shutdown()),
                model, SLOT(acceptShutdown()));

        connect(this, SIGNAL(sendIndextoModel(int)),
                model, SLOT(acceptIndexfromView(int)));

        connect(this, SIGNAL(sendFolder(QString)),
                model, SLOT(acceptFolderName(QString)));

        connect(model, SIGNAL(add_folder()),
                this, SLOT(refresh_folders()));

        connect(model, SIGNAL(delete_row()),
                this, SLOT(delete_row()));

        connect(model, SIGNAL(delete_folder()),
                this, SLOT(refresh_folders()));

//        connect(model, SIGNAL(rename_folder()),
//                this, SLOT(refresh_folders()));

        model->setTable(tableName);

        /* Устанавливаем названия колонок в таблице с сортировкой данных
         * */
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
    ui->tableView->setColumnHidden(3, true);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    connect(ui->tableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(slotCurrentPic(QModelIndex)));
//    model->select();
}


//Добавляем одну фотографию в папку "Others"
void MainWindow::on_actionAddOne_triggered()
{
    if(currentFolder == ""){
        QMessageBox::information(this, "Информация", "Добавьте назвние папки!");
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "C:/picDB/", tr("Images (*.png *.xpm *.jpg)"));
    QPixmap inixmap(fileName);
    QByteArray inByteArray;
    QBuffer inBuffer(&inByteArray);
    inBuffer.open(QIODevice::WriteOnly);
    inixmap.save(&inBuffer, "JPG");
    model->insertIntoTable(currentFolder,
                       QDateTime::currentDateTime().toString("dd.MM.yyyy_hh:mm:ss.jpg"),
                       inByteArray);
    model->selectFromTable(currentFolder);
}



void MainWindow::on_actionAddMany_triggered()
{
    if(currentFolder == ""){
        QMessageBox::information(this, "Информация", "Добавьте название папки!");
        return;
    }

    QStringList filenames =
    QFileDialog::getOpenFileNames(this,
                                  tr("JPG files"),
                                  QDir::currentPath(),
                                  tr("Bitmap files (*.jpg);;All files (*.*)") );


    ui->progressBar->setRange(0, filenames.count());

    if(worker->isRunning()){
        worker->terminate();
    } else {
        worker->setFilenamesList(filenames);
        worker->setFolder(currentFolder);
        worker->start();
    }
}



void MainWindow::slotCurrentPic(QModelIndex index)
{    
    currentIndex = index.row();
    currentFolder = model->data(model->index(index.row(), 1)).toString();
    emit sendIndextoModel(currentIndex);
}

//Отображаем фотографию в отдельном окошке
void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    Card *cd = new Card(model->data(model->index(index.row(), 3)).toByteArray());
    cd->show();
}


//отображаем ту же папку после удаления
void MainWindow::delete_row()
{
    model->select();
    model->selectFromTable(currentFolder);
}

void MainWindow::refresh_folders()
{
    //Если нет папок, то не добавлять фотографии в базу
     if(model->s.getCount() == 0)
         currentFolder = "";

     refreshListFolders();
}



void MainWindow::finishWorker()
{
    QMessageBox::information(this, "Информация", "Фотографии загружены");
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
