

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include <string>
#include <list>
#include <map>

#include <libdfsutil/dfsutil.h>
#include <libdfsutil/dfsfile.h>
#include <libdfsutil/dfsnet.h>
#include <libdfsutil/stream.h>
#include <libdfs/varlink.h>

using namespace std;

VarLink::VarLink()
{
}

VarLink::~VarLink()
{
}

void VarLink::open(const string &vardbPath)
{
    vardb = vardbPath;
    load();
}

string VarLink::get(const string &var) const
{
    map<string, string>::const_iterator it;

    if (var == "hostname")
        return OriNet_Hostname();
    if (var == "domainname")
        return OriNet_Domainname();
    if (var == "osname")
        return Util_GetOSType();
    if (var == "machtype")
        return Util_GetMachType();

    it = vars.find(var);
    if (it == vars.end())
        return "";

    return it->second;
}

void VarLink::set(const string &var, const string &val)
{
    vars[var] = val;

    save();
}

list<string> VarLink::getVars() const
{
    map<string, string>::const_iterator it;
    list<string> rval;

    for (it = vars.begin(); it != vars.end(); it++) {
        rval.push_back(it->first);
    }

    return rval;
}

/*
 * Parse variables of the form:
 *  ${VAR}
 *  ${VAR:DEFAULT}
 *
 * Default System Variables:
 *  hostname (gethostname
 *  domainname (getdomainname)
 *  osname (from environment variable OSTYPE)
 *  machtype (from environment variable MACHTYPE)
 */
string VarLink::parse(const string &link)
{
    return "";
}

string VarLink::getBlob()
{
    strwstream str;
    map<string, string>::const_iterator it;

    str.enableTypes();
    str.writeUInt32(vars.size());
    for (it = vars.begin(); it != vars.end(); it++) {
        str.writeLPStr(it->first);
        str.writeLPStr(it->second);
    }

    return str.str();
}

void VarLink::fromBlob(const string &blob)
{
    strstream str = strstream(blob);
    uint32_t entries;
    string var, val;

    str.enableTypes();
    entries = str.readUInt32();
    for (uint32_t i = 0; i < entries; i++) {
        str.readLPStr(var);
        str.readLPStr(val);

        vars[var] = val;
    }
}

void VarLink::load()
{
    if (OriFile_Exists(vardb)) {
        string blob = OriFile_ReadFile(vardb);
        fromBlob(blob);
    }
}

void VarLink::save()
{
    string blob = getBlob();
    OriFile_WriteFile(blob, vardb);
}

