#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDateTime>
#include <QtSerialPort/QSerialPort>
#include <QPushButton>
#include <QSignalMapper>
#include "mydatetime.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
     bool flag = false;

private slots:
     void browseMenu(int page);
     void menuMapper();
     void lightsMapper();
     void uart();
     void enableLights(int light);
     void labelDateTime();
     void serialReceived();
     void loadImage(QPushButton *btn,QString path);
     void console(QString color,QString text);

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QTime time;
    QDate date;
    QTimer *timerSerial; // timer for serial
    QTimer *timerDateTime; // timer for date and time
    QByteArray m_buffer;
    myDateTime dt;
  //  QSignalMapper *mapMenu;

};

#endif // MAINWINDOW_H
