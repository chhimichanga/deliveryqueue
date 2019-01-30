/* Created by Michael Chhim, Bruce Roderiques, Michael Smith, Yun Yang
The delivery queuing system will take in requests for outgoing deliveries and prioritize them into a queue, resulting in a streamlined process that will increase delivery accuracy and timeliness. */

#include "deliveryqueue.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DeliveryQueue dq;
    dq.show();

    return a.exec();
}
