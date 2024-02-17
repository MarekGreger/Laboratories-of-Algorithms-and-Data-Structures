#include <iostream>
#include <chrono>
#include <random>
#include <limits>
#include <algorithm>
#include <string>
#include <iomanip>
#include <fstream>
#include <clocale>

using std::swap;


int losowa_liczba(int min, int max)
{
	static std::default_random_engine gen(std::random_device{}());
	static std::uniform_int_distribution<int> dist;
	return dist(gen, std::uniform_int_distribution<int>::param_type{ min, max });
}

void wypelnij(int* tablica, int rozmiar, int min = 0, int max = std::numeric_limits<int>::max())
{
	for (int i = 0; i < rozmiar; ++i)
		tablica[i] = losowa_liczba(min, max);
}

bool jest_posortowane(int* tablica, int rozmiar)
{
	return std::is_sorted(tablica, tablica + rozmiar);
}

double mierz_czas(int* tablica, int rozmiar, void(*funkcja_sortujaca)(int*, int))
{
	auto start = std::chrono::high_resolution_clock::now();
	funkcja_sortujaca(tablica, rozmiar);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end - start;
	return duration.count();
}

/*

	parametry:
	 - funkcja_sortujaca - wskanik na funkcj¹c¹ sortowanie, musi przyjmowaæ dwa parametry: adres pocz¹tku tablicy (int*) oraz jej rozmiar (int)
	 - nazwa - nazwa testowanej funkcji, tylko w celach wypisania
	 - output - strumien do ktorego beda zapisane wyniki, domyslnie std::cerr, przy ostatnim uruchomieniu warto nadpisac otwartym strumieniem plikowym, aby sobie zebrac wyniki do pliku
	 - dodatkowe_miejsce - liczba dodatkowych elementow tablicy zaalokowanych PRZED poczatkiem tablicy, przykladowo gdy =1, pierwszym indeksem tablicy jest -1, ale dane rozpoczynaja sie od indeksu 0, moze sie przydac do sortowania przez wstawianie z wartownikiem
*/
void eksperyment(void(*funkcja_sortujaca)(int*, int), const std::string& nazwa, std::ostream& output = std::cerr, int dodatkowe_miejsce = 0)
{
	//ustawienia
	const double limit_czasu = 30.0; //sekund
	const int powtorzen = 3;
	const int rozmiar_poczatkowy = 1 << 10;
	/////////////////////////////////////////
	const int szerokosc = 100;
	int gwiazdek = szerokosc - nazwa.length() - 2;
	if (gwiazdek < 0)
		gwiazdek = 0;
	int i = 0;
	output << " ";
	for (; i < gwiazdek / 2; ++i)
		output << '*';
	output << " " << nazwa << " ";
	for (; i < gwiazdek; ++i)
		output << '*';
	output << "\n\n";
	output.flush();

	output << std::setw(9) << "N";
	output << std::setw(1) << "";
	for (int nr = 0; nr < powtorzen; ++nr)
		output << std::setw(9) << nr + 1 << " ";
	output << std::setw(12) << "œredna" << " ";
	output << "\n";
	for (int rozmiar = rozmiar_poczatkowy; ; rozmiar *= 2)
	{
		output << std::setw(9) << rozmiar << ": ";
		output.flush();
		int* pamiec = new int[dodatkowe_miejsce + rozmiar];
		int* tablica = pamiec + dodatkowe_miejsce;
		double czas = 0.0;

		int* pattern = new int[rozmiar];


		for (int nr = 0; nr < powtorzen; ++nr)
		{
			wypelnij(tablica, rozmiar);
			for (int i = 0; i < rozmiar; ++i)
				pattern[i] = tablica[i];
			std::sort(pattern, pattern + rozmiar);
			double c = mierz_czas(tablica, rozmiar, funkcja_sortujaca);
			if (!jest_posortowane(tablica, rozmiar))
			{
				output << "Tablica nieposortowana!!\n";
				if (&output != &std::cerr)
					std::cerr << "Tablica nieposortowana!!\n";
				return;
			}
			if (!std::equal(pattern, pattern + rozmiar, tablica, tablica + rozmiar))
			{
				output << "Tablica zawiera inne wartosci niz powinna!!\n";
				if (&output != &std::cerr)
					std::cerr << "Tablica zawiera inne wartosci niz powinna!!\n";
				return;
			}
			czas += c;
			output.precision(6);
			output << std::fixed << c << "  ";
			output.flush();
		}
		czas /= powtorzen;
		output << std::setw(12) << std::fixed << czas << "\n";
		output.flush();
		delete[] pamiec;
		delete[] pattern;
		if (czas > limit_czasu)
			break;
	}
	output << "\n";
	output.flush();
}




void sortowanie_babelkowe(int* tablica, int rozmiar)
{
	int pomoc;
	for (int i = 0; i < rozmiar; i++)
	{
		for (int j = 1; j < rozmiar; j++)
		{
			if (tablica[j - 1] > tablica[j])
			{
				pomoc = tablica[j - 1];
				tablica[j - 1] = tablica[j];
				tablica[j] = pomoc;
			}
		}
	}
}

void sortowanie_wybieranie(int* tablica, int rozmiar)
{
	int k;
	int x;
	for (int i = 0;i < rozmiar - 1;i++)
	{
		k = i;
		x = tablica[i];
		for (int j = i + 1;j < rozmiar;j++)
		{
			if (tablica[j] < x)
			{
				k = j;
				x = tablica[j];
			}
		}
		tablica[k] = tablica[i];
		tablica[i] = x;

	}
}

void sortowanie_wstawianie(int* tablica, int rozmiar)
{
	int x;
	int j;
	for (int i = 1;i < rozmiar;i++)
	{
		x = tablica[i];
		j = i - 1;
		while (j >= 0 && x < tablica[j])
		{
			tablica[j + 1] = tablica[j];
			j = j - 1;
		}
		tablica[j + 1] = x;
	}
}

void sortowanie_shella(int* tablica, int rozmiar)
{
	int h = 1;
	while (h < (rozmiar / 9))
	{
		h = 3 * h + 1;
	}
	while (h > 0)
	{
		for (int i = h; i < rozmiar; i++)

		{
			int x = tablica[i]; int j = i;
			while ((j >= h) && (x < tablica[j - h]))
			{
				tablica[j] = tablica[j - h]; j = j - h;
			}
			tablica[j] = x;
		}
		h = h / 3;
	}
}



int main()
{
	setlocale(LC_ALL, "");
	std::ofstream wyniki("wyniki_release.txt");

	std::ostream& output = std::cerr; //zmienic na = wyniki aby zapisywalo do pliku
	eksperyment(sortowanie_babelkowe, "Sortowanie b¹belkowe", output);
	eksperyment(sortowanie_wstawianie, "Sortowanie wstawianie", output);
	eksperyment(sortowanie_wybieranie, "Sortowanie wybieranie", output);
	eksperyment(sortowanie_shella, "Sortowanie shella", output);
	return 0;
}
