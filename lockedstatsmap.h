#ifndef LOCKEDSTATSMAP_H_
#define LOCKEDSTATSMAP_H_

#include <map>
#include "thread.h"

using namespace std;

template<class K, class V> class LockedStatsMap {
public:
	LockedStatsMap() {
		
	}
	
	void addStat(K key, V statIn) {
		AutoLock lock(&m_mutex);
		
		m_statMap[key] += statIn;
	}
	
	const V get(K key) {
		AutoLock lock(&m_mutex);
		
		return m_statMap[key];
	}
	
	const V operator[](K key) {
		return get(key);
	}
	
	vector<K>* keys() {
		AutoLock lock(&m_mutex);
		
		vector<K>* keys = new vector<K>();
		
		for(typename map<K, V>::iterator i = m_statMap.begin(); i != m_statMap.end(); i++) {
			keys->push_back(i->first);
		}
		
		return keys;
	}
private:
	resourceMutexRetryLock m_mutex;
	map<K, V> m_statMap;
};


#endif /*LOCKEDSTATSMAP_H_*/
