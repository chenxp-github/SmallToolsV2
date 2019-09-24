// DirMgr.cpp: implementation of the CDirMgr class.
//
//////////////////////////////////////////////////////////////////////
#include "dirmgr.h"
#include "memfile.h"
#include "file.h"
#include "memstk.h"
#include "syslog.h"
#include "mem_tool.h"

#define _MAX_PATH_ (LBUF_SIZE*2)

//////////////////////////////////////////////////////////////////////
#if _ASC_
//////////////////////////////////////////////////////////////////////
status_t CDirMgr::GetFileName(CMem *fullname,CFileBase *file, uint32_t filter)
{
    fsize_t i,j,k,len,dot,slash,dot_pos;
    char ext[_MAX_PATH_],single[_MAX_PATH_];
    char path[_MAX_PATH_];
    char separator = GetPathSplitor();
    const char *line = fullname->CStr();
    len = fullname->StrLen();
    file->SetSize(0);
    if(len <= 0 || len > _MAX_PATH_)
        return ERROR;

    dot_pos = len;
    if(filter == FN_FULLNAME)
    {
        file->Puts(line);
        return OK;
    }
    dot = FALSE;
    for(i = len - 1; i >= 0; i--)
    {
        if(line[i] == '.')
        {
            dot = TRUE;
            dot_pos = i;
            break;
        }
        if(line[i] == '/' || line[i] == '\\')
        {
            separator = line[i];
            break;
        }
    }

    k = 0;
    if(dot == TRUE)
    {
        for(j = i + 1; j < len; j++)
        {
            ext[k++] = line[j];
        }
    }
    ext[k] = 0;
    ////////////////////////////
    slash = FALSE;
    for(i = len - 1; i >= 0; i--)
    {
        if(line[i] == '/' || line[i] == '\\')
        {
            separator = line[i];
            slash = TRUE;
            break;
        }
    }
    k = 0;
    if(slash)
    {
        for(j = 0; j < i; j++)
        {
            path[k++] = line[j];
        }
    }
    path[k] = 0;
    ////////////////////////
    k = 0;
    for(j = i + 1; j <dot_pos; j++)
    {
        single[k++] = line[j];
    }

    single[k] = 0;
    ///////////////////////////////////
    if(filter & FN_PATH)
        file->Puts(path);
    if(filter & FN_SINGLE)
    {
        if(file->GetOffset() > 0)
            file->Putc(separator);
        file->Puts(single);
    }
    if(filter & FN_EXT)
    {
        if(file->GetOffset() > 0 && dot)
        {
            file->Putc('.');
        }
        file->Puts(ext);
    }
    return OK;
}

status_t CDirMgr::GetFileName(const char *line,CFileBase *file, uint32_t filter)
{
    CMem mem(line);
    return GetFileName(&mem,file,filter);
}

