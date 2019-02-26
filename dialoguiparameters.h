#ifndef DIALOGUIPARAMETERS_H
#define DIALOGUIPARAMETERS_H

#include <QDialog>
#include <QDoubleSpinBox>

enum border_type {BORDER_CONSTANT, BORDER_REPLICATE, BORDER_REFLECT, BORDER_REFLECT_101=4};

namespace Ui {
class DialogUiParameters;
}

class DialogUiParameters : public QDialog
{
    Q_OBJECT

public:
    explicit DialogUiParameters(bool checkBox1 = 0,
                                bool checkBox2 = 0,
                                bool Slider1 = 0,
                                bool Slider2 = 0,
                                bool maska = 0,
                                bool borders = 0,
                                bool zadaneMaski = 0,
                                bool zadaneMaskiBlura = 0,
                                bool checkBox_maska5x5 = 0,
                                QWidget *parent = nullptr);
    ~DialogUiParameters();

    bool check_dolny_prog;
    bool check_gorny_prog;
    short dolny_prog;
    short gorny_prog;
    double maska[9]; //dane maski 3x3
    double maska5x5[25]; //dane maski 5x5
    bool czy_uzywamy_maski5x5 = false;
    border_type border;
    int indeks_maski_wygladzania;
    QVector<QDoubleSpinBox *>maska_wektor;
    QVector<double>KURWA;
    QVector<QVector<double>>Tablica_MASEK;

private slots:
    void on_buttonBox_accepted();
    void on_horizontalSlider_valueChanged(int value);
    void on_horizontalSlider_2_valueChanged(int value);

    void on_zadaneMaski_currentIndexChanged(int index);

    void on_zadaneMaskiBlura_currentIndexChanged(int index);

    void on_checkBox_macierz5x5_stateChanged(int arg1);

private:
    Ui::DialogUiParameters *ui;

};

#endif // DIALOGUIPARAMETERS_H
