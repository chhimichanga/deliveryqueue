#include "adddelivery.h"
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

    // connect add delivery ui buttons and slots
    connect(addui->btnSubmit, &QPushButton::clicked, this, &frmAddDelivery::submit);
    connect(addui->btnCancel, &QPushButton::clicked, this, &frmAddDelivery::cancel);
}

// save delivery, then send to queue
void frmAddDelivery::submit()
{
    QString line, id;       // strings to read lines
    QStringList columns;    // string list to store tokens from line
    int intID = 0;

    // load save file to write to
    QFile fileIn("C:/Users/thekn/Documents/DeliveryQueueMaster/save.csv");
    QTextStream fileOut(&fileIn);     // load output stream

    // error message if file cannot open
    if(!fileIn.open(QIODevice::ReadWrite |  QIODevice::Append | QIODevice::Text))
        QMessageBox::information(this, "Error", "Cannot open save file for current deliveries.");
    else{     // add delivery to save file
        if(fileIn.pos() == 0){       // if file is empty, create headers before adding more data
            QString header = "ID,Required Delivery Date,Location,Shipping Method,Classification,Number of Items,Media Type,Required Ship Date,Required Start Date";
            fileOut << header << endl;   // send header to save file
        } else {
            fileIn.seek(0);      // move cursor to beginning of file
            fileIn.readLine();   // skip header line

            /*  find the largest delivery ID in the list of current deliveries
                this is important because when a delivery is edited, its ID stays the same,
                but the delivery gets moved to the end of the save file */
            while(!fileIn.atEnd()){
                line = fileIn.readLine();   // read line
                columns = line.split(",");  // split line into a string array
                id = columns.at(0);         // take first string in string array. this is the ID
                if(id.toInt() > intID)
                    intID = id.toInt();
            }

            // if there are no deliveries and only a header, set the new delivery's ID to 1
            if(id == nullptr)
                intID = 1;
            else    // otherwise, set new delivery's ID to the last added delivery's ID plus 1
                intID += 1;

            QString strDelivery;    // string to add to save file
            strDelivery += QString::number(intID) + ',';                                // unique ID
            strDelivery += addui->dteDeliveryDate->date().toString("dd/MM/yyyy") + ','; // delivery date
            strDelivery += addui->cboLocation->currentText() + ',';                     // location
            strDelivery += addui->cboShipping->currentText() + ',';                     // shipping method
            strDelivery += addui->cboClassification->currentText() + ',';               // classification
            strDelivery += QString::number(addui->spnNumberObjects->value()) + ',';     // number of items
            strDelivery += addui->cboMediaType->currentText() + ',';                    // media type
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
