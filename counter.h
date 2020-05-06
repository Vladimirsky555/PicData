#ifndef COUNTER_H
#define COUNTER_H

#include <QObject>

class Counter : public QObject
{
    Q_OBJECT

    int num;

public:
    explicit Counter(QObject *parent = 0);

    int getNum();


signals:
    void sendValue(int);


public slots:
    void setNum(int);

};

#endif // COUNTER_H
