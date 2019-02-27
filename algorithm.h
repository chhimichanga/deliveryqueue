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
    vector <Schedule> schedule;
public:

    Algorithm(Delivery *currentDeliveries);
    int getCount();
    int weekDay (int day, int week, int year);
    string calculateDateShip(string RDD, string Location);
    string calculateDateStart(string RSD, int  numOfitems, int staffinglevel);
    void datePlusDays( struct tm* date, int days );
    void Sorting(Delivery *data, int count);
    void CheckSchedule(vector <Schedule>& newSchedule, string d);
};

#endif // ALGORITHM_H
