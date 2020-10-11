#include "application.h"

#include <QSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QDebug>

Application::Application(int argc, char *argv[]) :
    QApplication(argc,argv)
{
    connectToDataBase();
}

void Application::connectToDataBase()
{
    /* Перед подключением к базе данных производим проверку на её существование.
     * В зависимости от результата производим открытие базы данных или её восстановление
     * */
//    if(!QFile("C:/DB/Screen.db").exists()){
    if(!QFile("Pic.db").exists()){
        this->restoreDataBase();
    } else {
        this->openDataBase();
    }
}



bool Application::openDataBase()
{
    /* База данных открывается по заданному пути
        * и имени базы данных, если она существует
        * */

       db = QSqlDatabase::addDatabase("QSQLITE");
       db.setHostName("ScreenDataBase");
//       db.setDatabaseName("C:/DB/Screen.db");
       db.setDatabaseName("Pic.db");
       if(db.open()){
           return true;
       } else {
           return false;
       }
}

bool Application::restoreDataBase()
{
    // Если база данных открылась ...
       if(this->openDataBase()){
           // Производим восстановление базы данных
           return (this->createTable()) ? true : false;
       } else {
           qDebug() << "Не удалось восстановить базу данных";
           return false;
       }
       return false;
}

void Application::closeDataBase()
{
    db.close();
}

bool Application::createTable()
{
    /* В данном случае используется формирование сырого SQL-запроса
     * с последующим его выполнением.
     * */
    QSqlQuery query;
    if(!query.exec( "CREATE TABLE  ScreenTable  ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "folder   VARCHAR(255)    NOT NULL,"
                    "name     VARCHAR(255)    NOT NULL,"
                    "pic      BLOB            NOT NULL"
                    " )"
                    )){
        qDebug() << "DataBase: error of create " << "ScreenTable";
        qDebug() << query.lastError().text();
        return false;
    } else {
        return true;
    }
    return false;
}
