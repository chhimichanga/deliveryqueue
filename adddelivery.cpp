#include "adddelivery.h"
#include "deliveryqueue.h"
#include "ui_frmAddDelivery.h"
#include "algorithm.h"
#include "delivery.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdlib.h>

#define MAX_DELIVERIES 1000 // maximum number of deliveries

// creates a form to add a delivery
frmAddDelivery::frmAddDelivery(QWidget *parent) :
    QWidget(parent), addui(new Ui::frmAddDelivery)
{
    addui->setupUi(this);

    // store input boxes into public variables for public access
    ledTransmittal      = addui->ledTransmittal;
    ledECN              = addui->ledECN;
    cboDestination      = addui->cboDestination;
    cboClassification   = addui->cboClassification;
    cboTransitMethod    = addui->cboTransitMethod;
    cboMediaType        = addui->cboMediaType;
    cboShipHull         = addui->cboShipHull;
    dteDeliveryDate     = addui->dteDeliveryDate;
    spnNumberOfItems    = addui->spnNumberItems;

    // connect add delivery ui buttons and slots
    connect(addui->btnSubmit, &QPushButton::clicked, this, &frmAddDelivery::submit);
    connect(addui->btnCancel, &QPushButton::clicked, this, &frmAddDelivery::cancel);
}

// save delivery, then send to queue
void frmAddDelivery::submit(){
    QFile fileIn("save.csv");       // load save file to write to
    QTextStream fileOut(&fileIn);   // load output stream

    // variables for reading each delivery's information
    QString line,
            transmittalID   = addui->ledTransmittal->text(),
            destination     = addui->cboDestination->currentText(),
            transitMethod   = addui->cboTransitMethod->currentText(),
            shipHull        = addui->cboShipHull->currentText(),
            ECN             = addui->ledECN->text(),
            mediaType       = addui->cboMediaType->currentText(),
            classification  = addui->cboClassification->currentText(),
            staffingLevel   = addui->cboStaffingLevel->currentText(),
            deliveryDate    = addui->dteDeliveryDate->date().toString("dd/MM/yyyy");
    int     numItems        = addui->spnNumberItems->value();

    // error message if file cannot open
    if(!fileIn.open(QIODevice::ReadWrite |  QIODevice::Append | QIODevice::Text))
        QMessageBox::information(this, "Error", "Cannot open save file for current deliveries.");
    else{     // add delivery to save file
        // if file is empty, create headers before adding more data
        if(fileIn.pos() == 0){
            QString header = "Transmittal #, Destination, Transit Method, Ship Name/Hull #, ECN/TECN, Media Type, # of Items, Classification, Staffing Level, Delivery Date, Ship Date, Start Date, Assigned Staff";
            fileOut << header << endl;   // send header to save file
        }

        // if duplicate transmission ID not found, continue writing new delivery to save file
        if(!duplicateFound(fileIn)){
            QString strDelivery;                                                        // string to add to save file
            strDelivery += addui->ledTransmittal->text() + ',';                         // unique transmittal #
            strDelivery += addui->cboDestination->currentText() + ',';                  // location
            strDelivery += addui->cboTransitMethod->currentText() + ',';                // transit method
            strDelivery += addui->cboShipHull->currentText() + ',';                     // ship name & hull #
            strDelivery += addui->ledECN->text() + ',';                                 // ECN
            strDelivery += addui->cboMediaType->currentText() + ',';                    // media type
            strDelivery += QString::number(addui->spnNumberItems->value()) + ',';       // number of items
            strDelivery += addui->cboClassification->currentText() + ',';               // classification
            strDelivery += addui->cboStaffingLevel->currentText() + ',';                // staffing level
            strDelivery += addui->dteDeliveryDate->date().toString("dd/MM/yyyy") + ','; // delivery date

            // move to end of file
            while(!fileIn.atEnd())
                fileIn.readLine();

            // send delivery to save file
            fileOut << strDelivery << endl;

            // close file, get ship date, then open again (within this function)
            fileIn.close();
            Delivery currentDeliveries[MAX_DELIVERIES]; // array of current deliveries
            Algorithm algorithm(currentDeliveries);     // declare new instance of Algorithm, pass in current deliveries
            fileIn.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text);

            // check ship date to determine need for planning alert
            fileIn.seek(0);                                 // move cursor to beginning of save.csv
            while(!fileIn.atEnd()){                         // read until reach end of file
                line = fileIn.readLine();                   // store line
                QStringList lineList = line.split(",");     // parse line using comma delimiter

                // if selected delivery's ID and the ID being read from the file match, throw planning alert
                if(transmittalID.compare(lineList.at(0)) == 0){
                    // if there are 7 or less days between today and ship date, show planning alert
                    if(QDate::currentDate().daysTo(QDate::fromString(lineList.at(10), "dd/MM/yyyy")) <= 7)
                        QMessageBox::information(this, "Planning Alert", "The delivery you have submitted has 7 or less days to ship!");
                    break;
                }
            }
            fileIn.close();

            // display success dialog after delivery is added
            QMessageBox::information(this, "Success", "Successfully submitted a delivery.");
        }
    }
}

void frmAddDelivery::cancel(){
    close();
}

// compare transmittal ID being added to existing transmittal IDs
bool frmAddDelivery::duplicateFound(QFile &fileIn){
    QString line, transmissionID;
    fileIn.seek(0);

    // don't throw error for blank transmittal IDs
    if(addui->ledTransmittal->text() == "")
        return false;
    else{
        // read to end of file
        while(!fileIn.atEnd()){
            line = fileIn.readLine();                   // store line
            QStringList lineList = line.split(",");     // split line
            transmissionID = lineList.at(0);            // read line until first comma

            // if selected delivery's ID and the ID being read from the file match, throw error
            if(transmissionID == addui->ledTransmittal->text()){
                QMessageBox::information(this, "Duplicate Transmittal ID", "There is already a delivery with the Transmission ID you entered. Please enter a unique Transmission ID.");
                return true;
            }
        }
    }

    return false;
}

frmAddDelivery::~frmAddDelivery(){
    delete addui;
}
