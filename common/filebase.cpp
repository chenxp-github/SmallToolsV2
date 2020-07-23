// FileBase.cpp: implementation of the CFileBase class.
//
//////////////////////////////////////////////////////////////////////
#include "filebase.h"
#include "syslog.h"
#include "mem_tool.h"
#include "mem.h"

///////////////////////////////////////////////////////////////////
//common part
///////////////////////////////////////////////////////////////////
CFileBase::CFileBase()
{
    this->InitBasic();
}

CFileBase::~CFileBase()
{
    this->Destroy();
}

char CFileBase::Getc()
{
    char ch = 0;
    this->Read(&ch,1);
    return ch;
}
fsize_t CFileBase::Putc(char ch)
{
    return this->Write(&ch,1);
}
fsize_t CFileBase::SearchBin(CFileBase *file_bin)
{
    ASSERT(file_bin);
    fsize_t old;
    char ch1,ch2;

    file_bin->Seek(0);

    while(!this->IsEnd())
    {
        old = this->GetOffset();
        file_bin->Seek(0);

        while (!this->IsEnd() && !file_bin->IsEnd()) 
        {
            ch2 = file_bin->Getc();
            ch1 = this->Getc();
            if(ch1 != ch2)
            {
                file_bin->Seek(0);
                break;              
            }
        }

        if(file_bin->IsEnd())
        {
            return this->GetOffset() - file_bin->GetSize();
        }

        this->Seek(old + 1);
    }

    return -1;
}


fsize_t CFileBase::WriteToFile(CFileBase *file)
{
    return this->WriteToFile(file,0,this->GetSize());
}
fsize_t CFileBase::WriteToFile(CFileBase *file, fsize_t start, fsize_t wsize, void *buf, int_ptr_t buf_size)
{
    fsize_t sum = 0;
    int_ptr_t need_read,ws,rs;
    int_ptr_t bsize = buf_size;

    ASSERT(file);
    
    this->Seek(start);

    while(sum < wsize)
    {
        need_read = bsize;
        if(need_read > wsize - sum)
            need_read = (int_ptr_t)(wsize - sum);
        rs = this->Read(buf,need_read);
        if(rs <= 0) break;

        ws = file->Write(buf,rs);
        if(ws <= 0) break;
        sum += ws;
        if(ws != rs)break;
    }
    
    return sum;
}
status_t CFileBase::FillBlock(fsize_t fill_size, char fill_ch)
{
    for(fsize_t i = 0; i<fill_size; i++)
    {
        this->Putc(fill_ch);
    }
    return OK;
}

fsize_t CFileBase::SeekEnd()
{
    return this->Seek(MAX_FSIZE);
}

status_t CFileBase::Destroy()
{
    WEAK_REF_DESTROY();
    FREE(this->file_name);
    this->SetSplitChars(NULL);
    return OK;
}

fsize_t CFileBase::SeekBack(fsize_t back_bytes)
{
    fsize_t off = this->GetOffset();
    off -= back_bytes;
    if(off < 0) off = 0;
    return this->Seek(off);
}
status_t CFileBase::ExpandFromOff(fsize_t exp_size)
{
    fsize_t b_size; 
    b_size = this->GetSize();
    this->SetSize(this->GetOffset());
    this->AddBlock(exp_size);
    if(this->GetSize() < b_size)
        this->SetSize(b_size);
    return OK;
}
status_t CFileBase::Insert(CFileBase *file,fsize_t start, fsize_t size)
{
    int_ptr_t bsize = FILEBASE_LBUF_SIZE;
    char temp[FILEBASE_LBUF_SIZE];
    fsize_t old_size;
    fsize_t old_off,off,sum = 0;
    int_ptr_t need_read,rs,ws;

    if(size <= 0)return ERROR;

    ASSERT(file);

    old_size = this->GetSize();
    old_off = this->GetOffset();

    this->AddBlock(size);
    sum = old_size - old_off;
    this->Seek(old_size);
    off = old_size;
    while(sum > 0)
    {
        need_read = bsize;
        if(need_read > sum)
            need_read = (int_ptr_t)sum;
        off -= need_read;
        this->Seek(off);
        rs = this->Read(temp,need_read);
        if(rs <= 0) break;
        sum -= rs;
        this->Seek(off + size);
        ws = this->Write(temp,rs);

        if(ws <= 0) break;
    }
    this->Seek(old_off);
    this->WriteFile(file,start,size);
    this->Seek(old_off + size);
    return OK;
}

status_t CFileBase::Insert(CFileBase *file)
{
    return this->Insert(file,0,file->GetSize());
}

status_t CFileBase::Delete(fsize_t del_size)
{
    fsize_t sum,off;
    char buf[FILEBASE_LBUF_SIZE];
    int_ptr_t need_read,rs;
    int_ptr_t bsize=FILEBASE_LBUF_SIZE;
    
    off = this->GetOffset();
    if(this->GetSize() - off < del_size)
        del_size = this->GetSize() - off;
    if(del_size <= 0) return ERROR;
    sum = this->GetSize() - off - del_size;
    while(sum > 0)
    {
        need_read = bsize;

        if(need_read > sum)
            need_read = (int_ptr_t)sum;     

        this->Seek(off + del_size);
        rs = this->Read(buf,need_read);
        if(rs <= 0) break;      
        this->Seek(off);
        this->Write(buf,rs);
        off += rs;
        sum -= rs;
    }   
    this->SetSize(this->GetSize() - del_size);
    return OK;
}
status_t CFileBase::DeleteLast(fsize_t bytes)
{
    this->SetSize(this->GetSize() - bytes);
    return OK;
}
bool CFileBase::IsEnd()
{
    return (this->GetOffset() >= this->GetSize());
}

fsize_t CFileBase::WriteFile(CFileBase *file, fsize_t start, fsize_t ws)
{
    char buf[FILEBASE_LBUF_SIZE];   
    return this->WriteFile(file,start,ws,buf,FILEBASE_LBUF_SIZE);
}

fsize_t CFileBase::WriteFile(CFileBase *file, fsize_t start, fsize_t ws, void *buf, int_ptr_t buf_size)
{
    ASSERT(file);

    fsize_t block_size = buf_size;
    fsize_t left,write_size,b_size,old_off;
    int_ptr_t rs;
    
    old_off = file->GetOffset();
    file->Seek(start);
    left = ws;

    while(left > 0)
    {
        b_size = left;
        if(b_size > block_size)
            b_size = block_size;
        
        rs = file->Read(buf,(int_ptr_t)b_size);
        if(rs > 0)
        {
            write_size = this->Write(buf,rs);
            if(write_size > 0)
                left -= write_size;
            if(write_size!=rs)
                break;
        }
        else
        {
            break;
        }
    }
    file->Seek(old_off);
    return ws - left;   
}

