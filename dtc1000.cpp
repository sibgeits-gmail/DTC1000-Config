#include "dtc1000.h"

dtc1000::dtc1000(QObject *parent)
    : QObject{parent}
{
    m_modbus = new QModbusRtuSerialClient();
    m_serial = new QSerialPort();
    m_socket = new QTcpSocket();

    connect(m_socket,     &QTcpSocket::connected,
            this,          &dtc1000::onTcpConnected);
    connect(m_socket,     &QTcpSocket::readyRead,
            this,          &dtc1000::onReadyRead);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, &dtc1000::onError);
}

void dtc1000::setModbusConnectionOptions(const QString &portName,
                                         QSerialPort::BaudRate baudRate,
                                         QSerialPort::Parity parity,
                                         QSerialPort::DataBits dataBits,
                                         QSerialPort::StopBits stopBits)
{
    m_modbus->setConnectionParameter(QModbusDevice::SerialPortNameParameter, portName);
    m_modbus->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, baudRate);
    m_modbus->setConnectionParameter(QModbusDevice::SerialParityParameter, parity);
    m_modbus->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, dataBits);
    m_modbus->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, stopBits);

    m_modbus->setTimeout(2000);
    m_modbus->setNumberOfRetries(2);
}

void dtc1000::setModbusEnabled()
{
    if (!m_modbus) return;
    if (!m_modbus->connectDevice()) {
        qDebug() << "Не удалось открыть Modbus устройство";
    }
    else{
        qDebug() << "Modbus устройство открыто";
    }
}

void dtc1000::setTcpEnabled(const QString &host, int port)
{
    m_socket->connectToHost(host, port);
}

void dtc1000::setTcpDisabled()
{
    m_socket->disconnectFromHost();
}

bool dtc1000::isTcpEnabled() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

void dtc1000::onTcpConnected()
{
    emit tcpConnected("TCP connection is up");
}

void dtc1000::onError(QAbstractSocket::SocketError err)
{
    Q_UNUSED(err);
    emit tcpConnectionError("❌ Ошибка: " + m_socket->errorString());
}

void dtc1000::setModbusDisabled()
{
    if (isModbusEnabled()) {
        m_modbus->disconnectDevice();
        qDebug() << "Modbus устройство зыкрыто";
    }
}

bool dtc1000::isModbusEnabled() const
{
    return m_modbus->state() == QModbusRtuSerialClient::ConnectedState;
}

bool dtc1000::isSerialEnabled() const
{
    return m_serial->isOpen();
}

void dtc1000::setSerialEnabled()
{
    setModbusDisabled();
    if (m_serial->open(QIODevice::ReadWrite)){
        qDebug() << "Serial устройство открыто";
    }
}

void dtc1000::setSerialDisabled()
{
    if (isSerialEnabled()){
        m_serial->close();
        qDebug() << "Serial устройство закрыто";
    }
}

void dtc1000::readReg(uint16_t reg)
{
    // Запрос
    qDebug() << "readReg";
    QModbusDataUnit req(QModbusDataUnit::HoldingRegisters, reg, 1);
    dtc1000::m_modbus_reply = dtc1000::m_modbus->sendReadRequest(req, 0x01);   // slave‑id = 1

    if (!dtc1000::m_modbus_reply) {
        qWarning() << "sendReadRequest() failed:" << dtc1000::m_modbus->errorString();
        return;
    }

    // Подписываемся на завершение
    connect(m_modbus_reply, &QModbusReply::finished,
            this, &dtc1000::replyCaptured);
}

QByteArray dtc1000::rtu_crc(const QByteArray &payload)
{
    uint16_t crc = 0xffff;
    for (uchar b : payload)
    {
        crc ^= b;
        for (int j = 0; j < 8; j++)
        {
            if(crc & 0x01)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc = crc >> 1;
        }
    }
    qDebug() << QString::number(crc, 16).toUpper();
    // Формируем массив из двух байтов (младший, потом старший)
    QByteArray result;
    result.append(static_cast<char>(crc & 0xFF));   // младший байт
    result.append(static_cast<char>((crc >> 8) & 0xFF));   // старший байт
    return result;
}

void dtc1000::readRegTcp(uint16_t reg)
{
    pendingAddress = reg;   //  Запоминаем запрашиваемый адрес, потому что в ответе адрес не приходит
    pendingCmd = Read;
    QString payloadstr = QString::number(reg, 16);
    payloadstr.prepend("0103");
    payloadstr.append("0001");
    QByteArray payload = QByteArray::fromHex(payloadstr.toLatin1());
    payload.append(dtc1000::rtu_crc(payload));

    qDebug() << tr("→ TX:  %1").arg(QString(payload.toHex(' ').toUpper()));

    m_socket->write(payload);
    m_socket->waitForBytesWritten(2000);
}

