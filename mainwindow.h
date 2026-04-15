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
#include <QTableWidget>
#include <QTimer>
#include <QHeaderView>

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
    QPushButton* btnAutoConnect;
    QPushButton* btnDisconnect;
    QPushButton* btnSetRTU;
    QPushButton* btnLoadDefault;

    QTableWidget* tableRegLeft;
    QTableWidget* tableRegRight;

    QLabel* modeLabel;
    QLabel* addrLabel;

    QComboBox* boxDevice;
    QComboBox* boxBaudRate;
    QComboBox* boxDataBits;
    QComboBox* boxStopBits;
    QComboBox* boxParity;

    QComboBox* boxSelector;

    QLineEdit* editIP;
    QLineEdit* editPort;
    QLineEdit* editAddr;

    QTextEdit* editResponse;

    QGroupBox* grpSerial;   // Объявляем здесь, а не в конструкторе, чтобы бы видимость была за пределами конструктора
    QGroupBox* grpTcpIp;
    QGroupBox* grpRtuLoad;

    QFormLayout* serialLayout;
    QFormLayout* tcpIpLayout;

private:
    dtc1000 *m_dtc = nullptr;
    QMap<QString, QPoint> map_table_1;
    QMap<QString, QPoint> map_table_2;
    void updateSerialPorts();

private slots:
    void handleCellChangedLeft(int row, int column);
    void handleCellChangedRight(int row, int column);
    void btnConnect_clicked();
    void btnAutoConnect_clicked();
    void btnDisconnect_clicked();
    void btnSetRtu_clicked();
    void updateTable(const QString &addr, int val);
    void updateUi(const QString &str);    // Вызывается после получения сигнала о завершении чтения регистра
    void onConnectionModeChanged(int index);


};
#endif // MAINWINDOW_H
