/*Generator danych do Hurtowni danych.
Skompiluj i uruchom.
Można poustawiać parametry, ale bez przesadnych wartości, bo nie ma zabezpieczeń i z braku czasu sporo hardkodu jest
Bartosz Rychcik s165659
Robert Nowicki s165496
*/
//#################################################################################################################################################_BIBLIOTEKI

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <cassert>
#include <cstdarg>
#include <random>
#include <map>
#include <algorithm>
#include <tuple>
#include <utility>
#pragma warning(disable:4996)
using namespace std;
//#################################################################################################################################################_DZIWNE

#define ________________________________________________________KOMENTARZ________________________________________________________________________(TRESC)
#define Read_String_Args(stream,ap,type) if (i != 0)stream << ',';stream << "'" << va_arg(ap, type) << "'";break
#define Read_Number_Args(stream,ap,type) if (i != 0)stream << ',';stream << va_arg(ap, type);break
#define Read_Point_Args(stream,ap,type) if (i != 0)stream << ',';point p = va_arg(ap, point);write << "geography::STGeomFromText('POINT(" << p.x << "," << p.y << ")', 4326)";break;
//#################################################################################################################################################_KONFIGURACJA

#define ILE_IMION 625
#define ILE_NAZWISK 20000
#define ILE_ULIC 1586
#define ILE_SAMOCHODOW 60 //pilnowac by byla wieksza/rowna od sumy samochodow uzywanych w miastach w strukturze ponizej.
#define ILE_KIEROWCOW 140 //zakladamy, ze 3 kierowcow jezdzi samochodem w trybie zmianowym a potem samochod sobie stoi
#define ILE_NAPRAW 50	
#define ILE_ZAMOWIEN 1000
#define ILE_OCZEKIWAN 200 //mozliwe ze nie wszystkie przejda gdy beda zajete miejsca postojowe
#define ILE_KOM 20
#define ILE_TRAS 200 //liczba ta jest suma tras z poszczegolnych miast ze structury miasto
#define ILE_MIEJSC_POSTOWYCH 40 //liczba ta jest suma miejsc_postojowych ze structury miasto
#define ILE_UZYTKOWNIKOW 1000
#define DZIS moja_data({05,11,2018}) 
#define TERAZ datetime({{05,11,2018},{00,00}})
#define PUNKT_ZERO datetime({{05,10,2018},{06,00}}) 
#define SERWIS string("MechCorp")
//#################################################################################################################################################_MIASTO

struct miasto
{
	string nazwa;
	int ile_centrum;
	int id_serwisu;
	bool czy_dostepna[7];
	int liczba_samochodow;
	pair<double, double> wspolrzedne_DD;
	int liczba_tras;
	int ile_postojow;
} miasto[] = { 
			 {string("Niechłonin"),2,362662,{ 1,1,0,1,0,1,1 },10,{53.170000000,20.040000000},50,10 },
			 {string("Gdańsk"),17,255534,{ 1,1,1,1,1,1,1 },20,{54.350000000,18.620000000},100,20 },
			 {string("Rościszewo"),4,371990,{ 1,1,0,0,1,1,1 },10,{52.900000000,19.770000000},50,10 }
		     };
//#################################################################################################################################################_USLUGA

struct usluga
{
	string nazwa;
	double koszt_staly;
	double koszt_per_km;
	bool czy_specjalna;
} usluga[] = {
			 { "przewóz osób",9.0,2.30,0 },
             { "przewóz zwierząt",10.0,2.80,1 },
			 { "transport dokumentów",10.0,1.40,1 },
			 { "przesyłka kurierska",10.0,1.40,1 },
			 { "zakupy",25.0,0.0,1 },
			 { "pomoc drogowa",40.0,0.0,1 }
			 }; //kontrakty reklamowe nie pasuja do struktury bazy, zreszta one sa zawsze oplacalne
//#################################################################################################################################################_SAMOCHOD

struct samochod
{
	string marka;
	string model;
} samochod[] = { 
			   {"Toyota","Auris"},
			   {"Ford","S-Max"},
			   {"Ford","Focus"},
			   {"Skoda","Octavia"},
			   {"Skoda","Fabia"} 
			   };
map<string, tuple<int,int,int>> samochody; //te sa potrzebne do trzymania kluczy glownych, dlatego mapa bo szybsza <nr_rej,<model,index,rok_prod>>
map<string, tuple<int, int, int>>::iterator it_sam[sizeof(miasto) / sizeof(miasto[0])]; //iteratory na pierwsze samochody z poszczegolnych miast
//#################################################################################################################################################_DATA

struct moja_data
{
	int dzien;
	int miesiac;
	int rok;
	friend ostream& operator << (ostream& os, const moja_data& md);
	friend bool operator >(moja_data& th, const moja_data& md);
	friend bool operator ==(moja_data& th, const moja_data& md);
};
//#################################################################################################################################################_CZAS

struct czas
{
	int godzina;
	int minuta;
	friend ostream& operator << (ostream& os, const czas& czas);
	friend bool operator >(czas& th, const czas& t);
	friend bool operator ==(czas& th, const czas& t);
};
//#################################################################################################################################################_DATETIME

struct datetime
{
	moja_data date;
	czas time;
	friend ostream& operator << (ostream& os, const datetime& dt );
	friend bool operator <(datetime& th, const datetime& dt);
	friend bool operator ==(datetime& th, const datetime& dt);
	friend datetime operator +(datetime& th, const datetime& dt);
};
//#################################################################################################################################################_NAPRAWA

struct naprawa
{
	int ile;
	datetime start[ILE_NAPRAW/2];
	datetime koniec[ILE_NAPRAW/2];
} naprawa[ILE_SAMOCHODOW];
//#################################################################################################################################################_UZYTKOWNIK

struct uzytkownik
{
	datetime data_rejestracji;
	int rabat;
	string sposob_rejestracji;
	string miasto;
	int rachunek;
	bool czy_ma_rachunek;
} uzytkownik[ILE_UZYTKOWNIKOW];
//#################################################################################################################################################_KIEROWCA

struct kierowca
{
	moja_data data_zatrudnienia;
} kierowca[ILE_KIEROWCOW];
//#################################################################################################################################################_JAZDA

struct jazda_samochodem 
{
	string samochod;
	datetime start;
	datetime koniec;
	int ile_zamowien;
	pair<datetime, datetime> zamowienia[50]; //wiecej niż 50 razy w jeden przedzial pewnie nie trafi generator, ale jak coś to trzeba zmienic
} jazda_samochodem[10000]; //jak wyrzuci blad to tu trzeba zwiekszyc
//#################################################################################################################################################_TRASY

struct trasy
{
	string miasto;
	string adres_poczatkowy;
	string adres_koncowy;
	double odleglosc;
	pair<double, double> wsp_pocz_DD;
} trasy[ILE_TRAS];  
//#################################################################################################################################################_POSTOJE

struct postoje
{
	pair<double, double> WSP_DD;
	int liczba_miejsc;
	int ind_dat;
	tuple<datetime, datetime, int, char> daty[ILE_OCZEKIWAN/2];
}postoje[ILE_MIEJSC_POSTOWYCH];
//#################################################################################################################################################_ZAMOWIENIA

struct zamowienia
{
	datetime czas_przyjecia;
	datetime czas_zakonczenia;
	int jazda_samochodem;
}zamowienia[ILE_ZAMOWIEN];
//#################################################################################################################################################_RACHUNEK

struct rachunek
{
	int id_rachunku;
	datetime data_wystawienia;
	int uzytkownik;
}rachunek[10000]; //jak wyrzuci blad to tu trzeba zwiekszyc
//#################################################################################################################################################_TABLICE

static int dni[] = { 0,31,59,90,120,151,181,212,243,273,304,334,365 };
string* imiona;
string* nazwiska;
string ulice[ILE_ULIC+5];
string pozytywne[ILE_KOM];
string negatywne[ILE_KOM];
enum plec { dziewucha, chlopak };
struct point { double x; double y; }; //specjalny typ zeby va_arg przepuscilo pair, bo niewiadomo czemu makro nie czyta template z utility
//#################################################################################################################################################_DEKLARACJE

