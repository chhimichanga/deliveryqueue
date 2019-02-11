#ifndef EDITDELIVERY_H
#define EDITDELIVERY_H

#include "delivery.h"
#include <QWidget>
#include <QtWidgets>

namespace Ui{
    class frmEditDelivery;
}

class frmEditDelivery : public QWidget
{
    Q_OBJECT

public: // public functions
    explicit frmEditDelivery(QWidget *parent = nullptr);
    ~frmEditDelivery();
    QLineEdit *transmission;
    QLineEdit *ECN;
    QComboBox *shipnumber;
    QComboBox *location;
    QComboBox *classification;
    QComboBox *shipping;
    QComboBox *mediaType;
    QComboBox *staffing;
    QDateEdit *deliveryDate;
    QSpinBox *numberOfItems;
    string transmissionN;

private slots:
    void submit();
    void cancel();

private:
    Ui::frmEditDelivery *editui;
};

#endif // EDITDELIVERY_H
