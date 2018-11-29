#include "algorithm.h"
#include <string>
#include <delivery.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <ctime>
#include <QDebug>
#include <QDir>

using namespace std;

Algorithm::Algorithm(Delivery *currentDeliveries)
{
    count = 0;

    // load save file
    ifstream file("save.csv");
    string str;         // temorary storage for reading
    getline(file, str); // skip the first line of headers

    while(getline(file, str)){ // read until reach end of file
        istringstream iss(str);
        string token;
        int j = 0;
        while(getline(iss, token, ',')){    // while delimiter is being read
            if(j==0) currentDeliveries[count].set_ID(stoi(token));                // set Delivery ID
            else if(j == 1) currentDeliveries[count].set_DateDeliver(token);       // set required delivery date
            else if(j == 2) currentDeliveries[count].set_Location(token);          // set location
            else if(j == 3) currentDeliveries[count].set_ShippingMethod(token);
            else if(j == 4) currentDeliveries[count].set_Classification(token);    // set classification
            else if(j == 5) currentDeliveries[count].set_NumItems(stoi(token));                       // set number of items
            else if(j == 6) currentDeliveries[count].set_MediaType(token);        // set media type
            j++;    // move on to next field in line
        }
        currentDeliveries[count].set_DateShip(CalculateDateShip(currentDeliveries[count].get_DateDeliver(), currentDeliveries[count].get_Location()));
        count++;   // move on to next line in file
    }
}

int Algorithm::get_Count(){
    return count;
}

int Algorithm::StringToValue(string s){
    // return classification level as an integer value
    if(s == "Secret") return 3;
    else if(s == "Unclassified") return 2;
    else if(s == "Confidential") return 1;
    // return destination as integer value
    else if(s == "Rota Spain") return 3;
    else if(s == "King's Bay Georgia" || s == "Area 51 Nevada" || s == "Pensacola Florida" ||
            s == "Great Lakes Illinois" || s == "Bremerton Washington") return 2;
    else if(s == "New London Connecticut") return 1;
    // return media type as integer value
    else if(s == "Documentation") return 1;
    else if(s == "Software") return 2;
    else if(s == "Hardware") return 3;

    else return 0;
}

int Algorithm::DayOfTheWeek (int day, int month, int year){
    // date converted to c++ time structure.
    // values include seconds, minutes, hour, day, month, year, weekday, yearday, DST
    tm time_in = {0, 0, 0, day, month, year, 0, 0, -1};

    time_t time_temp = mktime(&time_in);

    //Note: Return value of localtime is not threadsafe, because it might be
    // (and will be) reused in subsequent calls to std::localtime!
    const tm * time_out = localtime(&time_temp);
    if (time_out->tm_wday == 0) return 4;       // if it is sunday moving the schedule forward by 4 days
    else if (time_out->tm_wday == 4) return 1;  // if it is thursday moving the schedule forward by 1 days
    else if (time_out->tm_wday == 5) return 2;  // if it is friday moving the schedule forward by 2 days
    else if (time_out->tm_wday == 6) return 3;  // if it is saturday moving the schedule forward by 3 days
    else return 0;                              // Sunday == 0, Monday == 1, and so on ...
}

// do the calculation of date, plus or minus a number of days on a date
void Algorithm::DatePlusDays( struct tm* date, int days )
{
    // number of seconds in a day = 24 hours * 60 minutes * 60 seconds
    const time_t ONE_DAY = 24 * 60 * 60 ;

    // Seconds since start of epoch
    time_t date_seconds = mktime(date) + (days * ONE_DAY) ;

    // Update caller's date
    // Use localtime because mktime converts to UTC so may change date
    *date = *localtime( &date_seconds );
}

string Algorithm::CalculateDateShip(string RDD, string Location){
    struct tm date = { 0, 0, 0, 0, 0, 0, 0, 0, -1 };
    char buffer[80];
    string str; // return value

    // if the location is outside US
    if(Location == "Rota Spain" || Location == "Rota Spain"){
        istringstream rdd(RDD);
        string token;
        int j = 0;

        // separate the whole string of ship date into date, month and year
        while (getline(rdd, token, '/')){
            stringstream geek(token);
            int x = 0;
            geek >> x;
            if(j==0) date.tm_mday = x;
            else if(j==1) date.tm_mon = x - 1;
            else if(j==2) date.tm_year = x - 1900;
            j++;

        }

        DatePlusDays(&date, -21);   // 3 weeks to arrive at location once shipped

        // if the ship date is not Monday, Tuesday nor Wednesday, move the schedule forward
        if(DayOfTheWeek(date.tm_mday, date.tm_mon, date.tm_year) != 0)
            DatePlusDays(&date, -DayOfTheWeek(date.tm_mday, date.tm_mon, date.tm_year));

        // convert the date to string
        strftime(buffer,sizeof(buffer),"%d/%m/%Y", &date);
        str = buffer;
    }
    // if the location is in US
    else if(Location == "King's Bay Georgia" || Location == "Area 51 Nevada"|| Location == "Pensacola Florida" || Location == "Great Lakes Illinois" || Location == "Bremerton Washington"){
        istringstream rdd(RDD);
        string token;
        int j = 0;

        // separate the whole string of ship date into date, month and year
        while (getline(rdd, token, '/')){
            stringstream geek(token);
            int x = 0;
            geek >> x;
            if(j==0) date.tm_mday = x;
            else if(j==1) date.tm_mon = x - 1;
            else if(j==2) date.tm_year = x - 1900;
            j++;
        }

        DatePlusDays(&date, -7 );    // 1 week to arrive at location once shipped

        // if the ship date is not Monday, Tuesday nor Wednesday, move the schedule forward
        if(DayOfTheWeek(date.tm_mday, date.tm_mon, date.tm_year) != 0)
            DatePlusDays(&date, -DayOfTheWeek(date.tm_mday, date.tm_mon, date.tm_year));

        // convert the date to string
        strftime(buffer,sizeof(buffer),"%d/%m/%Y", &date);
        str = buffer;
    } else {
        istringstream rdd(RDD);
        string token;
        int j = 0;

        while (getline(rdd, token, '/')){
            stringstream geek(token);
            int x = 0;
            geek >> x;
            if(j==0) date.tm_mday = x;
            if(j==1) date.tm_mon = x - 1;
            if(j==2) date.tm_year = x - 1900;
            j++;
        }

        // if the ship date is not Monday, Tuesday nor Wednesday, move the schedule forward
        if(DayOfTheWeek(date.tm_mday, date.tm_mon, date.tm_year) != 0)
            DatePlusDays(&date, -DayOfTheWeek(date.tm_mday, date.tm_mon, date.tm_year));

        // convert the date to string
        strftime(buffer,sizeof(buffer),"%d/%m/%Y", &date);
        str = buffer;
    }

    return str;
}
