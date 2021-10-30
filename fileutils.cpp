#include "fileutils.h"
#include "sys_log.h"
#include "mem_tool.h"

static CMem _buf;

status_t CopySingleFile_Interruptable(CMem *_from,CMem *_to, fsize_t start, fsize_t size,fsize_t to_start,uint32_t mode,int *running)
{
    ASSERT(_from && _to);
    
    LOCAL_MEM(mem_from);
    LOCAL_MEM(mem_to);

    CMem *from = &mem_from;
    CMem *to = &mem_to;

    from->StrCpy(_from);
    to->StrCpy(_to);

    ASSERT(CDirMgr::StdPathSplitor(from));
    ASSERT(CDirMgr::StdPathSplitor(to));

#if _IS_LINUX_
    if(mode & MODE_COPY_LINK)
    {
        LOCAL_MEM(from_link);
        LOCAL_MEM(to_link);

        int n = readlink(from->CStr(),from_link.GetRawBuf(),from_link.GetMaxSize());    
        if(n > 0)
        {
            from_link.SetSize(n);   
            n = readlink(to->CStr(),to_link.GetRawBuf(),to_link.GetMaxSize());  
            if(n > 0)
            {
                to_link.SetSize(n);
                if(from_link.StrEqu(&to_link,true))
                {
                    return OK;
                }
                else
                {
                    if(mode & MODE_OVERWRITE)
                    {
                        unlink(to->CStr());
                    }
                }
            }
            return symlink(from_link.CStr(),to->CStr()) == 0;
        }
    }
#endif

    if(_buf.GetRawBuf() == NULL)
    {
        _buf.Init();
        _buf.Malloc(32*1024*1024); 
    }

    if(mode&MODE_CHECKSAME)
    {
        if(IsFileSame(from->CStr(),to->CStr()))
            return OK+1;
    }

    if(!(mode&MODE_OVERWRITE))
    {
        if(CDirMgr::IsFileExist(to))
            return OK+2;
    }

    CFile file_from;
    file_from.Init();

    if(!file_from.OpenFile(from->CStr(),"rb"))
    {
        XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
            "open src file %s error.",from->CStr());
        return ERROR;
    }
    
    ASSERT(CDirMgr::CreateFilePath(to));
    CFile file_to;
    file_to.Init();

	bool need_open_create = true;

	if(mode & MODE_PART_COPY)
	{
		if(file_to.OpenFile(to->CStr(),"r+"))
		{			
			need_open_create = false;
		}
	}
	
	if(need_open_create)
	{
		if(!file_to.OpenFile(to->CStr(),"wb+"))
		{
			XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
                "open dst file %s error.",to->CStr());
			return ERROR;
		}
	}

	if(to_start > file_to.GetSize())
	{
		file_to.FillBlock(to_start,0);
	}

    file_to.Seek(to_start);

    char *pbuf = _buf.GetRawBuf();
    int_ptr_t read_size,write_size;
    fsize_t buf_size;
    
    char str[1024];
    char str1[1024];
    if(mode & MODE_USE_LOG)
    {
        XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
            "%s ==> %s",from->CStr(),to->CStr());
    } 

    file_from.Seek(start);

    if(size <0)
        size = file_from.GetSize();

    if(file_from.GetSize()-start < size)
        size = file_from.GetSize()-start;

    crt_fsize_to_str(size,str);
    fsize_t total_size = 0;
    while(total_size < size)
    {
        if(running && !(*running))
            break;
        buf_size = _buf.GetMaxSize();
        if(buf_size > (size-total_size))
            buf_size = (size-total_size);

        read_size = file_from.Read(pbuf,(int_ptr_t)buf_size);
        if(read_size <= 0)return ERROR;
        write_size = file_to.Write(pbuf,read_size);
        total_size += write_size;
        crt_fsize_to_str(total_size,str1);
        if(mode & MODE_USE_LOG)
            printf("\r%s/%s",str1,str);
        fflush(stdout); 
        if(write_size != read_size)
            return ERROR;
    }

    if(mode & MODE_USE_LOG)
        printf("\n");

    return OK;
}

