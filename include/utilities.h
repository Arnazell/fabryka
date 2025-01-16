#ifndef UTILITIES_H
#define UTILITIES_H
#include <sys/ipc.h>
#include <sys/types.h>
#include <iostream>


// rodzaje bledow
#define IPC_RESULT_ERROR -1

// definicja semaforow
#define sem_do_odczytania_x 0
#define sem_do_odczytania_y 1
#define sem_do_odczytania_z 2

#define sem_wolne_miejsca_x 3
#define sem_wolne_miejsca_y 4
#define sem_wolne_miejsca_z 5

#define sem_shelf_x 7
#define sem_shelf_y 8
#define sem_shelf_z 9

#define sem_command 6

// czasy wykonywania czynnosci
#define speed_machine_a 30
#define speed_machine_b 20
#define speed_supply_x 1
#define speed_supply_y 1
#define speed_supply_z 1
#define speed_recieving_package 1
#define speed_making_order 1


namespace utils
{

    // ---------------------------- SEMAFORY ----------------------------- //
    // zwiekszenie wartosci semafora
    void semafor_p(int semid, int sem, int value);

    // zmniejszenie wartosci semafora
    void semafor_v(int semid, int sem, int value);

    // zwiekszenie wartosci semafora
    void semafor_set(int semid, int sem, int value);

    // odczytanie wartosci semafora
    int semafor_value(int semid, int sem);

    // tworzenie zbioru semaforow - inicjacja zerami, zwraca semid
    int utworz_zbior_semaforow(key_t key, int count);

    // usuwanie zbioru semaforow
    int usun_zbior_semaforow(key_t key);

    // uzysknanie semid gdy zbior semaforow istnieje; inaczej -1
    int get_semid(key_t);

    // ---------------------------- KOLEJKI ----------------------------- //
    // tworzenie kolejki komunikatow
    int utworz_kolejke(key_t key);

    // uzysknanie msgid gdy kolejka istnieje; inaczej -1
    int get_msid(key_t key);

    // usuniecie kolejki
    int usun_kolejke(int id);
    // ---------------------------- PAMIEC ----------------------------- //
    typedef struct PamiecWspoldzielona
    {
        int id;
        size_t size;
        int flg;
        char *adres;
    } PamiecWspoldzielona;

    // tworzenie segmentu pamieci
    int utworz_segment_pamieci_dzielonej(PamiecWspoldzielona *pDzielona, key_t klucz, long size);

    // dolaczenie segmentu pamieci -> zwraca adres
    char* dolacz_segment_pamieci(int shared_id);

    // pobranie id segmentu pamieci
    int get_shared_id(key_t klucz);

    // odlaczenie segmentu z biezacego procesu
    int odlacz_segment_pamieci_dzielonej(PamiecWspoldzielona *pDzielona);

    // ustawienie do usuniecia segmnetu
    int ustaw_do_usuniecia_segment(PamiecWspoldzielona *pDzielona);

    // pobranie informajci o rozmiarze segmentu
    size_t pobierz_rozmiar_pamieci(int shared_id);


    // ---------------------------- WSPOLNE OBIEKTY ----------------------------- //
    // ------------- PRODUKTY
    class ProductX
    {
    public:
        ProductX(short weight);
        // paramety produktu // 2 bajty -> jedna jednostka
        short m_weight;
    };

    class ProductY
    {
    public:
        ProductY(int weight);
        // paramety produktu 4 bajty -> dwie jednostki
        int m_weight;
    };

    class ProductZ
    {
    public:
        ProductZ(int weight, short pojemnosc);
        // paramety produktu 4+2 bajty -> trzy jednostki
        int m_weight;
        short m_pojemnosc;
    };

    // ----------ENUMY
    // typ produktu
    enum product_type
    {
        X, Y, Z
    };
    // komendy dyrektora
    enum Command
    {
        stop_magazyn,
        stop_fabryka,
        stop_magazyn_fabryka,
        stop_bez_zapisu
    };

    // ----------------- FUNKCJE POMOCNICZE -----------------------------
    // losowanie numeru z zakresu a, b
    int random_number(int min, int max);

}




#endif //UTILITIES_H
