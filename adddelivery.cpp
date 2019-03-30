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

#define MAX_DELIVERIES 100 // maximum number of deliveries

// creates a form to add a delivery
frmAddDelivery::frmAddDelivery(QWidget *parent) :
    QWidget(parent), addui(new Ui::frmAddDelivery)
{
    addui->setupUi(this);

    // store input boxes into public variables for public access
    transmission    = addui->ledTransmission;
    ECN             = addui->ledECN;
    location        = addui->cboLocation;
    classification  = addui->cboClassification;
    shipping        = addui->cboTransitMethod;
    mediaType       = addui->cboMediaType;
    shipnumber      = addui->cboShipHull;
    deliveryDate    = addui->dteDeliveryDate;
    numberOfItems   = addui->spnNumberObjects;

    // connect add delivery ui buttons and slots
    connect(addui->btnSubmit, &QPushButton::clicked, this, &frmAddDelivery::submit);
    connect(addui->btnCancel, &QPushButton::clicked, this, &frmAddDelivery::cancel);
}

// save delivery, then send to queue TODO DUPLICATE TRANSMISSION ID
void frmAddDelivery::submit(){
    // load save file to write to
    QFile fileIn("save.csv");
    QTextStream fileOut(&fileIn);   // load output stream
    QString line, transmissionID;   // strings for reading each line and transmission ID

    // error message if file cannot open
    if(!fileIn.open(QIODevice::ReadWrite |  QIODevice::Append | QIODevice::Text))
        QMessageBox::information(this, "Error", "Cannot open save file for current deliveries.");
    else{     // add delivery to save file
        // if file is empty, create headers before adding more data
        if(fileIn.pos() == 0){
            QString header = "Transmission #,Ship Name & Hull #,Engineering Change #,Media Type,Location,Transit Method,Number of items,Classification,Staffing Level,Required Delivery Date,Required Ship Date,Required Start Date";
            fileOut << header << endl;   // send header to save file
        }

        // if duplicate transmission ID not found, continue writing new delivery to save file
        if(!duplicateFound(fileIn)){
            QString strDelivery;    // string to add to save file
            strDelivery += addui->ledTransmission->text() + ',';                        // unique Transmission #
            strDelivery += addui->cboLocation->currentText() + ',';                     // location
            strDelivery += addui->cboTransitMethod->currentText() + ',';                // transit method
            strDelivery += addui->cboShipHull->currentText() + ',';                     // ship name & hull #
            strDelivery += addui->ledECN->text() + ',';                                 // ECN
            strDelivery += addui->cboMediaType->currentText() + ',';                    // media type
            strDelivery += QString::number(addui->spnNumberObjects->value()) + ',';     // number of items
            strDelivery += addui->cboClassification->currentText() + ',';               // classification
            strDelivery += addui->cboStaffing->currentText() + ',';                     // staffing level
            strDelivery += addui->dteDeliveryDate->date().toString("dd/MM/yyyy") + ','; // delivery date

            // move to end of file
            while(!fileIn.atEnd())
                fileIn.readLine();

            // send delivery to save file
            fileOut << strDelivery << endl;

            // close file, get ship date, then open again (within this function)
            fileIn.close();
            Delivery currentDeliveries[MAX_DELIVERIES]; // array of current deliveries
            Algorithm algorithm(currentDeliveries); // declare new instance of Algorithm, pass in current deliveries
            fileIn.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text);

            // check ship date to determine need for planning alert
            fileIn.seek(0);
            while(!fileIn.atEnd()){
                line = fileIn.readLine();   // store line
                QStringList lineList = line.split(",");    // split line
                transmissionID = lineList.at(0);     // read line until first comma

                // if selected delivery's ID and the ID being read from the file match, edit that delivery
                if(transmissionID == addui->ledTransmission->text()){
                    // if there are 7 or less days between today and ship date, show planning alert
                    if(QDate::currentDate().daysTo(QDate::fromString(lineList.at(10), "dd/MM/yyyy")) <= 7)
                        QMessageBox::information(this, "Planning Alert", "The delivery you have submitted has 7 or less days to ship!");
                    break;
                }
            }
            fileIn.close();

            QMessageBox::information(this, "Success", "Successfully submitted a delivery.");

        }
    }
}

void frmAddDelivery::cancel(){
    close();
}

// compare transmission id in form to other transmission ID
bool frmAddDelivery::duplicateFound(QFile &fileIn){
    QString line, transmissionID;
    fileIn.seek(0);

    while(!fileIn.atEnd()){
        line = fileIn.readLine();   // store line
        QStringList lineList = line.split(",");    // split line
        transmissionID = lineList.at(0);     // read line until first comma

        // if selected delivery's ID and the ID being read from the file match, edit that delivery
        if(transmissionID == addui->ledTransmission->text()){
            QMessageBox::information(this, "Duplicate Transmission ID", "There is already a delivery with the Transmission ID you entered. Please enter a unique Transmission ID.");
            return true;
        }
    }

    return false;
}

frmAddDelivery::~frmAddDelivery(){
    delete addui;
}