void generuj_jazde_samochodem(datetime, int, int, map<string, tuple<int, int, int>>::iterator, int*, int, int*, ofstream&);
pair<double, double> generuj_wspolrzedne(pair<double, double>, double, double);
int generuj_czas_dojazdu_taxi(pair<double, double>, pair<double, double>);
int wyznacz_jazde_samochodem(datetime, datetime, int, string, string);
bool czy_samochod_zepsul_sie_na_trasie(int, datetime, datetime);
bool compare(tuple<datetime, datetime, int, char>a, tuple<datetime, datetime, int, char>b);
pair<int, string> wyznacz_postoj(datetime, datetime, int);
datetime o_ktorej_sie_zepsul(int, datetime, datetime);
bool czy_samochod_dostepny(int, datetime, datetime);
void weryfikacja_poprawnosci_oczekiwanie(ofstream&);
double wyznacz_koszt_podstawowy(string, double);
datetime generuj_datetime(datetime, datetime);
moja_data generuj_date(moja_data, moja_data);
int ile_miejsc_pos_w_miescie(string miasto); //hardkodowana, modyfikować w przypadku zmian miast
void wartosci(ofstream&, string typy ...); //hardkodowana, modyfikować w przypadku wypisywania nowych typow danych
double generuj_odleglosc(double, double);
string generuj_nr_rejestracyjny(string); //hardkodowana, modyfikować w przypadku zmian miast
void generuj_komentarz(ofstream&, int);
string generuj_PESEL(plec, moja_data);
int wyznacz_kierowce(int, int, int);
int generuj_dlugosc_podrozy(double);
string generuj_sposob_rejestracji(); //hardkodowana, modyfikować w przypadku zmian sposobow rejestracji
string generuj_sposob_zamowienia(); //hardkodowana, modyfikować w przypadku zmian sposobow zamowienia
unsigned long long generuj_nrtel();
string itoa(unsigned long long);
void powiadom(string, clock_t*);
string miasto_samochodu(string); //hardkodowana, modyfikować w przypadku zmian miast
int id_oddzialu_miasta(string); //hardkodowana, modyfikować w przypadku zmian miast
void generuj_naprawe(int, int);
string generuj_typ_platnosci(); //hardkodowana, modyfikować w przypadku zmian typow platnosci
int liczba_samochodow(string); //hardkodowana, modyfikować w przypadku zmian miast
string generuj_nazwisko(plec);
int generuj_stanlicznika(int);
czas generuj_czas(czas, czas);
int indeks_miejsca_pos(string miasto); //hardkodowana, modyfikować w przypadku zmian miast
int generuj_koszt_naprawy();
int generuj_usluge(string); //hardkodowana, modyfikować w przypadku zmian uslug
string generuj_imie(plec);
int generuj_trase(string);
string generuj_adres();
void generuj_trasy();
int dtoi(moja_data);
moja_data itod(int);
void wczytaj_dane();
int generuj_rabat();
czas itot(int);
int ttoi(czas);
//#################################################################################################################################################_SZABLON

map<string, string> szablony = {
	{ "Usluga","insert into Usluga (\"NazwaUslugi\", \"KosztStaly\", \"KosztPerKM\", \"CzySpecjalna\") values " },
	{ "DostepnoscUslugi","insert into DostepnoscUslugi (\"FK_Miasto\", \"FK_Usluga\", \"CzyDostepna\") values " },
	{ "Miasto","insert into Miasto (\"NazwaMiasta\", \"IleCentrumHandlowych\") values " },
	{ "UslugaPrzezSamochod","insert into UslugaPrzezSamochod (\"FK_Samochod\", \"FK_Usluga\") values " },
	{ "Uzytkownik","insert into Uzytkownik (\"IdUzytkownika\", \"Imie\", \"Nazwisko\", \"SposobRejestracji\", \"DataRejestracji\", \"Rabat\", \"FK_Miasto\") values " },
	{ "Rachunek","insert into Rachunek (\"NrRachunku\", \"DataWystawienia\", \"DataOplacenia\", \"TypPlatnosci\", \"FK_Uzytkownik\") values " },
	{ "NaprawaSamochodu","insert into NaprawaSamochodu (\"IdNaprawy\", \"DataOddania\", \"DataOdebrania\", \"Koszt\", \"FK_Oddzial\", \"FK_Samochod\", \"FK_Miasto\") values " },
	{ "Oddzial","insert into Oddzial (\"IdSerwisu\", \"Telefon\", \"FK_Firma\", \"FK_Miasto\") values " },
	{ "Firma","insert into Firma (\"NazwaFirmy\", \"Telefon\") values " },
	{ "Samochod","insert into Samochod (\"NrRejestracyjny\", \"Marka\", \"Model\", \"StanLicznika\", \"RokProdukcji\", \"DataAktualizacjiStanuLicznika\", \"WaznoscPrzegladu\", \"DataOstatniegoPrzegladu\") values " },
	{ "MiejscePostojowe","insert into MiejscePostojowe (\"Wspolrzedne\", \"LiczbaMiejsc\", \"FK_Miasto\") values " },
	{ "Oczekiwanie","insert into Oczekiwanie (\"IdOczekiwania\", \"CzasRozpoczecia\", \"CzasZakonczenia\", \"FK_JazdaSamochodem\", \"FK_MiejscePostojowe\") values " },
	{ "JazdaSamochodem","insert into JazdaSamochodem (\"IdPrzypisania\", \"FK_Samochod\", \"FK_Kierowca\", \"CzasRozpoczecia\", \"CzasZakonczenia\") values " },
	{ "Kierowca","insert into Kierowca (\"IdKierowcy\", \"Imie1\", \"Imie2\", \"Nazwisko\", \"DataZatrudnienia\", \"PESEL\", \"WaznoscLicencji\") values " },
	{ "ZamowieniaUslugi","insert into ZamowienieUslugi (\"IdZamowienia\", \"FK_Usluga\", \"FK_Rachunek\", \"AdresPoczatkowy\", \"AdresKoncowy\", \"PozycjaNajblizszejTaxi\", \"CzasPrzyjeciaZamowienia\", \"CzasRozpoczecia\", \"CzasZakonczenia\", \"PrzejechaneKM\", \"SposobZamowienia\", \"Koszt\", \"FK_Miasto\", \"FK_JazdaSamochodem\") values " }
};
//#################################################################################################################################################_MAIN

