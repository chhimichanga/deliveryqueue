#ifndef DELIVERY_H
#define DELIVERY_H
#include <string>

using namespace std;

class Delivery  // delivery variables
{
   string   dateDeliver,
            location,
            classification,
            mediaType,
            shippingMethod,
            dateShip,
            dateStart,
            transmissionN,
            shipName,
            ECN;
   int      numItems,
            staffinglevel;

public:
   // delivery constructor
   Delivery();

   // Set delivery variables
   void   set_DateDeliver(string dateDeliverNew);
   void   set_Location(string locationNew);
   void   set_Classification(string classificationNew);
   void   set_MediaType(string mediaTypeNew);
   void   set_ShippingMethod(string shippingMethodNew);
   void   set_DateShip(string dateShipNew);
   void   set_DateStart(string dateStartNew);
   void   set_Transmission(string transmissionNew);
   void   set_ECN(string ecnNew);
   void   set_ShipName(string shipnameNew);
   void   set_NumItems(int numItemsNew);
   void   set_StaffingLevel(int staffinglevelNew);

   // Get delivery variables
   string get_Transmission() {return transmissionN;}
   string get_DateDeliver() {return dateDeliver;}
   string get_Location() {return location;}
   string get_Classification() {return classification;}
   string get_MediaType() {return mediaType;}
   string get_ShippingMethod() {return shippingMethod;}
   string get_DateShip() {return dateShip;}
   string get_DateStart() {return dateStart;}
   string get_ECN() {return ECN;}
   string get_ShipName() {return shipName;}
   int    get_NumItems() {return numItems;}
   int    get_StaffingLevel() {return staffinglevel;}
   void   print_delivery_info();
};
#endif // DELIVERY_H
