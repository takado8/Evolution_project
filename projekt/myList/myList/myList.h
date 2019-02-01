#pragma once
#include "pch.h"
#include "Evolution.h"
#include <iostream>
#include <string>
#include <random>

template <typename T>
class myList
{
private:
	friend class Evolution;
	struct Element
	{
		///Wartosc elementu.
		T value;
		///Wskaznik na nastepny element.
		Element *wNext = nullptr;
		///Pusty konstruktor.
		Element();
		///Konstruktor z waroscia elementu.
		Element(T &Val);
	};
	///Wskaznik pierwszego elementu listy.
	Element *wFirst = nullptr;
	///Wskaznik ostatniego elementu listy.
	Element *wLast = nullptr;
	///Ilosc elementow w liscie.
	unsigned int Count = 0;
	///przechowuje wartosc przystosowania 'osobnika'.
	double adjustment;

public:
	/// przeciazony operator []
	T& operator[](unsigned int index);

	/// przeciazony operator =
	myList<T>& operator=(myList &m1);

	///Domyslny konstruktor.
	myList();

	/// Konstruktor kopiujacy
	myList(myList &original);

	///Konstruktor listy z pierwszym elementem.
	myList(T &value);

	///Konstruktor listy z tablicy.
	myList(T *arr, int n);

	///Destruktor listy.
	~myList();

	///Getter wartosci int Count.
	int count();

	///Dodaj do listy przez referencje.
	void Add(T &val);

	///Wyciagnij wskaznik na element o indexie.
	T* Pull(int index);

	///Usun element o indexie.
	void RemoveAt(int index);

	///Usun wszystkie elementy z listy.
	void clean();

	///Wypisz w konsoli cala liste.
	void Print();

	/// rekombinacja osobnikow (na potrzeby klacy Evolution)
	void CrossingOver(myList &m1);

	/// losowy int z przedzialu min ; max.
	int get_r(int min, int max);
};

///============================================================================
///								 Konstruktory
///============================================================================

///----------------------------------------------------------------------------
///								Struct Element
///----------------------------------------------------------------------------

/// Domyslny konstruktor.
template <typename T>
myList<T>::Element::Element()
{

}

/// Z wartoscia elementu przekazana przez referencje.
template <typename T>
myList<T>::Element::Element(T &_Val)
{
	value = _Val;
}

///----------------------------------------------------------------------------
///								 class myList
///----------------------------------------------------------------------------

/// Domyslny konstruktor.
template <typename T>
myList<T>::myList()
{

}

/// Kopiujacy konstruktor.
template <typename T>
myList<T>::myList(myList &original)
{
	for (int i = 0; i < original.count(); i++)
	{
		Element *temp = new Element(original[i]);
		if (Count == 0)
		{
			wFirst = wLast = temp;
		}
		else
		{
			wLast->wNext = temp;
			wLast = temp;
		}
		Count++;
	}
}

/// Z pierwszym elementem listy, przekazanym przez referencje.
template <typename T>
myList<T>::myList(T &value)
{
	Add(value);
}

/// Z tablicy przekazanej przez wskaznik.
template <typename T>
myList<T>::myList(T *arr, int n)
{
	for (int i = 0; i < n; i++)
	{
		Add(arr[i]);
	}
}
///============================================================================
///						 Destruktory i przeciazone operatory
///============================================================================

/// myList
template <typename T>
myList<T>::~myList()
{
	while (Count > 0)
	{
		RemoveAt(0);
	}
}

/// operator []
template <typename T>
T& myList<T>::operator[](unsigned int index)
{
	return *Pull(index);
}
/// operator =
template <typename T>
myList<T>& myList<T>::operator=(myList &m1)
{
	if (this == &m1)
	{
		return *this;
	}
	else
	{
		clean();
		for (int i = 0; i < m1.count(); i++)
		{
			Element *temp = new Element(m1[i]);
			if (Count == 0)
			{
				wFirst = wLast = temp;
			}
			else
			{
				wLast->wNext = temp;
				wLast = temp;
			}
			Count++;
		}
		return *this;
	}
}
///============================================================================
///								    Metody
///============================================================================

///----------------------------------------------------------------------------
///							        myList
///----------------------------------------------------------------------------

