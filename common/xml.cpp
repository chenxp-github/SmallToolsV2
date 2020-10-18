// Xml.cpp: implementation of the CXml class.
//
//////////////////////////////////////////////////////////////////////
#include "xml.h"
#include "syslog.h"
#include "mem_tool.h"
#include "misc.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CXmlNode::CXmlNode()
{
    WEAK_REF_CLEAR();
    this->next = NULL;
}

CXmlNode::~CXmlNode()
{
    this->Destroy();
}

status_t CXmlNode::Init()
{            
    this->mf_attrib = NULL;
    this->mf_value = NULL;
    this->mem_name = NULL;
    NEW(this->mem_name,CMem);
    this->mem_name->Init();
    this->mem_name->Malloc(1024);

    this->next=NULL;
    this->child = NULL;
    this->parent = NULL;
    this->tail = NULL;
    this->value_type = VALUE_TYPE_NORMAL;
    return OK;
}

status_t CXmlNode::Destroy()
{
    WEAK_REF_DESTROY();
    DEL(this->mem_name);
    DEL(this->mf_attrib);
    DEL(this->mf_value);
    return OK;
}

status_t CXmlNode::Print()
{
    CXmlNode *p;
    static fsize_t l=0;
    fsize_t i;
    
    for(i=0;i<2*l;i++)
        syslog_printf(" ");

    syslog_printf("<%s>\n",this->mem_name->CStr());
    l++;
    /////////////
    p = this->child;
    while(p)
    {
        p -> Print();
        p = p->next;
    }
    l--;
    for(i=0;i<2*l;i++)
        syslog_printf(" ");

    syslog_printf("</%s>\n",this->mem_name->CStr());

    return OK;
}

CXmlNode * CXmlNode::Alloc()
{
    CXmlNode *p;
    NEW(p,CXmlNode);
    p->Init();
    return p;
}

status_t CXmlNode::Free(CXmlNode *node)
{
    CXmlNode *p , *q;

    if(node == NULL)
        return OK;

    p = node->child;
    if( p ) q = p->next;
    while(p)
    {
        CXmlNode::Free(p);
        p = q;
        if(q != NULL)
            q = q->next;
    }
    DEL( node );
    return OK;
}

status_t CXmlNode::AddChild(CXmlNode *node)
{
    if(node == NULL)
        return ERROR;

    if(tail == NULL)
    {
        ASSERT(child == NULL);
        this->child = node;
        node->next = NULL;
        tail = node;
    }
    else
    {
        tail->next = node;
        node->parent = tail->parent;
        node->next = NULL;
        tail = node;
    }
    return OK;
}

CXmlNode * CXmlNode::GetChild(int i)
{
    int c = 0;
    CXmlNode *p;
    if(i < 0)
        return NULL;

    p = this->child;
    while(p)
    {
        if(c == i)
            return p;
        c++;
        p = p->next;
    }
    return NULL;
}

CXmlNode *CXmlNode::GetChild()
{
    return this->child;
}

CXmlNode *CXmlNode::GetNext()
{
    return this->next;
}

CXmlNode *CXmlNode::GetParent()
{
    return this->parent;
}

status_t CXmlNode::AddAttrib(CFileBase *file)
{
    ASSERT(file);
    if(this->mf_attrib == NULL)
    {
        NEW(this->mf_attrib,CMemFile);
        this->mf_attrib->Init(1024,16);
    }
    this->mf_attrib->WriteFile(file);
    this->mf_attrib->Seek(0);
    return OK;
}

status_t CXmlNode::AddAttrib(const char *attrib, const char *val)
{
    if(this->mf_attrib == NULL)
    {
        NEW(this->mf_attrib,CMemFile);
        this->mf_attrib->Init(1024,16);
    }
    this->mf_attrib->Puts(attrib);
    this->mf_attrib->Puts("=\"");
    this->mf_attrib->Puts(val);
    this->mf_attrib->Puts("\"\r\n");
    return OK;
}

