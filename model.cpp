#include "model.h"

#include <QAction>
#include <QPixmap>
#include <QDataStream>
#include <QBuffer>
#include <QFileDialog>
#include <QMessageBox>

#include "card.h"
#include "imgwidget.h"
#include "addfolder.h"


Model::Model(QObject *parent) :
    QSqlTableModel(parent)
{
    setEditStrategy(OnFieldChange);

    //Экшены папок
    {
        QAction *A = actOpenViewer = new QAction(tr("Просмотр"), this);
        QPixmap p(":/images/openViewer.png");
        A->setIcon(QIcon(p));
        actOpenViewer->setShortcut(tr("Ctrl+O"));
        connect(A, SIGNAL(triggered()), this, SLOT(on_show_viewer()));
        folderActions << A;
    }{
        QAction *A = actAddFolder = new QAction(QIcon(":/images/addFolder.png"),tr("Добавить папку"), this);
        actAddFolder->setShortcut(tr("Ctrl+A"));
        connect(A, SIGNAL(triggered()), this, SLOT(on_addFolder()));
        folderActions << A;
    }{
        QAction *A = actDeleteFolder = new QAction(tr("Удалить папку"), this);
        QPixmap p(":/images/deleteFolder.png");
        A->setIcon(QIcon(p));
        actDeleteFolder->setShortcut(tr("Ctrl+D"));
        connect(A, SIGNAL(triggered()), this, SLOT(on_delete_folder()));
        folderActions << A;
    }
//    {
//            QAction *A = actSelectAll = new QAction(tr("Вся таблица"), this);
//            QPixmap p(":/images/showAllTable.png");
//            A->setIcon(QIcon(p));
//            actSelectAll->setShortcut(tr("Ctrl+Q"));
//            connect(A, SIGNAL(triggered()), this, SLOT(on_selectAll()));
//            folderActions << A;
//        }


    //Экшены таблицы
    {
        QAction *A = actshowPic = new QAction(tr("Просмотр"), this);
        QPixmap p(":/images/showPic.png");
        A->setIcon(QIcon(p));
        actshowPic->setShortcut(tr("Ctrl+S"));
        connect(A, SIGNAL(triggered()), this, SLOT(on_show_pic()));
        allActions << A;
    }{
        QAction *A = actDeleteRow = new QAction(tr("Удалить"), this);
        QPixmap p(":/images/delPic.png");
        A->setIcon(QIcon(p));
        actDeleteRow->setShortcut(tr("Ctrl+Z"));
        connect(A, SIGNAL(triggered()), this, SLOT(on_delete_row()));
        allActions << A;
    }

//    {
//        QAction *A = actRenameFolder = new QAction(this);
//        A->setText(tr("Переименовать папку"));
//        connect(A, SIGNAL(triggered()), this, SLOT(on_renameFolder()));
//        folderActions << A;
//    }


    s.loadFolders();
}

Model::~Model()
{
    s.saveFolders();
}



void Model::acceptIndexfromView(int id)
{
    currentIndex = id;
}

void Model::acceptFolderName(QString folder)
{
    this->currentFolder = folder;
}

void Model::on_addFolder()
{
    AddFolder d;
    d.exec();

    QString folder = d.getFolder();

    if(folder != ""){
        s.addFolder(folder);
    } else {
        return;
    }

    emit add_folder();
}

//void Model::on_renameFolder()
//{
//    AddFolder d;
//    d.setFolder(currentFolder);
//    d.exec();

//    for(int i = 0; i < s.getCount(); i++){
//        if(s.getFolderById(i)->getName() == currentFolder){
//            s.getFolderById(i)->setName(d.getFolder());
//        }
//    }

//    emit rename_folder();
//}


void Model::on_delete_row()
{
    removeRow(currentIndex);
    emit delete_row();
}

void Model::on_delete_folder()
{
    QMessageBox b;
    b.setText("Удаление папки");
    b.setInformativeText("Вы уверены, что желаете удалить папку?");
    b.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    b.setDefaultButton(QMessageBox::Cancel);
    b.exec();


    if(currentFolder != ""){
        removeRecords(currentFolder);//удаляем из базы
        s.deleteFolderByName(currentFolder);
    } else {
        return;
    }

    select();
    emit delete_folder();
}

//void Model::on_selectAll()
//{
//    select();
//}


void Model::on_show_pic()
{
//    QString folder = data(index(currentIndex, 1)).toString();
//    QString name = data(index(currentIndex, 2)).toString();
    QByteArray arr = data(index(currentIndex, 3)).toByteArray();
    Card *cd =  new Card(arr);

    connect(this, SIGNAL(shutdown()),
            cd, SLOT(shutdown()));

    cd->show();
}

void Model::on_show_viewer()
{
    ImgWidget *img = new ImgWidget();
    img->acceptDataForShow(pix);

    connect(this, SIGNAL(shutdown()),
            img, SLOT(shutdown()));

    pix.clear();
    img->show();
}

