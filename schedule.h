#ifndef SCHEDULE_H
#define SCHEDULE_H
#include <string>

using namespace std;

class Schedule
{

    string    date;
    int *staff_totalworkingminutes,
        *staff_occupiedminutes,
         *staff_availableminutes;

    int mailroom;
    int day,
        month,
        year;
public:
    Schedule();
    Schedule(string d);
    ~Schedule();
    void   set_numOfStaffs(int num);
    void   set_availableminutes(int *available);
    void   set_occupiedminutes(int *occupied);
    void  set_approvalhours(int hours);
    string    get_date() {return date;}
    int*    get_workingminutes() {return staff_totalworkingminutes;}
    int*    get_occupiedminutes() {return staff_occupiedminutes;}
    int*    get_availableminutes() {return staff_availableminutes;}
    int   get_approvalhours() {return mailroom;}
    int    get_day() {return day;}
    int    get_month() {return month;}
    int    get_year() {return year;}

};

#endif // SCHEDULE_H
