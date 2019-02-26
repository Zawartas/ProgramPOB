#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "histogram.h"
#include "histogram_2.h"
#include "dialoguiparameters.h"
#include <QSizePolicy>

constexpr int MAX_SIZE_OF_UNDO = 5;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionOtworzPlik, &QAction::triggered, this, &MainWindow::otworzPlik);
    connect(ui->actionInvert, &QAction::triggered, this, &MainWindow::actionInvertObrazka);

    connect(ui->actionDodawanieObrazow, SIGNAL(triggered()), this, SLOT(akcjaArytmetyczna()));
    connect(ui->actionOdejmowanieObrazow, SIGNAL(triggered()), this, SLOT(akcjaArytmetyczna()));
    connect(ui->actionOdejmowanieAbs, SIGNAL(triggered()), this, SLOT(akcjaArytmetyczna()));
    connect(ui->actionAND, SIGNAL(triggered()), this, SLOT(akcjaArytmetyczna()));
    connect(ui->actionOR, SIGNAL(triggered()), this, SLOT(akcjaArytmetyczna()));
    connect(ui->actionXOR, SIGNAL(triggered()), this, SLOT(akcjaArytmetyczna()));

    connect(ui->actionGaussian, SIGNAL(triggered()), this, SLOT(akcjaWygladzania()));
    connect(ui->actionBlur, SIGNAL(triggered()), this, SLOT(akcjaWygladzania()));
    connect(ui->actionMedianBlur, SIGNAL(triggered()), this, SLOT(akcjaWygladzania()));

    ui->actionCofnijZmiane->setEnabled(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}

/*----------------------------------------------------------------*/
/*----------------------------------------------------------------*/
/*-------FUNKCJE ZWIAZANE Z WCZYTANIEM/ZAPISANIEM PLIKU-----------*/
/*----------------------------------------------------------------*/
/*----------------------------------------------------------------*/
void MainWindow::otworzPlik()
{

    QString filters = "JPG files (*.jpg);;PNG files (*.png);;BMP files (*.bmp);;CSV files (*.csv)";
    QFileDialog dialog(this);

    const QString picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).last();
    //dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    QString nazwa_pliku = dialog.getOpenFileName(this, "Otwórz plik", picturesLocations, filters);

    int kropka = nazwa_pliku.lastIndexOf('.');
    QString postfix = nazwa_pliku.mid(kropka+1);
    if (postfix.toLower() == "csv")
        wczytajPlikCSV(nazwa_pliku);
    else
        wczytajPlik(nazwa_pliku);
}

bool MainWindow::wczytajPlik(const QString & nazwa_pliku)
{
    if (!mObrazek.load(nazwa_pliku)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load file"));
        return false;
    }

    mMatImage = cv::imread(nazwa_pliku.toStdString(), 0);   //wczytujemy juz jako Grayscale '0'
        poprzednieWersje.clear();                           //czyscimy tablice z poprzednimi wersjami
        mMatZmodyfikowany = mMatImage;
        poprzednieWersje.append(mMatZmodyfikowany.clone());         //ladujemy tam oryginal
    mMatOryginal = mMatImage.clone();                       //KOPIA bezpieczenstwa

    mObrazek = mObrazek.convertToFormat(QImage::Format_Grayscale8); //convert obrazka na 8 bitowy Grayscale

    mObrazekOryginal = mObrazek;
    mObrazekZmodyfikowany = QImage();

    ui->labelOutputPicture->clear();
    ui->histogramCentralny->clearHistogram();


    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4, Type: %5, Grayscale: %6")
        .arg(QDir::toNativeSeparators(nazwa_pliku)).arg(mObrazek.width()).arg(mObrazek.height()).arg(mObrazek.depth()).arg(mObrazek.format()).arg(mObrazek.allGray());
    statusBar()->showMessage(message);

    wyswietl(ui->labelObrazek, mObrazek);
    return true;
}

void MainWindow::on_actionZapiszZmodyfikowanyObrazJako_triggered()
{
    QFileDialog dialog;
    dialog.setNameFilter(tr("Images (*.png *.jpg)"));
    const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    QString file_name = dialog.getSaveFileName(this, "Zapisz plik jako");
    QString suffix = ".jpg";
    if (!file_name.contains(suffix, Qt::CaseInsensitive))
        file_name += suffix;
    mObrazekZmodyfikowany.save(file_name, "jpg", 100);
}

/*----------------------------------------------------------------*/
/*----------------------------------------------------------------*/
/*---------------------FUNKCJE TECHNICZNE-------------------------*/
/*----------------------------------------------------------------*/
/*----------------------------------------------------------------*/

void MainWindow::on_actionPowrotDoOryginalu_triggered()
{
    mObrazek = mObrazekOryginal;
    mMatZmodyfikowany.release();

    poprzednieWersje.clear();
    poprzednieWersje.append(mMatImage.clone());

    wyswietl(ui->labelOutputPicture, mObrazek);
}

void MainWindow::wyswietl(QLabel *gdzie, const QImage &obrazek) //f-cja wyswietla dany obrazek w danym panelu
{
    if (poprzednieWersje.size() > 1)
            ui->actionCofnijZmiane->setEnabled(true);

    int width = gdzie->width();
    int height = gdzie->height();
    if (width < obrazek.width() || height < obrazek.height())
        gdzie->setPixmap(QPixmap::fromImage(obrazek).scaled(width, height, Qt::KeepAspectRatio));
    else
        gdzie->setPixmap(QPixmap::fromImage(obrazek));

    gdzie->setAlignment(Qt::AlignCenter);

    QVector<int> histogram (256, 0);
    histogram = createHistogram(obrazek);

    if (gdzie == this->ui->labelObrazek){
        ui->histogramLewy->setHistogram(histogram);
    }
    else if (gdzie == this->ui->labelOutputPicture){
        ui->histogramCentralny->setHistogram(histogram);
        ui->histogramPrawy->repaint();
    }
    else if (gdzie == this->ui->labelOutputPicture2){
        ui->histogramPrawy->setHistogram(histogram);
        ui->histogramPrawy->repaint();
    }
}

