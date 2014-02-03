

#ifndef dfs_oricmd_h
#define dfs_oricmd_h

class OriPriv;

class OriCommand
{
public:
    OriCommand(OriPriv *priv);
    ~OriCommand();
    std::string process(const std::string &data);
private:
    std::string cmd_fsck(strstream &str);
    std::string cmd_snapshot(strstream &str);
    std::string cmd_snapshots(strstream &str);
    std::string cmd_status(strstream &str);
    std::string cmd_pull(strstream &str);
    std::string cmd_checkout(strstream &str);
    std::string cmd_merge(strstream &str);
    std::string cmd_varlink(strstream &str);
    std::string cmd_remote(strstream &str);
    std::string cmd_branch(strstream &str);
    std::string cmd_version(strstream &str);
    OriPriv *priv;
};

#endif

