#include "dialogarxcooeficients.h"
#include "ui_dialogarxcooeficients.h"

DialogARXCooeficients::DialogARXCooeficients(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogARXCooeficients)
{
    ui->setupUi(this);
}

DialogARXCooeficients::~DialogARXCooeficients()
{
    delete ui;
}
