#include "Crossing.h"

double secondsFrom(clock_t tStart)
{
	return (double)(clock() - tStart) / (double)CLOCKS_PER_SEC;
}

int intRand(const int& min, const int& max)
{
	static thread_local std::mt19937 generator;
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(generator);
}

DWORD  WINAPI city_simple(LPVOID lpCrossing)
{
	Crossing *c = static_cast<Crossing *>(lpCrossing);
	while (true)
	{
		c->Select();
	}
	return 0;
}

uint64_t Crossing::WaitFromLeft()
{
	WaitForSingleObject(left_sema, INFINITE);
		uint64_t id = ++last_generated_left;
		left.push(id);
	ReleaseSemaphore(left_sema, 1, nullptr);
	return id;
}

uint64_t Crossing::GetNextFromLeft()
{
	WaitForSingleObject(left_sema, INFINITE);
		uint64_t id = next_left;
	ReleaseSemaphore(left_sema, 1, nullptr);
	return id;
}

uint64_t Crossing::GetWaitingCountFromLeft()
{
	WaitForSingleObject(left_sema, INFINITE);
		uint64_t count = left.size();
	ReleaseSemaphore(left_sema, 1, nullptr);
	return count;
}

void Crossing::SignalFromLeft()
{
	WaitForSingleObject(left_sema, INFINITE);
		next_left = 0;
		left.pop();
	ReleaseSemaphore(left_sema, 1, nullptr);
}

void Crossing::AcceptFromLeft()
{
	WaitForSingleObject(left_sema, INFINITE);
		if (left.size() > 0)
			next_left = left.front();
		else
			next_left = 0;
	ReleaseSemaphore(left_sema, 1, nullptr);
}

uint64_t Crossing::WaitFromTop()
{
	WaitForSingleObject(top_sema, INFINITE);
		uint64_t id = ++last_generated_top;
		top.push(id);
	ReleaseSemaphore(top_sema, 1, nullptr);
	return id;
}

uint64_t Crossing::GetNextFromTop()
{
	WaitForSingleObject(top_sema, INFINITE);
		uint64_t id = next_top;
	ReleaseSemaphore(top_sema, 1, nullptr);
	return id;
}

uint64_t Crossing::GetWaitingCountFromTop()
{
	WaitForSingleObject(top_sema, INFINITE);
		uint64_t count = top.size();
	ReleaseSemaphore(top_sema, 1, nullptr);
	return count;
}

void Crossing::SignalFromTop()
{
	WaitForSingleObject(top_sema, INFINITE);
		next_top = 0;
		top.pop();
	ReleaseSemaphore(top_sema, 1, nullptr);
}

void Crossing::AcceptFromTop()
{
	WaitForSingleObject(top_sema, INFINITE);
		if (top.size() > 0)
			next_top = top.front();
		else
			next_top = 0;
	ReleaseSemaphore(top_sema, 1, nullptr);
}

void Crossing::Select()
{
	if (mode == lights_type::QueueOnly)
	{
		auto left_count = GetWaitingCountFromLeft();
		auto top_count = GetWaitingCountFromTop();

		if (left_count >= top_count && GetNextFromTop() == 0)
			AcceptFromLeft();
		else if (top_count >= left_count && GetNextFromLeft() == 0)
			AcceptFromTop();
	} else if (mode == lights_type::TimeOnly || mode == lights_type::TimeAndQueue)
	{
		bool can_switch_from_empty_queue = mode == lights_type::TimeAndQueue;

		if (releasing_left)
		{
			if (GetNextFromTop() == 0)
				AcceptFromLeft();
			if ((can_switch_from_empty_queue
				&& secondsFrom(last_left) > minimum_releasing_time
				&& GetWaitingCountFromLeft() == 0)
				|| secondsFrom(last_left) > maximum_releasing_time)
			{
				releasing_left = !releasing_left;
				last_top = clock();
			}
		}
		else
		{
			if (GetNextFromLeft() == 0)
				AcceptFromTop();
			if ((can_switch_from_empty_queue
				&& secondsFrom(last_top) > minimum_releasing_time
				&& GetWaitingCountFromTop() == 0)
				|| secondsFrom(last_top) > maximum_releasing_time)
			{
				releasing_left = !releasing_left;
				last_left = clock();
			}
		}
	}
}

Crossing::Crossing(lights_type mode)
{
	this->mode = mode;
	left_sema = CreateSemaphore(NULL, 1, 1, NULL);
	top_sema = CreateSemaphore(NULL, 1, 1, NULL);

	t_start = clock();
	last_left = t_start;
	last_top = t_start;

	mayor = CreateThread(NULL, 0, city_simple, this, 0, nullptr);
}