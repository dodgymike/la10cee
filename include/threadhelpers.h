#ifndef _THREADHELPERS_H_
#define _THREADHELPERS_H_

#include <string>
#include <pthread.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "platform.h"

using namespace std;

// Flags
#define THREADHELPERS_DEBUG
#define RESOURCEMUTEXRETRYLOCK_MAX_ATTEMPTS 100

// functions to help with common thread procedures


// This class can be used to lock resources intelligently
// It is the base class for all lock types (mutex, safeMutex, persistentMutex)
class resourceLock
{
public:
	virtual bool trylock() = 0;
	virtual bool lock() = 0;
	virtual bool unlock() = 0;

	resourceLock()
	{
		m_lockName = "Default";
		m_oldLockName = "Default";
		pthread_mutex_init(&m_lockNameLock, NULL);
	}

	// ***************************************************
	// This stuff is a hack to find bad locks
	// ***************************************************
	bool namedTryLock(const string& lockName)
	{
/*
		pthread_mutex_lock(&m_lockNameLock);
			m_oldLockName = m_lockName;
			m_lockName = lockName;
		pthread_mutex_unlock(&m_lockNameLock);
*/

		return trylock();
	}
		
	bool namedLock(const string& lockName)
	{
/*
		pthread_mutex_lock(&m_lockNameLock);
			m_oldLockName = m_lockName;
			m_lockName = lockName;
		pthread_mutex_unlock(&m_lockNameLock);
*/

		return lock();
	}

	const string& getOldLockName()
	{
/*
		pthread_mutex_lock(&m_lockNameLock);
			string tempOldLockName = m_oldLockName;
		pthread_mutex_unlock(&m_lockNameLock);

		return tempOldLockName;
*/

		return m_oldLockName;
	}

	const string& getLockName()
	{
/*
		pthread_mutex_lock(&m_lockNameLock);
			string tempLockName = m_lockName;
		pthread_mutex_unlock(&m_lockNameLock);

		return tempLockName;
*/

		return m_lockName;
	}

	virtual ~resourceLock() {};

private:
	string m_lockName;
	string m_oldLockName;
	pthread_mutex_t m_lockNameLock;
};

// This class should only be used with the classes below
// It provides an easy method of ensuring a 'resourceLock' object
// gets 'unlocked' even if an Exception is thrown.
// Simply declare a 'SafeResourceLock' on the stack, with your intended resourceLock
// object in the constructor and then when the stack unwinds, it will unlock your resourceLock
class SafeResourceLock //: public resourceLock
{
public:
	SafeResourceLock(resourceLock* myLock)
		: m_myLock(myLock)
	{}

	~SafeResourceLock()
	{
		m_myLock->unlock();
	}

	bool trylock()
	{
		return m_myLock->trylock();
	}

	bool lock()
	{
		m_myLock->lock();
		return true;
	}

	bool unlock()
	{
		m_myLock->unlock();
		return true;
	}

private:
	resourceLock* m_myLock;
};

class AutoLock //: public resourceLock
{
public:
	AutoLock(resourceLock* myLock)
		: m_myLock(myLock)
	{
		m_myLock->lock();
	}

	~AutoLock()
	{
		m_myLock->unlock();
	}

	/*
	bool trylock()
	{
		return m_myLock->trylock();
	}

	bool lock()
	{
		m_myLock->lock();
		return true;
	}

	bool unlock()
	{
		m_myLock->unlock();
		return true;
	}
	*/

private:
	resourceLock* m_myLock;
};

// This is a simple, fast mutex-based lock
class resourceMutexLock : public resourceLock
{
public:
	resourceMutexLock()
	{
		pthread_mutex_init(&m_lock, NULL);
	}

	~resourceMutexLock()
	{
		pthread_mutex_unlock(&m_lock);
	}

	bool trylock()
	{ 
		int rv = 0;
		if((rv = pthread_mutex_trylock(&m_lock)) == EBUSY)
		{ return false; }
		else if(rv == 0)
		{ return true; }
		else
		{ 
			cerr << "<resourceMutexLock::trylock>	Got a wierd return value [" << rv << "]" << endl;
			abort();
			return true; 
		}
	}

	bool lock()
	{
		pthread_mutex_lock(&m_lock);
		return true;
	}

	bool unlock()
	{
		pthread_mutex_unlock(&m_lock);
		return true;
	}

private:
	pthread_mutex_t	m_lock;
};

// This is a relatively simple, safe mutex-based lock
class resourceMutexRetryLock : public resourceLock
{
public:
	resourceMutexRetryLock()
	{
		pthread_mutex_init(&m_lock, NULL);
	}

	~resourceMutexRetryLock()
	{
		pthread_mutex_unlock(&m_lock);
	}

	bool trylock()
	{ 
		int trylockResult = pthread_mutex_trylock(&m_lock);
		if(trylockResult == EBUSY) {
			return false;
		} else if(trylockResult == EINVAL) {
			// oops!
			cerr << "<resourceMutexRetryLock::trylock>\t**** invalid lock when trying to lock! This shouldn't happen!!!! ****" << endl;
			sleep(1);
			throw("<resourceMutexRetryLock::trylock>\t**** invalid lock when trying to lock! This shouldn't happen!!!! ****");
		} else { 
			return true;
		}
		
		return true;
	}

	bool lock()
	{
		int attemptCounter = 0;

		while(!trylock())
		{
			attemptCounter++;
	
			if(attemptCounter > RESOURCEMUTEXRETRYLOCK_MAX_ATTEMPTS)
			{ 
				cerr << "<resourceMutexRetryLock>	new lock [" << getLockName() << "] still waiting for lock from old lock [" << getOldLockName() << "] after RESOURCEMUTEXRETRYLOCK_MAX_ATTEMPTS seconds" << endl; 
				sleep(5);
				throw("couldn't get lock after a lot of attempts");
			}
			sleep(1);
		}

		return true;
	}

	bool unlock()
	{
		pthread_mutex_unlock(&m_lock);
		return true;
	}

private:
	pthread_mutex_t m_lock;
};

#endif

