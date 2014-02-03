

#ifndef libdfs_treediff_h
#define libdfs_treediff_h

#include <string>
#include <vector>
//#include <boost/tr1/unordered_map.hpp>
#include <unordered_map>

#include "repo.h"
#include "tree.h"

struct TreeDiffEntry
{
    enum DiffType {
        Noop = '-', // only used as placeholder
        NewFile = 'A',
        NewDir = 'n',
        DeletedFile = 'D',
        DeletedDir = 'd',
        Modified = 'm',
        Renamed = 'R',
        // ModifiedDiff = 'M'
        MergeConflict = 'C',
        FileDirConflict = 'x',
        MergeResolved = 'c',
    } type;

    TreeDiffEntry();
    TreeDiffEntry(const std::string &filepath, DiffType t);

    std::string filepath; // path relative to repo, with leading '/'
    std::string newFilename; // filename of a file containing the new contents
    std::pair<ObjectHash, ObjectHash> hashes;
    AttrMap newAttrs;
    AttrMap attrsA, attrsB, attrsBase;

    // Merge Conflict Fields
    std::string fileA, fileB, fileBase;
    std::pair<ObjectHash, ObjectHash> hashA, hashB, hashBase;

    void _diffAttrs(const AttrMap &a_old, const AttrMap &a_new);
};

class TreeDiff
{
public:
    TreeDiff();
    void diffTwoTrees(const Tree::Flat &t1, const Tree::Flat &t2);
    void diffToDir(Commit from, const std::string &dir, Repo *r);
    TreeDiffEntry *getLatestEntry(const std::string &path);
    const TreeDiffEntry *getLatestEntry(const std::string &path) const;
    void append(const TreeDiffEntry &to_append);
    /**
     * @returns true if mergeInto causes TreeDiff to grow a layer
     * e.g. D+n or d+A
     */
    bool mergeInto(const TreeDiffEntry &to_merge);

    void mergeTrees(const TreeDiff &d1, const TreeDiff &d2);
    void mergeChanges(const TreeDiff &d1, const TreeDiff &diff);

    void applyTo(Tree::Flat *flat) const;
    Tree applyTo(Tree::Flat flat, Repo *dest_repo);
    void dump() const;

    std::vector<TreeDiffEntry> entries;

private:
    std::unordered_map<std::string, size_t> latestEntries;
    void _resetLatestEntry(const std::string &filepath);
};

#endif
