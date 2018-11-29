#include "delivery.h"
#include "algorithm.h"
#include "deliveryqueue.h"
#include "ui_frmDeliveryQueue.h"
#include "adddelivery.h"
#include "editdelivery.h"
#include <string>
#include "QString"
#include <fstream>
#include <sstream>
#define MAX_DELIVERIES 100 // maximum number of deliveries

// creates a delivery queue widget in a main window
DeliveryQueue::DeliveryQueue(QWidget *parent) :
    QMainWindow(parent), dqui(new Ui::DeliveryQueue)
{
    dqui->setupUi(this);

    // line edit and label for filter pattern
    dqui->lblFilterPattern->setBuddy(dqui->ledFilterPattern);
    // combobox and label for filter column
    dqui->lblFilterColumn->setBuddy(dqui->cboFilterColumn);

    // create new sort filter model. allows user to sort and filter the delivery table
    deliveryModel = new QSortFilterProxyModel;
    // create an empty delivery table with 0 rows and 9 columns
    deliveryTable = new QStandardItemModel(0, 9, parent);

    // add table headers
    deliveryTable->setHeaderData(0, Qt::Horizontal, "ID");
    deliveryTable->setHeaderData(1, Qt::Horizontal, "Required Delivery Date");
    deliveryTable->setHeaderData(2, Qt::Horizontal, "Location");
    deliveryTable->setHeaderData(3, Qt::Horizontal, "Shipping Method");
    deliveryTable->setHeaderData(4, Qt::Horizontal, "Classification");
    deliveryTable->setHeaderData(5, Qt::Horizontal, "# of Items");
    deliveryTable->setHeaderData(6, Qt::Horizontal, "Media Type");
    deliveryTable->setHeaderData(7, Qt::Horizontal, "Required Ship Date");
    deliveryTable->setHeaderData(8, Qt::Horizontal, "Required Start Date");

    // set source model for deliveryModel
    deliveryModel->setSourceModel(deliveryTable);

    // set deliveryModel as the model for the tree view
    dqui->treeView->setModel(deliveryModel);
    dqui->treeView->setSortingEnabled(true);

    // sort deliveries by ship date
    dqui->treeView->sortByColumn(7, Qt::AscendingOrder);

    // connect delivery queue ui buttons and slots
    connect(dqui->actNewDelivery, &QAction::triggered, this, &DeliveryQueue::addDelivery);
    connect(dqui->ledFilterPattern, &QLineEdit::textChanged, this, &DeliveryQueue::filterSyntaxChanged);
    connect(dqui->cboFilterColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DeliveryQueue::filterColumnChanged);
    connect(dqui->chkFilterCS, &QAbstractButton::toggled, this, &DeliveryQueue::filterSyntaxChanged);
    connect(dqui->chkSortCS, &QAbstractButton::toggled, this, &DeliveryQueue::sortChanged);
    connect(dqui->actRefreshQueue, &QAction::triggered, this, &DeliveryQueue::refreshQueue);
    connect(dqui->actEditDelivery, &QAction::triggered, this, &DeliveryQueue::editDelivery);
    connect(dqui->actDeleteDelivery, &QAction::triggered, this, &DeliveryQueue::deleteDelivery);

    // load queue if there are any deliveries
    refreshQueue();
}

// add a new delivery to the queue
void DeliveryQueue::addDelivery()
{
    frmAddDelivery *add = new frmAddDelivery();
    add->show();
}

// edit an existing delivery
void DeliveryQueue::editDelivery()
{
    // check if user have selected a delivery
    if(!dqui->treeView->selectionModel()->currentIndex().isValid())
        QMessageBox::information(this, "Failure", "You haven't selected the delivery to be edited.");
    else {
        int selectedRow = dqui->treeView->selectionModel()->currentIndex().row();       // selected index number
        QString selectedID = deliveryModel->index(selectedRow, 0).data().toString();    // save the selected ID
        // start a new instance of the edit form
        frmEditDelivery *edit = new frmEditDelivery();
        // store delivery ID in a variable to use in frmEditDelivery
        edit->deliveryID = selectedID.toInt();

        // open save file
        ifstream fileIn("C:/Users/thekn/Documents/DeliveryQueueMaster/save.csv");
        string line, id, token; // strings for reading each line, id, and the rest of the line after id
        int count;

        while(getline(fileIn, line)){       // read whole line
            istringstream stringIn(line);   // feed line into string stream
            count = 0;
            getline(stringIn, id, ',');     // read line until first comma
            if(id == selectedID.toStdString()){         // if selected delivery's ID and the ID being read from the file match, edit that delivery
                while(getline(stringIn, token, ',')){   // read the rest of the line
                    if(count == 0)      // delivery date column
                        edit->deliveryDate->setDate(QDate::fromString(QString::fromStdString(token), "dd/MM/yyyy"));
                    else if(count == 1) // location column
                        edit->location->setCurrentText(QString::fromStdString(token));
                    else if(count == 2) // shipping method column
                        edit->shipping->setCurrentText(QString::fromStdString(token));
                    else if(count == 3) // classification column
                        edit->classification->setCurrentText(QString::fromStdString(token));
                    else if(count == 4) // number of items column
                        edit->numberOfItems->setValue(stoi(token));
                    else if(count == 5) // media type column
                        edit->mediaType->setCurrentText(QString::fromStdString(token));

                    count++;    // move to next column in row
                }
            }
        }
        fileIn.close();
        edit->show();
    }
}

