#include "simpledisk.h"
#include "syslog.h"
#include "mem_tool.h"
#include "encoder.h"
#include "file.h"
#include "dirmgr.h"
#include "memstk.h"

struct file_info{
    int64_t index;
    char md5[36];
};

static status_t get_file_info(CMiniBson *bson,struct file_info *info)
{
    status_t ret = OK;
    
    if(!bson->GetInt64("index",&info->index))
    {
        info->index = -1;
        ret = ERROR;
    }
    
    CMem mem;
    if(bson->GetString("md5",&mem))
    {
        memcpy(info->md5,mem.GetRawBuf(),32);
        info->md5[32] = 0;
        ret = ERROR;
    }
   
    return ret;
}

CSimpleDisk::CSimpleDisk()
{
    this->InitBasic();
}
CSimpleDisk::~CSimpleDisk()
{
    this->Destroy();
}
status_t CSimpleDisk::InitBasic()
{
    WEAK_REF_CLEAR();
    this->mFileSystem = NULL;
    this->mIndexFile = NULL;
    this->iDstFile = NULL;
    this->mFastFsIndex = NULL;
    return OK;
}
status_t CSimpleDisk::Init()
{
    this->InitBasic();

    NEW(this->mFileSystem,CMiniBson);
    this->mFileSystem->Init();
    this->mFileSystem->AllocBuf(64*1024);

    NEW(this->mIndexFile,CIndexFile);
    this->mIndexFile->Init();
    
    NEW(mFastFsIndex,SimpleDiskHashFile);
    mFastFsIndex->Init();

    return OK;
}
status_t CSimpleDisk::Destroy()
{
    WEAK_REF_DESTROY();
    DEL(this->mFastFsIndex);
    DEL(this->mFileSystem);
    DEL(this->mIndexFile);
    this->InitBasic();
    return OK;
}

status_t CSimpleDisk::SetDstFile(CFileBase* iFile)
{
    this->iDstFile = iFile;
    this->mIndexFile->SetDstFile(this->iDstFile);
    this->mFileSystem->StartDocument();
    return OK;
}

status_t CSimpleDisk::AddFile(const char * name, CFileBase *file)
{
    ASSERT(file);

    LOCAL_MEM(mem);
    mem.StrCpy(name);
    CEncoder::EncodingConvert(LOCAL_ENCODING,ENCODING_UTF8,&mem);

    LOCAL_MEM(md5);
    CEncoder::Md5Encode(file,&md5);

    fsize_t off;
    this->mFileSystem->StartDocument(mem.CStr(),&off);
    this->mFileSystem->PutInt64("index",this->mIndexFile->GetBlockCount());
    this->mFileSystem->PutString("md5",&md5);
    this->mFileSystem->EndDocument(off);

    this->mIndexFile->AddBlock(file);
    return OK;
}

status_t CSimpleDisk::Save(void)
{
    this->AddFastFsIndex();
    this->mFileSystem->EndDocument();
    this->mIndexFile->AddBlock(this->mFileSystem->GetRawData());
    this->mIndexFile->Save();
    return OK;
}

#define STACK_SIZE 1024
struct add_folder_context{
    fsize_t stack[STACK_SIZE];
    int level;
    CSimpleDisk *self;
};

static status_t _add_folder(CClosure *closure)
{
    CLOSURE_PARAM_INT(e,0);
    CLOSURE_PARAM_PTR(const char*,filename,1);
    CLOSURE_PARAM_PTR(struct add_folder_context *,pc,10);
    ASSERT(filename && pc);

    CSimpleDisk *self = pc->self;

    if(e == CDirMgr::EVENT_BEGIN_DIR)
    {
        LOCAL_MEM(mem);
        mem.StrCpy(filename);
        CEncoder::EncodingConvert(LOCAL_ENCODING,ENCODING_UTF8,&mem);
        fsize_t off;
        self->mFileSystem->StartDocument(mem.CStr(),&off);
        ASSERT(pc->level >= 0 && pc->level < STACK_SIZE);
        pc->stack[pc->level] = off;
        pc->level++;
    }

    else if(e == CDirMgr::EVENT_SINGLE_FILE)
    {
        CLOSURE_PARAM_PTR(const char*,fullname,2);
        ASSERT(fullname);
        CFile file;
        file.Init();
        file.SetBufSize(1024*1024);

        if(file.OpenFile(fullname,"rb"))
        {
            self->AddFile(filename,&file);
            //XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,
            //  "add file %s",fullname
            //);
        }
        else
        {
            XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
                "simple disk add file error: %s",fullname
            );
        }
    }

    else if(e == CDirMgr::EVENT_END_DIR)
    {
        ASSERT(pc->level > 0 && pc->level <= STACK_SIZE);
        pc->level--;
        fsize_t off = pc->stack[pc->level];
        self->mFileSystem->EndDocument(off);
    }

    return TRUE;
}

