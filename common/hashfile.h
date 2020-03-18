#ifndef __HASH_FILE_H
#define __HASH_FILE_H

#include "ptrstk.h"
#include "indexfile.h"
#include "memfile.h"
#include "syslog.h"

#define HASH_FILE_META_DATA "HASH_META_DATA_V1"
#define HASH_FILE_META_DATA_LEN 17

template<class HashTable,class HashEntry>
class CHashFile{
public:
    int32_t m_HashMapCapacity;
    CIndexFile m_IndexFile;
    CPartFile m_IndexDataFile_PartFile;
    CMemFile m_IndexDataFile_MemFile;
    CMem m_IndexDataFile_Mem;
    CFileBase *i_UserMetaData;
public:
    CHashFile()
    {
        this->InitBasic();
    }

    ~CHashFile()
    {
        this->Destroy();
    }

    status_t InitBasic()
    {
        m_HashMapCapacity = 0;
        m_IndexFile.InitBasic();
        m_IndexDataFile_MemFile.InitBasic();
        m_IndexDataFile_PartFile.InitBasic();
        m_IndexDataFile_Mem.InitBasic();
        i_UserMetaData = NULL;
        return OK;
    }

    status_t Init()
    {
        m_IndexFile.Init();
        return OK;
    }
    
    status_t Destroy()
    {
        m_IndexDataFile_Mem.Destroy();
        m_IndexDataFile_MemFile.Destroy();
        m_IndexDataFile_PartFile.Destroy();
        m_IndexFile.Destroy();
        return OK;
    }

    static status_t Transfer(HashTable *old_map, HashTable *new_map, int new_size)
    {
        ASSERT(new_map);
        
        BEGIN_CLOSURE(shrink)
        {
            CLOSURE_PARAM_PTR(HashEntry*,entry,0);
            CLOSURE_PARAM_PTR(HashTable*,new_map,10);
            new_map->PutPtr(entry);
            closure->SetParamPointer(0,NULL); //mark removed
            return OK;
        }
        END_CLOSURE(shrink);
        
        new_map->Destroy();
        new_map->Init(new_size);
        shrink.SetParamPointer(10,new_map);
        old_map->EnumAll(&shrink);
        old_map->DiscardAll();
        return OK;
    }
    
    static status_t UpdateAllIndex(HashTable *map,int *max_index)
    {
        ASSERT(map && max_index);

        BEGIN_CLOSURE(update_cur_index)
        {
            CLOSURE_PARAM_PTR(HashEntry*,entry,0);
            CLOSURE_PARAM_INT(is_first,1);
            CLOSURE_PARAM_PTR(HashTable*,self,10);
            CLOSURE_PARAM_PTR(int*,auto_extra_id,11);
            if(is_first)
                entry->__cur_index = self->HashCode(entry);
            else
                entry->__cur_index = (*auto_extra_id)++;
            return OK;
        }
        END_CLOSURE(update_cur_index);
        
        int auto_extra_id = map->GetCapacity();
        update_cur_index.SetParamPointer(10,map);
        update_cur_index.SetParamPointer(11,&auto_extra_id);
        map->EnumAll(&update_cur_index);
        
        BEGIN_CLOSURE(update_next)
        {
            CLOSURE_PARAM_PTR(HashEntry*,entry,0);
            CLOSURE_PARAM_PTR(HashTable*,self,10);
            if(entry->__next)
            {
                entry->__next_index = entry->__next->__cur_index;
            }
            return OK;
        }
        END_CLOSURE(update_next);
        
        update_next.SetParamPointer(10,map);
        map->EnumAll(&update_next);
        
        *max_index = auto_extra_id;
        return OK;
    }

    static status_t PutToSequenceList(HashTable *map,CPtrStk *list, int list_size)
    {
        ASSERT(list && map);
        
        BEGIN_CLOSURE(put_to_list)
        {
            CLOSURE_PARAM_PTR(HashEntry*,entry,0);
            CLOSURE_PARAM_PTR(HashTable*,self,10);
            CLOSURE_PARAM_PTR(CPtrStk*,list,11);
            ASSERT(entry->__cur_index >= 0 && entry->__cur_index < list->GetLen());
            ASSERT(list->GetElem(entry->__cur_index) == NULL);
            list->SetElem(entry->__cur_index,entry);        
            return OK;
        }
        END_CLOSURE(put_to_list);
        
        list->Destroy();
        list->Init(list_size);
        
        for(int i = 0; i < list_size; i++)
        {
            list->Push(NULL);
        }
        
        put_to_list.SetParamPointer(10,map);
        put_to_list.SetParamPointer(11,list);
        map->EnumAll(&put_to_list);
        
        return OK;
    }

    status_t MakeMetaData(HashTable *map,CFileBase *file)
    {
        ASSERT(map && file);
        file->Puts(HASH_FILE_META_DATA);
        int32_t capacity = map->GetCapacity();
        file->Write(&capacity,sizeof(capacity));

        if(i_UserMetaData)
        {
            int32_t size = (int32_t)i_UserMetaData->GetSize();
            file->Write(&size,sizeof(size));
            file->Puts(i_UserMetaData);
        }
        return OK;
    }

