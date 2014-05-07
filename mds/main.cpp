
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <string>
#include <iostream>

using namespace std;

#include <libdfsutil/debug.h>
#include <libdfsutil/dfsutil.h>
#include <libdfsutil/dfsfile.h>
#include <libdfs/version.h>
#include <libdfs/localrepo.h>

#define MDS_LOGFILE         "/.ori/mds.log"

/********************************************************************
 *
 *
 * Command Infrastructure
 *
 *
 ********************************************************************/

#define CMD_DEBUG               1

typedef struct Cmd {
    const char *name;
    const char *desc;
    int (*cmd)(int argc, const char *argv[]);
    void (*usage)(void);
    int flags;
} Cmd;

// General Operations
int start_server();
int cmd_init(int argc, const char *argv[]);
int cmd_add(int argc, const char *argv[]);
int cmd_remove(int argc, const char *argv[]);
int cmd_list(int argc, const char *argv[]);
int cmd_hostadd(int argc, const char *argv[]);
int cmd_hostremove(int argc, const char *argv[]);
int cmd_hosts(int argc, const char *argv[]);
static int cmd_help(int argc, const char *argv[]);
// Debug Operations
static int cmd_foreground(int argc, const char *argv[]);

static Cmd commands[] = {
    {
        "init",
        "Interactively configure MDS",
        cmd_init,
        NULL,
        0,
    },
    {
        "add",
        "Add a repository to manage",
        cmd_add,
        NULL,
        0,
    },
    {
        "remove",
        "Remove a repository from MDS",
        cmd_remove,
        NULL,
        0,
    },
    {
        "list",
        "List registered repositories",
        cmd_list,
        NULL,
        0,
    },
    {
        "hostadd",
        "Add static host",
        cmd_hostadd,
        NULL,
        0,
    },
    {
        "hostremove",
        "Remove static host",
        cmd_hostremove,
        NULL,
        0,
    },
    {
        "hosts",
        "List static hosts",
        cmd_hosts,
        NULL,
        0,
    },
    {
        "help",
        "Display the help message",
        cmd_help,
        NULL,
        0,
    },
    /*
    {
        "foreground",
        "Foreground mode for debugging",
        cmd_foreground,
        NULL,
        CMD_DEBUG,
    },
    */
    { NULL, NULL, NULL, NULL }
};

static int lookupcmd(const char *cmd)
{
    int i;

    for (i = 0; commands[i].name != NULL; i++)
    {
        if (strcmp(commands[i].name, cmd) == 0)
            return i;
    }

    return -1;
}

static int cmd_help(int argc, const char *argv[])
{
    int i = 0;

    if (argc >= 2) {
        i = lookupcmd(argv[1]);
        if (i != -1 && commands[i].usage != NULL) {
            commands[i].usage();
            return 0;
        }
        if (i == -1) {
            printf("Unknown command '%s'\n", argv[1]);
        } else {
            printf("No help for command '%s'\n", argv[1]);
        }
        return 0;
    }

    printf("MDS (%s) - Command Line Interface\n\n",
            ORI_VERSION_STR);
    printf("Available commands:\n");
    for (i = 0; commands[i].name != NULL; i++)
    {
#ifndef DEBUG
        if (commands[i].flags & CMD_DEBUG)
            continue;
#endif /* DEBUG */
        if (commands[i].desc != NULL)
            printf("%-15s %s\n", commands[i].name, commands[i].desc);
    }

    return 0;
}

static int cmd_foreground(int argc, const char *argv[])
{
    string oriHome = Util_GetHome() + "/.ori";

    if (!OriFile_Exists(oriHome))
        OriFile_MkDir(oriHome);

    // Chdir so that coredumps are placed in ~/.ori
    chdir(oriHome.c_str());

    dfs_open_log(Util_GetHome() + MDS_LOGFILE);

    //return start_server();
}

int main(int argc, char *argv[])
{
    int idx;

    if (argc == 0) {
        pid_t pid;
        string oriHome = Util_GetHome() + "/.ori";

        if (!OriFile_Exists(oriHome))
            OriFile_MkDir(oriHome);

        // Chdir so that coredumps are placed in ~/.ori
        chdir(oriHome.c_str());

        dfs_open_log(Util_GetHome() + MDS_LOGFILE);

        pid = fork();
        if (pid == -1) {
            perror("fork");
            return 1;
        }
        if (pid > 0) {
            printf("MDS started as pid %d\n", pid);
            return 0;
        }

        //return start_server();
    }

    idx = lookupcmd(argv[1]);
    if (idx == -1) {
        printf("Unknown command '%s'\n", argv[1]);
        cmd_help(0, NULL);
        return 1;
    }

    return commands[idx].cmd(argc-1, (const char **)argv+1);
}

