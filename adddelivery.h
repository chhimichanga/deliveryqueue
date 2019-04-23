#ifndef ADDDELIVERY_H
#define ADDDELIVERY_H

#include "delivery.h"
#include <QWidget>
#include <QtWidgets>

namespace Ui{
    class frmAddDelivery;
}

class frmAddDelivery : public QWidget
{
    Q_OBJECT

public: // public functions
    explicit frmAddDelivery(QWidget *parent = nullptr);
    ~frmAddDelivery();

    // form objects
    QLineEdit *ledTransmittal;
    QLineEdit *ledECN;
    QComboBox *cboShipHull;
    QComboBox *cboDestination;
    QComboBox *cboClassification;
    QComboBox *cboTransitMethod;
    QComboBox *cboMediaType;
    QComboBox *cboStaffingLevel;
    QDateEdit *dteDeliveryDate;
    QSpinBox *spnNumberOfItems;
    string transmittalNumber;

private slots:
    void submit();
    void cancel();
    bool duplicateFound(QFile &fileIn); // prevents duplicate transmittal IDs when submitting new delivery

private:
    Ui::frmAddDelivery *addui;
};

#endif // ADDDELIVERY_H
