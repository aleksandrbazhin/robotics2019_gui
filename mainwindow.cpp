#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "linesdata.h"
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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadFieldPictures()
{
   this->ui->field1OpenGLWidget->setBackground(FieldOpenGLWidget::firstBgImageName);
   this->ui->field2OpenGLWidget->setBackground(FieldOpenGLWidget::secondBgImageName);
//   FieldLinesData d1 = {
//       FieldColumns::col_three, FieldColumns::robot, FieldColumns::col_one, FieldColumns::blocked,
//       FieldColumns::blocked, FieldColumns::col_two, FieldColumns::blocked, FieldColumns::blocked
//   };

//   this->ui->field1OpenGLWidget->setLines(d1);
//   this->ui->field1OpenGLWidget->update();

//   FieldLinesData d2 = {
//       FieldColumns::blocked, FieldColumns::blocked, FieldColumns::blocked, FieldColumns::robot,
//       FieldColumns::col_three, FieldColumns::blocked, FieldColumns::col_two, FieldColumns::col_one
//   };

//   this->ui->field2OpenGLWidget->setLines(d2);
    //   this->ui->field2OpenGLWidget->update();
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

void MainWindow::onComboSelect(int line_number, int field, int column)
{
    qDebug() << line_number;
    this->availableLines.remove(line_number);
    qDebug() << this->availableLines;
    this->updateSelections();

}
