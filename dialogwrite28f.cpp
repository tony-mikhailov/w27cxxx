#include "dialogwrite28f.h"
#include "ui_dialogwrite28f.h"

DialogWrite28F::DialogWrite28F(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogWrite28F)
{
    ui->setupUi(this);
}

DialogWrite28F::~DialogWrite28F()
{
    delete ui;
}

void DialogWrite28F::receivedeDataWrite28F(QString a, QString b, QString c)
{
    ui->chip->setText(a);
    ui->d_VPP->setText(b);
    ui->s_VPP->setText(c);
}
