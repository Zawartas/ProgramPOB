#include "dialoguiparameters.h"
#include "ui_dialoguiparameters.h"
#include "mainwindow.h"


DialogUiParameters::DialogUiParameters(bool a, bool b, bool c, bool d, bool e, bool f, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogUiParameters)
{
    ui->setupUi(this);

    ui->border_type_1->setChecked(true); //radiobutton on default
    ui->zadaneMaski->addItem("Laplasjan {0,1,0,1,-4,1,0,1,0}");
    ui->zadaneMaski->addItem("Laplasjan {1,1,1,1,-8,1,1,1,1}");
    ui->zadaneMaski->addItem("Laplasjan {-1,-1,-1,-1,9,-1,-1,-1,-1}");
    ui->zadaneMaski->addItem("Laplasjan 1,1,1,1,-7,1,1,1,1}");
    ui->zadaneMaski->addItem("Wygladzanie {1/9}");
    ui->zadaneMaski->addItem("Wygladzanie {1/10,1/5,1/10}");
    ui->zadaneMaski->addItem("Wygladzanie {1/16,1/8,1/16,1/8,1/4,1/8,1/16,1/8,1/16}");
    ui->zadaneMaski->addItem("Wygladzanie {1/16,1/8,1/16,1/8,1/4,1/8,1/16,1/8,1/16}");
    ui->zadaneMaski->addItem("Detekcja {-1,0,1,-2,0,2,-1,0,1}");
    ui->zadaneMaski->addItem("Detekcja {-1,-2,-1,0,0,2,0,0,1}");
    ui->zadaneMaski->addItem("Detekcja 1 i -4");

    a==false?ui->checkBox->hide():ui->checkBox->show();
    b==false?ui->checkBox_2->hide():ui->checkBox_2->show();
    if(c==false)
    {
        ui->horizontalSlider->hide();
        ui->spinBox->hide();
    }
    if(d==false)
    {
        ui->horizontalSlider_2->hide();
        ui->spinBox_2->hide();
    }
    if(e==false)
    {
        ui->doubleSpinBoxMax_0->hide();
        ui->doubleSpinBoxMax_1->hide();
        ui->doubleSpinBoxMax_2->hide();
        ui->doubleSpinBoxMax_3->hide();
        ui->doubleSpinBoxMax_4->hide();
        ui->doubleSpinBoxMax_5->hide();
        ui->doubleSpinBoxMax_6->hide();
        ui->doubleSpinBoxMax_7->hide();
        ui->doubleSpinBoxMax_8->hide();
    }
    if(f==false)
    {
        ui->border_type_1->hide();
        ui->border_type_2->hide();
        ui->border_type_3->hide();
        ui->border_type_4->hide();
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
    maska[0] = ui->doubleSpinBoxMax_0->value();
    maska[1] = ui->doubleSpinBoxMax_1->value();
    maska[2] = ui->doubleSpinBoxMax_2->value();
    maska[3] = ui->doubleSpinBoxMax_3->value();
    maska[4] = ui->doubleSpinBoxMax_4->value();
    maska[5] = ui->doubleSpinBoxMax_5->value();
    maska[6] = ui->doubleSpinBoxMax_6->value();
    maska[7] = ui->doubleSpinBoxMax_7->value();
    maska[8] = ui->doubleSpinBoxMax_8->value();

    ui->border_type_1->isChecked()?border=BORDER_CONSTANT:border=NONE;
    ui->border_type_2->isChecked()?border=BORDER_REPLICATE:border=NONE;
    ui->border_type_3->isChecked()?border=BORDER_REFLECT:border=NONE;
    ui->border_type_4->isChecked()?border=BORDER_WRAP:border=NONE;

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

void DialogUiParameters::on_zadaneMaski_currentIndexChanged(int index)
{
    switch (index) {
        case 0:
            ui->doubleSpinBoxMax_0->setValue(0);
            ui->doubleSpinBoxMax_1->setValue(1);
            ui->doubleSpinBoxMax_2->setValue(0);
            ui->doubleSpinBoxMax_3->setValue(1);
            ui->doubleSpinBoxMax_4->setValue(-4);
            ui->doubleSpinBoxMax_5->setValue(1);
            ui->doubleSpinBoxMax_6->setValue(0);
            ui->doubleSpinBoxMax_7->setValue(1);
            ui->doubleSpinBoxMax_8->setValue(0);
            break;
        case 1:
            ui->doubleSpinBoxMax_0->setValue(1);
            ui->doubleSpinBoxMax_1->setValue(1);
            ui->doubleSpinBoxMax_2->setValue(1);
            ui->doubleSpinBoxMax_3->setValue(1);
            ui->doubleSpinBoxMax_4->setValue(-8);
            ui->doubleSpinBoxMax_5->setValue(1);
            ui->doubleSpinBoxMax_6->setValue(1);
            ui->doubleSpinBoxMax_7->setValue(1);
            ui->doubleSpinBoxMax_8->setValue(1);
            break;
        case 4:
            ui->doubleSpinBoxMax_0->setValue(1/9);
            ui->doubleSpinBoxMax_1->setValue(1/9);
            ui->doubleSpinBoxMax_2->setValue(1/9);
            ui->doubleSpinBoxMax_3->setValue(1/9);
            ui->doubleSpinBoxMax_4->setValue(1/9);
            ui->doubleSpinBoxMax_5->setValue(1/9);
            ui->doubleSpinBoxMax_6->setValue(1/9);
            ui->doubleSpinBoxMax_7->setValue(1/9);
            ui->doubleSpinBoxMax_8->setValue(1/9);
            break;
        case 6:
            ui->doubleSpinBoxMax_0->setValue(0.0625);
            ui->doubleSpinBoxMax_1->setValue(0.125);
            ui->doubleSpinBoxMax_2->setValue(0.0625);
            ui->doubleSpinBoxMax_3->setValue(0.125);
            ui->doubleSpinBoxMax_4->setValue(0.25);
            ui->doubleSpinBoxMax_5->setValue(0.125);
            ui->doubleSpinBoxMax_6->setValue(0.0625);
            ui->doubleSpinBoxMax_7->setValue(0.125);
            ui->doubleSpinBoxMax_8->setValue(0.0625);
            break;
        case 8:
            ui->doubleSpinBoxMax_0->setValue(-1);
            ui->doubleSpinBoxMax_1->setValue(0);
            ui->doubleSpinBoxMax_2->setValue(1);
            ui->doubleSpinBoxMax_3->setValue(-2);
            ui->doubleSpinBoxMax_4->setValue(0);
            ui->doubleSpinBoxMax_5->setValue(2);
            ui->doubleSpinBoxMax_6->setValue(-1);
            ui->doubleSpinBoxMax_7->setValue(0);
            ui->doubleSpinBoxMax_8->setValue(1);
            break;
    }
}
