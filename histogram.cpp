#include "histogram.h"
#include "ui_histogram.h"

Histogram::Histogram(const QImage &obraz, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Histogram),
    mObraz(obraz)
{
    ui->setupUi(this);
    do_histogram(mObraz);
}

Histogram::Histogram(cv::Mat &obraz, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Histogram)
{
    ui->setupUi(this);
    do_histogram(obraz);
}

Histogram::~Histogram()
{
    delete ui;
}

void Histogram::do_histogram(QImage &obraz)
{
    QBarSet *dane = new QBarSet("Greyscale");

    QVector<int> tablica(256, 0);

    for (int x = 0; x < obraz.width(); x++)
        for (int y = 0; y < obraz.height(); y++)
        {
            int a = qGray(obraz.pixel(x,y));
            tablica[a]++;
        }

    for (int i = 0; i < 256/*tablica.size()*/; i++)
    {
        *dane << tablica[i];
    }

    QBarSeries *series = new QBarSeries();
    series->append(dane);

    series->setBarWidth(1.2);

    QChart *chart = new QChart();
    chart->addSeries(series);

    chart->setTitle("HISTOGRAM ODCIENI SZAROŚCI");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    for (int i = 0; i < 256 /*tablica.size()*/; i++)
    {
        categories << QString::number(i);
    }
    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);
    chart->axisX()->setGridLineVisible(0);
    chart->axisX()->setLabelsVisible(0);
    chart->axisX()->setLineVisible(0);


    //chart->legend()->setVisible(true);
    //chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *wykresik  = new QChartView(chart);
    wykresik->setRenderHint(QPainter::Antialiasing);

    ui->horizontalLayout->addWidget(wykresik);
}

void Histogram::do_histogram(cv::Mat &obraz)
{
    QBarSet *dane = new QBarSet("Greyscale");

    QVector<int> tablica(256, 0);

    cv::MatIterator_<uchar> it, end;
    for( it = obraz.begin<uchar>(), end = obraz.end<uchar>(); it != end; ++it)
    {
        int a = *it;
        tablica[a]++;
    }

    for (int i = 0; i < 256; i++)
        *dane << tablica[i];

    QBarSeries *series = new QBarSeries();
    series->append(dane);

    series->setBarWidth(1.2);

    QChart *chart = new QChart();
    chart->addSeries(series);

    chart->setTitle("HISTOGRAM ODCIENI SZAROŚCI");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    for (int i = 0; i < 256 /*tablica.size()*/; i++)
    {
        categories << QString::number(i);
    }
    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);
    chart->axisX()->setGridLineVisible(0);
    chart->axisX()->setLabelsVisible(0);
    chart->axisX()->setLineVisible(0);


    //chart->legend()->setVisible(true);
    //chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *wykresik  = new QChartView(chart);
    wykresik->setRenderHint(QPainter::Antialiasing);

    ui->horizontalLayout->addWidget(wykresik);
}
