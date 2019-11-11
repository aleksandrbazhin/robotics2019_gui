#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRandomGenerator>
#include <QtDebug>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    networkManager(new QNetworkAccessManager(this)),
    settings(QDir::currentPath()+"/settings.ini", QSettings::IniFormat)
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
    connect(this->networkManager, &QNetworkAccessManager::finished,
            this, &MainWindow::onNetworkResponse);

//    this->randomizeDistortion();
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

QVector<int> MainWindow::distortPosition(const QVector<int> &encoded_data)
{
    QVector<int> distorted = encoded_data;
    const QVector<int> checkbits = {0, 1, 3};
    const QVector<int> databits = {2, 4, 5, 6};
    // position checkbit of which to flip
    int distorted_checkbit_position = QRandomGenerator::global()->bounded(1, 4);
    int distorted_checkbit = checkbits[QRandomGenerator::global()->bounded(0, 2)];
    // position databit of which to flip
    int distorted_databit_position = QRandomGenerator::global()->bounded(1, 4);
    while (distorted_checkbit_position == distorted_databit_position) {
        distorted_databit_position = QRandomGenerator::global()->bounded(1, 4);
    }
    int distorted_databit = databits[QRandomGenerator::global()->bounded(0, 3)];
    distorted[distorted_checkbit_position] ^= (1 << distorted_checkbit);
    distorted[distorted_checkbit_position + 4] ^= (1 << distorted_checkbit);
    distorted[distorted_databit_position] ^= (1 << distorted_databit);
    distorted[distorted_databit_position + 4] ^= (1 << distorted_databit);
    return distorted;
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
    this->ui->sendPushButton->setEnabled(false);
    this->isFirstTeamTurn = true;

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

//    this->randomizeDistortion();
    this->updateSelections();
}

QString MainWindow::prepareData()
{
    QStringList str_data;
    QStringList encoded_string_data;
    QVector<int> encoded_data, disrtorted_data;

    for (auto position: this->columnsArray) {
        str_data.append(QString::number(position));
        int encoded = this->encodePosition(position);
        encoded_data.append(encoded);
        encoded_string_data.append(QString::number(encoded, 2).rightJustified(8, '0'));
    }
    QVector<int> distorted_data = this->distortPosition(encoded_data);
    QStringList distorted_string_data;
    QStringList distorted_string_data_dec;

    for (auto distorted: distorted_data) {
        distorted_string_data.append(QString::number(distorted, 2).rightJustified(8, '0'));
        distorted_string_data_dec.append(QString::number(distorted));
    }

    QString data_for_sending = distorted_string_data_dec.join(" ");
    this->ui->textEdit->append("Положение на поле:         " + str_data.join(" "));
    this->ui->textEdit->append("Код Хэмминга:              " + encoded_string_data.join(" "));
    this->ui->textEdit->append("Код Хэмминга с искажением: " + distorted_string_data.join(" "));
    this->ui->textEdit->append("В десятичном виде:         " + data_for_sending);
    return data_for_sending;
}

void MainWindow::paintTeamLabels()
{
    if (this->availableLines.size() == 0) {
        this->ui->field2_status->setVisible(false);
        this->ui->field2_status->setVisible(false);
        return;
    }

    if (this->isFirstTeamTurn) {
        this->ui->field1_status->setVisible(true);
        this->ui->field2_status->setVisible(false);
    } else {
        this->ui->field1_status->setVisible(false);
        this->ui->field2_status->setVisible(true);
    }
}

void MainWindow::sendData()
{
    QString data_string = this->prepareData();

    for (auto ip: this->settings.value("net/ips").toStringList()) {
        QString req_string = "http://" + ip + "/set?data=" + data_string;
        this->networkManager->get(QNetworkRequest(QUrl(req_string)));
        this->ui->textEdit->append("Requesting: " + req_string);
    }
}

void MainWindow::onNetworkResponse(QNetworkReply *reply)
{
    if (reply->error()) {
        this->ui->textEdit->append("ERROR   at: " + reply->request().url().host() + ", error msg: " + reply->errorString());
    } else {
        this->ui->textEdit->append("SUCCESS at: " + reply->request().url().host() + ", response: " + reply->readAll() );
    }
}
