#pragma once
#include <cstdint>
#include <queue>
#include <windows.h>

class Crossing
{
private:
	HANDLE left_sema = nullptr;
	std::queue<uint64_t> left;
	uint64_t last_generated_left = 0;;
	uint64_t next_left = 0;

	HANDLE top_sema = nullptr;
	std::queue<uint64_t> top;
	uint64_t last_generated_top = 0;
	uint64_t next_top = 0;
public:
	uint64_t WaitFromLeft();
	uint64_t GetNextFromLeft();
	uint64_t GetWaitingCountFromLeft();
	void SignalFromLeft();
	void ReleaseFromLeft();

	uint64_t WaitFromTop();
	uint64_t GetNextFromTop();
	uint64_t GetWaitingCountFromTop();
	void SignalFromTop();
	void ReleaseFromTop();

	Crossing();
};
