#include "mydatetime.h"
#include <QDateTime>
#include <QString>
#include <QDebug>

/**
  * @brief  Constructor which sets the current time and date
  * @param  time: current time
  * @param  date: current date
  * @retval None
  */
myDateTime::myDateTime(QTime time,QDate date)
{
    setTime(time);
    setDate(date);
}

/**
  * @brief  setter function to pass current time to a member variable
  * @param  time:  current time
  * @retval None
  */
void myDateTime::setTime(QTime time)
{
    mTime = time;
}

/**
  * @brief  getter function to return current time
  * @param  None
  * @retval current time
  */
QTime myDateTime::getTime()
{
    return mTime;
}

/**
  * @brief  setter function to pass current date to a member variable
  * @param  time:  current date
  * @retval None
  */
void myDateTime::setDate(QDate date)
{
    mDate = date;
}

/**
  * @brief  getter function to return current date
  * @param  None
  * @retval current date
  */
QDate myDateTime::getDate()
{
    return mDate;
}

/**
  * @brief  getter function to return current time as a string in 24hours format
  * @param  None
  * @retval current time string in 24hours format
  */
QString myDateTime::time24Hours()
{
   mTime = QTime::currentTime();
   return  mTimeString = mTime.toString("hh:mm:ss");
}

/**
  * @brief  getter function to return current time as a string in 12hours format
  * @param  None
  * @retval current time string in 12hours format
  */
QString myDateTime::time12Hours()
{
    mTime = QTime::currentTime();
    return  mTimeString = mTime.toString("hh:mm:ss ap");
}

/**
  * @brief  getter function to return current date as a string in a none-retarted format
  * @param  None
  * @retval current date string in a none-retarted format
  */
QString myDateTime::europeanDate()
{
    mDate = QDate::currentDate();
    return mDateString = mDate.toString("dd MMMM yyyy");
}

/**
  * @brief  getter function to return current date as a string in a retarted format
  * @param  None
  * @retval current date string in a retarted format
  */
QString myDateTime::americanDate()
{
    mDate = QDate::currentDate();
    return mDateString = mDate.toString("MMMM dd yyyy");
}


