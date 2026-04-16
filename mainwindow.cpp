#include "mainwindow.h"

#include <QWidget>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_dtc = new dtc1000;
    //-------------------------------Main Widget---------------------------------------//
    auto *centralWidget = new QWidget(this);
    centralWidget->setMinimumWidth(950);
    centralWidget->setMinimumHeight(750);
    auto *mainLayout1 = new QHBoxLayout(centralWidget);
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
    grpSerial->setMinimumWidth(250);
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


    //-------------------------------Modbus Address---------------------------------------//
    addrLabel = new QLabel("Modbus address:", this);
    editAddr = new QLineEdit();
    editAddr->setText("1");
    auto *layoutH3 = new QHBoxLayout();
    layoutH3->addWidget(addrLabel);
    layoutH3->addWidget(editAddr);

    //-------------------------------Modbus Address---------------------------------------//


    //-------------------------------Connect/Disconect---------------------------------------//
    btnConnect = new QPushButton("Connect", this);
    btnAutoConnect = new QPushButton("Auto Connect", this);
    btnDisconnect = new QPushButton("Disconnect", this);
    auto *layoutH1 = new QHBoxLayout();
    layoutH1->addWidget(btnConnect);
    layoutH1->addWidget(btnAutoConnect);
    layoutH1->addWidget(btnDisconnect);
    //-------------------------------Connect/Disconect---------------------------------------//


    //-------------------------------Set RTU/Load Default Settings---------------------------------------//
    grpRtuLoad = new QGroupBox(this);
    auto *layoutH2 = new QHBoxLayout();
    btnSetRTU = new QPushButton("Set RTU Mode", this);
    btnLoadDefault = new QPushButton("Load Default Settings", this);
    layoutH2->addWidget(btnSetRTU);
    layoutH2->addWidget(btnLoadDefault);
    grpRtuLoad->setLayout(layoutH2);
    //-------------------------------Set RTU/Load Default Settings---------------------------------------//


    //-------------------------------Register Table---------------------------------------//

    map_table_1["1000"] = QPoint(0, 2);
    map_table_1["1001"] = QPoint(1, 2);
    map_table_1["1002"] = QPoint(2, 2);
    map_table_1["1003"] = QPoint(3, 2);
    map_table_1["1004"] = QPoint(4, 2);
    map_table_1["1005"] = QPoint(5, 2);
    map_table_1["1006"] = QPoint(6, 2);
    map_table_1["100F"] = QPoint(7, 2);
    map_table_1["1010"] = QPoint(8, 2);
    map_table_1["1011"] = QPoint(9, 2);
    map_table_1["1020"] = QPoint(10, 2);
    map_table_1["1021"] = QPoint(11, 2);
    map_table_1["1023"] = QPoint(12, 2);
    //map_table_2["102A"] = QPoint(3, 2);       // Вручную указываются ячейки в обработчики для этого регистра (2 ячейки для 2ух битов выходов)
    map_table_1["1071"] = QPoint(15, 2);
    map_table_1["1072"] = QPoint(16, 2);
    map_table_1["1073"] = QPoint(17, 2);
    map_table_1["1074"] = QPoint(18, 2);
    map_table_1["1075"] = QPoint(19, 2);
    map_table_1["1076"] = QPoint(20, 2);

    QList<QString> registers = {"1000", "1001", "1002", "1003", "1004", "1005",
                            "1006", "100F", "1010", "1011", "1020", "1021",
                            "1023", "102A b5", "102A b6", "1071", "1072", "1073", "1074",
                            "1075", "1076"};
    QList<QString> regNames = {"Изм. значение (PV)", "Уставка (SV)", "Верхний предел", "Нижний предел", "Тип датчика", "Метод рег.",
                               "Метод упр. вых. 1", "Зона нечувствительности", "Гистерезис выхода 1", "Гистерезис выхода 2", "Режим раб. сигн. вых. 1", "Режим раб. сигн. вых. 2",
                               "Метод упр. вых. 2", "Выход 2", "Выход 1", "Адрес Modbus", "Формат данных", "Скорость обмена", "Длина пакета данных",
                               "Чётность", "Стоповые биты"};

    tableReg = new QTableWidget(this);
    tableReg->setColumnCount(3);
    tableReg->setHorizontalHeaderLabels({"Address", "Name", "Data"});
    tableReg->verticalHeader()->setVisible(false);
    tableReg->horizontalHeader()->setStretchLastSection(true);
    tableReg->setColumnWidth(0, 80);
    tableReg->setColumnWidth(1, 200);
    tableReg->setColumnWidth(2, 80);
    tableReg->setMinimumHeight(350);

    for (int row = 0; row < registers.count(); row++){
        tableReg->insertRow(row);
        tableReg->setItem(row, 0, new QTableWidgetItem(registers[row]));
        tableReg->item(row,0)->setTextAlignment(Qt::AlignCenter);
        tableReg->setItem(row, 1, new QTableWidgetItem(regNames[row]));
    }

    //-------------------------------Register Table---------------------------------------//

    editResponse = new QTextEdit(centralWidget);

    auto *layoutV1 = new QVBoxLayout();
    layoutV1->addWidget(grpTcpIp);
    layoutV1->addLayout(layoutH1);
    layoutV1->addLayout(layoutH3);

    auto *layoutConnection = new QHBoxLayout();
    layoutConnection->addWidget(grpSerial);
    layoutConnection->addLayout(layoutV1);

    auto *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(modeLabel);
    mainLayout->addWidget(boxSelector);
    mainLayout->addLayout(layoutConnection);
    mainLayout->addWidget(grpRtuLoad);
    mainLayout->addWidget(editResponse);

    mainLayout1->addLayout(mainLayout);
    mainLayout1->addWidget(tableReg);


    setCentralWidget(centralWidget);

    connect(boxSelector, &QComboBox::currentIndexChanged,
            this, &MainWindow::onConnectionModeChanged);
    connect(btnConnect, &QPushButton::clicked,
            this, &MainWindow::btnConnect_clicked);
    connect(btnAutoConnect, &QPushButton::clicked,
            this, &MainWindow::btnAutoConnect_clicked);
    connect(btnDisconnect, &QPushButton::clicked,
            this, &MainWindow::btnDisconnect_clicked);
    connect(btnSetRTU, &QPushButton::clicked,
            this, &MainWindow::btnSetRtu_clicked);
    connect(tableReg, &QTableWidget::cellChanged,
            this, &MainWindow::handleCellChanged);

    connect(m_dtc, &dtc1000::dataReceived, this, &MainWindow::updateTable);
    connect(m_dtc, &dtc1000::infoMessage, this, &MainWindow::updateUi);

    updateSerialPorts();
}