int main()
{
	imiona = new string[ILE_IMION + 5];
	nazwiska = new string[ILE_NAZWISK + 5];
	clock_t t = clock();
	//srand(time_t(NULL)); random_device rd; srand(rd());
	wczytaj_dane();
	generuj_trasy();
	ofstream write;
	write.open("C:/Users/wyima/Desktop/1.taxicorp.sql");
	

	________________________________________________________KOMENTARZ________________________________________________________________________(USLUGA)
	write << "-- Table: Usluga\n";
	for (int i = 0; i < sizeof(usluga) / sizeof(usluga[0]); i++)
	{
		write << szablony["Usluga"];
		wartosci(write, "sddb", usluga[i].nazwa, usluga[i].koszt_staly,usluga[i].koszt_per_km,usluga[i].czy_specjalna);
		write << "\n";
	}
	write << "go\n";
	powiadom("Usluga", &t);
	

	________________________________________________________KOMENTARZ________________________________________________________________________(MIASTO)
	write << "-- Table: Miasto\n";
	for (int i = 0; i < sizeof(miasto) / sizeof(miasto[0]); i++)
	{
		write << szablony["Miasto"];
		wartosci(write, "si", miasto[i].nazwa, miasto[i].ile_centrum);
		write << "\n";
	}
	write << "go\n";
	powiadom("Miasto", &t);
	

	________________________________________________________KOMENTARZ________________________________________________________________________(DOSTEPNOSCUSLUGI)
	write << "-- Table: DostepnoscUslugi\n";
	for (int i = 0; i < sizeof(miasto) / sizeof(miasto[0]); i++)
	{
		for (int j = 0; j < sizeof(usluga) / sizeof(usluga[0]); j++)
		{
			write << szablony["DostepnoscUslugi"];
			wartosci(write, "ssb", miasto[i].nazwa, usluga[j].nazwa, miasto[i].czy_dostepna[j]);
			write << "\n";
		}
	}
	write << "go\n";
	powiadom("DostepnoscUslugi", &t);
	

	________________________________________________________KOMENTARZ________________________________________________________________________(KIEROWCA)
	write << "-- Table: Kierowca\n";
	for (int i = 0; i < ILE_KIEROWCOW; i++)
	{
		string imie, imie2;
		int plec = rand() % 10;
		int drugie_imie = rand() % 10;
		kierowca[i].data_zatrudnienia = generuj_date({ 1,1,1990 }, { 1,1,2018 });

		write << szablony["Kierowca"];
		if (plec > 1)
		{
			imie = generuj_imie(plec::chlopak);
			if (drugie_imie > 7)
			{
				imie2 = imie;
				while (imie2 == imie)
					imie2 = generuj_imie(plec::chlopak);
				wartosci(write, "isssmsm",
					i + 1,
					imie,
					imie2,
					generuj_nazwisko(plec::chlopak),
					kierowca[i].data_zatrudnienia,
					generuj_PESEL(plec::chlopak, generuj_date({ 1,1,1960 }, { 1,1,kierowca[i].data_zatrudnienia.rok - 20 })),
					generuj_date(DZIS, { DZIS.dzien,DZIS.miesiac,DZIS.rok + 15 }));
			}
			else
				wartosci(write, "isssmsm",
					i + 1,
					imie,
					string("null"),
					generuj_nazwisko(plec::chlopak),
					kierowca[i].data_zatrudnienia,
					generuj_PESEL(plec::chlopak, generuj_date({ 1,1,1960 }, { 1,1,kierowca[i].data_zatrudnienia.rok - 20 })),
					generuj_date(DZIS, { DZIS.dzien,DZIS.miesiac,DZIS.rok + 15 }));
		}
		else
		{
			imie = generuj_imie(plec::dziewucha);
			if (drugie_imie > 7)
			{
				imie2 = imie;
				while (imie2 == imie)
					imie2 = generuj_imie(plec::dziewucha);
				wartosci(write, "isssmsm",
					i + 1,
					imie,
					imie2,
					generuj_nazwisko(plec::dziewucha),
					kierowca[i].data_zatrudnienia,
					generuj_PESEL(plec::dziewucha, generuj_date({ 1,1,1960 }, { 1,1,kierowca[i].data_zatrudnienia.rok - 20 })),
					generuj_date(DZIS, { DZIS.dzien,DZIS.miesiac,DZIS.rok + 15 }));
			}
			else
				wartosci(write, "isssmsm",
					i + 1,
					imie,
					string("null"),
					generuj_nazwisko(plec::dziewucha),
					kierowca[i].data_zatrudnienia,
					generuj_PESEL(plec::dziewucha, generuj_date({ 1,1,1960 }, { 1,1,kierowca[i].data_zatrudnienia.rok - 20 })),
					generuj_date(DZIS, { DZIS.dzien,DZIS.miesiac,DZIS.rok + 15 }));
		}

		write << "\n";
	}
	write << "go\n";
	powiadom("Kierowca", &t);
	

	________________________________________________________KOMENTARZ________________________________________________________________________(SAMOCHOD)
	write << "-- Table: Samochod\n";
	int samochod_ktory, rok_prod,ilosc=0;
	string wygenerowany;
	moja_data przeglad;
	for (int i = 0; i < sizeof(miasto) / sizeof(miasto[0]); i++)
	{
		for (int j = 0; j < miasto[i].liczba_samochodow; j++)
		{
			wygenerowany = generuj_nr_rejestracyjny(miasto[i].nazwa);
			if (bool(get<0>(samochody[wygenerowany])) != true)
			{
				samochod_ktory = rand() % (sizeof(::samochod) / sizeof(::samochod[0]));
				rok_prod = rand() % 19 + 2000;
				samochody[wygenerowany] = { samochod_ktory,ilosc,rok_prod };
				przeglad = generuj_date({ DZIS.dzien,DZIS.miesiac,DZIS.rok - 1 }, DZIS);
				if (rok_prod == 2018)przeglad = generuj_date({ 01,01,2018 }, DZIS);
				write << szablony["Samochod"];
				wartosci(write, "sssiimmm",
					wygenerowany,
					::samochod[samochod_ktory].marka,
					::samochod[samochod_ktory].model,
					generuj_stanlicznika(rok_prod),
					rok_prod,
					przeglad,
					moja_data({ przeglad.dzien,przeglad.miesiac,przeglad.rok + 1 }),
					przeglad);
				write << "\n";
				ilosc++;
			}
			else
				j--; //dzieki temu liczba wygenerowanych samochodow bedzie zawsze poprawna
		}
	}
	for (int k=0; ilosc < ILE_SAMOCHODOW; ilosc++,k++)
	{
		wygenerowany = generuj_nr_rejestracyjny(miasto[k % (sizeof(miasto) / sizeof(miasto[0]))].nazwa);
		if (bool(get<0>(samochody[wygenerowany])) != true)
		{
			samochod_ktory = rand() % (sizeof(::samochod) / sizeof(::samochod[0]));
			rok_prod = rand() % 19 + 2000;
			samochody[wygenerowany] = { samochod_ktory,ilosc,rok_prod };
			przeglad = generuj_date({ DZIS.dzien,DZIS.miesiac,DZIS.rok - 1 }, DZIS);
			if (rok_prod == 2018)przeglad = generuj_date({ 01,01,2018 }, DZIS);
			write << szablony["Samochod"];
			wartosci(write, "sssiimmm",
				wygenerowany,
				::samochod[samochod_ktory].marka,
				::samochod[samochod_ktory].model,
				generuj_stanlicznika(rok_prod),
				rok_prod,
				przeglad,
				moja_data({ przeglad.dzien,przeglad.miesiac,przeglad.rok + 1 }),
				przeglad);
			write << "\n";
			ilosc++;
		}
	}
	write << "go\n";
	powiadom("Samochod", &t);
	

	________________________________________________________KOMENTARZ________________________________________________________________________(FIRMA)
	write << "-- Table: Firma\n";
	write << szablony["Firma"];
	wartosci(write, "su", SERWIS,generuj_nrtel());
	write << "\n";
	write << "go\n";
	powiadom("Firma", &t);
	

	________________________________________________________KOMENTARZ________________________________________________________________________(ODDZIAL)
	write << "-- Table: Oddzial\n";
	for (int i = 0; i < sizeof(miasto) / sizeof(miasto[0]); i++)
	{
		write << szablony["Oddzial"];
		wartosci(write, "iuss", miasto[i].id_serwisu,generuj_nrtel(),SERWIS,miasto[i]);
		write << "\n";
	}
	write << "go\n";
	powiadom("Oddzial", &t);
	

	________________________________________________________KOMENTARZ________________________________________________________________________(UZYTKOWNIK)
	write << "-- Table: Uzytkownik\n";
	int FK_rachunek = 100000;
	for (int i = 0; i < ILE_UZYTKOWNIKOW; i++)
	{
		uzytkownik[i].miasto = miasto[rand() % (sizeof(miasto) / sizeof(miasto[0]))].nazwa;
		uzytkownik[i].rabat = generuj_rabat();
		uzytkownik[i].data_rejestracji = generuj_datetime({ {1,1,2010},{0,0} }, PUNKT_ZERO);
		uzytkownik[i].sposob_rejestracji = generuj_sposob_rejestracji();
		uzytkownik[i].czy_ma_rachunek = false;
		if (uzytkownik[i].sposob_rejestracji == "kontraktowy")
			uzytkownik[i].rachunek = FK_rachunek++;
		else
			uzytkownik[i].rachunek = 0;
		write << szablony["Uzytkownik"];
		if (rand() % 2)
			wartosci(write, "issscis",
				i + 1,
				generuj_imie(plec::chlopak),
				generuj_nazwisko(plec::chlopak),
				uzytkownik[i].sposob_rejestracji,
				uzytkownik[i].data_rejestracji,
				uzytkownik[i].rabat,
				uzytkownik[i].miasto
				);
		else
			wartosci(write, "issscis",
				i + 1,
				generuj_imie(plec::dziewucha),
				generuj_nazwisko(plec::dziewucha),
				uzytkownik[i].sposob_rejestracji,
				uzytkownik[i].data_rejestracji,
				uzytkownik[i].rabat,
				uzytkownik[i].miasto
			);
		write << "\n";
	}
	write << "go\n";
	powiadom("Uzytkownik", &t);
	delete[] imiona; delete[] nazwiska;

	________________________________________________________KOMENTARZ________________________________________________________________________(USLUGAPRZEZSAMOCHOD)
	write << "-- Table: UslugaPrzezSamochod\n";
	for (auto it=samochody.begin();it!=samochody.end();it++)
	{
		for (int i = 0; i < sizeof(usluga) / sizeof(usluga[0]); i++)
		{
			if (usluga[i].nazwa != "przewóz zwierząt" || (usluga[i].nazwa == "przewóz zwierząt"&&::samochod[get<0>(it->second)].model == "S-Max"))
			{
				write << szablony["UslugaPrzezSamochod"];
				wartosci(write, "ss", it->first, usluga[i].nazwa);
				write << "\n";
			}
		}
	}
	write << "go\n";
	powiadom("UslugaPrzezSamochod", &t);

	________________________________________________________KOMENTARZ________________________________________________________________________(NAPRAWASAMOCHODU)
	write << "-- Table: NaprawaSamochodu\n";
	map<string, tuple<int, int, int>>::iterator it_napraw;
	int losuj_miasto, skok, i = 0;
	it_sam[i] = samochody.begin();
	for (auto it2 = ++samochody.begin(); it2 != samochody.end(); it2++)
	{
		if (miasto_samochodu(it2->first) != miasto_samochodu(it_sam[i]->first))
			it_sam[++i] = it2;
	}
	for (int i = 0; i < ILE_NAPRAW; i++) //to jest tak skomplikowane zeby nie psuly sie samochody rezerwowe (zrobilem tak, bo w dalszych tabelach nie dalem rady koncepcyjnie ogarnac mnogosci rzeczy)
	{
		losuj_miasto = rand() % (sizeof(miasto)/sizeof(miasto[0]));
		it_napraw = it_sam[losuj_miasto];
		skok = rand() % (liczba_samochodow(miasto_samochodu(it_napraw->first))); //to jest tak zagmatwane bo mapa ma kolejnosc kluczy, a nie miast z tablicy (maskowane indeksy w odpowiedniej strukturze)
		for (int i = 0; i < skok; i++)it_napraw++; //it_napraw wskazuje teraz na wylosowany samochod do naprawy
		generuj_naprawe(get<1>(it_napraw->second),get<2>(it_napraw->second)); //czy ta funkcja dziala dobrze to ja nie wiem, ale chodzi o to by samochod juz w naprawie nie szedl znowu do niej
		write << szablony["NaprawaSamochodu"];
		wartosci(write, "icciiss",
			i+1,
			::naprawa[get<1>(it_napraw->second)].start[::naprawa[get<1>(it_napraw->second)].ile - 1],
			::naprawa[get<1>(it_napraw->second)].koniec[::naprawa[get<1>(it_napraw->second)].ile - 1],
			generuj_koszt_naprawy(),
			id_oddzialu_miasta(it_napraw->first),
			it_napraw->first,
			miasto_samochodu(it_napraw->first));
		write << "\n";
		
	}
	write << "go\n";
	powiadom("NaprawaSamochodu", &t);
	

	________________________________________________________KOMENTARZ________________________________________________________________________(JAZDASAMOCHODEM)
	write << "-- Table: JazdaSamochodem\n";
	int index = 1,dat=0,naprawy=0;
	for (datetime i = PUNKT_ZERO; i < TERAZ; i = i + datetime({ {00,00,0000}, {8,00} })) 
	{
		dat++;
		for (int j = 0; j < sizeof(miasto) / sizeof(miasto[0]); j++)
		{
			auto it = it_sam[j];
			naprawy = 0;
			if (dat % 2 == 1) //kazdy samochod pracuje
				for (int k = 0; k < liczba_samochodow(miasto_samochodu(it_sam[j]->first)); k++)
				{
					generuj_jazde_samochodem(i, dat, j, it, &naprawy, k, &index, write); 
					it++;
				}
			else if(dat%4==0) //tylko pewna czesc, bo pracownicy nie moga 24/7 pracowac
				for (int k = 0; k < liczba_samochodow(miasto_samochodu(it_sam[j]->first)); k++)
				{
					if (k % 2 == 0)
						generuj_jazde_samochodem(i, dat, j, it, &naprawy, k, &index, write);
					it++;
				}
			else //druga czesc
				for (int k = 0; k < liczba_samochodow(miasto_samochodu(it_sam[j]->first)); k++)
				{
					if (k % 2 == 1)
						generuj_jazde_samochodem(i, dat, j, it, &naprawy, k, &index, write);
					it++;
				}	
		}
	}
	write << "go\n";
	powiadom("JazdaSamochodem", &t);
	

	________________________________________________________KOMENTARZ________________________________________________________________________(ZAMOWIENIAUSLUGI)
	write << "-- Table: ZamowieniaUslugi\n";
	int _id = 1, usl, trasa, FK_JazdaSamochodem, uzytk, nr_rachunku = 0;
	string spo_zam;
	datetime czasprzyjeciazamowienia, czas_rozpoczecia, czas_zakonczenia;
	pair<double, double> pozycjataxi;
	double koszt;

	for (int i = 0; i < ILE_ZAMOWIEN; ) //nie kazde zlecenie jestesmy w stanie wykonac w podanych ramach czasowych, wiec inkrementacja tylko przy przyjeciu 
	{
		write << szablony["ZamowieniaUslugi"];
		uzytk = rand() % ILE_UZYTKOWNIKOW; //losujemy uzytkownika ktory zamowil sobie usluge
		usl = generuj_usluge(uzytkownik[uzytk].miasto); //losujemy usluge z uwzglednieniem dostepnosci tej uslugi w miescie
		trasa = generuj_trase(uzytkownik[uzytk].miasto); //losujemy trase z uwzglednieniem wystepowania tej trasy w miescie
		spo_zam = generuj_sposob_zamowienia(); //losujemy sposob zamowienia taxi
		czasprzyjeciazamowienia = generuj_datetime(PUNKT_ZERO, TERAZ);
		if (spo_zam == "złapana") //jesli jest zlapana na ulicy
		{
			pozycjataxi = trasy[trasa].wsp_pocz_DD; //to pozycja taxi jest taka jak punkt startowy trasy
			czas_rozpoczecia = czasprzyjeciazamowienia; //i czas_rozpoczecia jest taki sam jak czasprzyjecia zamowienia
		}
		else //gdy inaczej jest zamowiona
		{
			pozycjataxi = generuj_wspolrzedne(trasy[trasa].wsp_pocz_DD, 0.0005, 0.005); //to taxi gdzies jest blisko
			czas_rozpoczecia = czasprzyjeciazamowienia + datetime({ { 0,0,0 },{ 0,generuj_czas_dojazdu_taxi(pozycjataxi, trasy[trasa].wsp_pocz_DD)} }); //no i mamy opoznienie az dojedzie
		}
		czas_zakonczenia = czas_rozpoczecia + datetime({ { 0,0,0 },{ 0,generuj_dlugosc_podrozy(trasy[trasa].odleglosc)} }); //czas wysadzenia pasazera
		koszt = wyznacz_koszt_podstawowy(usluga[usl].nazwa, trasy[trasa].odleglosc)*(100.0 - uzytkownik[uzytk].rabat) / 100.0; //koszt podrozy z uwzglednionym rabatem uzytkownika
		koszt = int(koszt * 100) / 100.0;

		FK_JazdaSamochodem = wyznacz_jazde_samochodem(czasprzyjeciazamowienia,czas_zakonczenia,index,trasy[trasa].miasto,usluga[usl].nazwa); //nie moze byc kolizji czasowych, musi sie zgadzac miasto i przeznaczenie samochodu

		if (FK_JazdaSamochodem == -1) //jesli -1 to znaczy ze funkcja zwrocila blad i nie jestesmy w stanie zrealizowac takiego zamowienia (odrzucamy je)
			continue;
		else
			i++; //jak jest wszystko ok, no to mamy kolejne poprawne


		if (uzytkownik[uzytk].sposob_rejestracji == "kontraktowy")
		{
			if (!uzytkownik[uzytk].czy_ma_rachunek)
			{
				rachunek[nr_rachunku].data_wystawienia = generuj_datetime({ { 25,9,2018 },{ 00,00 } }, { { 30,9,2018 },{ 23,59 } });
				rachunek[nr_rachunku].id_rachunku = uzytkownik[uzytk].rachunek;
				rachunek[nr_rachunku].uzytkownik = uzytk;
				nr_rachunku++;
				uzytkownik[uzytk].czy_ma_rachunek = true;
			}
			wartosci(write, "isisspcccdsdsi",
				_id++,
				usluga[usl].nazwa,
				uzytkownik[uzytk].rachunek,
				trasy[trasa].adres_poczatkowy,
				trasy[trasa].adres_koncowy,
				point({ pozycjataxi.first,pozycjataxi.second }),
				czasprzyjeciazamowienia,
				czas_rozpoczecia,
				czas_zakonczenia,
				trasy[trasa].odleglosc,
				spo_zam,
				koszt,
				trasy[trasa].miasto,
				FK_JazdaSamochodem
			);
		}
		else
		{
			rachunek[nr_rachunku].data_wystawienia = czas_zakonczenia;
			rachunek[nr_rachunku].id_rachunku = FK_rachunek;
			rachunek[nr_rachunku].uzytkownik = uzytk;
			nr_rachunku++;
			wartosci(write, "isisspcccdsdsi",
				_id++,
				usluga[usl].nazwa,
				FK_rachunek++,
				trasy[trasa].adres_poczatkowy,
				trasy[trasa].adres_koncowy,
				point({ pozycjataxi.first,pozycjataxi.second }),
				czasprzyjeciazamowienia,
				czas_rozpoczecia,
				czas_zakonczenia,
				trasy[trasa].odleglosc,
				spo_zam,
				koszt,
				trasy[trasa].miasto,
				FK_JazdaSamochodem
			);
		}
		write << "\n";
	}
	write << "go\n";
	powiadom("ZamowieniaUslugi", &t);


	________________________________________________________KOMENTARZ________________________________________________________________________(RACHUNEK)
	write << "-- Table: Rachunek\n";
	for (int i = 0; i < nr_rachunku; i++)
	{
		write << szablony["Rachunek"];
		if (uzytkownik[rachunek[i].uzytkownik].sposob_rejestracji == "kontraktowy")
			wartosci(write, "iccsi", 
				rachunek[i].id_rachunku,
				rachunek[i].data_wystawienia,
				generuj_datetime({ {00,00,0000},{00,00} }, { {14,01,0000},{00,00} }),
				string("karta bezgotówkowa"),
				rachunek[i].uzytkownik
			);
		else
			wartosci(write, "iccsi",
				rachunek[i].id_rachunku,
				rachunek[i].data_wystawienia,
				rachunek[i].data_wystawienia,
				generuj_typ_platnosci(),
				rachunek[i].uzytkownik
			);
		write << "\n";
	}
	write << "go\n";
	powiadom("Rachunek", &t);
	

	________________________________________________________KOMENTARZ________________________________________________________________________(MIEJSCEPOSTOJOWE)
		write << "-- Table: MiejscePostojowe\n";
	int ind_pos = 0;
	for (int i = 0; i < sizeof(miasto) / sizeof(miasto[0]); i++)
	{
		for (int j = 0; j < miasto[i].ile_postojow; j++)
		{
			write << szablony["MiejscePostojowe"];
			postoje[ind_pos].liczba_miejsc = rand() % 30 + 10;
			postoje[ind_pos].WSP_DD = generuj_wspolrzedne(miasto[i].wspolrzedne_DD, 0.001, 0.06);
			postoje[ind_pos].ind_dat = 0;
			wartosci(write, "pis", postoje[ind_pos].WSP_DD, postoje[ind_pos].liczba_miejsc, miasto[i].nazwa);
			ind_pos++;
			write << "\n";
		}
	}
	write << "go\n";
	powiadom("MiejscePostojowe", &t);


	________________________________________________________KOMENTARZ________________________________________________________________________(OCZEKIWANIE)
	write << "-- Table: Oczekiwanie\n";
	for (int i = 0; i < ILE_OCZEKIWAN;)
	{
		czas_rozpoczecia = generuj_datetime(PUNKT_ZERO, TERAZ);
		czas_zakonczenia = generuj_datetime(czas_rozpoczecia + datetime({ {00,00,0000},{00,30} }), czas_rozpoczecia + datetime({ {00,00,0000},{02,00} }));
		pair<int, string> wolny_termin = wyznacz_postoj(czas_rozpoczecia, czas_zakonczenia, index);
		if (wolny_termin.first == -1)
			continue;
		else
			i++;
		int miejsce = indeks_miejsca_pos(wolny_termin.second) + rand() % ile_miejsc_pos_w_miescie(wolny_termin.second);
		postoje[miejsce].daty[postoje[miejsce].ind_dat] = { czas_rozpoczecia,czas_zakonczenia,wolny_termin.first, 's' };
		postoje[miejsce].ind_dat++;
		postoje[miejsce].daty[postoje[miejsce].ind_dat] = { czas_zakonczenia,czas_rozpoczecia,wolny_termin.first, 'k' };
		postoje[miejsce].ind_dat++;
	}

	weryfikacja_poprawnosci_oczekiwanie(write);

	write << "go\n";
	powiadom("Oczekiwanie", &t);


	________________________________________________________KOMENTARZ________________________________________________________________________(CSVKA)
	write.close();
	write.open("C:/Users/wyima/Desktop/1.mobilna.csv");
	write << "idZamowienia;CzasOczekiwania;JakoscObslugi;KomfortJazdy;Uwagi\n";
	for (int i = 0; i < ILE_ZAMOWIEN; i++)
		generuj_komentarz(write,i+1);
	write.close();
	powiadom("CSVKA", &t);
	cout << "Nacisnij losowy przycisk, by zakonczyc program...";
	std::cin.get();
}
//#################################################################################################################################################_DEFINICJE

