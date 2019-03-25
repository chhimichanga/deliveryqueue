#include "delivery.h"
#include "algorithm.h"
#include "deliveryqueue.h"
#include "ui_frmDeliveryQueue.h"
#include "adddelivery.h"
#include "change.h"
#include "schedule.h"
#include "editdelivery.h"
#include <string>
#include "QString"
#include <fstream>
#include <sstream>
#include <QDir>
#include "xlsxdocument.h"
#include <QDebug>

#define MAX_DELIVERIES 100 // maximum number of deliveries

// creates a delivery queue widget in a main window
DeliveryQueue::DeliveryQueue(QWidget *parent) :
    QMainWindow(parent), dqui(new Ui::DeliveryQueue){
    dqui->setupUi(this);

    // line edit and label for filter pattern
    dqui->lblFilterPattern->setBuddy(dqui->ledFilterPattern);
    // combobox and label for filter column
    dqui->lblFilterColumn->setBuddy(dqui->cboFilterColumn);

    // create new sort filter model. allows user to sort and filter the delivery table
    deliveryModel = new QSortFilterProxyModel;
    // create an empty delivery table with 0 rows and 10 columns
    deliveryTable = new QStandardItemModel(0, 13, parent);

    // add table headers
    deliveryTable->setHeaderData(0, Qt::Horizontal, "Transmission #");
    deliveryTable->setHeaderData(1, Qt::Horizontal, "Location");
    deliveryTable->setHeaderData(2, Qt::Horizontal, "Transit Method");
    deliveryTable->setHeaderData(3, Qt::Horizontal, "Ship Name/Hull #");
    deliveryTable->setHeaderData(4, Qt::Horizontal, "ECN/TECN");
    deliveryTable->setHeaderData(5, Qt::Horizontal, "Media Type");
    deliveryTable->setHeaderData(6, Qt::Horizontal, "# of Items");
    deliveryTable->setHeaderData(7, Qt::Horizontal, "Classification");
    deliveryTable->setHeaderData(8, Qt::Horizontal, "Staffing Level");
    deliveryTable->setHeaderData(9, Qt::Horizontal, "Required Delivery Date");
    deliveryTable->setHeaderData(10, Qt::Horizontal, "Required Ship Date");
    deliveryTable->setHeaderData(11, Qt::Horizontal, "Required Start Date");
    deliveryTable->setHeaderData(12, Qt::Horizontal, "Assigned Staff");

    // set source model for deliveryModel
    deliveryModel->setSourceModel(deliveryTable);

    // set deliveryModel as the model for the tree view
    dqui->treeView->setModel(deliveryModel);
    dqui->treeView->setSortingEnabled(true);

    // sort deliveries by ship date
    dqui->treeView->sortByColumn(11, Qt::AscendingOrder);

    // add context menu options
    dqui->treeView->addAction(dqui->actEditDelivery);
    dqui->treeView->addAction(dqui->actDeleteDelivery);
    dqui->treeView->addAction(dqui->actArchiveDelivery);

    // resize columns to fit
    for(int count = 0; count < 11; count++)
        dqui->treeView->resizeColumnToContents(count);

    // connect delivery queue ui buttons and slots
    connect(dqui->actAddManually, &QAction::triggered, this, &DeliveryQueue::addDelivery);
    connect(dqui->actImport, &QAction::triggered, this, &DeliveryQueue::importDelivery);
    connect(dqui->ledFilterPattern, &QLineEdit::textChanged, this, &DeliveryQueue::filterSyntaxChanged);
    connect(dqui->cboFilterColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DeliveryQueue::filterColumnChanged);
    connect(dqui->chkFilterCS, &QAbstractButton::toggled, this, &DeliveryQueue::filterSyntaxChanged);
    connect(dqui->actRefreshQueue, &QAction::triggered, this, &DeliveryQueue::refreshQueue);
    connect(dqui->actEditDelivery, &QAction::triggered, this, &DeliveryQueue::editDelivery);
    connect(dqui->actDeleteDelivery, &QAction::triggered, this, &DeliveryQueue::deleteDelivery);
    connect(dqui->actArchiveDelivery, &QAction::triggered, this, &DeliveryQueue::archiveDelivery);
    connect(dqui->actChangeSchedule, &QAction::triggered, this, &DeliveryQueue::changeSchedule);

    // load queue if there are any deliveries
    refreshQueue();
}

// add a new delivery to the queue
void DeliveryQueue::addDelivery(){
    frmAddDelivery *add = new frmAddDelivery();
    add->show();
}

