#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "histogram.h"
#include "dialoguiparameters.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionOtworzPlik, &QAction::triggered, this, &MainWindow::otworzPlik);
    connect(ui->actionInvert, &QAction::triggered, this, &MainWindow::ustawInvertObrazka);

    connect(ui->actionDodawanieObrazow, SIGNAL(triggered()), this, SLOT(akcjaArytmetyczna()));
    connect(ui->actionOdejmowanieObrazow, SIGNAL(triggered()), this, SLOT(akcjaArytmetyczna()));
    connect(ui->actionOdejmowanieAbs, SIGNAL(triggered()), this, SLOT(akcjaArytmetyczna()));
    connect(ui->actionAND, SIGNAL(triggered()), this, SLOT(akcjaArytmetyczna()));
    connect(ui->actionOR, SIGNAL(triggered()), this, SLOT(akcjaArytmetyczna()));
    connect(ui->actionXOR, SIGNAL(triggered()), this, SLOT(akcjaArytmetyczna()));

    connect(ui->actionGaussian, SIGNAL(triggered()), this, SLOT(akcjaWygladzania()));
    connect(ui->actionBlur, SIGNAL(triggered()), this, SLOT(akcjaWygladzania()));
    connect(ui->actionMedianBlur, SIGNAL(triggered()), this, SLOT(akcjaWygladzania()));

    ui->statusBar->addPermanentWidget(ui->progressBar);
    ui->progressBar->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::otworzPlik()
{
    QString filtr_rozszerzen = "Pliki JPG (*.jpg)";
    QString nazwa_pliku = QFileDialog::getOpenFileName(this, "Otwórz plik", QDir::homePath(), filtr_rozszerzen);
    wczytajPlik(nazwa_pliku);
}

bool MainWindow::wczytajPlik(const QString &nazwa_pliku)
{
    QImageReader reader(nazwa_pliku);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(nazwa_pliku), reader.errorString()));
        return false;
    }

    mMatImage = cv::imread(nazwa_pliku.toStdString(), 0);
                                                //wczytujemy juz jako Grayscale '0'
    ustawObrazek(newImage);

    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
        .arg(QDir::toNativeSeparators(nazwa_pliku)).arg(mObrazek.width()).arg(mObrazek.height()).arg(mObrazek.depth());
    statusBar()->showMessage(message);
    return true;
}

void MainWindow::ustawObrazek(const QImage &newImage)
{
    mObrazek = newImage;
    mObrazekOryginal = mObrazek;
    wyswietl(ui->labelObrazek, mObrazek);
}

void MainWindow::ustawInvertObrazka()
{
    mCzyIstniejeZmodyfikowanyObraz = true;

    mObrazekZmodyfikowany = mObrazek;
    mObrazekZmodyfikowany.invertPixels();

    wyswietl(ui->labelOutputPicture, mObrazekZmodyfikowany);
}


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
        Histogram *hist_zmodyfik = new Histogram(mObrazekZmodyfikowany);
        hist_zmodyfik->setMinimumSize(900,300);
        hist_zmodyfik->setWindowTitle("Histogram obrazu zmodyfikowanego");
        hist_zmodyfik->show();
    }
    else
        QMessageBox::information(this,"UWAGA","Nie dokonano żadnej modyfikacji.\n Brak danych dla Histogramu.");
}

void MainWindow::on_actionKonwertujNaGrayScale_triggered()
{
    Q_ASSERT(mObrazek.format() == QImage::Format_RGB32);
    for (int i = 0; i < mObrazek.height(); i++)
    {
        QRgb *pixel = reinterpret_cast<QRgb*>(mObrazek.scanLine(i));
        QRgb *end = pixel + mObrazek.width();
        for (; pixel != end; pixel++)
        {
            int gray = qGray(*pixel);
            *pixel = QColor(gray, gray, gray).rgb();
        }
    }
    wyswietl(ui->labelObrazek, mObrazek);
}

void MainWindow::on_actionPowrotDoOryginalu_triggered()
{
    mObrazek = mObrazekOryginal;
    wyswietl(ui->labelObrazek, mObrazek);
}