void wczytaj_dane()
{
	string s;
	int i = 0;
	ifstream rimie, rnazwisko, rulice,rpoz,rneg;
	rnazwisko.open("nazwiska.txt");
	rimie.open("imiona.txt");
	rulice.open("ulice.txt");
	rpoz.open("pozytywne.txt");
	rneg.open("negatywne.txt");
	while (rimie >> s)imiona[i++] = s;
	i ^= i;
	while (rnazwisko >> s)nazwiska[i++] = s;
	i ^= i;
	while (getline(rulice,s))ulice[i++] = s;
	i ^= i;
	while (getline(rpoz, s))pozytywne[i++] = s;
	i ^= i;
	while (getline(rneg, s))negatywne[i++] = s;

	rnazwisko.close();
	rulice.close();
	rimie.close();
	rpoz.close();
	rneg.close();
	if (imiona[0] == "" || nazwiska[0] == "" || ulice[0]== "" || pozytywne[0]=="" || negatywne[0]=="")
		assert(false);
}

int generuj_rabat()
{
	int rabat = rand() % 10;
	if (rabat >= 1 && rabat <= 5)
		return rabat * 10;
	else
		return 0;
}

unsigned long long generuj_nrtel()
{
	unsigned long long tel = rand() % 4 + 5;
	for (int i = 0; i < 8; i++)
		tel = tel * 10 + rand() % 10;
	return tel;
}

