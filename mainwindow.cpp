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


    //-------------------------------Register Table---------------------------------------//
    auto *layoutH4 = new QHBoxLayout();
    btnSaveConfig = new QPushButton("Save Config", this);
    btnLoadConfig = new QPushButton("Load Config", this);
    layoutH4->addWidget(btnSaveConfig);
    layoutH4->addWidget(btnLoadConfig);

    tableReg = new QTableWidget(this);
    tableReg->setColumnCount(3);
    tableReg->setHorizontalHeaderLabels({"Address", "Name", "Data"});
    tableReg->verticalHeader()->setVisible(false);
    tableReg->horizontalHeader()->setStretchLastSection(true);
    tableReg->setColumnWidth(0, 80);
    tableReg->setColumnWidth(1, 200);
    tableReg->setColumnWidth(2, 80);
    tableReg->setMinimumHeight(350);

    //-------------------------------Register Table---------------------------------------//


    //-------------------------------Edit Table---------------------------------------//
    auto *layoutH5 = new QHBoxLayout();
    btnAddRow = new QPushButton("+", this);
    btnAddRow->setStyleSheet("color: green; font-weight: bold; font-size: 18px;");
    btnDelRow = new QPushButton("-", this);
    btnDelRow->setStyleSheet("color: red; font-weight: bold; font-size: 18px;");
    layoutH5->addWidget(btnAddRow);
    layoutH5->addWidget(btnDelRow);

    //-------------------------------Edit Table---------------------------------------//

    editResponse = new QTextEdit(centralWidget);

    auto *layoutV1 = new QVBoxLayout();
    layoutV1->addWidget(grpTcpIp);
    layoutV1->addLayout(layoutH1);
    layoutV1->addLayout(layoutH3);

    auto *layoutV2 = new QVBoxLayout();
    layoutV2->addLayout(layoutH5);
    layoutV2->addWidget(tableReg);
    layoutV2->addLayout(layoutH4);

    auto *layoutConnection = new QHBoxLayout();
    layoutConnection->addWidget(grpSerial);
    layoutConnection->addLayout(layoutV1);

    auto *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(modeLabel);
    mainLayout->addWidget(boxSelector);
    mainLayout->addLayout(layoutConnection);
    mainLayout->addWidget(editResponse);

    mainLayout1->addLayout(mainLayout);
    mainLayout1->addLayout(layoutV2);


    setCentralWidget(centralWidget);

    connect(boxSelector, &QComboBox::currentIndexChanged,
            this, &MainWindow::onConnectionModeChanged);
    connect(btnSaveConfig, &QPushButton::clicked,
            this, &MainWindow::saveConfig);
    connect(btnLoadConfig, &QPushButton::clicked,
            this, &MainWindow::loadConfig);
    connect(btnAddRow, &QPushButton::clicked,
            this, &MainWindow::addRow);
    connect(btnDelRow, &QPushButton::clicked,
            this, &MainWindow::delRow);
    connect(btnConnect, &QPushButton::clicked,
            this, &MainWindow::btnConnect_clicked);
    connect(btnAutoConnect, &QPushButton::clicked,
            this, &MainWindow::btnAutoConnect_clicked);
    connect(btnDisconnect, &QPushButton::clicked,
            this, &MainWindow::btnDisconnect_clicked);
    connect(tableReg, &QTableWidget::cellChanged,
            this, &MainWindow::cellChanged);

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
        boxDevice->addItem(info.portName());
    }

    if (boxDevice->count() == 0) {
        boxDevice->addItem("Порты не найдены");
    }
}

void MainWindow::updateRegisters()
{
    m_dtc->registers.clear();
    for (int i = 0; i < tableReg->rowCount(); i++){
        m_dtc->registers.append(tableReg->item(i, 0)->text());
    }
}

