#include "lock.h"

lock::lock()
{
	xlocked = -1;
	sema_shared = CreateSemaphore(nullptr, 1, 1, nullptr);
	sema_exclusive = CreateSemaphore(nullptr, 1, 1, nullptr);
}

bool lock::shared_lock(int id)
{
	WaitForSingleObject(sema_exclusive, INFINITE);
		bool notX = xlocked == -1;
	ReleaseSemaphore(sema_exclusive, 1, nullptr);

	WaitForSingleObject(sema_shared, INFINITE);
		if (notX && slocked.find(id) == slocked.end())
		{
			slocked.insert(id);
		}
	ReleaseSemaphore(sema_shared, 1, nullptr);
	return notX;
}

bool lock::shared_unlock(int id)
{
	bool result = false;

	WaitForSingleObject(sema_shared, INFINITE);
		if (slocked.find(id) != slocked.end())
		{
			slocked.erase(id);
			result = true;
		}
	ReleaseSemaphore(sema_shared, 1, nullptr);

	return result;
}

bool lock::exclusive_lock(int id)
{
	int scount = 0;
	WaitForSingleObject(sema_shared, INFINITE);
		scount = slocked.size();
	ReleaseSemaphore(sema_shared, 1, nullptr);

	bool result = false;

	WaitForSingleObject(sema_exclusive, INFINITE);
		if (scount == 0 && xlocked == -1)
		{
			xlocked = id;
			result = true;
		}
	ReleaseSemaphore(sema_exclusive, 1, nullptr);
	return result;
}

bool lock::exclusive_unlock(int id)
{
	bool result = false;
	WaitForSingleObject(sema_exclusive, INFINITE);
		if (xlocked == id)
		{
			xlocked = -1;
			result = true;
		}
	ReleaseSemaphore(sema_exclusive, 1, nullptr);
	return result;
}
