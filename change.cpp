#include "change.h"
#include "ui_frmChangeSchedule.h"
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <sstream>
#include <fstream>
#include <iostream>
#include <QDate>
#include <string>
#include <vector>
#include <QDebug>
using namespace std;
frmChangeSchedule::frmChangeSchedule(QWidget *parent) :
    QWidget(parent), csui(new Ui::frmChangeSchedule)
{
    csui->setupUi(this);

    ifstream fileIn("schedule.txt");
    schedule = new int[180];
    string line, token;
    getline(fileIn, line);

    istringstream iss(line);
    getline(iss, token, ',');//skip the date

    int i = 0;
    while(getline(iss, token, ',')){

        schedule[i] = stoi(token);

        i++;

    }
    fileIn.close();

    // connect add delivery ui buttons and slots
    connect(csui->btnSubmit, &QPushButton::clicked, this, &frmChangeSchedule::submit);
    connect(csui->btnCancel, &QPushButton::clicked, this, &frmChangeSchedule::cancel);

}
void frmChangeSchedule::submit()
{
    QDate selectedDate= csui->calendarWidget->selectedDate();
    QDate currentDate = QDate::currentDate();
    int diff = currentDate.daysTo(selectedDate);
    schedule[diff] = csui->cboStaffAvailability->currentText().toInt();


    string write;
    ofstream fileOut;
    fileOut.open("tempSchedule.txt"); // load output stream

    write = currentDate.toString("dd/MM/yyyy").toStdString() + ',';
    for(int i = 0; i < 180; i++){
         write += to_string(schedule[i]) + ",";
    }
    fileOut << write << endl;
    fileOut.close();
    remove("schedule.txt");
    rename("tempSchedule.txt","schedule.txt");
     QMessageBox::information(this, "Success", "Your change has been updated.");


}
void frmChangeSchedule::cancel()
{
    close();
}

frmChangeSchedule::~frmChangeSchedule()
{
    delete csui;
}

void frmChangeSchedule::on_calendarWidget_clicked(const QDate &date)
{
    QDate currentDate = QDate::currentDate();
    int diff = currentDate.daysTo(date);
    csui->cboStaffAvailability->setCurrentText(QString::number(schedule[diff]));

}
