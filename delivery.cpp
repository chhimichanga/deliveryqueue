#include "delivery.h"

Delivery::Delivery(){   // initialize Delivery variables
    transmissionNumber = '\0';
    location = '\0';
    transitMethod= '\0';
    shipNameHullNumber = '\0';
    ECN = '\0';
    mediaType = '\0';
    numItems = 0;
    classification = '\0';
    staffinglevel = 1;
    dateDeliver = '\0';
    dateShip = '\0';
    dateStart = '\0';
    staff = '\0';
    staff_name = new string[3]{"mike", "michael", "bruce"};


}

// all setter functions for each Delivery variable

void Delivery::set_ShipNameHullNumber(string shipnameNew){
    shipNameHullNumber = shipnameNew;

}

void Delivery::set_TransmissionNumber(string transmissionNew){
    transmissionNumber = transmissionNew;
}

void Delivery::set_ECN(string ecnNew){
    ECN = ecnNew;
}

void Delivery::set_DateDeliver(string dateDeliverNew){
    dateDeliver = dateDeliverNew;
}

void Delivery::set_Location(string locationNew){
    location = locationNew;
}

void  Delivery::set_Classification(string classificationNew){
    classification = classificationNew;
}

void Delivery::set_MediaType(string mediaTypeNew){
    mediaType = mediaTypeNew;
}

void Delivery::set_TransitMethod(string shippingMethodNew){
    transitMethod = shippingMethodNew;
}

void Delivery::set_DateShip(string dateShipNew){
    dateShip = dateShipNew;
}

void Delivery::set_DateStart(string dateStartNew){
    dateStart = dateStartNew;
}

void Delivery::set_NumItems(int numItemsNew){
    numItems = numItemsNew;
}

void Delivery::set_StaffingLevel(int staffinglevelNew){
    staffinglevel = staffinglevelNew;
}
void Delivery::set_Staff(bool *bool_staff){

    for(int i = 0; i < 3; i++){
        if(bool_staff[i] == true){
            staff += staff_name[i] + ";";
        }
    }
}
void   Delivery::set_Staff(string staFF){
    staff = staFF;
}
