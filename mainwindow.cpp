#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    this->ui->comboBox_1_robot->setEnabled(true);
    this->ui->comboBox_2_robot->setCurrentText("");
    this->ui->comboBox_2_robot->setEnabled(true);
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

    this->availableLines = defaultAvailableLines;
    this->field1LinesData = defaultLinesData;
    this->field2LinesData = defaultLinesData;

    this->updateSelections();
}
