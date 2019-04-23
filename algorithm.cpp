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
#include "QString"

#include <windows.h>

using namespace std;

// creates form to edit delivery
Algorithm::Algorithm(Delivery *incomingDeliveries)
{
    currentDeliveries = incomingDeliveries;

    count = 0;                      // the number of deliveries
    string temp;                    // temporary storage for reading

    ifstream file("save.csv");      // load save file
    getline(file, temp);            // skip the first line of headers

    vector<Schedule> schedule;      // vector of schedules
    loadSchedule();                 // read schedule information from the local file

    while(getline(file, temp)){     // read file until reach the end
        bool edit = false;          // check if a delivery has been edited
        istringstream iss(temp);    // iss represents each delivery
        string token;
        int fieldNumber = 0;
        while(getline(iss, token, ',')){    // each line gets parsed with every field separated by a comma
            if(fieldNumber == 0){
                currentDeliveries[count].set_TransmittalNumber(token);      // transmission num
            }else if(fieldNumber == 1){
                currentDeliveries[count].set_Destination(token);            // set location
            }else if(fieldNumber == 2){
                currentDeliveries[count].set_TransitMethod(token);          // set shipping method
            }else if(fieldNumber == 3){
                currentDeliveries[count].set_ShipNameHullNumber(token);     // set ship hull number
            }else if(fieldNumber == 4){
                currentDeliveries[count].set_ECN(token);                    // set ECN
            }else if(fieldNumber == 5){
                currentDeliveries[count].set_MediaType(token);              // set media type
            }else if(fieldNumber == 6){
                currentDeliveries[count].set_NumItems(stoi(token));         // set number of items
            }else if(fieldNumber == 7){
                currentDeliveries[count].set_Classification(token);         // set classification
            }else if(fieldNumber == 8){
                currentDeliveries[count].set_StaffingLevel(stoi(token));    // set staffing level
            }else if(fieldNumber == 9){
                currentDeliveries[count].set_DateDeliver(token);            // set required delivery date
            }else if(fieldNumber == 10){
                currentDeliveries[count].set_DateShip(token);               // set ship date
            }else if(fieldNumber == 11){
                if(token == "") {           // if a delivery has been edited, the filed of required start date is going to be empty
                    edit = true;
                }
                else{                       // if a delivery has not been edited
                    edit = false;
                }
            }
            else if(fieldNumber == 12){     // set the assigned staffs if staffs are assigned already
                 currentDeliveries[count].set_Staff(token);
            }
            fieldNumber++;                  // move on to next field in line
        }

        if(fieldNumber == 10){               // use for the deliveries that just have been added
            calculateDateShip(count);        // calculate the ship date
            calculateDateStart(count, 0);    // calculate the start date for the deliveries just added
        }
        else{
            if(edit == true){
                calculateDateShip(count);     // if the delivery is edited, recalculate the ship date
                calculateDateStart(count, 1); // calculate the start date for deliveries that has assigned staffs
            }
            else{
                calculateDateStart(count, 1); // calculate the start date for deliveries that has assigned staffs
            }
        }

        count++;   // move on to next line in file
    }

    ofstream fileOut;
    fileOut.open("temp.csv");   // load output stream
    fileOut << "Transmittal #, Destination, Transit Method, Ship Name/Hull #, ECN/TECN, Media Type, # of Items, Classification, Staffing Level, Delivery Date, Ship Date, Start Date, Assigned Staff" << endl;
    for(int i = 0; i < count; i++){
        string strDelivery;     // string to add to save file
        // append edited delivery information to string

        strDelivery += currentDeliveries[i].get_TransmittalNumber() + ',';           // unique Transmission #
        strDelivery += currentDeliveries[i].get_Destination() + ',';                 // location
        strDelivery += currentDeliveries[i].get_TransitMethod() + ',';               // shipping method
        strDelivery += currentDeliveries[i].get_ShipNameHullNumber() + ',';          // ship name & hull #
        strDelivery += currentDeliveries[i].get_ECN() + ',';                         // ECN
        strDelivery += currentDeliveries[i].get_MediaType() + ',';                   // media type
        strDelivery += to_string(currentDeliveries[i].get_NumItems()) + ',';         // number of items
        strDelivery += currentDeliveries[i].get_Classification() + ',';              // classification
        strDelivery += to_string(currentDeliveries[i].get_StaffingLevel()) + ',';    // staffing level
        strDelivery += currentDeliveries[i].get_DateDeliver() + ',';                 // delivery date
        strDelivery += currentDeliveries[i].get_DateShip() + ",";                    // ship date
        strDelivery += currentDeliveries[i].get_DateStart() + ",";                   // start date
        strDelivery += currentDeliveries[i].get_Staff() + ",";                       // assigened staffs
        fileOut << strDelivery << endl;                                              // send delivery to save file

    }
    fileOut.close(); //close output file
    file.close();   //close input file

    // delete original file
    remove("save.csv");

    // rename temp.csv to save.csv
    rename("temp.csv","save.csv");

}

