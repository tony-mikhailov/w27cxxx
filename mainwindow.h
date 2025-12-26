#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QListWidgetItem>
#include <QTimer>

#include "dialogseldip.h"
#include "chipinfo.h"
#include "logicinfo.h"
#include "dialogbuf28id.h"
#include "dialogerase.h"
#include "dialogwrite.h"
#include "dialogwrite28.h"
#include "dialogwrite28f.h"
#include "dialogselvpp.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


signals:
    void sendDataSelDip(int);

    void chipUpdated();

    void bufferUpdated();

    void sendChipInfo(QString, int, int, int);

    void sendLogicInfo(QString, QString, int, QStringList);

    void signatureCodeSignal();

    void read28IDSignal();

    void write28IDSignal();

    void sendDatabuf28ID(QString, int);

    void sendDataErase(QString, QString, QString, int, bool);

    void chipErase();

    void chipReaded();

    void sendDataWrite(QString, QString, QString, int);

    void sendDataWrite28(QString, int, int);

    void sendDataWrite28F(QString, QString, QString);

    void chipWrite();

    void sendDataSelVpp(QString, int, int);


public slots:
    void autoDetect();

    void bufID28C(int value);

    void eraseToRead(int value);

    void ToSoftErase(int value);

    void writeToCEPulse(int value);

    void writeToVerification(int value);

    void writeToSDP_Disabled(int value);

    void writeToSDP_Enabled(int value);

    void module_28C(int value);

    void PageWrite_28C(int value);


