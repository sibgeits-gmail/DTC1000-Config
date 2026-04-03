#include "mainwindow.h"

#include <QWidget>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_dtc = new dtc1000;
    //-------------------------------Main Widget---------------------------------------//
    auto *centralWidget = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(centralWidget);
    //-------------------------------Main Widget---------------------------------------//


    //-------------------------------SERIAL SETTINGS---------------------------------------//
    grpSerial = new QGroupBox("Serial settings", centralWidget);
    serialLayout = new QFormLayout(grpSerial);

    boxDevice = new QComboBox(grpSerial);
    serialLayout->addRow("Device:", boxDevice);

    boxBaudRate = new QComboBox(grpSerial);
    boxBaudRate->addItem("38400", QSerialPort::Baud38400);
    boxBaudRate->addItem("19200", QSerialPort::Baud19200);
    boxBaudRate->addItem("9600", QSerialPort::Baud9600);
    serialLayout->addRow("Baud Rate:", boxBaudRate);

    boxParity = new QComboBox(grpSerial);
    boxParity->addItem("Even", QSerialPort::EvenParity);
    boxParity->addItem("Odd", QSerialPort::OddParity);
    boxParity->addItem("None", QSerialPort::NoParity);
    serialLayout->addRow("Parity:", boxParity);

    boxDataBits = new QComboBox(grpSerial);
    boxDataBits->addItem("8", QSerialPort::Data8);
    boxDataBits->addItem("7", QSerialPort::Data7);
    serialLayout->addRow("Data Bits:", boxDataBits);

    boxStopBits = new QComboBox(grpSerial);
    boxStopBits->addItem("1", QSerialPort::OneStop);
    boxStopBits->addItem("2", QSerialPort::TwoStop);
    serialLayout->addRow("Stop Bits:", boxStopBits);

    grpSerial->setEnabled(true);    // Изначально в комбобоксе выбран способ подключения Serial
    //-------------------------------SERIAL SETTINGS---------------------------------------//


    //-------------------------------TCP/IP SETTINGS---------------------------------------//
    grpTcpIp = new QGroupBox("TCP/IP settings", centralWidget);
    tcpIpLayout = new QFormLayout(grpTcpIp);

    //Поле ввода IP
    editIP = new QLineEdit(grpTcpIp);
    //editIP->setInputMask("000.000.000.000;");
    editIP->setText("192.168.124.177");
    tcpIpLayout->addRow("IP Address:", editIP);

    //Поле ввода Порта
    editPort = new QLineEdit(grpTcpIp);
    editPort->setText("4001");
    editPort->setPlaceholderText("4001"); // Подсказка внутри пустого поля

    // Добавляем валидатор: разрешаем только целые числа от 1 до 65535
    auto *portValidator = new QIntValidator(1, 65535, this);
    editPort->setValidator(portValidator);

    tcpIpLayout->addRow("Port:", editPort);

    grpTcpIp->setEnabled(false);    // Изначально в комбобоксе выбран способ подключения Serial
    //-------------------------------TCP/IP SETTINGS---------------------------------------//


    //-------------------------------Connection Type---------------------------------------//
    modeLabel = new QLabel("Connection Type:", this);
    boxSelector = new QComboBox(this);
    boxSelector->addItem("Serial Port"); // Индекс 0
    boxSelector->addItem("Modbus RTU over TCP"); // Индекс 1
    //-------------------------------Connection Type---------------------------------------//


    //-------------------------------Connect/Disconect---------------------------------------//
    btnConnect = new QPushButton("Connect", this);
    btnDisconnect = new QPushButton("Disconnect", this);
    auto *layoutH1 = new QHBoxLayout();
    layoutH1->addWidget(btnConnect);
    layoutH1->addWidget(btnDisconnect);
    //-------------------------------Connect/Disconect---------------------------------------//


    //-------------------------------Set RTU/Load Default Settings---------------------------------------//
    grpRtuLoad = new QGroupBox(this);
    auto *layoutH2 = new QHBoxLayout;
    btnSetRTU = new QPushButton("Set RTU Mode", this);
    btnLoadDefault = new QPushButton("Load Default Settings", this);
    layoutH2->addWidget(btnSetRTU);
    layoutH2->addWidget(btnLoadDefault);
    grpRtuLoad->setLayout(layoutH2);
    //-------------------------------Set RTU/Load Default Settings---------------------------------------//


    //-------------------------------Read Register---------------------------------------//
    grpRead = new QGroupBox("Read Register", centralWidget);
    auto *readLayout = new QFormLayout(grpRead);
    editReadAddr = new QLineEdit(grpRead);
    editReadAddr->setPlaceholderText("1000");
    editReadAddr->setMaxLength(4);

    btnRead = new QPushButton("Read Data", grpRead);
    readLayout->addRow("Address:", editReadAddr);
    readLayout->addRow(btnRead);
    //-------------------------------Read Register---------------------------------------//


    //-------------------------------Write Register---------------------------------------//
    grpWrite = new QGroupBox("Write Register", centralWidget);
    auto *writeLayout = new QFormLayout(grpWrite);

    editWriteAddr = new QLineEdit(grpWrite);
    editWriteAddr->setPlaceholderText("1000");
    editWriteAddr->setMaxLength(4);

    // Поле ввода данных (Data)
    editWriteData = new QLineEdit(grpWrite);
    editWriteData->setPlaceholderText("Value");

    // Кнопка записи
    btnWrite = new QPushButton("Write Data", grpWrite);

    // Добавляем элементы в форму
    writeLayout->addRow("Address:", editWriteAddr);
    writeLayout->addRow("Data:", editWriteData);
    writeLayout->addRow(btnWrite);
    //-------------------------------Write Register---------------------------------------//


    editResponse = new QTextEdit(centralWidget);

    auto* layoutRw = new QHBoxLayout();
    layoutRw->addWidget(grpRead);
    layoutRw->addWidget(grpWrite);

    auto layoutV1 = new QVBoxLayout();
    layoutV1->addWidget(grpTcpIp);
    layoutV1->addLayout(layoutH1);

    auto *layoutConnection = new QHBoxLayout();
    layoutConnection->addWidget(grpSerial);
    layoutConnection->addLayout(layoutV1);


    mainLayout->addWidget(modeLabel);
    mainLayout->addWidget(boxSelector);
    mainLayout->addLayout(layoutConnection);
    mainLayout->addWidget(grpRtuLoad);
    mainLayout->addLayout(layoutRw);
    mainLayout->addWidget(editResponse);
    mainLayout->addStretch();

    setCentralWidget(centralWidget);

    connect(boxSelector, &QComboBox::currentIndexChanged,
            this, &MainWindow::onConnectionModeChanged);
    connect(btnConnect, &QPushButton::clicked,
            this, &MainWindow::on_btnConnect_clicked);
    connect(btnRead, &QPushButton::clicked,
            this, &MainWindow::on_btnRead_clicked);
    connect(btnWrite, &QPushButton::clicked,
            this, &MainWindow::on_btnWrite_clicked);
    connect(btnDisconnect, &QPushButton::clicked,
            this, &MainWindow::on_btnDisconnect_clicked);
    connect(btnSetRTU, &QPushButton::clicked,
            this, &MainWindow::on_btnSetRtu_clicked);

    connect(m_dtc, &dtc1000::dataReady, this, &MainWindow::updateUi);
    connect(m_dtc, &dtc1000::tcpConnected, this, &MainWindow::onTcpConnected);
    connect(m_dtc, &dtc1000::tcpConnectionError, this, &MainWindow::updateUi);
    connect(m_dtc, &dtc1000::RtuIsSet, this, &MainWindow::updateUi);

    updateSerialPorts();
}

