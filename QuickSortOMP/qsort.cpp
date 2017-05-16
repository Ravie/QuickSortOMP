#include <iostream>
#include <ctime>
#include <omp.h>
using namespace std;

void copy_array(int* source, int *destination, const long length)
{
	for (int n = 0; n < length; n++)
		destination[n] = source[n];
}

void par_quickSort(int* a, const long n)
{
	long i = 0, j = n;
	int pivot = a[n / 2]; // опорный элемент
	do {
		while (a[i] < pivot) i++;
		while (a[j] > pivot) j--;
		if (i <= j)
		{
			std::swap(a[i], a[j]);
			i++; j--;
		}
	} while (i <= j);
	if (n < 100)
	{ // если размер массива меньше 100
	  // сортировка выполняется в текущем потоке
		if (j > 0)
			par_quickSort(a, j);
		if (n > i)
			par_quickSort(a + i, n - i);
		return;
	}
	//#pragma omp task shared(a)
#pragma omp parallel sections
	{
#pragma omp section
		if (j > 0)
			par_quickSort(a, j);
	//#pragma omp task shared(a)
#pragma omp section
		if (n > i)
			par_quickSort(a + i, n - i);
	}
	//#pragma omp taskwait
}

void quickSort(int* a, const long n)
{
	long i = 0, j = n;
	int pivot = a[n / 2]; // выбор опорного элемента
	do {
		while (a[i] < pivot) i++;
		while (a[j] > pivot) j--;
		if (i <= j)
		{
			std::swap(a[i], a[j]);
			i++; j--;
		}
	} while (i <= j);
	if (j > 0)
		quickSort(a, j);
	if (n > i)
		quickSort(a + i, n - i);
}

int check_sort(int* a, const long length)
{
	for (int n = 0; n < length - 1; n++)
		if (a[n]>a[n + 1])
			return 0;
	return 1;
}

int main()
{
	// Генерация массива
	int MAX_NUMBER, MAX_SIZE, num_of_threads, current_threads;
	double start_time, end_time, start_seq, end_seq, start_par, end_par;
	cout << "Enter number of threads: ";
	cin >> num_of_threads;
	cout << "Size of array: ";
	cin >> MAX_SIZE;
	cout << "Max element in array: ";
	cin >> MAX_NUMBER;
	srand((int)time(0));
	int *Array = new int[MAX_SIZE];
	for (int thread = 1; thread <= num_of_threads; thread += num_of_threads - 1)
	{
		start_time = omp_get_wtime();
#pragma omp parallel num_threads(thread)
		{
#pragma omp for
			for (int n = 0; n < MAX_SIZE; n++)
				Array[n] = rand() % MAX_NUMBER;
			current_threads = omp_get_num_threads();
		}
		end_time = omp_get_wtime();

		cout << "Random generation with " << current_threads << " thread(s): " << end_time - start_time << endl;
	}
	int *copy_of_array = new int[MAX_SIZE];
	copy_array(Array, copy_of_array, MAX_SIZE);

	start_par = omp_get_wtime();
	par_quickSort(Array, MAX_SIZE - 1);
	end_par = omp_get_wtime();
	cout << "Parallel sorting with " << current_threads << " thread(s): " << end_par - start_par << endl;

	start_seq = omp_get_wtime();
	quickSort(copy_of_array, MAX_SIZE - 1);
	end_seq = omp_get_wtime();
	cout << "Sequence sorting: " << end_seq - start_seq << endl;

	cout << " x" << (end_seq - start_seq) / (end_par - start_par) << " Acceleration" << endl;

	if (check_sort(Array, MAX_SIZE - 1))
		cout << "Array is sorted" << endl;
	else
		cout << "Array is unsorted" << endl;

	delete[] Array;
	delete[] copy_of_array;

	return 0;
}