status_t CSimpleDisk::AddFolder(CMem * dir)
{
    ASSERT(dir);
    struct add_folder_context context;
    context.level = 0;
    context.self = this;

    CClosure closure(_add_folder);
    closure.SetParamPointer(10,&context);
    CDirMgr::SearchDir(dir,true,&closure);

    return OK;
}

status_t CSimpleDisk::AddFolder(const char *dir)
{
    CMem mem(dir);
    return this->AddFolder(&mem);
}

status_t CSimpleDisk::LoadDiskImage(CFileBase * file)
{
    ASSERT(file);
    ASSERT(this->mIndexFile->LoadIndexFile(file));
    int64_t max = this->mIndexFile->GetBlockCount();
    ASSERT(max > 0);
    fsize_t fs_size = this->mIndexFile->GetBlockSize(max-1);
    this->mFileSystem->AllocBuf((int_ptr_t)fs_size);
    CMem *data = this->mFileSystem->GetRawData();
    ASSERT(data);
    this->mIndexFile->GetBlock(max-1,data);
    this->mFileSystem->LoadBson();

    if(!this->LoadFastFsIndex())
    {
        ASSERT(this->RebuildFastIndex());
    }
    return OK;
}

int64_t CSimpleDisk::GetFileSize(int64_t index)
{
    return this->mIndexFile->GetBlockSize(index);
}

status_t CSimpleDisk::GetFile(int64_t index , CFileBase * file)
{
    return this->mIndexFile->GetBlock(index,file);
}

status_t CSimpleDisk::GhostFile(int64_t index, CPartFile* file)
{
    return this->mIndexFile->GhostBlock(index,file);
}

static status_t _extract_all_files(CClosure *closure)
{
    CLOSURE_PARAM_INT(is_folder,0);

    if(!is_folder)
    {
        CLOSURE_PARAM_PTR(CMem*,destDir,11);
        CLOSURE_PARAM_PTR(CMemStk*,path,2);

        LOCAL_MEM(path_str);
        LOCAL_MEM(full);
        path->SavePath(&path_str);
        full.Puts(destDir);
        CEncoder::EncodingConvert(ENCODING_UTF8,LOCAL_ENCODING,&path_str);
        full.Puts(&path_str);
        CDirMgr::StdPathSplitor(&full);

        //XLOG(LOG_MODULE_COMMON,LOG_LEVEL_INFO,
        //  "saving file %s",full.CStr()
        //);

        CLOSURE_PARAM_PTR(CPartFile*,file,3);
        CDirMgr::SaveFileSuper(&full,file);
    }
    return OK;
}

status_t CSimpleDisk::ExtractAllFiles(CMem * destDir)
{
    ASSERT(destDir);    
    CClosure closure(_extract_all_files);
    closure.SetParamPointer(10,this);
    closure.SetParamPointer(11,destDir);
    this->Reset();
    this->SearchDirectory(&closure);
    return OK;
}
status_t CSimpleDisk::ExtractAllFiles(const char * destDir)
{
    CMem mem(destDir);
    return this->ExtractAllFiles(&mem);
}

static status_t _search_directory(CClosure *closure)
{   
    CLOSURE_PARAM_INT(e,CMiniBson::INDEX_EVENT);
    CLOSURE_PARAM_INT(type,CMiniBson::INDEX_TYPE);
    CLOSURE_PARAM_PTR(char*,name,CMiniBson::INDEX_NAME);
    CLOSURE_PARAM_PTR(CMemStk*,path,CMiniBson::INDEX_PATH);
    CLOSURE_PARAM_PTR(CSimpleDisk*,self,10);
    CLOSURE_PARAM_PTR(CClosure*,user,11);

    if(e == CMiniBson::EVENT_END_DOCUMENT)
    {
        if(type == CMiniBson::BSON_TYPE_DOCUMENT)
        {
            CLOSURE_PARAM_PTR(CMiniBson*,doc,CMiniBson::INDEX_VALUE);

            struct file_info info;
            get_file_info(doc,&info);

            if(info.index >= 0)
            {
                CPartFile pfile;
                pfile.Init();
                self->GhostFile(info.index,&pfile);

                user->SetParamInt(0,0); //file
                user->SetParamPointer(1,name);
                user->SetParamPointer(2,path);
                user->SetParamPointer(3,&pfile);
                user->SetParamInt64(4,info.index);
                user->SetParamPointer(5,info.md5);
                user->Run();
            }
            else
            {
                user->SetParamInt(0,1); //folder
                user->SetParamPointer(1,name);
                user->SetParamPointer(2,path);
                user->Run();
            }
        }
    }
    return OK;
}

