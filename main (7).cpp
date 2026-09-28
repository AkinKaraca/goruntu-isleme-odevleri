// Goruntu isleme odevleri - tek dosyada toplanmis hali
// Her gorev ayri bir fonksiyon, main icindeki menuden seciliyor.

#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <chrono>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// ------------------------------------------------------------
// GOREV 1: her pikseli 4'e bol ve 4 ile carp (tek thread)
// ------------------------------------------------------------
void gorev1(const Mat &img)
{
    int yukseklik = img.rows;
    int genislik = img.cols;

    Mat bolu4 = Mat::zeros(yukseklik, genislik, CV_8UC1);
    Mat carpi4 = Mat::zeros(yukseklik, genislik, CV_8UC1);

    // tum piksellere tek tek bak
    for (int i = 0; i < yukseklik; i++)
    {
        for (int j = 0; j < genislik; j++)
        {
            int parlaklik = img.at<uchar>(i, j);

            // 4'e bol
            bolu4.at<uchar>(i, j) = parlaklik / 4;

            // 4 ile carp (255'i gecerse 255 yap)
            int yeni = parlaklik * 4;
            if (yeni > 255)
            {
                yeni = 255;
            }
            carpi4.at<uchar>(i, j) = yeni;

            // ilk birkac pikselin degerlerini yazdir
            if (i == 0 && j < 5)
            {
                cout << "Piksel (" << i << ", " << j << ") -> " << parlaklik
                     << " | /4: " << (int)bolu4.at<uchar>(i, j)
                     << " | *4: " << (int)carpi4.at<uchar>(i, j) << endl;
            }
        }
    }

    imshow("Orijinal", img);
    imshow("4'e bolunmus", bolu4);
    imshow("4 ile carpilmis", carpi4);
    waitKey(0);
    destroyAllWindows();

    imwrite("gorev1_bolu4.jpg", bolu4);
    imwrite("gorev1_carpi4.jpg", carpi4);
}

// ------------------------------------------------------------
// GOREV 2: resmi 4 parcaya bol, her parca ayri thread'de
//          4'e bolunsun ve 4 ile carpilsin
// ------------------------------------------------------------

// her thread resmin kendine ait satir araligini isliyor
void parcaIsle(const Mat &img, Mat &bolu4, Mat &carpi4, int baslangic, int bitis, int no)
{
    for (int i = baslangic; i < bitis; i++)
    {
        for (int j = 0; j < img.cols; j++)
        {
            int parlaklik = img.at<uchar>(i, j);

            bolu4.at<uchar>(i, j) = parlaklik / 4;

            int yeni = parlaklik * 4;
            if (yeni > 255)
            {
                yeni = 255;
            }
            carpi4.at<uchar>(i, j) = yeni;
        }
    }
    cout << "Parca " << no << " bitti (satir " << baslangic << " - " << bitis - 1 << ")" << endl;
}

void gorev2(const Mat &img)
{
    int yukseklik = img.rows;
    int genislik = img.cols;

    Mat bolu4 = Mat::zeros(yukseklik, genislik, CV_8UC1);
    Mat carpi4 = Mat::zeros(yukseklik, genislik, CV_8UC1);

    // resmi 4 parcaya bol (yatay seritler)
    const int PARCA = 4;
    int parcaYuksekligi = yukseklik / PARCA;

    thread threadler[PARCA];

    for (int k = 0; k < PARCA; k++)
    {
        int baslangic = k * parcaYuksekligi;
        int bitis = baslangic + parcaYuksekligi;

        // yukseklik 4'e tam bolunmezse artan satirlar son parcaya gitsin
        if (k == PARCA - 1)
        {
            bitis = yukseklik;
        }

        threadler[k] = thread(parcaIsle, cref(img), ref(bolu4), ref(carpi4), baslangic, bitis, k + 1);
    }

    // hepsinin bitmesini bekle
    for (int k = 0; k < PARCA; k++)
    {
        threadler[k].join();
    }

    imshow("Orijinal", img);
    imshow("4'e bolunmus", bolu4);
    imshow("4 ile carpilmis", carpi4);
    waitKey(0);
    destroyAllWindows();

    imwrite("gorev2_bolu4.jpg", bolu4);
    imwrite("gorev2_carpi4.jpg", carpi4);
}

// ------------------------------------------------------------
// GOREV 3: parcalarin histogramini ayri thread'lerde cikar,
//          sonra toplayip ana resmin histogramini olustur
// ------------------------------------------------------------

// hist[deger] = o parlaklik degerine sahip piksel sayisi
void histogramCikar(const Mat &img, vector<int> &hist, int baslangic, int bitis)
{
    for (int i = baslangic; i < bitis; i++)
    {
        for (int j = 0; j < img.cols; j++)
        {
            int parlaklik = img.at<uchar>(i, j);
            hist[parlaklik]++;
        }
    }
}

