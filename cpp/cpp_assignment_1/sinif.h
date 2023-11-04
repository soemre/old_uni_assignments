#ifndef SINIF_BILGILERI_H
#define SINIF_BILGILERI_H

#include <iostream>
#include <map>
#include <string>
#include <iomanip>
using namespace std;

#include "structlar.h"
#include "ogrenci.h"

struct Sinif
{
public:
    Sinif(int ogrenciSayisi, SinavKatsayilari katsayilar) : ogrenciSayisi(ogrenciSayisi), katsayilar(katsayilar)
    {
        // Ogrencileri depolamak icin array yaratir.
        ogrenciler = new Ogrenci[ogrenciSayisi];

        int rastgeleOgrenciBilgiSayisi = rand() % 1200;

        // Verilen sayida rastgele ad ve soyadlar ile
        // rastgele not degeslerinde bunlari tersten bir sekilde yuzdeliklerini belirleyerek rastgelelik yaratır. ogrenciler oluştur.
        for (int i = 0; i < ogrenciSayisi; i++)
        {
            RastgeleOgrenciBilgileri rastgeleBilgiler = {
                i,
                ogrenciSayisi,
                rastgeleOgrenciBilgiSayisi,
            };

            *(ogrenciler + i) = Ogrenci::rastgeleOgrenci(rastgeleBilgiler);
        }
    }

    map<string, int> notDagilimi(bool print)
    {

        map<string, int> dagilim;

        for (unsigned int i = 0; i < ogrenciSayisi; i++)
        {
            NotBilgisi netNotBilgisi = (*(ogrenciler + i)).netNot(katsayilar);

            dagilim[netNotBilgisi.harf]++;
        }

        if (print)
        {
            cout << endl
                 << setw(40) << "Notlarin Sayisal Dagilimi (Net Nota Gore)" << endl;
            cout << setw(20) << "Harf Not" << setw(20) << "Alan Kisi Sayisi" << setw(20) << "Alan Kisi Yuzdesi" << endl
                 << string(60, '-') << endl;

            for (auto [k, v] : dagilim)
            {
                float notYuzdesi = (float)v / ogrenciSayisi * 100;

                cout << setw(20) << k << setw(20) << v << setw(20) << yuvarlanmisString(notYuzdesi) + "%" << endl;
            }
        }

        return dagilim;
    }

    void ogrencileriYazdir()
    {

        cout << endl
             << setw(70) << "Ogrenciler ve Notlari" << endl;

        cout << setw(10) << "Sira" << setw(20) << "Adi Soyadi" << setw(20) << "Yil Ici Not" << setw(20) << "Net Not" << endl
             << string(70, '-') << endl;

        for (unsigned int i = 0; i < ogrenciSayisi; i++)
        {

            Ogrenci ogr = *(ogrenciler + i);

            // Net notunu hesapla
            NotBilgisi netNotBilgisi = ogr.netNot(katsayilar);

            // Yil ici notunu hesapla
            NotBilgisi yilIciNotBilgisi = ogr.yilIciNot(katsayilar);

            // Ad Soyad ve Yil Sonu notunu yazdir Yazdir
            cout << setw(10) << ogr.no + 1;
            cout << setw(20) << ogr.ad + " " + ogr.soyad;
            cout << setw(20) << yilIciNotBilgisi.harf + " (" + yuvarlanmisString(yilIciNotBilgisi.sayisal) + ")";
            cout << setw(20) << netNotBilgisi.harf + " (" + yuvarlanmisString(netNotBilgisi.sayisal) + ")" << endl;
        }
    }

    void bilgileriYazdir()
    {
        float enDusukNot = -1;
        float enYuksekNot = -1;
        float ortalama = 0;

        for (int i = 0; i < ogrenciSayisi; i++)
        {
            NotBilgisi notBilgisi = (ogrenciler + i)->netNot(katsayilar);

            // Ortalamayi hesapla

            ortalama += notBilgisi.sayisal / ogrenciSayisi;

            // "En" degerlerini belirle

            if (enDusukNot == -1 || notBilgisi.sayisal < enDusukNot)
            {
                enDusukNot = notBilgisi.sayisal;
            }

            if (notBilgisi.sayisal > enYuksekNot)
            {
                enYuksekNot = notBilgisi.sayisal;
            }
        }

        cout << endl
             << setw(100) << "Sinif Bilgileri" << endl;
        cout << setw(20) << "Ogrenci Sayisi" << setw(20) << "Yil Ici Etkisi" << setw(20) << "En Dusuk Not" << setw(20) << "En Yuksek Not" << setw(20) << "Ortalama" << setw(20) << "Standart Sapma" << endl
             << string(120, '-') << endl;
        cout << setw(20) << ogrenciSayisi << setw(20) << yuvarlanmisString(katsayilar.yilIciEtki * 100) + "%" << setw(20) << setprecision(5) << enDusukNot << setw(20) << setprecision(5) << enYuksekNot << setw(20) << setprecision(5) << ortalama << setw(20) << setprecision(5) << standartSapma(ortalama) << endl;
    }

private:
    Ogrenci *ogrenciler;
    int ogrenciSayisi;
    SinavKatsayilari katsayilar;

    float standartSapma(float ortalama)
    {
        float deger = 0;

        for (int i = 0; i < ogrenciSayisi; i++)
        {
            NotBilgisi notBilgisi = (ogrenciler + i)->netNot(katsayilar);
            deger += pow(notBilgisi.sayisal - ortalama, 2);
        }

        return sqrt(deger / (ogrenciSayisi - 1));
    }

    string yuvarlanmisString(float sayi)
    {
        string sayiStringi = to_string(sayi);
        return sayiStringi.substr(0, sayiStringi.find(".") + 3);
    }
};

#endif