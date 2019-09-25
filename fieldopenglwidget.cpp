#include "fieldopenglwidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QDebug>

const QString FieldOpenGLWidget::firstBgImageName = ":/images/field1.png";
const QString FieldOpenGLWidget::secondBgImageName = ":/images/field2.png";
const QString FieldOpenGLWidget::robotImageName = ":/images/robot.png";
ImageNamesMap FieldOpenGLWidget::imageNames = {
    {FieldColumns::blocked, ":/images/blocked.png"},
    {FieldColumns::col_one, ":/images/select1.png"},
    {FieldColumns::col_two, ":/images/select2.png"},
    {FieldColumns::col_three, ":/images/select3.png"},
};


FieldOpenGLWidget::FieldOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent)
{
    this->linePixmaps = LinesImagePixmapMap({
        {FieldColumns::blocked, QPixmap(FieldOpenGLWidget::imageNames[FieldColumns::blocked])},
        {FieldColumns::robot, QPixmap(FieldOpenGLWidget::imageNames[FieldColumns::blocked])},
        {FieldColumns::col_one, QPixmap(FieldOpenGLWidget::imageNames[FieldColumns::col_one])},
        {FieldColumns::col_two, QPixmap(FieldOpenGLWidget::imageNames[FieldColumns::col_two])},
        {FieldColumns::col_three, QPixmap(FieldOpenGLWidget::imageNames[FieldColumns::col_three])},
    });
    this->lineImageRect = this->linePixmaps[FieldColumns::blocked].rect();
    this->robotPix = QPixmap(FieldOpenGLWidget::robotImageName);
}

void FieldOpenGLWidget::setBackground(QString name)
{
    this->bgImage = QPixmap(name);
}

void FieldOpenGLWidget::setLines(FieldLinesData lines)
{
    this->linesData = lines;
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
    painter.drawPixmap(0, 0, scaled);
    this->computeScalesOffsets();
    this->fillLines(painter);
    painter.end();
}

void FieldOpenGLWidget::computeScalesOffsets()
{
    // m-m-magic numbers
    qreal x_scale = qreal(this->rect().width() * 1.0049) / qreal(this->bgImage.rect().width()),
          y_scale = qreal(this->rect().height()) / qreal(this->bgImage.rect().height());
    this->scaledLineRect = QRectF(this->lineImageRect);
    this->scaledLineRect.setWidth(qreal(this->lineImageRect.width() * x_scale));
    this->scaledLineRect.setHeight(qreal(this->lineImageRect.height() * y_scale));

    this->firstLineScaledOffset = 0.9 * this->scaledLineRect.height() ;
    this->everyLineScaledOffset = 0.265 * this->scaledLineRect.height();

    this->robotXScaledWidth = 0.07 * this->scaledLineRect.width();
    this->robotXScaledHeight = 2 * this->scaledLineRect.height();
    this->robotXScaledOffset = 0.4 * this->robotXScaledWidth;
    this->robotYScaledOffset = -0.25 * this->robotXScaledHeight;
}

void FieldOpenGLWidget::fillLines(QPainter& painter)
{
    // two step drawing because of bug/strange behavior of painter
    QImage image(this->size(), QImage::Format_ARGB32);
    image.fill(0);
    QPainter new_painter;
    new_painter.begin(&image);
    qreal offset = this->firstLineScaledOffset;
    for (auto line : this->linesData) {
        if (line != FieldColumns::none) {
            QPixmap scaled_line(this->linePixmaps[line].scaled(
                                    int(this->scaledLineRect.width()),
                                    int(this->scaledLineRect.height()),
                                    Qt::IgnoreAspectRatio,
                                    Qt::SmoothTransformation));
            QPointF draw_point(0, offset);
            new_painter.drawPixmap(draw_point, scaled_line);
        }
        if (line == FieldColumns::robot) {
            QPixmap scaled_robot(this->robotPix.scaled(
                                    int(this->robotXScaledWidth),
                                    int(this->robotXScaledHeight),
                                    Qt::IgnoreAspectRatio,
                                    Qt::SmoothTransformation));
            QPointF draw_point(robotXScaledOffset, offset + robotYScaledOffset);
            new_painter.drawPixmap(draw_point, scaled_robot);
        }
        offset += this->scaledLineRect.height() + this->everyLineScaledOffset;
    }
    new_painter.end();

    painter.drawImage(this->rect(), image);
}
