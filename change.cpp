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
    schedule = new int[540];
    string line, token;
    getline(fileIn, line);

    istringstream iss(line);
    getline(iss, token, ',');//skip the date


    for(int i = 0; i < 540; i++){
        getline(iss, token, ',');
        schedule[i] = stoi(token);
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

    if(csui->checkBox->isChecked() == true) schedule[diff * 3] = 1;
    else  schedule[diff * 3] = 0;
    if(csui->checkBox_2->isChecked() == true) schedule[diff * 3 + 1] = 1;
    else  schedule[diff * 3 + 1] = 0;
    if(csui->checkBox_3->isChecked() == true) schedule[diff * 3 + 2] = 1;
    else  schedule[diff * 3 + 2] = 0;
    string write;
    ofstream fileOut;
    fileOut.open("tempSchedule.txt"); // load output stream

    write = currentDate.toString("dd/MM/yyyy").toStdString() + ',';
    for(int i = 0; i < 540; i++){
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
    if(schedule[diff * 3] == 1) csui->checkBox->setChecked(true);
    else {
        csui->checkBox->setChecked(false);
    }
    if(schedule[diff * 3 + 1] == 1) csui->checkBox_2->setChecked(true);
    else {
        csui->checkBox_2->setChecked(false);
    }
    if(schedule[diff * 3 + 2] == 1) csui->checkBox_3->setChecked(true);
    else {
        csui->checkBox_3->setChecked(false);
    }


}


void ChangeSchedule::on_checkBox_clicked(bool checked)
{
    csui->checkBox->setChecked(checked);
}

void ChangeSchedule::on_checkBox_2_clicked(bool checked)
{
    csui->checkBox_2->setChecked(checked);
}

void ChangeSchedule::on_checkBox_3_clicked(bool checked)
{
    csui->checkBox_3->setChecked(checked);
}
