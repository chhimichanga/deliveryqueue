#ifndef SCHEDULE_H
#define SCHEDULE_H
#include <string>

using namespace std;

class Schedule
{
    string  date;
    int     *staffMinutesTotal,
            *staffMinutesOccupied,
            *staffMinutesAvailable,
            day,
            month,
            year;

public:
    Schedule();
    Schedule(string d);
    ~Schedule();

    void    setStaffingLevel(int* staffinglevel);
    void    setMinutesAvailable(int *available);
    void    setMinutesOccupied(int *occupied);

    string  getDate() {return date;}
    int*    getMinutesTotal() {return staffMinutesTotal;}
    int*    getMinutesOccupied() {return staffMinutesOccupied;}
    int*    getMinutesAvailable() {return staffMinutesAvailable;}

    int     getDay() {return day;}
    int     getMonth() {return month;}
    int     getYear() {return year;}

};

#endif // SCHEDULE_H