// edit the staff schedule
void DeliveryQueue::changeSchedule(){
    frmChangeSchedule *change = new frmChangeSchedule();
    change->show();
}

// import a delivery from a file
void DeliveryQueue::importDelivery(){
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open File"), "",
            tr("Excel Spreadsheet (*.xlsx);;All Files (*)"));

    if (fileName.isEmpty())
        return;
    else {

        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                file.errorString());
            return;
        }

        frmAddDelivery *add = new frmAddDelivery();

        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_4_5);

        QXlsx::Document xlsx(fileName);

        //assumption that order of information is: transmission number, classification,
        //Media Type, Transit Method, (staffing level not on form), # of items, Required Delivery Date, ship/hull #
        //Location, ECN/TECN


        //transmission number
        add->transmission->setText(xlsx.read("B1").toString());
        //classification
        add->classification->setCurrentText(xlsx.read("B2").toString());
        //Media Type
        add->mediaType->setCurrentText(xlsx.read("B3").toString());
        //Transit Method
        add->shipping->setCurrentText(xlsx.read("B4").toString());
        //# of Items
        add->numberOfItems->setValue(xlsx.read("B5").toInt());
        //Required Delivery Date
        add->deliveryDate->setDate(QVariant(xlsx.read("B6")).toDate());
        //ship/hull #
        add->shipnumber->setCurrentText(xlsx.read("B7").toString());
        //Location
        add->location->setCurrentText(xlsx.read("B8").toString());
        //ECN/TECN
        add->ECN->setText(xlsx.read("B9").toString());

        add->show();
    }
}


// edit an existing delivery
void DeliveryQueue::editDelivery(){
    // check if user selected a delivery
    if(!dqui->treeView->selectionModel()->currentIndex().isValid())
        QMessageBox::information(this, "Failure", "You haven't selected the delivery to be edited.");
    else {
        // selected index number
        int selectedRow = dqui->treeView->selectionModel()->currentIndex().row();
        // save the selected delivery's Transmission #
        QString selected = deliveryModel->index(selectedRow, 0).data().toString();
        // start a new instance of the edit form
        frmEditDelivery *edit = new frmEditDelivery();

        // store delivery ID in a variable to use in frmEditDelivery
        edit->transmissionNumber = selected.toStdString();

        // open save file
        ifstream fileIn("save.csv");
        // strings for reading each line, id, and the rest of the line after id
        string line, transmission, token;
        int count;

        // read whole line
        while(getline(fileIn, line)){
            istringstream stringIn(line);   // feed line into string stream
            count = 0;
            getline(stringIn, transmission, ',');     // read line until first comma

            // if selected delivery's ID and the ID being read from the file match, edit that delivery
            if(transmission == selected.toStdString()){
                edit->ledTransmission->setText(QString::fromStdString(transmission));
                while(getline(stringIn, token, ',')){   // read the rest of the line
                    if(count == 0)      // lcoation
                        edit->cboLocation->setCurrentText(QString::fromStdString(token));
                    else if(count == 1) // transit method
                        edit->cboTransitMethod->setCurrentText(QString::fromStdString(token));
                    else if(count == 2) // ship name / hull#
                        edit->cboShipHull->setCurrentText(QString::fromStdString(token));
                    else if(count == 3) // ecn/tecn
                        edit->ledECN->setText(QString::fromStdString(token));
                    else if(count == 4) // media type
                        edit->cboMediaType->setCurrentText(QString::fromStdString(token));
                    else if(count == 5) // # of items
                        edit->spnNumberOfItems->setValue(stoi(token));
                    else if(count == 6) // Classification
                        edit->cboClassification->setCurrentText(QString::fromStdString(token));
                    else if(count == 7) // staffing level
                        edit->cboStaffing->setCurrentText(QString::fromStdString(token));
                    else if(count == 8)      // delivery date column
                        edit->dteDeliveryDate->setDate(QDate::fromString(QString::fromStdString(token), "dd/MM/yyyy"));

                    count++;    // move to next column in row
                }
            }
        }
        fileIn.close();
        edit->show();
      }
}

