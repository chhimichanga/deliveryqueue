#include "editdelivery.h"
#include "ui_frmEditDelivery.h"

// create form to edit a delivery
frmEditDelivery::frmEditDelivery(QWidget *parent) :
    QWidget(parent), editui(new Ui::frmEditDelivery)
{
    editui->setupUi(this);
}

void frmEditDelivery::submit()
{

}

void frmEditDelivery::cancel()
{
    close();
}

frmEditDelivery::~frmEditDelivery()
{
    delete editui;
}
