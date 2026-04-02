#pragma once

#include <QObject>
#include <QModbusClient>
#include <QModbusReply>
#include <QModbusRtuSerialClient>
#include <QModbusDataUnit>
#include <QRegularExpression>
#include <QSerialPort>
#include <QVariant>
#include <QTcpSocket>
#include <QtEndian>

class dtc1000 : public QObject
{
    Q_OBJECT

public:

    dtc1000(QObject *parent = nullptr);
    ~dtc1000() = default;

    void setModbusConnectionOptions(const QString &portName,
                                    QSerialPort::BaudRate baudRate,
                                    QSerialPort::Parity parity,
                                    QSerialPort::DataBits dataBits,
                                    QSerialPort::StopBits stopBits);

    void setSerialConnectionOptions(const QString &portName,
                                    QSerialPort::BaudRate baudRate,
                                    QSerialPort::Parity parity,
                                    QSerialPort::DataBits dataBits,
                                    QSerialPort::StopBits stopBits);

    void setModbusEnabled();
    void setModbusDisabled();
    bool isModbusEnabled() const;

    void setTcpEnabled(const QString &host, int port);
    void setTcpDisabled();
    bool isTcpEnabled() const;

    void setSerialEnabled();
    void setSerialDisabled();
    bool isSerialEnabled() const;

    void readReg(uint16_t reg);
    void readRegTcp(uint16_t reg);

    void writeReg(uint16_t reg, int16_t data);
    void writeRegTcp(uint16_t reg, int16_t data);

    void changeToRtu();


private slots:
    void onTcpConnected();
    void onError(QAbstractSocket::SocketError err);
    void onReadyRead();
    void replyCaptured();             // будет вызван, когда придёт ответ
    void asciiReplyCaptured();


private:
    QModbusClient *m_modbus;    // Для отправки RTU
    QTcpSocket *m_socket;  // Для отправки RTU
    QSerialPort *m_serial;  // Для отправки ASCII
    QTcpSocket *m_tcpSocket; // Для отправки ASCII
    QModbusReply *m_modbus_reply = nullptr;

    QByteArray rtu_crc(const QByteArray &payload);

    quint16 pendingAddress; //  Запоминаем адрес регистра, потому что в ответе на чтение не содержится адрес регистра
    enum pendingCmd     // Запоминаем вид команды(чтение/запись) при запросе, нужно в обработчике
    {
        Read,
        Write
    }pendingCmd;

signals:
    void dataReady(const QString &str); // Сигнал о завершении чтения регистра
    void tcpConnected(const QString &str);
    void tcpConnectionError(const QString &str);
    void RtuIsSet(const QString &str);
};


