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
    location = editui->cboLocation;
    classification = editui->cboClassification;
    shipping = editui->cboShipping;
    mediaType = editui->cboMediaType;
    staffing = editui->cboStaffing;
    deliveryDate = editui->dteDeliveryDate;
    numberOfItems = editui->spnNumberObjects;

    // connect edit delivery ui buttons and slots
    connect(editui->btnSubmit, &QPushButton::clicked, this, &frmEditDelivery::submit);
    connect(editui->btnCancel, &QPushButton::clicked, this, &frmEditDelivery::cancel);
}

void frmEditDelivery::submit()
{
    string line, id;   // strings to read lines
    QStringList columns;

    // load save file to write to
    ifstream fileIn("C:/Users/thekn/Documents/DeliveryQueueMaster/save.csv");
    ofstream fileOut;
    fileOut.open("C:/Users/thekn/Documents/DeliveryQueueMaster/temp.csv"); // load output stream

    // error message if file cannot open
    if(!fileOut.is_open())
        QMessageBox::information(this, "Error", "Cannot open temp file for current deliveries.");
    else    // add header to temp file
        fileOut << "ID,Required Delivery Date,Location,Shipping Method,Classification,Number of Items,Media Type,Required Ship Date,Required Start Date" << endl;

    // error message if file cannot open
    if(!fileIn.is_open())
        QMessageBox::information(this, "Error", "Cannot open save file for current deliveries.");
    else { // add delivery to save file
        fileIn.seekg(0);        // move cursor to beginning of save.csv
        getline(fileIn, line);  // skip header line

        while(getline(fileIn, line)){       // read until reach end of file
            istringstream stringIn(line);   // feed line to stream
            getline(stringIn, id, ',');     // parse line using comma delimiter, extract ID
            if(stoi(id) != deliveryID)          // if not the delivery being edited,
                fileOut << line << endl;        // write line to temp file
        }

        string strDelivery; // string to add to save file
        // append edited delivery information to string
        strDelivery += to_string(deliveryID) + ',';
        strDelivery += editui->dteDeliveryDate->date().toString("dd/MM/yyyy").toStdString() + ',';
        strDelivery += editui->cboLocation->currentText().toStdString() + ',';
        strDelivery += editui->cboShipping->currentText().toStdString() + ',';
        strDelivery += editui->cboClassification->currentText().toStdString() + ',';
        strDelivery += QString::number(editui->spnNumberObjects->value()).toStdString() + ',';
        strDelivery += editui->cboMediaType->currentText().toStdString() + ",";
        fileOut << strDelivery << endl;    // send delivery to save file

        // close both files
        fileOut.close();
        fileIn.close();
        // delete original file
        remove("C:/Users/thekn/Documents/DeliveryQueueMaster/save.csv");
        // rename temp.csv to save.csv
        rename("C:/Users/thekn/Documents/DeliveryQueueMaster/temp.csv","C:/Users/thekn/Documents/DeliveryQueueMaster/save.csv");

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
