#include <iostream>
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

    // tum piksellere tek tek bak
    for (int i = 0; i < yukseklik; i++)
    {
        for (int j = 0; j < genislik; j++)
        {
            int parlaklik = img.at<uchar>(i, j);

            // 0.75 ile carp, 20 ekle (+0.5 yuvarlamak icin)
            int yeni = (int)(parlaklik * 0.75 + 20 + 0.5);

            // 0-255 araligi disina ciktiysa sinirla
            // (bu formulde max deger 211 cikar ama kontrol koymak iyi)
            if (yeni > 255)
            {
                yeni = 255;
            }
            if (yeni < 0)
            {
                yeni = 0;
            }

            sonuc.at<uchar>(i, j) = yeni;

            // ilk birkac pikselin degerlerini yazdir
            if (i == 0 && j < 5)
            {
                cout << "Piksel (" << i << ", " << j << ") -> " << parlaklik
                     << " | yeni: " << (int)sonuc.at<uchar>(i, j) << endl;
            }
        }
    }

    // resimleri goster
    imshow("Orijinal", img);
    imshow("0.75 ile carpilip 20 eklenmis", sonuc);
    waitKey(0);
    destroyAllWindows();

    // kaydet
    imwrite("sonuc.jpg", sonuc);

    return 0;
}
