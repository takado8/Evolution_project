#include "pch.h"
#include "Evolution.h"
#include <random>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

///============================================================================
///								 Metody
///============================================================================

/// Losowy int z przedzialu min, max.
int get_random(int min, int max)
{
	static default_random_engine generator(random_device{}());
	uniform_real_distribution<double> distribution(min, max);
	return distribution(generator);
}

///----------------------------------------------------------------------------
///							class Evolution
///----------------------------------------------------------------------------

/// Generuje plik txt z losowa populacja.
void Evolution::generate_random_population_file()
{
	// Otworz plik / stworz jesli nie istnieje
	ofstream outfile(pathIN);
	//liczba osobnikow
	int j = population_size;
	int n;
	for (; j > 0; j--)
	{
		//liczba genów (dlugosc chromosomu / osobnika)
		if (chromosome_length == 0) // jesli ustawiona na 0, opcja losowa
		{									// z ustalonego w parametrach zakresu.
			n = get_random(chr_min, chr_max);
		}
		else
		{
			n = chromosome_length;
		}
		for (; n > 0; n--)
		{
			outfile << get_random(min_gen_value, max_gen_value) << " ";
		}
		// koniec chromosomu.
		outfile << endl;
	}
	outfile.close();
}
/// Wczytaj populacje z pliku.
void Evolution::read_population_file()
{
	ifstream file;
	file.open(pathIN, ios::in);

	if (file.good())
	{
		myList<int> m1;
		string line;
		while (getline(file, line))
		{
			istringstream strstrm(line);
			int number;
			m1.clean();
			while (strstrm >> number)
			{
				m1.Add(number);
			}
			population.Add(m1);
		}
		file.close();
	}
	else
	{
		cerr << "file not good\n";
		throw runtime_error("File not good.");
	}
}
/// Funkcja dopasowania. Obliczana jest wartosc funkcji dla kazdego genu,
/// nastepnie wyciagana jest srednia i zwracany jest jej szescian.
/// 
double Evolution::func(int x)
{
	switch (func_nr)
	{
	case 1: return -0.040625 * (x - 10) * (x - 90); break; // max x = 50
	case 2: return -0.0006205 * x * (x - 950); break; // max x = 475

	default: cerr << "Error: funkcja func_nr = " << func_nr << " nie istnieje. sprawdz dostepne funkcje\n"
		<< "potwierdz >>[ENTER]<<\n";
		getchar();
		throw invalid_argument("funkcja dopasowania nie istnieje. sprawdz dostepne funkcje");
		break;
	}
}

void Evolution::adjustmentFunction()
{
	
	maxAdjustment = -DBL_MAX;
	minAdjustment = DBL_MAX;
	double average;
	myList<myList<int>>::Element *temp = population.wFirst; // wsk. na osobnika /chromosom
	myList<int>::Element *t; // wskaznik na kolejne geny chromosomu

	for (int i = 0; i < population.count(); i++)
	{
		average = 0;
		t = temp->value.wFirst; // pierwszy gen chromosomu
		for (int k = 0; k < temp->value.count(); k++)
		{
			average += func(t->value); // ocena przez funkcje dopasowania.
			t = t->wNext; // przeskok na nastepny gen
		}												// srednia z powiekszonym dzielnikiem
		average = average / (temp->value.count() * 2); // w celu faworyzacji krotszych chromosomow.
		average = average * average * average; // szescian dla zwiekszenia roznicy miedzy elementami.   
		temp->value.adjustment = average;
		sumAdjustment += average;
		if (average > maxAdjustment) // sprawdzenie czy jest nowy max albo min.
		{
			maxAdjustment = average;
			maxindex = i;
		}
		if (average < minAdjustment)
		{
			minAdjustment = average;
		}
		temp = temp->wNext; // nastepny osobnik / chromosom
	}
}
/// Losowanie osobnikow do reprodukcji.
/// algorytm typu Monte Carlo method, Rejection sampling.
/// zwraca false, jesli nie udalo sie wylosowac osobnika przez okreslona ilosc iteracji.
int bl = 0;
int blRange = 5000;
bool Evolution::RejectionReproductionSelector()
{
	//losuj kandydata
	int n = population.count();
	int k = 0;
	int i = 0;
	int breakloop = 0;
	while (i < n*reproductionFactor)
	{
		k = get_random(0, n - 1); // losowy osobnik
		if (population[k].adjustment > get_random(minAdjustment, maxAdjustment))
		{							// jesli przystosowanie wieksze od losowej wartosci
			ToReproduction.Add(k);  // z przedzialu od min przystosowania do max to akceptuj,
			i++;					// jesli nie, odrzuc i probuj znowu.
		}
		else if (breakloop++ > blRange) // jesli odrzucono blRange razy (domyslnie 5000)
		{								// przerwij petle i zwroc false. wybor zostanie 
			if (bl++ > 10)				// dokonany przez selektor rankingowy z innej funkcji.
			{							// jesli przerwano 10 razy, zmniejsz liczbe odrzucen
				if (blRange > 1000)     // do przerwania petli o 1tys. //w celu przyspieszenia.
				{
					blRange -= 1000;
				}
				bl = 0;
			}
			return false;
		}
	}
	return true;
}
void Evolution::RankReproductionSelector()
{
	double average = sumAdjustment / population.count();
	int n = 0, i = 0, k = 0;
	while (n < population.count() * reproductionFactor)
	{
		//losujemy osobnika
		i = get_random(0, population.count() - 1);
		if (population[i].adjustment >= average) // jesli dostosowanie powyzej sredniej
		{
			if (get_random(0, 9) < (int)(r*10)) // szansa na rozmnazanie wg wsp. rozmnazania.
			{
				ToReproduction.Add(i);
				n++;
			}
		}
		else									// jesli ponizej sredniej
		{
			if (get_random(0, 9) < 10 - ((int)(r*10))) // prawdopodobienstwo przeciwne.(1 - r)
			{
				ToReproduction.Add(i);
				n++;
			}
		}
	}
}