string generuj_imie(plec x)
{
	string imie = imiona[rand() % ILE_IMION];
	if (x == chlopak)
		while (imie[imie.size() - 1] == 'a')
			imie = imiona[rand() % ILE_IMION];
	else
		while (imie[imie.size() - 1] != 'a')
			imie = imiona[rand() % ILE_IMION];
	return imie;
}

string generuj_nazwisko(plec x) //zakladamy, ze wszystkie nazwiska w txt sa w formie meskiej
{
	string nazwisko = nazwiska[rand() % ILE_NAZWISK];
	if (x == dziewucha && nazwisko[nazwisko.size() - 1] == 'i')
		nazwisko[nazwisko.size() - 1] = 'a';
	return nazwisko;
}

moja_data generuj_date(moja_data a = { 01,01,1900 }, moja_data b = DZIS) //zakladamy, ze nie ma lat przestepnych
{
	int d1 = dtoi(a);
	int d2 = dtoi(b);
	if (d1 == d2)return a;
	if (d2 > d1)
		d1 += rand() % (d2 - d1 + 1);
	else
		assert(false);
	return itod(d1);
}

czas generuj_czas(czas a = { 00,00 }, czas b = { 23,59 })
{
	int h1 = ttoi(a);
	int h2 = ttoi(b);

	if (h1 == h2)return a;
	if (h2 > h1)
		h1 += rand() % (h2 - h1 + 1);
	else
		h1 += rand() % (h2 + 24 * 60 - h1 + 1);
	return itot(h1);
}