    status_t LoadMetaData(CIndexFile *index_file,int index)
    {
        ASSERT(index_file);

        CPartFile file;
        index_file->GhostBlock(index,&file);
        
        char buf[1024];
        file.Seek(0);
        file.Read(buf,HASH_FILE_META_DATA_LEN);
        if(memcmp(buf,HASH_FILE_META_DATA,HASH_FILE_META_DATA_LEN))
            return ERROR;
        file.Read(&m_HashMapCapacity,sizeof(m_HashMapCapacity));

        if(i_UserMetaData)
        {
            int32_t size;
            file.Read(&size,sizeof(size));          
            if(size > 0)
            {
                file.WriteToFile(i_UserMetaData,file.GetOffset(),size);
            }
        }
        return OK;
    }

    status_t Search(HashEntry *key,HashEntry *out)
    {
        ASSERT(key && out);
        ASSERT(m_HashMapCapacity > 0);
        
        int index = HashTable::HashCode(key,m_HashMapCapacity);
        
        while(index >= 0)
        {   
            ASSERT(index < m_IndexFile.GetBlockCount());
            
            CPartFile file;
            file.Init();
            
            if(!m_IndexFile.GhostBlock(index,&file))
                return ERROR;
            
            out->Destroy();
            out->Init();
            
            file.Seek(0);
            file.Read(&out->__next_index,sizeof(out->__next_index));
            out->UnSerialize(&file);
            
            if(out->Comp(key) == 0)
                return OK;
            
            index = out->__next_index;
        }
        
        return ERROR;
    }

    status_t LoadHashFile(CFileBase *i_file)
    {
        ASSERT(i_file);
        m_IndexFile.Destroy();
        m_IndexFile.Init();
        ASSERT(m_IndexFile.LoadIndexFile(i_file));
        int block_count = (int)m_IndexFile.GetBlockCount();
        ASSERT(block_count > 0);
        ASSERT(this->LoadMetaData(&m_IndexFile,block_count-1));
        return OK;
    }

    status_t LoadHashFileGhost(CPartFile *part_file)
    {
        ASSERT(part_file);
        m_IndexDataFile_PartFile.Destroy();
        m_IndexDataFile_PartFile.Init();
        m_IndexDataFile_PartFile.Copy(part_file);
        return LoadHashFile(&m_IndexDataFile_PartFile);
    }

    status_t LoadHashFileCopy(CFileBase *file)
    {
        ASSERT(file);
        m_IndexDataFile_MemFile.Destroy();
        m_IndexDataFile_MemFile.Init();
        m_IndexDataFile_MemFile.WriteFile(file);
        return LoadHashFile(&m_IndexDataFile_MemFile);
    }
    
    status_t LoadHashFileGhost(CMem *mem)
    {
        ASSERT(mem);
        m_IndexDataFile_Mem.Destroy();
        m_IndexDataFile_Mem.Init();
        m_IndexDataFile_Mem.SetRawBuf(
            mem->GetRawBuf(),(int_ptr_t)mem->GetSize(),true);
        return LoadHashFile(&m_IndexDataFile_Mem);
    }

    status_t LoadHashFileTransfer(CMem *from)
    {
        ASSERT(from);
        m_IndexDataFile_Mem.Destroy();
        m_IndexDataFile_Mem.Init();
        m_IndexDataFile_Mem.Transfer(from);
        return LoadHashFile(&m_IndexDataFile_Mem);
    }

    status_t LoadHashFileTransfer(CMemFile *from)
    {
        ASSERT(from);
        m_IndexDataFile_MemFile.Destroy();
        m_IndexDataFile_MemFile.Init();
        m_IndexDataFile_MemFile.Transfer(from);
        return LoadHashFile(&m_IndexDataFile_MemFile);
    }

    status_t SaveHashFile(HashTable *map,CIndexFile *index_file,bool auto_shrink)
    {
        ASSERT(index_file && map);
        
        HashTable *used_map = map;
        HashTable tmp_map;
        int32_t max_index;
        int32_t old_capacity = map->GetCapacity();
        
        if(auto_shrink)
        {
            Transfer(map,&tmp_map,map->GetSize()*2);
            used_map = &tmp_map;
        }

        UpdateAllIndex(used_map,&max_index);

        CPtrStk list;
        PutToSequenceList(used_map,&list,max_index);
        
        ASSERT(list.GetLen() > 0);
        
        for(int i = 0; i < list.GetLen(); i++)
        {
            HashEntry *p = (HashEntry*)list.GetElem(i);
            if(p == NULL)
            {
                CMem dummy;
                dummy.Init();
                index_file->AddBlock(&dummy);
            }
            else
            {
                CMemFile data;
                data.Init();
                data.Write(&p->__next_index,sizeof(p->__next_index));
                p->Serialize(&data);
                index_file->AddBlock(&data);
            }
        }
        
        LOCAL_MEM(meta);
        MakeMetaData(used_map,&meta);
        index_file->AddBlock(&meta);
        index_file->Save();

        if(auto_shrink)
        {
            Transfer(&tmp_map,map,old_capacity);
        }

        return OK;
    }

    status_t SaveHashFile(HashTable *map,CFileBase *file,bool auto_shrink)
    {
        ASSERT(file && map);
        CIndexFile index_file;
        index_file.Init();
        index_file.SetDstFile(file);
        return SaveHashFile(map,&index_file,auto_shrink);
    }

    status_t SetUserMetaData(CFileBase *i_file)
    {
        i_UserMetaData = i_file;
        return OK;
    }

};

#endif
