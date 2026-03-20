#include "DialogDebug.hpp"
#include "ui_DialogDebug.h"

DialogDebug::DialogDebug(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogDebug)
{
    ui->setupUi(this);
}

DialogDebug::~DialogDebug()
{
    delete ui;
}

void DialogDebug::write(QString message)
{
    this->ui->textEdit->append("> " + message);
}
