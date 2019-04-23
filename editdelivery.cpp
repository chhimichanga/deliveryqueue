#include "editdelivery.h"
#include "ui_frmEditDelivery.h"
#include <fstream>
#include <sstream>
#include <QFile>
#include <QTextStream>
#include <QtWidgets>

// create form to edit a delivery
frmEditDelivery::frmEditDelivery(QWidget *parent) :
    QWidget(parent), editui(new Ui::frmEditDelivery)
{
    editui->setupUi(this);

    // store input boxes into public variables for public access
    cboShipHull         = editui->cboShipHull;
    ledECN              = editui->ledECN;
    ledTransmittal      = editui->ledTransmittal;
    cboDestination      = editui->cboDestination;
    cboClassification   = editui->cboClassification;
    cboTransitMethod    = editui->cboTransitMethod;
    cboMediaType        = editui->cboMediaType;
    cboStaffingLevel    = editui->cboStaffingLevel;
    dteDeliveryDate     = editui->dteDeliveryDate;
    spnNumberOfItems    = editui->spnNumberItems;
    chkEmployee1        = editui->chkEmployee1;
    chkEmployee2        = editui->chkEmployee2;
    chkEmployee3        = editui->chkEmployee3;

    // connect edit delivery ui buttons and slots
    connect(editui->btnSubmit, &QPushButton::clicked, this, &frmEditDelivery::submit);
    connect(editui->btnCancel, &QPushButton::clicked, this, &frmEditDelivery::cancel);
}

void frmEditDelivery::submit()
{
    // count the number of assigned staff
    int staff = 0;
    if(editui->chkEmployee1->isChecked() == true)
        staff++;
    if(editui->chkEmployee2->isChecked() == true)
        staff++;
    if(editui->chkEmployee3->isChecked() == true)
        staff++;

    // make sure the number of assigned staff matches the staffing level
    if(staff != editui->cboStaffingLevel->currentText().toInt()){
        QMessageBox::information(this, "Error", "Staffing level does not match the number of assigned staff.");
    } else {
        QFile fileIn("save.csv");           // load save file to read from
        QFile fileTemp("temp.csv");         // load temp file to write to
        QTextStream fileOut(&fileTemp);     // load output stream
        QString line;        // strings for reading each line and transmittal ID

        // error message if file cannot open
        if(!fileTemp.open(QIODevice::ReadWrite |  QIODevice::Append | QIODevice::Text))
            QMessageBox::information(this, "Error", "Cannot open temp file for current deliveries.");

        // error message if file cannot open
        if(!fileIn.open(QIODevice::ReadWrite |  QIODevice::Append | QIODevice::Text))
            QMessageBox::information(this, "Error", "Cannot open save file for current deliveries.");
        else { // add delivery to save file
            QString strDelivery;                                                            // string to add to save file
            strDelivery += editui->ledTransmittal->text() + ',';                            // unique transmittal #
            strDelivery += editui->cboDestination->currentText() + ',';                     // location
            strDelivery += editui->cboTransitMethod->currentText() + ',';                   // transit method
            strDelivery += editui->cboShipHull->currentText() + ',';                        // ship name & hull #
            strDelivery += editui->ledECN->text() + ',';                                    // ECN
            strDelivery += editui->cboMediaType->currentText() + ',';                       // media type
            strDelivery += QString::number(editui->spnNumberItems->value()) + ',';          // number of items
            strDelivery += editui->cboClassification->currentText() + ',';                  // classification
            strDelivery += editui->cboStaffingLevel->currentText() + ',';                   // staffing level
            strDelivery += editui->dteDeliveryDate->date().toString("dd/MM/yyyy") + ",,,";  // delivery date

            // assigned staff
            if(editui->chkEmployee1->isChecked() == true)
                strDelivery += "mike;";
            if(editui->chkEmployee2->isChecked() == true)
                strDelivery += "michael;";
            if(editui->chkEmployee3->isChecked() == true)
                strDelivery += "bruce;";
            strDelivery += ',';

            qDebug() << "attempting to save: " << transmittalID << " " << destination << " " << transitMethod << " " << shipHull << " " << ECN << " " << mediaType << " " << numItems << " " << classification << " " << staffingLevel << " " << deliveryDate.toString("dd/MM/yyyy");

            // copy save.csv to temp.csv, replacing the delivery being edited with its new information
            fileIn.seek(0);                                 // move cursor to beginning of save.csv
            while(!fileIn.atEnd()){                         // read until reach end of file
                line = fileIn.readLine();                   // store line
                QStringList lineList = line.split(",");     // parse line using comma delimiter

                for(int i = 0; i < 10; i++)
                    qDebug() << lineList.at(i);

                // copy deliveries to temp file
                if(transmittalID.compare(lineList.at(0)) == 0 && destination.compare(lineList.at(1)) == 0 && transitMethod.compare(lineList.at(2)) == 0 && shipHull.compare(lineList.at(3)) == 0 && ECN.compare(lineList.at(4)) == 0
                        && mediaType.compare(lineList.at(5)) == 0 && numItems == lineList.at(6).toInt() && classification.compare(lineList.at(7)) == 0 && staffingLevel == lineList.at(8).toInt() && deliveryDate.toString("dd/MM/yyyy").compare(lineList.at(9)) == 0){
                    qDebug() << "!!! found a match !!!";
                    qDebug() << "writing to save file: " << strDelivery;
                    fileOut << strDelivery; // copy edited delivery to temp file to replace old delivery
                }else{
                    qDebug() << "writing to save file: " << line;
                    fileOut << line;
                }
            }

            // close save and temp files
            fileTemp.close();
            fileIn.close();

            // delete original file
            remove("save.csv");

            // rename temp.csv to save.csv
            rename("temp.csv", "save.csv");

            QMessageBox::information(this, "Success", "Successfully edited a delivery.");
        }
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
