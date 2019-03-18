#ifndef CHANGE_H
#define CHANGE_H
#include <QWidget>
#include "schedule.h"
namespace Ui{
    class ChangeSchedule;
}

class ChangeSchedule : public QWidget
{
    int *schedule;
    Q_OBJECT
public: // public functions
    explicit ChangeSchedule(QWidget *parent = nullptr);
    ~ChangeSchedule();


private slots:
    void submit();
    void cancel();

    void on_calendarWidget_clicked(const QDate &date);

private:
    Ui::ChangeSchedule *csui;
};
#endif // CHANGE_H
