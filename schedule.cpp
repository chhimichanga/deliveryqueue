#include "schedule.h"
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>

Schedule::Schedule(){
    // initializing variables
    staff_totalworkingminutes = new int[3];
    staff_occupiedminutes = new int[3];
    staff_availableminutes = new int[3];

    // the 480 minutes here represent 8 hours in a work day
    for(int i = 0; i < 3; i++){
        staff_totalworkingminutes[i] = 480;
        staff_occupiedminutes[i] = 0;
        staff_availableminutes[i] = 480;
    }

    date = '\0';
    day = 0;
    month = 0;
    year = 0;
}

Schedule::Schedule(string d){
    //initializing variables
    staff_totalworkingminutes = new int[3];
    staff_occupiedminutes= new int[3];
    staff_availableminutes = new int[3];
    for(int i = 0; i < 3; i++){
        staff_totalworkingminutes[i] = 480;
        staff_occupiedminutes[i] = 0;
        staff_availableminutes[i] = 480;
    }

    date = d;

    // set day, month, year to a schedule object in case for use
    istringstream line(d);
    string token;
    int j = 0;
    while (getline(line, token, '/')){
        stringstream geek(token);
        int x = 0;
        geek >> x;
        if(j==0) day = x;
        else if(j==1) month = x;
        else if(j==2) year = x;
        j++;
    }

}

Schedule::~Schedule(){

}

//function for later use to change the number of available staffs
void Schedule::set_numOfStaffs(int n){
    for (int i =  (3-n); i < 3; i++){
        staff_totalworkingminutes[i] = 0;
        staff_availableminutes[i] = 0;
        // so if the parameter passed in (number of available staffs) is 2, the third value in array is going to be set 0
        //meaning there are only going to be 2 staffs having working hours
    }
}

void Schedule::set_availableminutes(int *available){
    for(int i = 0; i < 3; i ++){
        staff_availableminutes[i] = available[i];
        staff_occupiedminutes[i] = staff_totalworkingminutes[i] - staff_availableminutes[i]; // change occpied minutes as well
    }
}

void Schedule::set_occupiedminutes(int * occupied){
    for(int i = 0; i < 3; i++){
        staff_occupiedminutes[i] = occupied[i];
        staff_availableminutes[i] = staff_totalworkingminutes[i] - staff_occupiedminutes[i];

    }
}



