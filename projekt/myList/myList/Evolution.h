#pragma once
#include "pch.h"
#include "myList.h"
#include <string>

class Evolution
{
public:
	///===========================================================================
	///								Parametry
	///===========================================================================
	///
	/// Liczebnosc populacji.
	static const unsigned int population_size = 1600;
	/// Dlugosc chromosomu / osobnika.
	/// jesli 0, dlugosc wybierana losowo pomiedzy
	/// chr_min a chr_max
	static const unsigned int chromosome_length = 3;
	static const unsigned int chr_min = 2;
	static const unsigned int chr_max = 10;
	/// Min wartosc genu.
	static const unsigned int min_gen_value = 0;
	/// Max wartosc genu
	static const unsigned int max_gen_value = 5000;
	/// Wybor funkcji dopasowania
	static const unsigned int func_nr = 2;
	/// 1 -> max x = 50
	/// 2 -> max x = 475
	///
	/// ile % populacji przystepuje do rozmnazania.
	double reproductionFactor;

	/// Liczba pokolen. Dla dobrze dobranych parametrow
	/// powinno wystarczyc 100 - 200 pokolen.
	unsigned int iterations;

	/// plik wejsciowy
	std::string pathIN;

	/// plik wyjsciowy
	std::string pathOUT;

	/// wspolczynnik wymierania (0;1)
	double w;

	/// wsplolczynnik rozmnazania (0;1)
	double r;

	///---------------------------------------------------------------------------
	///
	/// Populacja.
	myList<myList<int>> population;
	/// Indeksy osobnikow do rozmnazania (z listy population).
	myList<int> ToReproduction;
	/// najwyzsza wartosc funkcji dostosowania sposrod osobnikow.
	double maxAdjustment;
	/// najnizsza wartosc funkcji dostosowania sposrod osobnikow.
	double minAdjustment;
	/// suma
	double sumAdjustment;
	/// indeks elementu o max wartosci funkcji.
	unsigned int maxindex = 0;
	friend class myList<int>;
	///
	///---------------------------------------------------------------------------
	///
	/// Konstruktor
	/// przyjmuje iteracje, sciezki pliku wejsciowego i wyjsciowego, % osobnikow ktorzy
	/// przystapia do rozmnazania, wspolczynnik wymierania i wsp rozmnazania z zakresu 0-1
	Evolution(unsigned int Iterations = 200,
		std::string path_in = "First Population.txt",
		std::string path_out = "Final Population.txt",
		double reproductionCount_factor = 0.5,
		double death_factor = 0.8,
		double reproduction_Factor = 0.8);
	///
	///---------------------------------------------------------------------------
	///								   Metody
	///---------------------------------------------------------------------------

	/// Start programu.
	void Evolve();

	///	Generuj plik txt z losowymi osobnikami wg powyzszych parametrow.
	void generate_random_population_file();

	/// Funkcja dopasowania. ocenia osobnikow.
	void adjustmentFunction();

	///rownania funkcji dopasowania przyjmuje wartosc genu.
    /// zwraca wartosc funkcji dopasowania.
	double func(int x);    

	///losowanie osobnikow ktorzy przystapia do rozmnazania.
	///algorytm typu Monte Carlo method - Rejection sampling.
	///zwraca false gdy nie udalo sie wyznaczyc osobnika.
	bool RejectionReproductionSelector();

	/// losowanie osobnikow przez ranking.
	void RankReproductionSelector();

	/// rozmnazanie.
	void reproduce();

	/// losowanie osobnikow do eliminacji.
	/// algorytm typu Monte Carlo method - Rejection sampling.
	/// zwraca false gdy nie udalo sie wyznaczyc osobnika.
	bool RejectionDeathSelector();

	/// losowanie osobnikow przez ranking.
	void RankDeathSelector();

	/// Wczytaj populacje z pliku txt do listy population.	
	void read_population_file();

	/// Zapisz populacje do pliku wyjsciowego.
	void write_population_file();
};