// delete an existing delivery from the queue
void DeliveryQueue::deleteDelivery(){
    // check if user have selected a delivery
    if(!dqui->treeView->selectionModel()->currentIndex().isValid())
        QMessageBox::information(this, "Failure", "You haven't selected the delivery to be deleted.");
    else {
        if(QMessageBox::question(this, "Delete?", "Are you sure you want to delete this delivery?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes){
            int row = dqui->treeView->selectionModel()->currentIndex().row();   // selected index number
            QString selected = deliveryModel->index(row, 0).data().toString();  // save the selected ID

            ifstream fileIn("save.csv");   // open save file
            ofstream fileOut;
            fileOut.open("temp.csv"); // writing contents except the deleted line to a temp file
            string line, transmission;

            while(getline(fileIn, line)){   // read until reach end of file
                istringstream stringIn(line); // feed line to stream
                getline(stringIn, transmission, ',');   // parse line using comma delimiter
                if(transmission != selected.toStdString()){    // if not the delivery being deleted,
                    fileOut << line << endl;         // write line to temp file
                }
            }

            // close both files
            fileOut.close();
            fileIn.close();
            // delete original file
            remove("save.csv");
            // rename temp.csv to save.csv
            rename("temp.csv","save.csv");
            refreshQueue();
        }
    }
}

// archive a completed delivery, then delete it from the queue
void DeliveryQueue::archiveDelivery(){
    QXlsx::Document testArchive ("testArchive.xlsx");
    QXlsx::Cell *cell;

    //xlsx tests start
    //xlsx headers are added already

    int currentCol = 1;
    int currentRow = 1;

    cell = testArchive.cellAt(currentRow,1); //tests to see if the first box on the current row is empty

    while(cell != nullptr){ //scans through the spreadsheet until it finds an empty line
        currentRow++;
        cell = testArchive.cellAt(currentRow,1);
    }

    //xlsx tests end

    int col = 0;

    // check if user have selected a delivery
    if(!dqui->treeView->selectionModel()->currentIndex().isValid())
        QMessageBox::information(this, "Failure", "You haven't selected the delivery to be archived.");
    else {
        if(QMessageBox::question(this, "Archive?", "Are you sure you want to archive this delivery? This will delete it from the queue.", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes){
            int row = dqui->treeView->selectionModel()->currentIndex().row();   // selected index number
            QString selected = deliveryModel->index(row, col).data().toString();  // save the selected ID

            while(selected != nullptr){
                testArchive.write(currentRow, currentCol, selected);
                col++;
                currentCol++;
                selected = deliveryModel->index(row, col).data().toString();
            }
            testArchive.save();

            ifstream fileIn("save.csv");   // open save file
            ofstream fileOut;
            fileOut.open("temp.csv"); // writing contents except the deleted line to a temp file
            string line, transmission;

            while(getline(fileIn, line)){   // read until reach end of file
                istringstream stringIn(line); // feed line to stream
                getline(stringIn, transmission, ',');   // parse line using comma delimiter
                qDebug() << QString().fromStdString(transmission);
                qDebug() << selected;

                if(transmission != selected.toStdString()){    // if not the delivery being deleted,
                    fileOut << line << endl;         // write line to temp file
                }
            }

            // close both files
            fileOut.close();
            fileIn.close();
            // delete original file
            remove("save.csv");
            // rename temp.csv to save.csv
            rename("temp.csv","save.csv");

            refreshQueue();
        }
    }
}

// assign an existing delivery to an employee
void DeliveryQueue::assignDelivery(){

}

// filter queue when user checks case sensitive filtering or enters a search expression
void DeliveryQueue::filterSyntaxChanged(){
    // set filter syntax to fixed string
    QRegExp::PatternSyntax syntax =
            QRegExp::PatternSyntax(QRegExp::FixedString);
    // set case sensitivity
    Qt::CaseSensitivity caseSensitivity =
            dqui->chkFilterCS->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
    // apply search expression, case sensitivity, and syntax
    QRegExp regExp(dqui->ledFilterPattern->text(), caseSensitivity, syntax);
    deliveryModel->setFilterRegExp(regExp);
}

// set the column by which the queue is filtered
void DeliveryQueue::filterColumnChanged(){
    deliveryModel->setFilterKeyColumn(dqui->cboFilterColumn->currentIndex());
}

// refresh queue
void DeliveryQueue::refreshQueue(){

    Delivery currentDeliveries[MAX_DELIVERIES]; // array of current deliveries
    Algorithm algorithm(currentDeliveries); // declare new instance of Algorithm, pass in current deliveries

    deliveryTable->removeRows(0, deliveryTable->rowCount()); // clear all items from the delivery table

    // delivery details
    int numberOfItems, staffingLevel, numberOfDeliveries;
    QString location, transitMethod, classification, mediaType, transmissionNumber, shipNameHullNumber, ECN, staff;
    QDate dateDeliver, dateShip, dateStart;

    // set number of deliveries
    numberOfDeliveries = algorithm.getCount();
    // go through current deliveries and transfer from save file to deliveryTable
    for(int row = 0; row < numberOfDeliveries; row++){
        // grab delivery information, cast as compatible Qt data types
        transmissionNumber =  QString::fromStdString(currentDeliveries[row].get_TransmissionNumber());
        shipNameHullNumber =  QString::fromStdString(currentDeliveries[row].get_ShipNameHullNumber());
        ECN =  QString::fromStdString(currentDeliveries[row].get_ECN());
        dateDeliver = QDate::fromString(QString::fromStdString(currentDeliveries[row].get_DateDeliver()), "dd/MM/yyyy");
        location = QString::fromStdString(currentDeliveries[row].get_Location());
        transitMethod = QString::fromStdString(currentDeliveries[row].get_TransitMethod());
        classification = QString::fromStdString(currentDeliveries[row].get_Classification());
        numberOfItems = currentDeliveries[row].get_NumItems();
        staffingLevel = currentDeliveries[row].get_StaffingLevel();
        mediaType = QString::fromStdString(currentDeliveries[row].get_MediaType());
        dateShip = QDate::fromString(QString::fromStdString(currentDeliveries[row].get_DateShip()), "dd/MM/yyyy");
        dateStart = QDate::fromString(QString::fromStdString(currentDeliveries[row].get_DateStart()), "dd/MM/yyyy");
        staff = QString::fromStdString(currentDeliveries[row].get_Staff());
        // insert a row below the last current row in the list
        deliveryTable->insertRow(row);
        // set each field's value for the new row
        deliveryTable->setData(deliveryTable->index(row, 0), transmissionNumber);
        deliveryTable->setData(deliveryTable->index(row, 1), location);
        deliveryTable->setData(deliveryTable->index(row, 2), transitMethod);
        deliveryTable->setData(deliveryTable->index(row, 3), shipNameHullNumber);
        deliveryTable->setData(deliveryTable->index(row, 4), ECN);
        deliveryTable->setData(deliveryTable->index(row, 5), mediaType);
        deliveryTable->setData(deliveryTable->index(row, 6), numberOfItems);
        deliveryTable->setData(deliveryTable->index(row, 7), classification);
        deliveryTable->setData(deliveryTable->index(row, 8), staffingLevel);
        deliveryTable->setData(deliveryTable->index(row, 9), dateDeliver);
        deliveryTable->setData(deliveryTable->index(row, 10), dateShip);
        deliveryTable->setData(deliveryTable->index(row, 11), dateStart);
        deliveryTable->setData(deliveryTable->index(row, 11), dateStart);
        deliveryTable->setData(deliveryTable->index(row, 12), staff);


    }
    colorCodeDeliveries();
}

// color code deliveries based on distance between today's date and delivery's ship date
void DeliveryQueue::colorCodeDeliveries(){
    // initialize loop increments and get today's date
    int column = 0, row = 0;
    // get today's date
    QDate currentDate = QDate::currentDate();
    // get number of rows and columsn in delivery table
    int numberOfRows = deliveryTable->rowCount();
    int numberOfColumns = deliveryTable->columnCount();

    //process for coloring an deliveyr row based on the days left until ship date
    for(row = 0; row < numberOfRows; row++) {
        //acquires ship date of the nth delivery based on rowCount and converts it to a comparable QDate
        QString strDeliveryDate = deliveryTable->data(deliveryTable->index(row, 11)).toString();
        QDate deliveryDate = QDate::fromString(strDeliveryDate,"yyyy-MM-dd");

        // if less than 7 days left to ship, color red
        if(currentDate.daysTo(deliveryDate) < 7)
            for(column = 0; column < numberOfColumns; column++)
                deliveryTable->setData(deliveryTable->index(row, column),  QColor (255,179,186), Qt::BackgroundRole);
        // if between 7 and 14 days, color yellow
        else if(currentDate.daysTo(deliveryDate) <= 14 && currentDate.daysTo(deliveryDate) >= 7)
            for(column = 0; column < numberOfColumns; column++)
                deliveryTable->setData(deliveryTable->index(row, column),  QColor (255,255,186), Qt::BackgroundRole);
        // if more than 14 days, color green
        else if(currentDate.daysTo(deliveryDate) > 14)
            for(column = 0; column < numberOfColumns; column++)
                deliveryTable->setData(deliveryTable->index(row, column),  QColor (186,255,201), Qt::BackgroundRole);
    }
}

DeliveryQueue::~DeliveryQueue(){
    delete dqui;
}

