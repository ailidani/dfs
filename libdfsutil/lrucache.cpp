

#include <assert.h>

#include <iostream>
//#include <boost/tr1/memory.hpp>
#include <memory>

#include <libdfsutil/debug.h>
#include <libdfsutil/lrucache.h>

using namespace std;

int
LRUCache_selfTest(void)
{
    LRUCache<string, string, 4> cache = LRUCache<string, string, 4>();

    cout << "Testing LRUCache ..." << endl;

    cache.put("A", "1");
    cache.put("B", "2");
    cache.put("C", "3");
    cache.put("D", "4");

    assert(cache.hasKey("A"));
    assert(cache.hasKey("B"));
    assert(cache.hasKey("C"));
    assert(cache.hasKey("D"));

    // Test eviction
    cache.put("E", "5");

    assert(cache.hasKey("E"));
    assert(!cache.hasKey("A"));

    // Test LRU reordering
    cache.get("B");
    cache.put("F", "6");

    assert(cache.hasKey("B"));
    assert(!cache.hasKey("C"));

    // Replacing key's should not evict
    cache.put("B", "NEW");

    assert(cache.hasKey("B"));
    assert(cache.hasKey("D"));
    assert(cache.hasKey("E"));
    assert(cache.hasKey("F"));

    return 0;
}

