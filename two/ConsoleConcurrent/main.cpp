#include <windows.h>
#include <cstdio>
#include <time.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <iterator>
#include "Crossing.h"
#include <random>

using namespace std;

Crossing *c;

HANDLE mayor;

enum class lights_type
{
	QueueOnly,
	TimeOnly,
	TimeAndQueue
};

double secondsFrom(clock_t tStart)
{
	return (double)(clock() - tStart) / (double)CLOCKS_PER_SEC;
}

int intRand(const int & min, const int & max) {
	static thread_local std::mt19937 generator;
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(generator);
}

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


DWORD WINAPI city_by_waiting_time(LPVOID b)
{
	bool can_switch_from_empty_queue = *static_cast<bool *>(b);

	clock_t t_start = clock();
	clock_t last_left = t_start;
	clock_t last_top = t_start;

	bool releasing_left = 0;

	double minimum_releasing_time = 2;
	double maximum_releasing_time = 5;

	while (true)
	{
		if (releasing_left)
		{
			if (c->GetNextFromTop() == 0)
				c->ReleaseFromLeft();
			if ((secondsFrom(last_left) > minimum_releasing_time && c->GetWaitingCountFromLeft() == 0) 
				|| secondsFrom(last_left) > maximum_releasing_time)
			{
				releasing_left = !releasing_left;
				last_top = clock();
			}
		} else
		{
			if (c->GetNextFromLeft() == 0)
				c->ReleaseFromTop();
			if ((secondsFrom(last_top) > minimum_releasing_time && c->GetWaitingCountFromTop() == 0)
				|| secondsFrom(last_top) > maximum_releasing_time)
			{
				releasing_left = !releasing_left;
				last_left = clock();
			}
		}
	}
	return 0;
}

DWORD WINAPI city_by_queue_size(LPVOID)
{
	while (true)
	{
		auto left_count = c->GetWaitingCountFromLeft();
		auto top_count = c->GetWaitingCountFromTop();

		if (left_count >= top_count && c->GetNextFromTop() == 0)
			c->ReleaseFromLeft();
		else if (top_count >= left_count && c->GetNextFromLeft() == 0)
			c->ReleaseFromTop();
	}
	return 0;
}

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

		//Going around
		Sleep(2400 + intRand(600, 2000 - id)); // id is only used for making distribution unique, to give different random number sequences.
		from_left = !from_left;
	}
	return 0;
}

int main(int argc, char** argv)
{
	c = new Crossing();

	int car_count = 7;
	lights_type type = lights_type::TimeAndQueue;
	
	if (argc > 0)


	switch(type)
	{
	case lights_type::QueueOnly:
		mayor = CreateThread(NULL, 0, city_by_queue_size, nullptr, 0, nullptr);
		break;
	case lights_type::TimeOnly:
		mayor = CreateThread(NULL, 0, city_by_waiting_time, new bool(false), 0, nullptr);
		break;
	case lights_type::TimeAndQueue:
		mayor = CreateThread(NULL, 0, city_by_waiting_time, new bool(true), 0, nullptr);
		break;
	}

	mayor = CreateThread(NULL, 0, city_by_waiting_time, new bool (true), 0, nullptr);

	for (int i = 0; i < car_count; i++)
	{
		CreateThread(NULL, 0, car, new car_params(i, i%2), 0, nullptr);
	}

	while(true)
	{
		
	}

	return 0;
}