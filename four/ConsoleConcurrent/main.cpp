#include <windows.h>
#include <cstdio>
#include <time.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <sstream>
#include <iterator>
#include <random>
#include <string>
#include "lock.h"

#define PRINT_TIME TRUE

using namespace std;

HANDLE *forks;
lock *l;

int intRand(const int& min, const int& max)
{
	static thread_local std::mt19937 generator;
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(generator);
}

class philosofer_params
{
public:
	int id;
	int meals_to_eat;
	int count;

	philosofer_params(int a, int meals, int count)
	{
		id = a;
		meals_to_eat = meals;
		this->count = count;
	}
};

DWORD WINAPI philosopher(LPVOID p)
{
	philosofer_params *params = static_cast<philosofer_params*>(p);
	uint32_t eaten = 0;
	uint32_t to_eat = params->meals_to_eat;
	int id = params->id;
	l->slock(id);
	
	int fork1 = id;
	int fork2 = (id + 1) % (params->count + 1);
	if (fork2 == 0)
	{
		fork2 = fork1;
		fork1 = 1;
	}

	printf("Filozof %d siada przy stole. Bedzie potrzebowal widelcow %d i %d.\n", id, fork1, fork2);

	while(eaten < to_eat)
	{
		//HUNGER
		if ((to_eat == 2 && eaten == 1) || (eaten == to_eat / 2 + 1))
		{
			printf("Filozof %d zjadl juz polowe posilkow i czeka, az reszta go dogoni.\n", id);
			l->sunlock(id);
			while (!l->xlock(id)){}
			l->xunlock(id);
			printf("Filozof %d uznal, ze moze jesc dalej.\n", id);
		}

		//PICKING
		
		WaitForSingleObject(forks[fork1], INFINITE);
		printf("Filozof %d podniosl widelec %d.\n", id, fork1);
		WaitForSingleObject(forks[fork2], INFINITE);
		int sleepingtime = 1000 + intRand(0, 1000-id);
		++eaten;
		printf("Filozof %d podniosl widelec %d i zaczyna posilek nr %d, ktory potrwa %.3f s.\n", id, fork2, eaten, sleepingtime/1000.f);
		

		//EATING
		Sleep(sleepingtime);

		printf("Filozof %d skonczyl jesc posilek nr %d.\n", id, eaten);

		//RELEASING
		printf("Filozof %d odlozyl widelec %d.\n", id, fork2);
		ReleaseSemaphore(forks[fork2], 1, nullptr);
		printf("Filozof %d odlozyl widelec %d.\n", id, fork1);
		ReleaseSemaphore(forks[fork1], 1, nullptr);

		//THINKING
		int thinkingtime = 2000 + intRand(0, 1000 - id);
		printf("Filozof %d zaczal rozmyslac, na %.3f s.\n", id, thinkingtime/1000.f);
		Sleep(3000);
		printf("Filozof %d skonczyl rozmyslac.\n", id);
	}


	return 0;
}

int main(int argc, char** argv)
{

	int philo_count = 5;
	int meals_count = 5;
	
	if (argc > 1)
	{
		try
		{
			int argumentOfCount = stoi(string(argv[1]));
			if (argumentOfCount > 1 && argumentOfCount < 100)
			{
				philo_count = argumentOfCount;
			}
			else
				printf("Invalid number of philosophers. Setting default of %d.\n", philo_count);
		} catch (std::invalid_argument i)
		{
			printf("Invalid number of philosophers. Setting default of %d.\n", philo_count);
		}
	}

	printf("Number of philosophers: %d.\n", philo_count);

	if (argc > 2)
	{
		try
		{
			int argumentOfCount = stoi(string(argv[2]));
			if (argumentOfCount > 1 && argumentOfCount < 100)
			{
				meals_count = argumentOfCount;
			}
			else
				printf("Invalid number of meals. Setting default of %d.\n", meals_count);
		}
		catch (std::invalid_argument i)
		{
			printf("Invalid number of meals. Setting default of %d.\n", meals_count);
		}
	}

	printf("Number of meals: %d.\n", meals_count);

	forks = new HANDLE[philo_count + 1];
	for (int i = 1; i <= philo_count; i++)
	{
		forks[i] = CreateSemaphore(nullptr, 1, 1, nullptr);
	}
	l = new lock();

	for (int i = 1; i <= philo_count; i++)
	{
		CreateThread(NULL, 0, philosopher, new philosofer_params(i, meals_count, philo_count), 0, nullptr);
	}

	clock_t t_start = clock();

	bool time_shown = false;
	while(true)
	{

	}

	return 0;
}