#ifndef MYDATETIME_H
#define MYDATETIME_H

#include <QObject>
#include <QDateTime>

class myDateTime
{
public:
    myDateTime();
    void setTime(QTime time);
    QTime getTime();

    void setDate(QDate date);
    QDate getDate();

    QString get24Hours();
    QString get12Hours();

    QString getEuropeanDate();
    QString getAmericanDate();


private:
    QTime mTime;
    QDate mDate;
    QString mTimeString;
    QString mDateString;
};

#endif // MYDATETIME_H
