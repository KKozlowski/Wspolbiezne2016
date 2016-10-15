#include <windows.h>
#include <cstdio>
#include <time.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <iterator>

#define SIZE_OF_RANDOM 12

using namespace std;

HANDLE ONE;
HANDLE TWO;

HANDLE SEMA;

int m = 0;

HANDLE main_sorting_thread;

vector<vector<double> *> the_matrix;
HANDLE matrix_semaphore;

int number_of_sorted = 0;
HANDLE nos_semaphore;

int ongoing_merges = 0;
HANDLE nmerges_semaphore;

void rand_matrix(int size)
{
	for (int i = 0;i < size;i++)
	{
		the_matrix.push_back(new vector<double>());
		for (int k = 0;k < size;k++)
		{
			the_matrix[i]->push_back(rand() * 0.01f);
		}
	}
}

void in_matrix()
{
	char method;
	cin >> method;
	int size;
	cin >> size;
	double probe;

	if (size <= 0) return;

	if (method == 'r')
		rand_matrix(size);
	else
		for (int i = 0;i < size;i++)
		{
			the_matrix.push_back(new vector<double>());
			for (int k = 0;k < size;k++)
			{
				cin >> probe;
				the_matrix[i]->push_back(probe);
			}
		}
}

void print_matrix(vector<vector<double> *> &m)
{
	for (int i = 0;i < the_matrix.size(); i++)
	{
		for (int k = 0;k < the_matrix[i]->size();k++)
		{
			printf("%.2lf ",  the_matrix[i]->at(k));
		}
		printf("\n");
	}
	printf("\n");
}

DWORD WINAPI One(LPVOID)
{
	while (1)
	{
		WaitForSingleObject(SEMA, INFINITE);
		Sleep((rand() % 10) * 150 + 100);
		printf("%s\n", "THREAD ONE");
		++m;
	};
	return 0;
};

DWORD WINAPI Two(LPVOID)
{
	while (1)
	{
		Sleep((rand() % 10) * 150 + 400);
		printf("%s, m equals %d\n", "THREAD TWO", m);
		ReleaseSemaphore(SEMA, 1, NULL);
	};
	return 0;
};



DWORD WINAPI SortRow(LPVOID index)
{
	int id = *static_cast<int*>(index);
	vector<double> *row = nullptr;
	
	WaitForSingleObject(matrix_semaphore, INFINITE);
		row = the_matrix[id];
	ReleaseSemaphore(matrix_semaphore, 1, nullptr);

	sort(row->begin(), row->end(), less_equal<double>());

	WaitForSingleObject(nos_semaphore, INFINITE);
		++number_of_sorted;
	ReleaseSemaphore(nos_semaphore, 1, nullptr);

	return 0;
};

DWORD WINAPI MergeRows(LPVOID)
{
	vector<double> *one = nullptr;
	vector<double> *two = nullptr;
	bool success = false;
	WaitForSingleObject(matrix_semaphore, INFINITE);
		//Get two threads to merge
		if (the_matrix.size() >= 2)
		{
			one = the_matrix[0];
			two = the_matrix[1];

			WaitForSingleObject(nmerges_semaphore, INFINITE);
				++ongoing_merges;
			ReleaseSemaphore(nmerges_semaphore, 1, nullptr);

			the_matrix.erase(the_matrix.begin(), the_matrix.begin() + 2);

			//Start new thread if it will have any use
			if (the_matrix.size() >= 2)
			{
				CreateThread(NULL, 0, MergeRows, nullptr, 0, nullptr);
			}

			success = true;
		}
	ReleaseSemaphore(matrix_semaphore, 1, nullptr);

	if (!success)
		return 1;

	vector<double> * result = new vector<double>();

	std::merge(one->begin(), one->end(), two->begin(), two->end(), std::back_inserter(*result));

	//Finish
	WaitForSingleObject(matrix_semaphore, INFINITE);
		the_matrix.push_back(result);
		WaitForSingleObject(nmerges_semaphore, INFINITE);
			--ongoing_merges;
			if (the_matrix.size() >= 2)
			{
				CreateThread(NULL, 0, MergeRows, nullptr, 0, nullptr);
			}
		ReleaseSemaphore(nmerges_semaphore, 1, nullptr);
	ReleaseSemaphore(matrix_semaphore, 1, nullptr);

	return 0;
};

DWORD WINAPI SortMatrix(LPVOID)
{
	print_matrix(the_matrix);

	//Sorting lines
	{
		WaitForSingleObject(matrix_semaphore, INFINITE);
			int matrix_size = the_matrix.size();
			for (int i = 0; i < matrix_size; i++)
			{
				CreateThread(NULL, 0, SortRow, new int(i), 0, nullptr);
			}
		ReleaseSemaphore(matrix_semaphore, 1, nullptr);

		while (1)
		{
			WaitForSingleObject(nos_semaphore, INFINITE);
				int nos_now = number_of_sorted;
			ReleaseSemaphore(nos_semaphore, 1, nullptr);

			if (nos_now == matrix_size)
			{
				print_matrix(the_matrix);
				break;
			}
		}
	}

	//Merging lines
	{
		int matrix_size = -111;
		int merges = - 9999;
		CreateThread(NULL, 0, MergeRows, nullptr, 0, nullptr);
		while (1)
		{
			WaitForSingleObject(matrix_semaphore, INFINITE);
				WaitForSingleObject(nmerges_semaphore, INFINITE);
					merges = ongoing_merges;
				ReleaseSemaphore(nmerges_semaphore, 1, nullptr);
				matrix_size = the_matrix.size();
				if (merges == 0 && matrix_size == 1)
				{
					print_matrix(the_matrix);
					break;
				}
			ReleaseSemaphore(matrix_semaphore, 1, nullptr);
			
		}
		
	}
	return 0;
}

void finalThings()
{
	vector<double> *finalRow = the_matrix[0];
	int rowSize = finalRow->size();

	double mediana = 0;

	if (finalRow->size() % 2 == 0)
	{
		mediana = (finalRow->at(rowSize / 2) + finalRow->at(rowSize / 2 -1)) / 2;
	} else
	{
		mediana = finalRow->at((rowSize-1) / 2);
	}

	printf("MEDIANA: %.2lf", mediana);
}

int main()
{
	srand(time(0));
	in_matrix();

	if (the_matrix.size() == 0)
	{
		printf("WRONG DATA");
		return 0;
	}
		

	if (the_matrix.size() == 1)
	{
		print_matrix(the_matrix);
		printf("MEDIANA: %.2lf", the_matrix[0]->at(0));
		return 0;
	}
		

	matrix_semaphore = CreateSemaphore(NULL, 1, 1, NULL);
	nos_semaphore = CreateSemaphore(NULL, 1, 1, NULL);
	nmerges_semaphore = CreateSemaphore(NULL, 1, 1, NULL);

	DWORD ID;
	main_sorting_thread = CreateThread(NULL, 0, SortMatrix, 0, 0, &ID);

	while(true)
	{
		DWORD code;
		GetExitCodeThread(main_sorting_thread, &code);
		if (code != STILL_ACTIVE)
		{
			break;
		}
	}

	finalThings();

	return 0;
}