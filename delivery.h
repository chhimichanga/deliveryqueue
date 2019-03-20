#ifndef DELIVERY_H
#define DELIVERY_H
#include <string>

using namespace std;

class Delivery  // delivery variables
{
   string   transmissionNumber,
            location,
            transitMethod,
            shipNameHullNumber,
            ECN,
            mediaType,
            classification,
            dateDeliver,
            dateShip,
            dateStart,
            staff;
   string   *staff_name;

   int      numItems,
            staffinglevel;

public:
   // delivery constructor
   Delivery();

   // Set delivery variables
   void   set_TransmissionNumber(string transmissionNew);
   void   set_Location(string locationNew);
   void   set_TransitMethod(string transitMethodNew);
   void   set_ShipNameHullNumber(string shipNameNew);
   void   set_ECN(string ecnNew);
   void   set_MediaType(string mediaTypeNew);
   void   set_NumItems(int numItemsNew);
   void   set_Classification(string classificationNew);
   void   set_StaffingLevel(int staffingLevelNew);
   void   set_DateDeliver(string dateDeliverNew);
   void   set_DateShip(string dateShipNew);
   void   set_DateStart(string dateStartNew);
   void   set_Staff(bool *staff);

   // Get delivery variables
   string get_TransmissionNumber() {return transmissionNumber;}
   string get_Location() {return location;}
   string get_TransitMethod() {return transitMethod;}
   string get_ShipNameHullNumber() {return shipNameHullNumber;}
   string get_ECN() {return ECN;}
   string get_MediaType() {return mediaType;}
   int    get_NumItems() {return numItems;}
   string get_Classification() {return classification;}
   int    get_StaffingLevel() {return staffinglevel;}
   string get_DateDeliver() {return dateDeliver;}
   string get_DateShip() {return dateShip;}
   string get_DateStart() {return dateStart;}
   string  get_Staff(){return staff;}
   void   print_delivery_info();
};
#endif // DELIVERY_H
