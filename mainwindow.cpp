#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "testsram.h"

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTimer>
#include <QTextStream>

#define NO_ERR 0
#define WRITABLE 1
#define NOT_WRITABLE 2

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serialPort(new QSerialPort)
{
    ui->setupUi(this);

    windowSelDip = new DialogSelDip;
    windowSelDip->setModal(true);
    windowSelDip->setWindowFlags(windowSelDip->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    connect(this, &MainWindow::sendDataSelDip, windowSelDip, &DialogSelDip::receivedeDataSelDip);

    windowChipInfo = new ChipInfo;
    windowChipInfo->setWindowFlags(windowChipInfo->windowFlags() & ~Qt::WindowMinMaxButtonsHint);
    connect(this, &MainWindow::sendChipInfo, windowChipInfo, &ChipInfo::receivedeChipInfo);

    windowLogicInfo = new LogicInfo;
    windowLogicInfo->setWindowFlags(windowLogicInfo->windowFlags() & ~Qt::WindowMinMaxButtonsHint);
    connect(this, &MainWindow::sendLogicInfo, windowLogicInfo, &LogicInfo::receivedeLogicInfo);

    windowbuf28ID = new Dialogbuf28ID;
    windowbuf28ID->setModal(true);
    windowbuf28ID->setWindowFlags(windowbuf28ID->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    connect(this, &MainWindow::sendDatabuf28ID, windowbuf28ID, &Dialogbuf28ID::receivedeDatabuf28ID);
    connect(windowbuf28ID, &Dialogbuf28ID::signalbufID28C, this, &MainWindow::bufID28C);

    windowErase = new DialogErase;
    windowErase->setModal(true);
    windowErase->setWindowFlags(windowErase->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    connect(this, &MainWindow::sendDataErase, windowErase, &DialogErase::receivedeDataErase);
    connect(windowErase, &DialogErase::signalWindowErase, this, &MainWindow::eraseToRead);
    connect(windowErase, &DialogErase::signalWindowSoftErase, this, &MainWindow::ToSoftErase);

    windowWrite = new DialogWrite;
    windowWrite->setModal(true);
    windowWrite->setWindowFlags(windowWrite->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    connect(this, &MainWindow::sendDataWrite, windowWrite, &DialogWrite::receivedeDataWrite);
    connect(windowWrite, &DialogWrite::signalCEPulseWrite, this, &MainWindow::writeToCEPulse);
    connect(windowWrite, &DialogWrite::signalWindowWrite, this, &MainWindow::writeToVerification);

    windowWrite28 = new DialogWrite28;
    windowWrite28->setModal(true);
    windowWrite28->setWindowFlags(windowWrite28->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    connect(this, &MainWindow::sendDataWrite28, windowWrite28, &DialogWrite28::receivedeDataWrite28);
    connect(windowWrite28, &DialogWrite28::signalWindowWrite28, this, &MainWindow::writeToVerification);
    connect(windowWrite28, &DialogWrite28::signalWindowWrite28SDP_D, this, &MainWindow::writeToSDP_Disabled);
    connect(windowWrite28, &DialogWrite28::signalWindowWrite28SDP_E, this, &MainWindow::writeToSDP_Enabled);
    connect(windowWrite28, &DialogWrite28::signalModule28C, this, &MainWindow::module_28C);
    connect(windowWrite28, &DialogWrite28::signalPageWrite28C, this, &MainWindow::PageWrite_28C);

    windowWrite28F = new DialogWrite28F;
    windowWrite28F->setModal(true);
    windowWrite28F->setWindowFlags(windowWrite28F->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    connect(this, &MainWindow::sendDataWrite28F, windowWrite28F, &DialogWrite28F::receivedeDataWrite28F);

    windowSelVpp = new DialogSelVpp;
    windowSelVpp->setModal(true);
    windowSelVpp->setWindowFlags(windowSelVpp->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    connect(this, &MainWindow::sendDataSelVpp, windowSelVpp, &DialogSelVpp::receivedeDataSelVpp);

    QObject::connect(this, SIGNAL(chipUpdated()), this, SLOT(resizeBuffers()));
    updatePortsConnection = QObject::connect(&updatePortsTimer, SIGNAL(timeout()), this, SLOT(reload_ports()));
    updatePortsTimer.setInterval(2000);
    updatePortsTimer.start();
    reload_ports();
//    this->setFixedSize(QSize(390, 550));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *)
{
    windowChipInfo->close();
    windowLogicInfo->close();
}

void MainWindow::sendCommand(const QByteArray &data)
{
    // clear serial buffer
    serialPort->clear();
    serialPort->write(data);
}

void MainWindow::writeData(const QByteArray &data)
{
    // clear serial buffer
    serialPort->clear();
    serialPort->write(data);
}

void MainWindow::log(QString str, QString color="black")
{
    ui->textBrowser->append("<font color=\"" + color + "\">" + str + "</font>");
    ui->textBrowser->moveCursor(QTextCursor::End);
}

void MainWindow::on_clearLogButton_clicked()
{
    ui->textBrowser->clear();
}

void MainWindow::on_connectButton_clicked()
{
    QListWidgetItem* item = ui->portList->currentItem();
    if(item == NULL) {
        QMessageBox::critical(this, tr("EPROM Programmer"), tr("Select serial port!"));
        on_updateButton_clicked();
        return;
    }
    if(!(item->flags() & Qt::ItemIsSelectable)) {
        QMessageBox::critical(this, tr("EPROM Programmer"), tr("Port is busy!"));
        on_updateButton_clicked();
        return;
    }
    log(QString("Connect to %1").arg(item->data(Qt::UserRole).toString().remove(QRegExp("[\\\\.]"))));
    serialPort->setPortName(item->data(Qt::UserRole).toString());
    serialPort->setBaudRate(QSerialPort::Baud115200);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    if(serialPort->open(QIODevice::ReadWrite)) {
        serialPort->setDataTerminalReady(true); //DTR "1"
        serialDataConnection = QObject::connect(serialPort, SIGNAL(readyRead()), this, SLOT(openSerialPort()));
        openPortsConnection = QObject::connect(&openPortsTimer, SIGNAL(timeout()), this, SLOT(openSerialPort()));
        openPortsTimer.setInterval(3000);
        openPortsTimer.start();
    } else {
        QMessageBox::critical(this, tr("EPROM Programmer"), tr("Error - failed to open serial port!"));
        log("Failed to open serial port!", "red");
        on_updateButton_clicked();
    }
}

void MainWindow::openSerialPort()
{
    QObject::disconnect(openPortsConnection);
    QObject::disconnect(serialDataConnection);
    const QByteArray readData = serialPort->readAll();
    if(readData.simplified().indexOf("APT", 0) != -1) {
        log("Connect successful", "green");
        ui->updateButton->setEnabled(false);
        ui->disconnectButton->setEnabled(true);
        ui->connectButton->setEnabled(false);
        ui->portList->setEnabled(false);
        ui->typeList->setEnabled(true);
        on_typeList_currentChanged(ui->typeList->currentIndex());
        return;
    }
    log("Arduino programmer not found", "red");
    closeSerialPort();
}

void MainWindow::on_disconnectButton_clicked()
{
    closeSerialPort();
}

void MainWindow::closeSerialPort()
{
    chipSelected = false;
    dramSelected = false;
    logicSelected = false;
    dipGroup = 0;
    chipGroup = 0;
    bufferClear = true;
    eraseButton = false;
    bufferID28 = false;
    TestSRAM = 0;
    voltage = 0.0;
    ui->voltageChipButton->setChecked(false);
    ui->progressBar->setValue(0);
    updateButtons(false, false);
    ui->clearLogButton->setEnabled(true);
    chipSelectSetEnabled(false);
    windowChipInfo->close();
    windowLogicInfo->close();
    ui->showButton->setChecked(false);
    if(serialPort->isOpen()) {
        serialPort->close();
        log("Disconnect...", "red");
        QObject::disconnect(serialDataConnection);
    }
    on_updateButton_clicked();
    ui->portList->setEnabled(true);
}

void MainWindow::on_updateButton_clicked()
{
    ui->updateButton->setEnabled(false);
    ui->connectButton->setEnabled(false);
    reload_ports();
    updatePortsTimer.start();
}

void MainWindow::on_portList_itemClicked(QListWidgetItem *item)
{
    if(!serialPort->isOpen()) {
        (void)item;
        ui->connectButton->setEnabled(true);
        ui->updateButton->setEnabled(true);
        updatePortsTimer.stop();
    }
}

void MainWindow::reload_ports()
{
    ui->portList->clear();
    const auto infos = QSerialPortInfo::availablePorts();
    for(const QSerialPortInfo &info : infos) {
        QListWidgetItem *item = new QListWidgetItem(info.portName(), ui->portList);
        item->setData(Qt::UserRole, info.systemLocation());
        if(info.isBusy()) {
            item->setText(info.portName() + " (Busy)");
            item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        }
    }
}

void MainWindow::on_chipInfoButton_clicked()
{
    if(chipSelected || dramSelected) {
        if(pos().x() > 365) {
            windowChipInfo->move(pos().x()-365, pos().y());
        } else {
            windowChipInfo->move(pos().x()+405, pos().y());
        }
        windowChipInfo->show();
    }
    if(logicSelected) {
        if(pos().x() > 365) {
            windowLogicInfo->move(pos().x()-365, pos().y());
        } else {
            windowLogicInfo->move(pos().x()+405, pos().y());
        }
        windowLogicInfo->show();
    }
}

void MainWindow::updateButtons(bool actions, bool buffer)
{
    ui->disconnectButton->setEnabled(actions);
    ui->clearLogButton->setEnabled(actions);
    ui->chipType->setEnabled(actions);
    ui->autoDetectButton->setEnabled(actions);
    ui->autoDetectButton2->setEnabled(actions);
    ui->autoDetectButton3->setEnabled(actions);
    ui->autoDetectButton4->setEnabled(actions);
    ui->autoDetectButton5->setEnabled(actions);
    ui->autoDetectButton6->setEnabled(actions);
    ui->autoDetectButton7->setEnabled(actions);
    ui->openFileButton->setEnabled(buffer);
    if(actions) {
        ui->chipInfoButton->setEnabled(chipSelected || dramSelected || logicSelected);
        ui->voltageChipButton->setEnabled(typeGroup == 0 || typeGroup == 1);
        ui->IdentificationReadButton->setEnabled(chipGroup == 2 && buf28ID);
        ui->IdentificationReadButton2->setEnabled(chipGroup == 2 && buf28ID);
        ui->IdentificationReadButton3->setEnabled(chipGroup == 2 && buf28ID);
        ui->openID28Button->setEnabled(chipGroup == 2 && buf28ID);
        ui->openID28Button2->setEnabled(chipGroup == 2 && buf28ID);
        ui->openID28Button3->setEnabled(chipGroup == 2 && buf28ID);
        ui->testSRAMButton->setEnabled(chipGroup == 6);
        ui->testSRAMButton2->setEnabled(chipGroup == 6);
        ui->testSRAMButton3->setEnabled(chipGroup == 6);
        ui->readChipButton->setEnabled(chipSelected);
        ui->eraseChipButton->setEnabled(eraseButton);
        ui->testDRAMButton->setEnabled(dramSelected);
        ui->listLogic->setEnabled(listLogicOk);
        ui->testLogicButton->setEnabled(logicSelected);
        ui->autoTestingButton->setEnabled(listLogicOk);
    } else {
        ui->chipInfoButton->setEnabled(false);
        ui->voltageChipButton->setEnabled(false);
        ui->IdentificationReadButton->setEnabled(false);
        ui->IdentificationReadButton2->setEnabled(false);
        ui->IdentificationReadButton3->setEnabled(false);
        ui->openID28Button->setEnabled(false);
        ui->openID28Button2->setEnabled(false);
        ui->openID28Button3->setEnabled(false);
        ui->testSRAMButton->setEnabled(false);
        ui->testSRAMButton2->setEnabled(false);
        ui->testSRAMButton3->setEnabled(false);
        ui->readChipButton->setEnabled(false);
        ui->eraseChipButton->setEnabled(false);
        ui->testDRAMButton->setEnabled(false);
        ui->listLogic->setEnabled(false);
        ui->testLogicButton->setEnabled(false);
        ui->autoTestingButton->setEnabled(false);
    }
    if(buffer) {
        ui->IdentificationWriteButton->setEnabled(bufferID28);
        ui->IdentificationWriteButton2->setEnabled(bufferID28);
        ui->IdentificationWriteButton3->setEnabled(bufferID28);
        ui->saveID28Button->setEnabled(bufferID28);
        ui->saveID28Button2->setEnabled(bufferID28);
        ui->saveID28Button3->setEnabled(bufferID28);
        ui->saveFileButton->setEnabled(!bufferClear);
        ui->showButton->setEnabled(!bufferClear || bufferID28);
        ui->writeChipButton->setEnabled(!bufferClear);
        ui->verifyChipButton->setEnabled(!bufferClear);
    } else {
        ui->IdentificationWriteButton->setEnabled(false);
        ui->IdentificationWriteButton2->setEnabled(false);
        ui->IdentificationWriteButton3->setEnabled(false);
        ui->saveID28Button->setEnabled(false);
        ui->saveID28Button2->setEnabled(false);
        ui->saveID28Button3->setEnabled(false);
        ui->saveFileButton->setEnabled(false);
        ui->showButton->setEnabled(false);
        ui->writeChipButton->setEnabled(false);
        ui->verifyChipButton->setEnabled(false);
    }
}

void MainWindow::chipSelectSetEnabled(bool state)
{
    ui->E27c16Button->setEnabled(state);   
    ui->E27c32Button->setEnabled(state);
    ui->E27c64Button->setEnabled(state);
    ui->E27c128Button->setEnabled(state);
    ui->E27c256Button->setEnabled(state);
    ui->E27c512Button->setEnabled(state);
    ui->E27c010Button->setEnabled(state);
    ui->E27c020Button->setEnabled(state);
    ui->E27c040Button->setEnabled(state);
    ui->E27c080Button->setEnabled(state);

    ui->E28c04Button->setEnabled(state);
    ui->E28c16Button->setEnabled(state);
    ui->E28c17Button->setEnabled(state);
    ui->E28c64Button->setEnabled(state);
    ui->E28c256Button->setEnabled(state);
    ui->E28c512Button->setEnabled(state);
    ui->E28c010Button->setEnabled(state);
    ui->E28c020Button->setEnabled(state);
    ui->E28c040Button->setEnabled(state);

    ui->F28f256Button->setEnabled(state);
    ui->F28f512Button->setEnabled(state);
    ui->F28f010Button->setEnabled(state);
    ui->F28f020Button->setEnabled(state);

    ui->F29c256Button->setEnabled(state);
    ui->F29c512Button->setEnabled(state);
    ui->F29c010Button->setEnabled(state);
    ui->F29c020Button->setEnabled(state);
    ui->F29c040Button->setEnabled(state);

    ui->F29f512Button->setEnabled(state);
    ui->F29f010Button->setEnabled(state);
    ui->F29f020Button->setEnabled(state);
    ui->F29f040Button->setEnabled(state);

    ui->SRAM16Button->setEnabled(state);
    ui->SRAM32Button->setEnabled(state);
    ui->SRAM64Button->setEnabled(state);
    ui->SRAM128Button->setEnabled(state);
    ui->SRAM256Button->setEnabled(state);
    ui->SRAM512Button->setEnabled(state);
    ui->SRAM010Button->setEnabled(state);
    ui->SRAM020Button->setEnabled(state);
    ui->SRAM040Button->setEnabled(state);

    ui->D4164Button->setEnabled(state);
    ui->D41256Button->setEnabled(state);

    // Clear radio buttons for correct work after reconnect
    ui->E27c16Button->setAutoExclusive(state);   
    ui->E27c32Button->setAutoExclusive(state);
    ui->E27c64Button->setAutoExclusive(state);
    ui->E27c128Button->setAutoExclusive(state);
    ui->E27c256Button->setAutoExclusive(state);
    ui->E27c512Button->setAutoExclusive(state);
    ui->E27c010Button->setAutoExclusive(state);
    ui->E27c020Button->setAutoExclusive(state);
    ui->E27c040Button->setAutoExclusive(state);
    ui->E27c080Button->setAutoExclusive(state);

    ui->E28c04Button->setAutoExclusive(state);
    ui->E28c16Button->setAutoExclusive(state);
    ui->E28c17Button->setAutoExclusive(state);
    ui->E28c64Button->setAutoExclusive(state);
    ui->E28c256Button->setAutoExclusive(state);
    ui->E28c512Button->setAutoExclusive(state);
    ui->E28c010Button->setAutoExclusive(state);
    ui->E28c020Button->setAutoExclusive(state);
    ui->E28c040Button->setAutoExclusive(state);

    ui->F28f256Button->setAutoExclusive(state);
    ui->F28f512Button->setAutoExclusive(state);
    ui->F28f010Button->setAutoExclusive(state);
    ui->F28f020Button->setAutoExclusive(state);

    ui->F29c256Button->setAutoExclusive(state);
    ui->F29c512Button->setAutoExclusive(state);
    ui->F29c010Button->setAutoExclusive(state);
    ui->F29c020Button->setAutoExclusive(state);
    ui->F29c040Button->setAutoExclusive(state);

    ui->F29f512Button->setAutoExclusive(state);
    ui->F29f010Button->setAutoExclusive(state);
    ui->F29f020Button->setAutoExclusive(state);
    ui->F29f040Button->setAutoExclusive(state);

    ui->SRAM16Button->setAutoExclusive(state);
    ui->SRAM32Button->setAutoExclusive(state);
    ui->SRAM64Button->setAutoExclusive(state);
    ui->SRAM128Button->setAutoExclusive(state);
    ui->SRAM256Button->setAutoExclusive(state);
    ui->SRAM512Button->setAutoExclusive(state);
    ui->SRAM010Button->setAutoExclusive(state);
    ui->SRAM020Button->setAutoExclusive(state);
    ui->SRAM040Button->setAutoExclusive(state);

    ui->D4164Button->setAutoExclusive(state);
    ui->D41256Button->setAutoExclusive(state);

    if(!state) {
        ui->E27c16Button->setChecked(false);     
        ui->E27c32Button->setChecked(false);
        ui->E27c64Button->setChecked(false);
        ui->E27c128Button->setChecked(false);
        ui->E27c256Button->setChecked(false);
        ui->E27c512Button->setChecked(false);
        ui->E27c010Button->setChecked(false);
        ui->E27c020Button->setChecked(false);
        ui->E27c040Button->setChecked(false);
        ui->E27c080Button->setChecked(false);

        ui->E28c04Button->setChecked(false);
        ui->E28c16Button->setChecked(false);
        ui->E28c17Button->setChecked(false);
        ui->E28c64Button->setChecked(false);
        ui->E28c256Button->setChecked(false);
        ui->E28c512Button->setChecked(false);
        ui->E28c010Button->setChecked(false);
        ui->E28c020Button->setChecked(false);
        ui->E28c040Button->setChecked(false);

        ui->F28f256Button->setChecked(false);
        ui->F28f512Button->setChecked(false);
        ui->F28f010Button->setChecked(false);
        ui->F28f020Button->setChecked(false);

        ui->F29c256Button->setChecked(false);
        ui->F29c512Button->setChecked(false);
        ui->F29c010Button->setChecked(false);
        ui->F29c020Button->setChecked(false);
        ui->F29c040Button->setChecked(false);

        ui->F29f512Button->setChecked(false);
        ui->F29f010Button->setChecked(false);
        ui->F29f020Button->setChecked(false);
        ui->F29f040Button->setChecked(false);

        ui->SRAM16Button->setChecked(false);
        ui->SRAM32Button->setChecked(false);
        ui->SRAM64Button->setChecked(false);
        ui->SRAM128Button->setChecked(false);
        ui->SRAM256Button->setChecked(false);
        ui->SRAM512Button->setChecked(false);
        ui->SRAM010Button->setChecked(false);
        ui->SRAM020Button->setChecked(false);
        ui->SRAM040Button->setChecked(false);

        ui->D4164Button->setChecked(state);
        ui->D41256Button->setChecked(state);
    }
}

void MainWindow::on_typeList_currentChanged(int index)
{
    dipGroup = 0;
    chipGroup = 0;
    listLogicOk = false;
    chipSelected = false;
    dramSelected = false;
    logicSelected = false;
    eraseButton = false;
    bufferClear = true;
    bufferID28 = false;
    ui->showButton->setChecked(false);
    windowChipInfo->close();
    windowLogicInfo->close();
    ui->voltageChipButton->setChecked(false);
    ui->progressBar->setValue(0);
    TestSRAM = 0;
    chipSelectSetEnabled(false);
    updateButtons(false, false);
    chipSelectSetEnabled(true);
    if(index == 0) {
        typeGroup = 0;
        ui->EPROM->setEnabled(true);
        ui->E_algorithmList->setEnabled(true);
        log("Mode of operation with EPROM memory chips");
        on_E_algorithmList_currentChanged(ui->E_algorithmList->currentIndex());
    } else
    if(index == 1) {
        typeGroup = 1;
        ui->FLASH->setEnabled(true);
        ui->F_algorithmList->setEnabled(true);
        log("Mode of operation with FLASH memory chips");
        on_F_algorithmList_currentChanged(ui->F_algorithmList->currentIndex());
    } else
    if(index == 2) {
        typeGroup = 2;
        ui->RAM->setEnabled(true);
        ui->R_algorithmList->setEnabled(true);
        log("Mode of operation with RAM memory chips");
        on_R_algorithmList_currentChanged(ui->R_algorithmList->currentIndex());
    } else
    if(index == 3) {    
        sendCommand("l");
        typeGroup = 3;
        log("Logic IC testing mode");
        ui->listLogic->clear();
        listLogic.clear();
        QFile fileCode("database_logic.txt");
        if(!fileCode.open(QIODevice::ReadOnly | QIODevice::Text)) {
            log("File database_logic.txt not found", "red");
        } else {
            QTextStream in(&fileCode);
            while(!in.atEnd()) {
                QString line = in.readLine().trimmed();
                QRegExp rx(QString("^\\$(4|7)[0-9]{3,4}$"));
                if(rx.indexIn(line) != -1) {
                    line = line.remove(QChar('$'));
                    ui->listLogic->addItem(line);
                    listLogic.append(line);
                 }
            }
            fileCode.close();
            countListLogic = listLogic.count();
            if(countListLogic > 0) {
                log(QString("Number of chips in the database - %1").arg(countListLogic));
                listLogicOk = true;
            } else {
                log("Сhips not found in the database", "red");
            }
        }
    }
    updateButtons(true, false);
}

void MainWindow::on_E_algorithmList_currentChanged(int index)
{
    dipGroup = 0;
    chipGroup = 0;
    chipSelected = false;
    bufferClear = true;
    eraseButton = false;
    bufferID28 = false;
    windowChipInfo->close();
    ui->showButton->setChecked(false);
    ui->progressBar->setValue(0);
    chipSelectSetEnabled(false);
    updateButtons(false, false);
    chipSelectSetEnabled(true);
    if(index == 0) {
        ui->E27C_dipList->setTabEnabled(3, true);
        ui->E27C_dipList->setCurrentIndex(3);
        log("Algorithm 27Cxxx selected");
    } else
    if(index == 1) {
        ui->E28C_dipList->setTabEnabled(3, true);
        ui->E28C_dipList->setCurrentIndex(3);
        log("Algorithm 28Cxxx selected");
    }
    updateButtons(true, false);
}

void MainWindow::on_F_algorithmList_currentChanged(int index)
{
    dipGroup = 0;
    chipGroup = 0;
    chipSelected = false;
    bufferClear = true;
    eraseButton = false;
    windowChipInfo->close();
    ui->showButton->setChecked(false);
    ui->progressBar->setValue(0);
    chipSelectSetEnabled(false);
    updateButtons(false, false);
    chipSelectSetEnabled(true);
    if(index == 0) {
        ui->F28F_dipList->setTabEnabled(1, true);
        ui->F28F_dipList->setCurrentIndex(1);
        log("Algorithm 28Fxxx selected");
    } else
    if(index == 1) {
        ui->F29C_dipList->setTabEnabled(2, true);
        ui->F29C_dipList->setCurrentIndex(2);
        log("Algorithm 29Cxxx selected");
    }
    if(index == 2) {
        ui->F29F_dipList->setTabEnabled(1, true);
        ui->F29F_dipList->setCurrentIndex(1);
        log("Algorithm 29Fxxx selected");
    }
    updateButtons(true, false);
}

void MainWindow::on_R_algorithmList_currentChanged(int index)
{
    dipGroup = 0;
    chipGroup = 0;
    chipSelected = false;
    dramSelected = false;
    bufferClear = true;
    windowChipInfo->close();
    ui->showButton->setChecked(false);
    ui->progressBar->setValue(0);
    chipSelectSetEnabled(false);
    updateButtons(false, false);
    chipSelectSetEnabled(true);
    if(index == 0) {
        ui->SRAM_dipList->setTabEnabled(3, true);
        ui->SRAM_dipList->setCurrentIndex(3);
        log("Algorithm SRAM selected");
    } else
    if(index == 1) {
        sendCommand("j");
        log("Algorithm DRAM selected");

    }
    updateButtons(true, false);
}

void MainWindow::on_E27C_dipList_currentChanged(int index)
{
    dipGroup = 0;
    chipGroup = 0;
    chipSelected = false;
    bufferClear = true;
    eraseButton = false;
    windowChipInfo->close();
    ui->showButton->setChecked(false);
    ui->voltageChipButton->setChecked(false);
    ui->progressBar->setValue(0);
    chipSelectSetEnabled(false);
    updateButtons(false, false);
    chipSelectSetEnabled(true);
    windowSelDip->move(pos().x()+65, pos().y()+160);
    if(index == 0) {
        dipGroup = 1;
        emit sendDataSelDip(dipGroup);
        int dialogCode = windowSelDip->exec();
        if(dialogCode == QDialog::Accepted) {
            sendCommand("max");
            log("Selected DIP24");
            ui->E27C_dipList->setTabEnabled(3, false);
        } else {
            ui->E27C_dipList->setTabEnabled(3, true);
            ui->E27C_dipList->setCurrentIndex(3);
            log("You have not confirmed the action");
        }
    } else
    if(index == 1) {
        dipGroup = 2;
        emit sendDataSelDip(dipGroup);
        int dialogCode = windowSelDip->exec();
        if(dialogCode == QDialog::Accepted) {
            sendCommand("may");
            log("Selected DIP28");
            ui->E27C_dipList->setTabEnabled(3, false);
        } else {
            ui->E27C_dipList->setTabEnabled(3, true);
            ui->E27C_dipList->setCurrentIndex(3);
            log("You have not confirmed the action");
        }
    } else
    if(index == 2) {
        dipGroup = 3;
        emit sendDataSelDip(dipGroup);
        int dialogCode = windowSelDip->exec();
        if(dialogCode == QDialog::Accepted) {
            sendCommand("maz");
            log("Selected DIP32");
            ui->E27C_dipList->setTabEnabled(3, false);
        } else {
            ui->E27C_dipList->setTabEnabled(3, true);
            ui->E27C_dipList->setCurrentIndex(3);
            log("You have not confirmed the action");
        }
    }
    updateButtons(true, false);
}

void MainWindow::on_E28C_dipList_currentChanged(int index)
{
    dipGroup = 0;
    chipGroup = 0;
    chipSelected = false;
    bufferClear = true;
    eraseButton = false;
    bufferID28 = false;
    windowChipInfo->close();
    ui->showButton->setChecked(false);
    ui->voltageChipButton->setChecked(false);
    ui->progressBar->setValue(0);
    chipSelectSetEnabled(false);
    updateButtons(false, false);
    chipSelectSetEnabled(true);
    windowSelDip->move(pos().x()+65, pos().y()+160);
    if(index == 0) {
        dipGroup = 1;
        emit sendDataSelDip(dipGroup);
        int dialogCode = windowSelDip->exec();
        if(dialogCode == QDialog::Accepted) {
            sendCommand("mbx");
            log("Selected DIP24");
            ui->E28C_dipList->setTabEnabled(3, false);
        } else {
            ui->E28C_dipList->setTabEnabled(3, true);
            ui->E28C_dipList->setCurrentIndex(3);
            log("You have not confirmed the action");
        }
    } else
    if(index == 1) {
        dipGroup = 2;
        emit sendDataSelDip(dipGroup);
        int dialogCode = windowSelDip->exec();
        if(dialogCode == QDialog::Accepted) {
            sendCommand("mby");
            log("Selected DIP28");
            ui->E28C_dipList->setTabEnabled(3, false);
        } else {
            ui->E28C_dipList->setTabEnabled(3, true);
            ui->E28C_dipList->setCurrentIndex(3);
            log("You have not confirmed the action");
        }
    } else
    if(index == 2) {
        dipGroup = 3;
        emit sendDataSelDip(dipGroup);
        int dialogCode = windowSelDip->exec();
        if(dialogCode == QDialog::Accepted) {
            sendCommand("mbz");
            log("Selected DIP32");
            ui->E28C_dipList->setTabEnabled(3, false);
        } else {
            ui->E28C_dipList->setTabEnabled(3, true);
            ui->E28C_dipList->setCurrentIndex(3);
            log("You have not confirmed the action");
        }
    }
    updateButtons(true, false);
}

void MainWindow::on_F28F_dipList_currentChanged(int index)
{
    dipGroup = 0;
    chipGroup = 0;
    chipSelected = false;
    bufferClear = true;
    eraseButton = false;
    windowChipInfo->close();
    ui->showButton->setChecked(false);
    ui->voltageChipButton->setChecked(false);
    ui->progressBar->setValue(0);
    chipSelectSetEnabled(false);
    updateButtons(false, false);
    chipSelectSetEnabled(true);
    windowSelDip->move(pos().x()+65, pos().y()+160);
    if(index == 0) {
        dipGroup = 3;
        emit sendDataSelDip(dipGroup);
        int dialogCode = windowSelDip->exec();
        if(dialogCode == QDialog::Accepted) {
            sendCommand("mcz");
            log("Selected DIP32");
            ui->F28F_dipList->setTabEnabled(1, false);
        } else {
            ui->F28F_dipList->setTabEnabled(1, true);
            ui->F28F_dipList->setCurrentIndex(1);
            log("You have not confirmed the action");
        }
    }
    updateButtons(true, false);
}

void MainWindow::on_F29C_dipList_currentChanged(int index)
{
    dipGroup = 0;
    chipGroup = 0;
    chipSelected = false;
    bufferClear = true;
    eraseButton = false;
    windowChipInfo->close();
    ui->showButton->setChecked(false);
    ui->voltageChipButton->setChecked(false);
    ui->progressBar->setValue(0);
    chipSelectSetEnabled(false);
    updateButtons(false, false);
    chipSelectSetEnabled(true);
    windowSelDip->move(pos().x()+65, pos().y()+160);
    if(index == 0) {
        dipGroup = 2;
        emit sendDataSelDip(dipGroup);
        int dialogCode = windowSelDip->exec();
        if(dialogCode == QDialog::Accepted) {
            sendCommand("mdy");
            log("Selected DIP28");
            ui->F29C_dipList->setTabEnabled(2, false);
        } else {
            ui->F29C_dipList->setTabEnabled(2, true);
            ui->F29C_dipList->setCurrentIndex(2);
            log("You have not confirmed the action");
        }
    } else
    if(index == 1) {
        dipGroup = 3;
        emit sendDataSelDip(dipGroup);
        int dialogCode = windowSelDip->exec();
        if(dialogCode == QDialog::Accepted) {
            sendCommand("mdz");
            log("Selected DIP32");
            ui->F29C_dipList->setTabEnabled(2, false);
        } else {
            ui->F29C_dipList->setTabEnabled(2, true);
            ui->F29C_dipList->setCurrentIndex(2);
            log("You have not confirmed the action");
        }
    }
    updateButtons(true, false);
}

void MainWindow::on_F29F_dipList_currentChanged(int index)
{
    dipGroup = 0;
    chipGroup = 0;
    chipSelected = false;
    bufferClear = true;
    eraseButton = false;
    windowChipInfo->close();
    ui->showButton->setChecked(false);
    ui->voltageChipButton->setChecked(false);
    ui->progressBar->setValue(0);
    chipSelectSetEnabled(false);
    updateButtons(false, false);
    chipSelectSetEnabled(true);
    windowSelDip->move(pos().x()+65, pos().y()+160);
    if(index == 0) {
        dipGroup = 3;
        emit sendDataSelDip(dipGroup);
        int dialogCode = windowSelDip->exec();
        if(dialogCode == QDialog::Accepted) {
            sendCommand("mfz");
            log("Selected DIP32");
            ui->F29F_dipList->setTabEnabled(1, false);
        } else {
            ui->F29F_dipList->setTabEnabled(1, true);
            ui->F29F_dipList->setCurrentIndex(1);
            log("You have not confirmed the action");
        }
    }
    updateButtons(true, false);
}

void MainWindow::on_SRAM_dipList_currentChanged(int index)
{
    dipGroup = 0;
    chipGroup = 0;
    chipSelected = false;
    bufferClear = true;
    eraseButton = false;
    windowChipInfo->close();
    ui->showButton->setChecked(false);
    ui->voltageChipButton->setChecked(false);
    ui->progressBar->setValue(0);
    chipSelectSetEnabled(false);
    updateButtons(false, false);
    chipSelectSetEnabled(true);
    windowSelDip->move(pos().x()+65, pos().y()+160);
    if(index == 0) {
        dipGroup = 1;
        emit sendDataSelDip(dipGroup);
        int dialogCode = windowSelDip->exec();
        if(dialogCode == QDialog::Accepted) {
            sendCommand("mgx");
            log("Selected DIP24");
            ui->SRAM_dipList->setTabEnabled(3, false);
        } else {
            ui->SRAM_dipList->setTabEnabled(3, true);
            ui->SRAM_dipList->setCurrentIndex(3);
            log("You have not confirmed the action");
        }
    } else
    if(index == 1) {
        dipGroup = 2;
        emit sendDataSelDip(dipGroup);
        int dialogCode = windowSelDip->exec();
        if(dialogCode == QDialog::Accepted) {
            sendCommand("mgy");
            log("Selected DIP28");
            ui->SRAM_dipList->setTabEnabled(3, false);
        } else {
            ui->SRAM_dipList->setTabEnabled(3, true);
            ui->SRAM_dipList->setCurrentIndex(3);
            log("You have not confirmed the action");
        }
    } else
    if(index == 2) {
        dipGroup = 3;
        emit sendDataSelDip(dipGroup);
        int dialogCode = windowSelDip->exec();
        if(dialogCode == QDialog::Accepted) {
            sendCommand("mgz");
            log("Selected DIP32");
            ui->SRAM_dipList->setTabEnabled(3, false);
        } else {
            ui->SRAM_dipList->setTabEnabled(3, true);
            ui->SRAM_dipList->setCurrentIndex(3);
            log("You have not confirmed the action");
        }
    }
    updateButtons(true, false);
}

void MainWindow::resizeBuffers()
{
    chipSelected = true;
    ui->chipInfoButton->setEnabled(true);
    ui->openFileButton->setEnabled(true);
    ui->readChipButton->setEnabled(true);
    bufWork.resize(bufSize);
    bufCheck.resize(bufSize);
    bufCheck.fill(0);
    emit bufferUpdated();
}

void MainWindow::on_E27c16Button_clicked()
{
    Chip = "27C16";
    bufSize = 0x07ff + 1;
    chipGroup = 1;
    CEPulse = 10000;
    RadioButton_clicked();
    sendCommand("1");
    emit chipUpdated();
}

void MainWindow::on_E27c32Button_clicked()
{
    Chip = "27C32";
    bufSize = 0x0fff + 1;
    chipGroup = 1;
    CEPulse = 10000;
    RadioButton_clicked();
    sendCommand("2");
    emit chipUpdated();
}

void MainWindow::on_E27c64Button_clicked()
{
    Chip = "27C64";
    bufSize = 0x1fff + 1;
    chipGroup = 1;
    CEPulse = 1000;
    RadioButton_clicked();
    sendCommand("3");
    emit chipUpdated();
}

void MainWindow::on_E27c128Button_clicked()
{
    Chip = "27C128";
    bufSize = 0x3fff + 1;
    chipGroup = 1;
    CEPulse = 1000;
    RadioButton_clicked();
    sendCommand("4");
    emit chipUpdated();
}

void MainWindow::on_E27c256Button_clicked()
{
    Chip = "27C256";
    bufSize = 0x7fff + 1;
    chipGroup = 1;
    CEPulse = 100;
    RadioButton_clicked();
    sendCommand("5");
    emit chipUpdated();
}

void MainWindow::on_E27c512Button_clicked()
{
    Chip = "27C512";
    bufSize = 0xffff + 1;
    chipGroup = 1;
    CEPulse = 100;
    RadioButton_clicked();
    sendCommand("6");
    emit chipUpdated();
}

void MainWindow::on_E27c010Button_clicked()
{
    Chip = "27C010";
    bufSize = 0x1ffff + 1;
    chipGroup = 1;
    CEPulse = 100;
    RadioButton_clicked();
    sendCommand("7");
    emit chipUpdated();
}

void MainWindow::on_E27c020Button_clicked()
{
    Chip = "27C020";
    bufSize = 0x3ffff + 1;
    chipGroup = 1;
    CEPulse = 100;
    RadioButton_clicked();
    sendCommand("8");
    emit chipUpdated();
}

void MainWindow::on_E27c040Button_clicked()
{
    Chip = "27C040";
    bufSize = 0x7ffff + 1;
    chipGroup = 1;
    CEPulse = 50;
    RadioButton_clicked();
    sendCommand("9");
    emit chipUpdated();
}

void MainWindow::on_E27c080Button_clicked()
{
    Chip = "27C080";
    bufSize = 0xfffff + 1;
    chipGroup = 1;
    CEPulse = 50;
    RadioButton_clicked();
    sendCommand("0");
    emit chipUpdated();
}

void MainWindow::on_E28c04Button_clicked()
{
    Chip = "28C04";
    bufSize = 0x01ff + 1;
    PageWrite = 1;
    buf28ID = 0;
    chipGroup = 2;
    RadioButton_clicked();
    sendCommand("1");
    emit chipUpdated();
}

void MainWindow::on_E28c16Button_clicked()
{
    Chip = "28C16";
    bufSize = 0x07ff + 1;
    PageWrite = 1;
    buf28ID = 32;
    chipGroup = 2;
    RadioButton_clicked();
    sendCommand("2");
    emit chipUpdated();
}

void MainWindow::on_E28c17Button_clicked()
{
    Chip = "28C17";
    bufSize = 0x07ff + 1;
    PageWrite = 1;
    buf28ID = 32;
    chipGroup = 2;
    RadioButton_clicked();
    sendCommand("3");
    emit chipUpdated();
}

void MainWindow::on_E28c64Button_clicked()
{
    Chip = "28C64";
    bufSize = 0x1fff + 1;
    PageWrite = 1;
    buf28ID = 32;
    chipGroup = 2;
    RadioButton_clicked();
    sendCommand("4");
    emit chipUpdated();
}

void MainWindow::on_E28c256Button_clicked()
{
    Chip = "28C256";
    bufSize = 0x7fff + 1;
    PageWrite = 64;
    buf28ID = 64;
    chipGroup = 2;
    RadioButton_clicked();
    sendCommand("5");
    emit chipUpdated();
}

void MainWindow::on_E28c512Button_clicked()
{
    Chip = "28C512";
    bufSize = 0xffff + 1;
    PageWrite = 128;
    buf28ID = 0;
    chipGroup = 2;
    RadioButton_clicked();
    sendCommand("6");
    emit chipUpdated();
}

void MainWindow::on_E28c010Button_clicked()
{
    Chip = "28C010";
    bufSize = 0x1ffff + 1;
    PageWrite = 128;
    buf28ID = 128;
    chipGroup = 2;
    RadioButton_clicked();
    sendCommand("7");
    emit chipUpdated();
}

void MainWindow::on_E28c020Button_clicked()
{
    Chip = "28C020";
    bufSize = 0x3ffff + 1;
    PageWrite = 128;
    buf28ID = 0;
    chipGroup = 2;
    RadioButton_clicked();
    sendCommand("8");
    emit chipUpdated();
}

void MainWindow::on_E28c040Button_clicked()
{
    Chip = "28C040";
    bufSize = 0x7ffff + 1;
    PageWrite = 256;
    buf28ID = 128;
    chipGroup = 2;
    RadioButton_clicked();
    sendCommand("9");
    emit chipUpdated();
}

void MainWindow::on_F28f256Button_clicked()
{
    Chip = "28F256";
    bufSize = 0x7fff + 1;
    chipGroup = 3;
    RadioButton_clicked();
    sendCommand("1");
    emit chipUpdated();
}

void MainWindow::on_F28f512Button_clicked()
{
    Chip = "28F512";
    bufSize = 0xffff + 1;
    chipGroup = 3;
    RadioButton_clicked();
    sendCommand("2");
    emit chipUpdated();
}

void MainWindow::on_F28f010Button_clicked()
{
    Chip = "28F010";
    bufSize = 0x1ffff + 1;
    chipGroup = 3;
    RadioButton_clicked();
    sendCommand("3");
    emit chipUpdated();
}

void MainWindow::on_F28f020Button_clicked()
{
    Chip = "28F020";
    bufSize = 0x3ffff + 1;
    chipGroup = 3;
    RadioButton_clicked();
    sendCommand("4");
    emit chipUpdated();
}

void MainWindow::on_F29c256Button_clicked()
{
    Chip = "29C256";
    bufSize = 0x7fff + 1;
    PageWrite = 64;
    chipGroup = 4;
    RadioButton_clicked();
    sendCommand("1");
    emit chipUpdated();
}

void MainWindow::on_F29c512Button_clicked()
{
    Chip = "29C512";
    bufSize = 0xffff + 1;
    PageWrite = 128;
    chipGroup = 4;
    RadioButton_clicked();
    sendCommand("2");
    emit chipUpdated();
}

void MainWindow::on_F29c010Button_clicked()
{
    Chip = "29C010";
    bufSize = 0x1ffff + 1;
    PageWrite = 128;
    chipGroup = 4;
    RadioButton_clicked();
    sendCommand("3");
    emit chipUpdated();
}

void MainWindow::on_F29c020Button_clicked()
{
    Chip = "29C020";
    bufSize = 0x3ffff + 1;
    PageWrite = 128;
    chipGroup = 4;
    RadioButton_clicked();
    sendCommand("4");
    emit chipUpdated();
}

void MainWindow::on_F29c040Button_clicked()
{
    Chip = "29C040";
    bufSize = 0x7ffff + 1;
    PageWrite = 256;
    chipGroup = 4;
    RadioButton_clicked();
    sendCommand("5");
    emit chipUpdated();
}

void MainWindow::on_F29f512Button_clicked()
{
    Chip = "29F512";
    bufSize = 0xffff + 1;
    chipGroup = 5;
    RadioButton_clicked();
    sendCommand("1");
    emit chipUpdated();
}

void MainWindow::on_F29f010Button_clicked()
{
    Chip = "29F010";
    bufSize = 0x1ffff + 1;
    chipGroup = 5;
    RadioButton_clicked();
    sendCommand("2");
    emit chipUpdated();
}

void MainWindow::on_F29f020Button_clicked()
{
    Chip = "29F020";
    bufSize = 0x3ffff + 1;
    chipGroup = 5;
    RadioButton_clicked();
    sendCommand("3");
    emit chipUpdated();
}

void MainWindow::on_F29f040Button_clicked()
{
    Chip = "29F040";
    bufSize = 0x7ffff + 1;
    chipGroup = 5;
    RadioButton_clicked();
    sendCommand("4");
    emit chipUpdated();
}

void MainWindow::on_SRAM16Button_clicked()
{
    Chip = "SRAM 16Kbit";
    bufSize = 0x07ff + 1;
    chipGroup = 6;
    RadioButton_clicked();
    sendCommand("1");
    emit chipUpdated();
}

void MainWindow::on_SRAM32Button_clicked()
{
    Chip = "SRAM 32Kbit";
    bufSize = 0x0fff + 1;
    chipGroup = 6;
    RadioButton_clicked();
    sendCommand("2");
    emit chipUpdated();
}

void MainWindow::on_SRAM64Button_clicked()
{
    Chip = "SRAM 64Kbit";
    bufSize = 0x1fff + 1;
    chipGroup = 6;
    RadioButton_clicked();
    sendCommand("3");
    emit chipUpdated();
}

void MainWindow::on_SRAM128Button_clicked()
{
    Chip = "SRAM 128Kbit";
    bufSize = 0x3fff + 1;
    chipGroup = 6;
    RadioButton_clicked();
    sendCommand("4");
    emit chipUpdated();
}

void MainWindow::on_SRAM256Button_clicked()
{
    Chip = "SRAM 256Kbit";
    bufSize = 0x7fff + 1;
    chipGroup = 6;
    RadioButton_clicked();
    sendCommand("5");
    emit chipUpdated();
}

void MainWindow::on_SRAM512Button_clicked()
{
    Chip = "SRAM 512Kbit";
    bufSize = 0xffff + 1;
    chipGroup = 6;
    RadioButton_clicked();
    sendCommand("6");
    emit chipUpdated();
}

void MainWindow::on_SRAM010Button_clicked()
{
    Chip = "SRAM 1024Kbit";
    bufSize = 0x1ffff + 1;
    chipGroup = 6;
    RadioButton_clicked();
    sendCommand("7");
    emit chipUpdated();
}

void MainWindow::on_SRAM020Button_clicked()
{
    Chip = "SRAM 2048Kbit";
    bufSize = 0x3ffff + 1;
    chipGroup = 6;
    RadioButton_clicked();
    sendCommand("8");
    emit chipUpdated();
}

void MainWindow::on_SRAM040Button_clicked()
{
    Chip = "SRAM 4096Kbit";
    bufSize = 0x7ffff + 1;
    chipGroup = 6;
    RadioButton_clicked();
    sendCommand("9");
    emit chipUpdated();
}

void MainWindow::RadioButton_clicked()
{
    if(chipGroup == 2 || chipGroup == 3 || chipGroup == 4 || chipGroup == 5 || eraseSet) {eraseButton = true;} else {eraseButton = false;}
    eraseSet = false;
    WandVSet = false;
    EandRSet = false;
    SoftEraseEnable = false;
    SDP_Disabled = false;
    SDP_Enabled = false;
    Module = 0;
    TestSRAM = 0;
    VPP = "Unknown";
    VPE = "Unknown";
    bufferID28 = false;
    bufferClear = true;
    ui->showButton->setChecked(false);
    ui->voltageChipButton->setChecked(false);
    ui->progressBar->setValue(0);
    updateButtons(true, false);
    if(Chip != "Unknown") {log(QString("Selected chip %1").arg(Chip));}
    emit sendChipInfo(Chip, bufSize, dipGroup, chipGroup);
}

void MainWindow::autoDetect()
{
    windowChipInfo->close();
    chipSelected = false;
    chipGroup = 0;
    Chip = "Unknown";
    RadioButton_clicked();
    updateButtons(false, false);
    chipSelectSetEnabled(false);
    if(SoftDetect == 1) {
        signatureCodeStart();
    } else {
        signatureCodeConnection = QObject::connect(this, SIGNAL(signatureCodeSignal()), this, SLOT(signatureCodeStart()));
        getVoltage();
    }
    log(QString("Reading device signature code from chip..."));
    chipSelectSetEnabled(true);
}

void MainWindow::on_autoDetectButton_clicked()
{
    Detect = 1;
    SoftDetect = 0;
    autoDetect();
}

void MainWindow::on_autoDetectButton2_clicked()
{
    Detect = 2;
    SoftDetect = 0;
    autoDetect();
}

void MainWindow::on_autoDetectButton3_clicked()
{
    Detect = 3;
    SoftDetect = 0;
    autoDetect();
}

void MainWindow::on_autoDetectButton4_clicked()
{
    Detect = 4;
    SoftDetect = 0;
    autoDetect();
}

void MainWindow::on_autoDetectButton5_clicked()
{
    Detect = 5;
    SoftDetect = 0;
    autoDetect();
}

void MainWindow::on_autoDetectButton6_clicked()
{
    Detect = 6;
    SoftDetect = 0;
    autoDetect();
}

void MainWindow::on_autoDetectButton7_clicked()
{
    Detect = 7;
    SoftDetect = 0;
    autoDetect();
}

void MainWindow::on_SoftDetectButton_clicked()
{
    Detect = 5;
    SoftDetect = 1;
    autoDetect();
}

void MainWindow::on_SoftDetectButton2_clicked()
{
    Detect = 6;
    SoftDetect = 1;
    autoDetect();
}

void MainWindow::signatureCodeStart()
{
    if(SoftDetect == 1) {
        serialDataConnection = QObject::connect(serialPort, SIGNAL(readyRead()), this, SLOT(signatureCode()));
        sendCommand("h");
    } else {
        QObject::disconnect(signatureCodeConnection);
        if(voltage < 11.5 || voltage > 12.5) {
            log("Incorrect voltage on pin A9", "red");
            log("Set voltage to 11.5 - 12.5 V");
            updateButtons(true, false);
            return;
        }
        serialDataConnection = QObject::connect(serialPort, SIGNAL(readyRead()), this, SLOT(signatureCode()));
        sendCommand("i");
    }
}

void MainWindow::signatureCode()
{
    QObject::disconnect(serialDataConnection);
    updateButtons(true, false);
    uint8_t countByte = 0;
    QString pattern;
    QString sig1;
    QString sig2;
    QString sig3;
    QString sig4;
    QString sigCode = "Unknown";
    QString type = "Unknown";
    QString manufacturer = "Unknown";
    QString name = "Unknown";
    QString tempVPP = "Unknown";
    QString tempVPE = "Unknown";
    const QByteArray data = serialPort->readAll();
    if(data.toHex().toUpper() == "45") {
        log("Connection error!", "red");
        return;
    }
    if(data.toHex().toUpper() == "46") {
        log("Read signature code error. Incorrect voltage pin A9", "red");
        return;
    }
    if(dipGroup == 1) {countByte = 4;}
    else if(dipGroup == 2) {if(SoftDetect == 1) {countByte = 2;} else {countByte = 6;}}
    else if(dipGroup == 3) {if(SoftDetect == 1) {countByte = 2;} else {countByte = 8;}}
    else {
        log("Error! DIP package type not selected", "red");
        return;
    }
    if(data.count() == countByte) {
        QString signature = data.toHex(0).toUpper();
        if(countByte == 2) {
            sig1 = signature.mid(0,4);
        }
        if(countByte == 4) {
            sig1 = signature.mid(0,4);
            sig2 = signature.mid(4,4);
        }
        if(countByte == 6) {
            sig1 = signature.mid(0,4);
            sig2 = signature.mid(4,4);
            sig3 = signature.mid(8,4);
        }
        if(countByte == 8) {
            sig1 = signature.mid(0,4);
            sig2 = signature.mid(4,4);
            sig3 = signature.mid(8,4);
            sig4 = signature.mid(12,4);
        }
        QFile fileCode("chiplist.txt");
        if(!fileCode.open(QIODevice::ReadOnly | QIODevice::Text)) {
            log("File chiplist.txt not found", "red");
            return;
        }
        QTextStream in(&fileCode);
        while(!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if(countByte == 2) {
                pattern = QString("^(%1):").arg(sig1);
            }
            if(countByte == 4) {
                pattern = QString("^(%1|%2):").arg(sig1, sig2);
            }
            if(countByte == 6) {
                pattern = QString("^(%1|%2|%3):").arg(sig1, sig2, sig3);
            }
            if(countByte == 8) {
                pattern = QString("^(%1|%2|%3|%4):").arg(sig1, sig2, sig3, sig4);
            }
            QRegExp rx(pattern, Qt::CaseInsensitive);
            if(rx.indexIn(line) != -1) {
                QStringList temp = line.split(":");
                for(uint8_t i = 0; i < temp.count(); i++) {
                    if(i == 0) {sigCode = temp[i].simplified();}
                    if(i == 1) {type = temp[i].simplified();}
                    if(i == 2) {manufacturer = temp[i].simplified();}
                    if(i == 3) {name = temp[i].simplified();}
                    if(i == 4) {tempVPP = temp[i].simplified();}
                    if(i == 5) {tempVPE = temp[i].simplified();}
                    if(i > 5)  {break;}
                }
                break;
            }
        }
        fileCode.close();
        if(sigCode == "" || sigCode == "Unknown") {
            sigCode = "Unknown";
            log(QString("Signature: %1").arg(sigCode), "red");
        } else {
            log(QString("Signature: %1").arg(sigCode), "blue");
        }
        if(sigCode != "Unknown") {
            if(type == "" || type == "Unknown") {
                type = "Unknown";
                log(QString("Type: %1").arg(type ), "red");
            } else {
                log(QString("Type: %1").arg(type ), "blue");
            }
            if(manufacturer == "" || manufacturer == "Unknown") {
                manufacturer = "Unknown";
                log(QString("Manufactured: %1").arg(manufacturer), "red");
            } else {
                log(QString("Manufactured: %1").arg(manufacturer), "blue");
            }
            if(name == "" || name == "Unknown") {
                name = "Unknown";
                log(QString("Name: %1").arg(name), "red");
            } else {
                log(QString("Name: %1").arg(name), "blue");
            }
            if(tempVPP.toFloat() > 5) {
                log(QString("VPP: %1").arg(tempVPP), "blue");
            } else {
                tempVPP = "Unknown";
            }
            if(tempVPE.toFloat() > 5) {
                eraseSet = true;
                log(QString("VPE: %1").arg(tempVPE), "blue");
             } else {
                tempVPE = "Unknown";
             }
            if(Detect == 1) {
                if(type == "27C16") {
                    on_E27c16Button_clicked();
                    ui->E27c16Button->setChecked(true);
                } else if(type == "27C32") {
                    on_E27c32Button_clicked();
                    ui->E27c32Button->setChecked(true);
                } else {
                    log("Chip not selected. Doesn't match type", "red");
                    eraseSet = false;
                }
            }
            if(Detect == 2) {
                if(type == "27C64") {
                    on_E27c64Button_clicked();
                    ui->E27c64Button->setChecked(true);
                } else if(type == "27C128") {
                    on_E27c128Button_clicked();
                    ui->E27c128Button->setChecked(true);
                } else if(type == "27C256") {
                    on_E27c256Button_clicked();
                    ui->E27c256Button->setChecked(true);
                } else if(type == "27C512") {
                    on_E27c512Button_clicked();
                    ui->E27c512Button->setChecked(true);
                } else {
                    log("Chip not selected. Doesn't match type", "red");
                    eraseSet = false;
                }
             }
            if(Detect == 3) {
                if(type == "27C010") {
                    on_E27c010Button_clicked();
                    ui->E27c010Button->setChecked(true);
                } else if(type == "27C020") {
                    on_E27c020Button_clicked();
                    ui->E27c020Button->setChecked(true);
                } else if(type == "27C040") {
                    on_E27c040Button_clicked();
                    ui->E27c040Button->setChecked(true);
                } else if(type == "27C080") {
                    on_E27c080Button_clicked();
                    ui->E27c080Button->setChecked(true);
                } else {
                    log("Chip not selected. Doesn't match type", "red");
                    eraseSet = false;
                }
            }
            if(Detect == 4) {
                if(type == "28F256") {
                    on_F28f256Button_clicked();
                    ui->F28f256Button->setChecked(true);
                } else if(type == "28F512") {
                    on_F28f512Button_clicked();
                    ui->F28f512Button->setChecked(true);
                } else if(type == "28F010") {
                    on_F28f010Button_clicked();
                    ui->F28f010Button->setChecked(true);
                } else if(type == "28F020") {
                    on_F28f020Button_clicked();
                    ui->F28f020Button->setChecked(true);
                } else {
                    log("Chip not selected. Doesn't match type", "red");
                    eraseSet = false;
                }
            }
            if(Detect == 5) {
                if(type == "29C256") {
                    on_F29c256Button_clicked();
                    ui->F29c256Button->setChecked(true);
                } else {
                   log("Chip not selected. Doesn't match type", "red");
                   eraseSet = false;
                }
            }
            if(Detect == 6) {
                if(type == "29C512") {
                    on_F29c512Button_clicked();
                    ui->F29c512Button->setChecked(true);
                } else if(type == "29C010") {
                    on_F29c010Button_clicked();
                    ui->F29c010Button->setChecked(true);
                } else if(type == "29C020") {
                    on_F29c020Button_clicked();
                    ui->F29c020Button->setChecked(true);
                } else if(type == "29C040") {
                    on_F29c040Button_clicked();
                    ui->F29c040Button->setChecked(true);
                } else {
                   log("Chip not selected. Doesn't match type.", "red");
                   eraseSet = false;
                }
            }
            if(Detect == 7) {
                if(type == "29F512") {
                    on_F29f512Button_clicked();
                    ui->F29f512Button->setChecked(true);
                } else if(type == "29F010") {
                    on_F29f010Button_clicked();
                    ui->F29f010Button->setChecked(true);
                } else if(type == "29F020") {
                    on_F29f020Button_clicked();
                    ui->F29f020Button->setChecked(true);
                } else if(type == "29F040") {
                    on_F29f040Button_clicked();
                    ui->F29f040Button->setChecked(true);
                } else {
                    log("Chip not selected. Doesn't match type", "red");
                    eraseSet = false;
                }
            }
            VPP = tempVPP;
            VPE = tempVPE;
        }
    } else {
        log("Read signature code error", "red");
    }
}

void MainWindow::on_IdentificationReadButton_clicked()
{
    bufferClear = true;
    ui->voltageChipButton->setChecked(false);
    ui->progressBar->setValue(0);
    updateButtons(false, false);
    if(Chip == "28C64") {
        emit sendDatabuf28ID(Chip, buf28ID);
        windowbuf28ID->move(pos().x()+110, pos().y()+190);
        windowbuf28ID->exec();
    }
    ui->progressBar->setMaximum(buf28ID);
    ui->progressBar->setValue(0);
    read28IDConnection = QObject::connect(this, SIGNAL(read28IDSignal()), this, SLOT(read28IDStart()));
    getVoltage();
    log(QString("Reading %1 bytes device identification code from chip...").arg(buf28ID));
}

void MainWindow::on_IdentificationReadButton2_clicked()
{
    on_IdentificationReadButton_clicked();
}

void MainWindow::on_IdentificationReadButton3_clicked()
{
    on_IdentificationReadButton_clicked();
}

void MainWindow::bufID28C(int value) {
    buf28ID = value;
    log(QString("%1 bytes bufferID selected").arg(buf28ID));
}

void MainWindow::read28IDStart()
{
    QObject::disconnect(read28IDConnection);
    if(voltage < 11.5 || voltage > 12.5) {
        log("Incorrect voltage on pin A9", "red");
        log("Set voltage to 11.5 - 12.5 V");
        if(!bufferID28) {ui->showButton->setChecked(false);}
        updateButtons(true, true);
        return;
    }
    serialDataConnection = QObject::connect(serialPort, SIGNAL(readyRead()), this, SLOT(read28ID()));
    if(Chip == "28C64") {
        if(buf28ID == 32){
           sendCommand("@o");
        }
        if(buf28ID == 64){
           sendCommand("%o");
        }
    } else {
        sendCommand("o");
    }
}

void MainWindow::read28ID()
{
    static uint16_t count = 0;
    const QByteArray data = serialPort->readAll();
    if(data.toHex().toUpper() == "45") {
        QObject::disconnect(serialDataConnection);
        bufferID28 = false;
        ui->showButton->setChecked(false);
        updateButtons(true, false);
        log("Connection error!", "red");
        count = 0;
        return;
    }
    if(data.toHex().toUpper() == "46") {
        QObject::disconnect(serialDataConnection);
        updateButtons(true, true);
        log("Read ID error. Incorrect voltage pin A9", "red");
        count = 0;
        return;
    }
    if(data.count()) {
        memcpy(&(bufWork.data())[count], data.data(), data.count());
        count += data.count();
    }
    ui->progressBar->setValue(count);
    if(count >= buf28ID) {
        QObject::disconnect(serialDataConnection);
        bufferID28 = true;
        ui->progressBar->setValue(buf28ID);
        updateButtons(true, true);
        log("Reading ID completed", "green");
        log("Open buffer to view device identification code");
        count = 0;
        emit bufferUpdated();
    }
}

void MainWindow::on_saveID28Button_clicked()
{
    ui->progressBar->setValue(0);
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save buffer"), "",
                                                    tr("Binary (*.bin);;All Files (*)"));
    if(fileName.isEmpty())
        return;
    else {
        if(fileName.rightRef(4).indexOf(".bin", 0) == -1) {
            fileName.append(QString(".bin"));
        }
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        file.write(bufWork, buf28ID);
        file.close();
        log(QString("Buffer saved to %1 file").arg(fileName));
    }
}

void MainWindow::on_saveID28Button2_clicked()
{
    on_saveID28Button_clicked();
}

void MainWindow::on_saveID28Button3_clicked()
{
    on_saveID28Button_clicked();
}

void MainWindow::on_openID28Button_clicked()
{
    bufferClear = true;
    ui->progressBar->setValue(0);
    if(Chip == "28C64") {
        emit sendDatabuf28ID(Chip, buf28ID);
        windowbuf28ID->move(pos().x()+110, pos().y()+190);
        windowbuf28ID->exec();
    }
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open binary to buffer"), "",
                                                    tr("Binary (*.bin);;All Files (*)"));
    if(fileName.isEmpty())
        return;
    else {
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        bufWork.clear();
        bufCheck.fill(0);
        bufWork.append(file.readAll());
        log(QString("Load from %1 file").arg(fileName));
        log(QString("Readed %1 bytes").arg(bufWork.count()));
        if((uint32_t)bufWork.count() < buf28ID) {
            bufWork.append((buf28ID - bufWork.count()), 0xff);
        }
        if((uint32_t)bufWork.count() > buf28ID) {
            log(QString("Deleated %1 bytes").arg(bufWork.count() - buf28ID));
            bufWork.resize(buf28ID);
        }
        bufferID28 = true;
        updateButtons(true, true);
        emit bufferUpdated();
    }
}

void MainWindow::on_openID28Button2_clicked()
{
    on_openID28Button_clicked();
}

void MainWindow::on_openID28Button3_clicked()
{
    on_openID28Button_clicked();
}

void MainWindow::on_IdentificationWriteButton_clicked()
{    
    ui->voltageChipButton->setChecked(false);
    ui->progressBar->setMaximum(buf28ID);
    ui->progressBar->setValue(0);
    updateButtons(false, false);
    write28IDConnection = QObject::connect(this, SIGNAL(write28IDSignal()), this, SLOT(write28IDStart()));
    getVoltage();
    log(QString("Writing %1 bytes device identification code to chip...").arg(buf28ID));
}

void MainWindow::on_IdentificationWriteButton2_clicked()
{
    on_IdentificationWriteButton_clicked();
}

void MainWindow::on_IdentificationWriteButton3_clicked()
{
    on_IdentificationWriteButton_clicked();
}

void MainWindow::write28IDStart()
{
    QObject::disconnect(write28IDConnection);
    if(voltage < 11.5 || voltage > 12.5) {
        log("Incorrect voltage on pin A9", "red");
        log("Set voltage to 11.5 - 12.5 V");
        updateButtons(true, true);
        return;
    }
    serialDataConnection = QObject::connect(serialPort, SIGNAL(readyRead()), this, SLOT(write28ID()));
    if(Chip == "28C64") {
        if(buf28ID == 32){
           sendCommand("@p");
        }
        if(buf28ID == 64){
           sendCommand("%p");
        }
    } else {
        sendCommand("p");
    }
}

void MainWindow::write28ID()
{
    QByteArray data;
    const QByteArray dataWrite = serialPort->readAll();
    if(dataWrite.toHex().toUpper()== "45") {
        QObject::disconnect(serialDataConnection);
        bufferID28 = false;
        ui->showButton->setChecked(false);
        updateButtons(true, false);
        log("Connection error!", "red");
    } else
    if(dataWrite.toHex().toUpper() == "46") {
        QObject::disconnect(serialDataConnection);
        updateButtons(true, true);
        log("Write ID error. Incorrect voltage pin A9", "red");
    } else
    if(dataWrite.toHex().toUpper() == "4F") {
        QObject::disconnect(serialDataConnection);
        updateButtons(true, true);
        ui->progressBar->setValue(buf28ID);
        log("Writing ID completed", "green");
    } else
    if(dataWrite.toHex().toUpper() == "53") {
        data.clear();
        if(buf28ID > 64) {
            for(uint8_t i = 0; i < 64; i++) {
                data.append(bufWork[i]);
            }
        } else {
            for(uint8_t i = 0; i < buf28ID; i++) {
                data.append(bufWork[i]);
            }
        }
        writeData(data);
        data.clear();
    } else
    if(dataWrite.toHex().toUpper() == "54") {
        data.clear();
        for(uint8_t i = 64; i < 128; i++) {
            data.append(bufWork[i]);
        }
        writeData(data);
        data.clear();
    } else {
        QObject::disconnect(serialDataConnection);
        updateButtons(true, true);
        log("Data transmission error!", "red");
    }
}

void MainWindow::on_testSRAMButton_clicked()
{
    log(QString("Testing Static RAM"));
    bufWork = bufTest;
    bufWork.resize(bufSize);
    bufCheck.fill(0);
    bufferClear = false;
    emit bufferUpdated();
    TestSRAM = true;
    WandVSet = true;
    on_writeChipButton_clicked();
}

void MainWindow::on_testSRAMButton2_clicked()
{
    on_testSRAMButton_clicked();
}

void MainWindow::on_testSRAMButton3_clicked()
{
    on_testSRAMButton_clicked();
}

void MainWindow::getVoltage()
{
    serialDataConnection = QObject::connect(serialPort, SIGNAL(readyRead()), this, SLOT(Voltage()));
    sendCommand("v");
}

void MainWindow::Voltage()
{
    QObject::disconnect(serialDataConnection);
    const QByteArray dataVoltage = serialPort->readAll();
    voltage = QString(dataVoltage).toFloat();
    if(voltage < 4 || voltage > 28) {voltage = 0.0;}
    emit signatureCodeSignal();
    emit read28IDSignal();
    emit write28IDSignal();
    emit chipErase();
    emit chipWrite();
}

void MainWindow::showVoltage()
{
    getVoltage();
    if(updateVoltageConnection) {
        ui->progressBar->setValue(voltage);
        ui->progressBar->setFormat(QString("%1 V").arg(voltage, 0, 'f', 1));
    }
}

void MainWindow::on_voltageChipButton_toggled(bool checked)
{
    if(checked) {
        updateVoltageConnection = QObject::connect(&updateVoltageTimer, SIGNAL(timeout()), this, SLOT(showVoltage()));
        updateVoltageTimer.setInterval(300);
        updateVoltageTimer.start();
        // Change progressBar params
        ui->progressBar->setMinimum(4);
        ui->progressBar->setMaximum(28);
        ui->progressBar->setFormat("%v");
        ui->progressBar->setTextVisible(true);
        ui->progressBar->setValue(voltage);
        ui->progressBar->setFormat(QString("%1 V").arg(voltage, 0, 'f', 1));
    } else {
        QObject::disconnect(updateVoltageConnection);
        updateVoltageTimer.stop();
        ui->progressBar->setMinimum(0);
        ui->progressBar->setMaximum(100);
        ui->progressBar->setFormat("%p%");
        ui->progressBar->setValue(0);
    }
}

void MainWindow::on_openFileButton_clicked()
{
    bufferID28 = false;
    ui->progressBar->setValue(0);
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open binary to buffer"), "",
                                                    tr("Binary (*.bin);;All Files (*)"));
    if(fileName.isEmpty())
        return;
    else {
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        bufWork.clear();
        bufCheck.fill(0);
        bufWork.append(file.readAll());
        log(QString("Load from %1 file").arg(fileName));
        log(QString("Readed %1 bytes").arg(bufWork.count()));
        if((uint32_t)bufWork.count() < bufSize) {
            bufWork.append((bufSize - bufWork.count()), 0xff);
        }
        if((uint32_t)bufWork.count() > bufSize) {
            log(QString("Deleated %1 bytes").arg(bufWork.count() - bufSize));
            bufWork.resize(bufSize);
        }
        bufferClear = false;
        updateButtons(true, true);
        emit bufferUpdated();
    }
}

void MainWindow::on_saveFileButton_clicked()
{
    ui->progressBar->setValue(0);
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save buffer"), "",
                                                    tr("Binary (*.bin);;All Files (*)"));
    if(fileName.isEmpty())
        return;
    else {
        if(fileName.rightRef(4).indexOf(".bin", 0) == -1) {
            fileName.append(QString(".bin"));
        }
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        file.write(bufWork);
        file.close();
        log(QString("Buffer saved to %1 file").arg(fileName));
    }
}

void MainWindow::on_showButton_toggled(bool checked)
{
    if(checked) {
//        this->setFixedSize(QSize(795, 550));
        updateBufConnection = QObject::connect(this, SIGNAL(bufferUpdated()), this, SLOT(showBuf()));
        emit bufferUpdated();
    } else {
//        this->setFixedSize(QSize(390, 550));
        QObject::disconnect(updateBufConnection);
    }
}

void MainWindow::showBuf()
{
    QTableWidget *tableWidget = ui->tableWidget;
    QTableWidgetItem *newItem;
    // clear table
    tableWidget->setRowCount(0);
    if(bufferID28) {
        tableWidget->setRowCount((buf28ID + 1) / 8);
    } else {
        tableWidget->setRowCount((bufSize + 1) / 8);
    }
    for(int row=0; row!=tableWidget->rowCount(); ++row) {
        for(int column=0; column!=tableWidget->columnCount(); ++column) {
            newItem = new QTableWidgetItem(QString::asprintf("%02X", (uint8_t)bufWork.data()[row * 8 + column]));
            if(bufCheck.data()[row * 8 + column] == NOT_WRITABLE) newItem->setForeground(QColor::fromRgb(255,0,0));
            if(bufCheck.data()[row * 8 + column] == WRITABLE) newItem->setForeground(QColor::fromRgb(255,127,0));
            tableWidget->setItem(row, column, newItem);
        }
    }
}

void MainWindow::eraseToRead(int value) {
    EandRSet = value;
}

void MainWindow::ToSoftErase(int value) {
    SoftEraseEnable = value;
}

void MainWindow::on_eraseChipButton_clicked()
{
    ui->voltageChipButton->setChecked(false);
    updateButtons(false, false);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);
    if(chipGroup == 4 || chipGroup == 5) {
        log(QString("Chip erasing..."));
        serialDataConnection = QObject::connect(serialPort, SIGNAL(readyRead()), this, SLOT(eraseData()));
        sendCommand("s");
    } else {
        checkEraseConnection = QObject::connect(this, SIGNAL(chipErase()), this, SLOT(eraseChipWindow()));
        getVoltage();
    }
}

void MainWindow::eraseChipWindow()
{
    QObject::disconnect(checkEraseConnection);
    if(chipGroup == 2 || chipGroup == 3) {VPE = "12.0";}
    QString derv = "Unknown";
    if(VPE != "Unknown") {derv = QString("%1 V").arg(VPE);}
    QString serv = "Unknown";
    if(voltage != 0) {serv = QString("%1 V").arg(voltage, 0, 'f', 1);}
    emit sendDataErase(Chip, derv, serv, chipGroup, SoftEraseEnable);
    windowErase->move(pos().x()+84, pos().y()+180);
    int dialogCode = windowErase->exec();
    if(dialogCode == QDialog::Accepted) {
        if(SoftEraseEnable) {
            log(QString("SoftErase enabled"));
            log(QString("Chip erasing..."));
            serialDataConnection = QObject::connect(serialPort, SIGNAL(readyRead()), this, SLOT(eraseData()));
            sendCommand("s");
        } else {
            emit sendDataSelVpp(Chip, chipGroup, 2);
            windowSelVpp->move(pos().x()+66, pos().y()+160);
            int dialogCode2 = windowSelVpp->exec();
            if(dialogCode2 == QDialog::Accepted) {
                log(QString("Chip erasing..."));
                if(chipGroup == 3) {
                    log(QString("Writing 0x00 to chip cells before erasing"));
                    serialDataConnection = QObject::connect(serialPort, SIGNAL(readyRead()), this, SLOT(eraseData28F()));
                } else {
                    serialDataConnection = QObject::connect(serialPort, SIGNAL(readyRead()), this, SLOT(eraseData()));
                }
                sendCommand("e");
            } else {
                log("You have not confirmed the action");
                updateButtons(true, true);
            }
        }
    } else {updateButtons(true, true);}
}

void MainWindow::eraseData()
{
    QObject::disconnect(serialDataConnection);
    const QByteArray dataErase = serialPort->readAll();
    updateButtons(true, true);
    if(dataErase.toHex().toUpper() == "45") {
       log("Connection error!", "red");
    } else
    if(dataErase.toHex().toUpper() == "4F") {
        ui->progressBar->setValue(100);
        log("Erase completed", "green");
        if(EandRSet) {
            log("Starting checking for emptiness");
            on_readChipButton_clicked();
        } else {
            log("Run read to check for emptiness");
        }
    } else if(dataErase.toHex().toUpper() == "46") {
        if(chipGroup == 1) {log("Erase error. Incorrect voltage pin A9", "red");} else {log("Erase error. Incorrect voltage pin OE", "red");}
    } else {
        log("Erase error", "red");
    }
}

void MainWindow::eraseData28F()
{
    const QByteArray dataErase = serialPort->readAll();
    if(dataErase.toHex().toUpper() == "45") {
        QObject::disconnect(serialDataConnection);
        updateButtons(true, true);
        log("Connection error!", "red");
    } else
    if(dataErase.toHex().toUpper() == "46") {
        QObject::disconnect(serialDataConnection);
        updateButtons(true, true);
        log("Erase error. Incorrect voltage pin VPP", "red");
    } else
    if(dataErase.toHex().toUpper() == "44") {
        QObject::disconnect(serialDataConnection);
        ui->progressBar->setValue(0);
        updateButtons(true, true);
        log("Erase error. Cannot write 0x00 to chip cells", "red");
    } else
    if(dataErase.toHex().toUpper() == "50") {
        ui->progressBar->setValue(5);
    } else
    if(dataErase.toHex().toUpper() == "51") {
        ui->progressBar->setValue(20);
    } else
    if(dataErase.toHex().toUpper() == "52") {
        ui->progressBar->setValue(35);
    } else
    if(dataErase.toHex().toUpper() == "53") {
        ui->progressBar->setValue(50);
    } else
    if(dataErase.toHex().toUpper() == "54") {
        ui->progressBar->setValue(65);
        log("Write 0x00 to chip cells successfully completed");
        log("Starting erase");
    } else
    if(dataErase.toHex().toUpper() == "55") {
        ui->progressBar->setValue(80);
    } else
    if(dataErase.toHex().toUpper() == "56") {
        ui->progressBar->setValue(90);
    } else
    if(dataErase.toHex().toUpper() == "4F") {
        QObject::disconnect(serialDataConnection);
        ui->progressBar->setValue(100);
        updateButtons(true, true);
        log("Erase completed", "green");
        if(EandRSet) {
            log("Starting checking for emptiness");
            on_readChipButton_clicked();
        }
    } else {
        QObject::disconnect(serialDataConnection);
        ui->progressBar->setValue(0);
        updateButtons(true, true);
        log("Erase error", "red");
    }
}

void MainWindow::on_readChipButton_clicked()
{
    ui->voltageChipButton->setChecked(false);
    bufferID28 = false;
    bufWork.resize(bufSize);
    bufCheck.fill(0);
    readChip();
    checkClearConnection = QObject::connect(this, SIGNAL(chipReaded()), this, SLOT(checkClear()));
}

void MainWindow::readChip()
{
    updateButtons(false, false);
    log(QString("Reading %1 bytes from chip...").arg(bufSize));
    ui->progressBar->setMaximum(bufSize);
    ui->progressBar->setValue(0);
    serialDataConnection = QObject::connect(serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
    sendCommand("r");
}

void MainWindow::readData()
{
    static uint32_t count = 0;
    const QByteArray data = serialPort->readAll();
    if(data.toHex().toUpper() == "45") {
        QObject::disconnect(serialDataConnection);
        bufferClear = true;
        ui->showButton->setChecked(false);
        updateButtons(true, false);
        log("Connection error!", "red");
        count = 0;
        return;
    }
    if(data.count()) {
        memcpy(&(bufWork.data())[count], data.data(), data.count());
        count += data.count();
    }
    ui->progressBar->setValue(count);
    if(count >= bufSize) {
        QObject::disconnect(serialDataConnection);
        ui->progressBar->setValue(bufSize);
        bufferClear = false;
        updateButtons(true, true);
        log(QString("Readed %1 bytes").arg(count));
        count = 0;
        emit chipReaded();
        emit bufferUpdated();
    }
}

void MainWindow::checkClear()
{
    uint32_t count;
    QObject::disconnect(checkClearConnection);
    log("Reading completed", "green");
    for(count = 0; count < bufSize; count++) {
        if(bufWork[count] != (char)0xff) {
            log(QString("Chip is not empty"));
            break;
        }
    }
    if(count >= bufSize) {
        log(QString("Chip is empty"));
    }
}

void MainWindow::writeToCEPulse(int value) {
    CEPulse = value;
}

void MainWindow::writeToVerification(int value) {
    WandVSet = value;
}

void MainWindow::writeToSDP_Disabled(int value) {
    SDP_Disabled = value;
}

void MainWindow::writeToSDP_Enabled(int value) {
    SDP_Enabled = value;
}

void MainWindow::module_28C(int value) {
    Module = value;
}

void MainWindow::PageWrite_28C(int value) {
    PageWrite = value;
}

void MainWindow::on_writeChipButton_clicked()
{
    ui->voltageChipButton->setChecked(false);
    ui->progressBar->setMaximum(bufSize);
    ui->progressBar->setValue(0);
    updateButtons(false, false);
    if(chipGroup == 1) {
        checkWriteConnection = QObject::connect(this, SIGNAL(chipWrite()), this, SLOT(writeChipWindow()));
        getVoltage();
    }
    else if(chipGroup == 3) {
        checkWriteConnection = QObject::connect(this, SIGNAL(chipWrite()), this, SLOT(writeChip28F()));
        getVoltage();
    }
    else if(chipGroup == 2 || chipGroup == 4) {
        QByteArray command;
        emit sendDataWrite28(Chip, Module, PageWrite);
        windowWrite28->move(pos().x()+92, pos().y()+160);
        int dialogCode = windowWrite28->exec();
        if(dialogCode == QDialog::Accepted) {
            log(QString("Writing %1 bytes to chip...").arg(bufSize));
            serialDataConnection = QObject::connect(serialPort, SIGNAL(readyRead()), this, SLOT(writeChip()));
            if(SDP_Disabled) {
                command += "q"; log(QString("SDP Disable"));
            } else if(SDP_Enabled) {
                command += "t"; log(QString("SDP Enable"));
            }
            if(Module) {
                command += "k"; log(QString("Chip Module Enable"));
            }
            if(PageWrite == 1) {command += ":";}
            else if(PageWrite == 16) {command += ";";}
            else if(PageWrite == 32) {command += ".";}
            else if(PageWrite == 64) {command += ",";}
            else if(PageWrite == 128) {command += "!";}
            else if(PageWrite == 256) {command += "?";}
            command += "w";
            sendCommand(command);
        } else {updateButtons(true, true);}
    }
    else if(chipGroup == 5 || chipGroup == 6) {
        log(QString("Writing %1 bytes to chip...").arg(bufSize));
        serialDataConnection = QObject::connect(serialPort, SIGNAL(readyRead()), this, SLOT(writeChip()));
        sendCommand("w");
    }
}

void MainWindow::writeChipWindow()
{
    QObject::disconnect(checkWriteConnection);
    QString dvpp = "Unknown";
    if(VPP != "Unknown") {dvpp = QString("%1 V").arg(VPP);}
    QString svpp = "Unknown";
    if(voltage != 0) {svpp = QString("%1 V").arg(voltage, 0, 'f', 1);}
    emit sendDataWrite(Chip, dvpp, svpp, CEPulse);
    windowWrite->move(pos().x()+92, pos().y()+160);
    int dialogCode = windowWrite->exec();
    if(dialogCode == QDialog::Accepted) {
        float VPPmax = 0.0;
        if(Chip == "27C16" || Chip == "27C32") {VPPmax = 26.0;} else
        if(Chip == "27C64" || Chip == "27C128" || Chip == "27C256") {VPPmax = 22.0;}
        else {VPPmax = 14.0;}
        float VPP2 = VPP.toFloat() + 0.5;
        if(VPP2 > 12.0 && VPP2 < VPPmax) {VPPmax = VPP2;}
        if(voltage < 11.5 || voltage > VPPmax) {
            log("Incorrect voltage on pin VPP", "red");
            log("Set the correct voltage");
            updateButtons(true, true);
            return;
        }
        windowSelVpp->move(pos().x()+66, pos().y()+160);
        emit sendDataSelVpp(Chip, chipGroup, 1);
        int dialogCode2 = windowSelVpp->exec();
        if(dialogCode2 == QDialog::Accepted) {
            log(QString("Writing %1 bytes to chip...").arg(bufSize));
            serialDataConnection = QObject::connect(serialPort, SIGNAL(readyRead()), this, SLOT(writeChip()));
            if(CEPulse == 25) {sendCommand("(w");}
            else if(CEPulse == 50) {sendCommand(")w");}
            else if(CEPulse == 100) {sendCommand("{w");}
            else if(CEPulse == 200) {sendCommand("}w");}
            else if(CEPulse == 500) {sendCommand("[w");}
            else if(CEPulse == 1000) {sendCommand("]w");}
            else if(CEPulse == 5000) {sendCommand("<w");}
            else if(CEPulse == 10000) {sendCommand(">w");}
            else if(CEPulse == 50000) {sendCommand("|w");}
        } else {
            log("You have not confirmed the action");
            updateButtons(true, true);
        }
   } else {updateButtons(true, true);}
}

void MainWindow::writeChip28F()
{
    QObject::disconnect(checkWriteConnection);
    QString dvpp = "Unknown";
    VPP = "12.0";
    if(VPP != "Unknown") {dvpp = QString("%1 V").arg(VPP);}
    QString svpp = "Unknown";
    if(voltage != 0) {svpp = QString("%1 V").arg(voltage, 0, 'f', 1);}
    emit sendDataWrite28F(Chip, dvpp, svpp);
    windowWrite28F->move(pos().x()+92, pos().y()+160);
    int dialogCode = windowWrite28F->exec();
    if(dialogCode == QDialog::Accepted) {
        if(voltage < 11.5 || voltage > 12.5) {
            log("Incorrect voltage on pin VPP", "red");
            log("Set voltage to 11.5 - 12.5 V");
            updateButtons(true, true);
            return;
        }
        windowSelVpp->move(pos().x()+66, pos().y()+160);
        emit sendDataSelVpp(Chip, chipGroup, 1);
        int dialogCode2 = windowSelVpp->exec();
        if(dialogCode2 == QDialog::Accepted) {
            log(QString("Writing %1 bytes to chip...").arg(bufSize));
            serialDataConnection = QObject::connect(serialPort, SIGNAL(readyRead()), this, SLOT(writeChip()));
            sendCommand("w");
        } else {
            log("You have not confirmed the action");
            updateButtons(true, true);
        }
   } else {updateButtons(true, true);}
}

void MainWindow::writeChip()
{
    static uint32_t count = 0;
    QByteArray data;
    const QByteArray dataWrite = serialPort->readAll();
    if(dataWrite.toHex().toUpper() == "45") {
        QObject::disconnect(serialDataConnection);
        bufferClear = true;
        ui->showButton->setChecked(false);
        updateButtons(true, false);
        log("Connection error!", "red");
        count = 0;
    } else
    if(dataWrite.toHex().toUpper() == "46") {
        QObject::disconnect(serialDataConnection);
        updateButtons(true, true);
        log("Write error. Incorrect voltage pin VPP", "red");
        count = 0;
    } else
    if(dataWrite.toHex().toUpper() == "44") {
        QObject::disconnect(serialDataConnection);
        ui->progressBar->setValue(0);
        updateButtons(true, true);
        log("Error write block", "red");
        count = 0;
    } else
    if(dataWrite.toHex().toUpper() == "4F" || count >=  bufSize) {
        QObject::disconnect(serialDataConnection);
        ui->progressBar->setValue(bufSize);
        updateButtons(true, true);
        log("Writing completed", "green");
        count = 0;
        if(WandVSet) {
            on_verifyChipButton_clicked();
            WandVSet = false;
        }
    } else
    if(dataWrite.toHex().toUpper() == "53") {
        data.clear();
        for(uint32_t i = count; i < count + 64; i++) {
            ui->progressBar->setValue(i);
            data.append(bufWork[i]);
        }
        writeData(data);
        data.clear();
        count += 64;
    } else {
        QObject::disconnect(serialDataConnection);
        ui->progressBar->setValue(0);
        updateButtons(true, true);
        log("Data transmission error!", "red");
        count = 0;
    }
}

void MainWindow::on_verifyChipButton_clicked()
{
    ui->voltageChipButton->setChecked(false);
    // Backup work buffer
    bufCheck = bufWork;
    readChip();
    verifyDataConnection = QObject::connect(this, SIGNAL(chipReaded()), this, SLOT(verifyData()));
}

void MainWindow::verifyData()
{
    QObject::disconnect(verifyDataConnection);
    uint32_t errors_count = 0;
    uint32_t warnings_count = 0;
    for(uint32_t i = 0; i < bufSize; i++) {
        if((bufWork[i] ^ bufCheck[i]) & bufCheck[i]) {
            errors_count++;
            bufWork[i] = bufCheck[i];
            // mark error byte
            bufCheck[i] = NOT_WRITABLE;
        } else if(bufWork[i] != bufCheck[i]) {
            warnings_count++;
            bufWork[i] = bufCheck[i];
            bufCheck[i] = WRITABLE;
        } else {
            bufCheck[i] = NO_ERR;
        }
    }
    if(errors_count || warnings_count) {
        log("Verification failed", "red");
        log(QString("Errors: %1").arg(errors_count));
        log(QString("Warnings: %1").arg(warnings_count));
        if(TestSRAM) {
            TestSRAM = 0;
            log("Test failed", "red");
        }
    } else {
        log("Verification successful", "green");
        if(TestSRAM) {
            TestSRAM = 0;
            log("Test completed successfully", "green");
        }
    }
    emit bufferUpdated();
}

void MainWindow::on_testDRAMButton_clicked()
{
    updateButtons(false, false);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);
    log(QString("Testing %1 chip...").arg(Chip));
    log("Wait...");
    serialDataConnection = QObject::connect(serialPort, SIGNAL(readyRead()), this, SLOT(testDRAM()));
    sendCommand("~");
}

void MainWindow::testDRAM()
{
    const QByteArray dataDRAM = serialPort->readAll();
    if(dataDRAM.toHex().toUpper() == "45") {
       QObject::disconnect(serialDataConnection);
       updateButtons(true, false);
       log("Connection error!", "red");
    } else
    if(dataDRAM.toHex().toUpper() == "50") {
        ui->progressBar->setValue(20);
        log("Test 1 passed");
    } else
    if(dataDRAM.toHex().toUpper() == "51") {
        ui->progressBar->setValue(40);
        log("Test 2 passed");
    } else
    if(dataDRAM.toHex().toUpper() == "52") {
        ui->progressBar->setValue(60);
        log("Test 3 passed");
    } else
    if(dataDRAM.toHex().toUpper() == "53") {
        ui->progressBar->setValue(80);
        log("Test 4 passed");
    } else
    if(dataDRAM.toHex().toUpper() == "4F") {
        QObject::disconnect(serialDataConnection);
        updateButtons(true, false);
        ui->progressBar->setValue(100);
        log("Test 5 passed");
        log("All tests completed successfully", "green");
    } else
    if(dataDRAM.toHex().toUpper().mid(0,2) == "46") {
        QObject::disconnect(serialDataConnection);
        ui->progressBar->setValue(0);
        updateButtons(true, false);
        log("Test failed", "red");
        log(QString("Error in cell %1").arg(QString(dataDRAM.mid(1))));
    } else {
        QObject::disconnect(serialDataConnection);
        ui->progressBar->setValue(0);
        updateButtons(true, false);
        log("Test error", "red");
    }
}

void MainWindow::on_D4164Button_clicked()
{
    Chip = "4164";
    chipGroup = 7;
    dipGroup = 4;
    bufSize = 0xffff + 1;
    dramSelected = true;
    ui->progressBar->setValue(0);
    updateButtons(true, false);
    if(Chip != "Unknown") {log(QString("Selected chip %1").arg(Chip));}
    emit sendChipInfo(Chip, bufSize, dipGroup, chipGroup);
    sendCommand("-");
}

void MainWindow::on_D41256Button_clicked()
{
    Chip = "41256";
    chipGroup = 7;
    dipGroup = 4;
    bufSize = 0x3ffff + 1;
    dramSelected = true;
    ui->progressBar->setValue(0);
    updateButtons(true, false);
    if(Chip != "Unknown") {log(QString("Selected chip %1").arg(Chip));}
    emit sendChipInfo(Chip, bufSize, dipGroup, chipGroup);
    sendCommand("+");
}

void MainWindow::selectLogicChip(QString value)
{
    logicSelected = false;
    bool ErrorTruth = false;
    countTrLogic = 0;
    NameLogic = "Unknown";
    DescLogic = "Unknown";
    PinLogic = "Unknown";
    TruthLogic.clear();
    QFile fileCode("database_logic.txt");
    if(!fileCode.open(QIODevice::ReadOnly | QIODevice::Text)) {
        log("File database_logic.txt not found", "red");
        ui->listLogic->clear();
        listLogic.clear();
        listLogicOk = false;
        updateButtons(true, false);
    } else {
        QTextStream in(&fileCode);
        while(!in.atEnd()) {
            QString line = in.readLine().trimmed();
            QRegExp rx(QString("^\\$(%1)$").arg(value));
            if(rx.indexIn(line) != -1) {
                NameLogic = value;
                line = in.readLine().trimmed();
                QRegExp rx1(QString("^\\$(4|7)[0-9]{3,4}$"));
                if(rx1.indexIn(line) != -1) {
                    break;
                } else {
                    DescLogic = line;
                }
                line = in.readLine().trimmed();
                QRegExp rx2(QString("^(14|16)$"));
                if(rx2.indexIn(line) != -1) {
                    PinLogic = line;
                } else {
                    break;
                }
                while(!in.atEnd()) {
                    line = in.readLine().trimmed();
                    QRegExp rx3(QString("^(\\$(4|7)[0-9]{3,4}|\\$)$"));
                    if(rx3.indexIn(line) != -1) {
                        break;
                    } else {
                        QRegExp rx4(QString("^[VG01LHXC]{"+PinLogic+"}$"));
                        if(rx4.indexIn(line) != -1) {
                            TruthLogic.append(line);
                        } else {
                            ErrorTruth = true;
                            TruthLogic.clear();
                            break;
                        }
                    }
                }
                countTrLogic = TruthLogic.count();
                break;
            }
        }
        fileCode.close();
        if(autotesting) {
            if(countTrLogic < 2) {
                log(QString("Error! Testing IC <a href=\"%1\">%1</a> missed").arg(value), "red");
                nextListLogic++;
                if(nextListLogic < countListLogic) {
                    selectLogicChip(listLogic[nextListLogic]);
                }
            }
        } else {
            if(NameLogic == "Unknown") {
                log("Error! Сhip not found in database", "red");
            } else
            if(DescLogic == "Unknown") {
                log("Error! No chip information in database", "red");
            } else
            if(PinLogic == "Unknown") {
                log("Error! Incorrect number of chip pins", "red");
            } else
            if(ErrorTruth) {
                log("Error! Invalid truth table", "red");
            } else
            if(!countTrLogic) {
                log("Error! No truth table", "red");
            } else
            if(countTrLogic < 2) {
                log("Error! Truth table incomplete", "red");
            } else {
                logicSelected = true;
            }
            if(logicSelected == false) {windowLogicInfo->close();}
            updateButtons(true, false);
        }
    }
}

void MainWindow::on_listLogic_itemClicked(QListWidgetItem *item)
{  
    if(item) {
        autotesting = false;
        updateButtons(false, false);
        ui->progressBar->setValue(0);
        log(QString("Selected chip %1").arg(ui->listLogic->currentItem()->text()));
        selectLogicChip(ui->listLogic->currentItem()->text());
        emit sendLogicInfo(NameLogic, DescLogic, PinLogic.toInt(), TruthLogic);
    }
}

void MainWindow::on_testLogicButton_clicked()
{
    updateButtons(false, false);
    log(QString("Testing %1 chip...").arg(NameLogic));
    serialDataConnection = QObject::connect(serialPort, SIGNAL(readyRead()), this, SLOT(testLogic()));
    sendCommand("u");
}

void MainWindow::testLogic()
{
    uint8_t error = 0;
    static uint8_t iTr;
    static uint8_t rTest;
    const QByteArray dataTest = serialPort->readAll();
    QByteArray data;
    if(dataTest.toHex().toUpper() == "45") {
       QObject::disconnect(serialDataConnection);
       updateButtons(true, false);
       log("Connection error!", "red");
    } else
    if(dataTest.toHex().toUpper() == "42") {
        data.clear();
        data.append(PinLogic.toInt());
        data.append(countTrLogic);
        writeData(data);
        data.clear();
        iTr = 0;
        rTest = 0;
    } else
    if(dataTest.toHex().toUpper() == "44") {
        QObject::disconnect(serialDataConnection);
        updateButtons(true, false);
        log("Data transmission error!", "red");
        log("Test stopped");
    } else
    if(dataTest.toHex().toUpper() == "43") {
         data.clear();
         data.append(TruthLogic[0]);
         writeData(data);
         data.clear();
     } else
     if(dataTest.toHex().toUpper() == "4F") {
         log(QString("<big><b><pre>%1</pre></b></big>").arg(TruthLogic[iTr]), "green");
         iTr++;
         if(iTr < countTrLogic) {
             data.clear();
             data.append(TruthLogic[iTr]);
             writeData(data);
             data.clear();
         } else {
             QObject::disconnect(serialDataConnection);
             updateButtons(true, false);
             log("Test finish");
             if(rTest == 1) {
                log("Test results: error!", "red");
             } else {
                log("Test results: successful!", "green");
             }
         }
    } else
    if(dataTest.size() == PinLogic.toInt()) {
         QString str = "";
         for(uint8_t i = 0; i < PinLogic.toInt(); i++) {
             if(QString(dataTest[i]) == "H") {str.append("<font color=\"red\">H</font>");}
             else if(QString(dataTest[i]) == "L") {str.append("<font color=\"red\">L</font>");}
             else if(QString(dataTest[i]) == QString(TruthLogic[iTr][i])) {str.append(QString(dataTest[i]));}
             else {str.append("<font color=\"red\">" + QString(dataTest[i]) + "</font>"); error = 1;}
         }
         log(QString("<big><b><pre>%1</pre></b></big>").arg(str));
         if(error == 1) {
             QObject::disconnect(serialDataConnection);
             updateButtons(true, false);
             log("Data transmission error!", "red");
             log("Test stopped");
             return;
         }
         rTest = 1;
         iTr++;
         if(iTr < countTrLogic) {
             data.clear();
             data.append(TruthLogic[iTr]);
             writeData(data);
             data.clear();
         } else {
             QObject::disconnect(serialDataConnection);
             updateButtons(true, false);
             log("Test finish");
             if(rTest == 1) {
                log("Test results: error!", "red");
             } else {
                log("Test results: successful!", "green");
             }
         }
    } else {
        QObject::disconnect(serialDataConnection);
        updateButtons(true, false);
        log("Data transmission error!", "red");
        log("Test stopped");
    }
}

void MainWindow::on_autoTestingButton_clicked()
{
    logicSelected = false;
    ui->listLogic->clearSelection();
    autotesting = true;
    windowLogicInfo->close();
    ResultAutoLogic.clear();
    updateButtons(false, false);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);
    log("Start autodetection");
    log("Find...");
    serialDataConnection = QObject::connect(serialPort, SIGNAL(readyRead()), this, SLOT(autoTestLogic()));
    sendCommand("n");
}

void MainWindow::autoTestLogic()
{
    static uint8_t iTr;
    uint8_t startTest = 0;
    const QByteArray dataTest = serialPort->readAll();
    QByteArray data;
    if(dataTest.toHex().toUpper() == "45") {
       QObject::disconnect(serialDataConnection);
       updateButtons(true, false);
       log("Connection error!", "red");
       return;
    } else
    if(dataTest.toHex().toUpper() == "42") {
        nextListLogic = 0;
        startTest = 1;
    } else
    if(dataTest.toHex().toUpper() == "43") {
         data.clear();
         data.append(TruthLogic[0]);
         writeData(data);
         data.clear();
         return;
    } else
    if(dataTest.toHex().toUpper() == "44") {
        log("Data transmission error!", "red");
        log(QString("Testing %1 missed").arg(NameLogic));
    } else
    if(dataTest.toHex().toUpper() == "41") {
        //чип не совпадает
    } else
    if(dataTest.toHex().toUpper() == "4F") {
        iTr++;
        if(iTr < countTrLogic) {
             data.clear();
             data.append(TruthLogic[iTr]);
             writeData(data);
             data.clear();
             return;
         } else {
             //добавить в массив успешный чип
             ResultAutoLogic.append(NameLogic);
         }
    } else {
        QObject::disconnect(serialDataConnection);
        data.clear();
        data.append(1);
        data.append(1);
        writeData(data);
        data.clear();
        ui->progressBar->setValue(0);
        updateButtons(true, false);
        log("Data transmission error!", "red");
        log("Search stopped");
        return;
    }
    //отправить новый чип на проверку
    if(startTest != 1) {nextListLogic++;}
    if(nextListLogic < countListLogic) {
        selectLogicChip(listLogic[nextListLogic]);
    }
    if(nextListLogic >= countListLogic) {
        QObject::disconnect(serialDataConnection);
        data.clear();
        data.append(1);
        data.append(1);
        writeData(data);
        data.clear();
        updateButtons(true, false);
        ui->progressBar->setValue(100);
        log("Search completed");
        //вывод совпадающих чипов
        if(ResultAutoLogic.count() > 0) {
            log(QString("Found %1 logic chips matched:").arg(ResultAutoLogic.count()));
            for(uint8_t i = 0; i < ResultAutoLogic.count(); i++) {
                log(QString("<a href=\"%1\">%1</a>").arg(ResultAutoLogic[i]));
            }
        } else {
            log("No matching logic chips found!");
        }
        return;
    }
    if(countTrLogic < 2) {
        QObject::disconnect(serialDataConnection);
        data.clear();
        data.append(1);
        data.append(1);
        writeData(data);
        data.clear();
        ui->progressBar->setValue(0);
        updateButtons(true, false);
        log("Search stopped");
    } else {
        iTr = 0;
        data.clear();
        data.append(PinLogic.toInt());
        data.append(countTrLogic);
        writeData(data);
        data.clear();
        ui->progressBar->setValue(nextListLogic * 100 / countListLogic);
    }
}

void MainWindow::on_textBrowser_anchorClicked(const QUrl &arg1)
{
    QList<QListWidgetItem*> chipitem = ui->listLogic->findItems(arg1.toString(),Qt::MatchExactly);
    if(chipitem.count() != 1) return;
    ui->listLogic->setCurrentItem(ui->listLogic->item(ui->listLogic->row(chipitem[0])));
    on_listLogic_itemClicked(ui->listLogic->item(ui->listLogic->row(chipitem[0])));
}

void MainWindow::on_showButton_clicked()
{

}