fsize_t CFileBase::WriteFile(CFileBase *file)
{
    ASSERT(file);
    return this->WriteFile(file,0,file->GetSize());
}
fsize_t CFileBase::WriteToFile(CFileBase *file,fsize_t wsize)
{
    return this->WriteToFile(file,0,wsize);
}

fsize_t CFileBase::WriteToFile(CFileBase *file,fsize_t start,fsize_t wsize)
{
    char buf[FILEBASE_LBUF_SIZE];   
    return this->WriteToFile(file,start,wsize,buf,FILEBASE_LBUF_SIZE);
}

status_t CFileBase::LoadFile(FILE_HANDLE fp)
{
    ASSERT(fp);

    char buf[FILEBASE_LBUF_SIZE];
    fsize_t fs,old_off;
    int_ptr_t rs,buf_size = FILEBASE_LBUF_SIZE;

    crt_fseek(fp,0,SEEK_END);
    fs = crt_ftell(fp);
    crt_fseek(fp,0,SEEK_SET);

    this->AddBlock(fs);
    this->Seek(0);

    do{
        old_off = crt_ftell(fp);
        crt_fread(fp,buf,buf_size);
        rs = (int_ptr_t)(crt_ftell(fp) - old_off);
        if(rs > 0)
        {
            this->Write(buf,rs);
        }
    }while(rs > 0);

    this->Seek(0);
    return OK;
}

fsize_t CFileBase::WriteToFile(FILE_HANDLE fp,fsize_t start,fsize_t wsize)
{
    ASSERT(fp);

    char buf[FILEBASE_LBUF_SIZE];
    int_ptr_t rs,bsize = FILEBASE_LBUF_SIZE;    
    fsize_t sum = 0,need_read;

    this->Seek(start);
    while(sum < wsize)
    {
        need_read = bsize;
        if(need_read > wsize - sum)
            need_read = wsize - sum;
        rs = this->Read(buf,(int_ptr_t)need_read);
        if(rs <= 0) break;
        crt_fwrite(fp,buf,rs);
        sum += rs;
    }
    return sum;
}

void* CFileBase::GetUserData()
{
    return this->user_data_;
}
void CFileBase::SetUserData(void *ud)
{
    this->user_data_ = ud;
}
//////////////////////////////////////////////////////////////////////
#if _ASC_
//////////////////////////////////////////////////////////////////////
#define MAKE_SZ_BUFFER() \
    char szBuffer [FILEBASE_LBUF_SIZE]; \
    crt_va_list pArgList;\
    crt_va_start(pArgList, szFormat);\
    crt_vsnprintf(szBuffer, FILEBASE_LBUF_SIZE, szFormat, pArgList);\
    crt_va_end (pArgList)\

fsize_t CFileBase::Log(const char *szFormat, ...)
{   
    this->Tab();
    if(szFormat == NULL)
        return OK;
    MAKE_SZ_BUFFER();
    this->Puts(szBuffer);
    this->Puts("\r\n");
    return OK;
}

status_t CFileBase::Eol()
{
    this->Puts("\r\n");
    return OK;
}

status_t CFileBase::Tab()
{
    for(int i = 0; i < log_tab_level_; i++)
        this->Write("    ",4);
    return OK;
}

status_t CFileBase::IncLogLevel(int inc)
{
    return this->SetLogLevel(log_tab_level_+inc);
}
status_t CFileBase::SetLogLevel(int l)
{
    this->log_tab_level_ = l;
    if(this->log_tab_level_ < 0)
        this->log_tab_level_ = 0;
    return OK;
}
status_t CFileBase::GetLogLevel()
{
    return this->log_tab_level_;
}

fsize_t CFileBase::Printf(const char *szFormat, ...)
{
    MAKE_SZ_BUFFER();
    return this->Puts(szBuffer);
}

status_t CFileBase::ReadLine_Reverse()
{
    char ch;

    if(this->GetOffset() <=0)
        return ERROR;

    ch = this->UnGetc();
    if(ch != '\n')
    {
        this->Seek(GetOffset()+1);
    }

    while(1)
    {
        if(this->GetOffset() <= 0)
            return OK;
        ch = this->UnGetc();
        if(ch == '\n')
        {
            this->Seek(GetOffset()+1);
            break;
        }
    }
    return OK;
}

status_t CFileBase::ReadLine_Reverse(CFileBase *file)
{
    fsize_t off;
    status_t ret;

    ret = this->ReadLine_Reverse(); 
    off = this->GetOffset();
    this->ReadLine(file);
    this->Seek(off);
    return ret;
}

status_t CFileBase::IsSpChar(char ch)
{
    const char *p;
    
    if(ch == 0 || ch == '\n')
        return TRUE;

    p = this->sp_chars;
    while(*p)
    {
        if(*p == ch)
            return TRUE;
        p++;
    }
    return FALSE;
}

status_t CFileBase::SetDefaultSpChars()
{
    this->sp_chars = (char*)" \t\r\n!@#$%^=&*()|+-\\*/{}[];.\":\',?/<>~";
    return OK;
}
//the return value is the number of bytes it reads.
fsize_t CFileBase::ReadWordWithEmptyChar(CFileBase *file)
{
    fsize_t k = 0;
    char ch;
    
    ASSERT(file);

    file->SetSize(0);

    if(this->IsEnd())
        return k;

    ch = this->Getc();
    if(ch == 0)return k;

    if(this->IsSpChar(ch))
    {
        file->Putc(ch);
        k = 1;
        return k;
    }
    while(!this->IsSpChar(ch))
    {
        file->Putc(ch);
        k++;
        if(this->IsEnd())
            break;
        ch = this->Getc();
    }
    if(this->IsSpChar(ch))
        this->UnGetc();
    return k;
}
status_t CFileBase::ReadString(CFileBase *file)
{
    fsize_t k;
    k = this->ReadWord(file);
    while(k)
    {
        file->Seek(0);
        if(!this->IsSpChar(file->Getc()))   
        {
            return OK;
        }
        k = this->ReadWord(file);
    }
    return ERROR;
}
status_t CFileBase::SkipEmptyChars()
{
    char ch;
    while(!this->IsEnd())
    {
        ch = this->Getc();
        if(!this->IsEmptyChar(ch) )
        {
            this->UnGetc();
            break;
        }
    }
    return OK;
}
fsize_t CFileBase::ReadWord(CFileBase *file)
{
    this->SkipEmptyChars();
    return this->ReadWordWithEmptyChar(file);
}

