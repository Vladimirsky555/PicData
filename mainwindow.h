#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QModelIndex>

#include "model.h"
#include "card.h"
#include "counter.h"
#include "worker.h"
#include "addfolder.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Model  *model;
    int currentIndex;
    Counter *counter;
    Worker *worker;
    QStringList folders;
    QString currentFolder;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void refreshListFolders();

private:
    void setupModel(const QString &tableName, const QStringList &headers);
    void createUI();

private slots:
    void on_actionAddOne_triggered();
    void on_actionAddMany_triggered();
    void on_listWidget_clicked(const QModelIndex &);
    void refresh_folders();//слот оновления, когда удаляем через экшен модели

    //Реакция на представлении
    void on_tableView_doubleClicked(const QModelIndex &index);
    void finishWorker(QString dir);//Слот для worker
    void acceptInfoForCounter(int);
    void getInfoForCounterFromImgLoader(int);
    void on_actionAddManyByViewer_triggered();
    void on_tableView_clicked(const QModelIndex &index);

signals:
    void sendIndextoModel(int);
    void sendFolder(QString);
    void sendWorker(Worker *);
    void closeImgWidget();
    void shutdown();

private:
    Ui::MainWindow *ui;

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
