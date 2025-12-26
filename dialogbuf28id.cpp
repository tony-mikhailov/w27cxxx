#include "dialogbuf28id.h"
#include "ui_dialogbuf28id.h"

Dialogbuf28ID::Dialogbuf28ID(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialogbuf28ID)
{
    ui->setupUi(this);
}

Dialogbuf28ID::~Dialogbuf28ID()
{
    delete ui;
}

void Dialogbuf28ID::receivedeDatabuf28ID(QString a, int b)
{
    if(a == "28C64") {
        if(b == 32) {ui->bufID28C_32->setChecked(true);}
        if(b == 64) {ui->bufID28C_64->setChecked(true);}
    }
}

void Dialogbuf28ID::on_bufID28C_32_clicked()
{
    emit signalbufID28C(32);
}

void Dialogbuf28ID::on_bufID28C_64_clicked()
{
    emit signalbufID28C(64);
}