int Algorithm::getCount(){// get the number of deliveries
    return count;
}

void Algorithm::loadSchedule(){
    QString currentDate = QDate::currentDate().toString("dd/MM/yyyy"); // current date

    if(QFileInfo("schedule.txt").exists() && !QDir("schedule.txt").exists()){ //The file exists and is not a folder

        ifstream fileIn("schedule.txt");
        if(!fileIn.is_open())                                                 // check if the program can open the local file
            MessageBoxA(NULL, "Cannot open schedule file.", "error", MB_OK);  // if it cannot open the file, an error message will be poped up
        else {                                                                // the program can open the local schedule file
            fileIn.seekg(0);                                                  // move cursor to beginning of schedule.txt
            string line, token, date;

            getline(fileIn, line);                                            // read the line
            istringstream iss(line);
            getline(iss, token, ',');                                         //the first string separated by comma is the date
            date = token;
            if(date == currentDate.toStdString()){                            // if user opens the program at the same date
                int i = 0;                                                    // represent the number of days ahead the current day
                while(getline(iss, token, ',')){                              // each token is the availbility of a staff
                    QDate issueDate = QDate::currentDate().addDays(i);                    // check which day does the data in the file belong to
                    Schedule newSchedule(issueDate.toString("dd/MM/yyyy").toStdString()); //  create a new shceudle object
                    int availability[3];                                        // local variable to store the availability of each staff
                    for(int p = 0; p < 3; p++){                                 // there are three numbers that represent the availability of staffs within a day
                        if(p!=0) getline(iss, token, ',');                      // read a value
                        availability[p] = stoi(token);                          // store the value into the local variable
                    }
                    newSchedule.setStaffingLevel(availability);                 // pass the availability of staffs to the new schedule object
                    schedule.push_back(newSchedule);                            // push the new object into the vector
                    i++;                                                        // next three numbers belong represent schedule of (i++) day from current day
                }
                fileIn.close();                                                 // close the schedule file
            }
            else{                                                               // if the user opens the program on a different day
                string write;                                                   // new data to write
                ofstream fileOut;
                fileOut.open("tempSchedule.txt");                               // load output stream, create a new output file
                for(int temp = 0; temp < 3; temp++){
                    getline(iss, token, ',');                                   // get rid of first three numbers that represent the availability of three staffs from the past day
                }
                write = currentDate.toStdString() + ",";                        // first write the current day
                int i = 0;                                                      // the number of days away from the current day
                while(getline(iss, token, ',')){                                // read until the end

                    int availability[3];                                        // local variable to store the availability of three staffs
                    QDate issueDate = QDate::currentDate().addDays(i);          // check what date is it based on the number of days away from the current day
                    Schedule newSchedule(issueDate.toString("dd/MM/yyyy").toStdString());//  create a new shceudle object
                    for(int p = 0; p < 3; p++){
                        if(p!=0) getline(iss, token, ',');                       // read a value
                        write += token + ",";                                    // store the value into the write string
                        availability[p] = stoi(token);                           // store the value into local variable
                    }
                    newSchedule.setStaffingLevel(availability);                  // pass the availability of staffs to the new schedule object
                    schedule.push_back(newSchedule);                            // push the new object into the vector
                    i++;                                                        // next three numbers belong represent schedule of (i++) day from current day
                }
                int avail[3] = {1,1,1};                                         // defult availability
                Schedule newSchedule(QDate::currentDate().addDays(179).toString("dd/yyyy").toStdString());  // create a new schedule object that represents the schedule of the date 180 days ahead
                newSchedule.setStaffingLevel(avail);                            // pass the default availability to the new object
                schedule.push_back(newSchedule);                                // push object in to the vector
                write += "1,1,1,";                                              // write the schedule of a new day into the file

                fileOut << write << endl;                                        // output the data to the file
                fileOut.close();                                                 //close the file
                fileIn.close();
                remove("schedule.txt");

                // rename temp.csv to save.csv
                rename("tempSchedule.txt","schedule.txt");
            }
        }
    }
    else{
        //The file doesn't exist, either the path doesn't exist or is the path of a folder
        //create a new file that contains a date and 540 "1," string
        string write;
        ofstream fileOut("schedule.txt");              // load output stream, create a new output file
        write += currentDate.toStdString() + ",";
        for(int i = 0; i < 180; i++){                   // 180 days
            for(int j = 0; j < 3; j++){                 // 3 staffs
                write += "1,";                          // availability
            }
            QDate issueDate = QDate::currentDate().addDays(i);      // calculate the date i days ahead of the current day
            Schedule newSchedule(issueDate.toString("dd/MM/yyyy").toStdString());   // create a new schedule object
            schedule.push_back(newSchedule);                       // push the object to the vector
        }
        fileOut << write << endl;
        fileOut.close();                        // close file
    }



}

