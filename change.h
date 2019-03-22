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

    void on_checkBox_clicked(bool checked);

    void on_checkBox_2_clicked(bool checked);

    void on_checkBox_3_clicked(bool checked);

private:
    Ui::ChangeSchedule *csui;
};
#endif // CHANGE_H
