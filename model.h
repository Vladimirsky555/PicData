#ifndef MODEL_H
#define MODEL_H

#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QtSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QDate>
#include <QDebug>

#include "data_pic.h"

class QAction;


/************************************************/
class Folder : public QObject {
    Q_OBJECT

    QString name;

public:
    Folder(QObject *parent = 0);
    Folder(QByteArray data, QObject *parent = 0);
    virtual ~Folder(){}

    QString getName();
    void setName(QString);
    QByteArray saveIt();
};
/************************************************/
class StorageFolders : public QObject {
    Q_OBJECT

    QList<Folder*> folders;

public:
    StorageFolders(QObject *parent = 0);
    virtual ~StorageFolders(){}

    void loadFolders();
    void saveFolders();

    Folder *getFolderById(int id);
    int getCount();
    void addFolder(QString);
    void addFolderByObject(Folder *f);
    void deleteFolder(int);
    bool deleteFolderByName(QString);
};
/************************************************/

class Model : public QSqlTableModel
{
    Q_OBJECT

    QList<Data_pic*> pix;
    QString currentFolder;

public:
    StorageFolders s;
    int currentIndex;

public:
    Model(QObject *parent = 0);
    virtual ~Model();

    //Список экшенов для предоставления доступа представлению
public:
    //Экшены таблицы
    QList<QAction*> allActions;
    QAction *actDeleteRow;
    QAction *actshowPic;

    //Экшены listWidget
    QList<QAction*> folderActions;
    QAction *actDeleteFolder;
    QAction *actOpenViewer;
    QAction *actAddFolder;
    QAction *actRenameFolder;

public:
    Data_pic *getItem(int id);
    void connectToDataBase();
    bool insertIntoTable(const QVariantList &data);      // Добавление записей в таблицу
    bool insertIntoTable(const QString &folder, const QString &name, const QByteArray &pic);
    bool removeRecords(QString folder);
    bool selectFromTable(QString folder);

    void addFolders(QStringList folders);
   void addFolder(QString folder);

public slots:
    void acceptIndexfromView(int id);
    void acceptFolderName(QString);

private slots:
    bool delete_from_db(Data_pic* pic);

protected slots:
    void on_addFolder();
    void delete_item();
    void on_delete_folder();
    void on_show_pic();
    void on_show_viewer();
    void acceptShutdown();

signals:
    void add_folder();
    void delete_folder();
    void shutdown();



};

#endif