// verify that the ship date falls on a Monday, Tuesday, or Wednesday. if not, move the ship date back to a Wednesday
int Algorithm::weekDay(int day, int month, int year){
    // date converted to c++ time structure.
    // values include seconds, minutes, hour, day, month, year, weekday, yearday, daylight savings time
    tm time_in = {0, 0, 0, day, month, year, 0, 0, -1};

    // seconds between start of epoch and the date being operated on
    time_t time_temp = mktime(&time_in);

    // Note: Return value of localtime is not threadsafe, because it might be
    // (and will be) reused in subsequent calls to std::localtime!
    const tm * time_out = localtime(&time_temp);

    if (time_out->tm_wday == 0) // if it is sunday, moving the ship date back by 4 days
        return 4;
    else if (time_out->tm_wday == 4)    // if it is thursday, moving the ship date back by 1 days
        return 1;
    else if (time_out->tm_wday == 5)    // if it is friday, moving the ship date back by 2 days
        return 2;
    else if (time_out->tm_wday == 6)    // if it is saturday, moving the ship date back by 3 days
        return 3;
    else                                // Monday == 1, Tuesday == 2, Wednesday == 3
        return 0;
}

// add or subtract days from a date
void Algorithm::datePlusDays( struct tm* date, int days )
{
    // number of seconds in a day = 24 hours * 60 minutes * 60 seconds
    const time_t SECONDS_IN_A_DAY = 24 * 60 * 60 ;

    // seconds between start of epoch and the date being operated on
    time_t date_seconds = mktime(date) + (days * SECONDS_IN_A_DAY) ;

    // Update caller's date
    // Use localtime because mktime converts to UTC so may change date
    *date = *localtime(&date_seconds);
}

