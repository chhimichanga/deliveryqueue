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
    QCheckBox *chkEmployee1;
    QCheckBox *chkEmployee2;
    QCheckBox *chkEmployee3;

    // public variables
    QString transmittalID;
    QString destination;
    QString transitMethod;
    QString shipHull;
    QString ECN;
    QString mediaType;
    int numItems;
    QString classification;
    int staffingLevel;
    QDate deliveryDate;


private slots:
    void submit();
    void cancel();

private:
    Ui::frmEditDelivery *editui;
};

#endif // EDITDELIVERY_H
