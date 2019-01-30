#ifndef ALGORITHM_H
#define ALGORITHM_H
#include <string.h>
#include "delivery.h"
#include "schedule.h"
#include <vector>
using namespace std;

class Algorithm
{
    int count;
    vector <Schedule> sche;
public:

    Algorithm(Delivery *currentDeliveries);
    int get_Count();
    int DayOfTheWeek (int day, int week, int year);
    string CalculateDateShip(string RDD, string Location);
    void DatePlusDays( struct tm* date, int days );
    void Sorting(Delivery *data, int count);
    int StringToValue(string s);
    void CheckSchedule(vector <Schedule>& newSchedule, string d);
};

#endif // ALGORITHM_H
