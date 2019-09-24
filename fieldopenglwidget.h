#ifndef FIELDOPENGLWIDGET_H
#define FIELDOPENGLWIDGET_H

#include <QOpenGLWidget>

class FieldOpenGLWidget: public QOpenGLWidget
{
    Q_OBJECT


public:
    static const QString firstBgImageName;
    static const QString secondBgImageName;
    static const QString robotLineImageName;
    static const QString firstColLineImageName;
    static const QString secondColLineImageName;
    static const QString thirdColLineImageName;
    FieldOpenGLWidget(QWidget *parent);
    void setBackground(QString name);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QBrush background;
    QImage bgImage;
    QImage robotLineImage;
    QImage firstColLineImage;
    QImage secondColLineImage;
    QImage thirdColLineImage;

};

#endif // FIELDOPENGLWIDGET_H