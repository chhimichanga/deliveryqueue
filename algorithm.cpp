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

Algorithm::Algorithm(Delivery *currentDeliveries)
{
    count = 0;
    string temp;         // temorary storage for reading

    // load save file
    ifstream file("save.csv");
    getline(file, temp); // skip the first line of headers

    vector<Schedule> schedule;  // vector of schedules
    loadSchedule();

    while(getline(file, temp)){ // read file until reach the end
        istringstream iss(temp);
        string token;
        int fieldNumber = 0;
        while(getline(iss, token, ',')){    // each line gets parsed with every field separated by a comma
            if(fieldNumber == 0){
                currentDeliveries[count].set_TransmissionNumber(token);
            }else if(fieldNumber == 1){
                currentDeliveries[count].set_Location(token);               // set location
            }else if(fieldNumber == 2){
                currentDeliveries[count].set_TransitMethod(token);         // set shipping method
            }else if(fieldNumber == 3){
                currentDeliveries[count].set_ShipNameHullNumber(token);               // set required delivery date
            }else if(fieldNumber == 4){
                currentDeliveries[count].set_ECN(token);                    // set required delivery date
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
            }
            fieldNumber++;    // move on to next field in line
        }
        currentDeliveries[count].set_DateShip(calculateDateShip(currentDeliveries[count].get_DateDeliver(), currentDeliveries[count].get_Location()));
        currentDeliveries[count].set_DateStart(calculateDateStart(currentDeliveries[count].get_DateShip(), currentDeliveries[count].get_NumItems(), currentDeliveries[count].get_StaffingLevel()));
        count++;   // move on to next line in file
    }
    ofstream fileOut;
    fileOut.open("temp.csv"); // load output stream
    fileOut << "Transmission #,Ship Nmae & Hull #,Engineering Change #,Media Type,Location,Transit Method,Number of items,Classification,Staffing Level,Required Delivery Date,"
            "Required Ship Date,Required Start Date" << endl;
    for(int i = 0; i < count; i++){
        string strDelivery; // string to add to save file
        // append edited delivery information to string

        strDelivery += currentDeliveries[i].get_TransmissionNumber() + ',';                         // unique Transmission #
        strDelivery += currentDeliveries[i].get_Location() + ',';                     // location
        strDelivery += currentDeliveries[i].get_TransitMethod() + ',';                     // shipping method
        strDelivery += currentDeliveries[i].get_ShipNameHullNumber() + ',';                      // ship name & hull #
        strDelivery += currentDeliveries[i].get_ECN() + ',';                                  // ECN
        strDelivery += currentDeliveries[i].get_MediaType() + ',';                    // media type
        strDelivery += to_string(currentDeliveries[i].get_NumItems()) + ',';     // number of items
        strDelivery += currentDeliveries[i].get_Classification() + ',';               // classification
        strDelivery += to_string(currentDeliveries[i].get_StaffingLevel()) + ',';               // staffing level
        strDelivery += currentDeliveries[i].get_DateDeliver() + ','; // delivery date
        strDelivery += currentDeliveries[i].get_DateShip() + ",";
        strDelivery += currentDeliveries[i].get_DateStart() + ",";
        fileOut << strDelivery << endl;    // send delivery to save file

    }
    fileOut.close();
    file.close();

    // delete original file
    remove("save.csv");

    // rename temp.csv to save.csv
    rename("temp.csv","save.csv");

}

