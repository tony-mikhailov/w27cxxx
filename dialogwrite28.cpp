#include "dialogwrite28.h"
#include "ui_dialogwrite28.h"

DialogWrite28::DialogWrite28(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogWrite28)
{
    ui->setupUi(this);
}

DialogWrite28::~DialogWrite28()
{
    delete ui;
}

void DialogWrite28::receivedeDataWrite28(QString a, int b, int c)
{
    Chip28 = a;
    ui->chip->setText(a);
    ui->WandV28->setChecked(false);
    if(a == "28C04" || a == "28C17") {
        ui->SDP_Disable->setEnabled(false);
        ui->SDP_Enable->setEnabled(false);
    } else {
        ui->SDP_Disable->setEnabled(true);
        ui->SDP_Enable->setEnabled(true);
    }
    ui->SDP_Disable->setChecked(false);
    ui->SDP_Enable->setChecked(false);
    if(a == "28C010" || a == "28C020" || a == "28C040") {
        ui->Module->show();
        ui->Module->setEnabled(true);
    } else {
        ui->Module->hide();
    }
    if(b) {
        ui->Module->setChecked(true);
    } else {
        ui->Module->setChecked(false);
    }
    ui->PageWrite_1->hide();
    ui->PageWrite_16->hide();
    ui->PageWrite_32->hide();
    ui->PageWrite_64->hide();
    ui->PageWrite_128->hide();
    ui->PageWrite_256->hide();
    if(a == "28C04") {
        ui->PageWrite_1->setGeometry(127, 130, 41, 17);
        ui->PageWrite_1->show();
    }
    if(a == "28C16") {
        ui->PageWrite_1->setGeometry(45, 130, 41, 17);
        ui->PageWrite_1->show();
        ui->PageWrite_16->setGeometry(87, 130, 41, 17);
        ui->PageWrite_16->show();
        ui->PageWrite_64->setGeometry(129, 130, 41, 17);
        ui->PageWrite_64->show();
    }
    if(a == "28C17") {
        ui->PageWrite_1->setGeometry(65, 130, 41, 17);
        ui->PageWrite_1->show();
        ui->PageWrite_32->setGeometry(107, 130, 41, 17);
        ui->PageWrite_32->show();
    }
    if(a == "28C64") {
        ui->PageWrite_1->setGeometry(45, 130, 41, 17);
        ui->PageWrite_1->show();
        ui->PageWrite_32->setGeometry(87, 130, 41, 17);
        ui->PageWrite_32->show();
        ui->PageWrite_64->setGeometry(129, 130, 41, 17);
        ui->PageWrite_64->show();
    }
    if(a == "28C256") {
        ui->PageWrite_64->setGeometry(65, 130, 41, 17);
        ui->PageWrite_64->show();
        ui->PageWrite_128->setGeometry(107, 130, 41, 17);
        ui->PageWrite_128->show();
    }
    if(a == "28C512") {
        ui->PageWrite_128->setGeometry(127, 130, 41, 17);
        ui->PageWrite_128->show();
    }
    if(a == "28C010") {
        if(b) {
            ui->PageWrite_64->setGeometry(127, 130, 41, 17);
            ui->PageWrite_64->show();
        } else {
            ui->PageWrite_128->setGeometry(65, 130, 41, 17);
            ui->PageWrite_128->show();
            ui->PageWrite_256->setGeometry(107, 130, 41, 17);
            ui->PageWrite_256->show();
        }
    }
    if(a == "28C020") {
        ui->PageWrite_128->setGeometry(127, 130, 41, 17);
        ui->PageWrite_128->show();
    }
    if(a == "28C040") {
        if(b) {
            ui->PageWrite_128->setGeometry(65, 130, 41, 17);
            ui->PageWrite_128->show();
            ui->PageWrite_256->setGeometry(107, 130, 41, 17);
            ui->PageWrite_256->show();
        } else {
            ui->PageWrite_256->setGeometry(127, 130, 41, 17);
            ui->PageWrite_256->show();
        }
    }
    if(a == "29C256") {
        ui->Module->hide();
        ui->PageWrite_64->setGeometry(127, 130, 41, 17);
        ui->PageWrite_64->show();
    }
    if(a == "29C512") {
        ui->Module->hide();
        ui->PageWrite_128->setGeometry(127, 130, 41, 17);
        ui->PageWrite_128->show();
    }
    if(a == "29C010") {
        ui->Module->hide();
        ui->PageWrite_128->setGeometry(127, 130, 41, 17);
        ui->PageWrite_128->show();
    }
    if(a == "29C020") {
        ui->Module->hide();
        ui->PageWrite_128->setGeometry(65, 130, 41, 17);
        ui->PageWrite_128->show();
        ui->PageWrite_256->setGeometry(107, 130, 41, 17);
        ui->PageWrite_256->show();
    }
    if(a == "29C040") {
        ui->Module->hide();
        ui->PageWrite_256->setGeometry(127, 130, 41, 17);
        ui->PageWrite_256->show();
    }
    if(c == 1) {ui->PageWrite_1->setChecked(true);}
    if(c == 16) {ui->PageWrite_16->setChecked(true);}
    if(c == 32) {ui->PageWrite_32->setChecked(true);}
    if(c == 64) {ui->PageWrite_64->setChecked(true);}
    if(c == 128) {ui->PageWrite_128->setChecked(true);}
    if(c == 256) {ui->PageWrite_256->setChecked(true);}
}