fsize_t CFileBase::Puts(const char *str)
{
    if(str == NULL)
        return ERROR;

    return this->Write(str,crt_strlen(str));
}
char CFileBase::UnGetc()
{
    char ch=0;
    fsize_t off;
    
    off = this->GetOffset();
    if(off > 0)
    {
        off--;
        this->Seek(off);
        off = this->GetOffset();
        ch =  this->Getc();
        this->Seek(off);
    }
    return ch;
}

status_t CFileBase::Dump()
{
    LOCAL_MEM(mem);
    fsize_t save_off;
    
    save_off = this->GetOffset();
    this->Seek(0);

    while(this->ReadLine(&mem))
    {
        syslog_printf("%s\n",mem.CStr());
    }

    this->Seek(save_off);
    return OK;
}
status_t CFileBase::ReadLine(CFileBase *file)
{
    char ch;

    ASSERT(file);
    if(this->IsEnd()) 
        return ERROR;
    file->SetSize(0);   
    ch = this->Getc();
    while(ch != '\n')
    {
        if(ch!='\r')
            file->Putc(ch);
        if(this->IsEnd())
            break;
        ch = this->Getc();
    }
    return OK;
}

status_t CFileBase::ReadLineWithEol(CFileBase *file,CFileBase *eol)
{
    char ch;
    
    ASSERT(file && eol);

    if(this->IsEnd()) 
        return ERROR;
    
    eol->SetSize(0);
    file->SetSize(0);   

    while(!this->IsEnd())
    {
        ch = this->Getc();
        if(ch == '\n')
        {
            eol->Putc(ch);
            break;
        }
        if(ch=='\r')
        {
            eol->Putc(ch);
        }
        else
        {
            file->Putc(ch);
        }
    }
    return OK;
}

status_t CFileBase::IsEmptyChar(char ch)
{
    return ch == ' '|| ch == 0  ||  ch == '\r' || ch == '\n' || ch == '\t';
}

status_t CFileBase::SetSplitChars(const char *sp_str)
{
    if(is_sp_chars_malloc)
    {
        FREE(this->sp_chars);
        is_sp_chars_malloc = false;
    }
    
    if(sp_str)
    {
        this->sp_chars = (char*)sp_str;
    }
    return OK;
}

status_t CFileBase::SkipEmptyChars_Reverse()
{
    char ch;
    while(this->GetOffset() > 0)
    {
        ch = this->UnGetc();
        if(!this->IsEmptyChar(ch) )
        {
            this->Getc();
            break;
        }
    }
    return OK;
}
//the return value is the number of bytes it reads.
fsize_t CFileBase::ReadWordWithEmptyChar_Reverse(CFileBase *file)
{
    fsize_t k = 0;
    char ch;
    
    ASSERT(file);

    file->SetSize(0);

    if(this->GetOffset() <= 0 )
        goto end;

    ch = this->UnGetc();
    if(ch == 0)
        goto end;
    
    if(this->IsSpChar(ch))
    {
        file->Putc(ch);
        k = 1;
        goto end;
    }
    while(!this->IsSpChar(ch))
    {
        file->Putc(ch);
		k++;

        if(this->GetOffset() <= 0)
            break;
 
        ch = this->UnGetc();
    }

    if(this->IsSpChar(ch))
        this->Getc();

end:        
    file->Reverse(0,file->GetOffset() - 1);
    return k;
}

fsize_t CFileBase::ReadWord_Reverse(CFileBase *file)
{
    this->SkipEmptyChars_Reverse();
    return this->ReadWordWithEmptyChar_Reverse(file);
}

char CFileBase::CharAt(fsize_t index)
{
    fsize_t old;
    char ch = 0;
    old = this->GetOffset();
    this->Seek(index);
    ch = this->Getc();
    this->Seek(old);
    return ch;
}

status_t CFileBase::SetChar(fsize_t index,char ch)
{
    fsize_t old;

    old = this->GetOffset();
    this->Seek(index);
    this->Putc(ch);
    this->Seek(old);
    return OK;
}

status_t CFileBase::Reverse(fsize_t start, fsize_t end)
{
    fsize_t i,t;
    char ch;
    
    t = end - start + 1;
    for(i = 0; i < t / 2; i++)
    {
        ch = this->CharAt(i + start);
        this->SetChar(i + start, this->CharAt(start + t - i -1 ));
        this->SetChar(start + t - i - 1, ch);
    }

    return OK;
}

status_t CFileBase::ReadString_Reverse(CFileBase *file)
{
    fsize_t k;
    k = this->ReadWord_Reverse(file);
    while(k)
    {
        file->Seek(0);
        if(!this->IsSpChar(file->Getc()))   
        {
            return OK;
        }
        k = this->ReadWord_Reverse(file);
    }
    return ERROR;
}

status_t CFileBase::InitBasic()
{
    WEAK_REF_CLEAR();
    this->SetDefaultSpChars();
#if _UNICODE_
    this->SetDefaultSpCharsW();
#endif
    this->user_data_ = NULL;
    this->file_name = NULL;
    this->log_tab_level_ = 0;
    this->is_sp_chars_malloc = false;
    return OK;
}

status_t CFileBase::Init()
{   
    this->InitBasic();    
    return OK;
}

char CFileBase::GetLastChar(bool empty_included)
{
    fsize_t old_off;
    char ch;

    old_off = this->GetOffset();
    this->Seek(this->GetSize() - 1);
    ch =  this->Getc();
    if(!empty_included)
    {
        while(IsEmptyChar(ch))
        {
            ch = this->UnGetc();
            if(this->GetOffset() <= 0)
                break;
        }
    }
    this->Seek(old_off);
    return ch;
}

status_t CFileBase::SetFileName(const char *fn)
{
    if(this->file_name)
    {
        FREE(this->file_name);
    }

    MALLOC(this->file_name,char,crt_strlen(fn) + 1);
    crt_strcpy(this->file_name,fn); 
    return OK;
}

const char *CFileBase::GetFileName()
{
    return this->file_name;
}

