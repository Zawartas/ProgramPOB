#include "dialoguiparameters.h"
#include "ui_dialoguiparameters.h"
#include "mainwindow.h"


DialogUiParameters::DialogUiParameters(bool a, bool b, bool c, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogUiParameters)
{
    ui->setupUi(this);
    a==false?ui->checkBox->hide():ui->checkBox->show();
    b==false?ui->checkBox_2->hide():ui->checkBox_2->show();
    if(c==false)
    {
        ui->horizontalSlider_2->hide();
        ui->spinBox_2->hide();
    }
}

DialogUiParameters::~DialogUiParameters()
{
    delete ui;
}

void DialogUiParameters::on_buttonBox_accepted()
{
    dolny_prog = static_cast<short>(ui->horizontalSlider->value());
    gorny_prog = static_cast<short>(ui->horizontalSlider_2->value());
    check_dolny_prog = ui->checkBox->isChecked();
    check_gorny_prog = ui->checkBox_2->isChecked();
    this->accept();
}

void DialogUiParameters::on_horizontalSlider_valueChanged(int value)
{
    if (value > ui->horizontalSlider_2->value())
        ui->horizontalSlider_2->setValue(value);
}

void DialogUiParameters::on_horizontalSlider_2_valueChanged(int value)
{
    if (value < ui->horizontalSlider->value())
        ui->horizontalSlider->setValue(value);
}
