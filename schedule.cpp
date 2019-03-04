#include "schedule.h"
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>

Schedule::Schedule(){
    // initializing variables
    staffMinutesTotal = new int[3];
    staffMinutesOccupied = new int[3];
    staffMinutesAvailable = new int[3];

    // the 480 minutes here represent 8 hours in a work day
    for(int i = 0; i < 3; i++){
        staffMinutesTotal[i] = 480;
        staffMinutesOccupied[i] = 0;
        staffMinutesAvailable[i] = 480;
    }

    date = '\0';
    day = 0;
    month = 0;
    year = 0;
}

// initialize the schedule for the date d being passed in
Schedule::Schedule(string d){
    //initializing variables
    staffMinutesTotal = new int[3];
    staffMinutesOccupied= new int[3];
    staffMinutesAvailable = new int[3];

    for(int i = 0; i < 3; i++){
        staffMinutesTotal[i] = 480;
        staffMinutesOccupied[i] = 0;
        staffMinutesAvailable[i] = 480;
    }

    date = d;

    // set day, month, year to a schedule object in case for use
    istringstream line(d);
    string token;
    int dateSection = 0;
    while (getline(line, token, '/')){
        stringstream geek(token);
        int x = 0;
        geek >> x;
        if(dateSection == 0)
            day = x;
        else if(dateSection == 1)
            month = x;
        else if(dateSection == 2)
            year = x;
        dateSection++;
    }

}

Schedule::~Schedule(){

}

// update staffing level for a delivery
void Schedule::setStaffingLevel(int n){
    for (int i = (3-n); i < 3; i++){
        staffMinutesTotal[i] = 0;
        staffMinutesAvailable[i] = 0;
        // so if the parameter passed in (number of available staffs) is 2, the third value in array is going to be set 0
        //meaning there are only going to be 2 staffs having working hours
    }
}

// update minutes available in each staff's workday
void Schedule::setMinutesAvailable(int *available){
    for(int i = 0; i < 3; i ++){
        staffMinutesAvailable[i] = available[i];
        staffMinutesOccupied[i] = staffMinutesTotal[i] - staffMinutesAvailable[i];
    }
}

// update minutes occupied in each staff's workday
void Schedule::setMinutesOccupied(int *occupied){
    for(int i = 0; i < 3; i++){
        staffMinutesOccupied[i] = occupied[i];
        staffMinutesAvailable[i] = staffMinutesTotal[i] - staffMinutesOccupied[i];

    }
}



