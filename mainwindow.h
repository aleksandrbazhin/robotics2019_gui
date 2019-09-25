#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <linecombobox.h>
#include "linesdata.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void loadFieldPictures();
    void initCombobox(LineComboBox* combo, int field, int line);
    const QList<int> defaultAvailableLines = {1, 2, 3, 4, 5, 6, 7, 8};
    QList<int> availableLines = defaultAvailableLines;
    const QVector<int> defaulColumnsArray = {0, 0, 0, 0, 0, 0, 0, 0};
    QVector<int> columnsArray = {0, 0, 0, 0, 0, 0, 0, 0};
    FieldLinesData field1LinesData = defaultLinesData;
    FieldLinesData field2LinesData = defaultLinesData;
    void updateSelections();
    void populateComboBox(LineComboBox* combo);
    FieldColumns index2Field(int i);
    const QVector<int> encode_table = {
        0b1101001, 0b0101010, 0b1000011, 0b1001100,
        0b0100101, 0b1100110, 0b0001111, 0b1110000};
    int encodePosition(int position);

private slots:
    void onComboSelect(int, int, int);
    void onDataReady();
    void reset();
    void sendData();

};

#endif // MAINWINDOW_H