void MainWindow::wyswietl(QLabel *gdzie, const QImage &obrazek) //f-cja wyswietla dany obrazek w danym panelu
{
    int width = gdzie->width();
    int height = gdzie->height();
    gdzie->setPixmap(QPixmap::fromImage(obrazek).scaled(width, height, Qt::KeepAspectRatio));
    gdzie->setAlignment(Qt::AlignCenter);
}

void MainWindow::on_actionProgowanieBinarne_triggered() //progowanie na 0 lub 255
{
    mCzyIstniejeZmodyfikowanyObraz = true;  //zmienna wprowadzona po to zeby program
                                            //wiedzial ze jest jakikolwiek obrazek
                                            //w drugim okienku
    mObrazekZmodyfikowany = mObrazek;       //pracujemy na Zmodyfikowanym Obrazku

    for (int i = 0; i < mObrazekZmodyfikowany.height(); i++)
    {
        QRgb *pixel = reinterpret_cast<QRgb*>(mObrazekZmodyfikowany.scanLine(i));
        QRgb *end = pixel + mObrazekZmodyfikowany.width();
        for (; pixel != end; pixel++)
        {
            int gray = qGray(*pixel);
            if (gray < 128)
                *pixel = QColor(0, 0, 0).rgb();
            else
                *pixel = QColor(255, 255, 255).rgb();
        }
    }
    wyswietl(ui->labelOutputPicture, mObrazekZmodyfikowany);
}

void MainWindow::on_actionProgowanieNprogow_triggered()
{
    bool ok;
    int ilosc_progow = QInputDialog::getInt(this, "Parametryzacja",
                                             "Podaj ilość progów:", QLineEdit::Normal,
                                             1,255,1,&ok);
//    if (ok && ilosc_progow)
//    {
//        QMessageBox::information (this,
//                        "Progowanie",
//                        QString("Wartość progowania to %1").arg(QString::number(ilosc_progow)));
//    }

    mCzyIstniejeZmodyfikowanyObraz = true;  //zmienna wprowadzona po to zeby program
                                            //wiedzial ze jest jakikolwiek obrazek
                                            //w drugim okienku
    mObrazekZmodyfikowany = mObrazek;       //pracujemy na Zmodyfikowanym Obrazku

    double prog = 255 - (255/((ilosc_progow+1)*2));

    //czesc zasadnicza algorytmu
        ilosc_przebiegow = 0;
        while (prog > 0)
        {
            ++ilosc_przebiegow;
            ui->progressBar->show();
            ui->progressBar->setMaximum(ilosc_progow+1);
            ui->progressBar->setValue(ilosc_przebiegow);
            for (int i = 0; i < mObrazekZmodyfikowany.height(); i++)
            {
                QRgb *pixel = reinterpret_cast<QRgb*>(mObrazek.scanLine(i));
                //QRgb *end = pixel + mObrazek.width();

                QRgb *pixelZmodyf = reinterpret_cast<QRgb*>(mObrazekZmodyfikowany.scanLine(i));
                QRgb *endZmodyf = pixelZmodyf + mObrazekZmodyfikowany.width();

                for (; pixelZmodyf != endZmodyf; pixelZmodyf++, pixel++)
                {
                    int gray = qGray(*pixel);
                    if (gray < prog)
                    {
                        int x = static_cast<int>(prog-(255/((ilosc_progow+1)*2)));
                        *pixelZmodyf =
                                QColor(x, x, x).rgb();
                    }
                    else if (ilosc_przebiegow == 1 && gray >= prog) //to sie uruchamia tylko
                                                                    //w pierwszym przebiegu zeby wypelnic
                                                                    //piksele od granicy ost progu do 255
                    {
                        *pixelZmodyf =
                                QColor(255, 255, 255).rgb();
                    }
                }
            }

            prog = prog - 255/(ilosc_progow+1);
        }
    wyswietl(ui->labelOutputPicture, mObrazekZmodyfikowany);
}

