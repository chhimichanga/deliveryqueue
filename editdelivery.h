#ifndef EDITDELIVERY_H
#define EDITDELIVERY_H

#include "delivery.h"
#include <QWidget>

namespace Ui{
    class frmEditDelivery;
}

class frmEditDelivery : public QWidget
{
    Q_OBJECT

public: // public functions
    explicit frmEditDelivery(QWidget *parent = nullptr);
    ~frmEditDelivery();

private slots:
    void submit();
    void cancel();

private:
    Ui::frmEditDelivery *editui;
};

#endif // EDITDELIVERY_H
