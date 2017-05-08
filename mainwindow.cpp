#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSignalMapper>
#include <QTimer>
#include <QDateTime>
#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include <QByteArray>
#include <QSerialPortInfo>
#include <QPushButton>
#include <QSettings>
#include "mydatetime.h";

#define GOOD        "lime"
#define BAD         "red"
#define WARNING     "orange"
#define HIGH        1
#define LOW         0
#define ON          "ON"
#define OFF         "OFF"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QMainWindow::showFullScreen();

    dateTimeMapper();
    menuMapper();
    loadSettings();
    uart();
    lightsMapper();
    tempMapper();
    alarmMapper();
    settingsmapper();
}

MainWindow::~MainWindow()
{
    delete ui;
}


//--------------------------------------------------------------------------------------------------------------//
// 											   Menu				           				        				//
//--------------------------------------------------------------------------------------------------------------//


/**
  * @brief  Function which maps control buttons of the GUI
  * @param  None
  * @retval None
  */
void MainWindow::menuMapper()
{
    QSignalMapper *mapMenu = new QSignalMapper(this);

    //Buttons to return to main menu
    mapMenu->setMapping(ui->btnReturnMenu1,0);
    mapMenu->setMapping(ui->btnReturnMenu2,0);
    mapMenu->setMapping(ui->btnReturnMenu3,0);
    mapMenu->setMapping(ui->btnReturnMenu4,0);

    //Buttons to browse to different pages
    mapMenu->setMapping(ui->btnGenSettings,1);
    mapMenu->setMapping(ui->btnGenLight,2);
    mapMenu->setMapping(ui->btnGenAlarm,3);
    mapMenu->setMapping(ui->btnGenTemp,4);

    //Connect return buttons to signalmapper
    connect(ui->btnReturnMenu1,SIGNAL(clicked(bool)),mapMenu,SLOT(map()));
    connect(ui->btnReturnMenu2,SIGNAL(clicked(bool)),mapMenu,SLOT(map()));
    connect(ui->btnReturnMenu3,SIGNAL(clicked(bool)),mapMenu,SLOT(map()));
    connect(ui->btnReturnMenu4,SIGNAL(clicked(bool)),mapMenu,SLOT(map()));

    //Connect browse buttons to signalmapper
    connect(ui->btnGenLight,SIGNAL(clicked(bool)),mapMenu,SLOT(map()));
    connect(ui->btnGenTemp,SIGNAL(clicked(bool)),mapMenu,SLOT(map()));
    connect(ui->btnGenAlarm,SIGNAL(clicked(bool)),mapMenu,SLOT(map()));
    connect(ui->btnGenSettings,SIGNAL(clicked(bool)),mapMenu,SLOT(map()));

    //Connect indexes to browseMenu()
    connect(mapMenu, SIGNAL(mapped(int)), this, SLOT(browseMenu(int)));
}

/**
  * @brief  Function to change current page in stacked widget
  * @param  page: enter page index of stacked widget
  * @retval None
  */
void MainWindow::browseMenu(int page)
{
    ui->stacked->setCurrentIndex(page);
}



//--------------------------------------------------------------------------------------------------------------//
// 											     UART    		           				        				//
//--------------------------------------------------------------------------------------------------------------//



/**
  * @brief  Function which setups UART connectection and automatically obaints the COM port
  * @param  None
  * @retval None
  */
void MainWindow::uart()
{
    timerSerial = new QTimer(this);

    // make sure only comport of the atmega168 is getting obaint
    foreach(const QSerialPortInfo &portInfo,QSerialPortInfo::availablePorts())
    {
        if(portInfo.hasVendorIdentifier() && portInfo.hasProductIdentifier())
        {
            if(portInfo.vendorIdentifier() == vendorID)
            {
                if(portInfo.productIdentifier() == productID)
                {
                    comPort = portInfo.portName();
                    serialFlag = !serialFlag;
                }
            }
        }
    }

    //If comport if found we're going to setup the uart connection
    if(serialFlag == true)
    {

        serial = new QSerialPort(this);
        serial->setPortName(comPort);
        serial->setBaudRate(QSerialPort::Baud9600);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);

        //Check if serialport can be openned
        if(serial->open(QSerialPort::ReadWrite))
            serialConnection = !serialConnection;

        //if connection is established we're going to start serial timer
        if(serialConnection == true)
        {
            console("lime","Serialport succesfully connected to " + comPort + " !");
            connect(timerSerial,SIGNAL(timeout()),this,SLOT(serialReceived()));
            statusLabel(ui->lblSerial,ON);
            timerSerial->start();
        }

        else
        {
            console("red","Unable to connect to serialport !");
            statusLabel(ui->lblSerial,OFF);
        }
    }

    //If COM port isnt found, connection cannot be made
    else
    {
        console("red","Unable to connect to serialport !");
        ui->lblTemp->setStyleSheet("border-image:  url(:/Button/Icons/noSignal.png);");
    }
}