void MainWindow::on_actionWyrownanie_triggered()
{
    mCzyIstniejeZmodyfikowanyObraz = true;  //zmienna wprowadzona po to zeby program
                                            //wiedzial ze jest jakikolwiek obrazek
                                            //w drugim okienku
    mObrazekZmodyfikowany = mObrazek;       //pracujemy na Zmodyfikowanym Obrazku

    QVector<unsigned int> tablica(256, 0);

    int ilosc_pikseli = mObrazek.width() * mObrazek.height();

    qDebug() << "ilosc pikseli: " << ilosc_pikseli << endl;

    for (int x = 0; x < mObrazek.width(); x++)
        for (int y = 0; y < mObrazek.height(); y++)
        {
            int a = qGray(mObrazek.pixel(x,y));
            tablica[a]++;
        }

    QVector<unsigned long> dystrybuanta(256, 0);
    dystrybuanta[0] = tablica[0];
    int min_dystrybuanty{0}/*, max_dystrybuanty{0}*/;
    for(int i = 1; i < 256; i++)
    {
        dystrybuanta[i] = dystrybuanta[i-1] + tablica[i];
        min_dystrybuanty = dystrybuanta[i]<dystrybuanta[i-1]?dystrybuanta[i]:min_dystrybuanty;
    }

    qDebug() << "ilosc pikseli: " << ilosc_pikseli << endl;
    qDebug() << "min_dystrybuanty: " << min_dystrybuanty << endl;

    QVector<int> dystrybuanta_zmodyfikowana(256, 0);
    for(int i = 1; i < 256; i++)
    {
        dystrybuanta_zmodyfikowana[i] = static_cast<double>((dystrybuanta[i] - min_dystrybuanty))/static_cast<double>(ilosc_pikseli - min_dystrybuanty) * 255;
        qDebug() << i << ": " << tablica[i] << "  " << dystrybuanta[i] << "  " << dystrybuanta_zmodyfikowana[i] <<  endl;
    }

    for (int i = 0; i < mObrazekZmodyfikowany.height(); i++)
    {
        QRgb *pixel = reinterpret_cast<QRgb*>(mObrazekZmodyfikowany.scanLine(i));
        QRgb *end = pixel +  mObrazekZmodyfikowany.width();
        for (; pixel != end; pixel++)
        {
            int gray = qGray(*pixel);
            int mod_kol = dystrybuanta_zmodyfikowana[gray];
            *pixel = QColor(mod_kol, mod_kol, mod_kol).rgb();
        }
    }

    wyswietl(ui->labelOutputPicture, mObrazekZmodyfikowany);

}

void MainWindow::on_actionRozciagniecie_triggered()
{
    mCzyIstniejeZmodyfikowanyObraz = true;  //zmienna wprowadzona po to zeby program
                                            //wiedzial ze jest jakikolwiek obrazek
                                            //w drugim okienku
    mObrazekZmodyfikowany = mObrazek;       //pracujemy na Zmodyfikowanym Obrazku

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

    for (int i = 0; i < mObrazekZmodyfikowany.height(); i++)
    {
        QRgb *pixel = reinterpret_cast<QRgb*>(mObrazekZmodyfikowany.scanLine(i));
        QRgb *end = pixel + mObrazekZmodyfikowany.width();
        for (; pixel != end; pixel++)
        {
            int gray = qGray(*pixel);
            int mod_kol = (256/static_cast<double>(maxGrayValue-minGrayValue))*(gray-minGrayValue);
            mod_kol = mod_kol<0?0:mod_kol;
            mod_kol = mod_kol>255?255:mod_kol;
            *pixel = QColor(mod_kol, mod_kol, mod_kol).rgb();
        }
    }

    wyswietl(ui->labelOutputPicture, mObrazekZmodyfikowany);
}

