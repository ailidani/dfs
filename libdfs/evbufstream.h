

#ifndef libdfs_evbufstream_h
#define libdfs_evbufstream_h

#include <event2/buffer.h>

#include <libdfsutil/stream.h>

class evbufstream : public bytestream
{
public:
    evbufstream(struct evbuffer *inbuf);

    bool ended();
    size_t read(uint8_t *buf, size_t n);
    size_t sizeHint() const;

private:
    struct evbuffer *buf;
    size_t bufSize;
};

class evbufwstream : public bytewstream
{
public:
    evbufwstream(struct evbuffer *inbuf = NULL);
    ~evbufwstream();
    ssize_t write(const void *ptr, size_t n);

    struct evbuffer *buf() const;

private:
    struct evbuffer *_buf;
    bool owner;
};

#endif