status_t CXmlNode::SetName(CMem *name)
{
    ASSERT(name);
    ASSERT(this->mem_name);
    this->mem_name->SetSize(0);
    this->mem_name->Puts(name);
    return OK;
}

status_t CXmlNode::SetName(const char *name)
{
    CMem mem(name);
    return SetName(&mem);
}

status_t CXmlNode::AddValueData(const char *str)
{
    CMem mem(str);
    return this->AddValueData(&mem);
}

status_t CXmlNode::AddValueData(CFileBase *file)
{
    char ch;
    fsize_t start,end;
    ASSERT(file);
    file->Seek(0);
    while( !file->IsEnd() )
    {
        ch = file->Getc();
        if(!CFileBase::IsEmptyChar(ch))
            break;
    }

    start = file->GetOffset() - 1;
    file->SeekEnd();
    while( file->GetOffset() > 0 )
    {
        ch = file->UnGetc();
        if(!CFileBase::IsEmptyChar(ch))
            break;
    }
    end = file->GetOffset() + 1;
    if(end - start > 0)
    {
        if(this->mf_value == NULL)
        {
            NEW(this->mf_value,CMemFile);
            this->mf_value->Init(1024,1024);
            this->mf_value->Seek(0);
        }
        this->mf_value->WriteFile(file,start,end-start);
        this->mf_value->Seek(0);
    }

    return OK;
}

status_t CXmlNode::SetValueType(int type)
{
    this->value_type = type;
    return OK;
}

status_t CXmlNode::WriteToFile(CFileBase *file)
{
    ASSERT(file);

    CXmlNode *p;
    LOCAL_MEM(mem_buf);

    file->Puts("\r\n<");
    file->Puts(this->mem_name->CStr());

    if(this->mf_attrib)
    {
        file->Putc(' ');
        this->mf_attrib->Seek(0);
        while(this->mf_attrib->ReadLine(&mem_buf))
        {
            if(mem_buf.C(0) == 0) continue;
            file->Puts(&mem_buf);
            file->Putc(' ');
        }
    }
    file->Puts(">\r\n");
    if(this->value_type == VALUE_TYPE_CDATA)
    {
        if(this->mf_value)
        {
            file->Puts("<![CDATA[\r\n");
            file->WriteFile(this->mf_value);
            file->Puts("\r\n]]>");
        }
    }
    if(this->value_type == VALUE_TYPE_NORMAL)
    {
        if(this->mf_value)
        {
            file->WriteFile(this->mf_value);
        }
    }
    /////////////
    p = this->child;
    while(p)
    {
        p->WriteToFile(file);
        p = p->next;
    }
    file->Puts("\r\n</");
    file->Puts(this->mem_name->CStr());
    file->Puts(">\r\n");
    return OK;
}

status_t CXmlNode::GetPathToStk(CMemStk *ss)
{
    CXmlNode *p;
    ASSERT(ss);
    ss->Clear();
    p = this;
    while(p)
    {
        ss->Push(p->mem_name);
        p = p->GetParent();
    }
    return OK;
}

status_t CXmlNode::GetPath(CFileBase *path)
{
    CMemStk ss;
    CMem *pstr;
    ASSERT(path);
    ss.Init(256);
    path->SetSize(0);

    this->GetPathToStk(&ss);
    if(ss.IsEmpty())
    {
        path->Putc('/');
    }
    pstr = ss.GetTopPtr();
    while(pstr)
    {
        if(pstr->GetRawBuf())
        {
            path->Putc('/');
            path->Puts(pstr->CStr());
        }
        else
        {
            break;
        }
        ss.DelTop();
        pstr = ss.GetTopPtr();
    }
    return OK;
}

