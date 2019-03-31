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

// save delivery, then send to queue

void frmAddDelivery::submit()
{
    // load save file to write to
    QFile fileIn("save.csv");
    QTextStream fileOut(&fileIn);     // load output stream

    // error message if file cannot open
    if(!fileIn.open(QIODevice::ReadWrite |  QIODevice::Append | QIODevice::Text))
        QMessageBox::information(this, "Error", "Cannot open save file for current deliveries.");
    else{     // add delivery to save file
        // if file is empty, create headers before adding more data
        if(fileIn.pos() == 0){
            QString header = "Transmission #,Ship Name & Hull #,Engineering Change #,Media Type,Location,Transit Method,Number of items,Classification,Staffing Level,Required Delivery Date,Required Ship Date,Required Start Date";
            fileOut << header << endl;   // send header to save file
        } else {
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

            fileOut << strDelivery << endl;  // send delivery to save file

            QMessageBox::information(this, "Success", "Successfully submitted a delivery.");


        }
    }
}

void frmAddDelivery::cancel()
{
    close();
}

frmAddDelivery::~frmAddDelivery()
{
    delete addui;
}
