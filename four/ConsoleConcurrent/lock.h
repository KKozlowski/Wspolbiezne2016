#pragma once
#include <set>
#include <windows.h>
class lock
{
private:
	std::set<int> slocked;
	int xlocked;

	HANDLE sema_shared;
	HANDLE sema_exclusive;
public:
	lock();

	bool shared_lock(int id);
	bool shared_unlock(int id);

	bool exclusive_lock(int id);
	bool exclusive_unlock(int id);
};