void DialogWrite28::on_WandV28_stateChanged(int arg1)
{
   emit signalWindowWrite28(arg1);
}

void DialogWrite28::on_SDP_Disable_stateChanged(int arg1)
{
   if(arg1) {ui->SDP_Enable->setChecked(false);}
   emit signalWindowWrite28SDP_D(arg1);
}

void DialogWrite28::on_SDP_Enable_stateChanged(int arg1)
{
   if(arg1) {ui->SDP_Disable->setChecked(false);}
   emit signalWindowWrite28SDP_E(arg1);
}

void DialogWrite28::on_Module_stateChanged(int arg1)
{
    if(Chip28 == "28C010") {
        if(arg1) {
            ui->PageWrite_128->hide();
            ui->PageWrite_256->hide();
            ui->PageWrite_64->setGeometry(127, 130, 41, 17);
            ui->PageWrite_64->show();
            ui->PageWrite_64->setChecked(true);
            emit signalPageWrite28C(64);
        } else {
            ui->PageWrite_64->hide();
            ui->PageWrite_128->setGeometry(65, 130, 41, 17);
            ui->PageWrite_128->show();
            ui->PageWrite_256->setGeometry(107, 130, 41, 17);
            ui->PageWrite_256->show();
            ui->PageWrite_128->setChecked(true);
            emit signalPageWrite28C(128);
        }
    }
    if(Chip28 == "28C040") {
        if(arg1) {
            ui->PageWrite_128->setGeometry(65, 130, 41, 17);
            ui->PageWrite_128->show();
            ui->PageWrite_256->setGeometry(107, 130, 41, 17);
            ui->PageWrite_128->setChecked(true);
            emit signalPageWrite28C(128);
        } else {
            ui->PageWrite_128->hide();
            ui->PageWrite_256->setGeometry(127, 130, 41, 17);
            ui->PageWrite_256->setChecked(true);
            emit signalPageWrite28C(256);
        }
    }
    emit signalModule28C(arg1);
}

void DialogWrite28::on_PageWrite_1_clicked()
{
    emit signalPageWrite28C(1);
}

void DialogWrite28::on_PageWrite_16_clicked()
{
    emit signalPageWrite28C(16);
}

void DialogWrite28::on_PageWrite_32_clicked()
{
    emit signalPageWrite28C(32);
}

void DialogWrite28::on_PageWrite_64_clicked()
{
    emit signalPageWrite28C(64);
}

void DialogWrite28::on_PageWrite_128_clicked()
{
    emit signalPageWrite28C(128);
}

void DialogWrite28::on_PageWrite_256_clicked()
{
    emit signalPageWrite28C(256);
}
