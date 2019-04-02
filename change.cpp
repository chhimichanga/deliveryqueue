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
    schedule = new int[540];        //local array that can store 540 numbers
    string line, token;
    getline(fileIn, line);          // get the only line from the file

    istringstream iss(line);
    getline(iss, token, ',');       //skip the date


    for(int i = 0; i < 540; i++){   // read the 540 numbers from the file
        getline(iss, token, ',');
        schedule[i] = stoi(token);  // store them into local array
    }
    fileIn.close(); //close the file

    // connect add delivery ui buttons and slots
    connect(csui->btnSubmit, &QPushButton::clicked, this, &frmChangeSchedule::submit);
    connect(csui->btnCancel, &QPushButton::clicked, this, &frmChangeSchedule::cancel);

    // get availability for current date
    on_calendarWidget_clicked(QDate::currentDate());
}

void frmChangeSchedule::submit()
{
    QDate selectedDate= csui->calendarWidget->selectedDate();   // date that has been selected
    QDate currentDate = QDate::currentDate();                   // current date
    int diff = currentDate.daysTo(selectedDate);                // selected date - current date

    if(csui->chkEmployee1->isChecked() == true) schedule[diff * 3] = 1; // if the check box is checked, change the number to 1 which represents the staff is available
    else  schedule[diff * 3] = 0;                                       // else change the  number to 0
    if(csui->chkEmployee2->isChecked() == true) schedule[diff * 3 + 1] = 1;
    else  schedule[diff * 3 + 1] = 0;
    if(csui->chkEmployee3->isChecked() == true) schedule[diff * 3 + 2] = 1;
    else  schedule[diff * 3 + 2] = 0;
    string write;
    ofstream fileOut;
    fileOut.open("tempSchedule.txt"); // load output stream

    write = currentDate.toString("dd/MM/yyyy").toStdString() + ',';  // update the current availability to file
    for(int i = 0; i < 540; i++){
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
    QDate currentDate = QDate::currentDate();  // current date
    int diff = currentDate.daysTo(date);  // diff = the date that has been clicked - current date

    if(schedule[diff * 3] == 1) // check if the staff 1 is available that day
        csui->chkEmployee1->setChecked(true); // if he is available set the checkbox checked
    else {
        csui->chkEmployee1->setChecked(false);
    }

    if(schedule[diff * 3 + 1] == 1)
        csui->chkEmployee2->setChecked(true);
    else {
        csui->chkEmployee2->setChecked(false);
    }

    if(schedule[diff * 3 + 2] == 1)
        csui->chkEmployee3->setChecked(true);
    else {
        csui->chkEmployee3->setChecked(false);
    }
}


void frmChangeSchedule::on_chkEmployee1_clicked(bool checked) // if the first checkbox on the display is clicked
{
    csui->chkEmployee1->setChecked(checked);
}

void frmChangeSchedule::on_chkEmployee2_clicked(bool checked) // if the second checkbox on the display is clicked
{
    csui->chkEmployee2->setChecked(checked);
}

void frmChangeSchedule::on_chkEmployee3_clicked(bool checked) // if the third checkbox on the display is clicked
{
    csui->chkEmployee3->setChecked(checked);
}
