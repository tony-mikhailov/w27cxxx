#include "dialogerase.h"
#include "ui_dialogerase.h"

DialogErase::DialogErase(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogErase)
{
    ui->setupUi(this);
}

DialogErase::~DialogErase()
{
    delete ui;
}

void DialogErase::receivedeDataErase(QString a, QString b, QString c, int d, bool e)
{
    ui->chip->setText(a);
    ui->d_VPE->setText(b);
    ui->s_VPE->setText(c);
    if(d == 1) {ui->pinE->setText("(pins A9, VPP)");}
    else if(d == 3) {ui->pinE->setText("(pin VPP)");}
    else {ui->pinE->setText("(pin OE)");}
    if(d == 3) {
        ui->EandR->hide();
    } else {
        ui->EandR->show();
        ui->EandR->setChecked(false);
    }
    ui->SoftErase->hide();
    if(d == 2) {
        ui->SoftErase->show();
        ui->SoftErase->setEnabled(true);
        if(a == "28C04" || a == "28C16" || a == "28C17" || a == "28C64") {
            ui->SoftErase->setEnabled(false);
        }
    }
    if(e) {
        ui->SoftErase->setChecked(true);
    } else {
        ui->SoftErase->setChecked(false);
    }
}

void DialogErase::on_SoftErase_stateChanged(int arg1)
{
   if(arg1) {
       ui->d_VPE->setEnabled(false);
       ui->s_VPE->setEnabled(false);
   } else {
       ui->d_VPE->setEnabled(true);
       ui->s_VPE->setEnabled(true);
   }
   emit signalWindowSoftErase(arg1);
}

void DialogErase::on_EandR_stateChanged(int arg1)
{
   emit signalWindowErase(arg1);
}