status_t CDirMgr::SearchDir_Interruptable(CMem *dir, bool recursive, CClosure *closure, int *running)
{
    ASSERT(dir);
    if(running && !(*running))
        return OK;

    bool separator_ended = false;
    int_ptr_t len = dir->StrLen();
    ASSERT(len > 0);

    if(dir->GetLastChar(true) == crt_get_path_splitor())
        separator_ended = 1;

    char filename[_MAX_PATH_];
    char old_path[_MAX_PATH_];
    void *p[32];

    LOCAL_MEM_WITH_SIZE(fullname,_MAX_PATH_);

    crt_get_cur_dir(old_path, _MAX_PATH_);
    if(crt_need_chdir_before_open())
        crt_chdir(dir->CStr());

    p[1] = filename;
    if(!crt_open_dir(dir->CStr(),p))
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "open dir '%s' fail.",dir->CStr()
        );
        return ERROR;
    }
    while(crt_read_dir(p))
    {
        if(running && !(*running))break;
        if(strcmp(filename,".") == 0)continue;
        if(strcmp(filename,"..") == 0)continue;

        bool is_dir = (p[2] != 0);

        fullname.StrCpy(dir);
        if(!separator_ended)
            fullname.Putc(crt_get_path_splitor());
        fullname.Puts(filename);

        int64_t size,last_write_time = 0;
        memcpy(&size,&p[3],sizeof(size));
        memcpy(&last_write_time,&p[5],sizeof(last_write_time));

        closure->SetParamPointer(1,filename);
        closure->SetParamPointer(2,(void*)fullname.CStr());
        closure->SetParamInt(3,is_dir);
        closure->SetParamInt64(4,size);
        closure->SetParamInt64(5,last_write_time);

        if(!is_dir)
        {
            closure->Run(EVENT_SINGLE_FILE);
        }
        else
        {
			closure->Run(EVENT_BEGIN_DIR);

            if(recursive)
            {
                SearchDir_Interruptable(&fullname,true,closure,running);
            }
            //set again, recursive call may change closure value
            closure->SetParamPointer(1,filename);
            closure->SetParamPointer(2,(void*)fullname.CStr());
            closure->SetParamInt(3,is_dir);
            closure->SetParamInt64(4,size);
            closure->SetParamInt64(5,last_write_time);
            closure->Run(EVENT_END_DIR);
        }
    }

    crt_close_dir(p);
    if(crt_need_chdir_before_open())
        crt_chdir(old_path);

    return OK;
}
status_t CDirMgr::SearchDir(CMem *dir, bool recursive, CClosure *closure)
{
    return SearchDir_Interruptable(dir,recursive,closure,NULL);
}
status_t CDirMgr::SearchDir(const char *dir, bool recursive, CClosure *closure)
{
    CMem mem(dir);
    return SearchDir(&mem,recursive,closure);
}
status_t CDirMgr::GetCurDir(CMem *dir)
{
    ASSERT(dir);
    int_ptr_t max = (int_ptr_t)(dir->GetMaxSize()-1);
    int_ptr_t size = (int_ptr_t)crt_get_cur_dir(dir->GetRawBuf(),max);
    if(size > 0)
    {
        dir->SetSize(size);
        return OK;
    }
    return ERROR;
}

status_t CDirMgr::ToAbsPath(CMem *prefix,CFileBase *file)
{
    ASSERT(file);

    LOCAL_MEM(buf);

    buf.Puts(prefix);
    buf.Putc(GetPathSplitor());
    buf.Puts(file);

    CMemStk stk;
    stk.Init(256);
    stk.LoadPath(&buf);

    int i;
    CMem *pmem;
    file->SetSize(0);
    for(i = 0; i < stk.GetLen(); i++)
    {
        pmem = stk.GetElem(i);
        ASSERT(pmem);
        if(pmem->CharAt(1) != ':')
            file->Putc(GetPathSplitor());
        file->Puts(pmem);
    }

    return OK;
}

status_t CDirMgr::ToAbsPath(const char *prefix,CFileBase *file)
{
    CMem mem(prefix);
    return ToAbsPath(&mem,file);
}

bool CDirMgr::IsAbsPath(CFileBase *dir)
{
    ASSERT(dir);

    if(dir->CharAt(1) == ':')
    {
        return true;
    }
    else if(dir->CharAt(0) == '\\' || dir->CharAt(0) =='/')
    {
        return true;
    }

    return false;
}
bool CDirMgr::IsAbsPath(const char *dir)
{
    ASSERT(dir);

    if(dir[1] == ':')
    {
        return true;
    }
    else if(dir[0] == '\\' || dir[0] =='/')
    {
        return true;
    }

    return false;
}
status_t CDirMgr::ToAbsPath(CFileBase *file)
{
    ASSERT(file);

    LOCAL_MEM_WITH_SIZE(mem,_MAX_PATH_);

    if(IsAbsPath(file))
    {
        mem.StrCpy("");
    }
    else
    {
        GetCurDir(&mem);
    }

    return ToAbsPath(mem.CStr(),file);
}

