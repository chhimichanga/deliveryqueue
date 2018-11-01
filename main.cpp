#include "window.h"
#include <QApplication>
#include <QStandardItem>
#include <QStandardItemModel>   // for object models
#include <QDate>                // for date and time
#include <QtWidgets>
#include <QColor>
#include <QString>

#include <iostream>
#include <stdio.h>
#include <QDebug>



// add a delivery to the list
void addDelivery(QAbstractItemModel *model, const QDate &requiredDeliveryDate, const QString &destination,
                 const QString &numberOfItems, const QString &transitMethod, const QString &mediaType,
                 const QString &classificationLevel)
{
    // insert a row below the last current row in the list
    model->insertRow(0);
    // set each field's value for the new row
    model->setData(model->index(0, 0), requiredDeliveryDate);
    model->setData(model->index(0, 1), destination);
    model->setData(model->index(0, 2), numberOfItems);
    model->setData(model->index(0, 3), transitMethod);
    model->setData(model->index(0, 4), mediaType);
    model->setData(model->index(0, 5), classificationLevel);


}

// abstract model
QAbstractItemModel *createDeliveryModel(QObject *parent)
{
    // create an empty delivery table with 0 rows and 6 columns
    QStandardItemModel *deliveryTable = new QStandardItemModel(0, 10, parent);

    // set name of each table field (column)
    //
    deliveryTable->setHeaderData(0, Qt::Horizontal, QObject::tr("Required Delivery Date"));
    deliveryTable->setHeaderData(1, Qt::Horizontal, QObject::tr("Destination"));
    deliveryTable->setHeaderData(2, Qt::Horizontal, QObject::tr("Number of Items"));
    deliveryTable->setHeaderData(3, Qt::Horizontal, QObject::tr("Transit Method"));
    deliveryTable->setHeaderData(4, Qt::Horizontal, QObject::tr("Media Type"));
    deliveryTable->setHeaderData(5, Qt::Horizontal, QObject::tr("Classification Level"));

    // add deliveries to list
    addDelivery(deliveryTable, QDate(2018, 10, 31), "Okinawa, Japan", "5", "Shipping", "Disc, Documentation", "Secret");
    addDelivery(deliveryTable, QDate(2018, 11, 11), "Fort Lauderdale", "3", "Hand-carry", "Hardware", "Unclassified");
    addDelivery(deliveryTable, QDate(2018, 12, 9), "Fort Knox", "4", "Shipping", "Documentation", "Unclassified");
    addDelivery(deliveryTable, QDate(2019, 1, 1), "Rota, Spain", "2", "Shipping", "Disc, Hardware, Documentation", "Top Secret");
    addDelivery(deliveryTable, QDate(2019, 2, 5), "Newport, Rhode Island", "19", "Shipping", "Hardware, Documentation", "Secret");
    addDelivery(deliveryTable, QDate(2019, 2, 5), "Juneau, Alaska", "1", "Shipping", "Hardware, Documentation", "Secret");
    addDelivery(deliveryTable, QDate(2019, 2, 5), "Sheep", "9", "Shipping", "Hardware, Documentation", "Secret");
    addDelivery(deliveryTable, QDate(2018, 11, 5), "Cow", "6", "Shipping", "Hardware, Documentation", "Secret");
    addDelivery(deliveryTable, QDate(2018, 11, 20), "Chicken", "11", "Shipping", "Hardware, Documentation", "Secret");


    //initialize loop increments and get today's date
    int colCount = 0;
    int rowCount = 0;
    QDate currentDate = QDate::currentDate();

    //process for coloring an entire row based on the days to the included date based on the input date
    //qDebug() << currentDate;
    for(rowCount = 0; rowCount <9; rowCount++) {
        //acquires date of the nth delivery based on rowCount and converts it to a comparable QDate
        QString gettingColored = deliveryTable->data(deliveryTable->index(rowCount, 0)).toString();
        QDate deliveryDate = QDate::fromString(gettingColored,"yyyy-MM-dd");

        //qDebug() << deliveryDate;

        if(currentDate.daysTo(deliveryDate) < 7){
            //qDebug() << currentDate.daysTo(Date);
            for(colCount = 0; colCount <6; colCount++) {
                deliveryTable->setData(deliveryTable->index(rowCount, colCount),  QColor (255,0,0), Qt::BackgroundRole);
            }
        }
        else
        if(currentDate.daysTo(deliveryDate) < 14 && currentDate.daysTo(deliveryDate) > 7){
            //qDebug() << currentDate.daysTo(Date);
            for(colCount = 0; colCount <6; colCount++) {
                deliveryTable->setData(deliveryTable->index(rowCount, colCount),  QColor (255,255,0), Qt::BackgroundRole);
            }
        }
        else
        if(currentDate.daysTo(deliveryDate) > 14){
            for(colCount = 0; colCount <6; colCount++) {
                deliveryTable->setData(deliveryTable->index(rowCount, colCount),  QColor (0,255,0), Qt::BackgroundRole);
            }
        }
    }


    return deliveryTable;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Window window;
    window.setSourceModel(createDeliveryModel(&window));
    window.show();
    return app.exec();
}
