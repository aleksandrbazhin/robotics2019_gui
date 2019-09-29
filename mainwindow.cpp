#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRandomGenerator>
#include <QtDebug>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    networkManager(new QNetworkAccessManager(this)),
    settings(QDir::currentPath()+"/settings.ini", QSettings::IniFormat),
    serial(new QSerialPort(this))
{
    ui->setupUi(this);
    this->loadFieldPictures();
    this->updateSelections();
    this->initCombobox(this->ui->comboBox_1_robot, 1, 0);
    this->initCombobox(this->ui->comboBox_1_1, 1, 1);
    this->initCombobox(this->ui->comboBox_1_2, 1, 2);
    this->initCombobox(this->ui->comboBox_1_3, 1, 3);
    this->initCombobox(this->ui->comboBox_2_robot, 2, 0);
    this->initCombobox(this->ui->comboBox_2_1, 2, 1);
    this->initCombobox(this->ui->comboBox_2_2, 2, 2);
    this->initCombobox(this->ui->comboBox_2_3, 2, 3);
    connect(this->ui->resetPushButton, SIGNAL(clicked()), this, SLOT(reset()));
    connect(this->ui->sendPushButton, SIGNAL(clicked()), this, SLOT(sendData()));

    this->randomizeDistortion();

    if (this->settings.value("gen/connection")  == "net") {
        connect(this->networkManager, &QNetworkAccessManager::finished,
                this, &MainWindow::onNetworkResponse);
    } else if (this->settings.value("gen/connection")  == "com") {
        connect(this->serial, &QSerialPort::readyRead,
                this, &MainWindow::onSerialReturn);
        connect(this->serial, &QSerialPort::errorOccurred,
                this, &MainWindow::onSerialError);
    } else {
        this->ui->textEdit->append("ERROR: Connection type not specified in setting.ini");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadFieldPictures()
{
   this->ui->field1OpenGLWidget->setBackground(FieldOpenGLWidget::firstBgImageName);
   this->ui->field2OpenGLWidget->setBackground(FieldOpenGLWidget::secondBgImageName);
}

void MainWindow::initCombobox(LineComboBox* combo, int field, int column)
{
    combo -> setPosition(field, column);
    connect(combo, SIGNAL(selected(int, int, int)),
            this, SLOT(onComboSelect(int, int, int)));
}

void MainWindow::updateSelections()
{
    this->populateComboBox(this->ui->comboBox_1_1);
    this->populateComboBox(this->ui->comboBox_1_2);
    this->populateComboBox(this->ui->comboBox_1_3);
    this->populateComboBox(this->ui->comboBox_2_1);
    this->populateComboBox(this->ui->comboBox_2_2);
    this->populateComboBox(this->ui->comboBox_2_3);
    if (this->availableLines.size() == 2) {
        this->ui->comboBox_1_robot->setEnabled(true);
        this->populateComboBox(this->ui->comboBox_1_robot);
    }
    if (this->availableLines.size() == 1) {
        this->ui->comboBox_2_robot->setEnabled(true);
        this->populateComboBox(this->ui->comboBox_2_robot);
    }
    this->ui->field1OpenGLWidget->setLines(this->field1LinesData);
    this->ui->field1OpenGLWidget->update();
    this->ui->field2OpenGLWidget->setLines(this->field2LinesData);
    this->ui->field2OpenGLWidget->update();

    this->paintTeamLabels();
}

void MainWindow::populateComboBox(LineComboBox* combo)
{
    if (combo->isEnabled()) {
        combo->clear();
        combo->addItem("");
        for(auto line: this->availableLines) {
            combo->addItem("Линия " + QString::number(line));
        }
    }
}

// I know, I know
FieldColumns MainWindow::index2Field(int i)
{
    assert(i < 4);
    switch(i) {
        case 0:
            return FieldColumns::robot;
        case 1:
            return FieldColumns::col_one;
        case 2:
            return FieldColumns::col_two;
        case 3:
            return FieldColumns::col_three;
        default:
            return FieldColumns::none;
    }
}

int MainWindow::encodePosition(int position)
{
    assert(position >= 1);
    assert(position <= 8);
    return this->encode_table[position - 1];
}

int MainWindow::distortPosition(int encodedPosition, int distorted_bit)
{
    return encodedPosition ^ (1 << distorted_bit);
}

void MainWindow::onComboSelect(int line_number, int field, int column)
{
    size_t line_index = size_t(line_number - 1);
    int column_index = 0;

    if (field == 1) {
        column_index = column;
        this->field1LinesData[line_index] = this->index2Field(column);
        this->field2LinesData[line_index] = FieldColumns::blocked;
    } else if (field == 2) {
        column_index = column + 4;
        this->field1LinesData[line_index] = FieldColumns::blocked;
        this->field2LinesData[line_index] = this->index2Field(column);
    }
    this->columnsArray[column_index] = line_number;
    this->availableLines.removeOne(line_number);
    if (this->availableLines.size() == 0) {
        this->onDataReady();
    }

    this->isFirstTeamTurn = !isFirstTeamTurn;

    this->updateSelections();
}

void MainWindow::onDataReady()
{
    this->ui->sendPushButton->setEnabled(true);
}

void MainWindow::reset()
{
    this->dataToSend = "";
    this->ui->sendPushButton->setEnabled(false);

    this->ui->comboBox_1_robot->setCurrentText("");
    this->ui->comboBox_2_robot->setCurrentText("");
    this->ui->comboBox_1_1->setCurrentText("");
    this->ui->comboBox_1_1->setEnabled(true);
    this->ui->comboBox_1_2->setCurrentText("");
    this->ui->comboBox_1_2->setEnabled(true);
    this->ui->comboBox_1_3->setCurrentText("");
    this->ui->comboBox_1_3->setEnabled(true);
    this->ui->comboBox_2_1->setCurrentText("");
    this->ui->comboBox_2_1->setEnabled(true);
    this->ui->comboBox_2_2->setCurrentText("");
    this->ui->comboBox_2_2->setEnabled(true);
    this->ui->comboBox_2_3->setCurrentText("");
    this->ui->comboBox_2_3->setEnabled(true);

    this->availableLines = this->defaultAvailableLines;
    this->field1LinesData = defaultLinesData;
    this->field2LinesData = defaultLinesData;
    this->columnsArray = this->defaulColumnsArray;

    this->randomizeDistortion();
    this->updateSelections();
}

QString MainWindow::prepareData()
{
    QStringList str_data;
    QStringList encoded_data;
    QStringList distorted_data;
    QStringList distorted_data_dec;
    int count = 0;

    for (auto position: this->columnsArray) {
        str_data.append(QString::number(position));
        int encoded = this->encodePosition(position);
        int distorted = encoded;
        if (distorted_pos == count || distorted_pos + 4 == count) {
            distorted = this->distortPosition(encoded, distorted_bit);
        }
        encoded_data.append(QString::number(encoded, 2).rightJustified(8, '0'));
        distorted_data.append(QString::number(distorted, 2).rightJustified(8, '0'));
        distorted_data_dec.append(QString::number(distorted));
        count ++ ;
    }
    QString data_for_sending = distorted_data_dec.join(" ");
    this->ui->textEdit->append("Положение на поле:         " + str_data.join(" "));
    this->ui->textEdit->append("Код Хэмминга:              " + encoded_data.join(" "));
    this->ui->textEdit->append("Код Хэмминга с искажением: " + distorted_data.join(" "));
    this->ui->textEdit->append("В десятичном виде:         " + data_for_sending);
    //this->ui->textEdit->verticalScrollBar.
    return data_for_sending;
}

void MainWindow::randomizeDistortion()
{
    this->distorted_pos = QRandomGenerator::global()->bounded(1, 4);
    this->distorted_bit = QRandomGenerator::global()->bounded(0, 7);
}

void MainWindow::paintTeamLabels()
{
    if (this->availableLines.size() == 0) {
        this->ui->field1_status->setText("");
        this->ui->field2_status->setText("");
        return;
    }

    if (this->isFirstTeamTurn) {
        this->ui->field1_status->setText("Ход 1 команды");
        this->ui->field2_status->setText("");
    } else {
        this->ui->field1_status->setText("");
        this->ui->field2_status->setText("Ход 2 команды");
    }
}

bool MainWindow::openSerialPort(QString port_name, int baud_rate)
{
    this->serial->setPortName(port_name);
    this->serial->setBaudRate(baud_rate);
    this->serial->setDataBits(QSerialPort::Data8);
    this->serial->setParity(QSerialPort::NoParity);
    this->serial->setStopBits(QSerialPort::OneStop);
    if (this->serial->open(QIODevice::ReadWrite)) {
        serial->setDataTerminalReady(true);
        return true;
    } else {
        return false;
    }
}


void MainWindow::sendDataByNet()
{
    for (auto ip: this->settings.value("net/ips").toStringList()) {
        QString req_string = "http://" + ip + "/set/" + this->dataToSend;
        this->networkManager->get(QNetworkRequest(QUrl(req_string)));
        this->ui->textEdit->append("Requesting: " + req_string);
    }
}

void MainWindow::sendNextInSerialQueue()
{
    if (this->serial->isOpen()) {
        this->serial->close();
        this->ui->textEdit->append("Port closed");
    }
    if (this->serialPortsQueue.isEmpty()) {
        return;
    }
    QString port_name = this->serialPortsQueue.dequeue();
    int baud_rate = this->settings.value("com/baud_rate").toInt();
    if (this->openSerialPort(port_name, baud_rate)) {
        this->ui->textEdit->append("Port " + port_name + " opened");
//        this->serial->write(this->dataToSend.toLatin1());
        this->serial->write("FUCK!");
    } else {
        this->ui->textEdit->append("ERROR: cannot open port " + port_name);
    }
}

void MainWindow::sendDataBySerial()
{
    for (auto port_name: this->settings.value("com/ports").toStringList()) {
        this->serialPortsQueue.enqueue(port_name);
    }
    this->sendNextInSerialQueue();
}

void MainWindow::sendData()
{
    this->dataToSend = this->prepareData();
    if (this->settings.value("gen/connection")  == "net") {
        this->sendDataByNet();
    } else if (this->settings.value("gen/connection")  == "com") {
        this->sendDataBySerial();
    } else {
        this->ui->textEdit->append("ERROR: Connection type not specified in setting.ini");
    }
}

void MainWindow::onNetworkResponse(QNetworkReply *reply)
{
    if(reply->error())
    {
        this->ui->textEdit->append("ERROR: " + reply->errorString() +
                                   " AT: " + reply->request().url().toString());
    } else if (reply->readAll() == "OK") {
        this->ui->textEdit->append("OK:         " + reply->request().url().toString());
    }
}

void MainWindow::onSerialReturn()
{
    const QByteArray data = serial->readAll();
    QString data_string(data);
    if (data_string.endsWith("\n")) {
        this->ui->textEdit->append("Success: " + tempSerialReceive + data_string);
        this->tempSerialReceive = "";
        this->sendNextInSerialQueue();
    } else {
        this->tempSerialReceive += data_string;
    }
}

void MainWindow::onSerialError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError) {
        if (error == QSerialPort::ResourceError || error == QSerialPort::OpenError) {
            if (this->serial->isOpen())
                this->serial->close();
        }
        this->ui->textEdit->append("ERROR: Serial: " + this->serial->errorString());
        this->sendNextInSerialQueue();
    }
}
