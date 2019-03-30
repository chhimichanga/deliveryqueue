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
    connect(csui->btnSubmit, &QPushButton::clicked, this, &frmChangeSchedule::submit);
    connect(csui->btnCancel, &QPushButton::clicked, this, &frmChangeSchedule::cancel);

    // get availability for current date
    on_calendarWidget_clicked(QDate::currentDate());
}

void frmChangeSchedule::submit()
{
    QDate selectedDate= csui->calendarWidget->selectedDate();
    QDate currentDate = QDate::currentDate();
    int diff = currentDate.daysTo(selectedDate);

    if(csui->chkEmployee1->isChecked() == true) schedule[diff * 3] = 1;
    else  schedule[diff * 3] = 0;
    if(csui->chkEmployee2->isChecked() == true) schedule[diff * 3 + 1] = 1;
    else  schedule[diff * 3 + 1] = 0;
    if(csui->chkEmployee3->isChecked() == true) schedule[diff * 3 + 2] = 1;
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

    if(schedule[diff * 3] == 1)
        csui->chkEmployee1->setChecked(true);
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


void frmChangeSchedule::on_chkEmployee1_clicked(bool checked)
{
    csui->chkEmployee1->setChecked(checked);
}

void frmChangeSchedule::on_chkEmployee2_clicked(bool checked)
{
    csui->chkEmployee2->setChecked(checked);
}

void frmChangeSchedule::on_chkEmployee3_clicked(bool checked)
{
    csui->chkEmployee3->setChecked(checked);
}
