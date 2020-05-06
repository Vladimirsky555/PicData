#include "card.h"
#include "ui_card.h"

Card::Card(QByteArray arr, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Card)
{
    ui->setupUi(this);
    setWindowTitle("Просмотр");

    QPixmap outPixmap = QPixmap();
    outPixmap.loadFromData(arr);
    if(outPixmap.width() > 1366 || outPixmap.height() > 786){
        this->setGeometry(0,30,outPixmap.width()*0.3,
                          outPixmap.height()*0.3);
        ui->picLabel->setPixmap(outPixmap.scaled(outPixmap.width() * 0.3,
                                                 outPixmap.height() * 0.3));

    }else{
        this->setGeometry(0,30,outPixmap.width(),outPixmap.height());
        ui->picLabel->setPixmap(outPixmap.scaled(outPixmap.width(),
                                                 outPixmap.height()));
    }
}

Card::~Card()
{
    delete ui;
}

void Card::shutdown()
{
    close();
}