CXmlNode * CXmlNode::GetNodeByPath(const char *path)
{
    CXmlNode *p,*pret;
    LOCAL_MEM(mem_buf);
    ASSERT(path);
    this->GetPath(&mem_buf);
    if(mem_buf.StrCmp(path) == 0)
        return this;
    p = this->child;
    while(p)
    {
        pret = p->GetNodeByPath(path);
        if(pret) return pret;
        p = p->next;
    }
    return NULL;
}

const char *CXmlNode::GetName()
{
    ASSERT(this->mem_name);
    return this->mem_name->CStr();
}

status_t CXmlNode::GetAttrib(const char *name, CFileBase *val)
{
    ASSERT(name && val);

    val->SetSize(0);

    if(this->mf_attrib == NULL)
        return ERROR;

    LOCAL_MEM(mem);

    this->mf_attrib->Seek(0);
    while(!mf_attrib->IsEnd())
    {
        this->GetNextAttrib(&mem,val);
        if(mem.StrCmp(name) == 0)
        {
            return OK;
        }
    }

    val->SetSize(0);
    return ERROR;
}

status_t CXmlNode::RestartAttrib()
{
    if(mf_attrib)
        mf_attrib->Seek(0);
    return OK;
}

status_t CXmlNode::GetNextAttrib(CFileBase *name, CFileBase *val)
{
    ASSERT(name && val);

    LOCAL_MEM(mem);
    LOCAL_MEM(mem_temp);

    val->SetSize(0);
    name->SetSize(0);
    if(this->mf_attrib == NULL)
        return ERROR;

    if(!this->mf_attrib->ReadLine(&mem))
        return ERROR;

    if(mem.C(0) == 0)
        return ERROR;

	mem.SetSplitChars(" =!?\t\r\n\"\'?/<>");
    mem.Trim();
    mem.Seek(0);
    mem.ReadWord(name);
    mem.ReadWord(&mem_temp);
    mem.ReadWord(&mem_temp);
    ASSERT(mem_temp.C(0) == '\"');
    if(!mem.ReadCStr(val))
    {
        return ERROR;
    }
    return OK;
}
CXmlNode * CXmlNode::GetChildByName(const char *child_name)
{
    ASSERT(child_name);
    CXmlNode *px;
    px = this->GetChild(0);
    while(px)
    {
        if(strcmp(px->GetName(),child_name) == 0)
            return px;
        px = px->next;
    }
    return NULL;
}

CMemFile *CXmlNode::GetValue()
{
    return mf_value;
}

status_t CXmlNode::GetStringValue(CMem *val)
{
    ASSERT(val);
    
    val->SetSize(0);
    if(this->GetValue() != NULL)
    {
        val->AutoRealloc((int_ptr_t)GetValue()->GetSize());
        val->Puts(this->GetValue());
        return OK;
    }    
    return ERROR;
}

int32_t CXmlNode::GetInt32Value()
{
    LOCAL_MEM(tmp);
    this->GetStringValue(&tmp);
    return str2int_32(tmp.CStr());
}

uint32_t CXmlNode::GetUInt32Value()
{
    return (uint32_t)GetInt32Value();
}

int64_t CXmlNode::GetInt64Value()
{
    LOCAL_MEM(tmp);
    this->GetStringValue(&tmp);
    return str2int_64(tmp.CStr());
}

uint64_t CXmlNode::GetUInt64Value()
{
    return (uint64_t)GetInt64Value();
}

int8_t CXmlNode::GetInt8Value()
{
    return (int8_t)GetInt32Value();
}

int16_t CXmlNode::GetInt16Value()
{
    return (int16_t)GetInt32Value();
}

uint8_t CXmlNode::GetUInt8Value()
{
    return (uint8_t)GetInt32Value();
}

uint16_t CXmlNode::GetUInt16Value()
{
    return (uint16_t)GetInt32Value();
}

bool CXmlNode::GetBooleanValue()
{
    LOCAL_MEM(tmp);
    this->GetStringValue(&tmp);
    if(tmp.StrCmp("1") == 0)
        return true;
    if(tmp.StrCmp("0") == 0)
        return false;
    return (tmp.StrICmp("true") == 0);
}