/**
  * @brief  Function which appends received bytes over serial to a QByteArray
  * @param  None
  * @retval None
  */
void MainWindow::serialReceived()
{
    //Read byte per byte
    if(serial->bytesAvailable())
    {
        m_buffer.append(serial->readAll()); //appends every byte to a QByteArray

        //check if last byte contains '\r' which indicates the whole string is sent
        if(m_buffer.contains("\r"))
        {
            int index = m_buffer.indexOf("\r"); //get index of '\r' in QByteArray
            QByteArray ba = m_buffer.left(index); //copy whole QByteArray in a new QByteArray without the '\r'
            m_buffer = m_buffer.mid(index+1); //make sure m_buffer is empty

            //Check if received string contains the word temp which indicates there is a new temp update
            if(ba.contains("temp"))
            {
                int tIndex =  ba.indexOf("_"); //Locate '_' postion to filter out the temperature value
                m_temp = ba.mid(tIndex + 1);
                ui->lblTemp->setText(setTemp()); //Update label value with new temperature
                autoModeFan(); //Enable / disable autofan mode
            }

            else if(ba.contains("AutoFanOn") && fanStarted == true)
            {
                fanStarted = false;
            }

            //Check if received string contains the word alarm which indicates the alarm got triggerd
            else if(ba.contains("Alarm_Triggerd"))
            {
                console(WARNING,"Alarm TRIGGERD!"); //print time when alarm got triggerd
            }
        }
    }
}

/**
  * @brief  Function to parse a string which is used to send over UART
  * @param  objec: chose which object you want to control e.g "led","fan", etc
  * @param  status: chose if the object is high or low
  * @param  pin: chose which pin you want to control
  * @retval parsed string
  */
QByteArray MainWindow::serialString(QByteArray object, int status, int pin)
{
    QByteArray str = object  + "_" + QByteArray::number(status) + "_" + QByteArray::number(pin) + "\r";
    return str;
}



//--------------------------------------------------------------------------------------------------------------//
// 											   Lights			           				        				//
//--------------------------------------------------------------------------------------------------------------//



/**
  * @brief  Function which maps buttons to control the lights
  * @param  None
  * @retval None
  */
void MainWindow::lightsMapper()
{
    connect(ui->btnLightRoom0,SIGNAL(clicked(bool)),this,SLOT(enableLights()));
    connect(ui->btnLightRoom1,SIGNAL(clicked(bool)),this,SLOT(enableLights()));
    connect(ui->btnLightRoom2,SIGNAL(clicked(bool)),this,SLOT(enableLights()));
    connect(ui->btnLightRoom3,SIGNAL(clicked(bool)),this,SLOT(enableLights()));
    connect(ui->btnLightRoom4,SIGNAL(clicked(bool)),this,SLOT(enableLights()));
    connect(ui->btnLightRoom5,SIGNAL(clicked(bool)),this,SLOT(enableLights()));
    connect(ui->btnLightRoom6,SIGNAL(clicked(bool)),this,SLOT(enableLights()));
    connect(ui->btnLightRoom7,SIGNAL(clicked(bool)),this,SLOT(enableLights()));

    ui->btnLightRoom0->setProperty("intval",0);
    ui->btnLightRoom1->setProperty("intval",1);
    ui->btnLightRoom2->setProperty("intval",2);
    ui->btnLightRoom3->setProperty("intval",3);
    ui->btnLightRoom4->setProperty("intval",4);
    ui->btnLightRoom5->setProperty("intval",5);
    ui->btnLightRoom6->setProperty("intval",6);
    ui->btnLightRoom7->setProperty("intval",7);
}

/**
  * @brief  Function which hold the logic to control the lights
  * @param  None
  * @retval None
  */
