#include "delivery.h"
#include "algorithm.h"
#include "deliveryqueue.h"
#include "ui_frmDeliveryQueue.h"
#include "adddelivery.h"
#include "editdelivery.h"

#define MAX_DELIVERIES 100 // maximum number of deliveries

// creates a delivery queue widget in a main window
DeliveryQueue::DeliveryQueue(QWidget *parent) :
    QMainWindow(parent), dqui(new Ui::DeliveryQueue)
{
    dqui->setupUi(this);

    // filter pattern line edit and label
    dqui->lblFilterPattern->setBuddy(dqui->ledFilterPattern);

    // filter syntax comboboxes
    dqui->lblFilterSyntax->setBuddy(dqui->cboFilterSyntax);

    // filter column
    dqui->lblFilterColumn->setBuddy(dqui->cboFilterColumn);

    // create new delivery table
    deliveryTable = new QStandardItemModel(0, 9, parent);

    // set deliverytable as the model for the tree view
    dqui->treeView->setModel(deliveryTable);
    dqui->treeView->setSortingEnabled(true);

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

    // connect delivery queue ui buttons and slots
    connect(dqui->actNewDelivery, &QAction::triggered, this, &DeliveryQueue::addDelivery);
    connect(dqui->ledFilterPattern, &QLineEdit::textChanged, this, &DeliveryQueue::filterSyntaxChanged);
    connect(dqui->cboFilterSyntax, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DeliveryQueue::filterSyntaxChanged);
    connect(dqui->cboFilterColumn, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DeliveryQueue::filterColumnChanged);
    connect(dqui->chkFilterCS, &QAbstractButton::toggled, this, &DeliveryQueue::filterSyntaxChanged);
    connect(dqui->chkSortCS, &QAbstractButton::toggled, this, &DeliveryQueue::sortChanged);
    connect(dqui->actRefreshQueue, &QAction::triggered, this, &DeliveryQueue::refreshQueue);
    connect(dqui->actEditDelivery, &QAction::triggered, this, &DeliveryQueue::editDelivery);
    connect(dqui->actDeleteDelivery, &QAction::triggered, this, &DeliveryQueue::deleteDelivery);
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
    frmEditDelivery *edit = new frmEditDelivery();
    edit->show();
}

// delete an existing delivery from the queue
void DeliveryQueue::deleteDelivery()
{
        // check if user have selected a delivery
    if(!dqui->treeView->selectionModel()->currentIndex().isValid()){
        QMessageBox::information(this, "Failure", "You haven't selected the delivery to be deleted.");
    }
    int rowidx = dqui->treeView->selectionModel()->currentIndex().row(); // selected index number
    QString selected = deliveryTable->index(rowidx , 0).data().toString(); // save the selected ID

    ifstream file1("C:/Users/thekn/Documents/DeliveryQueueMaster/save.csv");
    ofstream temp;
    temp.open("C:/Users/thekn/Documents/DeliveryQueueMaster/temp.csv"); // wirting contents except the deleted line in to a temp file
    string line;


    while(getline(file1, line)){ // read until reach end of file
        istringstream iss(line);
        string id;
        getline(iss, id, ',');
        if(id != selected.toStdString()){
           temp << line << endl; // write line in to temp file
        }

    }
    temp.close();
    file1.close();
    remove("C:/Users/thekn/Documents/DeliveryQueueMaster/save.csv"); // delete the original file
    rename("C:/Users/thekn/Documents/DeliveryQueueMaster/temp.csv","C:/Users/thekn/Documents/DeliveryQueueMaster/save.csv"); // change the name of temp file to save.csv
    refreshQueue();


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
 /*   QRegExp::PatternSyntax syntax =
            QRegExp::PatternSyntax(ui->cboFilterSyntax->itemData(ui->cboFilterSyntax->currentIndex()).toInt());
    Qt::CaseSensitivity caseSensitivity =
            ui->chkFilterCS->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;

    QRegExp regExp(ui->ledFilterPattern->text(), caseSensitivity, syntax);*/
}

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
    Delivery currentDeliveries[MAX_DELIVERIES];
    Algorithm algorithm(currentDeliveries);

    deliveryTable->removeRows(0, deliveryTable->rowCount()); // clear all items from the delivery table

    QString id, location, shippingMethod, classification, numberOfItems, mediaType;
    QDate dateDeliver, dateShip;

    // go through current deliveries and transfer from save file to deliveryTable
    for(int w = 0; w < algorithm.get_Count(); w++){
        // grab delivery information, cast as compatible Qt data types
        id = QString::fromStdString(currentDeliveries[w].get_ID());
        dateDeliver = QDate::fromString(QString::fromStdString(currentDeliveries[w].get_DateDeliver()), "dd/MM/yyyy");
        location = QString::fromStdString(currentDeliveries[w].get_Location());
        shippingMethod = QString::fromStdString(currentDeliveries[w].get_ShippingMethod());
        classification = QString::fromStdString(currentDeliveries[w].get_Classification());
        numberOfItems = QString::fromStdString(to_string(currentDeliveries[w].get_NumItems()));
        mediaType = QString::fromStdString(currentDeliveries[w].get_MediaType());
        dateShip = QDate::fromString(QString::fromStdString(currentDeliveries[w].get_DateShip()), "dd/MM/yyyy");

        // insert a row below the last current row in the list
        deliveryTable->insertRow(w);

        // set each field's value for the new row
        deliveryTable->setData(deliveryTable->index(w, 0), id);
        deliveryTable->setData(deliveryTable->index(w, 1), dateDeliver);
        deliveryTable->setData(deliveryTable->index(w, 2), location);
        deliveryTable->setData(deliveryTable->index(w, 3), shippingMethod);
        deliveryTable->setData(deliveryTable->index(w, 4), classification);
        deliveryTable->setData(deliveryTable->index(w, 5), numberOfItems);
        deliveryTable->setData(deliveryTable->index(w, 6), mediaType);
        deliveryTable->setData(deliveryTable->index(w, 7), dateShip);
    }
}

DeliveryQueue::~DeliveryQueue()
{
    delete dqui;
}