moja_data itod(int date)
{
	int i = 0;
	while (date % 365 > dni[i])i++;
	if (i == 0)
		return { 31,12,date / 365 };
	else
		return { (date % 365) - dni[i - 1],i,date / 365 };
}

czas itot(int time)
{
	time %= (24 * 60);

	return { time / 60,time % 60 };
}

int dtoi(moja_data date)
{
	return date.rok * 365 + dni[max(0,date.miesiac - 1)] + date.dzien;
}

int ttoi(czas time)
{
	return time.godzina * 60 + time.minuta;
}

string itoa(unsigned long long num)
{
	string _string = "";
	unsigned long long dl = 0, pom = num;
	while (num > 0)
	{
		dl++;
		num /= 10;
	}
	_string.resize(unsigned int(dl), '0');
	while (pom > 0)
	{
		_string[unsigned int(dl--) - 1] = char(pom % 10 + '0');
		pom /= 10;
	}
	return _string;
}

string generuj_nr_rejestracyjny(string miasto)
{
	string nrrej;
	unsigned int numer, i = 0;
	if (miasto == "Gdańsk")
		nrrej = "GDA ";
	else if (miasto == "Niechłonin")
		nrrej = "NDZ ";
	else if (miasto == "Rościszewo")
		nrrej = "WSE ";
	else
		assert(false);
	if (rand() % 2)
		numer = rand() % 900 + 100;
	else
		numer = rand() % 90 + 10;

	nrrej += itoa(numer);

	while (nrrej.size()<8)
		nrrej += char(rand() % 26 + 'A');
	return nrrej;
}

int generuj_stanlicznika(int rokprodukcji)
{
	return (2019 - rokprodukcji)*(rand() % 8000 + 8251);
}

string generuj_sposob_zamowienia()
{
	int val = rand() % 10;
	if (val < 2)
		return "złapana";
	else if (val >= 2 && val < 5)
		return "telefonicznie";
	else
		return "aplikacja";
}

string generuj_sposob_rejestracji()
{
	int val = rand() % 10;
	if (val < 1)
		return "kontraktowy";
	else if (val >= 1 && val < 4)
		return "telefoniczny";
	else
		return "aplikacja";
}

void powiadom(string komunikat = "", clock_t *t = 0x0)
{
	*t = clock() - *t;
	cout << "Zakonczono generowanie tablicy " << komunikat << " " << float(*t) / 1000 << "s\n";
}

string generuj_PESEL(plec x, moja_data data_urodzenia)
{
	unsigned long long PESEL = (data_urodzenia.rok % 100) * 100 + data_urodzenia.miesiac;
	if (data_urodzenia.rok >= 1800 && data_urodzenia.rok < 1900)
		PESEL += 80;
	else if (data_urodzenia.rok >= 2000 && data_urodzenia.rok < 2100)
		PESEL += 20;
	else if (data_urodzenia.rok >= 2100 && data_urodzenia.rok < 2200)
		PESEL += 40;
	else if (data_urodzenia.rok >= 1800 && data_urodzenia.rok < 1900)
		PESEL += 60;

	PESEL = PESEL * 100 + data_urodzenia.dzien;
	for (int i = 0; i < 3; i++)
		PESEL = PESEL * 10 + rand() % 10;
	PESEL *= 10;
	if (x == dziewucha)
		PESEL += 2 * rand() % 5;
	else
		PESEL += 1 + 2 * rand() % 5;

	int walidacja = 0, mnoznik = 7;
	unsigned long long c_PESEL = PESEL;
	while (c_PESEL > 0)
	{
		walidacja += mnoznik * (c_PESEL % 10);
		c_PESEL /= 10;
		mnoznik += 2;
		if (mnoznik == 5)mnoznik += 2;
		mnoznik %= 10;
	}

	PESEL *= 10;
	PESEL += walidacja % 10;

	string ret = itoa(PESEL);
	while (ret.size() < 11)
		ret = "0" + ret;
	return ret;
}

void wartosci(ofstream& write, string typy ...)
{
	va_list ap;
	va_start(ap, typy);
	write << "(";
	for (unsigned int i = 0; i < typy.size(); i++)
	{
		switch (typy[i])
		{
		case 'i':
			Read_Number_Args(write, ap, int);
		case 'd':
			Read_Number_Args(write, ap, double);
		case 's':
			Read_String_Args(write, ap, string);
		case 'b':
			Read_Number_Args(write, ap, bool);
		case 'm':
			Read_String_Args(write, ap, moja_data);
		case 't':
			Read_String_Args(write, ap, czas);
		case 'c':
			Read_String_Args(write, ap, datetime);
		case 'u':
			Read_Number_Args(write, ap, unsigned long long);
		case 'p':
			Read_Point_Args(write, ap, point);
		default:
			assert(false);
		}
	}
	write << ");";
	va_end(ap);
}

datetime generuj_datetime(datetime a, datetime b)
{
	if(a.date==b.date)
		return {generuj_date(a.date,b.date),generuj_czas(a.time,b.time)};
	else
		return { generuj_date(a.date,b.date),generuj_czas()};
}

int generuj_koszt_naprawy()
{
	int val = rand() % 300 + 150;
	return val * 10;
}

string miasto_samochodu(string nr_rej)
{
	if (nr_rej[0] == 'G')
		return "Gdańsk";
	else if (nr_rej[0] == 'N')
		return "Niechłonin";
	else if (nr_rej[0] == 'W')
		return "Rościszewo";
	else
	{
		assert(false);
		return "";
	}
}

int id_oddzialu_miasta(string nr_rej)
{
	if (nr_rej[0] == 'G')
		return 255534;
	else if (nr_rej[0] == 'N')
		return 362662;
	else if (nr_rej[0] == 'W')
		return 371990;
	else
	{
		assert(false);
		return -1;
	}
}

