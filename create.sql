create database taxicorpdb
go

use taxicorpdb
go

create table Firma(
	NazwaFirmy varchar(20) primary key,
	Telefon varchar(9) not null	
)
go

create table Miasto(
	NazwaMiasta varchar(20) primary key,	
	IleCentrumHandlowych integer not null	
)
go

create table Oddzial(
	IdSerwisu integer primary key,
	Telefon varchar(9) not null,
	FK_Firma varchar(20) foreign key references Firma not null,
	FK_Miasto varchar(20) foreign key references Miasto not null
)
go

create table Usluga(
	NazwaUslugi varchar(20) primary key,
	KosztStaly decimal(6, 2) not null,
	KosztPerKM decimal(4, 2) not null,
	CzySpecjalna bit not null
)
go

create table Samochod(
	NrRejestracyjny varchar(10) primary key,
	Marka varchar(20) not null,
	Model varchar(20) not null,
	StanLicznika integer not null,
	RokProdukcji integer not null,
	DataAktualizacjiStanuLicznika datetime not null,
	WaznoscPrzegladu datetime not null,
	DataOstatniegoPrzegladu datetime
)
go

create table UslugaPrzezSamochod(
	FK_Samochod varchar(10) foreign key references Samochod not null ,
	FK_Usluga varchar(20) foreign key references Usluga not null,
	primary key ("FK_Samochod", "FK_Usluga")
)
go

create table DostepnoscUslugi(
	FK_Miasto varchar(20) foreign key references Miasto not null,
	FK_Usluga varchar(20) foreign key references Usluga not null,
	CzyDostepna bit not null,
	primary key ("FK_Miasto", "FK_Usluga")
)
go

create table Uzytkownik(
	IdUzytkownika integer primary key,
	Imie varchar(15) not null,
	Nazwisko varchar(30) not null,
	SposobRejestracji varchar(20) not null,
	DataRejestracji datetime not null,
	Rabat integer not null,
	FK_Miasto varchar(20) foreign key references Miasto 
)
go

create table Kierowca(
	IdKierowcy integer primary key,
	Imie1 varchar(15) not null,
	Imie2 varchar(15),
	Nazwisko varchar(30) not null,
	DataZatrudnienia datetime,
	PESEL varchar(11) not null,
	WaznoscLicencji datetime
)
go

create table NaprawaSamochodu(
	IdNaprawy integer primary key,
	DataOddania datetime,
	DataOdebrania varchar(15) not null,
	Koszt decimal(4,2) not null,
	FK_Oddzial integer foreign key references Oddzial,
	FK_Samochod varchar(10) foreign key references Samochod not null,
	FK_Miasto varchar(20) foreign key references Miasto not null
)
go

create table JazdaSamochodem(
	IdPrzypisania integer primary key,
	FK_Samochod varchar(10) foreign key references Samochod,
	FK_Kierowca integer foreign key references Kierowca,
	CzasRozpoczecia datetime,
	CzasZakonczenia datetime	
)
go

create table MiejscePostojowe(	
	IdPostoju integer primary key,
	Wspolrzedne geography not null,
	LiczbaMiejsc integer not null,
	FK_Miasto varchar(20) foreign key references Miasto not null	
)
go

create table Oczekiwanie(
	IdOczekiwania integer primary key,
	CzasRozpoczecia datetime not null,
	CzasZakonczenia datetime,
	FK_JazdaSamochodem integer foreign key references JazdaSamochodem not null,
	FK_MiejscePostojowe integer foreign key references MiejscePostojowe not null	
)
go

create table Rachunek(
	NrRachunku varchar(15) primary key,
	DataWystawienia datetime not null,
	DataOplacenia datetime,
	TypPlatnosci varchar(10),
	FK_Uzytkownik integer foreign key references Uzytkownik not null	
)
go

create table ZamowienieUslugi(
	IdZamowienia varchar(20) primary key,
	FK_Usluga varchar(20) foreign key references Usluga,
	FK_Rachunek varchar(15) foreign key references Rachunek,
	AdresPoczatkowy varchar(40) not null,
	AdresKoncowy varchar(40) not null,
	PozycjaNajblizszejTaxi geography not null,
	CzasPrzyjeciaZamowienia datetime not null,
	CzasRozpoczecia datetime,
	CzasZakonczenia datetime,
	PrzejechaneKM decimal(4, 2),
	SposobZamowienia varchar(20) not null,
	Koszt decimal(4, 2),
	FK_Miasto varchar(20) foreign key references Miasto not null,
	FK_JazdaSamochodem integer foreign key references JazdaSamochodem	
)
go

-------------------------------------------------------------------------------------------------
SELECT * FROM ZamowienieUslugi
SELECT * FROM Rachunek
SELECT * FROM Oczekiwanie
SELECT * FROM MiejscePostojowe
SELECT * FROM JazdaSamochodem
SELECT * FROM NaprawaSamochodu
SELECT * FROM Kierowca
SELECT * FROM Uzytkownik
SELECT * FROM DostepnoscUslugi
SELECT * FROM UslugaPrzezSamochod
SELECT * FROM Samochod
SELECT * FROM Usluga
SELECT * FROM Oddzial
SELECT * FROM Miasto
SELECT * FROM Firma
--------------------------------------------------------------------------------------------------

DROP TABLE ZamowienieUslugi
DROP TABLE Rachunek
DROP TABLE Oczekiwanie
DROP TABLE MiejscePostojowe
DROP TABLE JazdaSamochodem
DROP TABLE NaprawaSamochodu
DROP TABLE Kierowca
DROP TABLE Uzytkownik
DROP TABLE DostepnoscUslugi
DROP TABLE UslugaPrzezSamochod
DROP TABLE Samochod
DROP TABLE Usluga
DROP TABLE Oddzial
DROP TABLE Miasto
DROP TABLE Firma