MainWindow::~MainWindow()
{

}


void MainWindow::updateSerialPorts() {
    boxDevice->clear(); // Очищаем старый список устройств

    // Получаем список всех доступных портов
    const auto infos = QSerialPortInfo::availablePorts();

    for (const QSerialPortInfo &info : infos) {
        // Добавляем системное имя (например, /dev/ttyUSB0 или COM3)
        boxDevice->addItem(info.portName());
    }

    if (boxDevice->count() == 0) {
        boxDevice->addItem("Порты не найдены");
    }
}

void MainWindow::updateTable(const QString &addr, int val)  //   Обновить таблицу при получении данных от устройства
{
    tableReg->blockSignals(true);   // Отключим сигналы таблиц, чтобы изменение значений в ячейках не вызвало отправку данных

    auto isEditing = [](QTableWidget* table, int row, int col){ // Проверка - редактируется ли ячейка пользователем
        return table->viewport()->focusWidget() != nullptr &&
                                table->currentRow() == row &&
                                table->currentColumn() == col;
    };

    if (map_table_1.contains(addr)){    // Обработчик для таблицы
        int row = map_table_1[addr].x();
        int col = map_table_1[addr].y();
        if (!isEditing(tableReg, row, col))
            tableReg->setItem(row, col, new QTableWidgetItem(QString::number(val)));
    }
    else if (addr == "102A"){   // Отдельный обработчик для этого регистра, потому что в нём 2 параметра
        tableReg->setItem(13, 2, new QTableWidgetItem(QString::number((val >> 5) & 1)));
        tableReg->setItem(14, 2, new QTableWidgetItem(QString::number((val >> 6) & 1)));
    }

    tableReg->blockSignals(false);
}

void MainWindow::handleCellChanged(int row, int column) // Записать в регистр после ручного ввода в таблицу
{
    QTableWidgetItem *itemVal = tableReg->item(row, column);
    QTableWidgetItem *itemAddr = tableReg->item(row, 0);

    bool ok;
    uint16_t hex_addr = static_cast<uint16_t>(itemAddr->text().toUShort(&ok, 16));
    int16_t dec_data = static_cast<int16_t>(itemVal->text().toShort(&ok, 10));

    if (ok) {
        qDebug() << "Записываем в контроллер:" << dec_data << "Address:" << hex_addr;
        if (boxSelector->currentIndex() == 0){  // Serial
            m_dtc->writeReg(hex_addr, dec_data);
        }
        else if (boxSelector->currentIndex() == 1){  // TCP/IP
            m_dtc->writeRegTcp(hex_addr, dec_data);
        }
    }
}

