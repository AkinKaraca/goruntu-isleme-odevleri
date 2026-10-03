#include <iostream>
#include <fstream>
#include <thread>
#include <opencv2/opencv.hpp>
#include <ctime>

using namespace std;
using namespace cv;

void gorev1(Mat img)
{
    Mat bolu4 = Mat::zeros(img.rows, img.cols, CV_8UC1);
    Mat carpi4 = Mat::zeros(img.rows, img.cols, CV_8UC1);

    for (int i = 0; i < img.rows; i++)
    {
        for (int j = 0; j < img.cols; j++)
        {
            int parlaklik = img.at<uchar>(i, j);
            bolu4.at<uchar>(i, j) = parlaklik / 4;

            int yeni = parlaklik * 4;
            if (yeni > 255) yeni = 255;
            carpi4.at<uchar>(i, j) = yeni;
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

void parcaIsle(Mat img, Mat bolu4, Mat carpi4, int baslangic, int bitis)
{
    for (int i = baslangic; i < bitis; i++)
    {
        for (int j = 0; j < img.cols; j++)
        {
            int parlaklik = img.at<uchar>(i, j);
            bolu4.at<uchar>(i, j) = parlaklik / 4;

            int yeni = parlaklik * 4;
            if (yeni > 255) yeni = 255;
            carpi4.at<uchar>(i, j) = yeni;
        }
    }
}

void gorev2(Mat img)
{
    Mat bolu4 = Mat::zeros(img.rows, img.cols, CV_8UC1);
    Mat carpi4 = Mat::zeros(img.rows, img.cols, CV_8UC1);

    int parca = img.rows / 4;

    thread t1(parcaIsle, img, bolu4, carpi4, 0, parca);
    thread t2(parcaIsle, img, bolu4, carpi4, parca, parca * 2);
    thread t3(parcaIsle, img, bolu4, carpi4, parca * 2, parca * 3);
    thread t4(parcaIsle, img, bolu4, carpi4, parca * 3, img.rows);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    imshow("Orijinal", img);
    imshow("4'e bolunmus", bolu4);
    imshow("4 ile carpilmis", carpi4);
    waitKey(0);
    destroyAllWindows();

    imwrite("gorev2_bolu4.jpg", bolu4);
    imwrite("gorev2_carpi4.jpg", carpi4);
}

void histogramCikar(Mat img, int hist[], int baslangic, int bitis)
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

void gorev3(Mat img)
{
    int h1[256] = {0}, h2[256] = {0}, h3[256] = {0}, h4[256] = {0};
    int parca = img.rows / 4;

    thread t1(histogramCikar, img, h1, 0, parca);
    thread t2(histogramCikar, img, h2, parca, parca * 2);
    thread t3(histogramCikar, img, h3, parca * 2, parca * 3);
    thread t4(histogramCikar, img, h4, parca * 3, img.rows);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    int anaHist[256] = {0};
    for (int i = 0; i < 256; i++)
    {
        anaHist[i] = h1[i] + h2[i] + h3[i] + h4[i];
    }

    ofstream dosya("gorev3_histogram.txt");
    dosya << "Deger Piksel_Sayisi" << endl;
    for (int i = 0; i < 256; i++)
    {
        dosya << i << " " << anaHist[i] << endl;
    }
    dosya.close();
    cout << "Histogram gorev3_histogram.txt dosyasina kaydedildi." << endl;

    int cizimYuksekligi = 300;
    Mat cizim = Mat::zeros(cizimYuksekligi, 512, CV_8UC1);

    int enBuyuk = 0;
    for (int i = 0; i < 256; i++)
    {
        if (anaHist[i] > enBuyuk) enBuyuk = anaHist[i];
    }

    for (int i = 0; i < 256; i++)
    {
        int cubukBoyu = (anaHist[i] * cizimYuksekligi) / enBuyuk;
        rectangle(cizim, Point(i * 2, cizimYuksekligi - 1),
                  Point(i * 2 + 1, cizimYuksekligi - 1 - cubukBoyu), Scalar(255), FILLED);
    }

    imshow("Orijinal", img);
    imshow("Histogram", cizim);
    waitKey(0);
    destroyAllWindows();
}

void gorev4(Mat img)
{
    Mat sonuc = Mat::zeros(img.rows, img.cols, CV_8UC1);

    for (int i = 0; i < img.rows; i++)
    {
        for (int j = 0; j < img.cols; j++)
        {
            int parlaklik = img.at<uchar>(i, j);
            int yeni = (parlaklik * 0.75) + 20;

            if (yeni > 255) yeni = 255;
            if (yeni < 0) yeni = 0;

            sonuc.at<uchar>(i, j) = yeni;
        }
    }

    imshow("Orijinal", img);
    imshow("0.75 ile carpilip 20 eklenmis", sonuc);
    waitKey(0);
    destroyAllWindows();

    imwrite("gorev4_sonuc.jpg", sonuc);
}

void gorev5(Mat img)
{
    Mat sonuc = Mat::zeros(img.rows, img.cols, CV_8UC1);

    clock_t baslangic = clock();

    for (int t = 0; t < 10; t++)
    {
        for (int i = 0; i < img.rows; i++)
        {
            for (int j = 0; j < img.cols; j++)
            {
                int parlaklik = img.at<uchar>(i, j);
                int yeni = (parlaklik * 0.75) + 20;

                if (yeni > 255) yeni = 255;
                if (yeni < 0) yeni = 0;

                sonuc.at<uchar>(i, j) = yeni;
            }
        }
        cout << t + 1 << ". deneme tamamlandi." << endl;
    }

    clock_t bitis = clock();
    double gecenSure = (double)(bitis - baslangic) / CLOCKS_PER_SEC;

    cout << "10 islem icin toplam sure: " << gecenSure << " saniye" << endl;
    cout << "Ortalama sure: " << gecenSure / 10.0 << " saniye" << endl;
}

int main()
{
    Mat img = imread("resim.jpg", IMREAD_GRAYSCALE);

    if (img.empty())
    {
        cout << "Resim bulunamadi! Lutfen ayni klasorde 'resim.jpg' oldugundan emin olun." << endl;
        return -1;
    }

    int secim = -1;
    while (secim != 0)
    {
        cout << "\n===== MENU =====\n";
        cout << "1 - 4'e bol / 4 ile carp\n";
        cout << "2 - 4'e bol / 4 ile carp (4 thread)\n";
        cout << "3 - Histogram (4 thread)\n";
        cout << "4 - 0.75 ile carp + 20 ekle\n";
        cout << "5 - Hiz olcumu (gorev 4)\n";
        cout << "0 - Cikis\n";
        cout << "Seciminiz: ";

        cin >> secim;

        if (secim == 1) gorev1(img);
        else if (secim == 2) gorev2(img);
        else if (secim == 3) gorev3(img);
        else if (secim == 4) gorev4(img);
        else if (secim == 5) gorev5(img);
        else if (secim != 0) cout << "Gecersiz secim!\n";
    }

    return 0;
}
