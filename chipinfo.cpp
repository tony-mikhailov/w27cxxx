#include "chipinfo.h"
#include "ui_chipinfo.h"

ChipInfo::ChipInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChipInfo)
{
    ui->setupUi(this);
}

ChipInfo::~ChipInfo()
{
    delete ui;
}

void ChipInfo::receivedeChipInfo(QString a, int b, int c, int d)
{
    ui->chip->setText(a);
    if(d == 7) {
        ui->size->setText(QString("%1  Bits").arg(b));
    } else {
        ui->size->setText(QString("%1  Bytes").arg(b));
    }
    if(c == 1) {
        ui->package_2->setText("DIP24");
        QPixmap myPixmap("image/dip24.png");
        ui->image->setPixmap(myPixmap);
    } else
    if(c == 2) {
        ui->package_2->setText("DIP28");
        QPixmap myPixmap("image/dip28.png");
        ui->image->setPixmap(myPixmap);
    } else
    if(c == 3) {
        ui->package_2->setText("DIP32");
        QPixmap myPixmap("image/dip32.png");
        ui->image->setPixmap(myPixmap);
    }
    if(c == 4) {
        ui->package_2->setText("DIP16");
        QPixmap myPixmap("image/dip16d.png");
        ui->image->setPixmap(myPixmap);
    }
    QString name = a;
    if(a == "SRAM 16Kbit") {name = "sram16";}
    if(a == "SRAM 32Kbit") {name = "sram32";}
    if(a == "SRAM 64Kbit") {name = "sram64";}
    if(a == "SRAM 128Kbit") {name = "sram128";}
    if(a == "SRAM 256Kbit") {name = "sram256";}
    if(a == "SRAM 512Kbit") {name = "sram512";}
    if(a == "SRAM 1024Kbit") {name = "sram010";}
    if(a == "SRAM 2048Kbit") {name = "sram020";}
    if(a == "SRAM 4096Kbit") {name = "sram040";}
    if(a == "4164") {name = "dram64";}
    if(a == "41256") {name = "dram256";}
    QPixmap myPixmap(QString("image/%1.png").arg(name));
    ui->image2->setPixmap(myPixmap);
    ui->image2->setScaledContents(true);
    if(d == 1) {
        ui->algorithm->setText("27Cxxx");
    } else
    if(d == 2) {
        ui->algorithm->setText("28Cxxx");
    } else
    if(d == 3) {
        ui->algorithm->setText("28Fxxx");
    } else
    if(d == 4) {
        ui->algorithm->setText("29Cxxx");
    } else
    if(d == 5) {
        ui->algorithm->setText("29Fxxx");
    } else
    if(d == 6) {
        ui->algorithm->setText("Static RAM");
    }
    if(d == 7) {
        ui->algorithm->setText("Dynamic RAM");
    }
    ui->VCC->setText("5 Volt");
    if(a == "28C04") {
        ui->organized->setText("512 x 8 bits");
    } else
    if(a == "27C16" || a == "28C16" || a == "28C17" || a == "SRAM 16Kbit") {
        ui->organized->setText("2K x 8 bits");
    } else
    if(a == "27C32" || a == "SRAM 32Kbit") {
        ui->organized->setText("4K x 8 bits");
    } else
    if(a == "27C64" || a == "28C64" || a == "SRAM 64Kbit") {
        ui->organized->setText("8K x 8 bits");
    } else
    if(a == "27C128" || a == "SRAM 128Kbit") {
        ui->organized->setText("16K x 8 bits");
    } else
    if(a == "27C256" || a == "28C256" || a == "28F256" || a == "29C256" || a == "29F256" || a == "SRAM 256Kbit") {
        ui->organized->setText("32K x 8 bits");
    } else
    if(a == "27C512" || a == "28C512" || a == "28F512" || a == "29C512" || a == "29F512" || a == "SRAM 512Kbit") {
        ui->organized->setText("64K x 8 bits");
    } else
    if(a == "27C010" || a == "28C010" || a == "28F010" || a == "29C010" || a == "29F010" || a == "SRAM 1024Kbit") {
        ui->organized->setText("128K x 8 bits");
    } else
    if(a == "27C020" || a == "28C020" || a == "28F020" || a == "29C020" || a == "29F020" || a == "SRAM 2048Kbit") {
        ui->organized->setText("256K x 8 bits");
    } else
    if(a == "27C040" || a == "28C040" || a == "29C040" || a == "29F040" || a == "SRAM 4096Kbit") {
        ui->organized->setText("512K x 8 bits");
    } else
    if(a == "27C080") {
        ui->organized->setText("1M x 8 bits");
    }
    if(a == "4164") {
        ui->organized->setText("64K x 1 bit");
    }
    if(a == "41256") {
        ui->organized->setText("256K x 1 bit");
    }
}
