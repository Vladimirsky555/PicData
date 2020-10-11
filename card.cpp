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
    ui->picLabel->setPixmap(outPixmap.scaledToHeight(600));
}

Card::~Card()
{
    delete ui;
}

void Card::shutdown()
{
    close();
}


