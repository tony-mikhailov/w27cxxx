#include "logicinfo.h"
#include "ui_logicinfo.h"

LogicInfo::LogicInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogicInfo)
{
    ui->setupUi(this);
}

LogicInfo::~LogicInfo()
{
    delete ui;
}

void LogicInfo::receivedeLogicInfo(QString a, QString b, int c, QStringList d)
{
    ui->chip->setText(a);
    ui->desc->setText(QString("%1").arg(b));
    ui->VCC->setText("5 Volt");
    if(c == 14) {
        ui->package_2->setText("DIP14");
        QPixmap myPixmap("image/dip14.png"); ui->image->setPixmap(myPixmap);
    } else {
        ui->package_2->setText("DIP16");
        QPixmap myPixmap("image/dip16.png"); ui->image->setPixmap(myPixmap);
    }
    ui->textBrowser->clear();
    for(uint8_t i = 0; i < d.count(); i++) {
        ui->textBrowser->append(d[i]);
    }
}
