

#define __STDC_LIMIT_MACROS

#include <stdint.h>

#include <string>

#if defined(__FreeBSD__) && !defined(__APPLE__)
#include <uuid.h>
#endif /* __FreeBSD__ */

#if defined(__APPLE__) || defined(__linux__)
#include <uuid/uuid.h>
#endif

#include "tuneables.h"

#include <libdfsutil/debug.h>

using namespace std;

/*
 * Generate a UUID
 */
std::string Util_NewUUID()
{
#if defined(__FreeBSD__) && !defined(__APPLE__)
    uint32_t status;
    uuid_t id;
    char *uuidBuf;

    uuid_create(&id, &status);
    if (status != uuid_s_ok) {
        printf("Failed to construct UUID!\n");
        return "";
    }
    uuid_to_string(&id, &uuidBuf, &status);
    if (status != uuid_s_ok) {
        printf("Failed to print UUID!\n");
        return "";
    }
    std::string rval(uuidBuf);
    free(uuidBuf);
    return rval;
#endif /* __FreeBSD__ */

#if defined(__APPLE__) || defined(__linux__)
    uuid_t id;
    uuid_generate(id);
    char id_buf[128];
    uuid_unparse(id, id_buf);
    
    std::string rval(id_buf);
    return rval;
#endif /* __APPLE__ || __linux__ */
}

