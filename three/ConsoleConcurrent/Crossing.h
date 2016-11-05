#pragma once
#include <cstdint>
#include <queue>
#include <windows.h>
#include <ctime>
#include <random>

enum class lights_type
{
	QueueOnly,
	TimeOnly,
	TimeAndQueue
};

double secondsFrom(clock_t tStart);

int intRand(const int& min, const int& max);

class Crossing
{
private:
	//LEFT
	HANDLE left_sema = nullptr;
	std::queue<uint64_t> left;
	uint64_t last_generated_left = 0;;
	uint64_t next_left = 0;

	//TOP
	HANDLE top_sema = nullptr;
	std::queue<uint64_t> top;
	uint64_t last_generated_top = 0;
	uint64_t next_top = 0;

	lights_type mode;

	bool releasing_left = false;

	//TIME STUFF STARTS HERE
	clock_t t_start;
	clock_t last_left;
	clock_t last_top;

	double minimum_releasing_time = 0;
	double maximum_releasing_time = 5;
	//ENDS HERE

	HANDLE mayor;

	friend DWORD WINAPI city_simple(LPVOID lpCrossing);

	uint64_t GetWaitingCountFromTop();
	uint64_t GetWaitingCountFromLeft();

	void AcceptFromLeft();
	void AcceptFromTop();
	void Select();
public:
	uint64_t WaitFromLeft();
	uint64_t GetNextFromLeft();
	void SignalFromLeft();

	uint64_t WaitFromTop();
	uint64_t GetNextFromTop();
	void SignalFromTop();

	Crossing(lights_type mode);
};
