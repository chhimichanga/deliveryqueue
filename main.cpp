#include "deliveryqueue.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DeliveryQueue dq;
    dq.show();

    return a.exec();
}
