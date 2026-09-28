#include <iostream>
#include <thread>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// her thread resmin kendine ait satir araligini isliyor
void parcaIsle(const Mat &img, Mat &bolu4, Mat &carpi4, int baslangic, int bitis, int no)
{
    for (int i = baslangic; i < bitis; i++)
    {
        for (int j = 0; j < img.cols; j++)
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
        }
    }
    cout << "Parca " << no << " bitti (satir " << baslangic << " - " << bitis - 1 << ")" << endl;
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

    // sonuc resimleri
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

        // her parca ayri bir thread'de calisiyor
        threadler[k] = thread(parcaIsle, cref(img), ref(bolu4), ref(carpi4), baslangic, bitis, k + 1);
    }

    // hepsinin bitmesini bekle
    for (int k = 0; k < PARCA; k++)
    {
        threadler[k].join();
    }

    // resimleri goster
    imshow("Orijinal", img);
    imshow("4'e bolunmus", bolu4);
    imshow("4 ile carpilmis", carpi4);
    waitKey(0);
    destroyAllWindows();

    // kaydet
    imwrite("bolu4.jpg", bolu4);
    imwrite("carpi4.jpg", carpi4);

    return 0;
}