void dtc1000::onReadyRead()     // Срабатывает при получении пакета по TCP
{
    QByteArray modbus_reply = m_socket->readAll();
    qDebug() << QString("← RX (RTU): %1").arg(QString(modbus_reply.toHex(' ').toUpper()));
    QByteArray data_bytes = nullptr;
    if (pendingCmd == Read)     // Если была команда на чтение, то в ответе не будет адреса
        data_bytes = modbus_reply.mid(3, 2);
    else if (pendingCmd == Write)
        data_bytes = modbus_reply.mid(4, 2);    // Если была команда на запись, то в ответе будет адрес
    qint16 signedVal = qFromBigEndian<qint16>(reinterpret_cast<const uchar*>(data_bytes.constData()));
    QString value = "Register: 0x" + QString::number(pendingAddress, 16) +  "   Data: " + QString::number(signedVal, 10);
    qDebug() << value;
    emit dataReady(value);
}

void dtc1000::writeRegTcp(uint16_t reg, int16_t data)
{
    pendingAddress = reg;   //  Запоминаем запрашиваемый адрес, потому что в ответе адрес не приходит
    pendingCmd = Write;
    QString payloadstr = QString::number(reg, 16) + QString("%1").arg(data, 4, 16, QChar('0'));
    payloadstr.prepend("0106");
    QByteArray payload = QByteArray::fromHex(payloadstr.toLatin1());
    payload.append(dtc1000::rtu_crc(payload));
    qDebug() << tr("→ TX:  %1").arg(QString(payload.toHex(' ').toUpper()));

    m_socket->write(payload);
    m_socket->waitForBytesWritten(2000);
}


void dtc1000::writeReg(uint16_t reg, int16_t data)
{
    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, reg, 1);
    writeUnit.setValue(0, data);
    dtc1000::m_modbus_reply = dtc1000::m_modbus->sendWriteRequest(writeUnit, 0x01);   // slave‑id = 1

    if (!dtc1000::m_modbus_reply) {
        qWarning() << "sendWriteRequest() failed:" << dtc1000::m_modbus->errorString();
        return;
    }

    connect(m_modbus_reply, &QModbusReply::finished,
            this, &dtc1000::replyCaptured);

}

void dtc1000::setSerialConnectionOptions(const QString &portName,
                                         QSerialPort::BaudRate baudRate,
                                         QSerialPort::Parity parity,
                                         QSerialPort::DataBits dataBits,
                                         QSerialPort::StopBits stopBits)
{
    m_serial->setPortName(portName);
    m_serial->setBaudRate(baudRate);
    m_serial->setDataBits(dataBits);
    m_serial->setParity(parity);
    m_serial->setStopBits(stopBits);
}

void dtc1000::changeToRtu()
{
    QByteArray payload = QByteArray("01061072000176"); // Записать 1 в регистр 0х1072
    payload.append("\r\n");
    payload.prepend(':');
    m_serial->write(payload);
    qDebug() << QString(payload.toHex(' ').toUpper());
    m_serial->waitForBytesWritten(1000);
    connect(m_serial, &QSerialPort::readyRead, this, &dtc1000::asciiReplyCaptured);

}

void dtc1000::replyCaptured()
{
    QModbusDataUnit result = m_modbus_reply->result();
    quint16 address = static_cast<quint16>(result.startAddress());
    QVariant v = result.value(0);
    quint16 raw = v.toUInt();               // беззнаковый 0‑65535
    qint16  signedVal = static_cast<qint16>(raw);
    QString regValue = "Register: 0x" + QString::number(address, 16) +  " Data:  " + QString::number(signedVal, 10);
    emit dataReady(regValue);

    // Очистка
    m_modbus_reply->deleteLater();
    m_modbus_reply = nullptr;
}

void dtc1000::asciiReplyCaptured()
{
    qDebug() << "Data ready";
    // Читаем всё, что пришло в буфер
    QByteArray data = m_serial->readAll();

    if (!data.isEmpty()){
        qDebug() << QString(data);
        if(data.contains("10720001"))   // Если пришёл ответ (в регистр 0х1072 записана 1)
            setSerialDisabled();
            emit RtuIsSet("Установлен режим RTU");
    }
    else
        setSerialDisabled();    
}

