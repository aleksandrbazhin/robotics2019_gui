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
    this->robotLineImage = QPixmap(FieldOpenGLWidget::robotLineImageName);
    this->firstColLineImage = QPixmap(FieldOpenGLWidget::firstColLineImageName);
    this->secondColLineImage = QPixmap(FieldOpenGLWidget::secondColLineImageName);
    this->thirdColLineImage = QPixmap(FieldOpenGLWidget::thirdColLineImageName);
}

void FieldOpenGLWidget::setBackground(QString name)
{
    this->bgImage = QPixmap(name);
}

void FieldOpenGLWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    QPixmap scaled(this->bgImage.scaled(this->rect().width(),
                                        this->rect().height(),
                                        Qt::IgnoreAspectRatio,
                                        Qt::SmoothTransformation));
    painter.drawPixmap(0,0,scaled);
    this->fillLines(painter);
    painter.end();
    this->update();
}

void FieldOpenGLWidget::fillLines(QPainter &painter)
{

}