void MainWindow::on_actionLaplacian_triggered()
{
    bool ok;
    int parametr = QInputDialog::getInt(this, "Parametryzacja",
                                             "Podaj ilość progów:", QLineEdit::Normal,
                                             1,255,2,&ok);
    while (parametr%2 == 0)
    {
        QMessageBox::warning(this, "UWAGA", "Podaj liczbę NIEPARZYSTĄ!!!");
        parametr = QInputDialog::getInt(this, "Parametryzacja",
                                                     "Podaj ilość progów:", QLineEdit::Normal,
                                                     1,255,2,&ok);
    }
    mCzyIstniejeZmodyfikowanyObraz = true;  //zmienna wprowadzona po to zeby program
                                            //wiedzial ze jest jakikolwiek obrazek
                                            //w drugim okienku

    cv::Mat output = mMatImage;

    //kombinowanie z laplacjanem i inna macierza
    //float m[9] = {1,-2,1,-2,4,-2,1,-2,1};
    //cv::Mat kernel(cv::Size(3,3), CV_8S, m);
    //filter2D(mMatImage, output, CV_8U, kernel);

    cv::Laplacian(mMatImage, output, CV_8U, parametr);
        //cv::cvtColor(output,output,CV_BGR2RGB);
    mObrazekZmodyfikowany = QImage(static_cast<uchar*>(output.data), output.cols, output.rows, static_cast<int>(output.step), QImage::Format_Grayscale8);
        //cv::namedWindow("My Image");
        //cv::imshow("My Image", output);

    wyswietl(ui->labelOutputPicture, mObrazekZmodyfikowany);
}

void MainWindow::on_actionExportToCSV_triggered()
{
    //w tej chwili podany jest oryginalnie wczytany mMatimage - ale moze byc to jakikolwiek Mat
    saveMatToCsv(MainWindow::matImage(), "output.csv");
}

void MainWindow::saveMatToCsv(const cv::Mat &matrix, std::string filename){
    std::ofstream outputFile(filename);
    outputFile << cv::format(matrix, 2) << std::endl;
    outputFile.close();
}

void MainWindow::on_actionWczytajCSV_triggered()
{
    cv::Mat img;
    cv::Ptr<cv::ml::TrainData> raw_data;
    cv::String sciezka = "D:/DEVEL/build-AtApp_2-Desktop_Qt_5_7_1_MinGW_32bit-Debug/output_2.csv";
    raw_data = cv::ml::TrainData::loadFromCSV(sciezka, 0);
        cv::Mat data = raw_data->getSamples();
        // optional if you have a color image and not just raw data
        data.convertTo(img, CV_8U);
        //img = img.reshape(3); //set number of channels

        // set the image type
        img.convertTo(img, CV_8U);

        // set the image size
        //cv::resize(img, img, cv::Size(320, 256));

        //std::cout << "img: " << img << std::endl;

        cv::namedWindow("img");
        cv::imshow("img", img);
        //cv::imshow("mat", mat);
        cv::waitKey(0);
}

void MainWindow::on_actionProgowanieBinarneOdwrocone_triggered()
{
    mCzyIstniejeZmodyfikowanyObraz = true;  //zmienna wprowadzona po to zeby program
                                            //wiedzial ze jest jakikolwiek obrazek
                                            //w drugim okienku
    mObrazekZmodyfikowany = mObrazek;       //pracujemy na Zmodyfikowanym Obrazku

    for (int i = 0; i < mObrazekZmodyfikowany.height(); i++)
    {
        QRgb *pixel = reinterpret_cast<QRgb*>(mObrazekZmodyfikowany.scanLine(i));
        QRgb *end = pixel + mObrazekZmodyfikowany.width();
        for (; pixel != end; pixel++)
        {
            int gray = qGray(*pixel);
            if (gray > 128)
                *pixel = QColor(0, 0, 0).rgb();
            else
                *pixel = QColor(255, 255, 255).rgb();
        }
    }
    wyswietl(ui->labelOutputPicture, mObrazekZmodyfikowany);
}

