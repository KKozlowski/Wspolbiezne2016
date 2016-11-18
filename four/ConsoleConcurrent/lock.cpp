#include "lock.h"

lock::lock()
{
	xlocked = -1;
	semaS = CreateSemaphore(nullptr, 1, 1, nullptr);
	semaX = CreateSemaphore(nullptr, 1, 1, nullptr);
}

bool lock::slock(int id)
{
	WaitForSingleObject(semaX, INFINITE);
		bool notX = xlocked == -1;
	ReleaseSemaphore(semaX, 1, nullptr);

	WaitForSingleObject(semaS, INFINITE);
		if (notX && slocked.find(id) == slocked.end())
		{
			slocked.insert(id);
		}
	ReleaseSemaphore(semaS, 1, nullptr);
	return notX;
}

bool lock::sunlock(int id)
{
	bool result = false;

	WaitForSingleObject(semaS, INFINITE);
		if (slocked.find(id) != slocked.end())
		{
			slocked.erase(id);
			result = true;
		}
	ReleaseSemaphore(semaS, 1, nullptr);

	return result;
}

bool lock::xlock(int id)
{
	int scount = 0;
	WaitForSingleObject(semaS, INFINITE);
		scount = slocked.size();
	ReleaseSemaphore(semaS, 1, nullptr);

	bool result = false;

	WaitForSingleObject(semaX, INFINITE);
		if (scount == 0 && xlocked == -1)
		{
			xlocked = id;
			result = true;
		}
	ReleaseSemaphore(semaX, 1, nullptr);
	return result;
}

bool lock::xunlock(int id)
{
	bool result = false;
	WaitForSingleObject(semaX, INFINITE);
		if (xlocked == id)
		{
			xlocked = -1;
			result = true;
		}
	ReleaseSemaphore(semaX, 1, nullptr);
	return result;
}
