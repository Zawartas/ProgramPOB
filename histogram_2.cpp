#include "histogram_2.h"
#include"QtDebug"

histogram_2::histogram_2(QWidget *parent) : QWidget(parent)
{

}

void histogram_2::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    int max_hist_val = 0;
    for (auto &x: mHistogram)
        max_hist_val=x>max_hist_val?x:max_hist_val;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    if (mHistogram.isEmpty()) {
        painter.fillRect(0, 0, width(), height(), QColor::fromRgb(200, 200, 200));
        return;
    }

    double barWidth = (width()*0.96) / static_cast<double>(mHistogram.size()); //szerokosc slupka
    double wspolczynnik_proporcji = height()/(static_cast<double>(max_hist_val*1.20));

    for (int i = 0; i < mHistogram.size(); ++i)
    {
        int h = static_cast<int>(mHistogram[i]*wspolczynnik_proporcji);
        int nizej = 20;
        //painter.drawLine(10, nizej, 10, height() - nizej); //Y
        painter.drawLine(10, height() - nizej, width()-5, height() - nizej); //X

        QFont font;
        font.setPixelSize(12);
        painter.setFont(font);
        painter.drawText(0, 15, "H I S T O G R A M");
        painter.drawText(10, height(), "0 - CZARNY");
        painter.drawText(width() - 70, height(), "BIAŁY - 255");

        QPen pen = painter.pen();
        pen.setStyle(Qt::NoPen);
        QRect r1((barWidth * i) + 10, height() - h - nizej, barWidth, mHistogram[i]*wspolczynnik_proporcji);
        //painter.drawRect(r1);
        painter.fillRect(r1, Qt::red);
    }
}

void histogram_2::setHistogram(const QVector<int> &histogram)
{
    mHistogram = histogram;
    repaint();
}

void histogram_2::clearHistogram()
{
    mHistogram.clear();
}