void MainWindow::on_actionBinaryzacjaZParametrami_triggered()
{
    DialogUiParameters *okno_na_parametry = new DialogUiParameters();
    okno_na_parametry->show();

    short int dol = 0, gora = 255;
    bool dol_bool = 0, gora_bool = 0;
    if (okno_na_parametry->exec() == QDialog::Accepted)
    {
        dol = okno_na_parametry->dolny_prog;
        gora = okno_na_parametry->gorny_prog;
        dol_bool = okno_na_parametry->check_dolny_prog;
        gora_bool = okno_na_parametry->check_gorny_prog;
    }

    mCzyIstniejeZmodyfikowanyObraz = true;  //zmienna wprowadzona po to zeby program
                                            //wiedzial ze jest jakikolwiek obrazek
                                            //w drugim okienku
    mObrazekZmodyfikowany = mObrazek;       //pracujemy na Zmodyfikowanym Obrazku

    //wybrany tylko dolny prog
    if (dol_bool && !gora_bool)
    {
        for (int i = 0; i < mObrazekZmodyfikowany.height(); i++)
        {
            QRgb *pixel = reinterpret_cast<QRgb*>(mObrazekZmodyfikowany.scanLine(i));
            QRgb *end = pixel + mObrazekZmodyfikowany.width();
            for (; pixel != end; pixel++)
            {
                int gray = qGray(*pixel);
                if (gray < dol)
                    *pixel = QColor(0, 0, 0).rgb();
                else
                    *pixel = QColor(255, 255, 255).rgb();
            }
        }
    }
    else if (!dol_bool && gora_bool)
    {
        for (int i = 0; i < mObrazekZmodyfikowany.height(); i++)
        {
            QRgb *pixel = reinterpret_cast<QRgb*>(mObrazekZmodyfikowany.scanLine(i));
            QRgb *end = pixel + mObrazekZmodyfikowany.width();
            for (; pixel != end; pixel++)
            {
                int gray = qGray(*pixel);
                if (gray > gora)
                    *pixel = QColor(0, 0, 0).rgb();
                else
                    *pixel = QColor(255, 255, 255).rgb();
            }
        }
    }
    else if (dol_bool && gora_bool && gora>dol)
    {
        for (int i = 0; i < mObrazekZmodyfikowany.height(); i++)
        {
            QRgb *pixel = reinterpret_cast<QRgb*>(mObrazekZmodyfikowany.scanLine(i));
            QRgb *end = pixel + mObrazekZmodyfikowany.width();
            for (; pixel != end; pixel++)
            {
                int gray = qGray(*pixel);
                if (gray > dol && gray < gora)
                    *pixel = QColor(255, 255, 255).rgb();
                else
                    *pixel = QColor(0, 0, 0).rgb();
            }
        }
    }
    else {

    }
    wyswietl(ui->labelOutputPicture, mObrazekZmodyfikowany);
}

void MainWindow::on_actionProgowanieZachowaniePoziomySzarosci_triggered()
{
    DialogUiParameters *okno_na_parametry = new DialogUiParameters(0, 0, 1, this);
    okno_na_parametry->show();

    short int dol = 0, gora = 255;
    if (okno_na_parametry->exec() == QDialog::Accepted)
    {
        dol = okno_na_parametry->dolny_prog;
        gora = okno_na_parametry->gorny_prog;
    }

    mCzyIstniejeZmodyfikowanyObraz = true;  //zmienna wprowadzona po to zeby program
                                            //wiedzial ze jest jakikolwiek obrazek
                                            //w drugim okienku
    mObrazekZmodyfikowany = mObrazek;       //pracujemy na Zmodyfikowanym Obrazku

    for (int i = 0; i < mObrazekZmodyfikowany.height(); i++)
    {
        QRgb *pixel = reinterpret_cast<QRgb*>(mObrazekZmodyfikowany.scanLine(i));
        QRgb *end = pixel + mObrazekZmodyfikowany.width();
        for (; pixel != end; pixel++)
        {
            int gray = qGray(*pixel);
            if (gray < dol)
                *pixel = QColor(0, 0, 0).rgb();
            else if (gray > gora)
                *pixel = QColor(255, 255, 255).rgb();
            else
                continue;
        }
    }
    wyswietl(ui->labelOutputPicture, mObrazekZmodyfikowany);
}

