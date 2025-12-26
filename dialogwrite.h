#ifndef DIALOGWRITE_H
#define DIALOGWRITE_H

#include <QDialog>

namespace Ui {
class DialogWrite;
}

class DialogWrite : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWrite(QWidget *parent = nullptr);
    ~DialogWrite();


private slots:
    void on_CEPulse_25_clicked();

    void on_CEPulse_50_clicked();

    void on_CEPulse_100_clicked();

    void on_CEPulse_200_clicked();

    void on_CEPulse_500_clicked();

    void on_CEPulse_1000_clicked();

    void on_CEPulse_5000_clicked();

    void on_CEPulse_10000_clicked();

    void on_CEPulse_50000_clicked();

    void on_WandV_stateChanged(int arg1);


private:
    Ui::DialogWrite *ui;


public slots:
    void receivedeDataWrite(QString a, QString b, QString c, int d);


signals:
    void signalWindowWrite(int);

    void signalCEPulseWrite(int);

};

#endif // DIALOGWRITE_H