void MainWindow::wyswietl(QLabel *gdzie, const cv::Mat &obrazek) //f-cja wyswietla dany obrazek w danym panelu
{
    mObrazekZmodyfikowany = QImage(static_cast<uchar*>(obrazek.data),
                                   obrazek.cols,
                                   obrazek.rows,
                                   static_cast<int>(obrazek.step),
                                   QImage::Format_Grayscale8);

    if (poprzednieWersje.size() > 1)
            ui->actionCofnijZmiane->setEnabled(true);

    int width = gdzie->width();
    int height = gdzie->height();
    if (width < mObrazekZmodyfikowany.width() || height < mObrazekZmodyfikowany.height())
        gdzie->setPixmap(QPixmap::fromImage(mObrazekZmodyfikowany).scaled(width, height, Qt::KeepAspectRatio));
    else
        gdzie->setPixmap(QPixmap::fromImage(mObrazekZmodyfikowany));

    gdzie->setAlignment(Qt::AlignCenter);

    QVector<int> histogram (256, 0);
    histogram = createHistogram(mObrazekZmodyfikowany);

    if (gdzie == this->ui->labelObrazek){
        ui->histogramLewy->setHistogram(histogram);
    }
    else if (gdzie == this->ui->labelOutputPicture){
        ui->histogramCentralny->setHistogram(histogram);
        ui->histogramPrawy->repaint();
    }
    else if (gdzie == this->ui->labelOutputPicture2){
        ui->histogramPrawy->setHistogram(histogram);
        ui->histogramPrawy->repaint();
    }
}

cv::Mat MainWindow::qimage_to_mat_ref(QImage &img, int format)
{
    return cv::Mat(img.height(), img.width(),
            format, img.bits(), img.bytesPerLine());
}

QVector<int> MainWindow::createHistogram(const QImage & obraz)
{
    QVector<int> histogram(256,0);
    for (int x = 0; x < obraz.width(); x++)
        for (int y = 0; y < obraz.height(); y++)
        {
            int a = qGray(obraz.pixel(x,y));
            histogram[a]++;
        }
    return histogram;
}



void MainWindow::on_actionCofnijZmiane_triggered()
{
    if (poprzednieWersje.size() > 1)
    {
        poprzednieWersje.last().release(); //czyscimy pamiec po tym Mat
        poprzednieWersje.removeLast(); //usuwamy go z wektora
        if (poprzednieWersje.size() == 1)
            ui->actionCofnijZmiane->setEnabled(false);
        mMatZmodyfikowany = poprzednieWersje.last().clone();

        wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
    }
}

void MainWindow::on_actionZapamietajObrazDoPorownania_triggered()
{
    if (mCzyIstniejeZmodyfikowanyObraz)
    {
        wyswietl(ui->labelOutputPicture2, poprzednieWersje.last());
    }
}

/*----------------------------------------------------------------*/
/*----------------------------------------------------------------*/
/*----------------WYSWIETLENIE HISTOGRAMU-------------------------*/
/*----------------------------------------------------------------*/
/*----------------------------------------------------------------*/
void MainWindow::on_actionPokazHistogramOryginalu_triggered()
{
    Histogram *hist_oryginalu = new Histogram(mObrazek);
    hist_oryginalu->setMinimumSize(900,300);
    hist_oryginalu->setWindowTitle("Histogram obrazu oryginalnego");
    hist_oryginalu->show();
}

void MainWindow::on_actionPokazHistogramZmodyfikowanego_triggered()
{
    if (mCzyIstniejeZmodyfikowanyObraz)
    {
        Histogram *hist_zmodyfik = new Histogram(poprzednieWersje.last());
        hist_zmodyfik->setMinimumSize(900,300);
        hist_zmodyfik->setWindowTitle("Histogram obrazu zmodyfikowanego");
        hist_zmodyfik->show();
    }
    else
        QMessageBox::information(this,"UWAGA","Nie dokonano żadnej modyfikacji.\n Brak danych dla Histogramu.");
}

/*----------------------------------------------------------------*/
/*----------------------------------------------------------------*/
/*------------------FUNKCJE ZWIAZANE Z CSV------------------------*/
/*----------------------------------------------------------------*/
/*----------------------------------------------------------------*/
void MainWindow::on_actionExportToCSV_triggered()
{
    //w tej chwili podany jest oryginalnie wczytany mMatimage - ale moze byc to jakikolwiek Mat
    bool ok;
    QString nazwa_pliku = QInputDialog::getText(this,
                                                "Podaj nazwę pliku:",
                                                "",QLineEdit::Normal,
                                                "output.csv", &ok);
    if (ok && !nazwa_pliku.isEmpty())
        saveMatToCsv(poprzednieWersje.last(), nazwa_pliku.toStdString());
}

void MainWindow::on_actionWczytajCSV_triggered()
{
    QString filters = "CSV files (*.csv)";
    QFileDialog dialog(this);

    const QString picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).last();
    //dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    QString nazwa_pliku = dialog.getOpenFileName(this, "Otwórz plik", picturesLocations, filters);

    cv::Mat img;
    cv::Ptr<cv::ml::TrainData> raw_data;
    cv::String sciezka = nazwa_pliku.toStdString();
    raw_data = cv::ml::TrainData::loadFromCSV(sciezka, 0);
    cv::Mat data = raw_data->getSamples();
    data.convertTo(img, CV_8U);
        //img = img.reshape(3); //set number of channels

        // set the image type
        img.convertTo(img, CV_8U);

        cv::namedWindow(nazwa_pliku.toStdString(), cv::WINDOW_NORMAL|cv::WINDOW_GUI_NORMAL);
        cv::imshow(nazwa_pliku.toStdString(), img);
}

void MainWindow::on_actionWczytajCSVdoObrobki_triggered()
{
    QString filters = "CSV files (*.csv)";
    QFileDialog dialog(this);

    const QString picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).last();
    //dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    QString nazwa_pliku = dialog.getOpenFileName(this, "Otwórz plik", picturesLocations, filters);

    cv::Mat img;
    cv::Ptr<cv::ml::TrainData> raw_data;
    cv::String sciezka = nazwa_pliku.toStdString();
    raw_data = cv::ml::TrainData::loadFromCSV(sciezka, 0);
    cv::Mat data = raw_data->getSamples();
    data.convertTo(img, CV_8U);
    img.convertTo(img, CV_8U);
    mMatImage       = img.clone();
    mMatOryginal    = img.clone();                          //KOPIA bezpieczenstwa

        poprzednieWersje.clear();                           //czyscimy tablice z poprzednimi wersjami
        poprzednieWersje.append(mMatImage.clone());         //ladujemy tam oryginal

    mObrazek = QImage(static_cast<uchar*>(mMatImage.data),
                       mMatImage.cols,
                       mMatImage.rows,
                       static_cast<int>(mMatImage.step),
                       QImage::Format_Grayscale8);
    mObrazekOryginal = mObrazek;
    mObrazekZmodyfikowany = QImage();

        ui->labelOutputPicture->clear();
        ui->histogramCentralny->clearHistogram();

        wyswietl(ui->labelObrazek, mObrazek);
}

void MainWindow::saveMatToCsv(const cv::Mat &matrix, std::string filename)
{
    std::ofstream outputFile(filename);
    outputFile << cv::format(matrix, 2) << std::endl;
    outputFile.close();
}

