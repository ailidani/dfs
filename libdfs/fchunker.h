

/*
 * Implements a fixed chunker
 */

#ifndef libdfs_fchunker_h
#define libdfs_fchunker_h

#include "chunker.h"

template<int chunk_size>
class FChunker
{
public:
    FChunker();
    ~FChunker();
    void chunk(ChunkerCB *cb);
};

template<int chunk_size>
FChunker<chunk_size>::FChunker()
{
}

template<int chunk_size>
FChunker<chunk_size>::~FChunker()
{
}

template<int chunk_size>
void FChunker<chunk_size>::chunk(ChunkerCB *cb)
{
    uint8_t *in = NULL;
    uint64_t len = 0;
    register uint64_t off = 0;
    register uint64_t start = 0;

    if (cb->load(&in, &len, &off) == 0) {
        assert(false);
        return;
    }

fastPath:
    while (len > off + chunk_size) {
        off += chunk_size;
        cb->match(in + start, off - start);
        start = off;
    }

    if (cb->load(&in, &len, &off) == 1) {
        start = off;
        goto fastPath;
    }

    if (len > start) {
        cb->match(in + start, len - start);
    }

    return;
}

#endif

