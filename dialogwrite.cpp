#include "dialogwrite.h"
#include "ui_dialogwrite.h"

DialogWrite::DialogWrite(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogWrite)
{
    ui->setupUi(this);
}

DialogWrite::~DialogWrite()
{
    delete ui;
}

void DialogWrite::receivedeDataWrite(QString a, QString b, QString c, int d)
{
    ui->chip->setText(a);
    ui->d_VPP->setText(b);
    ui->s_VPP->setText(c);
    ui->CEPulse_25->hide();
    ui->CEPulse_50->hide();
    ui->CEPulse_100->hide();
    ui->CEPulse_200->hide();
    ui->CEPulse_500->hide();
    ui->CEPulse_1000->hide();
    ui->CEPulse_5000->hide();
    ui->CEPulse_10000->hide();
    ui->CEPulse_50000->hide();
    if(a == "27C16" || a == "27C32") {
        ui->CEPulse_100->setGeometry(20, 110, 61, 17);
        ui->CEPulse_100->show();
        ui->CEPulse_500->setGeometry(80, 110, 61, 17);
        ui->CEPulse_500->show();
        ui->CEPulse_1000->setGeometry(140, 110, 61, 17);
        ui->CEPulse_1000->show();
        ui->CEPulse_5000->setGeometry(20, 130, 61, 17);
        ui->CEPulse_5000->show();
        ui->CEPulse_10000->setGeometry(80, 130, 61, 17);
        ui->CEPulse_10000->show();
        ui->CEPulse_50000->setGeometry(140, 130, 61, 17);
        ui->CEPulse_50000->show();
    }
    if(a == "27C64" || a == "27C128") {
        ui->CEPulse_100->setGeometry(20, 110, 61, 17);
        ui->CEPulse_100->show();
        ui->CEPulse_200->setGeometry(80, 110, 61, 17);
        ui->CEPulse_200->show();
        ui->CEPulse_500->setGeometry(140, 110, 61, 17);
        ui->CEPulse_500->show();
        ui->CEPulse_1000->setGeometry(20, 130, 61, 17);
        ui->CEPulse_1000->show();
        ui->CEPulse_5000->setGeometry(80, 130, 61, 17);
        ui->CEPulse_5000->show();
        ui->CEPulse_10000->setGeometry(140, 130, 61, 17);
        ui->CEPulse_10000->show();
    }
    if(a == "27C256") {
        ui->CEPulse_50->setGeometry(20, 110, 61, 17);
        ui->CEPulse_50->show();
        ui->CEPulse_100->setGeometry(80, 110, 61, 17);
        ui->CEPulse_100->show();
        ui->CEPulse_200->setGeometry(140, 110, 61, 17);
        ui->CEPulse_200->show();
        ui->CEPulse_500->setGeometry(20, 130, 61, 17);
        ui->CEPulse_500->show();
        ui->CEPulse_1000->setGeometry(80, 130, 61, 17);
        ui->CEPulse_1000->show();
        ui->CEPulse_5000->setGeometry(140, 130, 61, 17);
        ui->CEPulse_5000->show();
    }
    if(a == "27C512" || a == "27C010") {
        ui->CEPulse_25->setGeometry(20, 110, 61, 17);
        ui->CEPulse_25->show();
        ui->CEPulse_50->setGeometry(80, 110, 61, 17);
        ui->CEPulse_50->show();
        ui->CEPulse_100->setGeometry(140, 110, 61, 17);
        ui->CEPulse_100->show();
        ui->CEPulse_200->setGeometry(20, 130, 61, 17);
        ui->CEPulse_200->show();
        ui->CEPulse_500->setGeometry(80, 130, 61, 17);
        ui->CEPulse_500->show();
        ui->CEPulse_1000->setGeometry(140, 130, 61, 17);
        ui->CEPulse_1000->show();
    }
    if(a == "27C020" || a == "27C040" || a == "27C080") {
        ui->CEPulse_25->setGeometry(20, 110, 61, 17);
        ui->CEPulse_25->show();
        ui->CEPulse_50->setGeometry(80, 110, 61, 17);
        ui->CEPulse_50->show();
        ui->CEPulse_100->setGeometry(140, 110, 61, 17);
        ui->CEPulse_100->show();
        ui->CEPulse_200->setGeometry(20, 130, 61, 17);
        ui->CEPulse_200->show();
    }
    if(d == 25) {ui->CEPulse_25->setChecked(true);}
    if(d == 50) {ui->CEPulse_50->setChecked(true);}
    if(d == 100) {ui->CEPulse_100->setChecked(true);}
    if(d == 200) {ui->CEPulse_200->setChecked(true);}
    if(d == 500) {ui->CEPulse_500->setChecked(true);}
    if(d == 1000) {ui->CEPulse_1000->setChecked(true);}
    if(d == 5000) {ui->CEPulse_5000->setChecked(true);}
    if(d == 10000) {ui->CEPulse_10000->setChecked(true);}
    if(d == 50000) {ui->CEPulse_50000->setChecked(true);}
    ui->WandV->setChecked(false);
}

void DialogWrite::on_CEPulse_25_clicked()
{
    emit signalCEPulseWrite(25);
}

void DialogWrite::on_CEPulse_50_clicked()
{
    emit signalCEPulseWrite(50);
}

void DialogWrite::on_CEPulse_100_clicked()
{
    emit signalCEPulseWrite(100);
}

void DialogWrite::on_CEPulse_200_clicked()
{
    emit signalCEPulseWrite(200);
}

void DialogWrite::on_CEPulse_500_clicked()
{
    emit signalCEPulseWrite(500);
}

void DialogWrite::on_CEPulse_1000_clicked()
{
    emit signalCEPulseWrite(1000);
}

void DialogWrite::on_CEPulse_5000_clicked()
{
    emit signalCEPulseWrite(5000);
}

void DialogWrite::on_CEPulse_10000_clicked()
{
    emit signalCEPulseWrite(10000);
}

void DialogWrite::on_CEPulse_50000_clicked()
{
    emit signalCEPulseWrite(50000);
}

void DialogWrite::on_WandV_stateChanged(int arg1)
{
   emit signalWindowWrite(arg1);
}