void MainWindow::wczytajPlikCSV(const QString & nazwa_pliku)
{
    cv::Mat img;
    cv::Ptr<cv::ml::TrainData> raw_data;
    cv::String sciezka = nazwa_pliku.toStdString();
    raw_data = cv::ml::TrainData::loadFromCSV(sciezka, 0);
        cv::Mat data = raw_data->getSamples();
        data.convertTo(img, CV_8U);
        //img = img.reshape(3); //set number of channels

        // set the image type
        img.convertTo(img, CV_8U);
        mMatImage       = img.clone();
        mMatOryginal    = img.clone();                          //KOPIA bezpieczenstwa

            poprzednieWersje.clear();                           //czyscimy tablice z poprzednimi wersjami
            poprzednieWersje.append(mMatImage.clone());         //ladujemy tam oryginal

        mObrazek = QImage(static_cast<uchar*>(mMatImage.data),
                           mMatImage.cols,
                           mMatImage.rows,
                           static_cast<int>(mMatImage.step),
                           QImage::Format_Grayscale8);
        mObrazekOryginal = mObrazek;
        mObrazekZmodyfikowany = QImage();

        ui->labelOutputPicture->clear();
        ui->histogramCentralny->clearHistogram();

        wyswietl(ui->labelObrazek, mObrazek);
}

void MainWindow::on_actionPokazTabliceEXCEL_triggered()
{
    cv::namedWindow("tablica EXCEL dla obrazu", cv::WINDOW_NORMAL|cv::WINDOW_GUI_NORMAL);
    cv::imshow("tablica EXCEL dla obrazu", poprzednieWersje.last());
}


/*----------------------------------------------------------------*/
/*----------------------------------------------------------------*/
/*----------------FUNKCJE MODYFIKACJE OBRAZU----------------------*/
/*----------------------------------------------------------------*/
/*----------------------------------------------------------------*/

void MainWindow::actionInvertObrazka()
{
    mCzyIstniejeZmodyfikowanyObraz = true;

    mMatZmodyfikowany = poprzednieWersje.last().clone();
    cv::bitwise_not(mMatZmodyfikowany, mMatZmodyfikowany);

    if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO)
                poprzednieWersje.removeFirst();

    if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeFirst();
    poprzednieWersje.append(mMatZmodyfikowany.clone());

    wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
}

void MainWindow::on_actionProgowanieBinarne_triggered() //progowanie na 0 lub 255
{
    mCzyIstniejeZmodyfikowanyObraz = true;
    mMatZmodyfikowany = poprzednieWersje.last().clone();

    cv::MatIterator_<uchar> it, end;
    for( it = mMatZmodyfikowany.begin<uchar>(), end = mMatZmodyfikowany.end<uchar>(); it != end; ++it)
    {
        int gray = *it;
        if (gray < 128)
            *it = 0;
        else
            *it = 255;
    }

    if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeFirst();
    poprzednieWersje.append(mMatZmodyfikowany.clone());

    wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
}

void MainWindow::on_actionProgowanieNprogow_triggered()
{
    bool ok;
    int ilosc_progow = QInputDialog::getInt(this,
                                            "Parametryzacja",
                                            "Podaj ilość progów:",
                                            QLineEdit::Normal,
                                            1,255,1,&ok);

    mCzyIstniejeZmodyfikowanyObraz = true;
    mMatZmodyfikowany = poprzednieWersje.last().clone();

    double prog = 255 - (255/((ilosc_progow+1)*2));
    int ilosc_przebiegow = 0;

        while (prog > 0)
        {
            cv::MatIterator_<uchar> oryg, it, end;
            for( it = mMatZmodyfikowany.begin<uchar>(),
                 oryg = poprzednieWersje.last().begin<uchar>(),
                 end = mMatZmodyfikowany.end<uchar>(); it != end; ++it, ++oryg)
            {
                int gray = *oryg;
                if (gray < prog)
                {
                    *it = static_cast<unsigned char>(prog-(255/((ilosc_progow+1)*2)));
                }
                else if (ilosc_przebiegow == 0 && gray >= prog) //to sie uruchamia tylko w pierwszym przebiegu zeby wypelnic piksele od granicy ost progu do 255
                {
                    *it = 255;
                }
            }
            prog -= 255/(ilosc_progow+1);
            ilosc_przebiegow++;
        }

    if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeFirst();
    poprzednieWersje.append(mMatZmodyfikowany.clone());
    wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
}

void MainWindow::on_actionWyrownanie_triggered()
{
    mCzyIstniejeZmodyfikowanyObraz = true;

    QVector<unsigned int> tablica(256, 0);
    for (int x = 0; x < mObrazek.width(); x++)
        for (int y = 0; y < mObrazek.height(); y++)
        {
            int a = qGray(mObrazek.pixel(x,y));
            tablica[a]++;
        }

    QVector<unsigned long> dystrybuanta(256, 0);
    dystrybuanta[0] = tablica[0];
    int min_dystrybuanty{0};
    for(int i = 1; i < 256; i++)
    {
        dystrybuanta[i] = dystrybuanta[i-1] + tablica[i];
        min_dystrybuanty = dystrybuanta[i]<dystrybuanta[i-1]?dystrybuanta[i]:min_dystrybuanty;
    }

    int ilosc_pikseli = mObrazek.width() * mObrazek.height();
    QVector<unsigned char> dystrybuanta_zmodyfikowana(256, 0);
    for(int i = 0; i < 256; i++)
    {
        dystrybuanta_zmodyfikowana[i] = static_cast<double>((dystrybuanta[i] - min_dystrybuanty))/static_cast<double>(ilosc_pikseli - min_dystrybuanty) * 255;
    }

    mMatZmodyfikowany = poprzednieWersje.last().clone();

    cv::MatIterator_<uchar> it, end;
    for( it = mMatZmodyfikowany.begin<uchar>(), end = mMatZmodyfikowany.end<uchar>(); it != end; ++it)
    {
        int index = *it;
        *it = dystrybuanta_zmodyfikowana[index];
    }

    if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeFirst();
    poprzednieWersje.append(mMatZmodyfikowany.clone());
    wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
}

void MainWindow::on_actionRozciagniecie_triggered()
{
    mCzyIstniejeZmodyfikowanyObraz = true;
    mMatZmodyfikowany = poprzednieWersje.last().clone();

    QVector<unsigned int> tablica(256, 0);

    for (int x = 0; x < mObrazek.width(); x++)
        for (int y = 0; y < mObrazek.height(); y++)
        {
            int a = qGray(mObrazek.pixel(x,y));
            tablica[a]++;
        }

    int minGrayValue = 0, maxGrayValue = 0;
    for (int i = 0; i < 256; i++) if(tablica[i] > 0) {minGrayValue = i; break;}
    for (int i = 255; i >= 0; i--) if(tablica[i] > 0) {maxGrayValue = i; break;}

    cv::MatIterator_<uchar> it, end;
    for( it = mMatZmodyfikowany.begin<uchar>(), end = mMatZmodyfikowany.end<uchar>(); it != end; ++it)
    {
        int gray = *it;
        int mod_kol = (256/static_cast<double>(maxGrayValue-minGrayValue))*(gray-minGrayValue);
        mod_kol = mod_kol<0?0:mod_kol;
        mod_kol = mod_kol>255?255:mod_kol;
        *it = mod_kol;
    }
    if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeFirst();
    poprzednieWersje.append(mMatZmodyfikowany.clone());
    wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
}

