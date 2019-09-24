#include "mkdisk.h"
#include "syslog.h"
#include "mem_tool.h"
#include "fileinfolist.h"
#include "dirmgr.h"
#include "fileutils.h"

status_t MakeOrderSimpleDiskFromDir(const char *dir,CSimpleDisk *disk)
{
    ASSERT(dir && disk);
    
    LOCAL_MEM(abs_path);
    
    CFileInfoList filelist;
    filelist.Init(1024);
    filelist.SearchDir(dir,0,0);
    filelist.Sort(0);
    
    for(int i = 0; i < filelist.GetLen(); i++)
    {
        CFileInfo *info = filelist.GetElem(i);
        ASSERT(info);
        
        if(info->IsDir())
        {
            abs_path.SetSize(0);
            abs_path.Puts(dir);
            abs_path.Putc(crt_get_path_splitor());
            abs_path.Puts(info->GetName());
            
            fsize_t offset;
            disk->BeginFolder(info->GetName(),&offset);
            MakeOrderSimpleDiskFromDir(abs_path.CStr(),disk);
            disk->EndFolder(offset);
        }
        else
        {
            abs_path.StrCpy(info->GetName());
            CDirMgr::ToAbsPath(dir,&abs_path);
            LOCAL_DISK_FILE(file,abs_path.CStr(),"rb",true,true);
            disk->AddFile(info->GetName(),&file);
        }   
    }
    return OK;
}
