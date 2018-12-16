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

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class Histogram;
}

class Histogram : public QDialog
{
    Q_OBJECT

public:
    explicit Histogram(const QImage &, QWidget *parent = nullptr);
    ~Histogram();

    void do_histogram(QImage &);

private:
    Ui::Histogram *ui;
    QImage mObraz;
};

#endif // HISTOGRAM_H
