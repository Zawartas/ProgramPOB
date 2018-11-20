#ifndef DIALOGUIPARAMETERS_H
#define DIALOGUIPARAMETERS_H

#include <QDialog>

namespace Ui {
class DialogUiParameters;
}

class DialogUiParameters : public QDialog
{
    Q_OBJECT

public:
    explicit DialogUiParameters(bool = 1, bool = 1, bool = 1, QWidget *parent = nullptr);
    ~DialogUiParameters();

    bool check_dolny_prog;
    bool check_gorny_prog;
    short dolny_prog;
    short gorny_prog;

private slots:
    void on_buttonBox_accepted();
    void on_horizontalSlider_valueChanged(int value);
    void on_horizontalSlider_2_valueChanged(int value);

private:
    Ui::DialogUiParameters *ui;

};

#endif // DIALOGUIPARAMETERS_H