/// Getter warosci int Count.
template <typename T>
int myList<T>::count()
{
	return Count;
}
/// Dodaj przez referencje.
template <typename T>
void myList<T>::Add(T &val)
{
	Element *temp = new Element(val);
	if (Count == 0)
	{
		wFirst = wLast = temp;
	}
	else
	{
		wLast->wNext = temp;
		wLast = temp;
	}
	Count++;
}

/// Zwraca wskaznik na wartosc o indexie.
template <typename T>
T* myList<T>::Pull(int index)
{
	if (index >= Count || index < 0)
	{
		std::cerr << "Index Out of range exception in myList::Pull(int)\n";
		std::cerr << "index = " << index << std::endl;
		std::cerr << "Count = " << Count << std::endl;
		throw std::out_of_range("Index Out of range exception in myList::Pull(int)");
	}
	else
	{
		Element *temp = wFirst;
		for (int i = 0; i < index; i++)
		{
			temp = temp->wNext;
		}
		return &temp->value;
	}
}

/// Usun element o indexie.
template <typename T>
void myList<T>::RemoveAt(int index)
{
	if (index < 0 || index >= Count)
	{
		throw std::out_of_range("Index out of range in myList::RemoveAt(int)");
	}
	Element *temp;
	if (index == 0) // pierwszy element.
	{
		temp = wFirst;
		wFirst = wFirst->wNext;
		delete temp;
	}
	else // pozostale elementy.
	{
		Element *prev = nullptr;
		temp = wFirst;
		for (int i = 0; i < index; i++)
		{
			prev = temp;
			temp = temp->wNext;
		}
		prev->wNext = temp->wNext;
		delete temp;
	}
	Count--;
}
/// Wyczysc liste.
template <typename T>
void myList<T>::clean()
{
	while (Count > 0)
	{
		RemoveAt(0);
	}
	wFirst = nullptr;
	wLast = nullptr;
	adjustment = 0;
}

/// Wypisz wszystkie wartosci listy w konsoli.
template <typename T>
void myList<T>::Print()
{
	if (Count == 0)
	{
		std::cout << "\nList is empty.\n";
	}
	else
	{
		Element *temp = wFirst;
		std::cout << "\n[";
		for (int i = 0; i < Count; i++)
		{
			std::cout << temp->value;
			if (i < Count - 1) std::cout << ", ";
			temp = temp->wNext;
		}
		std::cout << "]\n";
	}
}

/// Losowy int z przedzialu min, max.
template <typename T>
int myList<T>::get_r(int min, int max)
{
	static std::default_random_engine generator(std::random_device{}());
	std::uniform_real_distribution<double> distribution(min, max);
	return distribution(generator);
}

/// Wymiana fragmentow 2 list. na potrzeby klasy Evolution.
template <typename T>
void myList<T>::CrossingOver(myList &m1)
{
	//  [A,A,B,B]  wejscie
	//  [C,C,D,D]
	//
	//  [A,A,D,D]  wyjscie
	//  [B,B,C,C]

	if (Count < 2) Add(wFirst->value);
	if (m1.Count < 2) m1.Add(m1.wFirst->value);

	Element *temp = wFirst;
	Element *temp2 = m1.wFirst;
	Element *ptr;
	int h1 = get_r(0, Count - 2);
	int h2 = get_r(0, m1.Count - 2);

	for (int i = 0; i < h1; i++)
	{
		temp = temp->wNext;
	}
	for (int i = 0; i < h2; i++)
	{
		temp2 = temp2->wNext;
	}
	ptr = temp->wNext;
	temp->wNext = temp2->wNext;
	wLast->wNext = m1.wFirst;
	wLast = m1.wLast;
	m1.wFirst = ptr;
	m1.wLast = temp2;

	int oldCount = Count;
	Count = h1 + m1.Count - h2;
	m1.Count = h2 + oldCount - h1;

	//mutacja 0,1 % szansy
	if (get_r(0, 1000) == 7)
	{
		int rand = get_r(Evolution::min_gen_value, Evolution::max_gen_value);
		RemoveAt(get_r(0, count() - 1));
		Add(rand);
		m1.RemoveAt(get_r(0, m1.count() - 1));
		m1.Add(rand);
	}
}
