#ifndef SCHEDULE_H
#define SCHEDULE_H
#include <string>

using namespace std;

class Schedule
{

    string    date;
    int    hours,
           work;
    int day,
        month,
        year;
public:
    Schedule();
    Schedule(string d);
    ~Schedule();
    void   set_workinghours(int h);
    void   set_totalhours(int h);
    string    get_date() {return date;}
    int    get_workinghours() {return work;}
    int    get_totalhours() {return hours;}
    int    get_day() {return day;}
    int    get_month() {return month;}
    int    get_year() {return year;}

};

#endif // SCHEDULE_H