//if the first char is special char, the word_only is ignored
fsize_t CFileBase::SearchStr(const char *str, status_t case_sensive,status_t word_only)
{
    const char *s2;
    char ch1,ch2;
    fsize_t old,pos,len;
    fsize_t flag = 0;
    
    if(str == NULL) return -1;
    len = crt_strlen(str);
    while(!this->IsEnd())
    {
        old = this->GetOffset();        
        ch1 = this->Getc();
        s2 = str;
        ch2 = *s2;
        if(!case_sensive)
        {
            ch1 = crt_tolower(ch1);
            ch2 = crt_tolower(ch2);
        }
        while (ch2 && !(ch1 - ch2)) 
        {
            s2 ++;
            ch2 = *s2;
            
            if(this->IsEnd())
            {flag = 1;break;}
            
            ch1=this->Getc();
            if(!case_sensive)
            {
                ch1 = crt_tolower(ch1);
                ch2 = crt_tolower(ch2);
            }
        }
        if(!*s2)
        {
            pos = this->GetOffset() - len - 1;
            if(flag) pos++; //if hit end
            
            if(!word_only)
            {
                return pos;
            }
            else if(IsSpChar(CharAt(pos)))
            {
                return pos;
            }
            else
            {
                if((pos == 0||IsSpChar(CharAt(pos - 1)))&&IsSpChar(CharAt(pos + len)))
                    return pos;
            }

        }
        this->Seek(old + 1);
    }
    return -1;
}

status_t CFileBase::ReplaceStr(const char *src_str, const char *des_str, status_t case_sensive, status_t word_only,CFileBase *new_file)
{
    fsize_t pos,old_pos,len1;

    ASSERT(new_file);
    ASSERT(new_file != this);

    len1 = crt_strlen(src_str);
    new_file->SetSize(0);
    old_pos = this->GetOffset();
    if(old_pos > 0)
    {
        this->WriteToFile(new_file,0,old_pos);
    }

    this->Seek(old_pos);
    while(!this->IsEnd())
    {
        pos = this->SearchStr(src_str,case_sensive,word_only);
        if(pos < 0)
        {
            this->WriteToFile(new_file,old_pos,this->GetSize() - old_pos);
            break;
        }
        else
        {
            this->WriteToFile(new_file,old_pos,pos - old_pos);
            new_file->Puts(des_str);
            old_pos = pos + len1;
            this->Seek(old_pos);
        }
    }
    return OK;
}

status_t CFileBase::ReadQuoteStr(char escape_char,char end_char,CFileBase *file)
{
    ASSERT(file);

    char ch;

    file->SetSize(0);
    while(!this->IsEnd())
    {
        ch = this->Getc();
        
        if(ch == escape_char) //escape char
        {
            file->Putc(ch);
            ch = this->Getc();
            file->Putc(ch);
        }
        else if(ch == end_char)
        {
            break;
        }
        else
        {
            file->Putc(ch);
        }
    }
    return OK;
}

status_t CFileBase::ReadCStr(CFileBase *file)
{
    return this->ReadQuoteStr('\\','\"',file);
}

int CFileBase::Getc_GB(char *letter)
{
    ASSERT(letter);
    
    char ch;
    int ret = 0;

    ch = this->Getc();
    if((uint8_t)ch < 128)
    {
        letter[0] = ch;
        letter[1] = 0;
        ret = 1;
    }
    else
    {
        letter[0] = ch;
        letter[1] = this->Getc();
        ret = 2;
    }

    letter[2] = 0;
    return ret;
}

int CFileBase::Getc_Utf8(char *utf8)
{
    ASSERT(utf8);

    uint8_t b0 = (uint8_t)this->Getc();
    utf8[0] = b0;

    if(b0 < 0x80)
    {       
        utf8[1] = 0;
        return 1;
    }
    else
    {
        if((b0 & 0xe0)==0xc0)
        {
            utf8[1] = (uint8_t)this->Getc();
            utf8[2] = 0;
            return 2;
        }
        else
        {               
            if((b0 & 0xf0)==0xe0)
            {
                utf8[1] = (uint8_t)this->Getc();
                utf8[2] = (uint8_t)this->Getc();
                utf8[3] = 0;
                return 3;
            }
            else
            {
                utf8[1] = (uint8_t)this->Getc();
                utf8[2] = (uint8_t)this->Getc();
                utf8[3] = (uint8_t)this->Getc();
                utf8[4] = 0;
                return 4;
            }
        }
    }

    return 0;
}
status_t CFileBase::InStrList(const char *str, const char *str_list, status_t case_sensive)
{
    fsize_t c;

    CMem mem(str_list); 
    LOCAL_MEM(buf);

    mem.Seek(0);
    mem.SetSplitChars(",");
    while(mem.ReadWordWithEmptyChar(&buf))
    {
        if(buf.C(0) == ',')
            continue;
        if(case_sensive)
            c = crt_strcmp(buf.CStr(),str) == 0;
        else
            c = crt_stricmp(buf.CStr(),str) == 0;
        if(c)return TRUE;
    }
    return FALSE;
}

int_ptr_t CFileBase::StrLen()
{
    return (int_ptr_t)this->GetSize();
}

status_t CFileBase::StrCat(const char *str)
{
    this->SeekEnd();
    this->Puts(str);
    return OK;
}

int CFileBase::StrCmp(const char *str)
{
    CMem mem(str);
    return this->StrCmp(&mem);
}

status_t CFileBase::StrCat(CFileBase *file)
{
    this->SeekEnd();
    this->Puts(file);
    return OK;
}

int CFileBase::StrCmp(CFileBase *file)
{
    unsigned char c1,c2;

    if(this == file)
        return 0;

    this->Seek(0);
    file->Seek(0);
    
    fsize_t i = 0;
    fsize_t len1 = this->StrLen();
    fsize_t len2 = file->StrLen();
    while(i < len1 && i < len2)
    {
        c1 = this->Getc();
        c2 = file->Getc();
        if(c1 != c2)
            return c1 - c2;
        i++;
    }
    return (int)(len1 - len2);
}

int CFileBase::StrICmp(CFileBase *file)
{
    unsigned char c1,c2;

    if(this == file)
        return 0;

    this->Seek(0);
    file->Seek(0);
    
    fsize_t i = 0;
    fsize_t len1 = this->StrLen();
    fsize_t len2 = file->StrLen();
    while(i < len1 && i < len2)
    {
        c1 = this->Getc();
        c2 = file->Getc();
        c1 = crt_tolower(c1);
        c2 = crt_tolower(c2);
        if(c1 != c2)
            return c1 - c2;
        i++;
    }
    return (int)(len1 - len2);
}

int CFileBase::StrICmp(const char *str)
{
    CMem mem(str);
    return this->StrICmp(&mem);
}

status_t CFileBase::StrCpy(CFileBase *file)
{
    this->SetSize(0);
    this->Puts(file);
    return OK;
}

