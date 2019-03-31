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

    void on_chkEmployee1_clicked(bool checked);

    void on_chkEmployee2_clicked(bool checked);

    void on_chkEmployee3_clicked(bool checked);

private:
    Ui::frmChangeSchedule *csui;
};
#endif // CHANGE_H
