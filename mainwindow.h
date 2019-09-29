#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <linecombobox.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
#include <QTimer>
#include "linesdata.h"
#include <QSerialPort>
#include <QQueue>

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
    void updateSelections();
    void populateComboBox(LineComboBox* combo);
    FieldColumns index2Field(int i);
    int encodePosition(int position);
    int distortPosition(int encodedPosition, int distorted_bit);
    QNetworkAccessManager *networkManager;
    QString prepareData();
    void randomizeDistortion();
    void paintTeamLabels();
    bool openSerialPort(QString port_name, int baud_rate);
    void sendDataByNet();
    void sendDataBySerial();
    void sendNextInSerialQueue();
    QSettings settings;
    QSerialPort *serial = nullptr;
    FieldLinesData field1LinesData = defaultLinesData;
    FieldLinesData field2LinesData = defaultLinesData;
    const QVector<int> defaulColumnsArray = {0, 0, 0, 0, 0, 0, 0, 0};
    const QList<int> defaultAvailableLines = {1, 2, 3, 4, 5, 6, 7, 8};
    QList<int> availableLines = defaultAvailableLines;
    QVector<int> columnsArray = {0, 0, 0, 0, 0, 0, 0, 0};
    const QVector<int> encode_table = {
        0b1101001, 0b0101010, 0b1000011, 0b1001100,
        0b0100101, 0b1100110, 0b0001111, 0b1110000
    };
    int distorted_pos;
    int distorted_bit;
    bool isFirstTeamTurn = true;
    QQueue<QString> serialPortsQueue;
    QString dataToSend = "";
    QString tempSerialReceive = "";

private slots:
    void onComboSelect(int, int, int);
    void onDataReady();
    void reset();
    void sendData();
    void onNetworkResponse(QNetworkReply *reply);
    void onSerialReturn();
    void onSerialError(QSerialPort::SerialPortError error);

};

#endif // MAINWINDOW_H
