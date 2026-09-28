#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// her thread kendi satir araligindaki piksellerin histogramini cikariyor
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

int main()
{
    // resmi tek kanalli (gri) olarak oku
    Mat img = imread("resim.jpg", IMREAD_GRAYSCALE);

    if (img.empty())
    {
        cout << "Resim bulunamadi!" << endl;
        return -1;
    }

    int yukseklik = img.rows;
    int genislik = img.cols;
    cout << "Boyut: " << genislik << " x " << yukseklik << endl;
    cout << "Toplam piksel: " << genislik * yukseklik << endl << endl;

    const int PARCA = 4;
    int parcaYuksekligi = yukseklik / PARCA;

    // her parca icin ayri histogram (256 parlaklik degeri, hepsi 0'la basliyor)
    // ayri olduklari icin thread'ler birbirine karismiyor
    vector<vector<int>> parcaHist(PARCA, vector<int>(256, 0));

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

        threadler[k] = thread(histogramCikar, cref(img), ref(parcaHist[k]), baslangic, bitis);
    }

    // hepsinin bitmesini bekle
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

    // toplam piksel sayisina esit mi?
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
    ofstream dosya("histogram.txt");
    dosya << "Deger Piksel_Sayisi" << endl;
    for (int deger = 0; deger < 256; deger++)
    {
        dosya << deger << " " << anaHist[deger] << endl;
    }
    dosya.close();
    cout << endl << "Histogram histogram.txt dosyasina kaydedildi." << endl;

    // histogrami cizip goster (siyah zemin, beyaz cubuklar)
    int cizimYuksekligi = 300;
    Mat cizim = Mat::zeros(cizimYuksekligi, 256 * 2, CV_8UC1);

    // en buyuk degeri bul, cubuklari ona gore olceklemek icin
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

    imwrite("histogram.png", cizim);

    return 0;
}