void MainWindow::btnConnect_clicked()
{
    m_dtc->setSerialDisabled(); // Если после нажатия Set RTU Mode не будет ответа на ascii команду, то serial не закроется

    if(m_dtc->isModbusEnabled() || m_dtc->isTcpEnabled()){
        editResponse->append("Соединение уже установлено");
        return;
    }
    m_dtc->slaveID = editAddr->text().toUShort();
    if (boxSelector->currentIndex() == 0){  // Serial
        m_dtc->setSerialConnectionOptions(boxDevice->currentText(),
                                          static_cast<QSerialPort::BaudRate>(boxBaudRate->currentData().toInt()),
                                          static_cast<QSerialPort::Parity>(boxParity->currentData().toInt()),
                                          static_cast<QSerialPort::DataBits>(boxDataBits->currentData().toInt()),
                                          static_cast<QSerialPort::StopBits>(boxStopBits->currentData().toInt()));
        m_dtc->setModbusEnabled();
    }
    else if (boxSelector->currentIndex() == 1){  // TCP/IP
        m_dtc->setTcpEnabled(editIP->text(), editPort->text().toInt());
    }
}

void MainWindow::btnAutoConnect_clicked()   // Перебор параметров serial
{
    if(m_dtc->isModbusEnabled() || m_dtc->isTcpEnabled()){
        editResponse->append("Соединение уже установлено");
        return;
    }

    m_dtc->slaveID = editAddr->text().toUShort();

    QList<QSerialPort::BaudRate> baudrate = {QSerialPort::BaudRate::Baud38400,
                                             QSerialPort::BaudRate::Baud19200,
                                             QSerialPort::BaudRate::Baud9600,
                                             QSerialPort::BaudRate::Baud4800,
                                             QSerialPort::BaudRate::Baud2400};
    QList<QSerialPort::Parity> parity = {QSerialPort::Parity::EvenParity,
                                         QSerialPort::Parity::OddParity,
                                         QSerialPort::Parity::NoParity};
    QList<QSerialPort::DataBits> dataBits = {QSerialPort::DataBits::Data7,
                                             QSerialPort::DataBits::Data8};
    QList<QSerialPort::StopBits> stopBits = {QSerialPort::StopBits::OneStop,
                                             QSerialPort::StopBits::TwoStop};

    auto parityToString = [](QSerialPort::Parity parity){
        switch (parity) {
            case QSerialPort::NoParity:   return "None";
            case QSerialPort::EvenParity: return "Even";
            case QSerialPort::OddParity:  return "Odd";
            default: return "Unknown parity";
        }
    };

    for (auto baud : baudrate){
        for (auto par : parity){
            for (auto db : dataBits){
                for (auto sb : stopBits){
                    m_dtc->setSerialConnectionOptions(boxDevice->currentText(), baud, par, db, sb);
                    m_dtc->setModbusEnabled();
                    if (m_dtc->isModbusEnabled()){
                        editResponse->append("Baudrate: " + QString::number(baud) + '\n' +
                                             "Parity: " + parityToString(par) + '\n' +
                                             "Data Bits: " + QString::number(db) + '\n' +
                                             "Stop Bits: " + QString::number(sb));
                        return;
                    }
                }
            }
        }
    }
}

void MainWindow::btnDisconnect_clicked()
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

void MainWindow::btnSetRtu_clicked() // Поменять формат сообщений с ASCII на RTU
{
    if (m_dtc->isModbusEnabled())
        m_dtc->setModbusDisabled();
    else if (m_dtc->isTcpEnabled())
        m_dtc->setTcpDisabled();
    m_dtc->setSerialConnectionOptions(boxDevice->currentText(),
                                      static_cast<QSerialPort::BaudRate>(boxBaudRate->currentData().toInt()),
                                      static_cast<QSerialPort::Parity>(boxParity->currentData().toInt()),
                                      static_cast<QSerialPort::DataBits>(boxDataBits->currentData().toInt()),
                                      static_cast<QSerialPort::StopBits>(boxStopBits->currentData().toInt()));
    m_dtc->slaveID = editAddr->text().toUShort();
    m_dtc->changeToRtu();
}

void MainWindow::updateUi(const QString &str)   // Вывести что-нибудь в терминал
{
    editResponse->append(str);
}

void MainWindow::onConnectionModeChanged(int index) {   // Изменение способа подключения
    bool isSerial = (index == 0);
    grpSerial->setEnabled(isSerial);
    grpTcpIp->setEnabled(!isSerial);
    updateSerialPorts();
}
