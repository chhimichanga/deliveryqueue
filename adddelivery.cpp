#include "adddelivery.h"
#include "ui_frmAddDelivery.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdlib.h>

// global variable
int rand();

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
    // load save file to write to
    QFile file1("C:/Users/thekn/Documents/DeliveryQueueMaster/save.csv");
    QTextStream out(&file1); // load output stream

    // error message if file cannot open
    if(!file1.open(QIODevice::WriteOnly |  QIODevice::Append | QIODevice::Text)){
           QMessageBox::information(this, "Error", "Cannot open save file.");
    }
    else{ // add delivery to save file
        if(file1.pos() == 0){ // if file is empty, create headers before adding more data
            QString header;
            header = "ID,Required Delivery Date,Location,Shipping Method,Classification,Number of Items,Media Type,Required Ship Date,Required Start Date";
            out << header << endl; // send header to save file
        }

        QString strDelivery; // string to add to save file
        strDelivery += QString::number(rand());
        strDelivery += ',';
        strDelivery += addui->dteDeliveryDate->date().toString("dd/MM/yyyy");
        strDelivery += ',';
        strDelivery += addui->cboLocation->currentText();
        strDelivery += ',';
        strDelivery += addui->cboShipping->currentText();
        strDelivery += ',';
        strDelivery += addui->cboClassification->currentText();
        strDelivery += ',';
        strDelivery += QString::number(addui->spnNumberObjects->value());
        strDelivery += ',';
        strDelivery += addui->cboMediaType->currentText();
        strDelivery += ",";
        out << strDelivery << endl;    // send delivery to save file
        QMessageBox::information(this, "Success", "Successfully submitted a delivery.");
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
