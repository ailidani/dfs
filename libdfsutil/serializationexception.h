

#ifndef libdfsutil_serializationexception_h
#define libdfsutil_serializationexception_h

class SerializationException : public std::exception
{
public:
    SerializationException(const std::string &msg) {
        errorString = msg;
    }
    virtual ~SerializationException() throw()
    {
    }
    virtual const char* what() const throw()
    {
        return errorString.c_str();
    }
private:
    std::string errorString;
};

#endif

