#include "syslog.h"
#include "mem_tool.h"
#include "mem.h"
#include "memfile.h"
#include "memstk.h"
#include "file.h"
#include "misc.h"
#include "mem_tool.h"
#include "syslog.h"
#include "globals.h"
#include "commandline.h"
#include "mmapfile.h"
#include "fileutils.h"
#include "simpledisk.h"
#include "tcp.h"

int g_argc = 0;
char **g_argv = NULL;

int main(int argc, char **argv)
{
    CSocket::StartNet();
    g_globals.Init();
    g_globals_ptr = &g_globals;
    g_argc = argc;
    g_argv = argv;
    int ok = g_globals.Main(argc, argv);
    g_globals.Destroy();
    CSocket::EndNet();
    return ok ? 0:1;
}

