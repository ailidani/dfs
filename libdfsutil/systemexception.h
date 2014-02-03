
#ifndef libdfsutil_systemexception_h
#define libdfsutil_systemexception_h

#include <string.h>

#ifndef _WIN32
#include <errno.h>
#endif

#include <string>
#include <exception>

class SystemException : public std::exception
{
public:
    SystemException() {
        errorString = "Exception: ";
        errorString += strerror(errno);
        errnum = errno;
    }
    SystemException(int errorNumber) {
        errorString = "Exception: ";
        errorString += strerror(errorNumber);
        errnum = errorNumber;
    }
    virtual ~SystemException() throw()
    {
    }
    virtual int getErrno() const throw()
    {
        return errnum;
    }
    virtual const char* what() const throw()
    {
        // Generate error string strerror_r
        return errorString.c_str();
    }
private:
    std::string errorString;
    int errnum;
};

#endif
