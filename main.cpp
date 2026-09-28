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

    // sonuc resimleri icin bos matrisler
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