// use the required delivery date to calculate the required ship date based on the delivery's location
void Algorithm::calculateDateShip(int count){
    string dateDeliver = currentDeliveries[count].get_DateDeliver();
    string Location = currentDeliveries[count].get_Destination();
    struct tm dateShipTemp = { 0, 0, 0, 0, 0, 0, 0, 0, -1 };    // store delivery date here, then subtract as many days as needed to get ship date
    string dateShip;         // return value

    istringstream inDateDeliver(dateDeliver);
    string token;

    // parse ship date and store its day, month, and year into dateShipTemp
    int dateSection = 0;
    while (getline(inDateDeliver, token, '/')){
        stringstream geek(token);
        int x = 0;
        geek >> x;
        if(dateSection == 0)
            dateShipTemp.tm_mday = x;
        else if(dateSection == 1)
            dateShipTemp.tm_mon = x - 1;
        else if(dateSection == 2)
            dateShipTemp.tm_year = x - 1900;
        dateSection++;    // move to next date field
    }

    // if the location is outside US
    if(Location == "Holy Loch Scotland")
        datePlusDays(&dateShipTemp, -21);   // 3 weeks to arrive at location once shipped
    // if the location is in a faraway location in the US
    else if(Location == "King's Bay Georgia" || Location == "San Diego California"|| Location == "Pensacola Florida" || Location == "Pearl Harbor Hawaii" || Location == "Bremerton Washington")
        datePlusDays(&dateShipTemp, -7);    // 1 week to arrive at location once shipped
    // if the location is in a nearby location in the US
    else if(Location == "New London Connecticut")
        datePlusDays(&dateShipTemp, -3);    // 3 days to arrive at location once shipped

    // if the ship date is not Monday, Tuesday, nor Wednesday, move the ship date back
    if(weekDay(dateShipTemp.tm_mday, dateShipTemp.tm_mon, dateShipTemp.tm_year) != 0)
        datePlusDays(&dateShipTemp, -weekDay(dateShipTemp.tm_mday, dateShipTemp.tm_mon, dateShipTemp.tm_year));

    // convert the date to string
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y", &dateShipTemp);
    dateShip = buffer;

    currentDeliveries[count].set_DateShip(dateShip);
    return;
}

