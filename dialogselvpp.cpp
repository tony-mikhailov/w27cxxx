#include "dialogselvpp.h"
#include "ui_dialogselvpp.h"

DialogSelVpp::DialogSelVpp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSelVpp)
{
    ui->setupUi(this);
}

DialogSelVpp::~DialogSelVpp()
{
    delete ui;
}

void DialogSelVpp::receivedeDataSelVpp(QString a, int b, int c)
{
    QString text = "";
    if(a == "27C16") {
        if(c == 1) {
            text = "To apply programming voltage (Vpp) to the chip (pin VPP), set jumper JP2 to position 1 as shown in the picture and click OK.";
        }
        if(c == 2) {
            text = "To apply erase voltage (Vpe) to the chip (pin VPP), set jumper JP2 to position 1 as shown in the picture and click OK.";
        }
        QPixmap myPixmap("image/vpp1.png"); ui->image->setPixmap(myPixmap);
    }
    if(a == "27C64" || a == "27C128" || a == "27C256") {
        if(c == 1) {
            text = "To apply programming voltage (Vpp) to the chip (pin VPP), set jumper JP2 to position 2 as shown in the picture and click OK.";
        }
        if(c == 2) {
            text = "To apply erase voltage (Vpe) to the chip (pin VPP), set jumper JP2 to position 2 as shown in the picture and click OK.";
        }
        QPixmap myPixmap("image/vpp2.png"); ui->image->setPixmap(myPixmap);
    }
    if(a == "27C32" || a == "27C512" || a == "27C080") {
        if(c == 1) {
            text = "To apply programming voltage (Vpp) to the chip (pin VPP), set jumper JP2 to position 3 as shown in the picture and click OK.";
        }
        if(c == 2) {
            text = "To apply erase voltage (Vpe) to the chip (pin VPP), set jumper JP2 to position 3 as shown in the picture and click OK.";
        }
        QPixmap myPixmap("image/vpp3.png"); ui->image->setPixmap(myPixmap);
    }
    if(a == "27C010" || a == "27C020" || a == "27C040") {
        if(c == 1) {
            text = "To apply programming voltage (Vpp) to the chip (pin VPP), set jumper JP2 to position 4 as shown in the picture and click OK.";
        }
        if(c == 2) {
            text = "To apply erase voltage (Vpe) to the chip (pin VPP), set jumper JP2 to position 4 as shown in the picture and click OK.";
        }
        QPixmap myPixmap("image/vpp4.png"); ui->image->setPixmap(myPixmap);
    }
    if(b == 2) {
        text = "To apply erase voltage (Vpe) to the chip (pin OE), set jumper JP2 to position 3 as shown in the picture and click OK.";
        QPixmap myPixmap("image/vpp3.png"); ui->image->setPixmap(myPixmap);
    }

    if(b == 3) {
        if(c == 1) {
            text = "To apply programming voltage (Vpp) to the chip (pin VPP), set jumper JP2 to position 4 as shown in the picture and click OK.";
        }
        if(c == 2) {
            text = "To apply erase voltage (Vpe) to the chip (pin VPP), set jumper JP2 to position 4 as shown in the picture and click OK.";
        }
        QPixmap myPixmap("image/vpp4.png"); ui->image->setPixmap(myPixmap);
    }
    ui->text->setText(text);
}