void MainWindow::enableLights()
{
   QObject* obj = sender(); //get object from sented signal
   QPushButton* btn = qobject_cast<QPushButton*>(obj); //Cast object to a QPushbutton object
   QByteArray str = "";
   bool ok;
   int lightNumber = btn->property("intval").toInt(&ok);

   //if serial connection is open the following code will be executed
   if(serialConnection == true)
   {
        //The if-else statement toggles the button
        if(btn->isChecked() == true) //Button is on
        {
            str = serialString("led",HIGH,lightNumber);
            loadImage(btn,ON);
            console(GOOD,"Lights in Room " + QByteArray::number(lightNumber + 1) + " turned ON");
        }
        else //Button is off
        {
           str = serialString("led",LOW,lightNumber);
           loadImage(btn,OFF);
           console(GOOD,"Lights in Room " + QByteArray::number(lightNumber + 1) + " turned OFF");
        }
        serial->write(str);
   }

   //if serial connection is closed the following code will be executed, this prevents the application to crash
   else
   {
       console(BAD,"Unable to turn lights on in room " + QString::number(lightNumber + 1) + " ON!");
   }
}



//--------------------------------------------------------------------------------------------------------------//
// 											   Temperature		           				        				//
//--------------------------------------------------------------------------------------------------------------//



/**
  * @brief  Function which maps button to control the fan
  * @param  None
  * @retval None
  */
void MainWindow::tempMapper()
{
    connect(ui->btnTemp,SIGNAL(clicked(bool)),this,SLOT(enableFan()));
    connect(ui->radioTempC,SIGNAL(clicked(bool)),this,SLOT(cTemp()));
    connect(ui->dialFan,SIGNAL(valueChanged(int)),this,SLOT(setAutoFanTemp(int)));
    connect(ui->radioTempF,SIGNAL(clicked(bool)),this,SLOT(fTemp()));

    ui->btnTemp->setProperty("intval",12);

    if(sTemp == "Celsius")
        ui->lblDialTemp->setText(QString::number( ui->dialFan->value()) + "ºC");
    else
        ui->lblDialTemp->setText(QString::number(convertToF( ui->dialFan->value())) + "ºF");
}

/**
  * @brief  Function which hold the logic to control the fans
  * @param  None
  * @retval None
  */
void MainWindow::enableFan()
{
    QObject* obj = sender(); //get object from sented signal
    QPushButton* btn = qobject_cast<QPushButton*>(obj); //Cast object to a QPushbutton object
    QByteArray str = "";
    bool ok;
    int fanNumber = btn->property("intval").toInt(&ok);

    //if serial connection is open the following code will be executed
    if(serialConnection == true)
    {
        //The if-else statement toggles the button
        if(btn->isChecked() == true) //Button is on
        {
            str = serialString("fan",HIGH,fanNumber);
            loadImage(btn,ON);
            statusLabel(ui->lblFan,ON);
            console(GOOD,"Fan turned ON");
        }

        //Button is off
        else
        {
            str = serialString("fan",LOW,fanNumber);
            loadImage(btn,OFF);
            statusLabel(ui->lblFan,OFF);
            console(GOOD,"Fan turned OFF");
            ui->checkAutoFan->setChecked(false);
            startFan = false;
        }
        serial->write(str); //Send string to uC
    }

    //if serial connection is closed the following code will be executed, this prevents the application to crash
    else
    {
        console(BAD,"Unable to turn fan ON!");
    }
}

/**
  * @brief  Function which controls if temperature is displayed as *C or *F
  * @param  None
  * @retval returns QByteArray with converted temp and degree symbols
  */
QByteArray MainWindow::setTemp()
{
    QByteArray nTemp;

    if(sTemp == "Celsius")
        nTemp = m_temp + "ºC";
    else
        nTemp = QByteArray::number(convertToF(m_temp.toInt())) + "ºF";
    return nTemp;
}

/**
  * @brief  Function to display temperature in *C on a label
  * @param  None
  * @retval None
  */
void MainWindow::cTemp()
{
    sTemp = "Celsius";
    if(serialConnection == true)
        ui->lblTemp->setText(setTemp());
    else
        ui->lblTemp->setStyleSheet("border-image:  url(:/Button/Icons/noSignal.png);");
}

/**
  * @brief  Function to display temperature in *F on a label
  * @param  None
  * @retval None
  */
void MainWindow::fTemp()
{
    sTemp = "Fahrenheit";
    if(serialConnection == true)
        ui->lblTemp->setText(setTemp());
    else
        ui->lblTemp->setStyleSheet("border-image:  url(:/Button/Icons/noSignal.png);");
}

/**
  * @brief  Function to set temperature to a specific value as reference when the fan is suposed to toggle
  * @param  value: value of the dial
  * @retval None
  */