void MainWindow::on_actionLaplacian_triggered()
{
    mMatZmodyfikowany = poprzednieWersje.last().clone();
    cv::Laplacian(mMatImage, mMatZmodyfikowany, -1);

    if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeFirst();
    poprzednieWersje.append(mMatZmodyfikowany.clone());
    wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
}

void MainWindow::on_actionProgowanieBinarneOdwrocone_triggered()
{
    mCzyIstniejeZmodyfikowanyObraz = true;
    mMatZmodyfikowany = poprzednieWersje.last().clone();

    cv::MatIterator_<uchar> it, end;
    for( it = mMatZmodyfikowany.begin<uchar>(), end = mMatZmodyfikowany.end<uchar>(); it != end; ++it)
    {
        int gray = *it;
        if (gray > 128)
            *it = 0;
        else
            *it = 255;
    }

    if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeFirst();
    poprzednieWersje.append(mMatZmodyfikowany.clone());
    wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
}

void MainWindow::on_actionBinaryzacjaZParametrami_triggered()
{
    DialogUiParameters *okno_na_parametry = new DialogUiParameters(1,1,1,1);
    okno_na_parametry->show();

    short int dol = 0, gora = 255;
    bool dol_bool = 0, gora_bool = 0;
    if (okno_na_parametry->exec() == QDialog::Accepted)
    {
        dol = okno_na_parametry->dolny_prog;
        gora = okno_na_parametry->gorny_prog;
        dol_bool = okno_na_parametry->check_dolny_prog;
        gora_bool = okno_na_parametry->check_gorny_prog;


        mCzyIstniejeZmodyfikowanyObraz = true;
        cv::MatIterator_<uchar> it, end;

        if (dol_bool && !gora_bool)
        {
            for( it = mMatZmodyfikowany.begin<uchar>(), end = mMatZmodyfikowany.end<uchar>(); it != end; ++it)
            {
                    int gray = *it;
                    if (gray < dol)
                        *it = 0;
                    else
                        *it = 255;
            }
        }
        else if (!dol_bool && gora_bool)
        {
            for( it = mMatZmodyfikowany.begin<uchar>(), end = mMatZmodyfikowany.end<uchar>(); it != end; ++it)
            {
                    int gray = *it;
                    if (gray > gora)
                        *it = 0;
                    else
                        *it = 255;
            }
        }
        else if (dol_bool && gora_bool && gora>dol)
        {
            for( it = mMatZmodyfikowany.begin<uchar>(), end = mMatZmodyfikowany.end<uchar>(); it != end; ++it)
            {
                    int gray = *it;
                    if (gray > dol && gray < gora) *it = 0;
                    else *it = 255;
            }
        }
        if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeFirst();
        poprzednieWersje.append(mMatZmodyfikowany.clone());
    }

    wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
}

void MainWindow::on_actionProgowanieZachowaniePoziomySzarosci_triggered()
{
    DialogUiParameters *okno_na_parametry = new DialogUiParameters(0, 0, 1);
    okno_na_parametry->show();

    short int dol = 0, gora = 255;
    if (okno_na_parametry->exec() == QDialog::Accepted)
    {
        dol = okno_na_parametry->dolny_prog;
        gora = okno_na_parametry->gorny_prog;

        mCzyIstniejeZmodyfikowanyObraz = true;

        cv::MatIterator_<uchar> it, end;
        for( it = mMatZmodyfikowany.begin<uchar>(), end = mMatZmodyfikowany.end<uchar>(); it != end; ++it)
        {
                int gray = *it;
                if (gray < dol)
                    *it = 0;
                else if (gray > gora)
                    *it = 255;
                else
                    continue;
        }
        if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeFirst();
        poprzednieWersje.append(mMatZmodyfikowany.clone());
    }
    wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
}

void MainWindow::on_actionRozciaganieZakresWgParametrow_triggered()
{
    DialogUiParameters *okno_na_parametry = new DialogUiParameters(0, 0, 1);
    okno_na_parametry->show();

    short int dol = 0, gora = 255;
    if (okno_na_parametry->exec() == QDialog::Accepted)
    {
        dol = okno_na_parametry->dolny_prog;
        gora = okno_na_parametry->gorny_prog;


        mCzyIstniejeZmodyfikowanyObraz = true;
        mObrazekZmodyfikowany = QImage(static_cast<uchar*>(poprzednieWersje.last().data),
                                       poprzednieWersje.last().cols,
                                       poprzednieWersje.last().rows,
                                       static_cast<int>(poprzednieWersje.last().step),
                                       QImage::Format_Grayscale8);

        for (int i = 0; i < mObrazekZmodyfikowany.height(); i++)
        {
            QRgb *pixel = reinterpret_cast<QRgb*>(mObrazekZmodyfikowany.scanLine(i));
            QRgb *end = pixel + mObrazekZmodyfikowany.width();
            for (; pixel != end; pixel++)
            {
                int gray = qGray(*pixel);
                if (gray > dol && gray <= gora)
                {
                    int mod_kol = (256/static_cast<double>(gora-dol))*(gray-dol);
                    mod_kol = mod_kol<0?0:mod_kol;
                    mod_kol = mod_kol>255?255:mod_kol;
                    *pixel = QColor(mod_kol, mod_kol, mod_kol).rgb();
                }
                else
                    *pixel = QColor(0, 0, 0).rgb();
            }
        }
        if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeFirst();
        poprzednieWersje.append(qimage_to_mat_ref(mObrazekZmodyfikowany, CV_8U));
    }

    wyswietl(ui->labelOutputPicture, mObrazekZmodyfikowany);
}

void MainWindow::on_actionRozjasnianie_triggered()
{
    DialogUiParameters *okno_na_parametry = new DialogUiParameters(0, 0, 1);
    okno_na_parametry->show();

    short int dol = 0;
    if (okno_na_parametry->exec() == QDialog::Accepted)
    {
        dol = okno_na_parametry->dolny_prog;

        mCzyIstniejeZmodyfikowanyObraz = true;
        mMatZmodyfikowany = poprzednieWersje.last().clone();

        cv::MatIterator_<uchar> it, end;
        for( it = mMatZmodyfikowany.begin<uchar>(), end = mMatZmodyfikowany.end<uchar>(); it != end; ++it)
        {
            short gray = *it;
            gray = gray + dol;
            gray = gray<0?0:gray;
            gray = gray>255?255:gray;
            *it = gray;
        }

        if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO)
            poprzednieWersje.removeFirst();

        poprzednieWersje.append(mMatZmodyfikowany.clone());
    }

    wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
}

