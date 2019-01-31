#ifndef DELIVERYQUEUE_H
#define DELIVERYQUEUE_H

#include <QMainWindow>
#include <QDate>
#include <stdio.h>
#include <QtWidgets>

namespace Ui {
class DeliveryQueue;
}

class DeliveryQueue : public QMainWindow
{
    Q_OBJECT

public:
    explicit DeliveryQueue(QWidget *parent = nullptr);
    ~DeliveryQueue();

private slots:
    void addDelivery();
    void editDelivery();
    void deleteDelivery();
    void archiveDelivery();
    void assignDelivery();
    void importFile();
    void filterSyntaxChanged();
    void filterColumnChanged();
    void refreshQueue();
    //void contextMenu(const QPoint &);

private:
    Ui::DeliveryQueue *dqui;
    QSortFilterProxyModel *deliveryModel;
    QStandardItemModel *deliveryTable;
    QAbstractItemModel *createDeliveryModel(QObject *parent);
    void colorCodeDeliveries();
};

#endif // DELIVERYQUEUE_H