MainWindow::~MainWindow()
{

}


void MainWindow::updateSerialPorts() {
    boxDevice->clear(); // Очищаем старый список

    // Получаем список всех доступных портов
    const auto infos = QSerialPortInfo::availablePorts();

    for (const QSerialPortInfo &info : infos) {
        // Добавляем системное имя (например, /dev/ttyUSB0 или COM3)
        // Можно также добавить описание для удобства: info.description()
        boxDevice->addItem(info.portName());
    }

    if (boxDevice->count() == 0) {
        boxDevice->addItem("Порты не найдены");
    }
}

void MainWindow::on_btnConnect_clicked()
{
    m_dtc->setSerialDisabled();
    ///Проверка, не подключено ли уже

    if(m_dtc->isModbusEnabled() || m_dtc->isTcpEnabled()){
        editResponse->append("Соединение уже установлено");
        return;
    }
    if (boxSelector->currentIndex() == 0){  // Serial
        m_dtc->setModbusConnectionOptions("boxDevice->currentText(),
                                          static_cast<QSerialPort::BaudRate>(boxBaudRate->currentData().toInt()),
                                          static_cast<QSerialPort::Parity>(boxParity->currentData().toInt()),
                                          static_cast<QSerialPort::DataBits>(boxDataBits->currentData().toInt()),
                                          static_cast<QSerialPort::StopBits>(boxStopBits->currentData().toInt()));
        m_dtc->setModbusEnabled();
        if (m_dtc->isModbusEnabled()) {
            editResponse->append("✔ Cоединение установлено");
        }
        else{
            editResponse->append("❌ Не удалось установить соединение");
        }
    }
    else if (boxSelector->currentIndex() == 1){  // TCP/IP
        if (!m_dtc->isTcpEnabled())
            m_dtc->setTcpEnabled(editIP->text(), editPort->text().toInt());
    }

}

