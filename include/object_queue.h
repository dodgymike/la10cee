// $Id$
#ifndef OBJECT_QUEUE_H
#define OBJECT_QUEUE_H

#include <queue>

#include "threadhelpers.h"

using namespace std;

template<class T> class ObjectQueue {
public:
	ObjectQueue() {
		m_consumerComplete = false;
		m_producerComplete = false;
	};
	~ObjectQueue() {
	};
	
	bool push(const T& newT) {
		AutoLock lock(&m_queueMutex);
		
		m_queue.push(newT);
		return true;
	};
	
	size_t size() {
		AutoLock lock(&m_queueMutex);
		
		return m_queue.size();
	};
	
	bool pop(T& newT) {
		AutoLock lock(&m_queueMutex);
		
		if(m_queue.size() <= 0) {
			return false;
		}
		
		newT = m_queue.front();
		m_queue.pop();
		return true;
	};
	
	bool pop() {
		AutoLock lock(&m_queueMutex);
		
		if(m_queue.size() <= 0) {
			return false;
		}
		
		m_queue.pop();
		return true;
	};
	
	T& front() {
		AutoLock lock(&m_queueMutex);
		
		return m_queue.front();
	}
	
	// accessors
	bool consumerComplete() {
		AutoLock lock(&m_queueMutex);
		
		return m_consumerComplete;
	};
	bool consumerComplete(bool consumerComplete) {
		AutoLock lock(&m_queueMutex);
		
		return(m_consumerComplete = consumerComplete);
	};

	bool producerComplete() {
		AutoLock lock(&m_queueMutex);
		
		return m_producerComplete;
	};
	bool producerComplete(bool producerComplete) {
		AutoLock lock(&m_queueMutex);
		
		return(m_producerComplete = producerComplete);
	};
private:
	queue<T> m_queue;
	bool m_consumerComplete;
	bool m_producerComplete;
	resourceMutexRetryLock m_queueMutex;
};

/*
$Log$
*/
#endif