double CXmlNode::GetDoubleValue()
{
    LOCAL_MEM(tmp);
    this->GetStringValue(&tmp);
    return atof(tmp.CStr());
}

float CXmlNode::GetFloatValue()
{
    return (float)GetDoubleValue();
}
/*=======================================================*/
CXml::CXml()
{   
    this->InitBasic();
}

CXml::~CXml()
{
    this->Destroy();
}

status_t CXml::InitBasic()
{
    WEAK_REF_CLEAR();
    this->root = NULL;
    this->interrupt_parsing = false;
    return OK;
}

status_t CXml::Init()
{        
    return this->InitBasic();
}

status_t  CXml::Destroy()
{
    WEAK_REF_DESTROY();
    if( this->root == NULL)
        return OK;
    CXmlNode::Free(this->root);
    this->root = NULL;
    return OK;
}

CXmlNode * CXml:: GetRoot()
{
    return this->root;
}

status_t CXml:: AddRoot(CXmlNode *node)
{
    if(this->root != NULL)
        return ERROR;
    this->root = node;
    return OK;
}

status_t CXml::LoadXml(CFileBase *file)
{
    status_t ret;
    CXmlNode *px = NULL,*parent = NULL;
    CXmlReader xml_reader;
    CMem *pstr;
    ASSERT(this->root==NULL);
    ASSERT(file);

    interrupt_parsing = false;
    file->Seek(0);
    xml_reader.Init();
    xml_reader.SetXmlFile(file);
    while((ret = xml_reader.EnterNextItem()))
    {
        if(interrupt_parsing)break;
        pstr = xml_reader.mem_stk->GetTopPtr();
        if(pstr == NULL)
            break;
        if(ret == XML_RET_ENTER || ret == XML_RET_SINGLE)
        {
            NEW(px,CXmlNode);
            px->Init();
            if(this->root == NULL)
                this->root = px;

            if( ! xml_reader.IsFileEmpty(xml_reader.mf_attrib))
            {
                px->AddAttrib(xml_reader.mf_attrib);
            }
            px->SetName(pstr);
            px->parent = parent;

            if(parent)
                parent->AddChild(px);
            if(ret == XML_RET_ENTER)
                parent = px;
            if(ret == XML_RET_SINGLE)
            {
                px = px->parent;
                xml_reader.mem_stk->DelTop();
            }
        }
        else if(ret == XML_RET_LEAVE)
        {
            if(px == NULL)
                break;
            if( !xml_reader.IsFileEmpty(xml_reader.mf_value))
            {
                px->AddValueData(xml_reader.mf_value);
                px->SetValueType(xml_reader.value_type);
            }

            parent = px->parent;
            px = parent;
            xml_reader.mem_stk->DelTop();
        }
    }
    return OK;
}

status_t CXml::WriteToFile(CFileBase *file,CMem *header)
{
    ASSERT(this->root);
    ASSERT(file);
    file->Puts(header);
    return this->root->WriteToFile(file);
}

status_t CXml::WriteToFile(const char *fn,CMem *header)
{
    CFile file;
    file.Init();
    if(!file.OpenFile(fn,"wb+"))
        return ERROR;
    this->WriteToFile(&file,header);
    file.Destroy();
    return OK;
}

CXmlNode * CXml::GetNodeByPath(const char *path)
{
    ASSERT(path);

    if(this->root)
        return this->root->GetNodeByPath(path);
    else
        return NULL;
}

status_t CXml::LoadXml(const char *fn)
{
    CMemFile mf;
    mf.Init();
    if(!mf.LoadFile(fn))
        return ERROR;
    return this->LoadXml(&mf);
}

status_t CXml::InterruptParsing()
{
    interrupt_parsing = true;
    return OK;
}

bool CXml::IsInterrupted()
{
    return interrupt_parsing;
}
