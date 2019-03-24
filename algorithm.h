#ifndef ALGORITHM_H
#define ALGORITHM_H
#include <string.h>
#include "delivery.h"
#include "schedule.h"
#include <vector>
#include <QDate>
using namespace std;

class Algorithm
{
    int count;
    vector <Schedule> schedule;
    Delivery *currentDeliveries;
public:
    void loadSchedule();
    Algorithm(Delivery *currentDeliveries);
    int getCount();
    int weekDay (int day, int week, int year);
    void calculateDateShip(int count);
    void calculateDateStart(int count, bool set);
    void datePlusDays( struct tm* date, int days );
    void Sorting(Delivery *data, int count);
    void CheckSchedule(vector <Schedule>& newSchedule, string d);
};

#endif // ALGORITHM_H
