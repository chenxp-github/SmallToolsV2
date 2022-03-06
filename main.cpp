#include "sys_log.h"
#include "mem_tool.h"
#include "mem.h"
#include "memfile.h"
#include "memstk.h"
#include "file.h"
#include "misc.h"
#include "mem_tool.h"
#include "sys_log.h"
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
    printf("process %d:catch signal %d, exit\n",getpid(),signatl);
    g_abnormal_exit = 1;

    //first time, just quitMainLoop, after this use old signal handler
    if(g_globals_ptr && g_globals_ptr->m_MainLoopRunning)
    {
        g_globals_ptr->QuitMainLoop();    
    }
    else
    {
        //default signal handler
        signal(signatl, SIG_DFL);
        kill(getpid(),signatl);
    }
}
#endif

int main(int argc, char **argv)
{
#if (!HAVE_WINDOWS_H)
    signal(SIGINT, on_terminate);
    signal(SIGTERM,on_terminate); 
    signal(SIGHUP,on_terminate); 
    signal(SIGPIPE,SIG_IGN);
#endif

#if _DEBUG_
    Mem_Tool_Init("/tmp/leak.txt");
#endif

    CSocket::StartNet();
    g_globals.Init();
    g_globals_ptr = &g_globals;
    g_argc = argc;
    g_argv = argv;  
    int ok = g_globals.Main(argc, argv);
    g_globals.Destroy();
    CSocket::EndNet();

#if _DEBUG_
    Mem_Tool_Destroy();
#endif

    if(g_abnormal_exit)
        return 2;

    return ok ? 0:1;
}

