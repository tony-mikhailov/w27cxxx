#ifndef DIALOGWRITE28F_H
#define DIALOGWRITE28F_H

#include <QDialog>

namespace Ui {
class DialogWrite28F;
}

class DialogWrite28F : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWrite28F(QWidget *parent = nullptr);
    ~DialogWrite28F();

private:
    Ui::DialogWrite28F *ui;

public slots:
    void receivedeDataWrite28F(QString a, QString b, QString c);

};

#endif // DIALOGWRITE28F_H