void generuj_naprawe(int index,int rok_prod)
{
	datetime start, koniec;
	kolejna_proba:
		start = generuj_datetime({ { 01,01,rok_prod },{ 00,00 } }, TERAZ);
		koniec = generuj_datetime(start, start + datetime({ { 07,01,0000 },{ 00,00 } }));
		for (int i = 0; i < naprawa[index].ile; i++)
		{
			if (naprawa[index].koniec[i]<start || koniec<naprawa[index].start[i])
			{
			}
			else
				goto kolejna_proba;
		}
		::naprawa[index].start[::naprawa[index].ile] = start;
		::naprawa[index].koniec[::naprawa[index].ile] = koniec;
		::naprawa[index].ile++;
}

string generuj_typ_platnosci()
{
	int val = rand() % 10;
	if (val < 3)
		return "gotówka";
	else if (val >= 3 && val < 8)
		return "karta płatnicza";
	else 
		return "voucher";
}

int liczba_samochodow(string miasto)
{
	for (int i = 0; i < sizeof(miasto) / sizeof(miasto[0]); i++)
	{
		if (::miasto[i].nazwa == miasto)
			return ::miasto[i].liczba_samochodow;
	}
	
	assert(false);
	return -1;
}

int wyznacz_kierowce(int miasto, int samochod, int data)
{
	int ret = 0;
	for (int i = 0; i < miasto; i++)
		ret += (liczba_samochodow(miasto_samochodu(it_sam[i]->first)) * 3); //pierwszy indeks kierowcy w naszym miescie
	if (data % 4 == 1)
		ret += (3 * samochod);
	else if (data % 4 == 3)
		ret += (3 * samochod + 2);
	else
		ret += (3 * samochod + 1);
	return ret+1;
}

bool czy_samochod_dostepny(int index, datetime a, datetime b)
{
	for (int i = 0; i < naprawa[index].ile; i++)
	{
		if (naprawa[index].koniec[i] < a || b < naprawa[index].start[i])
		{
		}
		else
			return false;
	}
	return true;
}

bool czy_samochod_zepsul_sie_na_trasie(int index, datetime a, datetime b)
{
	for (int i = 0; i < naprawa[index].ile; i++)
		if (a < naprawa[index].start[i] && naprawa[index].start[i] < b)
			return true;
	return false;
}

datetime o_ktorej_sie_zepsul(int index, datetime a, datetime b)
{
	for (int i = 0; i < naprawa[index].ile; i++)
		if (a < naprawa[index].start[i] && naprawa[index].start[i] < b)
			return naprawa[index].start[i];
	assert(false);
	return datetime({ {}, {} });
}

void generuj_jazde_samochodem(datetime i, int dat,int j, map<string, tuple<int, int, int>>::iterator it,int* naprawy,int k,int* index, ofstream& write)
{
	write << szablony["JazdaSamochodem"];
	if (czy_samochod_dostepny(get<1>(it->second), i, i + datetime({ { 00,00,0000 },{ 8,00 } })))
	{
		jazda_samochodem[*index].samochod = it->first;
		jazda_samochodem[*index].start = i;
		jazda_samochodem[*index].koniec = i + datetime({ { 00,00,0000 },{ 8,00 } });
		wartosci(write, "isicc", (*index)++, it->first, wyznacz_kierowce(j, k, dat), i, i + datetime({ { 00,00,0000 },{ 8,00 } }));
	}
	else
	{
		(*naprawy)++;
		auto it_nap = it_sam[j];
		for (int p = 0; p < liczba_samochodow(miasto_samochodu(it_sam[j]->first)) + (*naprawy); p++)
			it_nap++; //tu jest znowu uposledzony wybor samochodu rezerwowego (wystepuje po tych normalnych)

		if (czy_samochod_zepsul_sie_na_trasie(get<1>(it->second), i, i + datetime({ { 00,00,0000 },{ 8,00 } })))
		{
			datetime zepsucie = o_ktorej_sie_zepsul(get<1>(it->second), i, i + datetime({ { 00,00,0000 },{ 8,00 } }));
			jazda_samochodem[*index].samochod = it->first;
			jazda_samochodem[*index].start = i;
			jazda_samochodem[*index].koniec = i + datetime({ { 00,00,0000 },{ 8,00 } });
			wartosci(write, "isicc", (*index)++, it->first, wyznacz_kierowce(j, k, dat), i, zepsucie); //jedziemy dobrym
			write << "\n";
			jazda_samochodem[*index].samochod = it_nap->first;
			jazda_samochodem[*index].start = i;
			jazda_samochodem[*index].koniec = i + datetime({ { 00,00,0000 },{ 8,00 } });
			write << szablony["JazdaSamochodem"];
			wartosci(write, "isicc", (*index)++, it_nap->first, wyznacz_kierowce(j, k, dat), zepsucie, i + datetime({ { 00,00,0000 },{ 8,00 } })); //dali nam inny
		}
		else //calkowicie niedostepny wiec od razu daja nam inny
		{
			wartosci(write, "isicc", (*index)++, it_nap->first, wyznacz_kierowce(j, k, dat), i, i + datetime({ { 00,00,0000 },{ 8,00 } }));
			jazda_samochodem[*index].samochod = it_nap->first;
			jazda_samochodem[*index].start = i;
			jazda_samochodem[*index].koniec = i + datetime({ { 00,00,0000 },{ 8,00 } });
		}
	}
	write << "\n";
}

double wyznacz_koszt_podstawowy(string usluga, double km)
{
	for (int i = 0; i < sizeof(usluga) / sizeof(usluga[0]); i++)
		if (::usluga[i].nazwa == usluga)
			return ::usluga[i].koszt_staly + ::usluga[i].koszt_per_km*km;
	assert(false);
	return -1.0;
}

double generuj_odleglosc(double fMin, double fMax)
{
	float f = (float)rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}

void generuj_trasy()
{
	int index = 0;
	for (int i = 0; i < sizeof(miasto) / sizeof(miasto[0]); i++)
	{
		for (int j = 0; j < miasto[i].liczba_tras; j++)
		{
			trasy[index].miasto = miasto[i].nazwa;
			trasy[index].adres_poczatkowy = generuj_adres();
			trasy[index].adres_koncowy = generuj_adres();
			trasy[index].odleglosc = generuj_odleglosc(0.5,17.8);
			trasy[index].wsp_pocz_DD = generuj_wspolrzedne(miasto[i].wspolrzedne_DD,0.001,0.06); //punkt odniesienia zeby potem taxi w poblizu bylo
			index++;
		}
	}
}

string generuj_adres()
{
	static int ost_los = -1; //ostatni wylosowany, zeby nie generowalo adresow na tej samej ulicy
	int numerek = rand() % 99+1;
	while (numerek == ost_los)
		numerek = rand() % 99 + 1;
	string ulica = ::ulice[rand() % ILE_ULIC];
	ost_los = numerek;
	return ulica + itoa(numerek);
}

pair<double, double> generuj_wspolrzedne(pair<double, double> odnies,double min,double max)
{
	double x = generuj_odleglosc(min, max);
	double y = generuj_odleglosc(min, max);
	x = rand() % 2 ? x : -x;
	y = rand() % 2 ? y : -y;

	return { odnies.first + x,odnies.second + y };
}

int generuj_usluge(string miasto)
{
	int val = rand()%20;
	if (val < 14)
		val = 0;
	else if (val >= 14 && val <= 15)
		val = 3;
	else if (val == 16)
		val = 5;
	else if (val == 17)
		val = 4;
	else if (val == 18)
		val = 2;
	else if (val = 19)
		val = 1;

	for (int i = 0; i < sizeof(miasto) / sizeof(miasto[0]); i++)
		if (::miasto[i].nazwa == miasto && ::miasto[i].czy_dostepna[val])
			return val;
	return 0;
} 

int generuj_trase(string miasto)
{
	int a=0;
	for (int i = 0; i < sizeof(miasto) / sizeof(miasto[0]); i++)
	{
		if (::miasto[i].nazwa == miasto)
			return rand() % (::miasto[i].liczba_tras) + a;
		else
			a += ::miasto[i].liczba_tras;
	}
	assert(false);
	return -1;
}

