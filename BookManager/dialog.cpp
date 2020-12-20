#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    connect(ui->sureBtn,&QPushButton::clicked,[=](){
        emit this->Ensure();
        this->close();
    });
    connect(ui->cancelBtn,&QPushButton::clicked,[=](){
       this->close();
    });
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::setMessage(QString mess){
    ui->ques->setText(mess);
}
