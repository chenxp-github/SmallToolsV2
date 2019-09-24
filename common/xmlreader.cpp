// CXmlReader.cpp: implementation of the CXmlReader class.
//
//////////////////////////////////////////////////////////////////////
#include "xmlreader.h"
#include "syslog.h"
#include "mem_tool.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CXmlReader::CXmlReader()
{
    this->i_file_xml = NULL;
    this->mode = XML_MODE_NORMAL;
    this->mf_attrib = NULL;
    this->mf_value = NULL;
    this->mem_stk = NULL;
    this->value_type = VALUE_TYPE_ERR;
}
CXmlReader::~CXmlReader()
{
    this->Destroy();
}
status_t CXmlReader::Init()
{
    this->i_file_xml = NULL;
    this->mode = XML_MODE_NORMAL;
    this->mf_attrib = NULL;
    this->mf_value = NULL;
    this->mem_stk = NULL;
    this->value_type = VALUE_TYPE_ERR;
    NEW(this->mf_attrib,CMemFile);
    NEW(this->mf_value,CMemFile);
    NEW(this->mem_stk,CMemStk);
    this->mf_attrib->Init();
    this->mf_value->Init();
    this->mem_stk->Init(256);
    return OK;
}
status_t CXmlReader::Destroy()
{
    DEL(this->mf_attrib);
    DEL(this->mf_value);
    DEL(this->mem_stk);
    return OK;
}
status_t CXmlReader::EnterNextItem()
{
    char ch;
    fsize_t n,b_off;
    int ret_code = ERROR;
    CMem *pstr;
    LOCAL_MEM_WITH_SIZE(buf,1024);
    char *pbuf = buf.GetRawBuf();

    this->mf_value->SetSize(0);
    this->mf_attrib->SetSize(0);
    while(!i_file_xml->IsEnd())
    {
        /*======================================================*/
        if(this->mode == XML_MODE_NORMAL)
        {
            while(1)
            {   if(this->i_file_xml->IsEnd())
                    goto err;
                ch = this->i_file_xml->Getc();
                if(ch =='<') break;

                this->mf_value->Putc(ch);

            }
            this->mode = XML_MODE_ITEM;
        }//if(this->mode == XML_MODE_NORMAL)
        /*======================================================*/
        if(this->mode == XML_MODE_ITEM)
        {
            ch = this->Getc_NotWhite(this->i_file_xml);
            if(ch == '>')
            {
                b_off = this->i_file_xml->GetOffset();
                ch = this->UnGetc_NotWhite(this->i_file_xml);
                ch = this->UnGetc_NotWhite(this->i_file_xml);
                if(ch != '/')
                {
                    this->i_file_xml->Seek(b_off);
                    this->value_type = VALUE_TYPE_NORMAL;
                    this->mode = XML_MODE_NORMAL;
                    ret_code = XML_RET_ENTER;
                    goto ok;
                }
                else //for single item
                {
                    //this->mem_stk->DelTop(); //do not delete stack top here
                    this->i_file_xml->Seek(b_off);
                    this->value_type = VALUE_TYPE_NORMAL;
                    this->mode = XML_MODE_NORMAL;
                    ret_code = XML_RET_SINGLE;
                    goto ok;
                }
            }
            else if(ch != '-' && ch != '!' && ch != '/' && ch != '?')
            {
                this->UnGetc_NotWhite(this->i_file_xml);
                this->i_file_xml->ReadString(&buf);
                this->mem_stk->Push(&buf);
                while( TRUE )
                {
                    n = this->i_file_xml->ReadWord(&buf);
                    if(n <= 0) goto err;

                    if(buf.C(0) == '/')
                    {
                        ch = this->i_file_xml->Getc();
                        if(ch == '>')
                        {
                            //this->mem_stk->DelTop(); //do not delete stack top here
                            this->value_type = VALUE_TYPE_NORMAL;
                            this->mode = XML_MODE_NORMAL;
                            ret_code = XML_RET_SINGLE;
                            goto ok;
                        }
                    }
                    if(buf.C(0) == '>')
                    {
                        this->value_type = VALUE_TYPE_NORMAL;
                        this->mode = XML_MODE_NORMAL;
                        ret_code = XML_RET_ENTER;
                        goto ok;
                    }
                    this->mf_attrib->Puts(&buf);
                    if(this->Skip("=") == ERROR)
                        goto err;
                    this->mf_attrib->Puts("=\"");

                    if(this->Skip("\"") == ERROR)
                        goto err;
                    if(!this->i_file_xml->ReadCStr(&buf))
                        goto err;
                    this->mf_attrib->Puts(&buf);
                    this->mf_attrib->Puts("\"\r\n");
                }
            }
            else if(ch == '/')
            {
                this->i_file_xml->ReadString(&buf);
                if( this->Skip(">") == ERROR)
                    goto err;
                this->mode = XML_MODE_NORMAL;
                pstr = this->mem_stk->GetTopPtr();
                if(pstr == NULL)goto err;
                if(crt_strcmp(pstr->CStr(),buf.CStr()) == 0)
                {
                    //this->mem_stk->DelTop(); //do not del stack top here
                    ret_code = XML_RET_LEAVE;
                    goto ok;
                }
                else
                {
                    XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
                        "syntax error token matches  %s\n",buf.CStr()
                    );
                    //goto err;  // add some fault tolerence
                }
            }
            else if(ch == '!')
            {
                b_off = this->i_file_xml->GetOffset();
                this->i_file_xml->SkipEmptyChars();
                this->i_file_xml->Read(buf.GetRawBuf(),7);
                buf.SetSize(7);

                if(crt_strcmp(buf.CStr(),"[CDATA[") == 0)
                {
                    this->mode = XML_MODE_CDATA;
                }
                else
                {
                    this->i_file_xml->Seek(b_off);
                    b_off = this->i_file_xml->GetOffset();
                    this->i_file_xml->SkipEmptyChars();
                    this->i_file_xml->Read(pbuf,2);
                    if(pbuf[0] == '-' && pbuf[1] == '-')
                    {
                        this->mode = XML_MODE_COMMENT;
                    }
                    else
                    {
                        while(!this->i_file_xml->IsEnd())
                        {
                            ch = this->i_file_xml->Getc();
                            if(ch == '>')
                            {
                                this->mode = XML_MODE_NORMAL;
                                break;
                            }
                        }
                    }
                }
            }//if(ch == '!')
            else if(ch == '?')
            {
                while(!this->i_file_xml->IsEnd())
                {
                    ch = this->i_file_xml->Getc();
                    if(ch == '?')
                    {
                        ch = this->Getc_NotWhite(this->i_file_xml);
                        if(ch == '>')
                        {
                            this->mode = XML_MODE_NORMAL;
                            break;
                        }
                        else
                        {
                            this->i_file_xml->UnGetc();
                        }
                    }
                }
            }//if(ch == '?')
        } //mode == XML_MODE_ITEM
        /*======================================================*/
        if(this->mode == XML_MODE_CDATA)
        {
            while(! this->i_file_xml->IsEnd())
            {
                ch = this->i_file_xml->Getc();
                if(ch == ']')
                {
                    b_off = this->i_file_xml->GetOffset();
                    this->i_file_xml->Read(pbuf,2);
                    if(pbuf[0] == ']' && pbuf[1] == '>')
                    {
                        this->mode = XML_MODE_NORMAL;
                        break;
                    }
                    else
                    {
                        this->i_file_xml->Seek(b_off);
                    }
                }
                this->mf_value->Putc(ch);
            }
            this->value_type = VALUE_TYPE_CDATA;
        }
        /*======================================================*/
        if(this->mode == XML_MODE_COMMENT)
        {
            while(! this->i_file_xml->IsEnd())
            {
                ch = this->i_file_xml->Getc();
                if(ch == '-')
                {
                    b_off = this->i_file_xml->GetOffset();
                    this->i_file_xml->Read(pbuf,2);
                    if(pbuf[0] == '-' && pbuf[1] == '>')
                    {
                        this->mode = XML_MODE_NORMAL;
                        break;
                    }
                    else
                    {
                        this->i_file_xml->Seek(b_off);
                    }
                }
            }
        }
        /*======================================================*/
    } //while(1)
