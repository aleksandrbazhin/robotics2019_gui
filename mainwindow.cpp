#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRandomGenerator>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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
        this->ui->comboBox_2_robot->setEnabled(true);
    }
    if (this->availableLines.size() <= 2) {
        this->populateComboBox(this->ui->comboBox_1_robot);
        this->populateComboBox(this->ui->comboBox_2_robot);
    }
    this->ui->field1OpenGLWidget->setLines(this->field1LinesData);
    this->ui->field1OpenGLWidget->update();
    this->ui->field2OpenGLWidget->setLines(this->field2LinesData);
    this->ui->field2OpenGLWidget->update();
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
    this->updateSelections();
}

void MainWindow::onDataReady()
{
    this->ui->sendPushButton->setEnabled(true);
}

void MainWindow::reset()
{
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

    this->updateSelections();
}

void MainWindow::sendData()
{
    QStringList str_data;
    QStringList encoded_data;
    QStringList distorted_data;
    QStringList distorted_data_dec;
    int distorted_pos = QRandomGenerator::global()->bounded(1, 3);
    int distorted_bit = QRandomGenerator::global()->bounded(0, 6);
    int count = 0;
    // BUG adding space at first place
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
    this->ui->textEdit->append("Положение на поле:         " + str_data.join(" "));
    this->ui->textEdit->append("Код Хэмминга:              " + encoded_data.join(" "));
    this->ui->textEdit->append("Код Хэмминга с искажением: " + distorted_data.join(" "));
    this->ui->textEdit->append("В десятичном виде:         " + distorted_data_dec.join(" "));

}