void MainWindow::on_actionRozciaganieZakresWgParametrow_triggered()
{
    DialogUiParameters *okno_na_parametry = new DialogUiParameters(0, 0, 1, this);
    okno_na_parametry->show();

    short int dol = 0, gora = 255;
    if (okno_na_parametry->exec() == QDialog::Accepted)
    {
        dol = okno_na_parametry->dolny_prog;
        gora = okno_na_parametry->gorny_prog;
    }

    mCzyIstniejeZmodyfikowanyObraz = true;  //zmienna wprowadzona po to zeby program
                                            //wiedzial ze jest jakikolwiek obrazek
                                            //w drugim okienku
    mObrazekZmodyfikowany = mObrazek;       //pracujemy na Zmodyfikowanym Obrazku

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

    wyswietl(ui->labelOutputPicture, mObrazekZmodyfikowany);
}

void MainWindow::initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

bool MainWindow::saveFile(const QString &fileName)
{
    QImageWriter writer(fileName);

    if (!writer.write(mObrazekZmodyfikowany)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot write %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName)), writer.errorString());
        return false;
    }
    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
    statusBar()->showMessage(message);
    return true;
}

void MainWindow::on_actionZapiszZmodyfikowanyObrazJako_triggered()
{
    QFileDialog dialog(this, tr("Zapisz jako..."));
    initializeImageFileDialog(dialog, QFileDialog::AcceptSave);

    while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first())) {}
}

void MainWindow::on_actionRozjasnianie_triggered()
{
    DialogUiParameters *okno_na_parametry = new DialogUiParameters(0, 0, 0, this);
    okno_na_parametry->show();

    short int dol = 0;
    if (okno_na_parametry->exec() == QDialog::Accepted)
    {
        dol = okno_na_parametry->dolny_prog;
    }

    mCzyIstniejeZmodyfikowanyObraz = true;
    mObrazekZmodyfikowany = mObrazek;

    for (int i = 0; i < mObrazekZmodyfikowany.height(); i++)
    {
        QRgb *pixel = reinterpret_cast<QRgb*>(mObrazekZmodyfikowany.scanLine(i));
        QRgb *end = pixel + mObrazekZmodyfikowany.width();
        for (; pixel != end; pixel++)
        {
            int gray = qGray(*pixel);
            {
                gray = gray + dol;
                gray = gray<0?0:gray;
                gray = gray>255?255:gray;
                *pixel = QColor(gray, gray, gray).rgb();
            }
        }
    }
    wyswietl(ui->labelOutputPicture, mObrazekZmodyfikowany);
}

