#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QMessageBox>

#include <QSerialPortInfo>

#include "dtc1000.h"



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QPushButton* btnConnect;
    QPushButton* btnDisconnect;
    QPushButton* btnRead;
    QPushButton* btnWrite;
    QPushButton* btnSetRTU;
    QPushButton* btnLoadDefault;

    QLabel* modeLabel;

    QComboBox* boxDevice;
    QComboBox* boxBaudRate;
    QComboBox* boxDataBits;
    QComboBox* boxStopBits;
    QComboBox* boxParity;

    QComboBox* boxSelector;

    QLineEdit* editIP;
    QLineEdit* editPort;
    QLineEdit* editReadAddr;
    QLineEdit* editWriteAddr;
    QLineEdit* editWriteData;

    QTextEdit* editResponse;

    QGroupBox* grpSerial;   // Объявляем здесь, а не в конструкторе, чтобы бы видимость была за пределами конструктора
    QGroupBox* grpTcpIp;
    QGroupBox* grpRead;
    QGroupBox* grpWrite;
    QGroupBox* grpRtuLoad;


    QFormLayout* serialLayout;
    QFormLayout* tcpIpLayout;

private:
    dtc1000 *m_dtc = nullptr;
    void updateSerialPorts();

private slots:
    void on_btnConnect_clicked();
    void on_btnDisconnect_clicked();
    void onTcpConnected();
    void on_btnRead_clicked();
    void on_btnWrite_clicked();
    void on_btnSetRtu_clicked();
    void updateUi(const QString &str);    // Вызывается после получения сигнала о завершении чтения регистра
    void onConnectionModeChanged(int index);


};
#endif // MAINWINDOW_H
