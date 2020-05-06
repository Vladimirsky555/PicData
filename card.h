#ifndef CARD_H
#define CARD_H

#include <QWidget>

namespace Ui {
class Card;
}

class Card : public QWidget
{
    Q_OBJECT

public:
    Card(QByteArray arr, QWidget *parent = 0);
    virtual ~Card();

private slots:
    void shutdown();

private:
    Ui::Card *ui;
};

#endif // CARD_H
