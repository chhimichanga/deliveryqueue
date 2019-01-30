#include "schedule.h"
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
Schedule::Schedule(){
    hours = 24;
    work = 0;
    date = '\0';
    day = 0;
    month = 0;
    year = 0;
}
Schedule::Schedule(string d)
{

    hours = 24; // 2 staffs, each staff works 8 hours a day
    work = 0;
    date = d;


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

void Schedule::set_totalhours(int h){
    hours = h;
}

void Schedule::set_workinghours(int h){
    work += h;
}
