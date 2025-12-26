#ifndef DIALOGWRITE28_H
#define DIALOGWRITE28_H

#include <QDialog>

namespace Ui {
class DialogWrite28;
}

class DialogWrite28 : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWrite28(QWidget *parent = nullptr);
    ~DialogWrite28();


private slots:
    void on_WandV28_stateChanged(int arg1);

    void on_SDP_Disable_stateChanged(int arg1);

    void on_SDP_Enable_stateChanged(int arg1);

    void on_Module_stateChanged(int arg1);


private:
    Ui::DialogWrite28 *ui;

    QString Chip28;


public slots:
    void receivedeDataWrite28(QString a, int b, int c);


private slots:
    void on_PageWrite_1_clicked();

    void on_PageWrite_16_clicked();

    void on_PageWrite_32_clicked();

    void on_PageWrite_64_clicked();

    void on_PageWrite_128_clicked();

    void on_PageWrite_256_clicked();


signals:
    void signalWindowWrite28(int);

    void signalWindowWrite28SDP_D(int);

    void signalWindowWrite28SDP_E(int);

    void signalModule28C(int);

    void signalPageWrite28C(int);


};

#endif // DIALOGWRITE28_H