status_t CFileBase::StrCpy(const char *str)
{
    CMem mem(str);
    return this->StrCpy(&mem);
}
status_t CFileBase::EndWith(CFileBase *file_str, status_t case_sensive, status_t skip_empty)
{
    ASSERT(file_str);

    fsize_t i,len1,len2;
    unsigned char c1,c2;

    len1 = this->StrLen();
    len2 = file_str->StrLen();
    this->Seek(len1);
    file_str->Seek(len2);
    if(skip_empty)
    {
        while(len1 >= 0)
        {
            c1 = this->UnGetc();
            if(!IsEmptyChar(c1))
            {
                this->Getc();
                break;
            }

            len1 --;
        }
    }

    if(len1 <= 0 || len2 <= 0 || len1 < len2)
        return FALSE;

    for(i = len2 - 1; i >= 0; i--)
    {
        c1 = this->UnGetc();
        c2 = file_str->UnGetc();

        if(!case_sensive)
        {
            c1 = crt_tolower(c1);
            c2 = crt_tolower(c2);
        }

        if(c1 != c2)
            return FALSE;
    }

    return TRUE;
}
status_t CFileBase::EndWith(const char *str, status_t case_sensive, status_t skip_empty)
{
    CMem mem(str);
    return this->EndWith(&mem,case_sensive,skip_empty);
}
status_t CFileBase::StartWith(CFileBase *file_str, status_t case_sensive, status_t skip_empty)
{
    ASSERT(file_str);
    
    fsize_t i,len;
    unsigned char c1,c2;

    this->Seek(0);
    if(skip_empty)
        this->SkipEmptyChars();
    
    len = file_str->StrLen();
    if(this->GetSize() < len)
        return FALSE;
    file_str->Seek(0);
    for(i = 0; i < len; i++)
    {
        c1 = this->Getc();
        c2 = file_str->Getc();
        if( ! case_sensive)
        {
            c1 = crt_tolower(c1);
            c2 = crt_tolower(c2);
        }

        if(c1 != c2)
            return FALSE;       
    }

    return TRUE;
}

status_t CFileBase::StartWith(const char *str, status_t case_sensive, status_t skip_empty)
{
    CMem mem(str);
    return this->StartWith(&mem,case_sensive,skip_empty);
}

status_t CFileBase::Trim()
{
    fsize_t i,l,n;
    
    l = this->StrLen();
    for(i = l - 1; i >= 0; i--)
    {
        if(!IsEmptyChar(this->CharAt(i)))
            break;
    }
    l = i + 1;
    for(i = 0; i < l; i++)
    {
        if(!IsEmptyChar(this->CharAt(i)) )
            break;
    }
    l -= i;
    n = i;
    for(i = 0; i < l; i++)
    {
        this->SetChar(i,this->CharAt(i+n));
    }
    this->SetSize(l);
    return OK;
}
//read left to file, from current offset
fsize_t CFileBase::ReadLeftStr(CFileBase *file,status_t skip_empty)
{
    fsize_t ret = 0;
    file->SetSize(0);
    if(skip_empty)this->SkipEmptyChars();
    ret = this->WriteToFile(file,this->GetOffset(),this->GetSize() - this->GetOffset());
    return ret;
}

fsize_t CFileBase::Puts(CFileBase *file_str)
{
    ASSERT(file_str);
    fsize_t len = file_str->StrLen();
    return this->WriteFile(file_str,0,len);
}

fsize_t CFileBase::Split(CFileBase *file)
{
    ASSERT(file);

    char ch;
    fsize_t lines = 0;

    file->SetSize(0);
    this->Seek(0);
    while(!this->IsEnd())
    {
        ch = this->Getc();
        if(this->IsSpChar(ch))
        {
            file->Putc('\n');
            lines ++;
        }
        else
        {
            file->Putc(ch);
        }
    }
    return lines;
}

status_t CFileBase::ReadKeyValue(CFileBase *file_key, const char *split_chars, CFileBase *file_value)
{
    ASSERT(file_key && file_value && split_chars);
    status_t find = FALSE;
    
    file_value->SetSize(0);
    LOCAL_MEM(mem);
    LOCAL_MEM(buf);
    this->Seek(0);
    while(this->ReadLine(&mem))
    {
        if(mem.C(0) == 0)continue;
        mem.Seek(0);
        mem.SetSplitChars(split_chars);
        mem.ReadString(&buf);
        buf.Trim();
        if(buf.StrICmp(file_key) == 0)
        {
            mem.ReadWord(&buf);
            if(CFileBase::InStrList(buf.CStr(),split_chars,0))
            {
                mem.ReadLeftStr(file_value,1);
                find = TRUE;
                break;              
            }
        }
    }
    return find;
}

status_t CFileBase::ReadKeyValue(const char *key, CFileBase *file_value)
{
    ASSERT(key && file_value);
    CMem mem(key);
    return this->ReadKeyValue(&mem,":,=",file_value);
}

status_t CFileBase::LoadFile(const char *fn)
{
    FILE_HANDLE fp = crt_fopen(fn,"rb");
    if(!crt_is_file_handle(fp))
    {
        return ERROR;
    }
    this->LoadFile(fp);
    crt_fclose(fp);
    return OK;
}
fsize_t CFileBase::WriteToFile(const char *fn)
{
    return this->WriteToFile(fn,0,this->GetSize());     
}
fsize_t CFileBase::WriteToFile(const char *fn,fsize_t start,fsize_t wsize)
{
    fsize_t sum = 0;
    FILE_HANDLE fp = crt_fopen(fn , "wb+");
    if(!crt_is_file_handle(fp))
    {
        return -1;
    }
    sum = this->WriteToFile(fp,start,wsize);
    crt_fclose(fp);
    return sum;
}

int_ptr_t CFileBase::ReadZeroEndString(CFileBase *str)
{
    ASSERT(str);
    char ch;
    int_ptr_t size = 0;
    str->SetSize(0);
    while(!this->IsEnd())
    {
        ch = this->Getc();
        size ++;
        if(ch == 0)break;
        str->Putc(ch);
    }
    return size;
}

status_t CFileBase::StrLwr()
{
    fsize_t i,len = this->StrLen();
    for(i = 0; i < len; i++)
    {
        this->SetChar(i,crt_tolower(this->CharAt(i)));
    }
    return OK;
}

status_t CFileBase::StrUpr()
{
    fsize_t i,len = this->StrLen();
    for(i = 0; i < len; i++)
    {
        this->SetChar(i,crt_toupper(this->CharAt(i)));
    }
    return OK;
}
//////////////////////////////////////////////////////////////////////////
#endif //#if _ASC_
//////////////////////////////////////////////////////////////////////////
wchar_t CFileBase::GetcW()
{
    wchar_t w = 0;
    this->Read(&w,sizeof(wchar_t));
    return w;
}

