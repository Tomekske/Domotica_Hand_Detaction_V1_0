#ifndef MYDATETIME_H
#define MYDATETIME_H

#include <QObject>
#include <QDateTime>

class myDateTime
{
public:
    myDateTime(QTime time,QDate date);
    void setTime(QTime time);
    QTime getTime();

    void setDate(QDate date);
    QDate getDate();

    QString time24Hours();
    QString time12Hours();

    QString europeanDate();
    QString americanDate();


private:
    QTime mTime;
    QDate mDate;
    QString mTimeString;
    QString mDateString;
};

#endif // MYDATETIME_H
