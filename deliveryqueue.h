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
    Ui::DeliveryQueue *dqui;
    explicit DeliveryQueue(QWidget *parent = nullptr);
    ~DeliveryQueue() override;

private slots:
    void addDelivery();
    void importDelivery();
    void editDelivery();
    void deleteDelivery();
    void archiveDelivery();
    void filterSyntaxChanged();
    void filterColumnChanged();

public slots:
    void refreshQueue();
    void changeSchedule();
    bool event(QEvent *event) override;

private:
    QSortFilterProxyModel *deliveryModel;
    QStandardItemModel *deliveryTable;
    QAbstractItemModel *createDeliveryModel(QObject *parent);
    void colorCodeDeliveries();
};

#endif // DELIVERYQUEUE_H

