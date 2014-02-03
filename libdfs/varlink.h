

#ifndef libdfs_varlink_h
#define libdfs_varlink_h

class VarLink
{
public:
    VarLink();
    ~VarLink();
    void open(const std::string &vardbPath);
    std::string get(const std::string &var) const;
    void set(const std::string &var, const std::string &val);
    std::list<std::string> getVars() const;
    std::string parse(const std::string &link);
private:
    std::string getBlob();
    void fromBlob(const std::string &blob);
    void load();
    void save();
    std::string vardb;
    std::map<std::string, std::string> vars;
};

#endif