void gorev3(const Mat &img)
{
    int yukseklik = img.rows;
    int genislik = img.cols;
    cout << "Toplam piksel: " << genislik * yukseklik << endl << endl;

    const int PARCA = 4;
    int parcaYuksekligi = yukseklik / PARCA;

    // her parca icin ayri histogram (256 parlaklik degeri, hepsi 0'la basliyor)
    vector<vector<int>> parcaHist(PARCA, vector<int>(256, 0));

    thread threadler[PARCA];

    for (int k = 0; k < PARCA; k++)
    {
        int baslangic = k * parcaYuksekligi;
        int bitis = baslangic + parcaYuksekligi;

        if (k == PARCA - 1)
        {
            bitis = yukseklik;
        }

        threadler[k] = thread(histogramCikar, cref(img), ref(parcaHist[k]), baslangic, bitis);
    }

    for (int k = 0; k < PARCA; k++)
    {
        threadler[k].join();
    }

    // parca histogramlarini toplayip ana histogrami olustur
    vector<int> anaHist(256, 0);
    for (int k = 0; k < PARCA; k++)
    {
        for (int deger = 0; deger < 256; deger++)
        {
            anaHist[deger] += parcaHist[k][deger];
        }
    }

    // kontrol icin resmin tamaminin histogramini tek seferde cikar
    vector<int> kontrolHist(256, 0);
    for (int i = 0; i < yukseklik; i++)
    {
        for (int j = 0; j < genislik; j++)
        {
            kontrolHist[img.at<uchar>(i, j)]++;
        }
    }

    // her parcanin histogram toplamini yazdir
    for (int k = 0; k < PARCA; k++)
    {
        long long parcaToplam = 0;
        for (int deger = 0; deger < 256; deger++)
        {
            parcaToplam += parcaHist[k][deger];
        }
        cout << "Parca " << k + 1 << " histogram toplami: " << parcaToplam << endl;
    }

    // ana histogram toplami
    long long anaToplam = 0;
    for (int deger = 0; deger < 256; deger++)
    {
        anaToplam += anaHist[deger];
    }
    cout << "Birlestirilmis histogram toplami: " << anaToplam << endl;

    if (anaToplam == (long long)genislik * yukseklik)
    {
        cout << "Toplam piksel sayisina esit, dogru." << endl;
    }
    else
    {
        cout << "HATA: toplam piksel sayisina esit degil!" << endl;
    }

    // tek seferde cikan histogramla ayni mi?
    bool ayniMi = true;
    for (int deger = 0; deger < 256; deger++)
    {
        if (anaHist[deger] != kontrolHist[deger])
        {
            ayniMi = false;
            break;
        }
    }
    if (ayniMi)
    {
        cout << "Tek seferde cikan histogramla ayni, dogru." << endl;
    }
    else
    {
        cout << "HATA: histogramlar farkli!" << endl;
    }

    // sadece piksel sayisi 0'dan buyuk olan degerleri yazdir
    cout << endl << "Deger -> Piksel sayisi" << endl;
    for (int deger = 0; deger < 256; deger++)
    {
        if (anaHist[deger] > 0)
        {
            cout << deger << " -> " << anaHist[deger] << endl;
        }
    }

    // histogrami dosyaya kaydet
    ofstream dosya("gorev3_histogram.txt");
    dosya << "Deger Piksel_Sayisi" << endl;
    for (int deger = 0; deger < 256; deger++)
    {
        dosya << deger << " " << anaHist[deger] << endl;
    }
    dosya.close();
    cout << endl << "Histogram gorev3_histogram.txt dosyasina kaydedildi." << endl;

    // histogrami ciz (siyah zemin, beyaz cubuklar)
    int cizimYuksekligi = 300;
    Mat cizim = Mat::zeros(cizimYuksekligi, 256 * 2, CV_8UC1);

    int enBuyuk = 0;
    for (int deger = 0; deger < 256; deger++)
    {
        if (anaHist[deger] > enBuyuk)
        {
            enBuyuk = anaHist[deger];
        }
    }

    for (int deger = 0; deger < 256; deger++)
    {
        int cubukBoyu = (int)((double)anaHist[deger] / enBuyuk * (cizimYuksekligi - 1));
        rectangle(cizim, Point(deger * 2, cizimYuksekligi - 1),
                  Point(deger * 2 + 1, cizimYuksekligi - 1 - cubukBoyu), Scalar(255), FILLED);
    }

    imshow("Orijinal", img);
    imshow("Histogram", cizim);
    waitKey(0);
    destroyAllWindows();

    imwrite("gorev3_histogram.png", cizim);
}