void MainWindow::on_actionSciemnianie_triggered()
{
    DialogUiParameters *okno_na_parametry = new DialogUiParameters(0, 0, 1);
    okno_na_parametry->show();

    short int gora = 0;
    if (okno_na_parametry->exec() == QDialog::Accepted)
    {
        gora = okno_na_parametry->dolny_prog;

        mCzyIstniejeZmodyfikowanyObraz = true;
        mMatZmodyfikowany = poprzednieWersje.last().clone();

        cv::MatIterator_<uchar> it, end;
        for( it = mMatZmodyfikowany.begin<uchar>(), end = mMatZmodyfikowany.end<uchar>(); it != end; ++it)
        {
            short gray = *it;
            gray = gray - gora;
            gray = gray<0?0:gray;
            gray = gray>255?255:gray;
            *it = gray;
        }

        if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO)
            poprzednieWersje.removeFirst();

        poprzednieWersje.append(mMatZmodyfikowany.clone());
    }
    wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
}

void::MainWindow::akcjaArytmetyczna()
{
    //otwieramy OPEN DIALOG dla obrazu ktory chcemy dodac do naszego obrazu
    //nalezaloby wpierw sprawdzic cy istnieje oryginalny obraz
    if (!mObrazek.isNull())
    {
        QString filtr_rozszerzen = "Pliki JPG (*.jpg)";
        QString nazwa_pliku = QFileDialog::getOpenFileName(this, "Otwórz plik do dodania", QDir::homePath(), filtr_rozszerzen);
        cv::Mat secondPicture;
        secondPicture = cv::imread(nazwa_pliku.toStdString(), 0);


        if (secondPicture.empty()) {
            QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                     tr("Cannot load %1")
                                     .arg(QDir::toNativeSeparators(nazwa_pliku)));
        }
        else if (secondPicture.size() != mMatImage.size())
        {
            QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                     tr("Obrazy rożnych rozmiarów."));
        }
        else
        {
            cv::imshow("Dodawany obraz", secondPicture);
            QMessageBox::StandardButton odpowiedz;
            QObject* button = QObject::sender();

            if (button == ui->actionDodawanieObrazow)
            {
                odpowiedz = QMessageBox::question(this, "Pytanie", "Dodać obrazy?");
                if (odpowiedz == QMessageBox::Yes)
                {
                    mMatZmodyfikowany = mMatImage + secondPicture;
                }
            }
            else if (button == ui->actionOdejmowanieObrazow)
            {
                odpowiedz = QMessageBox::question(this, "Pytanie", "Odjąć obrazy?");
                if (odpowiedz == QMessageBox::Yes)
                {
                    cv::subtract(mMatImage, secondPicture, mMatZmodyfikowany);
                }
            }
            else if (button == ui->actionOdejmowanieAbs)
            {
                odpowiedz = QMessageBox::question(this, "Pytanie", "Odjąć obrazy (z modułem)?");
                if (odpowiedz == QMessageBox::Yes)
                {
                    cv::absdiff(mMatImage, secondPicture, mMatZmodyfikowany);
                }
            }
            else if (button == ui->actionAND)
            {
                odpowiedz = QMessageBox::question(this, "Pytanie", "Zastosować AND dla obrazów?");
                if (odpowiedz == QMessageBox::Yes)
                {
                    cv::bitwise_and(mMatImage, secondPicture, mMatZmodyfikowany);
                }
            }
            else if (button == ui->actionOR)
            {
                odpowiedz = QMessageBox::question(this, "Pytanie", "Zastosować OR dla obrazów?");
                if (odpowiedz == QMessageBox::Yes)
                {
                    cv::bitwise_or(mMatImage, secondPicture, mMatZmodyfikowany);
                }
            }
            else if (button == ui->actionXOR)
            {
                odpowiedz = QMessageBox::question(this, "Pytanie", "Zastosować XOR dla obrazów?");
                if (odpowiedz == QMessageBox::Yes)
                {
                    cv::bitwise_xor(mMatImage, secondPicture, mMatZmodyfikowany);
                }
            }

        if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeFirst();
        poprzednieWersje.append(mMatZmodyfikowany.clone());
        wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
        }
    }
}

void::MainWindow::akcjaWygladzania()
{
    QObject* button = QObject::sender();
    if (button == ui->actionBlur)
    {
        bool ok;
        int parametr = QInputDialog::getInt(this, "Parametryzacja",
                                                 "Podaj siłę blura:", QLineEdit::Normal,
                                                 1,255,2,&ok);
        if (parametr%2 == 0) parametr++;

        mCzyIstniejeZmodyfikowanyObraz = true;

        cv::blur(poprzednieWersje.last(), mMatZmodyfikowany, cv::Size(parametr, parametr)); //to 3 moznaby zdialogowac

        if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeFirst();
        poprzednieWersje.append(mMatZmodyfikowany.clone());
        wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
    }
    else if (button == ui->actionGaussian)
    {
        bool ok;
        int parametr = QInputDialog::getInt(this, "Parametryzacja",
                                                 "Podaj siłę gausa:", QLineEdit::Normal,
                                                 1,255,2,&ok);
        if (parametr%2 == 0) parametr++;

        mCzyIstniejeZmodyfikowanyObraz = true;

        cv::GaussianBlur(poprzednieWersje.last(), mMatZmodyfikowany, cv::Size(parametr, parametr), 0, 0); //to 3 moznaby zdialogowac

        if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeFirst();
        poprzednieWersje.append(mMatZmodyfikowany.clone());
        wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
    }
    else if (button == ui->actionMedianBlur)
    {
        DialogUiParameters *okno_na_parametry = new DialogUiParameters(0, 0, 0, 0, 0, 1, 0, 1);
        okno_na_parametry->show();
        int parametr;
        if (okno_na_parametry->exec() == QDialog::Accepted)
        {
            parametr = okno_na_parametry->indeks_maski_wygladzania;
            qDebug() << parametr;
            qDebug() << okno_na_parametry->border;

            mCzyIstniejeZmodyfikowanyObraz = true;

            switch (parametr)
            {
                case 0:
                    cv::blur(poprzednieWersje.last(), mMatZmodyfikowany, cv::Size(3,3), cv::Point(-1,-1), okno_na_parametry->border);
                    break;
                case 1:
                    cv::blur(poprzednieWersje.last(), mMatZmodyfikowany, cv::Size(3,5), cv::Point(-1,-2), okno_na_parametry->border);
                    break;
                case 2:
                    cv::blur(poprzednieWersje.last(), mMatZmodyfikowany, cv::Size(5,3), cv::Point(-2,-1), okno_na_parametry->border);
                    break;
                case 3:
                    cv::blur(poprzednieWersje.last(), mMatZmodyfikowany, cv::Size(5,5), cv::Point(-2,-2), okno_na_parametry->border);
                    break;
                case 4:
                    cv::blur(poprzednieWersje.last(), mMatZmodyfikowany, cv::Size(7,7), cv::Point(-3,-3), okno_na_parametry->border);
                    break;
                default:
                    cv::blur(poprzednieWersje.last(), mMatZmodyfikowany, cv::Size(1,1), cv::Point(-1,-1), okno_na_parametry->border);
            }
            if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeFirst();
            poprzednieWersje.append(mMatZmodyfikowany.clone());
            wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
        }

    }

}

