#ifndef ADDDELIVERY_H
#define ADDDELIVERY_H

#include <QWidget>

namespace Ui{
    class frmAddDelivery;
}

class frmAddDelivery : public QWidget
{
    Q_OBJECT

public: // public functions
    explicit frmAddDelivery(QWidget *parent = nullptr);
    ~frmAddDelivery();

private slots:
    void submit();
    void cancel();

private:
    Ui::frmAddDelivery *addui;
};

#endif // ADDDELIVERY_H
