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
    QLineEdit *ledTransmission;
    QLineEdit *ledECN;
    QComboBox *cboShipHull;
    QComboBox *cboLocation;
    QComboBox *cboClassification;
    QComboBox *cboTransitMethod;
    QComboBox *cboMediaType;
    QComboBox *cboStaffing;
    QDateEdit *dteDeliveryDate;
    QSpinBox *spnNumberOfItems;
    string transmissionNumber;

private slots:
    void submit();
    void cancel();

private:
    Ui::frmEditDelivery *editui;
};

#endif // EDITDELIVERY_H