private slots:
    void closeEvent(QCloseEvent *);

    void on_clearLogButton_clicked();

    void on_connectButton_clicked();

    void openSerialPort();

    void on_disconnectButton_clicked();

    void on_updateButton_clicked();

    void on_portList_itemClicked(QListWidgetItem *item);

    void reload_ports();

    void on_chipInfoButton_clicked();

    void on_typeList_currentChanged(int index);

    void on_E_algorithmList_currentChanged(int index);

    void on_F_algorithmList_currentChanged(int index);

    void on_R_algorithmList_currentChanged(int index);

    void on_E27C_dipList_currentChanged(int index);

    void on_E28C_dipList_currentChanged(int index);

    void on_F28F_dipList_currentChanged(int index);

    void on_F29C_dipList_currentChanged(int index);

    void on_F29F_dipList_currentChanged(int index);

    void on_SRAM_dipList_currentChanged(int index);

    void resizeBuffers();

    void on_E27c16Button_clicked();

    void on_E27c32Button_clicked();

    void on_E27c64Button_clicked();

    void on_E27c128Button_clicked();

    void on_E27c256Button_clicked();

    void on_E27c512Button_clicked();

    void on_E27c010Button_clicked();

    void on_E27c020Button_clicked();

    void on_E27c040Button_clicked();

    void on_E27c080Button_clicked();

    void on_E28c04Button_clicked();

    void on_E28c16Button_clicked();

    void on_E28c17Button_clicked();

    void on_E28c64Button_clicked();

    void on_E28c256Button_clicked();

    void on_E28c512Button_clicked();

    void on_E28c010Button_clicked();

    void on_E28c020Button_clicked();

    void on_E28c040Button_clicked();

    void on_F28f256Button_clicked();

    void on_F28f512Button_clicked();

    void on_F28f010Button_clicked();

    void on_F28f020Button_clicked();

    void on_F29c256Button_clicked();

    void on_F29c512Button_clicked();

    void on_F29c010Button_clicked();

    void on_F29c020Button_clicked();

    void on_F29c040Button_clicked();

    void on_F29f512Button_clicked();

    void on_F29f010Button_clicked();

    void on_F29f020Button_clicked();

    void on_F29f040Button_clicked();

    void on_SRAM16Button_clicked();

    void on_SRAM32Button_clicked();

    void on_SRAM64Button_clicked();

    void on_SRAM128Button_clicked();

    void on_SRAM256Button_clicked();

    void on_SRAM512Button_clicked();

    void on_SRAM010Button_clicked();

    void on_SRAM020Button_clicked();

    void on_SRAM040Button_clicked();

    void RadioButton_clicked();

    void on_autoDetectButton_clicked();

    void on_autoDetectButton2_clicked();

    void on_autoDetectButton3_clicked();

    void on_autoDetectButton4_clicked();

    void on_autoDetectButton5_clicked();

    void on_autoDetectButton6_clicked();

    void on_autoDetectButton7_clicked();

    void on_SoftDetectButton_clicked();

    void on_SoftDetectButton2_clicked();

    void signatureCodeStart();

    void signatureCode();

    void on_IdentificationReadButton_clicked();

    void on_IdentificationReadButton2_clicked();

    void on_IdentificationReadButton3_clicked();

    void read28IDStart();

    void read28ID();

    void on_saveID28Button_clicked();

    void on_saveID28Button2_clicked();

    void on_saveID28Button3_clicked();

    void on_openID28Button_clicked();

    void on_openID28Button2_clicked();

    void on_openID28Button3_clicked();

    void on_IdentificationWriteButton_clicked();

    void on_IdentificationWriteButton2_clicked();

    void on_IdentificationWriteButton3_clicked();

    void write28IDStart();

    void write28ID();

    void on_testSRAMButton_clicked();

    void on_testSRAMButton2_clicked();

    void on_testSRAMButton3_clicked();

    void Voltage();

    void showVoltage();

    void on_voltageChipButton_toggled(bool checked);

    void on_openFileButton_clicked();

    void on_saveFileButton_clicked();

    void on_showButton_toggled(bool checked);

    void showBuf();

    void on_eraseChipButton_clicked();

    void eraseChipWindow();

    void eraseData();

    void eraseData28F();

    void on_readChipButton_clicked();

    void readData();

    void checkClear();

    void on_writeChipButton_clicked();

    void writeChipWindow();

    void writeChip28F();

    void writeChip();

    void on_verifyChipButton_clicked();

    void verifyData();

    void on_testDRAMButton_clicked();

    void testDRAM();

    void on_D4164Button_clicked();

    void on_D41256Button_clicked();

    void selectLogicChip(QString value);

    void on_listLogic_itemClicked(QListWidgetItem *item);

    void on_testLogicButton_clicked();

    void testLogic();

    void on_autoTestingButton_clicked();

    void autoTestLogic();

    void on_textBrowser_anchorClicked(const QUrl &arg1);


    void on_showButton_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serialPort = NULL;

    DialogSelDip *windowSelDip;
    ChipInfo *windowChipInfo;
    LogicInfo *windowLogicInfo;
    Dialogbuf28ID *windowbuf28ID;
    DialogErase *windowErase;
    DialogWrite *windowWrite;
    DialogWrite28 *windowWrite28;
    DialogWrite28F *windowWrite28F;
    DialogSelVpp *windowSelVpp;

    QTimer updatePortsTimer;
    QTimer openPortsTimer;
    QTimer updateVoltageTimer;
    QMetaObject::Connection updatePortsConnection;
    QMetaObject::Connection serialDataConnection;
    QMetaObject::Connection openPortsConnection;
    QMetaObject::Connection signatureCodeConnection;
    QMetaObject::Connection read28IDConnection;
    QMetaObject::Connection write28IDConnection;
    QMetaObject::Connection verifyDataConnection;
    QMetaObject::Connection checkClearConnection;
    QMetaObject::Connection updateBufConnection;
    QMetaObject::Connection updateVoltageConnection;
    QMetaObject::Connection checkEraseConnection;
    QMetaObject::Connection checkWriteConnection;

    uint8_t typeGroup = 0; // 0 - EPROM, 1 - FLASH, 2 - RAM, 3 - LOGIC
    uint8_t dipGroup = 0;  // 1 - dip24, 2 - dip28, 3 - dip32, 4 - dip16
    uint8_t chipGroup = 0; // 1 - 27Series, 2 - 28CSeries, 3 - 28FSeries, 4 - 29CSeries, 5 - 29FSeries, 6 - SRAM, 7 - DRAM
    uint8_t Detect = 0;    // 1 - 24d-27, 2 - 28d-27, 3 - 32d-27, 4 - 32d-28F, 5 - 28d-29C, 6 - 32d-29C, 7 - 32d-29F
    bool chipSelected = false;
    bool dramSelected = false;
    bool logicSelected = false;
    bool bufferClear = true;
    bool bufferID28 = false;
    uint32_t bufSize = 0;
    uint16_t buf28ID = 0;
    bool eraseButton = false;
    bool eraseSet = false;
    bool SoftEraseEnable = false;
    uint16_t CEPulse = 100;
    uint8_t WandVSet = 0;
    uint8_t EandRSet = 0;
    uint8_t SDP_Disabled = 0;
    uint8_t SDP_Enabled  = 0;
    uint16_t PageWrite = 0;
    uint8_t Module = 0;
    uint8_t SoftDetect = 0;
    uint8_t TestSRAM = 0;
    QString Chip = "Unknown";
    QString VPP = "Unknown";
    QString VPE = "Unknown";
    float voltage = 0.0;

    QByteArray bufWork;
    QByteArray bufCheck;

    QString NameLogic = "Unknown";
    QString DescLogic = "Unknown";
    QString PinLogic = "Unknown";
    QStringList TruthLogic;
    uint8_t countTrLogic = 0;
    bool listLogicOk = false;
    uint16_t countListLogic = 0;
    uint16_t nextListLogic = 0;
    QList<QString> listLogic;
    bool autotesting = false;
    QStringList ResultAutoLogic;

    void sendCommand(const QByteArray &data);

    void writeData(const QByteArray &data);

    void log(QString str, QString color);

    void closeSerialPort();

    void updateButtons(bool actions, bool buffer);

    void chipSelectSetEnabled(bool state);

    void getVoltage();

    void readChip();

};

#endif // MAINWINDOW_H
