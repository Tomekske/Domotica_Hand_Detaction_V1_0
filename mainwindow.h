#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDateTime>
#include <QtSerialPort/QSerialPort>
#include <QPushButton>
#include <QLabel>
#include <QSignalMapper>
#include "mydatetime.h"

#include <QDebug>
#define QD qDebug() << __FILE__ << __LINE__

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

     //menu
     void browseMenu(int page);
     void menuMapper();

     //uart
     void uart();
     void serialReceived();
     QByteArray serialString(QByteArray object,int status,int pin);

     //lights
     void lightsMapper();
     void enableLights();

     //temperature
     QByteArray setTemp();
     void setAutoFanTemp(int value);
     int convertToF(int celsius);
     void cTemp();
     void fTemp();
     void tempMapper();
     void enableFan();
     void on_checkAutoFan_clicked();
     void autoModeFan();

     //alarm
     void alarmMapper();
     void alarm();

     //date and time
     void dateTimeMapper();
     void labelDateTime();
     void amTime();
     void euTime();
     void amDate();
     void euDate();

     //settings
     void settingsmapper();
     void saveSettings();
     void loadSettings();

     //style
     void loadImage(QPushButton *btn,QString status);
     void console(QString color,QString text);
     void statusLabel(QLabel *lbl,QString status);

private:
    Ui::MainWindow *ui;

    //uart
    QSerialPort *serial;
    QString comPort;
    quint16  vendorID = 6790;
    quint16  productID = 29987;
    QByteArray m_buffer;
    bool serialFlag = false;
    bool serialConnection = false;

    //date and time
    QTime time;
    QDate date;
    QString sTime = "";
    QString sDate = "";

    //timers
    QTimer *timerSerial; // timer for serial
    QTimer *timerDateTime; // timer for date and time
    myDateTime *dt;

    //temperature
    QString sTemp = "";
    QByteArray m_temp = "";
    int dialValue = 0;
    bool startFan = false;
    bool fanStarted = false;
};

#endif // MAINWINDOW_H
