#ifndef DIALOGUIPARAMETERS_H
#define DIALOGUIPARAMETERS_H

#include <QDialog>

enum border_type {BORDER_CONSTANT, BORDER_REPLICATE, BORDER_REFLECT, BORDER_WRAP, NONE};

namespace Ui {
class DialogUiParameters;
}

class DialogUiParameters : public QDialog
{
    Q_OBJECT

public:
    explicit DialogUiParameters(bool checkBox_1 = 0,
                                bool checkBox_2 = 0,
                                bool horizSlider_1 = 0,
                                bool horizSlider_2 = 0,
                                bool matrix = 0,
                                bool bordersRadioButtons = 0, QWidget *parent = nullptr);
    ~DialogUiParameters();

    bool check_dolny_prog;
    bool check_gorny_prog;
    short dolny_prog;
    short gorny_prog;
    double maska[9]; //dane maski 3x3
    border_type border;

private slots:
    void on_buttonBox_accepted();
    void on_horizontalSlider_valueChanged(int value);
    void on_horizontalSlider_2_valueChanged(int value);

    void on_zadaneMaski_currentIndexChanged(int index);

private:
    Ui::DialogUiParameters *ui;

};

#endif // DIALOGUIPARAMETERS_H
