#ifndef DELIVERY_H
#define DELIVERY_H
#include <string>

using namespace std;

class Delivery  // delivery variables
{
   string   transmittalNumber,
            destination,
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
   void   set_TransmittalNumber(string transmittalNew);
   void   set_Destination(string destinationNew);
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
   void   set_Staff(string staFF);

   // Get delivery variables
   string get_TransmittalNumber() {return transmittalNumber;}
   string get_Destination() {return destination;}
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
   string get_Staff() {return staff;}
   string*  get_StaffName() {return staff_name;}
   void   print_delivery_info();
};
#endif // DELIVERY_H
