#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QImage>
#include <QtWidgets>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QSignalMapper>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/ml.hpp" //to read from CSV

#include <fstream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    unsigned short ilosc_przebiegow{};

    cv::Mat matImage() const {return mMatImage;}

private slots:
    void otworzPlik();
    bool wczytajPlik(const QString &);
    void ustawObrazek(const QImage &);
    void ustawInvertObrazka();
    void on_actionPokazHistogramOryginalu_triggered();
    void on_actionPokazHistogramZmodyfikowanego_triggered();
    void on_actionKonwertujNaGrayScale_triggered();
    void on_actionPowrotDoOryginalu_triggered();
    void wyswietl(QLabel *, const QImage &);
    void on_actionProgowanieBinarne_triggered();
    void on_actionProgowanieNprogow_triggered();
    void on_actionWyrownanie_triggered();
    void on_actionRozciagniecie_triggered();
    void on_actionLaplacian_triggered();
    void on_actionExportToCSV_triggered();
    static void saveMatToCsv(const cv::Mat &, std::string);
    void on_actionWczytajCSV_triggered();
    void on_actionProgowanieBinarneOdwrocone_triggered();
    void on_actionBinaryzacjaZParametrami_triggered();
    void on_actionProgowanieZachowaniePoziomySzarosci_triggered();
    void on_actionRozciaganieZakresWgParametrow_triggered();
    void on_actionZapiszZmodyfikowanyObrazJako_triggered();
    static void initializeImageFileDialog(QFileDialog &, QFileDialog::AcceptMode);
    bool saveFile(const QString &fileName);
    void on_actionRozjasnianie_triggered();
//    void on_actionDodawanieObrazow_triggered();
//    void on_actionOdejmowanieObrazow_triggered();
//    void on_actionAND_triggered();

    void akcjaArytmetyczna();
    void akcjaWygladzania();

private:
    Ui::MainWindow *ui;

    QImage mObrazek;
    QImage mObrazekOryginal;
    QImage mObrazekDwa; //do doawania,mnozenia, odejmowania i tak dalej
    QImage mObrazekZmodyfikowany;
    cv::Mat mMatImage;
    cv::Mat mMatImageDwa;
    bool mCzyIstniejeZmodyfikowanyObraz {false};
};

#endif // MAINWINDOW_H
