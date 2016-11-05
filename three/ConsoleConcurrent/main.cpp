#include <windows.h>
#include <cstdio>
#include <time.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <sstream>
#include <iterator>
#include "Crossing.h"
#include <random>
#include <string>

#define PRINT_TIME TRUE

using namespace std;

Crossing *c;

HANDLE mayor;

static const char * LightsStrings[] = { "Queue only", "Time only", "Time and queue" };

HANDLE time_sema;
double max_time = 0;
double min_time = 10000;
double time_sum = 0;
double time_sum_divider = 0;



class car_params
{
public:
	int id;
	bool from_left;

	car_params(int a, bool b)
	{
		id = a;
		from_left = b;
	}
};



DWORD WINAPI car(LPVOID p)
{
	car_params *params = static_cast<car_params*>(p);
	int id = params->id;
	bool from_left = params->from_left;

	uint64_t waiting_id = 0;
	while(true)
	{
		waiting_id = from_left ? c->WaitFromLeft() : c->WaitFromTop();
		printf("Car nr %d waits before crossing from %s.\n", id, from_left ? "west" : "north");
		clock_t tStart = clock();

		uint64_t allowed_id = 0;
		do
		{
			allowed_id = from_left ? c->GetNextFromLeft() : c->GetNextFromTop();
		} while (allowed_id != waiting_id);


		//Crossing entering
		printf("Car nr %d enters the crossing from %s.\n", id, from_left ? "west" : "north");
		Sleep(1000); 

		//Crossing exiting
		double time = secondsFrom(tStart);
		printf("Car nr %d exits the crossing after %.2lf s.\n", id, time);
		from_left ? c->SignalFromLeft() : c->SignalFromTop();

#if PRINT_TIME
		WaitForSingleObject(time_sema, INFINITE);
		if (max_time < time) max_time = time;
		if (min_time > time) min_time = time;
		time_sum += time;
		time_sum_divider += 1;
		ReleaseSemaphore(time_sema, 1, nullptr);
#endif
		//Going around
		Sleep(2400 + intRand(600, 2000 - id)); // id is only used for making distribution unique, to give different random number sequences.
		from_left = !from_left;
	}
	return 0;
}

int main(int argc, char** argv)
{
	
	time_sema = CreateSemaphore(NULL, 1, 1, NULL);

	int car_count = 7;
	lights_type type = lights_type::TimeAndQueue;
	
	if (argc > 1)
	{
		try
		{
			int argumentOfCount = stoi(string(argv[1]));
			if (argumentOfCount > 1 && argumentOfCount < 100)
			{
				car_count = argumentOfCount;
			}
			else
				printf("Invalid number of cars. Setting default of %d.\n", car_count);
		} catch (std::invalid_argument i)
		{
			printf("Invalid number of cars. Setting default of %d.\n", car_count);
		}
	}

	printf("Number of cars: %d.\n", car_count);

	if (argc > 2)
	{
		if (argv[2][0] == 'q')
			type = lights_type::QueueOnly;
		else if (argv[2][0] == 't')
			type = lights_type::TimeOnly;
		else if (argv[2][0] == 'b')
			type = lights_type::TimeAndQueue;
	}

	c = new Crossing(type);

	printf("Contol type: %s.\n", LightsStrings[(int)type]);

	for (int i = 0; i < car_count; i++)
	{
		CreateThread(NULL, 0, car, new car_params(i, i%2), 0, nullptr);
	}

	clock_t t_start = clock();

	bool time_shown = false;
	while(true)
	{
#if PRINT_TIME
		if (!time_shown && secondsFrom(t_start)>60)
		{
			cout << "\n***\n";
			cout << "MINIMUM TIME: " << min_time << endl;
			cout << "MAXIMUM TIME: " << max_time << endl;
			if (time_sum_divider != 0)
				cout << "AVERAGE TIME: " << time_sum / time_sum_divider << endl;
			cout << "***\n\n";
			time_shown = true;
			return 0;
		}
#endif
	}

	return 0;
}