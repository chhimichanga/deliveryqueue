#ifndef CHANGE_H
#define CHANGE_H
#include <QWidget>
#include "schedule.h"
namespace Ui{
    class frmChangeSchedule;
}

class frmChangeSchedule : public QWidget
{
    int *schedule;
    Q_OBJECT
public: // public functions
    explicit frmChangeSchedule(QWidget *parent = nullptr);
    ~frmChangeSchedule();


private slots:
    void submit();
    void cancel();

    void on_calendarWidget_clicked(const QDate &date);

private:
    Ui::frmChangeSchedule *csui;
};
#endif // CHANGE_H
