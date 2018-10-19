/* Michael Chhim, Bruce Roderiques, Yun Yang, Michael Smith, Tuan-Son Le
This is a basic sort/filter model of the delivery queue display. */

#include "window.h"

#include <QApplication>
#include <QStandardItemModel>   // for object models
#include <QTime>                // for date and time

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

QAbstractItemModel *createDeliveryModel(QObject *parent)
{
    QStandardItemModel *model = new QStandardItemModel(0, 6, parent);

    // set name of each field in list
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Required Delivery Date"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Destination"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Number of Items"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Transit Method"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Media Type"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Classification Level"));

    // add deliveries to list
    addDelivery(model, QDate(2018, 10, 31), "Okinawa, Japan", "5", "Shipping", "Disc, Documentation", "Secret");
    addDelivery(model, QDate(2018, 11, 11), "Fort Lauderdale", "3", "Hand-carry", "Hardware", "Unclassified");
    addDelivery(model, QDate(2018, 12, 9), "Fort Knox", "4", "Shipping", "Documentation", "Unclassified");
    addDelivery(model, QDate(2019, 1, 1), "Rota, Spain", "2", "Shipping", "Disc, Hardware, Documentation", "Top Secret");
    addDelivery(model, QDate(2019, 2, 5), "Reykjavik, Iceland", "3", "Shipping", "Hardware, Documentation", "Secret");

    return model;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Window window;
    window.setSourceModel(createDeliveryModel(&window));
    window.show();
    return app.exec();
}