void MainWindow::on_btnDisconnect_clicked()
{
    if (m_dtc->isModbusEnabled()) {
        m_dtc->setModbusDisabled();
        editResponse->append("Соединение разорвано");
        return;
    }
    else if (m_dtc->isTcpEnabled()){
        m_dtc->setTcpDisabled();
        editResponse->append("TCP-соединение разорвано");
    }
    else{
        editResponse->append("Нет активных соединений");
    }
}

void MainWindow::on_btnSetRtu_clicked()
{
    if (boxSelector->currentIndex() == 0){  // Serial
        m_dtc->readReg(0x1000);
    }
    else if (boxSelector->currentIndex() == 1){  // TCP/IP
        m_dtc->readRegTcp(0x1000);
    }

    m_dtc->setSerialConnectionOptions("/dev/"+boxDevice->currentText(),
                                      static_cast<QSerialPort::BaudRate>(boxBaudRate->currentData().toInt()),
                                      static_cast<QSerialPort::Parity>(boxParity->currentData().toInt()),
                                      static_cast<QSerialPort::DataBits>(boxDataBits->currentData().toInt()),
                                      static_cast<QSerialPort::StopBits>(boxStopBits->currentData().toInt()));
    m_dtc->setSerialEnabled();
    m_dtc->changeToRtu();
}

void MainWindow::on_btnRead_clicked()
{
    if (!(m_dtc->isModbusEnabled() || m_dtc->isTcpEnabled())) {
        QMessageBox::warning(this, tr("Нет соединения"),
                             tr("Сначала дождитесь установки соединения."));
        return;
    }

    QString reg = editReadAddr->text();  /// Добавить проверку ввода
    bool ok;
    // Преобразуем строку, считая, что в ней записано HEX-число
    uint16_t hex_reg = static_cast<uint16_t>(reg.toUShort(&ok, 16));

    if (boxSelector->currentIndex() == 0){  // Serial
        m_dtc->readReg(hex_reg);
    }
    else if (boxSelector->currentIndex() == 1){  // TCP/IP
        m_dtc->readRegTcp(hex_reg);
    }
}

void MainWindow::on_btnWrite_clicked()
{
    if (!(m_dtc->isModbusEnabled() || m_dtc->isTcpEnabled())) {
        QMessageBox::warning(this, tr("Нет соединения"),
                             tr("Сначала дождитесь установки соединения."));
        return;
    }

    bool ok;
    QString addr = editWriteAddr->text();  /// Добавить проверку ввода
    QString data = editWriteData->text();
    uint16_t hex_addr = static_cast<uint16_t>(addr.toUShort(&ok, 16));
    int16_t dec_data = static_cast<int16_t>(data.toShort(&ok, 10));
    if (boxSelector->currentIndex() == 0){  // Serial
        m_dtc->writeReg(hex_addr, dec_data);
    }
    else if (boxSelector->currentIndex() == 1){  // TCP/IP
        m_dtc->writeRegTcp(hex_addr, dec_data);
    }
}

void MainWindow::updateUi(const QString &str)
{
    editResponse->append(str);
}

void MainWindow::onTcpConnected()   // Переделать под updateUI
{
    editResponse->append("✔ TCP‑соединение установлено");
}

void MainWindow::onConnectionModeChanged(int index) {
    bool isSerial = (index == 0);

    // Включаем одну группу, выключаем другую
    grpSerial->setEnabled(isSerial);
    grpTcpIp->setEnabled(!isSerial);
    updateSerialPorts();
}