status_t CSimpleDisk::SearchDirectory(CClosure *closure)
{
    ASSERT(closure);
    CClosure tmp_closure(_search_directory);
    tmp_closure.SetParamPointer(10,this);
    tmp_closure.SetParamPointer(11,closure);

    CMemStk path;
    path.Init(1024);

    tmp_closure.SetParamInt(CMiniBson::INDEX_IS_IN_ARRAY,0);
    tmp_closure.SetParamPointer(CMiniBson::INDEX_PATH,&path);
    this->mFileSystem->Traverse(1,0,&tmp_closure);
    return OK;
}

status_t CSimpleDisk::Reset(void)
{
    return this->mFileSystem->ResetPointer();
}

status_t CSimpleDisk::BeginFolder(const char *name, fsize_t *offset)
{
    ASSERT(name && offset);
    LOCAL_MEM(mem);
    mem.StrCpy(name);
    CEncoder::EncodingConvert(LOCAL_ENCODING,ENCODING_UTF8,&mem);
    return mFileSystem->StartDocument(mem.CStr(),offset);
}

status_t CSimpleDisk::EndFolder(fsize_t offset)
{
    return mFileSystem->EndDocument(offset);
}
status_t CSimpleDisk::GetFileOffsetAndSize(int64_t index, int64_t *offset, int64_t *size)
{
    ASSERT(offset && size);
    *offset = -1; *size = 0;
    *offset = this->mIndexFile->GetBlockOffset(index);
    if(*offset < 0) return ERROR;
    *size = this->mIndexFile->GetBlockSize(index);
    return OK;
}

status_t CSimpleDisk::LoadFastFsIndex()
{
    int64_t max = this->mIndexFile->GetBlockCount();
    ASSERT(max > 2);    
    return LoadFastFsIndex(max-2);
}

status_t CSimpleDisk::LoadFastFsIndex(int64_t index)
{
    mFastFsIndex->Destroy();
    mFastFsIndex->Init();
    
    CPartFile part_file;
    if(!mIndexFile->GhostBlock(index,&part_file))
        return ERROR;

    if(!mFastFsIndex->LoadHashFileGhost(&part_file))
        return ERROR;
    return OK;
}

status_t CSimpleDisk::GetFsEntry(const char *_filename,CSimpleDiskFsEntry *entry)
{
    ASSERT(_filename);
    
    const char *filename = _filename;
    LOCAL_MEM(mem);
    
    if(_filename[0] != '/')
    {
        mem.Puts("/");
        mem.Puts(_filename);
        filename = mem.CStr();
    }
    
    CSimpleDiskFsEntry tmp;
    tmp.Init();
    tmp.SetFileName(filename);
    return mFastFsIndex->Search(&tmp,entry);
}

status_t CSimpleDisk::BuildFastFsIndex(CFileBase *out)
{
    ASSERT(out);

    this->Reset();

    BEGIN_CLOSURE(build_index)
    {
        CLOSURE_PARAM_INT(is_folder,0);
        if(is_folder)return OK;

        CLOSURE_PARAM_PTR(char*,name,1);
        CLOSURE_PARAM_PTR(CMemStk*,path,2);
        CLOSURE_PARAM_INT64(index,4);
        CLOSURE_PARAM_PTR(char*,md5,5);
        CLOSURE_PARAM_PTR(CSimpleDiskFsIndex*,map,10);
        
        LOCAL_MEM(path_str);
        path->SavePath(&path_str);
    
        CSimpleDiskFsEntry *ptr;
        NEW(ptr,CSimpleDiskFsEntry);
        ptr->Init();
        ptr->SetFileName(&path_str);
        ptr->SetMd5(md5);
        ptr->SetIndex(index);

        if(!map->PutPtr(ptr))
        {
            DEL(ptr);
        }

        return OK;
    }
    END_CLOSURE(build_index);

    CSimpleDiskFsIndex hashmap;
    hashmap.Init(1024*10);
    build_index.SetParamPointer(10,&hashmap);
    this->SearchDirectory(&build_index);
    mFastFsIndex->SaveHashFile(&hashmap,out,true);
    return OK;
}

status_t CSimpleDisk::AddFastFsIndex()
{
    CMemFile mf;
    mf.Init();
    ASSERT(this->BuildFastFsIndex(&mf));
    ASSERT(mIndexFile->AddBlock(&mf));
    return OK;
}

status_t CSimpleDisk::RebuildFastIndex()
{
    CMemFile mf;
    mf.Init();
    ASSERT(this->BuildFastFsIndex(&mf));
    return mFastFsIndex->LoadHashFileTransfer(&mf);
}