fsize_t CFileBase::PutcW(wchar_t uc)
{
    return this->Write(&uc,sizeof(wchar_t));
}
//////////////////////////////////////////////////////////////////////////
#if _UNICODE_
//////////////////////////////////////////////////////////////////////////
wchar_t CFileBase::CharAtW(fsize_t index)
{
    wchar_t u;
    fsize_t old_off = this->GetOffset();

    this->Seek(index * sizeof(wchar_t));
    this->Read(&u,sizeof(wchar_t));
    this->Seek(old_off);
    return u;
}
status_t CFileBase::SetCharW(fsize_t index, wchar_t uc)
{
    fsize_t old_off = this->GetOffset();

    this->Seek(index * sizeof(wchar_t));
    this->Write(&uc,sizeof(wchar_t));
    this->Seek(old_off);
    return OK;
}

int_ptr_t CFileBase::StrLenW()
{
    return (int_ptr_t)(this->GetSize()/sizeof(wchar_t));    
}

int CFileBase::StrCmpW(CFileBase *file)
{
    wchar_t c1,c2;

    if(this == file)
        return 0;

    this->Seek(0);
    file->Seek(0);
    fsize_t i = 0;
    fsize_t len1 = this->StrLenW();
    fsize_t len2 = file->StrLenW();

    while(i < len1 && i < len2)
    {
        c1 = this->GetcW();
        c2 = file->GetcW();
        if(c1 != c2)
            return c1 - c2;
        i++;
    }
    return (int)(len1 - len2);
}

int CFileBase::StrICmpW(CFileBase *file)
{
    wchar_t c1,c2;

    if(this == file)
        return 0;

    this->Seek(0);
    file->Seek(0);
    fsize_t i = 0;
    fsize_t len1 = this->StrLenW();
    fsize_t len2 = file->StrLenW();
    while(i < len1 && i < len2)
    {
        c1 = this->GetcW();
        c2 = file->GetcW();
        c1 = crt_tolwr_w(c1);
        c2 = crt_tolwr_w(c2);
        if(c1 != c2)
            return c1 - c2;
        i++;
    }
    return (int)(len1 - len2);
}

status_t CFileBase::StrCpyW(CFileBase *file)
{
    this->SetSize(0);
    this->PutsW(file);
    return OK;
}

status_t CFileBase::StrCpyW(const wchar_t *str)
{
    CMem mem(str);
    return this->StrCpyW(&mem);
}

int CFileBase::StrICmpW(const wchar_t *str)
{
    CMem mem(str);
    return this->StrICmpW(&mem);
}

int CFileBase::StrCmpW(const wchar_t *str)
{
    CMem mem(str);
    return this->StrCmpW(&mem);
}

status_t CFileBase::StrLwrW()
{
    this->Seek(0);
    fsize_t i,len = this->StrLenW();
    for(i = 0; i < len; i++)
    {
        this->SetCharW(i,crt_tolwr_w(this->CharAtW(i)));
    }
    return OK;
}

status_t CFileBase::StrCatW(CFileBase *file)
{
    this->SeekEnd();
    this->PutsW(file);
    return OK;
}

status_t CFileBase::StrCatW(const wchar_t *str)
{
    this->SeekEnd();
    this->PutsW(str);
    return OK;
}
fsize_t CFileBase::PutsW(const wchar_t *str)
{
    ASSERT(str);
    return this->Write(str,crt_strlen_w(str) * sizeof(wchar_t));
}
fsize_t CFileBase::PutsW(CFileBase *file_str)
{
    ASSERT(file_str);
    fsize_t len = file_str->StrLenW();
    return this->WriteFile(file_str,0,len*sizeof(wchar_t));
}
fsize_t CFileBase::PrintfW(const wchar_t *szFormat, ...)
{
    wchar_t szBuffer [FILEBASE_LBUF_SIZE];
    crt_va_list pArgList ;
    crt_va_start (pArgList, szFormat);
    crt_wvsprintf(szBuffer, szFormat, pArgList) ;
    crt_va_end (pArgList) ;
    return this->PutsW(szBuffer);   
}

status_t CFileBase::IsEmptyCharW(wchar_t uc)
{
    return uc == 0 || uc == L'\t' || uc == L'\n' || uc == L'\r' || uc == L' ';
}
status_t CFileBase::SetDefaultSpCharsW()
{
    this->sp_chars_w = L" \t\r\n!@#$%^=&*()|+-\\*/{}[];.\":\',?/<>~";
    return OK;
}

status_t CFileBase::ReadLineW(CFileBase *file)
{
    const wchar_t *uc=L"\r\n";
    wchar_t ch;
    fsize_t ret;

    ASSERT(file);

    if(this->IsEnd()) 
        return ERROR;

    file->SetSize(0);
    ret = this->Read(&ch,sizeof(wchar_t));
    if(ret <= 1)
        return ERROR;
    
    while(ch != uc[1])
    {
        if(ch!=uc[0])
        {
            file->PutcW(ch);
        }
        
        if(this->IsEnd())
            break;
        
        ret = this->Read(&ch,sizeof(wchar_t));
        if(ret <= 1)
            return ERROR;
    }
    return OK;
}
status_t CFileBase::ReadLine_ReverseW()
{
    wchar_t ch;
    const wchar_t *uc=L"\n";
    
    ch = this->UnGetcW();
    if(ch != uc[0])
        this->PutcW(ch);

    while(1)
    {
        ch = this->UnGetcW();
        if(this->GetOffset() <= 0)
            return OK;
        if(ch == uc[0])
        {
            this->PutcW(ch);
            break;
        }
    }
    return OK;
}
wchar_t CFileBase::UnGetcW()
{
    wchar_t ch = 0;
    fsize_t off;
    
    off = this->GetOffset();
    if(off > 0)
    {
        off -= sizeof(wchar_t);
        this->Seek(off);
        off = this->GetOffset();
        ch =  this->GetcW();
        this->Seek(off);
    }
    return ch;
}

