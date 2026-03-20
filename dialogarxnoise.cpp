#include "dialogarxnoise.h"
#include "ui_dialogarxnoise.h"

DialogARXnoise::DialogARXnoise(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogARXnoise)
{
    ui->setupUi(this);
}

DialogARXnoise::~DialogARXnoise()
{
    delete ui;
}
