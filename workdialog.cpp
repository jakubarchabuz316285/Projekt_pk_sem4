#include "workdialog.h"
#include "ui_workdialog.h"

WorkDialog::WorkDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WorkDialog)
{
    ui->setupUi(this);
}

WorkDialog::~WorkDialog()
{
    delete ui;
}

void WorkDialog::on_RdioLocal_toggled(bool checked)
{
    ui->GBoxNetwork->setEnabled(!checked);
}
