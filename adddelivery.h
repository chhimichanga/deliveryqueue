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
    Ui::frmAddDelivery *addui;
};

#endif // ADDDELIVERY_H
