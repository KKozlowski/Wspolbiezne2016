#pragma once
#include <set>
#include <windows.h>
class lock
{
private:
	std::set<int> slocked;
	int xlocked;

	HANDLE semaS;
	HANDLE semaX;
public:
	lock();

	bool slock(int id);
	bool sunlock(int id);

	bool xlock(int id);
	bool xunlock(int id);
};
