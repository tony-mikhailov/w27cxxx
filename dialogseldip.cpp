#include "dialogseldip.h"
#include "ui_dialogseldip.h"

#include <QFile>

DialogSelDip::DialogSelDip(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSelDip)
{
    ui->setupUi(this);
}

DialogSelDip::~DialogSelDip()
{
    delete ui;
}

void DialogSelDip::receivedeDataSelDip(int a)
{
    QString text = "";
    if(a == 1) {
        text = "To supply Vcc power to the chip (pin 24 of DIP24), set jumper JP1 to position 1 as shown in the picture and click OK.";
        QPixmap myPixmap("image/vcc1.png"); ui->image->setPixmap(myPixmap);
    }
    if(a == 2) {
        text = "To supply Vcc power to the chip (pin 28 of DIP28), set jumper JP1 to position 2 as shown in the picture and click OK.";
        QPixmap myPixmap("image/vcc2.png"); ui->image->setPixmap(myPixmap);
    }
    if(a == 3) {
        text = "To supply Vcc power to the chip (pin 32 of DIP32), set jumper JP1 to position 3 as shown in the picture and click OK.";
        QPixmap myPixmap("image/vcc3.png");
        ui->image->setPixmap(myPixmap);
    }
    ui->text->setText(text);
}
