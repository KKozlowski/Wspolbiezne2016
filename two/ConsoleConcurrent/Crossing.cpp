#include "Crossing.h"

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

void Crossing::ReleaseFromLeft()
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

void Crossing::ReleaseFromTop()
{
	WaitForSingleObject(top_sema, INFINITE);
		if (top.size() > 0)
			next_top = top.front();
		else
			next_top = 0;
	ReleaseSemaphore(top_sema, 1, nullptr);
}

Crossing::Crossing()
{
	left_sema = CreateSemaphore(NULL, 1, 1, NULL);
	top_sema = CreateSemaphore(NULL, 1, 1, NULL);
}
