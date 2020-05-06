#include "counter.h"

Counter::Counter(QObject *parent) : QObject(parent)
{
    this->num = num;
}

int Counter::getNum()
{
    return this->num;
}

void Counter::setNum(int num)
{
    this->num = num;
    emit sendValue(this->num);
}