void MainWindow::setAutoFanTemp(int value)
{
    dialValue = value;

    if(sTemp == "Celsius")
        ui->lblDialTemp->setText(QString::number(value) + "ºC");
    else
        ui->lblDialTemp->setText(QString::number(convertToF(value)) + "ºF");
}

/**
  * @brief  Function which convert *C to *F
  * @param  celsius: pass in a integer celsius value
  * @retval Returns temperature in *F
  */
int MainWindow::convertToF(int celsius)
{
    return (celsius*(1.8))+32;
}

/**
  * @brief  Function to enable/disable auto mode to control the fan
  * @param  None
  * @retval None
  */
void MainWindow::on_checkAutoFan_clicked()
{
    QByteArray str;
    if(serialConnection == true)
    {
        if(ui->checkAutoFan->isChecked() == true)
        {
            startFan = true;
            ui->btnTemp->setChecked(true);
            loadImage(ui->btnTemp,ON);
            statusLabel(ui->lblFan,ON);
        }

        else
        {
             ui->btnTemp->setEnabled(true);
             ui->btnTemp->setChecked(false);
             loadImage(ui->btnTemp,OFF);
             str = serialString("fanAuto",LOW,12);
             serial->write(str);
             startFan = false;
             statusLabel(ui->lblFan,OFF);
        }
    }

    else
    {
        console(BAD,"Unable to turn fan ON!");
        ui->checkAutoFan->setCheckable(false);
    }
}

/**
  * @brief  Function with logic when the fan is supposed to turn on, based on Hysteresis theorem
  * @param  None
  * @retval None
  */
void MainWindow::autoModeFan()
{
    QByteArray str;
    int nTemp =  m_temp.toInt();

    if((nTemp >= (dialValue + 1))  && startFan == true)
    {
        str = serialString("fan",HIGH,12);

        if(fanStarted == false)
        {
            console(WARNING,"Fan turned ON");
            fanStarted = !fanStarted;
        }

        fanStarted = true;
    }
    else if(nTemp <= (dialValue - 1) )
    {
        str = serialString("fan",LOW,12);
        if(fanStarted == true)
        {
            console(WARNING,"Fan turned OFF");
            fanStarted = !fanStarted;
        }
        fanStarted = false;
    }
    serial->write(str);
}



//--------------------------------------------------------------------------------------------------------------//
// 											     Alarm      	           				        				//
//--------------------------------------------------------------------------------------------------------------//



/**
  * @brief  Function which maps alarm button
  * @param  None
  * @retval None
  */
void MainWindow::alarmMapper()
{
    connect(ui->btnAlarm,SIGNAL(clicked(bool)),this,SLOT(alarm()));
}

/**
  * @brief  Function which hold the logic to control the alarm
  * @param  None
  * @retval None
  */
void MainWindow::alarm()
{
    QByteArray str = "";
    if(serialConnection == true)
    {
        //The if-else statement toggles the button
        if(ui->btnAlarm->isChecked() == true) //Button is on
        {
            str = serialString("alarm",HIGH,0);
            loadImage(ui->btnAlarm,ON);
            statusLabel(ui->lblAlarm,ON);

            console(GOOD,"Alarm turned ON");
        }
        else //Button is off
        {
           str = serialString("alarm",LOW,0);
           loadImage(ui->btnAlarm,OFF);
           console(GOOD,"Alarm turned OFF");
           statusLabel(ui->lblAlarm,OFF);
        }
        serial->write(str);
    }

    else
    {
        console(BAD,"Unable to turn alarm ON!");
    }
}



//--------------------------------------------------------------------------------------------------------------//
// 											   Date and Time	           				        				//
//--------------------------------------------------------------------------------------------------------------//



/**
  * @brief  Function to map date and time slots
  * @param  None
  * @retval None
  */
void MainWindow::dateTimeMapper()
{
    timerDateTime = new QTimer(this);
    dt = new myDateTime(time,date);

    connect(ui->radioTimeAM,SIGNAL(clicked(bool)),this,SLOT(amTime()));
    connect(ui->radioTimeEU,SIGNAL(clicked(bool)),this,SLOT(euTime()));
    connect(ui->radioDateAM,SIGNAL(clicked(bool)),this,SLOT(amDate()));
    connect(ui->radioDateEU,SIGNAL(clicked(bool)),this,SLOT(euDate()));
    connect(timerDateTime,SIGNAL(timeout()),this,SLOT(labelDateTime()));

    timerDateTime->start();
}

