#ifndef CHIPINFO_H
#define CHIPINFO_H

#include <QWidget>

namespace Ui {
class ChipInfo;
}

class ChipInfo : public QWidget
{
    Q_OBJECT

public:
    explicit ChipInfo(QWidget *parent = nullptr);
    ~ChipInfo();

private:
    Ui::ChipInfo *ui;

public slots:
    void receivedeChipInfo(QString a, int b, int c, int d);

};

#endif // CHIPINFO_H
