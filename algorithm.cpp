#include "algorithm.h"
#include <string>
#include <delivery.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <ctime>
#include <QDebug>

using namespace std;

Algorithm::Algorithm(Delivery *currentDeliveries)
{
    count = 0;

    // load save file
    ifstream file("C:/Users/thekn/Documents/DeliveryQueueMaster/save.csv");
    //if (!file.is_open()) {
    //  cout << "Error opening file";
    //}
    string str;         // temorary storage for reading
    getline(file, str); // skip the first line of headers

    while(getline(file, str)){ // read until reach end of file
        istringstream iss(str);
        string token;
        int j = 0;
        while(getline(iss, token, ',')){    // while delimiter is being read

            if(j==0) currentDeliveries[count].set_ID(token);                // set Delivery ID
            else if(j==1) currentDeliveries[count].set_DateDeliver(token);       // set required delivery date
            else if(j==2) currentDeliveries[count].set_Location(token);          // set location
            else if(j==3) currentDeliveries[count].set_ShippingMethod(token);
            else if(j==4) currentDeliveries[count].set_Classification(token);    // set classification
            else if(j==5){
                stringstream geek(token);
                int x = 0;
                geek >> x;
                currentDeliveries[count].set_NumItems(x);                       // set number of items
            }
            else if(j==6) currentDeliveries[count].set_MediaType(token);        // set media type
            //qDebug() << j;
            j++;    // move on to next field in line
        }
        //qDebug() << QString::fromStdString(currentDeliveries[count].get_DateDeliver()) +" "+ QString::fromStdString(currentDeliveries[count].get_Location()) +" "+ QString::fromStdString(CalculateDateShip(currentDeliveries[count].get_DateDeliver(), currentDeliveries[count].get_Location()));
        currentDeliveries[count].set_DateShip(CalculateDateShip(currentDeliveries[count].get_DateDeliver(), currentDeliveries[count].get_Location()));
        count++;   // move on to next line in file
    }

    Sorting(currentDeliveries, count);// sort the delivery queue based on ship date

}
int Algorithm::get_Count(){
    return count;
}

void Algorithm::Sorting(Delivery data[], int count){

    for(int i = 0; i < count; i++){     // find a delivery that has a closest ship date in each loop

        int index = i;      // store the index of the delivery that has a closest ship date
        int nearest_date_day, nearest_date_month, nearest_date_year;    // store the closet date, month and year
        int top_classification = StringToValue(data[i].get_Classification()); // store the highest classification
        int max_num_items = data[i].get_NumItems(); // store the largest number of items
        int farest_Location = StringToValue(data[i].get_Location()); // store the farest location
        int most_valued_document = StringToValue(data[i].get_MediaType()); // store the most valued type of media
        istringstream rsd(data[i].get_DateShip());
        string token;
        int m = 0; 
        // separate the whole string of ship date into date, month and year
        while (getline(rsd, token, '/')){
            stringstream geek(token); // convert string to integer
            int x = 0;
            geek >> x; 
            if(m==0) nearest_date_day = x;  //   first string goes to day
            if(m==1) nearest_date_month = x; //  month
            if(m==2) nearest_date_year = x;   // year
            m++;
        }

        for(int j = i + 1; j < count; j++){  // compare the delivery at i-th index with the rest of the deliveries
            int next_date_day, next_date_month, next_date_year; // store the next delivery data for comparsion later
            int next_classification = StringToValue(data[j].get_Classification());
            int next_num_items = data[j].get_NumItems();
            int next_Location = StringToValue(data[j].get_Location());
            int next_valued_document = StringToValue(data[j].get_MediaType());
            istringstream next_rsd(data[j].get_DateShip());
            string next_token;
            int n = 0;
             // separate the whole string of ship date into date, month and year
            while (getline(next_rsd, next_token, '/')){
                stringstream geek(next_token);
                int x = 0;
                geek >> x;
                if(n==0) next_date_day = x;
                if(n==1) next_date_month = x;
                if(n==2) next_date_year = x;
                n++;
            }
            // value ship date > classification > location     
            // do the comparsion, compare the year first, if found a closer date, store the index and swap with the delivery at i-th index 
            if(next_date_year < nearest_date_year) {
                nearest_date_day = next_date_day; 
                nearest_date_month = next_date_month;
                nearest_date_year = next_date_year;
                index = j;
            }
            else if(next_date_year == nearest_date_year && next_date_month < nearest_date_month){ // if it is the same year, compare the month
                nearest_date_day = next_date_day;
                nearest_date_month = next_date_month;
                nearest_date_year = next_date_year;
                index = j;
                // if it is the same month, compare the day
                if(next_date_year == nearest_date_year && next_date_month <= nearest_date_month && next_date_day < nearest_date_day){
                    nearest_date_day = next_date_day; 
                    nearest_date_month = next_date_month;
                    nearest_date_year = next_date_year;
                    index = j;

                }
                else if(next_date_year<= nearest_date_year && next_date_month <= nearest_date_month && next_date_day == nearest_date_day){
                    // if it is also the same day, compare the classification level
                    if(next_classification > top_classification){
                        top_classification = next_classification;
                        index = j;
                    }
                    else if(next_classification == top_classification){
                        if(next_Location > farest_Location){
                            farest_Location = next_Location;
                            index = j;
                        }
                        // if two deliveries have the exact same ship date and classification level, compare the location to determine which should go first
                        else if(next_Location == farest_Location){
                            if(next_num_items > max_num_items){
                                max_num_items = next_num_items;
                                index = j;
                            }
                             // if two deliveries have the exact same ship date, classification level and number of items, compare the number of items to determine which should go first
                            else if(next_num_items == max_num_items){
                                if(next_valued_document > most_valued_document){
                                    most_valued_document = next_valued_document;
                                    index = j;
                                }
                            }

                        }
                    }
                }
            }
        }
        // if the loop found a closer delivery, swap that delivery data with the delivery at i-th index
        if(index != i){
           // cout << "swap";
            Delivery swap = data[i];
            data[i] = data[index];
            data[index] = swap;

        }
    }
}