void MainWindow::on_actionZastosujMaske_triggered()
{
    DialogUiParameters *okno_na_parametry = new DialogUiParameters(0, 0, 0, 0, 1, 1, 1, 0, 1);
    okno_na_parametry->show();
    if (okno_na_parametry->exec() == QDialog::Accepted)
    {
        mCzyIstniejeZmodyfikowanyObraz = true;

        QVector<double> maska;
        if (okno_na_parametry->czy_uzywamy_maski5x5 == false)
            for (int i = 0; i < 9; i++) maska << static_cast<double>(okno_na_parametry->maska[i]);
        else if (okno_na_parametry->czy_uzywamy_maski5x5 == true)
            for (int i = 0; i < 25; i++) maska << static_cast<double>(okno_na_parametry->maska5x5[i]);

        implementMask(poprzednieWersje.last(), mMatZmodyfikowany, maska, okno_na_parametry->border);

        if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeFirst();
        poprzednieWersje.append(mMatZmodyfikowany.clone());
        wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
    }
}

void MainWindow::implementMask(cv::Mat& Input, cv::Mat& Output, QVector<double>maska, int BORDER_TYPE)
{
    CV_Assert(Input.depth() == CV_8U);

    int mask_size = static_cast<int>(sqrt(maska.size()));

    int padding = mask_size/2;
    cv::Mat temp;
    cv::copyMakeBorder(Input, temp, padding, padding, padding, padding, BORDER_TYPE);

    Output = temp.clone();

    int channels = Output.channels();
    int nRows = Output.rows;
    int nCols = Output.cols * channels;

    int i,j;
    QVector<uchar*>p;       //wskaznik do unsigned char, do poczatku kazdego wiersza obrazka
    QVector<uchar*>p_temp;  //wskaznik do unsigned char, do poczatku kazdego wiersza obrazka
    for( i = 0; i < nRows; ++i)
    {
        p.push_back(Output.ptr<uchar>(i));
        p_temp.push_back(temp.ptr<uchar>(i));
    }

    for( i = padding; i < nRows-padding-1; ++i) //po rzedach
    {
        for ( j = padding; j < nCols-padding; ++j) //po kolumnach
        {
            int suma = 0, k = 0;
            for (int m = 0; m < mask_size; m++)
            {
                for(int n = 0; n < mask_size; n++)
                {
                    double maska_wartosc = maska[k++];
                    int wartosc_piksela = p_temp[i-1+m][j-1+n];
                    int wartosc_piksela_po_pomnozeniu = maska_wartosc * wartosc_piksela;
                    suma = suma + wartosc_piksela_po_pomnozeniu;
                }
            }
            double modyfikator = suma/*/(mask_size*mask_size)*/;

            modyfikator<0?modyfikator=0:0;
            modyfikator>255?modyfikator=255:255;
            p[i][j] = static_cast<uchar>(modyfikator);
        }
    }
    p_temp.clear();
    p.clear();
    temp.release();

    //przywrocenie standardowego rozmiaru
    cv::Point lewa_gora, prawy_dol;
    lewa_gora = cv::Point(padding, padding);
    prawy_dol = cv::Point(Output.cols - padding, Output.rows - padding);

    cv::Rect nowy_rozmiar(lewa_gora, prawy_dol);
    Output = Output(nowy_rozmiar);
}

void MainWindow::on_actionFiltracja0stopni_triggered()
{
    cv::Mat Input = poprzednieWersje.last();
    CV_Assert(Input.depth() == CV_8U);

    int padding = 1;
    cv::Mat temp;
    cv::copyMakeBorder(Input, temp, padding, padding, padding, padding, 1);

    mMatZmodyfikowany = temp.clone();

    int channels = mMatZmodyfikowany.channels();
    int nRows = mMatZmodyfikowany.rows;
    int nCols = mMatZmodyfikowany.cols * channels;

    int i,j;
    QVector<uchar*>p; //wskaznik do unsigned char, do poczatku kazdego wiersza obrazka
    QVector<uchar*>p_temp; //wskaznik do unsigned char, do poczatku kazdego wiersza obrazka
    for( i = 0; i < nRows; ++i){
        p.push_back(mMatZmodyfikowany.ptr<uchar>(i));
        p_temp.push_back(temp.ptr<uchar>(i));
    }

    for( i = padding; i < nRows-padding-1; ++i) //po rzedach
    {
        for ( j = padding; j < nCols-padding; ++j) //po kolumnach
        {
            if (p_temp[i-1][j] == p_temp[i+1][j])
                p[i][j] = p_temp[i-1][j];
        }
    }
    p_temp.clear();
    p.clear();
    temp.release();

    //przywrocenie standardowego rozmiaru
    cv::Point lewa_gora, prawy_dol;
    lewa_gora = cv::Point(padding, padding);
    prawy_dol = cv::Point(mMatZmodyfikowany.cols - padding, mMatZmodyfikowany.rows - padding);

    cv::Rect nowy_rozmiar(lewa_gora, prawy_dol);
    mMatZmodyfikowany = mMatZmodyfikowany(nowy_rozmiar);

    if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeFirst();
    poprzednieWersje.append(mMatZmodyfikowany.clone());
    wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
}