int generuj_dlugosc_podrozy(double km)
{
	int predkosc = rand()%15 + 20;
	return int(ceil(km*60/predkosc));
}

int generuj_czas_dojazdu_taxi(pair<double, double> a, pair<double, double> b) 
{
	return generuj_dlugosc_podrozy(hypot(fabs(a.first - b.first), fabs(a.second - b.second))/0.00005);

}

int wyznacz_jazde_samochodem(datetime a, datetime b,int liczba_jazd,string miasto,string usluga)
{
	for (int i = 1; i < liczba_jazd; i++)
	{
		if (jazda_samochodem[i].start < a && b < jazda_samochodem[i].koniec && miasto_samochodu(jazda_samochodem[i].samochod) == miasto)
		{
			if (usluga != "przewóz zwierząt" || (usluga == "przewóz zwierząt" && samochod[get<0>(samochody[jazda_samochodem[i].samochod])].model == "S-Max"))
			{
				bool dobry = true;
				for (int j = 0; j < jazda_samochodem[i].ile_zamowien; j++)
				{
					if (jazda_samochodem[i].zamowienia[j].second < a || b < jazda_samochodem[i].zamowienia[j].first)
					{
					}
					else
						dobry = false;
				}

				if (dobry)
				{
					jazda_samochodem[i].zamowienia[jazda_samochodem[i].ile_zamowien].first = a;
					jazda_samochodem[i].zamowienia[jazda_samochodem[i].ile_zamowien].second = b;
					jazda_samochodem[i].ile_zamowien++;
					return i;
				}
			}
		}
		else if (b < jazda_samochodem[i].start)
			return -1;
	}
	return -1;
}

pair<int,string> wyznacz_postoj(datetime a, datetime b,int liczba_jazd)
{
	for (int i = 1; i < liczba_jazd; i++)
	{
		if (jazda_samochodem[i].start < a && b < jazda_samochodem[i].koniec)
		{
			bool dobry = true;
			for (int j = 0; j < jazda_samochodem[i].ile_zamowien; j++)
			{
				if (jazda_samochodem[i].zamowienia[j].second < a || b < jazda_samochodem[i].zamowienia[j].first)
				{
				}
				else
					dobry = false;
			}

			if (dobry)
			{
				jazda_samochodem[i].zamowienia[jazda_samochodem[i].ile_zamowien].first = a;
				jazda_samochodem[i].zamowienia[jazda_samochodem[i].ile_zamowien].second = b;
				jazda_samochodem[i].ile_zamowien++;
				return {i,miasto_samochodu(jazda_samochodem[i].samochod)};
			}
		}
		else if (b < jazda_samochodem[i].start)
			return {-1,""};
	}
	return { -1,"" };
}

void generuj_komentarz(ofstream& write,int id)
{
	int val = rand() % 10;
	int kom = rand() % 10;
	if (val < 4) //pozytywne
	{
		if (kom < 8) //bez koma
			write << id << ";" << rand() % 3 + 3 << ";" << rand() % 3 + 3 << ";" << rand() % 3 + 3 << "; \n";
		else //z komem
			write << id << ";" << rand() % 3 + 3 << ";" << rand() % 3 + 3 << ";" << rand() % 3 + 3 << ";" << pozytywne[rand() % ILE_KOM] << "\n";
	}
	else if (val == 4)//negatywne
	{
		if (kom < 8) //bez koma
			write << id << ";" << rand() % 3 + 1 << ";" << rand() % 3 +1 << ";" << rand() % 3 + 1 << "; \n";
		else //z komem
			write << id << ";" << rand() % 3 + 1 << ";" << rand() % 3 + 1 << ";" << rand() % 3 + 1 << ";" << negatywne[rand() % ILE_KOM] << "\n";
	}
	else //pominieta ocena
		write << id << ";" << 0 << ";" << 0 << ";" << 0 << "; \n";
}

int ile_miejsc_pos_w_miescie(string miasto)
{
	for (int i = 0; i < sizeof(::miasto) / sizeof(::miasto[0]); i++)
		if (::miasto[i].nazwa == miasto)
			return ::miasto[i].ile_postojow;
	assert(false);
	return -1;
}

int indeks_miejsca_pos(string miasto)
{
	int val = 0;
	for (int i = 0; i < sizeof(::miasto) / sizeof(::miasto[0]); i++)
		if (::miasto[i].nazwa == miasto)
			return val;
		else
			val += ::miasto[i].ile_postojow;
	assert(false);
	return -1;
}

bool compare(tuple<datetime, datetime, int, char> a, tuple<datetime, datetime, int, char> b)
{
	return get<0>(a) < get<0>(b);
}

void weryfikacja_poprawnosci_oczekiwanie(ofstream& write)
{
	int zajetosc = 0;
	int ind_ocze = 1;
	for (int i = 0; i < ILE_MIEJSC_POSTOWYCH; i++)
	{
		sort(postoje[i].daty, postoje[i].daty + postoje[i].ind_dat,compare);
		for (int j = 0; j < postoje[i].ind_dat; j++)
		{
			if (get<3>(postoje[i].daty[j]) == 's')
			{
				zajetosc++;
				if (zajetosc < postoje[i].liczba_miejsc)
				{
					write << szablony["Oczekiwanie"];
					wartosci(write, "iccip", ind_ocze++, get<0>(postoje[i].daty[j]), get<1>(postoje[i].daty[j]), get<2>(postoje[i].daty[j]), postoje[i].WSP_DD);
					write << "\n";
				}
			}
			else
				zajetosc--;
		}
	}
}
//#################################################################################################################################################_PRZYJACIELE

ostream& operator << (ostream& os, const moja_data& md)
{
	if (md.dzien > 31 || md.dzien < 1 || md.miesiac < 1 || md.miesiac>12)
	{
		cout << md.rok << "-" << setfill('0') << setw(2) << md.miesiac << "-" << setfill('0') << setw(2) << md.dzien;
		assert(false);
	}
	os << md.rok << "-" << setfill('0') << setw(2) << md.miesiac << "-" << setfill('0') << setw(2) << md.dzien;
	return os;
}

bool operator >(moja_data& th, const moja_data& md)
{
	return dtoi(th) < dtoi(md);
}

bool operator ==(moja_data& th, const moja_data& md)
{
	return dtoi(th) == dtoi(md);
}

ostream & operator<<(ostream & os, const czas & czas)
{
	if (czas.minuta < 0 || czas.minuta>59 || czas.godzina < 0 || czas.godzina>23)
	{
		cout << setfill('0') << setw(2) << czas.godzina << ":" << setfill('0') << setw(2) << czas.minuta;
		assert(false);
	}
	os << setfill('0') << setw(2) << czas.godzina << ":" << setfill('0') << setw(2) << czas.minuta;
	return os;
}

bool operator >(czas& th, const czas& t)
{
	return ttoi(th) < ttoi(t);
}

bool operator ==(czas& th, const czas& t)
{
	return ttoi(th) == ttoi(t);
}

ostream& operator << (ostream& os, const datetime& dt)
{
	os << dt.date << " " << dt.time;
	return os;
}

bool operator <(datetime& th, const datetime& dt)
{
	if (th.date == dt.date)
		return th.time > dt.time;
	else
		return th.date > dt.date;
}

bool operator ==(datetime& th, const datetime& dt)
{
	return (th.date == dt.date && th.time == dt.time);
}

datetime operator +(datetime& th, const datetime& dt)
{
	datetime ret;
	if (ttoi(th.time) + ttoi(dt.time) > ttoi({ 23,59 }))
		ret = { itod(dtoi(th.date) + dtoi(dt.date) + 1),itot(ttoi(th.time) + ttoi(dt.time)) };
	else
		ret = { itod(dtoi(th.date) + dtoi(dt.date)),itot(ttoi(th.time) + ttoi(dt.time)) };

	return ret;
}