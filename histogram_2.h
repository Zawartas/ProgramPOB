#ifndef HISTOGRAM_2_H
#define HISTOGRAM_2_H

#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QPen>
#include <QRect>

class histogram_2 : public QWidget
{
    Q_OBJECT
public:
    explicit histogram_2(QWidget *parent = nullptr);

    void setHistogram(const QVector<int> &histogram);
    void clearHistogram();

protected:
    void paintEvent(QPaintEvent *event = nullptr) Q_DECL_OVERRIDE;

    QVector<int> mHistogram;
};

#endif // HISTOGRAM_2_H