void MainWindow::on_actionFiltracja90stopni_triggered()
{
    cv::Mat Input = poprzednieWersje.last();
    CV_Assert(Input.depth() == CV_8U);

    int padding = 1;
    cv::Mat temp;
    cv::copyMakeBorder(Input, temp, padding, padding, padding, padding, 1);

    cv::Mat Output = temp.clone();

    int channels = Output.channels();
    int nRows = Output.rows;
    int nCols = Output.cols * channels;

    int i,j;
    QVector<uchar*>p; //wskaznik do unsigned char, do poczatku kazdego wiersza obrazka
    QVector<uchar*>p_temp; //wskaznik do unsigned char, do poczatku kazdego wiersza obrazka
    for( i = 0; i < nRows; ++i){
        p.push_back(Output.ptr<uchar>(i));
        p_temp.push_back(temp.ptr<uchar>(i));
    }
    for( i = padding; i < nRows-padding-1; ++i) //po rzedach
    {
        for ( j = padding; j < nCols-padding; ++j) //po kolumnach
        {
            if (p_temp[i][j-1] == p_temp[i+1][j+1])
                p[i][j] = p_temp[i][j-1];
        }
    }
    p_temp.clear();
    p.clear();
    temp.release();

    //przywrocenie standardowego rozmiaru
    cv::Point lewa_gora, prawy_dol;
    lewa_gora = cv::Point(padding, padding);
    prawy_dol = cv::Point(Output.cols - padding, Output.rows - padding);

    cv::Rect nowy_rozmiar(lewa_gora, prawy_dol);
    mMatZmodyfikowany = Output(nowy_rozmiar);

    if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeFirst();
    poprzednieWersje.append(mMatZmodyfikowany.clone());
    wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
}

void MainWindow::on_actionRoberts_triggered()
{
    bool ok;
    int parametr;
    int opcja = QInputDialog::getInt(this, "Parametryzacja",
                                             "Ktora maska\n"
                                             "0 = 1 <<  0 << 0 << -1\n"
                                             "1 = 0 << -1 << 1 <<  0\n",
                                        QLineEdit::Normal,0,1,1,&ok);
    if (ok){
        bool ok2;
        parametr = QInputDialog::getInt(this,   "Parametryzacja",
                                                "Podaj opcje BORDER\n"
                                                "0 = BORDER_CONSTANT\n"
                                                "1 = BORDER_REPLICATE\n"
                                                "2 = BORDER_REFLECT\n"
                                                "3 = BORDER_REFLECT_101",
                                                QLineEdit::Normal,0,3,1,&ok2);
        if (ok2){

            QVector<double> maska;
            if (!opcja)
                maska << 1 << 0 << 0 << -1;
            else
                maska << 0 << -1 << 1 << 0;

            implementMask(poprzednieWersje.last(), mMatZmodyfikowany, maska, parametr);

            if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeFirst();
            poprzednieWersje.append(mMatZmodyfikowany.clone());
            wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
        }
    }
}

void MainWindow::on_actionDetekcjaSzkieletu_triggered()
{
    mCzyIstniejeZmodyfikowanyObraz = true;
    cv::Mat img = poprzednieWersje.last().clone();

    cv::threshold(img, img, 127, 255, cv::THRESH_BINARY);

    cv::Mat skel(img.size(), CV_8UC1, cv::Scalar(0));
    cv::Mat temp(img.size(), CV_8UC1);

    cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));

    bool done;
    do
    {
      cv::morphologyEx(img, temp, cv::MORPH_OPEN, element);
      cv::bitwise_not(temp, temp);
      cv::bitwise_and(img, temp, temp);
      cv::bitwise_or(skel, temp, skel);
      cv::erode(img, img, element);
      double max;
      cv::minMaxLoc(img, 0, &max);
      done = (max == 0);
    } while (!done);

    if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeFirst();
    poprzednieWersje.append(skel.clone());
    wyswietl(ui->labelOutputPicture, skel);
}

void MainWindow::on_actionErozja_triggered()
{
    int erosion_elem = QInputDialog::getInt(this, "Parametryzacja",
                                         "Wybierz:\n"
                                         "1 - kwadrat\n"
                                         "2 - romb", QLineEdit::Normal,
                                         1,2,1);
    int erosion_size = QInputDialog::getInt(this, "Parametryzacja",
                                         "Wybierz rozmiar erozji:\n", QLineEdit::Normal,
                                         3,7,2);
    int iteracje = QInputDialog::getInt(this, "Parametryzacja",
                                         "Ile iteracji:\n", QLineEdit::Normal,
                                         1,50,1);
    int erosion_type=1;
    if( erosion_elem == 1 ){ erosion_type = cv::MORPH_RECT; }
    else if( erosion_elem == 2 ){ erosion_type = cv::MORPH_CROSS; }

    cv::Mat element = getStructuringElement( erosion_type,
                                       cv::Size( erosion_size, erosion_size ));

    mCzyIstniejeZmodyfikowanyObraz = true;

    cv::erode( poprzednieWersje.last(), mMatZmodyfikowany, element, cv::Point(-1,-1), iteracje );

    if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeFirst();
    poprzednieWersje.append(mMatZmodyfikowany.clone());
    wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
}

void MainWindow::on_actionDylatacja_triggered()
{    
    int dilate_elem = QInputDialog::getInt(this, "Parametryzacja",
                                         "Wybierz:\n"
                                         "1 - kwadrat\n"
                                         "2 - romb", QLineEdit::Normal,
                                         1,2,1);
    int dilate_size = QInputDialog::getInt(this, "Parametryzacja",
                                         "Wybierz rozmiar erozji:\n", QLineEdit::Normal,
                                         3,7,2);
    int iteracje = QInputDialog::getInt(this, "Parametryzacja",
                                         "Ile iteracji:\n", QLineEdit::Normal,
                                         1,50,1);
    int erosion_type=1;
    if( dilate_elem == 1 ){ erosion_type = cv::MORPH_RECT; }
    else if( dilate_elem == 2 ){ erosion_type = cv::MORPH_CROSS; }

    cv::Mat element = getStructuringElement( erosion_type,
                                       cv::Size( dilate_size, dilate_size ));

    mCzyIstniejeZmodyfikowanyObraz = true;

    cv::dilate( poprzednieWersje.last(), mMatZmodyfikowany, element, cv::Point(-1,-1), iteracje );

    if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO)
        poprzednieWersje.removeFirst();
    poprzednieWersje.append(mMatZmodyfikowany.clone());

    wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
}

void MainWindow::on_actionOtwarcie_triggered()
{
    int erosion_elem = QInputDialog::getInt(this, "Parametryzacja",
                                         "Wybierz:\n"
                                         "1 - kwadrat\n"
                                         "2 - romb", QLineEdit::Normal,
                                         1,2,1);
    int size = QInputDialog::getInt(this, "Parametryzacja",
                                         "Wybierz rozmiar erozji:\n", QLineEdit::Normal,
                                         3,7,2);
    int erosion_type=1;
    if( erosion_elem == 1 ){ erosion_type = cv::MORPH_RECT; }
    else if( erosion_elem == 2 ){ erosion_type = cv::MORPH_CROSS; }

    cv::Mat element = getStructuringElement( erosion_type,
                                       cv::Size( size, size ));

    cv::dilate( poprzednieWersje.last(), mMatZmodyfikowany, element );
    cv::erode( mMatZmodyfikowany, mMatZmodyfikowany, element );
    mCzyIstniejeZmodyfikowanyObraz = true;
    if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO)
        poprzednieWersje.removeFirst();
    poprzednieWersje.append(mMatZmodyfikowany.clone());
    wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
}

