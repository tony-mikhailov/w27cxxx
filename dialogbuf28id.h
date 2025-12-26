#ifndef DIALOGBUF28ID_H
#define DIALOGBUF28ID_H

#include <QDialog>

namespace Ui {
class Dialogbuf28ID;
}

class Dialogbuf28ID : public QDialog
{
    Q_OBJECT

public:
    explicit Dialogbuf28ID(QWidget *parent = nullptr);
    ~Dialogbuf28ID();

private:
    Ui::Dialogbuf28ID *ui;


public slots:
    void receivedeDatabuf28ID(QString a, int b);


private slots:
    void on_bufID28C_32_clicked();

    void on_bufID28C_64_clicked();


signals:
    void signalbufID28C(int);

};

#endif // DIALOGBUF28ID_H
