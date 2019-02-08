#include "algorithm.h"
#include <string>
#include <delivery.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <ctime>
#include <QDebug>
#include <QDir>
#include "schedule.h"
#include <vector>

using namespace std;

Algorithm::Algorithm(Delivery *currentDeliveries)
{
    count = 0;
    string str;         // temorary storage for reading

    // load save file
    ifstream file("save.csv");
    getline(file, str); // skip the first line of headers

    vector<Schedule> sche;

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
            else if(j == 7) currentDeliveries[count].set_StaffingLevel(stoi(token));
            j++;    // move on to next field in line
        }
        currentDeliveries[count].set_DateShip(CalculateDateShip(currentDeliveries[count].get_DateDeliver(), currentDeliveries[count].get_Location()));

        count++;   // move on to next line in file
    }

//     after done calculating required ship date
//     next, we are going to calculate the required start date by checking shedule
    for(int k = 0; k < count; k++){ // go through each delivery
        string shipDate = currentDeliveries[k].get_DateShip();// store the calculated shipping date in a variable
        string startDate; // calculate the start date
        struct tm date = { 0, 0, 0, 0, 0, 0, 0, 0, -1 };
        char buffer[80];
        istringstream rsd(shipDate);
        string token;
        int j = 0;
        while (getline(rsd, token, '/')){
            stringstream geek(token);
            int x = 0;
            geek >> x;
            if(j==0) date.tm_mday = x;
            if(j==1) date.tm_mon = x - 1;
            if(j==2) date.tm_year = x - 1900;
            j++;
        }

        DatePlusDays(&date, -3);   // every delivery has to wait 3 days for approval after submitting
        // if the date to start shipping is not Monday, Tuesday nor Wednesday, move the schedule forward
        if(DayOfTheWeek(date.tm_mday, date.tm_mon, date.tm_year) != 0)
            DatePlusDays(&date, -DayOfTheWeek(date.tm_mday, date.tm_mon, date.tm_year));

        // convert the date to string
        strftime(buffer,sizeof(buffer),"%d/%m/%Y", &date);
        startDate = buffer;// now start date is set 3 days prior to ship date

        // for later calculation
        int requiredminutes = 0;
        int requiredpeople = 0;
        int requiredapproval = 0;
        // calculate the amount of hours needed for preparing delivery
        if(currentDeliveries[k].get_NumItems() <= 10){
            if(currentDeliveries[k].get_StaffingLevel() == 1){
                requiredminutes = 240;  // one person needs 240 minutes for preparing
                requiredpeople = 1;
            }
            else if(currentDeliveries[k].get_StaffingLevel() == 2){
                requiredminutes = 120;  // each person needs 120 minutes for preparing
                requiredpeople = 2;
            }
            else if(currentDeliveries[k].get_StaffingLevel() == 3){
                requiredminutes = 80; // each person needs 80 minutes for preparing
                requiredpeople = 3;
            }
            requiredapproval = 2; // it takes mailroom 2 hours to approve delivery less than 40 items
        }

        else if(currentDeliveries[k].get_NumItems() > 10 && currentDeliveries[k].get_NumItems() <= 50){
            if(currentDeliveries[k].get_StaffingLevel() == 1){
                requiredminutes = 480;  // one person needs 480 minutes for preparing
                requiredpeople = 1;
            }
            else if(currentDeliveries[k].get_StaffingLevel() == 2){
                requiredminutes = 240;  // each person needs 240 minutes for preparing
                requiredpeople = 2;
            }
            else if(currentDeliveries[k].get_StaffingLevel() == 3){

                requiredminutes = 160; // each person needs 160 minutes for preparing
                requiredpeople = 3;
            }
            requiredapproval = 2; // it takes mailroom 2 hours to approve delivery less than 50 items
        }
        else if(currentDeliveries[k].get_NumItems() > 50){
            if(currentDeliveries[k].get_StaffingLevel() == 1){
                requiredminutes = 960;  // one person needs 960 minutes for preparing
                requiredpeople = 1;
            }
            else if(currentDeliveries[k].get_StaffingLevel() == 2){
                requiredminutes = 480;  // each person needs 480 hours for preparing
                requiredpeople = 2;
            }
            else if(currentDeliveries[k].get_StaffingLevel() == 3){

                requiredminutes = 320; // each person needs 320 minutes for preparing
                requiredpeople = 3;
            }
            requiredapproval = 4; // it takes mailroom 4 hours to approve delivery more than 50 items
        }


        int found = 0;  // find the perfect day to start preparing
        int totalwork = requiredminutes * requiredpeople;

        while(found == 0){
            auto it = find_if(sche.begin(), sche.end(), [&startDate](Schedule& obj) {return obj.get_date() == startDate;});// check if the schedule object that hold existed in the vector
            auto index = std::distance(sche.begin(), it); // the index where schedule locate at vector

            if (it == sche.end()){  // if the shcedule on a specifc date is not initialized yet
                Schedule newSchedule(startDate); // create a new instance of shcedule and push it to the vector
                sche.push_back(newSchedule);
                char new_buffer[80]; // buffer to store the required start date

                // check if it is on Monday, Tuesday, Wednesday
                if(DayOfTheWeek(date.tm_mday, date.tm_mon, date.tm_year) != 0)
                    DatePlusDays(&date, -DayOfTheWeek(date.tm_mday, date.tm_mon, date.tm_year));
                strftime(new_buffer,sizeof(new_buffer),"%d/%m/%Y", &date);
                startDate = new_buffer;

            }

            else if (it != sche.end()){// if the schedule object is already created, check if this delivery fits in the schedule
                int *available = sche.at(index).get_availableminutes();

                bool staff[3] = {false, false, false};
                for(int l = 0; l < 3; l++){

                    if (requiredpeople == 0 || totalwork == 0){
                        break;
                    }
                    if (available[l] >= requiredminutes && totalwork >= requiredminutes){ // worker has more available time than the required preparing time
                        totalwork = totalwork - requiredminutes; // update the total amount of working hours for all staffs
                        --requiredpeople; // done assigning the job to one of the staffs
                        available[l] = available[l] - requiredminutes; // update the staff schedule
                        sche.at(index).set_availableminutes(available); // passing to schedule object
                        staff[l] = true; // staff number l can work on that delivery on that date
                    }
                    else if (available[l] >= totalwork && totalwork <= requiredminutes){ // the condition that worker has already done most of it on another day
                        available[l] = available[l] - totalwork;
                        sche.at(index).set_availableminutes(available); // passing to schedule object
                        --requiredpeople;
                        totalwork = 0;
                        staff[l] = true;
                    }

                }
                   
                if (requiredpeople == 0 || totalwork == 0){ // the condition that required preparing time can be fitted in the delivery staff working schedule
                    if(sche.at(index).get_approvalhours() >= requiredapproval){// if the mailroom has available time also

                        sche.at(index).set_approvalhours(requiredapproval);
                        requiredapproval = 0;
                        found = 1; // the delivery has been fitted in the schedule, exit the loop
                    }
                    else{ // if mailroom does not have available time to approve the delivery
                        char new_buffer[80]; // buffer to store the required start date
                        DatePlusDays(&date, -1);
                        strftime(new_buffer,sizeof(new_buffer),"%d/%m/%Y", &date);
                        startDate = new_buffer;
                    }

                 }
                else{ // there is still required preparing time left, meaning nobodys has not enough available time
                    int temp = requiredpeople;
                    for(int m = 0; m < 3; m ++){ // find one delivery staff who still has some available time left
                        if(staff[m] == false && available[m] != 0){
                            totalwork -= available[m];
                            available[m] = 0;
                            sche.at(index).set_availableminutes(available);
                            staff[m] = true;
                            --requiredpeople;
                         }
                         if(requiredpeople == 0){
                            break;
                         }
                    }
                    requiredpeople = temp;
                    char new_buffer[80]; // buffer to store the required start date
                    DatePlusDays(&date, -1);
                    strftime(new_buffer,sizeof(new_buffer),"%d/%m/%Y", &date);
                    startDate = new_buffer;
                }    
            }
            qDebug() << QString::number(requiredpeople) + "--" + QString::number(totalwork);
            qDebug() << QString::number(currentDeliveries[k].get_ID()) + "--" + QString::fromStdString(startDate);
        }
        currentDeliveries[k].set_DateStart(startDate); // once exit the loop, passing calculated start date to delivery object
    }
}
//void CheckSchedule(vector <Schedule>& newSchedule, string d){
//    Schedule abc(d);
//    newSchedule.push_back(abc);
//}
int Algorithm::get_Count(){
    return count;
}

int Algorithm::StringToValue(string s){

    // return destination as integer value
    if(s == "Holy Loch Scotland") return 3;
    else if(s == "Bremerton Washington" || s == "Pensacola Florida" || s == "San Diego California" ||
            s == "King's Bay Georgia" || s == "Pearl Harbor Hawaii") return 2;
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
    if(Location == "Holy Loch Scotland"){
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
    // if the location is in a faraway location in the US
    else if(Location == "King's Bay Georgia" || Location == "San Diego California"|| Location == "Pensacola Florida" || Location == "Pearl Harbor Hawaii" || Location == "Bremerton Washington"){
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
    } else {    // if the location is in a nearby location in the US
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
        DatePlusDays(&date, -3 );

        // if the ship date is not Monday, Tuesday nor Wednesday, move the schedule forward
        if(DayOfTheWeek(date.tm_mday, date.tm_mon, date.tm_year) != 0)
            DatePlusDays(&date, -DayOfTheWeek(date.tm_mday, date.tm_mon, date.tm_year));

        // convert the date to string
        strftime(buffer,sizeof(buffer),"%d/%m/%Y", &date);
        str = buffer;
    }

    return str;
}