status_t CDirMgr::IsDirExist(CMem *dir)
{
    char tmp[_MAX_PATH_];
    void *p[32];

    p[1] = tmp;
    if(crt_open_dir(dir->CStr(),p))
    {
        crt_close_dir(p);
        return TRUE;
    }

    return FALSE;
}
status_t CDirMgr::IsFileExist(CMem *file)
{
    ASSERT(file);
    FILE_HANDLE fp;
    fp = crt_fopen(file->CStr(),"rb");
    if(!crt_is_file_handle(fp))
        return FALSE;
    crt_fclose(fp);
    return TRUE;
}
char CDirMgr::GetPathSplitor()
{
    return crt_get_path_splitor();
}
status_t CDirMgr::CreateDir(CMem *dir)
{
    ASSERT(dir);
    return crt_mkdir(dir->CStr());
}
status_t CDirMgr::DeleteFile(CMem *filename)
{
    ASSERT(filename);
    return crt_unlink(filename->CStr());
}
status_t CDirMgr::DeleteDir(CMem *dir)
{
    ASSERT(dir);
    return crt_rmdir(dir->CStr());
}
status_t CDirMgr::DeleteDir(const char *dir)
{
    CMem mem(dir);
    return DeleteDir(&mem);
}

status_t CDirMgr::CreateDirSuper(CMem *dir)
{
    ASSERT(dir);

    LOCAL_MEM(path);
    LOCAL_MEM(buf);

    dir->SetSplitChars(":\\/");
    dir->Seek(0);

    if(dir->CharAt(1) == ':')
    {
        path.Putc(dir->Getc());
        path.Putc(dir->Getc());
    }

    while(dir->ReadString(&buf))
    {
        char str[2];
        str[0] = GetPathSplitor();
        str[1] = 0;
        path.StrCat(str);
        path.StrCat(&buf);
        if(!IsDirExist(&path))
        {
            ASSERT(CreateDir(&path));
        }
    }
    return OK;
}
status_t CDirMgr::CreateFilePath(CMem *fullname)
{
    ASSERT(fullname);
    LOCAL_MEM(path);
    GetFileName(fullname,&path,FN_PATH);
    return CreateDirSuper(&path);
}
status_t CDirMgr::SaveFileSuper(CMem *fullname, CFileBase *data)
{
    ASSERT(fullname && data);
    if(CreateFilePath(fullname))
    {
        if(data->WriteToFile(fullname->CStr()) >= 0)
            return OK;
    }
    return ERROR;
}
status_t CDirMgr::CopyFileSuper(CMem *from, CMem *to)
{
    ASSERT(from && to);
    ASSERT(CreateFilePath(to));
    CFile file;
    ASSERT(file.OpenFile(from->CStr(),"rb"));
    file.WriteToFile(to->CStr());
    return OK;
}

status_t CDirMgr::IsPathSplitor(char ch)
{
    return ch == '/' || ch == '\\';
}

fsize_t CDirMgr::GetFileSize(CMem *filename)
{
    ASSERT(filename);

    FILE_HANDLE fp = crt_fopen(filename->CStr(),"rb");
    if(!crt_is_file_handle(fp))
        return 0;
    crt_fseek(fp,0,SEEK_END);
    fsize_t size = crt_ftell(fp);
    crt_fclose(fp);
    return size;
}

