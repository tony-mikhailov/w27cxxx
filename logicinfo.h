#ifndef LOGICINFO_H
#define LOGICINFO_H

#include <QWidget>

namespace Ui {
class LogicInfo;
}

class LogicInfo : public QWidget
{
    Q_OBJECT

public:
    explicit LogicInfo(QWidget *parent = nullptr);
    ~LogicInfo();

private:
    Ui::LogicInfo *ui;

public slots:
    void receivedeLogicInfo(QString a, QString b, int c, QStringList d);

};

#endif // LOGICINFO_H
