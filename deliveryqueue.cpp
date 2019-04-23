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

#define MAX_DELIVERIES 1000 // maximum number of deliveries

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
    // create an empty delivery table with 0 rows and 13 columns
    deliveryTable = new QStandardItemModel(0, 13, parent);

    // add table headers
    deliveryTable->setHeaderData(0, Qt::Horizontal, "Transmittal #");
    deliveryTable->setHeaderData(1, Qt::Horizontal, "Destination");
    deliveryTable->setHeaderData(2, Qt::Horizontal, "Transit Method");
    deliveryTable->setHeaderData(3, Qt::Horizontal, "Ship Name/Hull #");
    deliveryTable->setHeaderData(4, Qt::Horizontal, "ECN/TECN");
    deliveryTable->setHeaderData(5, Qt::Horizontal, "Media Type");
    deliveryTable->setHeaderData(6, Qt::Horizontal, "# of Items");
    deliveryTable->setHeaderData(7, Qt::Horizontal, "Classification");
    deliveryTable->setHeaderData(8, Qt::Horizontal, "Staffing Level");
    deliveryTable->setHeaderData(9, Qt::Horizontal, "Delivery Date");
    deliveryTable->setHeaderData(10, Qt::Horizontal, "Ship Date");
    deliveryTable->setHeaderData(11, Qt::Horizontal, "Start Date");
    deliveryTable->setHeaderData(12, Qt::Horizontal, "Assigned Staff");

    // set source model for deliveryModel
    deliveryModel->setSourceModel(deliveryTable);

    // set deliveryModel as the model for the tree view
    dqui->treeView->setModel(deliveryModel);
    dqui->treeView->setSortingEnabled(true);

    // sort deliveries by start date
    dqui->treeView->sortByColumn(11, Qt::AscendingOrder);

    // add context menu options
    dqui->treeView->addAction(dqui->actEditDelivery);
    dqui->treeView->addAction(dqui->actDeleteDelivery);
    dqui->treeView->addAction(dqui->actArchiveDelivery);

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

        //transmission number
        add->ledTransmittal->setText(xlsx.read("B1").toString());
        //Location
        add->cboDestination->setCurrentText(xlsx.read("B2").toString());
        //Transit Method
        add->cboTransitMethod->setCurrentText(xlsx.read("B3").toString());
        //ship/hull #
        add->cboShipHull->setCurrentText(xlsx.read("B4").toString());
        //ECN/TECN
        add->ledECN->setText(xlsx.read("B5").toString());
        //Media Type
        add->cboMediaType->setCurrentText(xlsx.read("B6").toString());
        //# of Items
        add->spnNumberOfItems->setValue(xlsx.read("B7").toInt());
        //classification
        add->cboClassification->setCurrentText(xlsx.read("B8").toString());
        //Required Delivery Date
        add->dteDeliveryDate->setDate(QVariant(xlsx.read("B9")).toDate());

        add->show();
    }
}


