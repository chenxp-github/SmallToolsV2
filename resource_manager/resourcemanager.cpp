#include "resourcemanager.h"
#include "syslog.h"
#include "mem_tool.h"

CResourceManager::CResourceManager()
{
    this->InitBasic();
}
CResourceManager::~CResourceManager()
{
    this->Destroy();
}
status_t CResourceManager::InitBasic()
{
    WEAK_REF_ID_CLEAR();
    LUA_USER_DATA_CLEAR();
    this->m_HashMap.InitBasic();
    m_HashFile.InitBasic();
    m_PathPrefix.InitBasic();
    return OK;
}
status_t CResourceManager::Init()
{
    this->InitBasic();
    WEAK_REF_ID_INIT();
    this->m_HashMap.Init(1024*10);
    m_HashFile.Init();
    m_PathPrefix.Init();
    this->SetPathPrefix("");
    return OK;
}
status_t CResourceManager::Destroy()
{
    m_PathPrefix.Destroy();
    m_HashMap.Destroy();
    m_HashFile.Destroy();
    this->InitBasic();
    return OK;
}
int CResourceManager::Comp(CResourceManager *_p)
{
    ASSERT(_p);
    if(this==_p)return 0;
    return 0;
}
status_t CResourceManager::Copy(CResourceManager *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;
    ASSERT(0);
    return OK;
}
status_t CResourceManager::Print(CFileBase *_buf)
{
    _buf->Log("HashMap = {");
    _buf->IncLogLevel(1);
    m_HashMap.Print(_buf);
    _buf->IncLogLevel(-1);
    _buf->Log("}");
    return OK;
}
CResourceHashMap* CResourceManager::GetHashMap()
{
    return &this->m_HashMap;
}

status_t CResourceManager::AddResource(CResource *res)
{
    ASSERT(res);
    return m_HashMap.Put(res);
}

status_t CResourceManager::SaveResourceFile(const char *filename)
{
    ASSERT(filename);

    CMemFile mf;
    mf.Init();
    
    ASSERT(
        m_HashFile.SaveHashFile(&m_HashMap,&mf,true)
    );
    
    return mf.WriteToFile(filename) > 0;
}

status_t CResourceManager::LoadResourceFile(CFileBase *i_file)
{
    ASSERT(i_file);
    m_HashFile.Destroy();
    m_HashFile.Init();
    return m_HashFile.LoadHashFile(i_file);
}

status_t CResourceManager::SearchResource(CResource *key, CResource *value)
{
    ASSERT(key && value);

    int old_weak_ref_id = value->__weak_ref_id;
    status_t ret = m_HashFile.Search(key,value);
    value->__weak_ref_id = old_weak_ref_id;
    return ret;
}


status_t CResourceManager::SearchByString(CMem *theme,CMem *language,CMem *id, CResource *value)
{
    ASSERT(theme && language && id);
    
    CResource tmp;
    tmp.Init();
    tmp.SetTheme(theme);
    tmp.SetLanguage(language);
    tmp.SetId(id);
    return SearchResource(&tmp,value);
}

status_t CResourceManager::SearchByString(const char *theme,const char *language,const char *id, CResource *value)
{
    ASSERT(theme && language && id);
    CMem mem_theme(theme);
    CMem mem_language(language);
    CMem mem_id(id);
    return this->SearchByString(&mem_theme,&mem_language,&mem_id,value);
}

status_t CResourceManager::LoadResourceFile(const char *filename)
{
    ASSERT(filename);

    CMem mf;
    mf.Init();
    ASSERT(mf.LoadFile(filename));

    m_HashFile.Destroy();
    m_HashFile.Init();
    return m_HashFile.LoadHashFileTransfer(&mf);
}

status_t CResourceManager::SetPathPrefix(CMem* _pathprefix)
{
    this->m_PathPrefix.Copy(_pathprefix);
    return OK;
}
status_t CResourceManager::SetPathPrefix(const char *_pathprefix)
{
    CMem _mem(_pathprefix);
    return this->SetPathPrefix(&_mem);
}
CMem* CResourceManager::GetPathPrefix()
{
    return &this->m_PathPrefix;
}
const char* CResourceManager::GetPathPrefixStr()
{
    return this->m_PathPrefix.CStr();
}

status_t CResourceManager::LoadResourceFileGhost(CMem *data)
{
    ASSERT(data);
    m_HashFile.Destroy();
    m_HashFile.Init();
    return m_HashFile.LoadHashFileGhost(data);
}

status_t CResourceManager::LoadResourceFileCopy(CFileBase *data)
{
    ASSERT(data);
    m_HashFile.Destroy();
    m_HashFile.Init();
    return m_HashFile.LoadHashFileCopy(data);
}

status_t CResourceManager::LoadResourceFileTransfer(CMem *from)
{
    ASSERT(from);
    m_HashFile.Destroy();
    m_HashFile.Init();
    return m_HashFile.LoadHashFileTransfer(from);
}

status_t CResourceManager::DumpResourceFile(CFileBase *out)
{
    ASSERT(out);

    CIndexFile *index_file = &m_HashFile.m_IndexFile;
    int total_block = index_file->GetBlockCount() - 1; //skip meta data

    int total = 0;
    for(int i = 0; i < total_block; i++)
    {
        CPartFile tmp;
        index_file->GhostBlock(i,&tmp);
        if(tmp.GetSize() > 0)
        {
            total ++;
            out->Log("[%d] = {",i);
            out->IncLogLevel(1);
            
            CResource r;
            r.Init();
            tmp.Read(&r.__next_index,sizeof(r.__next_index));
            r.UnSerialize(&tmp);
            r.Print(out);
            out->IncLogLevel(-1);
            out->Log("}");
        }
    }

    out->Log("");
    out->Log("total resources = %d",total);

    return OK;
}

status_t CResourceManager::DumpResourceFile(const char *filename)
{
    ASSERT(filename);
    CMemFile tmp;
    tmp.Init();
    this->DumpResourceFile(&tmp);
    tmp.WriteToFile(filename);
    return OK;
}

