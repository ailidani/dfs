

#ifndef libdfsutil_runtimeexception_h
#define libdfsutil_runtimeexception_h

#include <string.h>

#ifndef _WIN32
#include <errno.h>
#endif

#include <string>
#include <exception>

enum OriErrorCode {
    ORIEC_INVALIDARGS,
    ORIEC_UNSUPPORTEDVERSION,
    ORIEC_INDEXDIRTY,
    ORIEC_INDEXCORRUPT,
};

class RuntimeException : public std::exception
{
public:
    RuntimeException(OriErrorCode code, const std::string &msg) {
        errorString = "Exception: ";
        errorString += msg;
        errorCode = code;
    }
    RuntimeException(OriErrorCode code, const char *msg) {
        errorString = "Exception: ";
        errorString += msg;
        errorCode = code;
    }
    virtual ~RuntimeException() throw()
    {
    }
    virtual OriErrorCode getCode() throw()
    {
        return errorCode;
    }
    virtual const char* what() const throw()
    {
        return errorString.c_str();
    }
private:
    std::string errorString;
    OriErrorCode errorCode;
};

#endif
