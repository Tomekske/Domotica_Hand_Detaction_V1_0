#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSignalMapper>
#include <QTimer>
#include <QDateTime>
#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include <QByteArray>

#include "mydatetime.h";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), //,Qt::FramelessWindowHint
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//   QMainWindow::showFullScreen();

    timerDateTime = new QTimer(this);
    timerSerial = new QTimer(this);

    dt.setTime(time);

    menuMapper();
    lightsMapper();
    uart();

    connect(timerDateTime,SIGNAL(timeout()),this,SLOT(labelDateTime()));
    timerDateTime->start(200);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::browseMenu(int page)
{
    ui->stacked->setCurrentIndex(page);
}

void MainWindow::menuMapper()
{
    QSignalMapper *mapMenu = new QSignalMapper(this);

    mapMenu->setMapping(ui->btnReturnMenu1,0);
    mapMenu->setMapping(ui->btnReturnMenu2,0);
    mapMenu->setMapping(ui->btnReturnMenu3,0);
    mapMenu->setMapping(ui->btnReturnMenu4,0);
    mapMenu->setMapping(ui->btnGenLight,1);
    mapMenu->setMapping(ui->btnGenTemp,2);
    mapMenu->setMapping(ui->btnGenAlarm,3);
    mapMenu->setMapping(ui->btnGenSettings,4);

    connect(ui->btnReturnMenu1,SIGNAL(clicked(bool)),mapMenu,SLOT(map()));
    connect(ui->btnReturnMenu2,SIGNAL(clicked(bool)),mapMenu,SLOT(map()));
    connect(ui->btnReturnMenu3,SIGNAL(clicked(bool)),mapMenu,SLOT(map()));
    connect(ui->btnReturnMenu4,SIGNAL(clicked(bool)),mapMenu,SLOT(map()));
    connect(ui->btnGenLight,SIGNAL(clicked(bool)),mapMenu,SLOT(map()));
    connect(ui->btnGenTemp,SIGNAL(clicked(bool)),mapMenu,SLOT(map()));
    connect(ui->btnGenAlarm,SIGNAL(clicked(bool)),mapMenu,SLOT(map()));
    connect(ui->btnGenSettings,SIGNAL(clicked(bool)),mapMenu,SLOT(map()));

    connect(mapMenu, SIGNAL(mapped(int)), this, SLOT(browseMenu(int)));
}

void MainWindow::lightsMapper()
{
    QSignalMapper *mapLight = new QSignalMapper(this);
    mapLight->setMapping(ui->btnLightRoom1,1);
    mapLight->setMapping(ui->btnLightRoom2,2);
    mapLight->setMapping(ui->btnLightRoom3,3);
    mapLight->setMapping(ui->btnLightRoom4,4);
    mapLight->setMapping(ui->btnLightRoom5,5);
    mapLight->setMapping(ui->btnLightRoom6,6);
    mapLight->setMapping(ui->btnLightRoom7,7);
    mapLight->setMapping(ui->btnLightRoom8,8);

    connect(ui->btnLightRoom1,SIGNAL(clicked(bool)),mapLight,SLOT(map()));
    connect(ui->btnLightRoom2,SIGNAL(clicked(bool)),mapLight,SLOT(map()));
    connect(ui->btnLightRoom3,SIGNAL(clicked(bool)),mapLight,SLOT(map()));
    connect(ui->btnLightRoom4,SIGNAL(clicked(bool)),mapLight,SLOT(map()));
    connect(ui->btnLightRoom5,SIGNAL(clicked(bool)),mapLight,SLOT(map()));
    connect(ui->btnLightRoom6,SIGNAL(clicked(bool)),mapLight,SLOT(map()));
    connect(ui->btnLightRoom7,SIGNAL(clicked(bool)),mapLight,SLOT(map()));
    connect(ui->btnLightRoom8,SIGNAL(clicked(bool)),mapLight,SLOT(map()));

    connect(mapLight, SIGNAL(mapped(int)), this, SLOT(enableLights(int)));
}

