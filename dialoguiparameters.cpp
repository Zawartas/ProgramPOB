#include "dialoguiparameters.h"
#include "ui_dialoguiparameters.h"
#include "mainwindow.h"


DialogUiParameters::DialogUiParameters(bool checkbox1,
                                       bool checkbox2,
                                       bool slider1,
                                       bool slider2,
                                       bool maska,
                                       bool borders,
                                       bool zadaneMaski,
                                       bool zadaneMaskiBlura,
                                       bool checkBox_maska5x5,
                                       QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogUiParameters)
{
    ui->setupUi(this);

    maska_wektor = {
                    // ---==3x3==---
                    //    6   7  8
                    //    11 12 13
                    //    16 17 18
                    ui->mask_0, ui->mask_1, ui->mask_2, ui->mask_3, ui->mask_4,
                    ui->mask_5, ui->mask_6, ui->mask_7, ui->mask_8, ui->mask_9,
                    ui->mask_10, ui->mask_11, ui->mask_12, ui->mask_13, ui->mask_14,
                    ui->mask_15, ui->mask_16, ui->mask_17, ui->mask_18, ui->mask_19,
                    ui->mask_20, ui->mask_21, ui->mask_22, ui->mask_23, ui->mask_24
                    };

    ui->zadaneMaskiBlura->addItem("macierz 3 x 3");
    ui->zadaneMaskiBlura->addItem("macierz 3 x 5");
    ui->zadaneMaskiBlura->addItem("macierz 5 x 3");
    ui->zadaneMaskiBlura->addItem("macierz 5 x 5");
    ui->zadaneMaskiBlura->addItem("macierz 7 x 7");

    ui->border_type_1->setChecked(true); //radiobutton on default

    Tablica_MASEK.push_back({99, 99, 99, 99, 99, 99, 0, 1, 0, 99, 99, 1,-4, 1, 99, 99, 0, 1, 0, 99, 99, 99, 99, 99, 99});
    Tablica_MASEK.push_back({99, 99, 99, 99, 99, 99, 1, 1, 1, 99, 99, 1,-8, 1, 99, 99, 1, 1, 1, 99, 99, 99, 99, 99, 99});
    Tablica_MASEK.push_back({99, 99, 99, 99, 99, 99, 1,-2, 1, 99, 99, -2, 4,-2, 99, 99, 1,-2, 1, 99, 99, 99, 99, 99, 99});
    Tablica_MASEK.push_back({99, 99, 99, 99, 99, 99, 0,-1, 0, 99, 99, -1, 4,-1, 99, 99, 0,-1, 0, 99, 99, 99, 99, 99, 99});
    Tablica_MASEK.push_back({99, 99, 99, 99, 99, 99, 0.111,0.111,0.111, 99, 99, 0.111,0.111,0.111,99, 99, 0.111,0.111,0.111, 99, 99, 99, 99, 99, 99});
    Tablica_MASEK.push_back({99, 99, 99, 99, 99, 99, 0.1,0.1,0.1, 99, 99, 0.1,0.2,0.1,99, 99, 0.1,0.1,0.1, 99, 99, 99, 99, 99, 99});
    Tablica_MASEK.push_back({99, 99, 99, 99, 99, 99, 0.083,0.083,0.083, 99, 99, 0.083,0.333,0.083,99, 99, 0.083,0.083,0.083, 99, 99, 99, 99, 99, 99});
    Tablica_MASEK.push_back({99, 99, 99, 99, 99, 99, 0.0625,0.125,0.0625, 99, 99, 0.125,0.25,0.125,99, 99, 0.0625,0.125,0.0625, 99, 99, 99, 99, 99, 99});
    Tablica_MASEK.push_back({99, 99, 99, 99, 99, 99, -1,0,1, 99, 99, -2,0,2,99, 99, -1,0,1, 99, 99, 99, 99, 99, 99});
    Tablica_MASEK.push_back({99, 99, 99, 99, 99, 99, -1,-2,-1, 99, 99, 0,0,0,99, 99, 1,2,1, 99, 99, 99, 99, 99, 99});
    Tablica_MASEK.push_back({99, 99, 99, 99, 99, 99, -1,0,0, 99, 99, 0,1,0,99, 99, 0,0,0, 99, 99, 99, 99, 99, 99});
    Tablica_MASEK.push_back({99, 99, 99, 99, 99, 99, -1,-1,-1, 99, 99, -1, 9,-1,99, 99, -1,-1,-1, 99, 99, 99, 99, 99, 99});
    Tablica_MASEK.push_back({99, 99, 99, 99, 99, 99,  0,-1, 0, 99, 99, -1, 5,-1,99, 99,  0,-1, 0, 99, 99, 99, 99, 99, 99});
    Tablica_MASEK.push_back({99, 99, 99, 99, 99, 99,  1,-2, 1, 99, 99, -2, 5,-2,99, 99,  1,-2, 1, 99, 99, 99, 99, 99, 99});
    Tablica_MASEK.push_back({99, 99, 99, 99, 99, 99,  0,-1, 0, 99, 99, -1,20,-1,99, 99,  0,-1, 0, 99, 99, 99, 99, 99, 99});
    Tablica_MASEK.push_back({99, 99, 99, 99, 99, 99,  1, 1, 1, 99, 99,  1,-2, 1,99, 99, -1,-1,-1, 99, 99, 99, 99, 99, 99});
    Tablica_MASEK.push_back({99, 99, 99, 99, 99, 99,  1, 1,-1, 99, 99,  1,-2,-1,99, 99,  1, 1,-1, 99, 99, 99, 99, 99, 99});

    ui->zadaneMaski->addItem("Laplasjan  {0, 1, 0, 1,-4, 1, 0, 1, 0}");                      //0
    ui->zadaneMaski->addItem("Laplasjan  {1, 1, 1, 1,-8, 1, 1, 1, 1}");                      //1
    ui->zadaneMaski->addItem("Laplasjan  {1,-2, 1,-2, 4,-2, 1,-2, 1}");                      //2  
    ui->zadaneMaski->addItem("Laplasjan  {0,-1, 0,-1, 4,-1, 0,-1, 0}");                      //3
    ui->zadaneMaski->addItem("Wygladzanie  {1/9,  1/9, 1/9, 1/9,1/9, 1/9, 1/9, 1/9, 1/9}");  //4
    ui->zadaneMaski->addItem("Wygladzanie  {1/10,1/10,1/10,1/10,1/5,1/10,1/10,1/10,1/10}");  //5
    ui->zadaneMaski->addItem("Wygladzanie  {1/12,1/12,1/12,1/12,1/3,1/12,1/12,1/12,1/12}");  //6
    ui->zadaneMaski->addItem("Gauss  {1/16,1/8,1/16,1/8,1/4,1/8,1/16,1/8,1/16}");            //7   
    ui->zadaneMaski->addItem("Detekcja  {-1,0,1,-2,0,2,-1,0,1}");                            //8    
    ui->zadaneMaski->addItem("Detekcja  {-1,-2,-1,0,0,0,1,2,1}");                            //9    
    ui->zadaneMaski->addItem("Detekcja  {-1,0,0,0,1,0,0,0,0}");                              //10    
    ui->zadaneMaski->addItem("Wyostrzanie  {-1,-1,-1,-1, 9,-1,-1,-1,-1}");                   //11    
    ui->zadaneMaski->addItem("Wyostrzanie  { 0,-1, 0,-1, 5,-1, 0,-1, 0}");                   //12    
    ui->zadaneMaski->addItem("Wyostrzanie  { 1,-2, 1,-2, 5,-2, 1,-2, 1}");                   //13   
    ui->zadaneMaski->addItem("Wyostrzanie  { 0,-1, 0,-1,20,-1, 0,-1, 0}");                   //14
    ui->zadaneMaski->addItem("Prewitt N  { 1, 1, 1, 1,-2, 1,-1,-1,-1}");                     //15
    ui->zadaneMaski->addItem("Prewitt W  { 1, 1,-1, 1,-2,-1, 1, 1,-1}");                     //16

    checkbox1==false?ui->checkBox->hide():ui->checkBox->show();
    checkbox2==false?ui->checkBox_2->hide():ui->checkBox_2->show();

    if(slider1==false)
    {
        ui->horizontalSlider->hide();
        ui->spinBox->hide();
    }
    if(slider2==false)
    {
        ui->horizontalSlider_2->hide();
        ui->spinBox_2->hide();
    }
    if(maska==false)
    {
        for (int i = 0; i < 25; ++i)
        {
            maska_wektor[i]->hide();
        }
    }
    if (checkBox_maska5x5 == false)
    {
        ui->checkBox_macierz5x5->hide();
    }
    for (int i = 0; i < 6; ++i)  maska_wektor[i]->hide();
    maska_wektor[9]->hide();
    maska_wektor[10]->hide();
    maska_wektor[14]->hide();
    maska_wektor[15]->hide();
    for (int i = 19; i < 25; ++i)  maska_wektor[i]->hide();
    if(borders==false)
    {
        ui->border_type_1->hide();
        ui->border_type_2->hide();
        ui->border_type_3->hide();
        ui->border_type_4->hide();
    }
    if(zadaneMaski==false)
    {
        ui->zadaneMaski->hide();
    }
    if(zadaneMaskiBlura==false)
    {
        ui->zadaneMaskiBlura->hide();
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
    for (int i = 0, k = 6, m = 11, n = 16; i < 9; ++i)
    {
        if (i < 3)
            maska[i] = maska_wektor[k++]->value();
        else if (i > 2 && i < 6)
            maska[i] = maska_wektor[m++]->value();
        else if (i > 5)
            maska[i] = maska_wektor[n++]->value();
    }
    if (ui->checkBox_macierz5x5->isChecked())
    {
        czy_uzywamy_maski5x5 = true;
        for (int i = 0; i < 25; ++i)
            maska5x5[i] = maska_wektor[i]->value();
    }

    if (ui->border_type_1->isChecked()) border=BORDER_CONSTANT; //w Designerze DEFAULT enabled na tej wartosci
    if (ui->border_type_2->isChecked()) border=BORDER_REPLICATE;
    if (ui->border_type_3->isChecked()) border=BORDER_REFLECT;
    if (ui->border_type_4->isChecked()) border=BORDER_REFLECT_101;

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
    int i;
    int mask_size = 25;
    switch (index) {
        case 0: for (i = 0; i < mask_size; ++i) maska_wektor[i]->setValue(Tablica_MASEK[index][i]); break;
        case 1: for (i = 0; i < mask_size; ++i) maska_wektor[i]->setValue(Tablica_MASEK[index][i]); break;
        case 2: for (i = 0; i < mask_size; ++i) maska_wektor[i]->setValue(Tablica_MASEK[index][i]); break;
        case 3: for (i = 0; i < mask_size; ++i) maska_wektor[i]->setValue(Tablica_MASEK[index][i]); break;
        case 4: for (i = 0; i < mask_size; ++i) maska_wektor[i]->setValue(Tablica_MASEK[index][i]); break;
        case 5: for (i = 0; i < mask_size; ++i) maska_wektor[i]->setValue(Tablica_MASEK[index][i]); break;
        case 6: for (i = 0; i < mask_size; ++i) maska_wektor[i]->setValue(Tablica_MASEK[index][i]); break;
        case 7: for (i = 0; i < mask_size; ++i) maska_wektor[i]->setValue(Tablica_MASEK[index][i]); break;
        case 8: for (i = 0; i < mask_size; ++i) maska_wektor[i]->setValue(Tablica_MASEK[index][i]); break;
        case 9: for (i = 0; i < mask_size; ++i) maska_wektor[i]->setValue(Tablica_MASEK[index][i]); break;
        case 10: for (i = 0; i < mask_size; ++i) maska_wektor[i]->setValue(Tablica_MASEK[index][i]); break;
        case 11: for (i = 0; i < mask_size; ++i) maska_wektor[i]->setValue(Tablica_MASEK[index][i]); break;
        case 12: for (i = 0; i < mask_size; ++i) maska_wektor[i]->setValue(Tablica_MASEK[index][i]); break;
        case 13: for (i = 0; i < mask_size; ++i) maska_wektor[i]->setValue(Tablica_MASEK[index][i]); break;
        case 14: for (i = 0; i < mask_size; ++i) maska_wektor[i]->setValue(Tablica_MASEK[index][i]); break;
        case 15: for (i = 0; i < mask_size; ++i) maska_wektor[i]->setValue(Tablica_MASEK[index][i]); break;
        case 16: for (i = 0; i < mask_size; ++i) maska_wektor[i]->setValue(Tablica_MASEK[index][i]); break;
    }
}

void DialogUiParameters::on_zadaneMaskiBlura_currentIndexChanged(int index)
{
    indeks_maski_wygladzania = index;
}

void DialogUiParameters::on_checkBox_macierz5x5_stateChanged(int prawda)
{
    double temp[] { 0.012,0.024,0.037,0.0246,0.0123,0.024,
                    0.049,0.074,0.0494,0.0247,0.037,0.0741,
                    0.1111,0.0741,0.037,0.0247,0.0494,0.0741,
                    0.0494,0.0247,0.0123,0.0247,0.037,0.0247,0.0123};
    if (prawda){
        for (int i = 0; i < 25; ++i){
            maska_wektor[i]->show();
            maska_wektor[i]->setValue(temp[i]);
        }
    }
    else
    {
        for (int i = 0; i < 6; ++i)  maska_wektor[i]->hide();
        maska_wektor[9]->hide();
        maska_wektor[10]->hide();
        maska_wektor[14]->hide();
        maska_wektor[15]->hide();
        for (int i = 19; i < 25; ++i)  maska_wektor[i]->hide();
    }
}
