#ifndef DIALOGSELDIP_H
#define DIALOGSELDIP_H

#include <QDialog>

namespace Ui {
class DialogSelDip;
}

class DialogSelDip : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelDip(QWidget *parent = nullptr);
    ~DialogSelDip();

private:
    Ui::DialogSelDip *ui;

public slots:
    void receivedeDataSelDip(int a);

};

#endif // DIALOGSELDIP_H      