// calculate the start date of a delivery based on ship date, number of items, staffing level, and staff schedule
void Algorithm::calculateDateStart(int count, bool set){
    int itemCount = currentDeliveries[count].get_NumItems();
    int staffingLevel = currentDeliveries[count].get_StaffingLevel();
    string dateShipTemp = currentDeliveries[count].get_DateShip();  // store the calculated shipping date in a variable
    string dateStart;                // calculate the start date
    struct tm dateStartTemp = { 0, 0, 0, 0, 0, 0, 0, 0, -1 };

    // set up input stream to parse ship date
    istringstream inDateShip(dateShipTemp);

    // parse ship date and store its month, day, and year into dateStartTemp
    string token;
    int dateSection = 0;
    while (getline(inDateShip, token, '/')){
        stringstream geek(token);
        int x = 0;
        geek >> x;
        if(dateSection == 0)
            dateStartTemp.tm_mday = x;
        if(dateSection == 1)
            dateStartTemp.tm_mon = x - 1;
        if(dateSection == 2)
            dateStartTemp.tm_year = x - 1900;
        dateSection++;
    }

    // every delivery must be approved by the mail room, which takes about 3 business days on average.
    // move date back 3 days from ship date
    datePlusDays(&dateStartTemp, -3);

    // if moving back 3 days lands on a saturday or sunday, move back 2 days
    if(weekDay(dateStartTemp.tm_mday, dateStartTemp.tm_mon, dateStartTemp.tm_year) == 3 ||
            weekDay(dateStartTemp.tm_mday, dateStartTemp.tm_mon, dateStartTemp.tm_year) == 4)
        datePlusDays(&dateStartTemp, -2);

    // convert the date to string
    // now start date is set 3 days prior to ship date
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y", &dateStartTemp);
    dateStart = buffer;

    int requiredMinutes = 0;
    int requiredPeople = 0;

    // calculate the amount of hours needed to prepare a delivery
    // deliveries with 10 or less items need 4 hours (240 minutes) to pack.
    // divide work evenly among staff: 1 staff need 240 minutes, 2 staff need 120 minutes, 3 need 80
    if(itemCount <= 10)
        requiredMinutes = 240 / staffingLevel;

    // deliveries with more than 10 and up to 50 items take 8 hours (480 minutes) to pack
    // 1 staff need 480 minutes, 2 need 240, 3 need 160
    else if(itemCount > 10 && itemCount <= 50)
        requiredMinutes = 480 / staffingLevel;

    // deliveries with more than 50 items take 16 hours (960 minutes) to pack
    // 1 staff need 960 minutes, 2 need 480, 3 need 320
    else if(itemCount > 50)
        requiredMinutes = 960 / staffingLevel;

    requiredPeople = staffingLevel; // record number of people needed
    int totalWork = requiredMinutes * requiredPeople; // total man-hours needed for delivery

    // now that the time needed to pack the delivery is determined, find a good day to start packing.
    int dateStartFound = 0;     // find the perfect day to start preparing delivery
    char bufferDateStart[80];   // buffer to store the required start date
    bool staff[3] = {false, false, false}; // chosen staffs for a delivery

    // if a delivery is alread set with assigned staffs
    if(set == 1){
        string assignedStaffs = currentDeliveries[count].get_Staff(); // get the assigned staffs from the object
        istringstream iss(assignedStaffs);
        while(getline(iss, token, ';')){                              // read until the end
            for(int three = 0; three < 3; three++){
                if(token == currentDeliveries[count].get_StaffName()[three]){ // if the data matches the name
                    staff[three] = true;                                      // then set the staff to be true working on the delivery
                    requiredPeople--;                                         // needed person minus one
                    break;                                                    // go back to the outer loop to read next staff in the string
                }
            }
        }
    }
    while(dateStartFound == 0){
        // check if the schedule object on a specific date exists
        auto it = find_if(schedule.begin(), schedule.end(), [&dateStart](Schedule& obj){return obj.getDate() == dateStart;});
        auto index = std::distance(schedule.begin(), it);   // the schedule's index in the vector

        // if the schedule on a specifc date is not initialized yet,
        //  that means a work schedule has not been set for that date (no new deliveries planned)
        if (it == schedule.end()){
            // check if current start date is on saturday or sunday. if so, move back to friday
            if(weekDay(dateStartTemp.tm_mday, dateStartTemp.tm_mon, dateStartTemp.tm_year) == 3)
                datePlusDays(&dateStartTemp, -1);
            else if(weekDay(dateStartTemp.tm_mday, dateStartTemp.tm_mon, dateStartTemp.tm_year) == 4)
                datePlusDays(&dateStartTemp, -2);

            // set start date
            strftime(bufferDateStart, sizeof(bufferDateStart), "%d/%m/%Y", &dateStartTemp);
            dateStart = bufferDateStart;

            // create a new instance of schedule and push it to the vector
            Schedule newSchedule(dateStart);
            schedule.push_back(newSchedule);
        }

        // if the schedule object is already created, check if this delivery fits in the schedule
        else if (it != schedule.end()){

            // get minutes available for that day
            int *minutesAvailable = schedule.at(index).getMinutesAvailable();
            bool staff_oneday[3] = {false, false, false};
            int order[3]; // order[0] stores the number of staff who has the most time left, order[2] stores the number of staff who has the least time left
            // go through each staff's working schedule for the day being checked
            // find out who has the most sufficient time in a day and sort them in order
            if(minutesAvailable[0] >= minutesAvailable[1]){
                if(minutesAvailable[1] >= minutesAvailable[2]){
                    order[0] = 0;
                    order[1] = 1;
                    order[2] = 2;
                }
                else{
                    if(minutesAvailable[2] >= minutesAvailable[0]){
                        order[0] = 2;
                        order[1] = 0;
                        order[2] = 1;
                    }
                    else{
                        order[0] = 0;
                        order[1] = 2;
                        order[2] = 1;
                    }
                }
            }
            else{
                if(minutesAvailable[0] >= minutesAvailable[2]){
                    order[0] = 1;
                    order[1] = 0;
                    order[2] = 2;
                }
                else{
                    if(minutesAvailable[2] >= minutesAvailable[1]){
                        order[0] = 2;
                        order[1] = 1;
                        order[2] = 0;
                    }
                    else{
                        order[0] = 1;
                        order[1] = 2;
                        order[2] = 0;
                    }
                }
            }
            // order[0] represents the person who has the most sufficient time,
            for(int p = 0; p < 3; p++){
                int n = order[p]; // check the person who the most sufficient time first
                // the required preparing time can be fit into the delivery staff's working schedule
                if (totalWork == 0)                         // if the total man-hours needed is 0, then break the loop
                    break;
                if (requiredPeople == 0 && staff[n] == false) // if staffs are already chosen for a delivery, and this staff is not the chosen one, jump to next loop
                    continue;
                if (requiredPeople != 0 && staff[n] == true)    // if the delivery still needs other staffs to do it, and this staff is not one of the other staffs, jump to next loop
                    continue;
                // worker has enough time to work on the delivery
                if (minutesAvailable[n] >= requiredMinutes && totalWork >= requiredMinutes){
                    staff[n] = true;                                   // staff n can work on that delivery on that date
                    staff_oneday[n] = true;                                     // this staff has done this job this day
                    minutesAvailable[n] -= requiredMinutes;                     // update staff n's schedule
                    totalWork -= requiredMinutes;                               // update the total working time for all staff
                    if(requiredPeople != 0)
                        --requiredPeople;                                           // done assigning job to one staff

                    schedule.at(index).setMinutesAvailable(minutesAvailable);   // update minutes available for staff n
                }
                // worker can finish the rest of the work
                else if (minutesAvailable[n] >= totalWork && totalWork <= requiredMinutes){
                    staff[n] = true;                                           // staff n can work on that delivery on that date
                    staff_oneday[n] = true;                                    // this staff has done this job this day
                    minutesAvailable[n] = minutesAvailable[n] - totalWork;       // update the total working time for all staff
                    totalWork = 0;                                              // set total work to 0
                    if(requiredPeople != 0)
                        --requiredPeople;
                    schedule.at(index).setMinutesAvailable(minutesAvailable); // update minutes available for staff n
                }
            }

            // the required preparing time can be fit into the delivery staff's working schedule
            if (totalWork == 0)
                dateStartFound = 1; // the delivery has been fit into the schedule. exit the while loop
            // the assigned staff do not have enough time on that day to finish the delivery. move the start date back one day
            else{

                // find a delivery staff who still has some available time left.
                for(int m = 0; m < 3; m ++){ // go through three staffs
                    if(requiredPeople == 0 && staff[m] == true && staff_oneday[m] == false && minutesAvailable[m] != 0){ // if the staff is one of the assigned staffs and he has not
                        totalWork -= minutesAvailable[m];                                                                // working on this delivery today yet, but he still has some left time
                        minutesAvailable[m] = 0;                                                                        // upodate the total man-hours needed and the available minute the staffs
                        schedule.at(index).setMinutesAvailable(minutesAvailable);                                        // have left

                    }
                    if(requiredPeople != 0 && staff[m] == false && staff_oneday[m] == false && minutesAvailable[m] != 0){// if the staff has not been assigned to the delivery job yet and he still
                        totalWork -= minutesAvailable[m];                                                               // has some time left at the day
                        minutesAvailable[m] = 0;
                        schedule.at(index).setMinutesAvailable(minutesAvailable);                                       // update the available time the staff has left
                        staff[m] = true;                                                                                // and set staff to be one of the assigned staffs
                        --requiredPeople;
                    }

                }

                datePlusDays(&dateStartTemp, -1);                                                                       // if the loop goes through to this point, meaning that the job still has work left to do
                strftime(bufferDateStart, sizeof(bufferDateStart), "%d/%m/%Y", &dateStartTemp);                         // so we push the required start date forward by one day
                dateStart = bufferDateStart;
            }
        }
    }
    if(set == 0) currentDeliveries[count].set_Staff(staff);// if the assigned staffs are being set for the first time, then set the assigned staffs
    currentDeliveries[count].set_DateStart(dateStart);      // update the calcaulted required start date
    return;
}
