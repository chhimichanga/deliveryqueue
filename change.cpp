#include "change.h"
#include "ui_ChangeSchedule.h"
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
ChangeSchedule::ChangeSchedule(QWidget *parent) :
    QWidget(parent), csui(new Ui::ChangeSchedule)
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
    connect(csui->btnsubmit, &QPushButton::clicked, this, &ChangeSchedule::submit);
    connect(csui->btncancel, &QPushButton::clicked, this, &ChangeSchedule::cancel);

}
void ChangeSchedule::submit()
{
    QDate selectedDate= csui->calendarWidget->selectedDate();
    QDate currentDate = QDate::currentDate();
    int diff = currentDate.daysTo(selectedDate);
    schedule[diff] = csui->comboBox->currentText().toInt();


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
void ChangeSchedule::cancel()
{
    close();
}

ChangeSchedule::~ChangeSchedule()
{
    delete csui;
}

void ChangeSchedule::on_calendarWidget_clicked(const QDate &date)
{
    QDate currentDate = QDate::currentDate();
    int diff = currentDate.daysTo(date);
    csui->comboBox->setCurrentText(QString::number(schedule[diff]));

}