/// Rozmnazanie.
///
void Evolution::reproduce()
{
	for (int i = 0; i < ToReproduction.count() - 1; i += 2)
	{
		auto r1 = population[ToReproduction[i]];
		auto r2 = population[ToReproduction[i + 1]];
		r1.CrossingOver(r2);
		population.Add(r1);
		population.Add(r2);
	}
}
/// Losowanie osobnikow do eliminacji.
/// to samo co w RejectionReproductionSelector() tylko odwrotnie.
int bl2 = 0;
int bl2Range = 10000;
bool Evolution::RejectionDeathSelector()
{   
	int n = population.count();
	int k = 0;
	int i = 0;
	int breakLoop = 0;
	while (i < ToReproduction.count())
	{
		k = get_random(0, n - 1);
		if (population[k].adjustment <= get_random(minAdjustment-1, maxAdjustment+1))
		{
			population.RemoveAt(k);
			n = population.count();
			i++;
		}
		else
		{
			if (breakLoop++ > bl2Range)
			{
				if (bl2++ > 10)
				{
					if (bl2Range > 1000)
					{
						bl2Range -= 1000;
					}
					bl2 = 0;
				}
				//cout << "endles DeathSelector loop....\n";
				return false;
			}
		}
	}
	return true;
}
void Evolution::RankDeathSelector()
{
	double average = sumAdjustment / population.count();
	int n = 0, i = 0;
	while (population.count() > population_size)
	{
		//losujemy osobnika
		i = get_random(0, population.count() - 1);
		if (population[i].adjustment < average) //jesli dostosowanie ponizej sredniej
		{
			if (get_random(0, 9) < (int)(r * 10)) // szansa na smierc wg wsp. wymierania.
			{
				population.RemoveAt(i);
			}
		}
		else    // jesli dostosowanie powyzej sredniej
		{
			if (get_random(0, 9) < 10 - ((int)(r * 10))) // prawdopodobienstwo przeciwne.(1 - r)
			{
				population.RemoveAt(i);
			}
		}
	}
}
/// start programu. 
void Evolution::Evolve()
{
	generate_random_population_file();

	read_population_file();
	if (population.count() < 2 / reproductionFactor)
	{
		cerr << "\nminimalna wielkosc populacji dla obecnego wspolczynnika rozmnazania -k wynosi " << 2 / reproductionFactor << endl
			<< "nacisnij enter aby zakonczyc program.\n";
		getchar();
		return;
	}
	for (int i = 0; i < iterations; i++)
	{
		system("cls");
		cout << "i: " << i + 1 << "/" << iterations << endl;
		cout << "min adjustment: " << minAdjustment << endl;
		cout << "max adjustment: " << maxAdjustment << endl;

		adjustmentFunction();
		population[maxindex].Print();

		if (!RejectionReproductionSelector())// czasem przy zbyt malym lub zbyt duzym
		{									 // zroznicowaniu osobnikow, wybieranie przez
			RankReproductionSelector();      // odrzucanie jest drogie, wiec przelaczamy
		}									 // na selektor rankingowy, ktory jest bardziej ogolny.
		reproduce();						 
		if (!RejectionDeathSelector())
		{
			RankDeathSelector();
		}
		ToReproduction.clean();
	}
	write_population_file(); // zapisujemy populacje do pliku.
	
	cout << "\nkoniec programu wcisnij enter...\n";
	getchar();
}

void Evolution::write_population_file()
{
	// Otworz plik / stwórz jesli nie istnieje
	ofstream outfile(pathOUT);

	for (int i = 0; i < population.count(); i++)
	{
		for (int k = 0; k < population[i].count(); k++)
		{
			outfile << population[i][k] << " ";
		}
		outfile << endl;
	}
}
/// konstruktor z przekazanymi parametrami. lub domyslnymi.
Evolution::Evolution(unsigned int p, string path_i, string path_o, double k, double _w, double _r)
{
	pathIN = path_i;
	pathOUT = path_o;
	iterations = p;
	reproductionFactor = k; // ilosciowy wsp rozmnazania (ile % populacji przystapi)
	w = _w; // wspolczynnik wymierania
	r = _r; // wsp rozmnazania (% szans)
}