status_t CFileBase::ReadLine_ReverseW(CFileBase *file)
{
    fsize_t off;

    this->ReadLine_ReverseW();  
    off = this->GetOffset();
    this->ReadLineW(file);
    this->Seek(off);
    
    return OK;
}
//the return value is the number of words it reads.
fsize_t CFileBase::ReadWordWithEmptyCharW(CFileBase *file)
{
    ASSERT(file);

    fsize_t k = 0;
    wchar_t ch;
    
    file->SetSize(0);
    if(this->IsEnd())return k;

    ch = this->GetcW();
    if(ch == 0)return k;
    
    if(this->IsSpCharW(ch))
    {
        file->PutcW(ch);
        k = 1;
        return k;
    }
    while(!this->IsSpCharW(ch))
    {
        file->PutcW(ch);
        k++;
        if(this->IsEnd())
            break;
        ch = this->GetcW();
    }
    if(this->IsSpCharW(ch))
        this->UnGetcW();
    return k;
}
status_t CFileBase::IsSpCharW(wchar_t ch)
{
    const wchar_t *p;

    if(ch == 0 || ch == L'\n')
        return TRUE;

    p = this->sp_chars_w;
    while(*p)
    {
        if(*p == ch)
            return TRUE;
        p++;
    }
    
    return FALSE;
}

//the return value is the number of bytes it reads.
fsize_t CFileBase::ReadWordWithEmptyCharW_Reverse(CFileBase *file)
{
    fsize_t k = 0;
    wchar_t ch;
    
    ASSERT(file);

    file->SetSize(0);

    if(this->GetOffset() <= 0 )
        goto end;

    ch = this->UnGetcW();
    if(ch == 0)
        goto end;
    
    if(this->IsSpCharW(ch))
    {
        file->PutcW(ch);
        k = 1;
        goto end;
    }
    while(!this->IsSpCharW(ch))
    {
        file->PutcW(ch);
		k++;

        if(this->GetOffset() <= 0)
            break;
        ch = this->UnGetcW();
    }

    if(this->IsSpCharW(ch))
        this->GetcW();
end:        
    file->ReverseW(0,file->GetOffset() - 1);
    return k;
}

status_t CFileBase::ReverseW(fsize_t start, fsize_t end)
{
    fsize_t i,t;
    wchar_t ch;
    
    t = end - start + 1;
    for(i = 0; i < (fsize_t)(t/sizeof(wchar_t)); i++)
    {
        ch = this->CharAtW(i + start);
        this->SetCharW(i + start, this->CharAtW(start + t - i -1 ));
        this->SetCharW(start + t - i - 1, ch);
    }

    return OK;
}
status_t CFileBase::SkipEmptyCharsW()
{
    wchar_t ch;
    while(!this->IsEnd())
    {
        ch = this->GetcW();
        if( ! this->IsEmptyCharW(ch) )
        {
            this->UnGetcW();
            break;
        }
    }
    return OK;
}
status_t CFileBase::SkipEmptyCharsW_Reverse()
{
    wchar_t ch;
    while(this->GetOffset() > 0)
    {
        ch = this->UnGetcW();
        if( ! this->IsEmptyCharW(ch) )
        {
            this->GetcW();
            break;
        }
    }
    return OK;
}
status_t CFileBase::SetSplitCharsW(const wchar_t *sp_str)
{
    if(sp_str)
        this->sp_chars_w = sp_str;
    return OK;
}
fsize_t CFileBase::ReadWordW(CFileBase *file)
{
    this->SkipEmptyCharsW();
    return this->ReadWordWithEmptyCharW(file);
}
status_t CFileBase::ReadStringW(CFileBase *file)
{
    fsize_t k;
    k = this->ReadWordW(file);
    while(k)
    {
        file->Seek(0);
        if(!this->IsSpCharW(file->GetcW())) 
        {
            return OK;
        }
        k = this->ReadWordW(file);
    }
    return ERROR;
}
fsize_t CFileBase::ReadWordW_Reverse(CFileBase *file)
{
    this->SkipEmptyCharsW_Reverse();
    return this->ReadWordWithEmptyCharW_Reverse(file);
}
status_t CFileBase::ReadStringW_Reverse(CFileBase *file)
{
    fsize_t k;
    k = this->ReadWordW_Reverse(file);
    while(k)
    {
        file->Seek(0);
        if(!this->IsSpCharW(file->GetcW())) 
        {
            return OK;
        }
        k = this->ReadWordW_Reverse(file);
    }
    return ERROR;
}
//if the first char is special char, the word_only is ignored
fsize_t CFileBase::SearchStrW(const wchar_t *str, status_t case_sensive,status_t word_only)
{
    const wchar_t *s2;
    wchar_t ch1,ch2;
    fsize_t old,pos,len;
    fsize_t flag = 0;
    
    if(str == NULL)
        return -1;

    len = crt_strlen_w(str);
    
    while(!this->IsEnd())
    {
        old = this->GetOffset();        
        ch1 = this->GetcW();
        s2 = str;
        ch2 = *s2;
        if(!case_sensive)
        {
            ch1 = crt_tolwr_w(ch1);
            ch2 = crt_tolwr_w(ch2);
        }
        while (ch2 && !(ch1 - ch2)) 
        {
            s2 ++;
            ch2 = *s2;
            
            if(this->IsEnd())
            {flag = 1;break;}
            
            ch1=this->GetcW();
            if(!case_sensive)
            {
                ch1 = crt_tolwr_w(ch1);
                ch2 = crt_tolwr_w(ch2);
            }
        }
        if(!*s2)
        {
            pos = (this->GetOffset()/sizeof(wchar_t)) - len - 1;
            if(flag) pos++; //if hit end
            
            if(!word_only)
            {
                return pos;
            }
            else if(IsSpCharW(CharAtW(pos)))
            {
                return pos;
            }
            else
            {
                if((pos == 0||IsSpCharW(CharAtW(pos - 1)))&&IsSpCharW(CharAtW(pos + len)))
                    return pos;
            }

        }
        this->Seek(old + sizeof(wchar_t));
    }
    return -1;
}
status_t CFileBase::ReplaceStrW(const wchar_t *src_str, const wchar_t *des_str, status_t case_sensive, status_t word_only,CFileBase *new_file)
{
    fsize_t pos,old_pos,len1;

    ASSERT(new_file);
    ASSERT(new_file != this);

    len1 = crt_strlen_w(src_str);

    new_file->SetSize(0);
    
    old_pos = this->GetOffset();
    if(old_pos > 0)
    {
        this->WriteToFile(new_file,0,old_pos);
    }

    this->Seek(old_pos);

    while(!this->IsEnd())
    {
        pos = this->SearchStrW(src_str,case_sensive,word_only);
        if(pos < 0)
        {
            this->WriteToFile(new_file,old_pos,this->GetSize() - old_pos);
            break;
        }
        else
        {
            this->WriteToFile(new_file,old_pos,(pos*sizeof(wchar_t)) - old_pos);
            new_file->PutsW(des_str);
            old_pos = (pos + len1) * sizeof(wchar_t);

            this->Seek(old_pos);
        }
    }
    return OK;
}

