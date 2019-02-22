#include "delivery.h"

Delivery::Delivery(){   // initialize Delivery variables
    transmissionN = '\0';
    shipName = '\0';
    ECN = '\0';
    dateDeliver = '\0';
    location = '\0';
    classification = '\0';
    mediaType = '\0';
    shippingMethod= '\0';
    dateShip = '\0';
    dateStart = '\0';
    numItems = 0;
    staffinglevel = 1;
}

// all setter functions for each Delivery variable

void Delivery::set_ShipName(string shipnameNew){
    shipName = shipnameNew;

}
void Delivery::set_Transmission(string transmissionNew){
    transmissionN = transmissionNew;
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

void Delivery::set_ShippingMethod(string shippingMethodNew){
    shippingMethod = shippingMethodNew;
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
