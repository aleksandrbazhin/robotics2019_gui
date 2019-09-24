#include "fieldopenglwidget.h"

#include <QPainter>
#include <QPaintEvent>

const QString FieldOpenGLWidget::firstBgImageName = ":/images/field1.png";
const QString FieldOpenGLWidget::secondBgImageName = ":/images/field2.png";
const QString FieldOpenGLWidget::robotLineImageName = ":/images/select_robot.png";
const QString FieldOpenGLWidget::firstColLineImageName = ":/images/select1.png";
const QString FieldOpenGLWidget::secondColLineImageName = ":/images/select2.png";
const QString FieldOpenGLWidget::thirdColLineImageName = ":/images/select3.png";

FieldOpenGLWidget::FieldOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent)
{
    this->robotLineImage = QImage(FieldOpenGLWidget::robotLineImageName);
    this->firstColLineImage = QImage(FieldOpenGLWidget::firstColLineImageName);
    this->secondColLineImage = QImage(FieldOpenGLWidget::secondColLineImageName);
    this->thirdColLineImage = QImage(FieldOpenGLWidget::thirdColLineImageName);
}

void FieldOpenGLWidget::setBackground(QString name)
{
    this->bgImage = QImage(name);
}

void FieldOpenGLWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;

    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage(event->rect(), this->bgImage);
//    painter.fillRect(event->rect(), this->background);

    painter.end();
    this->update();
}
