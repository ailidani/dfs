
#ifndef libdfsutil_lrucache_h
#define libdfsutil_lrucache_h

#include <assert.h>

#include <list>
#include <utility>
#include <stdexcept>
//#include "oritr1.h"
#include <unordered_map>
#include <unordered_set>

#include "debug.h"
#include "rwlock.h"

template <class K, class V, int MAX>
class LRUCache
{
public:
    typedef std::list<K> lru_list;
    typedef std::unordered_map<K,
            std::pair<V, typename lru_list::iterator> > lru_cache;
    LRUCache() : numItems(0) {}
    ~LRUCache() {}

    void put(K key, V value)
    {
        RWKey::sp ckey = lock.writeLock();

        typename lru_cache::iterator it = cache.find(key);

        if (numItems >= MAX && it == cache.end())
            evict(ckey);
        if (it != cache.end())
        {
            lru.erase((*it).second.second);
            numItems--;
        }

        assert(numItems == lru.size());

        typename lru_list::iterator p = lru.insert(lru.end(), key);
        cache[key] = std::make_pair(value, p);
        numItems++;
    }

    const V &get(K key)
    {
        RWKey::sp ckey = lock.writeLock();

        typename lru_cache::iterator it = cache.find(key);

        if (it != cache.end()) {
            lru.splice(lru.end(), lru, (*it).second.second);
            return (*it).second.first;
        }

        throw std::runtime_error("Key doesn't exist!");
    }

    /// Atomic get which returns true if key is cached (and value returned)
    bool get(K key, V &value)
    {
        RWKey::sp ckey = lock.writeLock();

        typename lru_cache::iterator it = cache.find(key);
        if (it == cache.end()) {
            return false;
        }
        else {
            lru.splice(lru.end(), lru, (*it).second.second);
            value = (*it).second.first;
            return true;
        }
    }

    bool hasKey(K key)
    {
        RWKey::sp ckey = lock.readLock();

        typename lru_cache::iterator it = cache.find(key);
        return it != cache.end();
    }
    void invalidate(K key)
    {
        RWKey::sp ckey = lock.writeLock();

        typename lru_cache::iterator it = cache.find(key);
        if (it == cache.end()) return;

        lru.erase((*it).second.second);
        cache.erase(it);
        numItems--;
    }
    void clear()
    {
        RWKey::sp ckey = lock.writeLock();

        lru.clear();
        cache.clear();
        numItems = 0;
    }
private:
    void evict(RWKey::sp ckey)
    {
        typename lru_cache::iterator it = cache.find(lru.front());

        assert(!lru.empty());

        cache.erase(it);
        lru.pop_front();
        numItems--;
    }
    lru_list lru;
    lru_cache cache;

    RWLock lock;
    // Because std::list::size() on GCC is O(N)
    uint32_t numItems;
};

#endif

