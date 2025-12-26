#ifndef DIALOGERASE_H
#define DIALOGERASE_H

#include <QDialog>

namespace Ui {
class DialogErase;
}

class DialogErase : public QDialog
{
    Q_OBJECT

public:
    explicit DialogErase(QWidget *parent = nullptr);
    ~DialogErase();

private slots:   
    void on_SoftErase_stateChanged(int arg1);

    void on_EandR_stateChanged(int arg1);


private:
    Ui::DialogErase *ui;


public slots:
    void receivedeDataErase(QString a, QString b, QString c, int d, bool e);


signals:
    void signalWindowErase(int);

    void signalWindowSoftErase(int);

};

#endif // DIALOGERASE_H
