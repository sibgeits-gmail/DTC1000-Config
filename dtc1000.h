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
#include <QTimer>

class dtc1000 : public QObject
{
    Q_OBJECT

public:

    dtc1000(QObject *parent = nullptr);
    ~dtc1000() = default;


    void setSerialConnectionOptions(const QString &portName,
                                    QSerialPort::BaudRate baudRate,
                                    QSerialPort::Parity parity,
                                    QSerialPort::DataBits dataBits,
                                    QSerialPort::StopBits stopBits);

    void setModbusEnabled();    // Не умеет работать с ASCII
    void setModbusDisabled();
    bool isModbusEnabled() const;

    void setTcpEnabled(const QString &host, int port);
    void setTcpDisabled();
    bool isTcpEnabled() const;

    void setSerialEnabled();    // Для работы с ASCII
    void setSerialDisabled();
    bool isSerialEnabled() const;

    void requestUpdate();

    void readRegTcp(uint16_t reg);

    void writeReg(uint16_t reg, int16_t data);
    void writeRegTcp(uint16_t reg, int16_t data);

    void changeToRtu();

    uint8_t slaveID;
    QList<QString> registers;
    uint8_t rx_counter, tx_counter;


private:
    void processNextRequest();

private slots:
    void onTcpConnected();
    void onError(QAbstractSocket::SocketError err);
    void onReadyRead(); // Данные пришли в сокет
    void replyCaptured();

private:
    QModbusClient *m_modbus;    // Для отправки RTU
    QTcpSocket *m_socket;  // Для отправки RTU
    QSerialPort *m_serial;  // Для отправки ASCII
    QModbusReply *m_modbus_reply = nullptr;
    QTimer* m_tmr;
    QList<QModbusDataUnit> m_requestQueue; // Очередь запросов для отправки по serial
    QByteArray rtu_crc(const QByteArray &payload);  // Расчёт crc для работы по TCP

    quint16 pendingAddress; //  Запоминаем адрес регистра, потому что в ответе на чтение не содержится адрес регистра
    enum pendingCmd     // Запоминаем вид команды(чтение/запись) при запросе по TCP, нужно в обработчике
    {
        Read,
        Write
    }pendingCmd;

signals:
    void dataReceived(const QString &addr, int val);
    void infoMessage(const QString &str); // Передача информации в терминал UI
};


