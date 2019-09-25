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
    QVector<int> columnsArray = {0, 0, 0, 0, 0, 0, 0, 0};
    FieldLinesData field1LinesData = defaultLinesData;
    FieldLinesData field2LinesData = defaultLinesData;
    void updateSelections();
    void populateComboBox(LineComboBox* combo);
    FieldColumns index2Field(int i);

private slots:
    void onComboSelect(int, int, int);
    void onDataReady();
    void reset();

};

#endif // MAINWINDOW_H
