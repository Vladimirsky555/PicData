#include "card.h"
#include "ui_card.h"

#include <QDesktopWidget>

Card::Card(QByteArray arr, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Card)
{
    ui->setupUi(this);
    setWindowTitle("Просмотр");

    //Размещение по центру экрана
    const auto screenSize = size();
    move(screenSize.width()/2, screenSize.height()/2 - size().height()/2);

    QPixmap outPixmap = QPixmap();
    outPixmap.loadFromData(arr);
    ui->picLabel->setPixmap(outPixmap.scaledToHeight(650));
}

Card::~Card()
{
    delete ui;
}

void Card::shutdown()
{
    close();
}