// ------------------------------------------------------------
// GOREV 4: her pikseli 0.75 ile carp, 20 ekle
// ------------------------------------------------------------
void gorev4(const Mat &img)
{
    int yukseklik = img.rows;
    int genislik = img.cols;

    Mat sonuc = Mat::zeros(yukseklik, genislik, CV_8UC1);

    for (int i = 0; i < yukseklik; i++)
    {
        for (int j = 0; j < genislik; j++)
        {
            int parlaklik = img.at<uchar>(i, j);

            // 0.75 ile carp, 20 ekle (+0.5 yuvarlamak icin)
            int yeni = (int)(parlaklik * 0.75 + 20 + 0.5);

            // 0-255 araligi disina ciktiysa sinirla
            if (yeni > 255)
            {
                yeni = 255;
            }
            if (yeni < 0)
            {
                yeni = 0;
            }

            sonuc.at<uchar>(i, j) = yeni;

            if (i == 0 && j < 5)
            {
                cout << "Piksel (" << i << ", " << j << ") -> " << parlaklik
                     << " | yeni: " << (int)sonuc.at<uchar>(i, j) << endl;
            }
        }
    }

    imshow("Orijinal", img);
    imshow("0.75 ile carpilip 20 eklenmis", sonuc);
    waitKey(0);
    destroyAllWindows();

    imwrite("gorev4_sonuc.jpg", sonuc);
}

// ------------------------------------------------------------
// GOREV 5: gorev 4'teki islemin hizini olc
// ------------------------------------------------------------
void gorev5(const Mat &img)
{
    int yukseklik = img.rows;
    int genislik = img.cols;

    Mat sonuc = Mat::zeros(yukseklik, genislik, CV_8UC1);

    // tek olcum yeterli olmaz, ayni islemi birkac kez tekrarlayip ortalama alacagiz
    const int TEKRAR = 10;
    double toplamSure = 0;

    for (int t = 0; t < TEKRAR; t++)
    {
        // sadece islem suresini olcuyoruz (resim okuma ve gosterme dahil degil)
        auto baslangic = chrono::high_resolution_clock::now();

        for (int i = 0; i < yukseklik; i++)
        {
            for (int j = 0; j < genislik; j++)
            {
                int parlaklik = img.at<uchar>(i, j);

                int yeni = (int)(parlaklik * 0.75 + 20 + 0.5);

                if (yeni > 255)
                {
                    yeni = 255;
                }
                if (yeni < 0)
                {
                    yeni = 0;
                }

                sonuc.at<uchar>(i, j) = yeni;
            }
        }

        auto bitis = chrono::high_resolution_clock::now();

        chrono::duration<double, milli> gecen = bitis - baslangic;
        cout << "Deneme " << t + 1 << ": " << gecen.count() << " ms" << endl;
        toplamSure += gecen.count();
    }

    double ortalama = toplamSure / TEKRAR;
    cout << endl << "Ortalama sure: " << ortalama << " ms" << endl;
    cout << "Piksel basina ortalama: " << ortalama * 1000000.0 / (genislik * yukseklik) << " ns" << endl;
}

// ------------------------------------------------------------
// MAIN: resmi oku, menuden gorev sec
// ------------------------------------------------------------
int main(int argc, char **argv)
{
    // istenirse resim adi komut satirindan verilebilir, yoksa resim.jpg
    string resimAdi = "resim.jpg";
    if (argc > 1)
    {
        resimAdi = argv[1];
    }

    // resmi tek kanalli (gri) olarak oku
    Mat img = imread(resimAdi, IMREAD_GRAYSCALE);

    if (img.empty())
    {
        cout << "Resim bulunamadi: " << resimAdi << endl;
        return -1;
    }

    cout << "Boyut: " << img.cols << " x " << img.rows << endl;

    int secim = -1;
    while (secim != 0)
    {
        cout << endl;
        cout << "===== MENU =====" << endl;
        cout << "1 - 4'e bol / 4 ile carp" << endl;
        cout << "2 - 4'e bol / 4 ile carp (4 thread)" << endl;
        cout << "3 - Histogram (4 thread)" << endl;
        cout << "4 - 0.75 ile carp + 20 ekle" << endl;
        cout << "5 - Hiz olcumu (gorev 4)" << endl;
        cout << "0 - Cikis" << endl;
        cout << "Secim: ";

        if (!(cin >> secim))
        {
            break;
        }
        cout << endl;

        if (secim == 1)
        {
            gorev1(img);
        }
        else if (secim == 2)
        {
            gorev2(img);
        }
        else if (secim == 3)
        {
            gorev3(img);
        }
        else if (secim == 4)
        {
            gorev4(img);
        }
        else if (secim == 5)
        {
            gorev5(img);
        }
        else if (secim != 0)
        {
            cout << "Gecersiz secim!" << endl;
        }
    }

    return 0;
}
