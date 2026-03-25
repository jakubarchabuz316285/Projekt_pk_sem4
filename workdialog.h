#ifndef WORKDIALOG_H
#define WORKDIALOG_H

#include <QWidget>

namespace Ui {
class WorkDialog;
}

class WorkDialog : public QWidget
{
    Q_OBJECT

public:
    explicit WorkDialog(QWidget *parent = nullptr);
    ~WorkDialog();

private slots:
    void on_ChkLocalSimulation_checkStateChanged(const Qt::CheckState &arg1);

    void on_RdioLocal_clicked();

    void on_RdioLocal_toggled(bool checked);

private:
    Ui::WorkDialog *ui;
};

#endif // WORKDIALOG_H
