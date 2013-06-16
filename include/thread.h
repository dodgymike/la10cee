#ifndef THREAD_ADAPTER_H
#define THREAD_ADAPTER_H


#include <queue>
#include <fstream>

#include "object_queue.h"

class Thread
{
public:
	Thread()
		: m_running(false), m_paused(false), m_complete(false)
	{};

	virtual ~Thread()
	{};

	virtual bool run() = 0;
	virtual bool start()
	{
		{
			AutoLock runningLock(&m_runningLock);
			m_running = true;
		}
		
		bool runResult = run();
		
		{
			AutoLock runningLock(&m_runningLock);
			m_complete = true;
		}
		
		return runResult;
	};

	virtual bool pause(bool pauseFlag)
	{
		AutoLock pausedLock(&m_pausedLock);
//			cerr << "pauseFlag [" << pauseFlag << "]" << endl;
			m_paused = pauseFlag;
	
		return true;
	};

	virtual bool stop() {
		{
			AutoLock runningLock(&m_runningLock);
			m_running = false;
		}

		return _stop();
	};
	
	virtual bool _stop() {
		return true;
	}

	bool running()
	{
		AutoLock runningLock(&m_runningLock);

		return m_running;
	};
	
	bool complete()
	{
		AutoLock runningLock(&m_runningLock);

		return m_complete;
	};
	

protected:
	resourceMutexRetryLock m_runningLock;
	resourceMutexRetryLock m_pausedLock;
	
	bool m_running;
	bool m_paused;
	bool m_complete;
};

#endif