void MainWindow::on_actionZamkniecie_triggered()
{
    int erosion_elem = QInputDialog::getInt(this, "Parametryzacja",
                                             "Wybierz:\n"
                                             "1 - kwadrat\n"
                                             "2 - romb", QLineEdit::Normal,
                                             1,2,1);
    int size = QInputDialog::getInt(this, "Parametryzacja",
                                             "Wybierz rozmiar erozji:\n", QLineEdit::Normal,
                                             3,7,2);
    int erosion_type=1;
    if( erosion_elem == 1 )
    {
        erosion_type = cv::MORPH_RECT;
    }
    else if( erosion_elem == 2 ){ erosion_type = cv::MORPH_CROSS; }

    cv::Mat element = getStructuringElement( erosion_type, cv::Size( size, size ));

    cv::erode( poprzednieWersje.last(), mMatZmodyfikowany, element );
    cv::dilate( mMatZmodyfikowany, mMatZmodyfikowany, element );
    mCzyIstniejeZmodyfikowanyObraz = true;
    if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO)
        poprzednieWersje.removeFirst();
    poprzednieWersje.append(mMatZmodyfikowany.clone());
    wyswietl(ui->labelOutputPicture, mMatZmodyfikowany);
}

void MainWindow::on_actionSegmentacja_Watershed_triggered()
{
    using namespace cv;
    Mat src = poprzednieWersje.last().clone();

    // Change the background from white to black, since that will help later to extract
    // better results during the use of Distance Transform
    cv::MatIterator_<uchar> it, end;
    for( it = mMatZmodyfikowany.begin<uchar>(), end = mMatZmodyfikowany.end<uchar>(); it != end; ++it)
    {
        if ( *it == 255) *it = 0;
    }

    threshold(src, src, 0, 255, THRESH_BINARY_INV + THRESH_OTSU);

    // Create a kernel that we will use for accuting/sharpening our image
    Mat kernel = (Mat_<float>(3,3) <<
            1,  1,  1,
            1, -8,  1,
            1,  1,  1); // an approximation of second derivative, a quite strong kernel

    // do the laplacian filtering as it is
    // well, we need to convert everything in something more deeper then CV_8U
    // because the kernel has some negative values,
    // and we can expect in general to have a Laplacian image with negative values
    // BUT a 8bits unsigned int (the one we are working with) can contain values from 0 to 255
    // so the possible negative number will be truncated
    Mat imgLaplacian;
    Mat sharp = src; // copy source image to another temporary one
    filter2D(sharp, imgLaplacian, CV_8UC1, kernel);
    src.convertTo(sharp, CV_8UC1);
    Mat imgResult = sharp - imgLaplacian;

    // convert back to 8bits gray scale
    imgResult.convertTo(imgResult, CV_8UC1);

    //------------------------
    src = imgResult; // copy back
    // Create binary image from source image
    Mat bw = src;
    //cvtColor(src, bw, COLOR_BGR2GRAY);


    threshold(bw, bw, 40, 255, THRESH_BINARY | THRESH_OTSU);

    // Perform the distance transform algorithm
    Mat dist;
    distanceTransform(bw, dist, DIST_L2, 3);
    // Normalize the distance image for range = {0.0, 1.0}
    // so we can visualize and threshold it
    normalize(dist, dist, 0, 1., NORM_MINMAX);


    // Threshold to obtain the peaks
    // This will be the markers for the foreground objects
    threshold(dist, dist, .5, 1., THRESH_BINARY);

    // Dilate a bit the dist image
    Mat kernel1 = Mat::ones(3, 3, CV_8UC1);
    dilate(dist, dist, kernel1);


    // Create the CV_8U version of the distance image
    // It is needed for findContours()
    Mat dist_8u;
    dist.convertTo(dist_8u, CV_8U);


    // Find total markers
    std::vector<std::vector<Point> > contours;
    findContours(dist_8u, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // Create the marker image for the watershed algorithm
    Mat markers = Mat::zeros(dist.size(), CV_32SC1);
    // Draw the foreground markers
    for (size_t i = 0; i < contours.size(); i++)
        drawContours(markers, contours, static_cast<int>(i), Scalar::all(static_cast<int>(i)+1), -1);

    // Draw the background marker
    //circle(markers, Point(5,5), 2/*3*/, CV_RGB(255,255,255), -1);

    cvtColor(src,src,COLOR_GRAY2BGR); //convert to 3 channels

    watershed(src, markers);


    Mat mark = Mat::zeros(markers.size(), CV_8UC3);

    markers.convertTo(mark, CV_8UC1);

    bitwise_not(mark, mark);
    //imshow("8 - Markers_v2", mark); // uncomment this if you want to see how the mark
                                  // image looks like at that point

    // Generate random colors
    std::vector<Vec3b> colors;
    for (size_t i = 0; i < contours.size(); i++)
    {
        int b = theRNG().uniform(0, 255);
        int g = theRNG().uniform(0, 255);
        int r = theRNG().uniform(0, 255);
        colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
    }
    // Create the result image
    Mat dst = Mat::zeros(markers.size(), CV_8UC3);
    // Fill labeled objects with random colors
    for (int i = 0; i < markers.rows; i++)
    {
        for (int j = 0; j < markers.cols; j++)
        {
            int index = markers.at<int>(i,j);
            if (index > 0 && index <= static_cast<int>(contours.size()))
                dst.at<Vec3b>(i,j) = colors[index-1];
            else
                dst.at<Vec3b>(i,j) = Vec3b(0,0,0);
        }
    }
    // Visualize the final image
    cvtColor(dst, dst, COLOR_BGR2RGB);
    int width = ui->labelOutputPicture->width();
    int height = ui->labelOutputPicture->height();
    if (width < mObrazekZmodyfikowany.width() || height < mObrazekZmodyfikowany.height())
        ui->labelOutputPicture->setPixmap(QPixmap::fromImage(QImage(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888).scaled(width, height, Qt::KeepAspectRatio)));
    else
        ui->labelOutputPicture->setPixmap(QPixmap::fromImage(QImage(dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888)));

    ui->labelOutputPicture->setAlignment(Qt::AlignCenter);

    ui->histogramCentralny->setHistogram(createHistogram(QImage(static_cast<uchar*>(dst.data),
                                                                dst.cols,
                                                                dst.rows,
                                                                static_cast<int>(dst.step),
                                                                QImage::Format_Grayscale8)));
    mCzyIstniejeZmodyfikowanyObraz = true;
    if (poprzednieWersje.size() > MAX_SIZE_OF_UNDO) poprzednieWersje.removeLast();
    poprzednieWersje.append(dst.clone());
    if (poprzednieWersje.size() > 1) ui->actionCofnijZmiane->setEnabled(true);

}


