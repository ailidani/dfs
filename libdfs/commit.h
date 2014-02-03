

#ifndef libdfs_commit_h
#define libdfs_commit_h

#include <stdint.h>
#include <time.h>

#include <utility>
#include <string>

#include <libdfsutil/key.h>
#include <libdfsutil/objecthash.h>

#define COMMIT_VERSION                  0x00000001
#define COMMIT_FLAG_IS_GRAFT            0x00000001
#define COMMIT_FLAG_HAS_SIGNATURE       0x00000002

class Commit
{
public:
    Commit();
    ~Commit();
    void setParents(ObjectHash p1, ObjectHash p2 = ObjectHash());
    std::pair<ObjectHash, ObjectHash> getParents() const;
    void setMessage(const std::string &msg);
    std::string getMessage() const;
    void setTree(const ObjectHash &tree);
    ObjectHash getTree() const;
    void setUser(const std::string &user);
    std::string getUser() const;
    void setSnapshot(const std::string &snapshot);
    std::string getSnapshot() const;
    void setTime(time_t t);
    time_t getTime() const;

    // Digital Signatures
    // XXX: Support retrieving key fingerprint
    void sign(const PrivateKey &key);
    bool verify(const PublicKey &key);
    bool hasSignature();

    // Grafting
    void setGraft(const std::string &repo,
                  const std::string &path,
                  const ObjectHash &commidId);
    std::pair<std::string, std::string> getGraftRepo() const;
    ObjectHash getGraftCommit() const;

    // Serialization
    std::string getBlob(bool withSignature = true) const;
    void fromBlob(const std::string &blob);

    ObjectHash hash() const; // TODO: cache this
    void print() const;
private:
    uint32_t version;
    uint32_t flags;
    std::pair<ObjectHash, ObjectHash> parents;
    std::string message;
    ObjectHash treeObjId;
    std::string user;
    std::string snapshotName;
    time_t date;
    std::string signature;
    std::string graftRepo;
    std::string graftPath;
    ObjectHash graftCommitId;
};

#endif