ok:
    return ret_code;
err:
    return ERROR;
}
status_t CXmlReader::Skip(const char *str)
{
    LOCAL_MEM_WITH_SIZE(buf,1024);
    this->i_file_xml->ReadWord(&buf);
    if(crt_strcmp(buf.CStr(),str) == 0)
        return OK;
    XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
        "syntax error is expected.\n",str
    );
    return ERROR;
}
status_t CXmlReader::IsFileEmpty(CFileBase *mf)
{
    status_t ret = TRUE;
    ASSERT(mf);
    mf->Seek(0);
    while( !mf->IsEnd() )
    {
        if( ! CFileBase::IsEmptyChar(mf->Getc()) )
        {
            ret = FALSE;
            break;
        }
    }
    return ret;
}
status_t CXmlReader::GetCurPath(CFileBase *file)
{
    return this->mem_stk->SavePath(file);
}
status_t CXmlReader::SetXmlFile(CFileBase *file)
{
    ASSERT(file);
    file->SetSplitChars(" =!?\t\r\n\"\'?/<>");
    file->Seek(0);
    this->i_file_xml = file;
    return OK;
}
char CXmlReader::Getc_NotWhite(CFileBase *file)
{
    ASSERT(file);
    char ch;

    while(!file->IsEnd())
    {
        ch = file->Getc();
        if(!CFileBase::IsEmptyChar(ch))
            return ch;
    }
    return 0;
}
char CXmlReader::UnGetc_NotWhite(CFileBase *file)
{
    ASSERT(file);

    char ch;
    while(file->GetOffset() > 0)
    {
        ch = file->UnGetc();
        if(!CFileBase::IsEmptyChar(ch))
            return ch;
    }
    return 0;
}
