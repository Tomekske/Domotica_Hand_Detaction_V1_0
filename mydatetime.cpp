#include "mydatetime.h"
#include <QDateTime>
#include <QString>
#include <QDebug>
myDateTime::myDateTime()
{

}

void myDateTime::setTime(QTime time)
{
    mTime = time;
}

QTime myDateTime::getTime()
{
    return mTime;
}

void myDateTime::setDate(QDate date)
{
    mDate = date;
}

QDate myDateTime::getDate()
{
    return mDate;
}

QString myDateTime::get24Hours()
{
   mTime = QTime::currentTime();
   return  mTimeString = mTime.toString("hh:mm:ss");
}

QString myDateTime::get12Hours()
{
    mTime = QTime::currentTime();
    return  mTimeString = mTime.toString("hh:mm:ss ap");
}

QString myDateTime::getEuropeanDate()
{
    mDate = QDate::currentDate();
    return mDateString = mDate.toString("dd MMMM yyyy");
}

QString myDateTime::getAmericanDate()
{
    mDate = QDate::currentDate();
    return mDateString = mDate.toString("MMMM dd yyyy");
}