void::MainWindow::akcjaArytmetyczna()
{
    //otwieramy OPEN DIALOG dla obrazu ktory chcemy dodac do naszego obrazu
    //nalezaloby wpierw sprawdzic cy istnieje oryginalny obraz
    if (!mObrazek.isNull())
    {
        QString filtr_rozszerzen = "Pliki JPG (*.jpg)";
        QString nazwa_pliku = QFileDialog::getOpenFileName(this, "Otwórz plik do dodania", QDir::homePath(), filtr_rozszerzen);
        QImageReader reader(nazwa_pliku);
        reader.setAutoTransform(true);
        const QImage newImage = reader.read();
        if (newImage.isNull()) {
            QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                     tr("Cannot load %1: %2")
                                     .arg(QDir::toNativeSeparators(nazwa_pliku), reader.errorString()));
        }
        else if (newImage.size() != mObrazek.size())
        {
            QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                     tr("Obrazy rożnych rozmiarów."));
        }
        else
        {
            wyswietl(ui->labelOutputPicture, newImage);
            QMessageBox::StandardButton odpowiedz;
            QObject* button = QObject::sender();
            cv::Mat output;
            mMatImageDwa = cv::imread(nazwa_pliku.toStdString(), 0);
            if (button == ui->actionDodawanieObrazow)
            {
                odpowiedz = QMessageBox::question(this, "Pytanie", "Dodać obrazy?");
                if (odpowiedz == QMessageBox::Yes)
                {
                    output = mMatImage + mMatImageDwa;
                }
            }
            else if (button == ui->actionOdejmowanieObrazow)
            {
                odpowiedz = QMessageBox::question(this, "Pytanie", "Odjąć obrazy?");
                if (odpowiedz == QMessageBox::Yes)
                {
                    cv::subtract(mMatImage, mMatImageDwa, output);
//                    double min, max;
//                    cv::minMaxLoc(output, &min, &max);
//                    qDebug() << min << " " << max << endl;
                }
            }
            else if (button == ui->actionOdejmowanieAbs)
            {
                odpowiedz = QMessageBox::question(this, "Pytanie", "Odjąć obrazy (z modułem)?");
                if (odpowiedz == QMessageBox::Yes)
                {
                    cv::absdiff(mMatImage, mMatImageDwa, output);
//                    double min, max;
//                    cv::minMaxLoc(output, &min, &max);
//                    qDebug() << min << " " << max << endl;
                }
            }
            else if (button == ui->actionAND)
            {
                odpowiedz = QMessageBox::question(this, "Pytanie", "Zastosować AND dla obrazów?");
                if (odpowiedz == QMessageBox::Yes)
                {
                    cv::bitwise_and(mMatImage, mMatImageDwa, output);
                }
            }
            else if (button == ui->actionOR)
            {
                odpowiedz = QMessageBox::question(this, "Pytanie", "Zastosować OR dla obrazów?");
                if (odpowiedz == QMessageBox::Yes)
                {
                    cv::bitwise_or(mMatImage, mMatImageDwa, output);
                }
            }
            else if (button == ui->actionXOR)
            {
                odpowiedz = QMessageBox::question(this, "Pytanie", "Zastosować XOR dla obrazów?");
                if (odpowiedz == QMessageBox::Yes)
                {
                    cv::bitwise_xor(mMatImage, mMatImageDwa, output);
                }
            }
        mObrazekDwa = QImage(static_cast<uchar*>(output.data), output.cols, output.rows, static_cast<int>(output.step), QImage::Format_Grayscale8);
        wyswietl(ui->labelOutputPicture, mObrazekDwa);
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
        mObrazekZmodyfikowany = mObrazek;
        cv::Mat output;
        cv::blur(mMatImage, output, cv::Size(parametr, parametr)); //to 3 moznaby zdialogowac
        mObrazekZmodyfikowany = QImage(static_cast<uchar*>(output.data), output.cols, output.rows, static_cast<int>(output.step), QImage::Format_Grayscale8);
        wyswietl(ui->labelOutputPicture, mObrazekZmodyfikowany);
    }
    else if (button == ui->actionGaussian)
    {
        bool ok;
        int parametr = QInputDialog::getInt(this, "Parametryzacja",
                                                 "Podaj siłę blura:", QLineEdit::Normal,
                                                 1,255,2,&ok);
        if (parametr%2 == 0) parametr++;

        mCzyIstniejeZmodyfikowanyObraz = true;
        mObrazekZmodyfikowany = mObrazek;
        cv::Mat output;
        cv::GaussianBlur(mMatImage, output, cv::Size(parametr, parametr), 0, 0); //to 3 moznaby zdialogowac
        mObrazekZmodyfikowany = QImage(static_cast<uchar*>(output.data), output.cols, output.rows, static_cast<int>(output.step), QImage::Format_Grayscale8);
        wyswietl(ui->labelOutputPicture, mObrazekZmodyfikowany);
    }
    else if (button == ui->actionMedianBlur)
    {
        bool ok;
        int parametr = QInputDialog::getInt(this, "Parametryzacja",
                                                 "Podaj siłę blura:", QLineEdit::Normal,
                                                 1,255,2,&ok);
        if (parametr%2 == 0) parametr++;

        mCzyIstniejeZmodyfikowanyObraz = true;
        mObrazekZmodyfikowany = mObrazek;
        cv::Mat output;
        cv::medianBlur(mMatImage, output, parametr); //to 3 moznaby zdialogowac
        mObrazekZmodyfikowany = QImage(static_cast<uchar*>(output.data), output.cols, output.rows, static_cast<int>(output.step), QImage::Format_Grayscale8);
        wyswietl(ui->labelOutputPicture, mObrazekZmodyfikowany);
    }
}
