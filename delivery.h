#ifndef DELIVERY_H
#define DELIVERY_H
#include <string>

using namespace std;

class Delivery  // delivery variables
{
   string id,
          dateDeliver,
          location,
          classification,
          mediaType,
          shippingMethod,
          dateShip,
          dateStart;
   int    numItems;

public:
   // delivery constructor
   Delivery();

   // Set delivery variables
   void   set_ID(string idNew);
   void   set_DateDeliver(string dateDeliverNew);
   void   set_Location(string locationNew);
   void   set_Classification(string classificationNew);
   void   set_MediaType(string mediaTypeNew);
   void   set_ShippingMethod(string shippingMethodNew);
   void   set_DateShip(string dateShipNew);
   void   set_DateStart(string dateStartNew);
   void   set_NumItems(int numItemsNew);

   // Get delivery variables
   string get_ID() {return id;}
   string get_DateDeliver() {return dateDeliver;}
   string get_Location() {return location;}
   string get_Classification() {return classification;}
   string get_MediaType() {return mediaType;}
   string get_ShippingMethod() {return shippingMethod;}
   string get_DateShip() {return dateShip;}
   string get_DateStart() {return dateStart;}
   int    get_NumItems() {return numItems;}
   void   print_delivery_info();
};
#endif // DELIVERY_H