int Algorithm::StringToValue(string s){ // convert the string to numerical value for easier comparsion
    if(s == "Secret"|| s == "secret") return 3;
    else if(s == "Unclassified" ||s ==  "Unclassified") return 2;
    else if(s == "confidential" || s == "Confidential") return 1;
    else if(s == "Rota Spain"|| s == "Rota Spain") return 3;
    else if(s == "King's Bay Georgia" || s == "Area 51 Nevada"|| s == "Pensacola Florida"||s == "Great Lakes Illinois" || s == "Bremerton Washington"
            ||s == "King's Bay Georgia" || s == "b"|| s== "c"||s == "d" || s == "e") return 2;
    else if(s == "New London Connecticut" || s == "local") return 1;
    else if(s == "Documentation" || s== "documentation") return 1;
    else if(s == "Software") return 2;
    else if(s == "Hardware" || s== "Hardware") return 3;
    else return 0;
}

int Algorithm::DayOfTheWeek (int day, int month, int year){
    tm time_in = { 0, 0, 0, // second, minute, hour
        day, month, year}; // 1-based day, 0-based month, year since 1900

    time_t time_temp = mktime(&time_in);

    //Note: Return value of localtime is not threadsafe, because it might be
    // (and will be) reused in subsequent calls to std::localtime!
    const tm * time_out = localtime(&time_temp);
    if (time_out->tm_wday == 0) return 4; // if it is sunday moving the schedule forward by 4 days
    else if (time_out->tm_wday == 4) return 1;  // if it is thursday moving the schedule forward by 1 days
    else if (time_out->tm_wday == 5) return 2;   // if it is friday moving the schedule forward by 2 days
    else if (time_out->tm_wday == 6) return 3;   // if it is saturday moving the schedule forward by 3 days
    else return 0;
    //Sunday == 0, Monday == 1, and so on ...
 //   cout << "Today is this day of the week: " << time_out->tm_wday << "\n";
  //  cout << "(Sunday is 0, Monday is 1, and so on...)\n";

}

void Algorithm::DatePlusDays( struct tm* date, int days ) // do the calculation of date, plus or minus a number of days on a date
{
    const time_t ONE_DAY = 24 * 60 * 60 ;
    // Seconds since start of epoch
    time_t date_seconds = mktime( date ) + (days * ONE_DAY) ;

    // Update caller's date
    // Use localtime because mktime converts to UTC so may change date
    *date = *localtime( &date_seconds ) ; ;
}

string Algorithm::CalculateDateShip(string RDD, string Location){
    struct tm date = { 0, 0, 12 }; // nominal time midday (arbitrary).
    char buffer[80];
    string str; // return value

    if(Location == "Rota Spain" || Location == "Rota Spain"){ // if the location is outside US
        istringstream rdd(RDD);
        string token;
        int j = 0;
         // separate the whole string of ship date into date, month and year
        while (getline(rdd, token, '/')){
            stringstream geek(token);
            int x = 0;
            geek >> x;
            if(j==0) date.tm_mday = x;
            if(j==1) date.tm_mon = x - 1;
            if(j==2) date.tm_year = x - 1900;
            j++;

        }
        DatePlusDays(&date, -21);   // 3 weeks to arrive at location once shipped
        if(DayOfTheWeek(date.tm_mday, date.tm_mon, date.tm_year) != 0){   // if the ship date is not Monday, Tuesday nor Wednesday, move the shcedule forward
            DatePlusDays(&date, -DayOfTheWeek(date.tm_mday, date.tm_mon, date.tm_year));
        }
        // convert the date to string
        strftime(buffer,sizeof(buffer),"%d/%m/%Y", &date);
        str = buffer;
    }
    // if the location is in US
    else if(Location == "King's Bay Georgia" || Location == "Area 51 Nevada"|| Location == "Pensacola Florida"||Location == "Great Lakes Illinois" || Location == "Bremerton Washington"){
        istringstream rdd(RDD);
        string token;
        int j = 0;
        // separate the whole string of ship date into date, month and year
        while (getline(rdd, token, '/')){
            stringstream geek(token);
            int x = 0;
            geek >> x;
            if(j==0) date.tm_mday = x;
            if(j==1) date.tm_mon = x - 1;
            if(j==2) date.tm_year = x - 1900;
            j++;

        }

        DatePlusDays(&date, -7);    // 1 week to arrive at location once shipped
        // if the ship date is not Monday, Tuesday nor Wednesday, move the shcedule forward
        if(DayOfTheWeek(date.tm_mday, date.tm_mon, date.tm_year) != 0){
            DatePlusDays(&date, -DayOfTheWeek(date.tm_mday, date.tm_mon, date.tm_year));
        }
        // convert the date to string
        strftime(buffer,sizeof(buffer),"%d/%m/%Y", &date);
        str = buffer;
    }
    else{
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
        // if the ship date is not Monday, Tuesday nor Wednesday, move the shcedule forward
        if(DayOfTheWeek(date.tm_mday, date.tm_mon, date.tm_year) != 0){
            DatePlusDays(&date, -DayOfTheWeek(date.tm_mday, date.tm_mon, date.tm_year));
        }
        // convert the date to string
        strftime(buffer,sizeof(buffer),"%d/%m/%Y", &date);
        str = buffer;
        }

    return str;

}
