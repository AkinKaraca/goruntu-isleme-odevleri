#include <iostream>
#include <chrono>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

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

    // sonuc resmi icin bos matris
    Mat sonuc = Mat::zeros(yukseklik, genislik, CV_8UC1);

    // tek olcum yeterli olmaz, ayni islemi birkac kez tekrarlayip ortalama alacagiz
    const int TEKRAR = 10;
    double toplamSure = 0;

    for (int t = 0; t < TEKRAR; t++)
    {
        // sadece islem suresini olcuyoruz (resim okuma ve gosterme dahil degil)
        auto baslangic = chrono::high_resolution_clock::now();

        // tum piksellere tek tek bak
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
            }
        }

        auto bitis = chrono::high_resolution_clock::now();

        // gecen sureyi milisaniye olarak hesapla
        chrono::duration<double, milli> gecen = bitis - baslangic;
        cout << "Deneme " << t + 1 << ": " << gecen.count() << " ms" << endl;
        toplamSure += gecen.count();
    }

    double ortalama = toplamSure / TEKRAR;
    cout << endl << "Ortalama sure: " << ortalama << " ms" << endl;
    cout << "Piksel basina ortalama: " << ortalama * 1000000.0 / (genislik * yukseklik) << " ns" << endl;

    // resimleri goster
    imshow("Orijinal", img);
    imshow("0.75 ile carpilip 20 eklenmis", sonuc);
    waitKey(0);
    destroyAllWindows();

    // kaydet
    imwrite("sonuc.jpg", sonuc);

    return 0;
}