void Model::acceptShutdown()
{
    emit shutdown();
}






//Функци взаимодействия с базой данных
/************************************************/
bool Model::selectFromTable(QString folder)
{
    pix.clear();
    QString queryText =
            "SELECT                           \n"
            "    id,                          \n"
            "    Folder,                      \n"
            "    Name,                        \n"
            "    Pic                          \n"
            "FROM ScreenTable                 \n"
            "WHERE Folder = :FOLDER       ;   \n";

    QSqlQuery qry;
    qry.prepare(queryText);
    qry.bindValue(":FOLDER", folder);


        if(!qry.exec()){
            qCritical() << qry.lastError().databaseText().toUtf8().data();
            return false;
        }

        while(qry.next()){
            QByteArray arr = qry.value("Pic").toByteArray();
            pix.append(arr);
        }

       setQuery(qry);
       return true;
}


bool Model::insertIntoTable(const QVariantList &data)
{
    /* Запрос SQL формируется из QVariantList,
     * в который передаются данные для вставки в таблицу.
     * */
    QSqlQuery query;
    /* В начале SQL запрос формируется с ключами,
     * которые потом связываются методом bindValue
     * для подстановки данных из QVariantList
     * */
    query.prepare("INSERT INTO ScreenTable (Folder, Name, Pic)"
                  "VALUES (:Folder, :Name, :Pic)");
    query.bindValue(":Folder",      data[0].toString());
    query.bindValue(":Name",        data[1].toString());
    query.bindValue(":Pic",         data[2].toByteArray());

    // После чего выполняется запросом методом exec()
    if(!query.exec()){
        qDebug() << "error insert into " << "ScreenTable";
        qDebug() << query.lastError().text();
        return false;
    } else {
        return true;
    }
    return false;
}


bool Model::insertIntoTable(const QString &folder, const QString &name, const QByteArray &pic)
{
    QVariantList data;
    data.append(folder);
    data.append(name);
    data.append(pic);

    if(insertIntoTable(data))
        return true;
    else
        return false;
}

bool Model::removeRecord(const int id)
{
    QSqlQuery query;
        query.prepare("DELETE FROM ScreenTable "
                      "WHERE id= :ID ;");
        query.bindValue(":ID", id);
        if(!query.exec()){
            qDebug() << "error delete row " << "ScreenTable";
            qDebug() << query.lastError().text();
            return false;
        } else {
            return true;
        }
        return false;
}

bool Model::removeRecords(QString folder)
{
    QSqlQuery query;
        query.prepare("DELETE FROM ScreenTable "
                      "WHERE Folder = :FOLDER ;");
        query.bindValue(":FOLDER", folder);
        if(!query.exec()){
            qDebug() << "error delete row " << "ScreenTable";
            qDebug() << query.lastError().text();
            return false;
        } else {
            return true;
        }
        return false;
}





/************************************************/
Folder::Folder(QObject *parent)  :
    QObject(parent){}

Folder::Folder(QByteArray data, QObject *parent) :
    QObject(parent)
{
    QDataStream str(data);
    str >> this->name;
}

QString Folder::getName()
{
    return this->name;
}

void Folder::setName(QString folder)
{
    this->name = folder;
}

QByteArray Folder::saveIt()
{
    QByteArray arr;
    QDataStream str(&arr, QIODevice::WriteOnly);
    str << this->name;
    return arr;
}



/************************************************/
StorageFolders::StorageFolders(QObject *parent) :
    QObject(parent){}


Folder *StorageFolders::getFolderById(int id)
{
    return folders.at(id);
}


int StorageFolders::getCount()
{
    return folders.count();
}

void StorageFolders::addFolder(QString folder)
{
    for(int i = 0; i < folders.count(); i++){
        if(folders.at(i)->getName() == folder)
            return;
    }
    Folder *F = new Folder();
    F->setName(folder);
    folders.append(F);
    return;
}

void StorageFolders::addFolderByObject(Folder *f)
{
    folders.append(f);
}

void StorageFolders::deleteFolder(int id)
{
    folders.removeAt(id);
}

bool StorageFolders::deleteFolderByName(QString folder)
{
    for(int i = 0; i < folders.count(); i++){
        if(folders.at(i)->getName() == folder){
            folders.removeOne(folders.at(i));
            return true;
        }
    }
    return false;
}


void StorageFolders::loadFolders()
{
    QFile f("Folders.db");
       if (!f.exists()) return;

       f.open(QFile::ReadOnly);
       QByteArray data = f.readAll();
       f.close();

       QDataStream str(data);

       while (!str.atEnd()) {
           QByteArray arr;
           str >> arr;

           Folder *F = new Folder(arr);
           addFolderByObject(F);
       }

}

void StorageFolders::saveFolders()
{
    QFile f("Folders.db");
        f.open(QFile::WriteOnly|QFile::Truncate);

        QDataStream str(&f);

        for (int i = 0; i < folders.count(); i++)
            str << folders.at(i)->saveIt();

        f.close();
}