wchar_t CFileBase::GetLastCharW(bool empty_included)
{
    fsize_t old_off;
    wchar_t ch;

    old_off = this->GetOffset();
    this->Seek(this->GetSize() - sizeof(wchar_t));
    ch =  this->GetcW();
    if(!empty_included)
    {
        while(IsEmptyCharW(ch))
        {
            ch = this->UnGetcW();
            if(this->GetOffset() <= 0)
                break;
        }
    }
    this->Seek(old_off);
    return ch;
}
status_t CFileBase::ReadLineW(fsize_t line_index,CFileBase *file)
{
    ASSERT(file);
    
    fsize_t i = 0;

    this->Seek(0);
    while(this->ReadLineW(file))
    {
        if(i == line_index)
        {
            return OK;
        }
        i++;    
    }
    file->SetSize(0);
    return ERROR;
}
status_t CFileBase::EndWithW(CFileBase *file_str, status_t case_sensive, status_t skip_empty)
{
    ASSERT(file_str);

    fsize_t i,len1,len2;
    wchar_t c1,c2;

    len1 = this->StrLenW();
    len2 = file_str->StrLenW();

    this->Seek(len1 * sizeof(wchar_t));
    file_str->Seek(len2 * sizeof(wchar_t));

    if(skip_empty)
    {
        while(len1 >= 0)
        {
            c1 = this->UnGetcW();
            if(!IsEmptyCharW(c1))
            {
                this->GetcW();
                break;
            }

            len1 --;
        }
    }

    if(len1 <= 0 || len2 <= 0 || len1 < len2)
        return FALSE;

    for(i = len2 - 1; i >= 0; i--)
    {
        c1 = this->UnGetcW();
        c2 = file_str->UnGetcW();

        if(!case_sensive)
        {
            c1 = crt_tolwr_w(c1);
            c2 = crt_tolwr_w(c2);
        }

        if(c1 != c2)
            return FALSE;
    }

    return TRUE;
}
status_t CFileBase::EndWithW(const wchar_t *str, status_t case_sensive, status_t skip_empty)
{
    CMem mem(str);
    return this->EndWithW(&mem,case_sensive,skip_empty);
}
status_t CFileBase::StartWithW(CFileBase *file_str, status_t case_sensive, status_t skip_empty)
{
    ASSERT(file_str);
    
    fsize_t i,len;
    wchar_t c1,c2;

    this->Seek(0);
    if(skip_empty)
        this->SkipEmptyCharsW();
    
    len = file_str->StrLenW();

    if(this->StrLenW() < len)
        return FALSE;

    file_str->Seek(0);
    for(i = 0; i < len; i++)
    {
        c1 = this->GetcW();
        c2 = file_str->GetcW();
        if( ! case_sensive)
        {
            c1 = crt_tolwr_w(c1);
            c2 = crt_tolwr_w(c2);
        }

        if(c1 != c2)
            return FALSE;       
    }

    return TRUE;
}

status_t CFileBase::StartWithW(const wchar_t *str, status_t case_sensive, status_t skip_empty)
{
    CMem mem(str);
    return this->StartWith(&mem,case_sensive,skip_empty);
}
status_t CFileBase::InStrListW(const wchar_t *str, const wchar_t *str_list, status_t case_sensive)
{
    fsize_t c;

    CMem mem(str);  
    LOCAL_MEM(buf);

    mem.Seek(0);
    mem.SetSplitCharsW(L",");

    while(mem.ReadWordWithEmptyCharW(&buf))
    {
        if(buf.CharAtW(0) == L',')
            continue;
        if(case_sensive)
            c = crt_strcmp_w(buf.CStrW(),str) == 0;
        else
            c = crt_stricmp_w(buf.CStrW(),str) == 0;
        if(c)
            return TRUE;
    }
    return FALSE;
}

status_t CFileBase::ReadKeyValueW(CFileBase *file_key, const wchar_t *split_chars, CFileBase *file_value)
{
    ASSERT(file_key && file_value && split_chars);

    status_t find = FALSE;
    file_value->SetSize(0);

    LOCAL_MEM(mem);
    LOCAL_MEM(buf);
    
    this->Seek(0);
    while(this->ReadLineW(&mem))
    {
        if(mem.CharAtW(0) == 0)
            continue;

        mem.Seek(0);
        mem.SetSplitCharsW(split_chars);
        mem.ReadStringW(&buf);
        buf.TrimW();
        if(buf.StrICmpW(file_key) == 0)
        {
            mem.ReadWordW(&buf);
            if(CFileBase::InStrListW(buf.CStrW(),split_chars,0))
            {
                mem.ReadLeftStrW(file_value,1);
                find = TRUE;
                break;              
            }
        }
    }

    return find;
}

status_t CFileBase::ReadKeyValueW(const wchar_t *key, CFileBase *file_value)
{
    CMem mem(key);
    return this->ReadKeyValueW(&mem,L":,=",file_value);
}

status_t CFileBase::TrimW()
{
    fsize_t i,l,n;
    
    l = this->StrLenW();

    for(i = l - 1; i >= 0; i--)
    {
        if(!IsEmptyCharW(this->CharAtW(i)))
            break;
    }
    
    l = i + 1;
    for(i = 0; i < l; i++)
    {
        if( ! IsEmptyCharW(this->CharAtW(i)) )
            break;
    }
    l -= i;
    n = i;
    for(i = 0; i < l; i++)
    {
        this->SetCharW(i,this->CharAtW(i+n));
    }
    this->SetSize(l*sizeof(wchar_t));
    return OK;
}
//read left to file, from current offset
fsize_t CFileBase::ReadLeftStrW(CFileBase *file,status_t skip_empty)
{
    fsize_t ret = 0;
    file->SetSize(0);
    if(skip_empty)this->SkipEmptyCharsW();
    ret = this->WriteToFile(file,this->GetOffset(),this->GetSize()-this->GetOffset());
    return ret;
}

fsize_t CFileBase::SplitW(CFileBase *file)
{
    ASSERT(file);

    wchar_t ch;
    fsize_t lines = 0;

    file->SetSize(0);
    this->Seek(0);
    while(!this->IsEnd())
    {
        ch = this->GetcW();
        if(this->IsSpCharW(ch))
        {
            file->PutcW(L'\n');
            lines ++;
        }
        else
        {
            file->PutcW(ch);
        }
    }
    return lines;
}
//////////////////////////////////////////////////////////////////////////
#endif //#if _UNICODE_
//////////////////////////////////////////////////////////////////////////

