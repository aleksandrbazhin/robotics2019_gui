#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "linesdata.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->loadFieldPictures();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadFieldPictures()
{
   this->ui->field1OpenGLWidget->setBackground(FieldOpenGLWidget::firstBgImageName);
   this->ui->field2OpenGLWidget->setBackground(FieldOpenGLWidget::secondBgImageName);
   FieldLinesData d1 = {
       FieldColumns::col_three, FieldColumns::robot, FieldColumns::col_one, FieldColumns::none,
       FieldColumns::blocked, FieldColumns::col_two, FieldColumns::none, FieldColumns::blocked
   };

   this->ui->field1OpenGLWidget->setLines(d1);
   this->ui->field1OpenGLWidget->update();

   FieldLinesData d2 = {
       FieldColumns::col_three, FieldColumns::robot, FieldColumns::col_one, FieldColumns::none,
       FieldColumns::blocked, FieldColumns::col_two, FieldColumns::none, FieldColumns::blocked
   };

   this->ui->field2OpenGLWidget->setLines(d2);
   this->ui->field2OpenGLWidget->update();
}