// delete an existing delivery from the queue
void DeliveryQueue::deleteDelivery()
{
    // check if user have selected a delivery
    if(!dqui->treeView->selectionModel()->currentIndex().isValid())
        QMessageBox::information(this, "Failure", "You haven't selected the delivery to be deleted.");
    else {
        if(QMessageBox::question(this, "Delete?", "Are you sure you want to delete this delivery?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes){
            int row = dqui->treeView->selectionModel()->currentIndex().row();   // selected index number
            qDebug() << "selected row index" << QString::number(row);
            QString selected = deliveryTable->index(row, 0).data().toString();  // save the selected ID
            qDebug() << "selected ID" << selected;

            ifstream fileIn("C:/Users/thekn/Documents/DeliveryQueueMaster/save.csv");   // open save file
            ofstream fileOut;
            fileOut.open("C:/Users/thekn/Documents/DeliveryQueueMaster/temp.csv"); // writing contents except the deleted line to a temp file
            string line, id;

            while(getline(fileIn, line)){   // read until reach end of file
                istringstream stringIn(line); // feed line to stream
                getline(stringIn, id, ',');   // parse line using comma delimiter
                qDebug() << QString::fromStdString(id);
                if(id != selected.toStdString()){    // if not the delivery being deleted,
                    qDebug() << QString::fromStdString(line);
                    fileOut << line << endl;         // write line to temp file
                }
            }

            // close both files
            fileOut.close();
            fileIn.close();
            // delete original file
            remove("C:/Users/thekn/Documents/DeliveryQueueMaster/save.csv");
            // rename temp.csv to save.csv
            rename("C:/Users/thekn/Documents/DeliveryQueueMaster/temp.csv","C:/Users/thekn/Documents/DeliveryQueueMaster/save.csv");
            refreshQueue();
        }
    }
}

// archive a completed delivery, then delete it from the queue
void DeliveryQueue::archiveDelivery()
{

}

// assign an existing delivery to an employee
void DeliveryQueue::assignDelivery()
{

}

// import a delivery from a file
void DeliveryQueue::importFile()
{

}

// change how input is read when syntax changes
void DeliveryQueue::filterSyntaxChanged()
{
/*    QRegExp::PatternSyntax syntax =
            QRegExp::PatternSyntax(ui->cboFilterSyntax->itemData(ui->cboFilterSyntax->currentIndex()).toInt());
    Qt::CaseSensitivity caseSensitivity =
            ui->chkFilterCS->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;

    QRegExp regExp(ui->ledFilterPattern->text(), caseSensitivity, syntax);
*/}

// set the column by which the queue is filtered
void DeliveryQueue::filterColumnChanged()
{

}

// sort the queue
void DeliveryQueue::sortChanged()
{
  
}

// refresh queue
void DeliveryQueue::refreshQueue(){
    Delivery currentDeliveries[MAX_DELIVERIES]; // array of current deliveries
    Algorithm algorithm(currentDeliveries); // declare new instance of Algorithm, pass in current deliveries

    deliveryTable->removeRows(0, deliveryTable->rowCount()); // clear all items from the delivery table

    // delivery details
    int id, numberOfItems;
    QString location, shippingMethod, classification, mediaType;
    QDate dateDeliver, dateShip;

    // go through current deliveries and transfer from save file to deliveryTable
    for(int row = 0; row < algorithm.get_Count(); row++){
        // grab delivery information, cast as compatible Qt data types
        id = currentDeliveries[row].get_ID();
        dateDeliver = QDate::fromString(QString::fromStdString(currentDeliveries[row].get_DateDeliver()), "dd/MM/yyyy");
        location = QString::fromStdString(currentDeliveries[row].get_Location());
        shippingMethod = QString::fromStdString(currentDeliveries[row].get_ShippingMethod());
        classification = QString::fromStdString(currentDeliveries[row].get_Classification());
        numberOfItems = currentDeliveries[row].get_NumItems();
        mediaType = QString::fromStdString(currentDeliveries[row].get_MediaType());
        dateShip = QDate::fromString(QString::fromStdString(currentDeliveries[row].get_DateShip()), "dd/MM/yyyy");

        // insert a row below the last current row in the list
        deliveryTable->insertRow(row);
        // set each field's value for the new row
        deliveryTable->setData(deliveryTable->index(row, 0), id);
        deliveryTable->setData(deliveryTable->index(row, 1), dateDeliver);
        deliveryTable->setData(deliveryTable->index(row, 2), location);
        deliveryTable->setData(deliveryTable->index(row, 3), shippingMethod);
        deliveryTable->setData(deliveryTable->index(row, 4), classification);
        deliveryTable->setData(deliveryTable->index(row, 5), numberOfItems);
        deliveryTable->setData(deliveryTable->index(row, 6), mediaType);
        deliveryTable->setData(deliveryTable->index(row, 7), dateShip);
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
        QString strDeliveryDate = deliveryTable->data(deliveryTable->index(row, 7)).toString();
        QDate deliveryDate = QDate::fromString(strDeliveryDate,"yyyy-MM-dd");

        // if less than 7 days left to ship, color red
        if(currentDate.daysTo(deliveryDate) < 7)
            for(column = 0; column < numberOfColumns; column++)
                deliveryTable->setData(deliveryTable->index(row, column),  QColor (255,0,0), Qt::BackgroundRole);
        // if between 7 and 14 days, color yellow
        else if(currentDate.daysTo(deliveryDate) <= 14 && currentDate.daysTo(deliveryDate) >= 7)
            for(column = 0; column < numberOfColumns; column++)
                deliveryTable->setData(deliveryTable->index(row, column),  QColor (255,255,0), Qt::BackgroundRole);
        // if more than 14 days, color green
        else if(currentDate.daysTo(deliveryDate) > 14)
            for(column = 0; column < numberOfColumns; column++)
                deliveryTable->setData(deliveryTable->index(row, column),  QColor (0,255,0), Qt::BackgroundRole);
    }
}

DeliveryQueue::~DeliveryQueue()
{
    delete dqui;
}

