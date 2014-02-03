

#ifndef libdfs_chunker_h
#define libdfs_chunker_h

class ChunkerCB
{
public:
    // Report a match
    virtual void match(const uint8_t *buf, uint32_t len) = 0;
    /*
     * Request more data from the buffer. This method should return 1 when 
     * there is more data available and zero when done. Offset should always be 
     * set to point to at least hashLen bytes.
     */
    virtual int load(uint8_t **buf, uint64_t *len, uint64_t *off) = 0;
};

#endif

