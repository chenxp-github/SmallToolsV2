#include "x_folder_differ.h"
#include "sys_log.h"
#include "mem_tool.h"
#include "dirmgr.h"
#include "file.h"

CxFolderDiffer::CxFolderDiffer()
{
    this->InitBasic();
}

CxFolderDiffer::~CxFolderDiffer()
{
    this->Destroy();
}

status_t CxFolderDiffer::InitBasic()
{
/*##Begin InitBasic##*/
/*##End InitBasic##*/
    m_flags = 0;
    return OK;
}

status_t CxFolderDiffer::Init()
{
    this->InitBasic();
/*##Begin Init##*/
/*##End Init##*/
    this->SetFollowLinks(true);
    this->SetRecursive(true);
    return OK;
}

status_t CxFolderDiffer::Destroy()
{
/*##Begin Destroy##*/
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

/*@@ Insert Function Here @@*/
status_t CxFolderDiffer::ReadFinalLink(const char *name, CMem *out)
{
    ASSERT(name && out);

    status_t is_link = FALSE;
    out->SetSize(0);
    out->Puts(name);

#if _IS_LINUX_
    char buf[LBUF_SIZE];
    LOCAL_MEM(cur_dir);
    int level = 0;
    while(level++ < 10)
    {
        CDirMgr::GetFileName(out,&cur_dir,FN_PATH);
        int n = readlink(out->CStr(),buf,sizeof(buf));
        if(n >= 0)
        {
            is_link = TRUE;
            out->SetSize(0);
            out->Write(buf,n);
            if(!CDirMgr::IsAbsPath(out))
            {
                CDirMgr::ToAbsPath(&cur_dir,out);
            }
        }
        else
        {
            break;            
        }
    }
#endif

    return is_link;
}

int64_t CxFolderDiffer::GetFileModifyTime(CMem *fn)
{
    ASSERT(fn);
    struct stat statbuf;
	memset(&statbuf,0,sizeof(statbuf));
    lstat(fn->CStr(),&statbuf);
    return statbuf.st_mtim.tv_sec*1000LL+statbuf.st_mtim.tv_nsec/1000000LL;    
}

status_t CxFolderDiffer::GetFolderInfos(const char *folder, CxFileInfoList *list)
{
    ASSERT(folder && list);

    BEGIN_CLOSURE(on_search_dir)
    {
        CLOSURE_PARAM_INT(event,0);
        CLOSURE_PARAM_PTR(char*,file_name,1);
        CLOSURE_PARAM_PTR(char*,full_name,2);
        CLOSURE_PARAM_INT(is_dir,3);
        CLOSURE_PARAM_INT64(size,4);
        CLOSURE_PARAM_INT64(last_write_time,5);
        CLOSURE_PARAM_PTR(const char*, root_dir,10);
        CLOSURE_PARAM_PTR(CxFileInfoList*,list,11);
        CLOSURE_PARAM_PTR(CxFolderDiffer*,self,12);
        
        const char *path = full_name + strlen(root_dir);
        if(path[0] == '/' || path[0] == '\\')
            path++;

        CxFileInfo info;
        info.Init();
        info.SetModifyTime(last_write_time);
        info.SetIsDir(is_dir);
        info.SetFullPath(path);
        info.SetSize(size);
      
        if(event == CDirMgr::EVENT_BEGIN_DIR)        
        {            
            list->Push(&info);
        }
        else if(event == CDirMgr::EVENT_SINGLE_FILE)
        {
            LOCAL_MEM(link);
            if(self->FollowLinks() && ReadFinalLink(full_name,&link))
            {
                info.SetSize(CDirMgr::GetFileSize(&link));
                info.SetModifyTime(CxFolderDiffer::GetFileModifyTime(&link));
            }
            list->Push(&info);
        }
        return OK;
    }
    END_CLOSURE(on_search_dir);

    LOCAL_MEM(mem);
    mem.Puts(folder);
    CDirMgr::ToAbsPath(&mem);

    
    on_search_dir.SetParamPointer(10,(void*)mem.CStr());
    on_search_dir.SetParamPointer(11,list);        
    on_search_dir.SetParamPointer(12,this);

    CDirMgr::SearchDir(&mem,this->Recursive(),&on_search_dir);
    return OK;
}

status_t CxFolderDiffer::SaveFolderSnapshot(CxFileInfoList *list, const char *fn)
{
    ASSERT(list && fn);
    CMiniBson bson;
    bson.Init();
    bson.StartDocument();
    list->SaveBson(&bson);
    bson.EndDocument();
    return bson.GetRawData()->WriteToFile(fn) > 0;
}

status_t CxFolderDiffer::LoadFolderSnapshot(const char *bson_fn, CxFileInfoList *list)
{
    ASSERT(bson_fn && list);
    CFile file;
    file.Init();
    if(!file.OpenFile(bson_fn,"rb"))
        return ERROR;
    return list->LoadBson(&file);
}

static bool file_changed(CxFileInfo *left, CxFileInfo *right)
{
    ASSERT(left && right);
    if(left == right)
        return false;
    if(left->GetSize() != right->GetSize())
        return true;
    if(left->GetModifyTime() != right->GetModifyTime())
        return true;
    return false;
}

status_t CxFolderDiffer::CompTwoFolder(CxFileInfoList *left, CxFileInfoList *right, CxFileInfoList *create_list, CxFileInfoList *delete_list)
{
    ASSERT(left && right && create_list && delete_list);

    int i;

    left->Sort(0);
    right->Sort(0);

    for(i = 0; i < left->GetLen(); i++)
    {
        CxFileInfo *l_info = left->GetElem(i);
        CxFileInfo *r_info = right->BSearchNode(l_info,0);

        if(r_info == NULL)
        {
            CxFileInfo tmp;
            tmp.Init();
            tmp.Copy(l_info);
            tmp.SetOperation(CxFileInfo::OPERATION_CREATE);
            create_list->InsOrdered(&tmp,0,1);
        }
        else if(file_changed(l_info,r_info) && (!l_info->IsDir()))
        {   
            CxFileInfo tmp;
            tmp.Init();
            tmp.Copy(l_info);
            tmp.SetOperation(CxFileInfo::OPERATION_OVERWRITE);
            create_list->InsOrdered(&tmp,0,1);
        }
    }

    for(i = 0; i < right->GetLen(); i++)
    {
        CxFileInfo *r_info = right->GetElem(i);
        CxFileInfo *l_info = left->BSearchNode(r_info,0);
        if(l_info == NULL)
        {
            CxFileInfo tmp;
            tmp.Init();
            tmp.Copy(r_info);
            tmp.SetOperation(CxFileInfo::OPERATION_DELETE);
            delete_list->InsOrdered(&tmp,1,1);
        }
    }

    return OK;
}