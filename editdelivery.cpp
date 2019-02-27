#include "editdelivery.h"
#include "ui_frmEditDelivery.h"
#include <fstream>
#include <sstream>
#include <QtWidgets>

// create form to edit a delivery
frmEditDelivery::frmEditDelivery(QWidget *parent) :
    QWidget(parent), editui(new Ui::frmEditDelivery)
{
    editui->setupUi(this);

    // store input boxes into public variables for public access
    cboShipHull      = editui->cboShipHull;
    ledECN             = editui->ledECN;
    ledTransmission    = editui->ledTransmission;
    cboLocation        = editui->cboLocation;
    cboClassification  = editui->cboClassification;
    cboTransitMethod        = editui->cboShipping;
    cboMediaType       = editui->cboMediaType;
    cboStaffing        = editui->cboStaffing;
    dteDeliveryDate    = editui->dteDeliveryDate;
    spnNumberOfItems   = editui->spnNumberObjects;

    // connect edit delivery ui buttons and slots
    connect(editui->btnSubmit, &QPushButton::clicked, this, &frmEditDelivery::submit);
    connect(editui->btnCancel, &QPushButton::clicked, this, &frmEditDelivery::cancel);
}

void frmEditDelivery::submit()
{
    string line, id;   // strings to read lines

    // load save file to write to
    ifstream fileIn("save.csv");
    ofstream fileOut;
    fileOut.open("temp.csv"); // load output stream

    // error message if file cannot open
    if(!fileOut.is_open())
        QMessageBox::information(this, "Error", "Cannot open temp file for current deliveries.");
    else    // add header to temp file
        fileOut << "Transmission #,Ship Nmae & Hull #,Engineering Change #,Media Type,Location,Transit Method,Number of items,Classification,Staffing Level,Required Delivery Date,"
                "Required Ship Date,Required Start Date" << endl;

    // error message if file cannot open
    if(!fileIn.is_open())
        QMessageBox::information(this, "Error", "Cannot open save file for current deliveries.");
    else { // add delivery to save file
        fileIn.seekg(0);        // move cursor to beginning of save.csv
        getline(fileIn, line);  // skip header line

        while(getline(fileIn, line)){       // read until reach end of file
            istringstream stringIn(line);   // feed line to stream
            getline(stringIn, id, ',');     // parse line using comma delimiter, extract ID
            if(id != transmissionNumber)          // if not the delivery being edited,
                fileOut << line << endl;        // write line to temp file
        }

        string strDelivery; // string to add to save file
        // append edited delivery information to string

        strDelivery += editui->ledTransmission->text().toStdString() + ',';                         // unique Transmission #
        strDelivery += editui->cboLocation->currentText().toStdString() + ',';                     // location
        strDelivery += editui->cboShipping->currentText().toStdString() + ',';                     // shipping method
        strDelivery += editui->cboShipHull->currentText().toStdString() + ',';                      // ship name & hull #
        strDelivery += editui->ledECN->text().toStdString() + ',';                                  // ECN
        strDelivery += editui->cboMediaType->currentText().toStdString() + ',';                    // media type
        strDelivery += QString::number(editui->spnNumberObjects->value()).toStdString() + ',';     // number of items
        strDelivery += editui->cboClassification->currentText().toStdString() + ',';               // classification
        strDelivery += editui->cboStaffing->currentText().toStdString() + ',';               // staffing level
        strDelivery += editui->dteDeliveryDate->date().toString("dd/MM/yyyy").toStdString() + ','; // delivery date
        fileOut << strDelivery << endl;    // send delivery to save file

        // close both files
        fileOut.close();
        fileIn.close();

        // delete original file
        remove("save.csv");

        // rename temp.csv to save.csv
        rename("temp.csv","save.csv");

        QMessageBox::information(this, "Success", "Successfully edited a delivery.");
    }
}

void frmEditDelivery::cancel()
{
    close();
}

frmEditDelivery::~frmEditDelivery()
{
    delete editui;
}