int Algorithm::getCount(){
    return count;
}
void Algorithm::loadSchedule(){
    QString currentDate = QDate::currentDate().toString("dd/MM/yyyy");

    if(QFileInfo("shcedule.txt").exists() && !QDir("shcedule.txt").exists()){
        //The file exists and is not a folder
        ifstream fileIn("schedule.txt");
        if(!fileIn.is_open())
            MessageBoxA(NULL, "Cannot open schedule file.", "error", MB_OK);
        else { // add delivery to save file
            fileIn.seekg(0);        // move cursor to beginning of save.csv
            string line, token, date;

            getline(fileIn, line);
            istringstream iss(line);
            getline(iss, token, ',');//read the date
            date = token;
            if(date == currentDate.toStdString()){
                int i = 0;
                while(getline(iss, token, ',')){
                    QDate issueDate = QDate::currentDate().addDays(i);
                    Schedule newSchedule(issueDate.toString("dd/MM/yyyy").toStdString());
                    newSchedule.setStaffingLevel(stoi(token));
                    schedule.push_back(newSchedule);
                    i++;
                }
            }
            else{
                string write;
                ofstream fileOut;
                fileOut.open("tempSchedule.txt"); // load output stream
                getline(iss, token, ',');//skip the first number
                write = currentDate.toStdString() + iss.str() + "3,";
                int i = 0;
                while(getline(iss, token, ',')){
                    QDate issueDate = QDate::currentDate().addDays(i);
                    Schedule newSchedule(issueDate.toString("dd/MM/yyyy").toStdString());
                    newSchedule.setStaffingLevel(stoi(token));
                    schedule.push_back(newSchedule);
                    i++;
                }
                Schedule newSchedule(QDate::currentDate().addDays(179).toString("dd/mm/yyyy").toStdString());
                newSchedule.setStaffingLevel(stoi(token));
                schedule.push_back(newSchedule);
                i++;
                fileOut << write << endl;
                fileOut.close();

            }

        }

        fileIn.close();
    }
    else{
        //The file doesn't exist, either the path doesn't exist or is the path of a folder
        //create a new file that contains a date and 180 "3," string

        string write;
        ofstream fileOut("schedule.txt");
        write += currentDate.toStdString() + ",";
        for(int i = 0; i < 180; i++){
            write += "3,";
            QDate issueDate = QDate::currentDate().addDays(i);
            Schedule newSchedule(issueDate.toString("dd/MM/yyyy").toStdString());
            schedule.push_back(newSchedule);
        }
        fileOut << write << endl;
        fileOut.close();
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
string Algorithm::calculateDateShip(string dateDeliver, string Location){
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

    return dateShip;
}

string Algorithm::calculateDateStart(string dateShip, int itemCount, int staffingLevel){
    string dateShipTemp = dateShip;  // store the calculated shipping date in a variable
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

            // go through each staff's working schedule for the day being checked
            bool staffAvailable[3] = {false, false, false};
            for(int n = 0; n < 3; n++){
                // the required preparing time can be fit into the delivery staff's working schedule
                if (requiredPeople == 0 || totalWork == 0)
                    break;

                // worker has enough time to work on the delivery
                if (minutesAvailable[n] >= requiredMinutes /*&& totalWork >= requiredMinutes*/){
                    staffAvailable[n] = true;                                   // staff n can work on that delivery on that date
                    minutesAvailable[n] -= requiredMinutes;                     // update staff n's schedule
                    totalWork -= requiredMinutes;                               // update the total working time for all staff
                    --requiredPeople;                                           // done assigning job to one staff
                    schedule.at(index).setMinutesAvailable(minutesAvailable);   // update minutes available for staff n
                }
                // worker can finish the rest of the work
                else if (minutesAvailable[n] >= totalWork && totalWork <= requiredMinutes){
                    staffAvailable[n] = true;
                    minutesAvailable[n] = minutesAvailable[n] - totalWork;
                    totalWork = 0;                                              // set total work to 0
                    --requiredPeople;
                    schedule.at(index).setMinutesAvailable(minutesAvailable);
                }
            }

            // the required preparing time can be fit into the delivery staff's working schedule
            if (requiredPeople == 0 || totalWork == 0)
                dateStartFound = 1; // the delivery has been fit into the schedule. exit the while loop
            // the assigned staff do not have enough time on that day to finish the delivery. move the start date back one day
            else{
                int requiredPeopleTemp = requiredPeople;

                // find a delivery staff who still has some available time left.
                for(int m = 0; m < 3; m ++){
                    if(staffAvailable[m] == false && minutesAvailable[m] != 0){
                        totalWork -= minutesAvailable[m];
                        minutesAvailable[m] = 0;
                        schedule.at(index).setMinutesAvailable(minutesAvailable);
                        staffAvailable[m] = true;
                        --requiredPeople;
                    }
                    // the delivery can be fit into the schedule
                    if(requiredPeople == 0)
                        break;
                }
                requiredPeople = requiredPeopleTemp;
                datePlusDays(&dateStartTemp, -1);
                strftime(bufferDateStart, sizeof(bufferDateStart), "%d/%m/%Y", &dateStartTemp);
                dateStart = bufferDateStart;
            }
        }
    }

    return dateStart;
}