status_t CopySingleFile_Interruptable(CMem *_from,CMem *_to, fsize_t start, fsize_t size, uint32_t mode,int *running)
{
	return CopySingleFile_Interruptable(_from,_to,start,size,0,mode,running);
}

status_t CopySingleFile_Interruptable(CMem *from,CMem *to, uint32_t mode, int *running)
{
    return CopySingleFile_Interruptable(from,to,0,-1,mode,running);
}

status_t CopySingleFile_Interruptable(const char *from,const char *to,uint32_t mode,int *running)
{
    ASSERT(from);
    ASSERT(to);

    CMem mem_from(from);
    CMem mem_to(to);

    return CopySingleFile_Interruptable(&mem_from,&mem_to,mode,running);
}

status_t CopySingleFile(CMem *_from,CMem *_to, fsize_t start, fsize_t size,fsize_t to_start,uint32_t mode)
{
    return CopySingleFile_Interruptable(_from,_to,start,size,to_start,mode,NULL);
}

status_t CopySingleFile(CMem *_from,CMem *_to, fsize_t start, fsize_t size, uint32_t mode)
{
    return CopySingleFile_Interruptable(_from,_to,start,size,mode,NULL);
}

status_t CopySingleFile(CMem *from,CMem *to, uint32_t mode)
{
    return CopySingleFile_Interruptable(from,to,mode,NULL);
}

status_t CopySingleFile(const char *from,const char *to,uint32_t mode)
{
    return CopySingleFile_Interruptable(from,to,mode,NULL);
}

bool IsTextFile(CFileBase *file)
{
    ASSERT(file);

    file->Seek(0);
    while(!file->IsEnd())
    {
        unsigned char ch = file->Getc();
        if(   (ch < 32) &&
              (ch != 0x0a) &&  /* Not an LF */
              (ch != 0x0d) &&  /* Not a CR */
              (ch != 0x09) &&  /* Not a TAB */
              (ch != 0x0c)) {  /* Not a form feed */
            return false;
        }
    }
    return true;
}

bool IsTextFile(const char *fn)
{
    LOCAL_DISK_FILE(file,fn,"rb",true,false); 
    return IsTextFile(&file);   
}

status_t ConvertEol(CFileBase *in, CFileBase *out, int mode)
{
    ASSERT(in && out);

    in->Seek(0);
    out->SetSize(0);

    while(!in->IsEnd())
    {
        char ch = in->Getc();
        if(ch == '\r')continue;
        
        if(mode == 0) //win -> unix
        {
            out->Putc(ch);
        }
        else //unix->win
        {
            if(ch == '\n')
                out->Puts("\r\n");
            else
                out->Putc(ch);
        }
    }

    return OK;
}

status_t ConvertEol(const char *in, const char *out, int mode)
{
    ASSERT(in && out);
    LOCAL_DISK_FILE(file_in,in,"rb",true,true);
    LOCAL_DISK_FILE(file_out,out,"wb+",false,true);
    return ConvertEol(&file_in,&file_out,mode);
}

bool IsFileSame(CFileBase *src1, CFileBase *src2)
{
    ASSERT(src1 && src2);
    
    if(src1->GetSize() != src2->GetSize())
        return false;
    
    src1->Seek(0);
    src2->Seek(0);
    fsize_t size = src1->GetSize();
    for(int i = 0; i < size; i++)
    {
        if(src1->Getc() != src2->Getc())
            return false;
    }
    return true;
}

bool IsFileSame(const char *fn1, const char *fn2)
{
    ASSERT(fn1 && fn2);
    
    LOCAL_DISK_FILE(file1,fn1,"rb",true,false);
    LOCAL_DISK_FILE(file2,fn2,"rb",true,false);
    return IsFileSame(&file1,&file2);
}