void MainWindow::uart()
{
    serial = new QSerialPort(this);
    serial->setPortName("COM8");
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial-> setParity(QSerialPort::NoParity);
    serial-> setStopBits(QSerialPort::OneStop);
    serial-> setFlowControl(QSerialPort::NoFlowControl);

    if(serial->open(QIODevice::ReadWrite))
    {
        console("lime","ok");
        connect(timerSerial,SIGNAL(timeout()),this,SLOT(serialReceived()));
        timerSerial->start();

    }

    else
    {
        console("red","ok");

    }

}
void MainWindow::enableLights(int light)
{
    QByteArray rood = "0b1100010";
    QByteArray wit =  "0b1100011";
    static bool r1 = false;
    static bool r2 = false;
    static bool r3 = false;
    static bool r4 = false;
    static bool r5 = false;
    static bool r6 = false;
    static bool r7 = false;
    static bool r8 = false;



    if (light == 1)
    {
        if(r1 == false)
        {
            serial->write("a");
            loadImage(ui->btnLightRoom1,":/Button/Icons/Toggle_ON.png");

            r1 = !r1;
        }

        else
        {
            serial->write("A");
            loadImage(ui->btnLightRoom1,":/Button/Icons/Toggle_OFF.png");
            r1 = !r1;

        }
    }

    else if(light == 2)
    {
        if(r2 == false)
        {
            serial->write("b");
            loadImage(ui->btnLightRoom2,":/Button/Icons/Toggle_ON.png");

            r2 = !r2;
        }

        else
        {
            serial->write("B");
            loadImage(ui->btnLightRoom2,":/Button/Icons/Toggle_OFF.png");

            r2 = !r2;
        }
    }
    else if(light == 3)
    {
        if(r3 == false)
        {
            serial->write("c");
            loadImage(ui->btnLightRoom3,":/Button/Icons/Toggle_ON.png");

            r3 = !r3;
        }

        else
        {
            serial->write("C");
            loadImage(ui->btnLightRoom3,":/Button/Icons/Toggle_OFF.png");

            r3 = !r3;
        }
    }

    else if(light == 4)
    {
        if(r4 == false)
        {
            serial->write("d");
            loadImage(ui->btnLightRoom4,":/Button/Icons/Toggle_ON.png");

            r4 = !r4;
        }

        else
        {
            serial->write("D");
            loadImage(ui->btnLightRoom4,":/Button/Icons/Toggle_OFF.png");

            r4 = !r4;
        }
    }

    else if(light == 5)
    {
        if(r5 == false)
        {
            serial->write("e");
            loadImage(ui->btnLightRoom5,":/Button/Icons/Toggle_ON.png");

            r5 = !r5;
        }

        else
        {
            serial->write("E");
            loadImage(ui->btnLightRoom5,":/Button/Icons/Toggle_OFF.png");

            r5 = !r5;
        }
    }

    else if(light == 6)
    {
        if(r6 == false)
        {
            serial->write("f");
            loadImage(ui->btnLightRoom6,":/Button/Icons/Toggle_ON.png");

            r6 = !r6;
        }

        else
        {
            serial->write("F");
            loadImage(ui->btnLightRoom6,":/Button/Icons/Toggle_OFF.png");

            r6 = !r6;
        }
    }

    else if(light == 7)
    {
        if(r7 == false)
        {
            serial->write("g");
            loadImage(ui->btnLightRoom7,":/Button/Icons/Toggle_ON.png");

            r7 = !r7;
        }

        else
        {
            serial->write("G");
            loadImage(ui->btnLightRoom7,":/Button/Icons/Toggle_OFF.png");

            r7 = !r7;
        }
    }
    else if(light == 8)
    {
        if(r8 == false)
        {
            serial->write("h");
            loadImage(ui->btnLightRoom8,":/Button/Icons/Toggle_ON.png");

            r8 = !r8;
        }

        else
        {
            serial->write("H");
            loadImage(ui->btnLightRoom8,":/Button/Icons/Toggle_OFF.png");
            r8 = !r8;
        }
    }
}

void MainWindow::labelDateTime()
{
    ui->lblTime->setText(dt.get24Hours());
    ui->lblDate->setText(dt.getEuropeanDate());
}

void MainWindow::serialReceived()
{
/* byte per byte uitlezen */
 //   QByteArray ba;
    QTime time = QTime::currentTime();
    QString textTime = time.toString("hh:mm:ss");
  //  qDebug() << "1";


    if(serial->bytesAvailable())
    {
  //      qDebug() << "2";

        m_buffer.append(serial->readAll());
        //volledig binnen?
        if(m_buffer.contains("\r"))
        {
 //           qDebug() << "3";

            int index = m_buffer.indexOf("\r");
            QByteArray ba = m_buffer.left(index);
            m_buffer = m_buffer.mid(index+1);

             if(ba == "ok")
                 qDebug() << "4";
             else
                 qDebug() << ba;

        }

        else
        {
                qDebug() << m_buffer;
        }
    }
}

void MainWindow::loadImage(QPushButton *btn,QString path)
{
    QString pathRelative = QString("border-image: url(%1)").arg(path);
    btn->setStyleSheet(pathRelative);

}

void MainWindow::console(QString color, QString text)
{
    QString txtConsole = QString("%1 ---- <font color=%2> %3 </font>").arg(dt.get24Hours(),color,text);
    ui->txtConsole->setText(txtConsole);
}
