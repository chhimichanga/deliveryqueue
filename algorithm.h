#ifndef ALGORITHM_H
#define ALGORITHM_H
#include <string.h>
#include "delivery.h"

using namespace std;

class Algorithm
{
    int count;

public:

    Algorithm(Delivery *currentDeliveries);
    int get_Count();
    int DayOfTheWeek (int day, int week, int year);
    string CalculateDateShip(string RDD, string Location);
    void DatePlusDays( struct tm* date, int days );
    void Sorting(Delivery *data, int count);
    int StringToValue(string s);
};

#endif // ALGORITHM_H