/**
  * @brief  Function which sets time and date format to labels
  * @param  None
  * @retval None
  */
void MainWindow::labelDateTime()
{
    if(sTime == "eu")
        ui->lblTime->setText(dt->time24Hours());
    else
        ui->lblTime->setText(dt->time12Hours());

    if(sDate == "eu")
        ui->lblDate->setText(dt->europeanDate());
    else
        ui->lblDate->setText(dt->americanDate());
}

/**
  * @brief  Function to set 12hours clock with a radiobutton
  * @param  None
  * @retval None
  */
void MainWindow::amTime()
{
    sTime = "am";
}

/**
  * @brief  Function to set 24hours clock with a radiobutton
  * @param  None
  * @retval None
  */
void MainWindow::euTime()
{
    sTime = "eu";
}

/**
  * @brief  Function to set date to Day/Month/Year with a radiobutton
  * @param  None
  * @retval None
  */
void MainWindow::amDate()
{
    sDate = "am";
}

/**
  * @brief  Function to set date to Month/Day/Year with a radiobutton
  * @param  None
  * @retval None
  */
void MainWindow::euDate()
{
    sDate = "eu";
}



//--------------------------------------------------------------------------------------------------------------//
// 											     Settings      	           				        				//
//--------------------------------------------------------------------------------------------------------------//



/**
  * @brief  Function to map settings connects
  * @param  None
  * @retval None
  */
void MainWindow::settingsmapper()
{
    connect(ui->btnSave,SIGNAL(clicked(bool)),this,SLOT(saveSettings()));
}

/**
  * @brief  Function to save user settings
  * @param  None
  * @retval None
  */
void MainWindow::saveSettings()
{
    QSettings settings("TM","Domotica"); //No fucking idea why you need those parameters
    settings.beginGroup("settings");
    settings.setValue("Time",sTime); //set setings
    settings.setValue("Date",sDate);
    settings.setValue("Temperature",sTemp);
    settings.endGroup();
    console(GOOD,"Settings succesfully saved!");
}

/**
  * @brief  Function to load user settings
  * @param  None
  * @retval None
  */
void MainWindow::loadSettings()
{
    QSettings settings("TM","Domotica");
    settings.beginGroup("settings");
    sTime = settings.value("Time").toString(); //get settings
    sDate = settings.value("Date").toString();
    sTemp = settings.value("Temperature").toString();
    settings.endGroup();

    if(sTime == "eu")
        ui->radioTimeEU->setChecked(true);
    else
        ui->radioTimeAM->setChecked(true);

    if(sDate == "eu")
        ui->radioDateEU->setChecked(true);
    else
        ui->radioDateAM->setChecked(true);

    if(sTemp == "Celsius")
        ui->radioTempC->setChecked(true);
    else
        ui->radioTempF->setChecked(true);

    console(GOOD,"Settings succesfully loaded!");
}



//--------------------------------------------------------------------------------------------------------------//
// 											     Style      	           				        				//
//--------------------------------------------------------------------------------------------------------------//



/**
  * @brief  Function which loads the correct image to the button which is toggled
  * @param  btn:  Button which is getting toggled
  * @param  status: state of button (ON/OFF)
  * @retval None
  */
void MainWindow::loadImage(QPushButton *btn,QString status)
{
    QString pathRelative;

    if(status == "ON")
        pathRelative = "border-image: url(:/Button/Icons/Toggle_ON.png)";
    else
        pathRelative = "border-image: url(:/Button/Icons/Toggle_OFF.png)";

    btn->setStyleSheet(pathRelative);
}

/**
  * @brief  Function to print text the the textbox
  * @param  color: sets the text color, every color has a feedback meaning
  * @param  text: sets the text to display
  * @retval None
  */
void MainWindow::console(QString color, QString text)
{
    QString setTime = "";
    if(sTime == "eu")
        setTime = dt->time24Hours();
    else
        setTime = dt->time12Hours();

    QString txtConsole = QString("%1 ---- <font color=%2> %3 </font>").arg(setTime,color,text);
    ui->txtConsole->append(txtConsole);
}

/**
  * @brief  Function to display updated labels
  * @param  lbl:  label which is getting updated
  * @param  status: state of label (ON/OFF)
  * @retval None
  */
void MainWindow::statusLabel(QLabel *lbl, QString status)
{
    if(status == "ON")
        lbl->setStyleSheet("background-color: #00CC00; color: white;");
    else
        lbl->setStyleSheet("background-color: #f5f5f5; color: black;");
}
