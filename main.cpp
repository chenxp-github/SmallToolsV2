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
int g_abnormal_exit = 0;

#if (!HAVE_WINDOWS_H)
void on_terminate(int signatl) 
{
    printf("process %d:catch signal %d\n",getpid(),signatl);
    g_globals.QuitMainLoop();
    g_abnormal_exit = 1;
}
#endif

int main(int argc, char **argv)
{
#if (!HAVE_WINDOWS_H)
    signal(SIGINT, on_terminate);
    signal(SIGTSTP,on_terminate);    
    signal(SIGTERM,on_terminate); 
#endif

    CSocket::StartNet();
    g_globals.Init();
    g_globals_ptr = &g_globals;
    g_argc = argc;
    g_argv = argv;  
    int ok = g_globals.Main(argc, argv);
    g_globals.Destroy();
    CSocket::EndNet();

    if(g_abnormal_exit)
        return 2;

    return ok ? 0:1;
}

