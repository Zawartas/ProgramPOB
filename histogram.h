#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QDialog>
#include <QPixmap>
#include <QImage>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class Histogram;
}

class Histogram : public QDialog
{
    Q_OBJECT

public:
    explicit Histogram(const QImage &, QWidget *parent = nullptr);
    explicit Histogram(cv::Mat &, QWidget *parent = nullptr);
    ~Histogram();

    void do_histogram(QImage &);
    void do_histogram(cv::Mat &);

private:
    Ui::Histogram *ui;
    QImage mObraz;
};

#endif // HISTOGRAM_H