status_t CDirMgr::StdPathSplitor(CMem *str)
{
    ASSERT(str);
	ASSERT(!str->IsReadOnly());

    int_ptr_t i,len = str->StrLen();
    char *p = str->GetRawBuf();
    char sp = crt_get_path_splitor();
    for(i = 0; i < len; i++)
    {
        if(p[i] == '/' || p[i] =='\\')
            p[i] = sp;
    }
    return OK;
}
//////////////////////////////////////////////////////////////////////
#endif  //#if _ASC_
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
#if _UNICODE_
//////////////////////////////////////////////////////////////////////
status_t CDirMgr::GetFileNameW(CMem *line,CFileBase *file, uint32_t filter)
{
    int_ptr_t i,j,k,len,dot,slash,dot_pos;
    wchar_t ext[_MAX_PATH_],single[_MAX_PATH_];
    wchar_t path[_MAX_PATH_];
    wchar_t separator = GetPathSplitorW();
    len = line->StrLenW();
    const wchar_t *p = line->CStrW();

    file->SetSize(0);
    if(len <= 0 || len > _MAX_PATH_)
        return ERROR;

    dot_pos = len;
    if(filter == FN_FULLNAME)
    {
        file->PutsW(p);
        return OK;
    }
    dot = FALSE;
    for(i = len - 1; i >= 0; i--)
    {
        if(p[i] == L'.')
        {
            dot = TRUE;
            dot_pos = i;
            break;
        }
        if(p[i] == L'/' || p[i] == L'\\')
        {
            separator = p[i];
            break;
        }
    }

    k = 0;
    if(dot == TRUE)
    {
        for(j = i + 1; j < len; j++)
        {
            ext[k++] = p[j];
        }
    }
    ext[k] = 0;

    slash = FALSE;
    for(i = len - 1; i >= 0; i--)
    {
        if(p[i] == L'/' || p[i] == L'\\')
        {
            separator = p[i];
            slash = TRUE;
            break;
        }
    }
    k = 0;
    if(slash)
    {
        for(j = 0; j < i; j++)
        {
            path[k++] = p[j];
        }
    }
    path[k] = 0;

    k = 0;
    for(j = i + 1; j <dot_pos; j++)
    {
        single[k++] = p[j];
    }

    single[k] = 0;

    if(filter & FN_PATH)
        file->PutsW(path);
    if(filter & FN_SINGLE)
    {
        if(file->GetOffset() > 0)
            file->PutcW(separator);
        file->PutsW(single);
    }
    if(filter & FN_EXT)
    {
        if(file->GetOffset() > 0 && dot)
        {
            file->PutcW(L'.');
        }
        file->PutsW(ext);
    }
    return OK;
}

wchar_t CDirMgr::GetPathSplitorW()
{
    return crt_get_path_splitor();
}

status_t CDirMgr::ToAbsPathW(CMem *prefix,CFileBase *file)
{
    ASSERT(file);

    LOCAL_MEM(mem);
    LOCAL_MEM(buf);

    buf.PutsW(prefix);
    buf.PutcW(GetPathSplitorW());
    buf.PutsW(file);

    CMemStk stk;
    stk.Init(256);
    buf.SetSplitCharsW(L"/\\");
    buf.Seek(0);
    while(buf.ReadStringW(&mem))
    {
        if(mem.StrICmpW(L"..") == 0)
        {
            stk.DelTop();
        }
        else if(mem.StrICmpW(L".") != 0)
        {
            stk.Push(&mem);
        }
    }

    int i;
    CMem *pmem;
    file->SetSize(0);
    for(i = 0; i < stk.GetLen(); i++)
    {
        pmem = stk.GetElem(i);
        ASSERT(pmem);
        if(pmem->CharAtW(1) != L':')
            file->PutcW(GetPathSplitorW());
        file->PutsW(pmem);
    }
    return OK;
}
status_t CDirMgr::ToAbsPathW(CFileBase *file)
{
    LOCAL_MEM(mem);

    if(file->CharAtW(1) == L':')
    {
        return OK;
    }
    else if(file->CharAtW(0) == L'\\' || file->CharAtW(0) == L'/')
    {
        return OK;
    }
    GetCurDirW(&mem);
    return ToAbsPathW(&mem,file);
}
status_t CDirMgr::IsDirExistW(CMem *dir)
{
    ASSERT(dir);

    wchar_t tmp[_MAX_PATH_];
    void *p[32];
    p[1] = tmp;
    if(crt_open_dir_w(dir->CStrW(),p))
    {
        crt_close_dir(p);
        return TRUE;
    }
    return FALSE;
}

status_t CDirMgr::GetCurDirW(CMem *dir)
{
    ASSERT(dir);
    int_ptr_t max = (int_ptr_t)((dir->GetMaxSize()-1)*sizeof(wchar_t));
    int_ptr_t size = crt_get_cur_dir_w((wchar_t*)dir->GetRawBuf(),max);
    if(size > 0)
    {
        dir->SetSize(size*sizeof(wchar_t));
        return OK;
    }
    return OK;
}

//////////////////////////////////////////////////////////////////////
#endif  //#if _UNICODE_
//////////////////////////////////////////////////////////////////////
