#ifndef DIALOGSELVPP_H
#define DIALOGSELVPP_H

#include <QDialog>

namespace Ui {
class DialogSelVpp;
}

class DialogSelVpp : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelVpp(QWidget *parent = nullptr);
    ~DialogSelVpp();

private:
    Ui::DialogSelVpp *ui;

public slots:
    void receivedeDataSelVpp(QString a, int b, int c);

};

#endif // DIALOGSELVPP_H
