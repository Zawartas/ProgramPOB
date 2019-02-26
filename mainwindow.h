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
#include <vector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    cv::Mat getMatImage() const {return mMatImage;}

    QVector<int> createHistogram(const QImage &);

private slots:
    void otworzPlik();
    bool wczytajPlik(const QString &);
    void actionInvertObrazka();
    void wyswietl(QLabel *, const QImage &);
    void wyswietl(QLabel *, const cv::Mat &);

    void on_actionPokazHistogramOryginalu_triggered();
    void on_actionPokazHistogramZmodyfikowanego_triggered();
    void on_actionPowrotDoOryginalu_triggered();
    void on_actionProgowanieBinarne_triggered();
    void on_actionProgowanieNprogow_triggered();
    void on_actionWyrownanie_triggered();
    void on_actionRozciagniecie_triggered();
    void on_actionLaplacian_triggered();
    void on_actionProgowanieBinarneOdwrocone_triggered();
    void on_actionBinaryzacjaZParametrami_triggered();
    void on_actionProgowanieZachowaniePoziomySzarosci_triggered();
    void on_actionRozciaganieZakresWgParametrow_triggered();
    void on_actionZapiszZmodyfikowanyObrazJako_triggered();
    void on_actionRozjasnianie_triggered();
    void akcjaArytmetyczna();
    void akcjaWygladzania();

    void on_actionExportToCSV_triggered();
    void on_actionWczytajCSV_triggered();
    static void saveMatToCsv(const cv::Mat &, std::string);
    void wczytajPlikCSV(const QString &);
    void implementMask(cv::Mat& Input, cv::Mat& Output, QVector<double>maska, int BORDER_TYPE);

    void on_actionZastosujMaske_triggered();
    void on_actionFiltracja0stopni_triggered();
    void on_actionFiltracja90stopni_triggered();
    void on_actionRoberts_triggered();
    void on_actionDetekcjaSzkieletu_triggered();
    void on_actionErozja_triggered();
    void on_actionDylatacja_triggered();
    void on_actionOtwarcie_triggered();
    void on_actionZamkniecie_triggered();
    void on_actionSegmentacja_Watershed_triggered();
    void on_actionSciemnianie_triggered();
    void on_actionCofnijZmiane_triggered();
    void on_actionZapamietajObrazDoPorownania_triggered();
    void on_actionPokazTabliceEXCEL_triggered();
    void on_actionWczytajCSVdoObrobki_triggered();

    static cv::Mat qimage_to_mat_ref(QImage &img, int format);

private:
    Ui::MainWindow *ui;

    QImage mObrazek;
    QImage mObrazekOryginal;
    QImage mObrazekZmodyfikowany;

    cv::Mat mMatImage;
    cv::Mat mMatOryginal;
    cv::Mat mMatZmodyfikowany;

    QVector<cv::Mat> poprzednieWersje;

    bool mCzyIstniejeZmodyfikowanyObraz {false};
};

#endif // MAINWINDOW_H
