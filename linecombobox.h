#ifndef LINECOMBOBOX_H
#define LINECOMBOBOX_H

#include <QObject>
#include <QComboBox>
#include <QDebug>

class LineComboBox: public QComboBox
{
    Q_OBJECT
public:
    LineComboBox(QWidget *parent): QComboBox (parent) {
        connect(this, SIGNAL(currentIndexChanged(QString)),
                this, SLOT(onSelect(QString)));
    }
    void setPosition(int field, int column) {
        this->field = field;
        this->column = column;
    }
    virtual ~LineComboBox(){}

signals:
    void selected(int value, int field, int column);

private:
    int field;
    int column;

private slots:
    void onSelect(const QString& str) {
        if (str.size() == 0) return;
        bool ok;
        int line_number = QString(str.back()).toInt(&ok);
        if (ok) {
            this->setEnabled(false);
            emit selected(line_number, this->field, this->column);
        }
    }
};

#endif // LINECOMBOBOX_H


