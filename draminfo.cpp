#include "draminfo.h"
#include "ui_draminfo.h"

DramInfo::DramInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DramInfo)
{
    ui->setupUi(this);
}

DramInfo::~DramInfo()
{
    delete ui;
}

void DramInfo::receivedeDramInfo(QString a)
{
    int b = 0;
    QString c = "";
    QString d = "";
    ui->chip->setText(a);
    if(a == "4164") {b = 65536; c = "64K x 1 bit"; d = "image/dram64.png";}
    if(a == "41256") {b = 262144; c = "256K x 1 bit"; d = "image/dram256.png";}
    ui->size->setText(QString("%1  Bytes").arg(b));
    ui->package_2->setText("DIP16");
    ui->algorithm->setText("Dynamic RAM");
    ui->VCC->setText("5 Volt");
    ui->organized->setText(c);
    QPixmap myPixmap("image/dip16d.png");
    ui->image->setPixmap(myPixmap);
    QPixmap myPixmap2(QString("%1").arg(d));
    ui->image2->setPixmap(myPixmap2);
    ui->image2->setScaledContents(true);
}