// edit an existing delivery
void DeliveryQueue::editDelivery(){
    // check if user selected a delivery
    if(!dqui->treeView->selectionModel()->hasSelection())
        QMessageBox::information(this, "Failure", "You haven't selected the delivery to be edited.");
    else {
        // selected row number
        int selectedRow = dqui->treeView->selectionModel()->currentIndex().row();

        // start a new instance of the edit form
        frmEditDelivery *edit = new frmEditDelivery();

        // save the selected delivery's old information
        edit->transmittalID     = deliveryModel->index(selectedRow, 0).data().toString();
        edit->destination       = deliveryModel->index(selectedRow, 1).data().toString();
        edit->transitMethod     = deliveryModel->index(selectedRow, 2).data().toString();
        edit->shipHull          = deliveryModel->index(selectedRow, 3).data().toString();
        edit->ECN               = deliveryModel->index(selectedRow, 4).data().toString();
        edit->mediaType         = deliveryModel->index(selectedRow, 5).data().toString();
        edit->numItems          = deliveryModel->index(selectedRow, 6).data().toInt();
        edit->classification    = deliveryModel->index(selectedRow, 7).data().toString();
        edit->staffingLevel     = deliveryModel->index(selectedRow, 8).data().toInt();
        edit->deliveryDate      = deliveryModel->index(selectedRow, 9).data().toDate();

        qDebug() << "attempting to edit: " << edit->transmittalID << " " << edit->destination << " " << edit->transitMethod << " " << edit->shipHull << " " << edit->ECN << " " << edit->mediaType << " " << edit->numItems << " " << edit->classification << " " << edit->staffingLevel << " " << edit->deliveryDate.toString("dd/MM/yyyy");
        QFile fileIn("save.csv");   // open save file
        QString line;               // strings for reading each line

        // error message if file cannot open
        if(!fileIn.open(QIODevice::ReadWrite |  QIODevice::Append | QIODevice::Text))
            QMessageBox::information(this, "Error", "Cannot open save file for current deliveries.");
        else{
            fileIn.seek(0);                 // move cursor to beginning of save.csv
            while(!fileIn.atEnd()){         // read until reach end of file
                line = fileIn.readLine();   // store line
                QStringList lineList = line.split(","); // parse line using comma delimiter

                // if selected delivery's information matches that of the delivery being read, edit that delivery
                if(edit->transmittalID.compare(lineList.at(0)) == 0 && edit->destination.compare(lineList.at(1)) == 0 && edit->transitMethod.compare(lineList.at(2)) == 0 && edit->shipHull.compare(lineList.at(3)) == 0 && edit->ECN.compare(lineList.at(4)) == 0
                        && edit->mediaType.compare(lineList.at(5)) == 0 && edit->numItems == lineList.at(6).toInt() && edit->classification.compare(lineList.at(7)) == 0 && edit->staffingLevel == lineList.at(8).toInt() && edit->deliveryDate.toString("dd/MM/yyyy").compare(lineList.at(9)) == 0){
                    for(int i = 0; i < 10; i++)
                        qDebug() << lineList.at(i);
                    // fill fields with delivery's information
                    edit->ledTransmittal->setText(lineList.at(0));
                    edit->cboDestination->setCurrentText(lineList.at(1));
                    edit->cboTransitMethod->setCurrentText(lineList.at(2));
                    edit->cboShipHull->setCurrentText(lineList.at(3));
                    edit->ledECN->setText(lineList.at(4));
                    edit->cboMediaType->setCurrentText(lineList.at(5));
                    edit->spnNumberOfItems->setValue(lineList.at(6).toInt());
                    edit->cboClassification->setCurrentText(lineList.at(7));
                    edit->cboStaffingLevel->setCurrentText(lineList.at(8));
                    edit->dteDeliveryDate->setDate(QDate::fromString(lineList.at(9), "dd/MM/yyyy"));

                    istringstream iss(lineList.at(12).toStdString());
                    string* staff_name = new string[3]{"mike", "michael", "bruce"};
                    string name;
                    while(getline(iss, name, ';')){
                        if(name == staff_name[0]) edit->chkEmployee1->setChecked(true);
                        else if(name == staff_name[1]) edit->chkEmployee2->setChecked(true);
                        else if(name == staff_name[2]) edit->chkEmployee3->setChecked(true);
                    }
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
    if(!dqui->treeView->selectionModel()->hasSelection())
        QMessageBox::information(this, "Failure", "You haven't selected the delivery to be deleted.");
    else {
        int row = dqui->treeView->selectionModel()->currentIndex().row();   // selected index number
        if(QMessageBox::question(this, "Delete?", "Are you sure you want to delete this delivery?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes){
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

    cell = testArchive.cellAt(currentRow, currentCol); //tests to see if the 1st cell is empty

    // if the cell is empty, fill the row with header data
    if(cell == nullptr){
        testArchive.write(currentRow, currentCol++, "Transmittal #");
        testArchive.write(currentRow, currentCol++, "Destination");
        testArchive.write(currentRow, currentCol++, "Transit Method");
        testArchive.write(currentRow, currentCol++, "Ship Name/Hull #");
        testArchive.write(currentRow, currentCol++, "ECN/TECN");
        testArchive.write(currentRow, currentCol++, "Media Type");
        testArchive.write(currentRow, currentCol++, "# of Items");
        testArchive.write(currentRow, currentCol++, "Classifcation");
        testArchive.write(currentRow, currentCol++, "Staffing Level");
        testArchive.write(currentRow, currentCol++, "Delivery Date");
        testArchive.write(currentRow, currentCol++, "Ship Date");
        testArchive.write(currentRow, currentCol++, "Start Date");
        testArchive.write(currentRow, currentCol++, "Assigned Staff");
        testArchive.write(currentRow, currentCol, "Date Archived");
        currentCol = 1; // reset column number for next row
        currentRow++;   // move to next row
    }

    cell = testArchive.cellAt(currentRow, currentCol);  // copy first column in next row

    while(cell != nullptr){ //scans through the spreadsheet until it finds an empty line
        currentRow++;
        cell = testArchive.cellAt(currentRow, currentCol);
    }

    int col = 0;
    // check if user have selected a delivery
    if(!dqui->treeView->selectionModel()->hasSelection())
        QMessageBox::information(this, "Failure", "You haven't selected the delivery to be archived.");
    else {
        int row = dqui->treeView->selectionModel()->currentIndex().row();   // selected index number
        if(QMessageBox::question(this, "Archive?", "Are you sure you want to archive this delivery? This will delete it from the queue.", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes){
            QString selected = deliveryModel->index(row, col).data().toString();  // save the selected transmittal ID

            ifstream fileIn("save.csv");    // open save file
            ofstream fileOut;
            fileOut.open("temp.csv");       // writing contents except the deleted line to a temp file
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

            // copy delivery data to the empty row in spreadsheet
            while(selected != nullptr){
                testArchive.write(currentRow, currentCol, selected);

                col++;
                currentCol++;
                selected = deliveryModel->index(row, col).data().toString();
            }
            testArchive.write(currentRow, currentCol, QDate::currentDate().toString()); // write date archived
            testArchive.save();

            refreshQueue();

        }
    }
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
        transmissionNumber =  QString::fromStdString(currentDeliveries[row].get_TransmittalNumber());
        location = QString::fromStdString(currentDeliveries[row].get_Destination());
        transitMethod = QString::fromStdString(currentDeliveries[row].get_TransitMethod());
        shipNameHullNumber =  QString::fromStdString(currentDeliveries[row].get_ShipNameHullNumber());
        ECN =  QString::fromStdString(currentDeliveries[row].get_ECN());
        mediaType = QString::fromStdString(currentDeliveries[row].get_MediaType());
        numberOfItems = currentDeliveries[row].get_NumItems();
        classification = QString::fromStdString(currentDeliveries[row].get_Classification());
        staffingLevel = currentDeliveries[row].get_StaffingLevel();
        dateDeliver = QDate::fromString(QString::fromStdString(currentDeliveries[row].get_DateDeliver()), "dd/MM/yyyy");
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
        QString strDeliveryDate = deliveryTable->data(deliveryTable->index(row, 10)).toString();
        QDate deliveryDate = QDate::fromString(strDeliveryDate, "yyyy-MM-dd");

        // if less than 7 days left to ship, color red
        if(currentDate.daysTo(deliveryDate) <= 7)
            for(column = 0; column < numberOfColumns; column++)
                deliveryTable->setData(deliveryTable->index(row, column),  QColor (255,179,186), Qt::BackgroundRole);
        // if between 7 and 14 days, color yellow
        else if(currentDate.daysTo(deliveryDate) <= 14 && currentDate.daysTo(deliveryDate) > 7)
            for(column = 0; column < numberOfColumns; column++)
                deliveryTable->setData(deliveryTable->index(row, column),  QColor (255,255,186), Qt::BackgroundRole);
        // if more than 14 days, color green
        else if(currentDate.daysTo(deliveryDate) > 14)
            for(column = 0; column < numberOfColumns; column++)
                deliveryTable->setData(deliveryTable->index(row, column),  QColor (186,255,201), Qt::BackgroundRole);
    }
}

// refresh the queue every time the delivery queue window becomes active
bool DeliveryQueue::event(QEvent *event){
    if(event->type() == QEvent::WindowActivate){
        refreshQueue();

        // resize columns to fit
        for(int count = 0; count < 11; count++)
            dqui->treeView->resizeColumnToContents(count);
    }
    return QWidget::event(event);
}

DeliveryQueue::~DeliveryQueue(){
    delete dqui;
}

