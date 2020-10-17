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
        connect(A, SIGNAL(triggered()), this, SLOT(delete_item()));
        allActions << A;
    }

    s.loadFolders();
}

Model::~Model()
{
    s.saveFolders();
}

Data_pic *Model::getItem(int id)
{
    if(id < 0 || id >= pix.size()) return 0;
    return pix.at(id);
}



void Model::acceptIndexfromView(int id)
{
    currentIndex = id;
}

void Model::acceptFolderName(QString folder)
{
    this->currentFolder = folder;
}

bool Model::delete_from_db(Data_pic *pic)
{
    QSqlQuery query;
    query.setForwardOnly(true);
    query.prepare("DELETE FROM ScreenTable WHERE id = :ID ;");
    query.bindValue(":ID", pic->Id());

    if(!query.exec()){
        qCritical() << query.lastError().databaseText().toUtf8().data();
        qCritical() << query.lastError().driverText();
        qCritical() << query.lastError().nativeErrorCode();
        return false;
    } else {
        qDebug() << "Удалил картинку" + pic->Name();
        return true;
    }
    return false;
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


void Model::delete_item()
{
    Data_pic *pic = getItem(currentIndex);
    if(!pic)return;

    int n = QMessageBox::warning(0, "Предупреждение", "Неужели удалить эту запись?",
                                 "Да", "Нет", QString(), 0, 1);
    if(!n) {
        beginRemoveRows(QModelIndex(), currentIndex, currentIndex);
        delete_from_db(pic);//Из базы удаляем
        pix.removeOne(pic);
        delete pic;
        endRemoveRows();

        pix.clear();
        selectFromTable(currentFolder);
    }
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
            "    folder,                      \n"
            "    name,                        \n"
            "    pic                          \n"
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
            Data_pic *pic = new Data_pic(this, qry);
            pix.append(pic);
        }

       setQuery(qry);
       return true;
}


bool Model::insertIntoTable(const QVariantList &data)
{
    QSqlQuery query;
    query.prepare("INSERT INTO ScreenTable (folder, name, pic)"
                  "VALUES (:folder, :name, :pic)");
    query.bindValue(":folder",      data[0].toString());
    query.bindValue(":name",        data[1].toString());
    query.bindValue(":pic",         data[2].toByteArray());

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


bool Model::removeRecords(QString folder)
{
    QSqlQuery query;
        query.prepare("DELETE FROM ScreenTable WHERE Folder = :FOLDER ;");
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