void MainWindow::updateTable(const QString &addr, int val)  //   Обновить таблицу при получении данных от устройства
{
    tableReg->blockSignals(true);   // Отключим сигналы таблиц, чтобы изменение значений в ячейках не вызвало отправку данных
    int col = 2;
    bool done = false;

    auto isEditing = [](QTableWidget* table, int row, int col){ // Проверка - редактируется ли ячейка пользователем
        return table->viewport()->focusWidget() != nullptr &&
                                table->currentRow() == row &&
                                table->currentColumn() == col;
    };

    for (int i = 0; i < m_dtc->registers.size(); i++){  // Сначал проверить, есть ли регистр с выводом бита
        if (m_dtc->registers[i].contains(addr) && m_dtc->registers[i].contains("bit")){
            uint8_t bit = m_dtc->registers[i].right(1).toUInt();    // Номер бита, который нужно вывести прописан справа (102A bit5)
            int row = i;
            if (!isEditing(tableReg, row, col))
                tableReg->setItem(row, col, new QTableWidgetItem(QString::number((val >> bit) & 1)));
            done = true;
        }
    }

    if (done) return;

    int row = m_dtc->registers.indexOf(addr);
    if (!isEditing(tableReg, row, col))
        tableReg->setItem(row, col, new QTableWidgetItem(QString::number(val)));

    tableReg->blockSignals(false);
}

void MainWindow::cellChanged(int row, int column) // Записать в регистр после ручного ввода в таблицу
{
    if (column != 2) {
        return;
    }

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

void MainWindow::saveConfig()   // Сохранить таблицу в json
{
    QString filePath = QFileDialog::getSaveFileName(this,
                                                    "Сохранить конфигурацию",
                                                    "",
                                                    "JSON файлы (*.json);;Все файлы (*)");
    if (filePath.isEmpty()) return; // Если пользователь нажал "Отмена"

    QJsonArray rootArray;

    for (int i = 0; i < tableReg->rowCount(); i++){
        QJsonObject row;
        row["address"] = tableReg->item(i, 0) ? tableReg->item(i, 0)->text() : "";
        row["name"]    = tableReg->item(i, 1) ? tableReg->item(i, 1)->text() : "";
        row["data"]    = tableReg->item(i, 2) ? tableReg->item(i, 2)->text() : "";
        rootArray.append(row);
    }
    QJsonDocument doc(rootArray);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void MainWindow::loadConfig() {
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    "Загрузить конфигурацию",
                                                    "",
                                                    "JSON файлы (*.json);;Все файлы (*)");
    if (filePath.isEmpty()) return; // Если пользователь нажал "Отмена"
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (doc.isNull() || !doc.isArray()) return;

    tableReg->blockSignals(true);
    QJsonArray rootArray = doc.array();
    tableReg->setRowCount(0); // Очищаем таблицу перед загрузкой

    for (int i = 0; i < rootArray.size(); ++i) {
        QJsonObject row = rootArray.at(i).toObject();
        tableReg->insertRow(i);

        tableReg->setItem(i, 0, new QTableWidgetItem(row["address"].toString()));
        tableReg->setItem(i, 1, new QTableWidgetItem(row["name"].toString()));
        tableReg->setItem(i, 2, new QTableWidgetItem(row["data"].toString()));
    }
    tableReg->blockSignals(false);
    updateRegisters();
}

void MainWindow::addRow()
{
    AddRegDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted){
        QString addr = dialog.getAddress();
        QString name = dialog.getName();

        if (addr.isEmpty()) return; // Не добавляем пустые строки

        int row = tableReg->rowCount();
        tableReg->insertRow(row);
        tableReg->setItem(row, 0, new QTableWidgetItem(addr));
        tableReg->setItem(row, 1, new QTableWidgetItem(name));

        updateRegisters();
    }
}

void MainWindow::delRow()
{
    int currentRow = tableReg->currentRow();
    int lastRow = tableReg->rowCount() - 1;
    if (currentRow != -1) { // Проверка, что строка действительно выбрана
        tableReg->removeRow(currentRow);
    }
    else if (lastRow >= 0){
        tableReg->removeRow(lastRow);
    }
    updateRegisters();
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
                    m_dtc->changeToRtu();
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
