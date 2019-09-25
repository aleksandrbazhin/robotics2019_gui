#ifndef FIELDOPENGLWIDGET_H
#define FIELDOPENGLWIDGET_H

#include <QOpenGLWidget>
#include "linesdata.h"
#include <unordered_map>

typedef std::unordered_map<FieldColumns, QString> ImageNamesMap;
typedef std::unordered_map<FieldColumns, QPixmap> LinesImagePixmapMap;

class FieldOpenGLWidget: public QOpenGLWidget
{
    Q_OBJECT


public:
    static const QString firstBgImageName;
    static const QString secondBgImageName;
    static const QString robotImageName;
    static ImageNamesMap imageNames;
    FieldOpenGLWidget(QWidget *parent);
    void setBackground(QString name);
    void setLines(FieldLinesData lines);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QBrush background;
    QPixmap bgImage;
    QPixmap robotPix;
    LinesImagePixmapMap linePixmaps;
    FieldLinesData linesData = {
        FieldColumns::none, FieldColumns::none, FieldColumns::none, FieldColumns::none,
        FieldColumns::none, FieldColumns::none, FieldColumns::none, FieldColumns::none
    };
    QRectF lineImageRect;
    QRectF scaledLineRect;
    qreal firstLineScaledOffset;
    qreal everyLineScaledOffset;
    qreal robotXScaledWidth;
    qreal robotXScaledHeight;
    qreal robotXScaledOffset;
    qreal robotYScaledOffset;

    void computeScalesOffsets();
    void fillLines(QPainter& painter);

};

#endif // FIELDOPENGLWIDGET_H
