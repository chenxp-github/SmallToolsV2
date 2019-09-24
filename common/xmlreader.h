// CXmlReader.h: interface for the CXmlReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CXmlReader_H__7EB396A9_2415_4B47_B265_D4EA5CD69BE4__INCLUDED_)
#define AFX_CXmlReader_H__7EB396A9_2415_4B47_B265_D4EA5CD69BE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "memfile.h"
#include "mem.h"
#include "memstk.h"

#define VALUE_TYPE_ERR      0
#define VALUE_TYPE_CDATA    1
#define VALUE_TYPE_NORMAL   2

#define XML_RET_ENTER   1
#define XML_RET_LEAVE   2
#define XML_RET_SINGLE  3

class CXmlReader  
{
private:
    enum
    {
        XML_MODE_NORMAL,
        XML_MODE_CDATA,
        XML_MODE_COMMENT,
        XML_MODE_ITEM
    };
public:
    CFileBase *i_file_xml;
    CMemFile *mf_attrib;
    CMemFile *mf_value;
    CMemStk  *mem_stk;
    status_t value_type;
    status_t mode;
public:
    static char UnGetc_NotWhite(CFileBase *file);
    static char Getc_NotWhite(CFileBase *file);
    status_t SetXmlFile(CFileBase *file);
    status_t GetCurPath(CFileBase *file);
    static status_t IsFileEmpty(CFileBase *mf);
    status_t Skip(const char *str);
    status_t EnterNextItem();
    status_t Destroy();
    status_t Init();
    CXmlReader();
    virtual ~CXmlReader();
};

#endif // !defined(AFX_CXmlReader_H__7EB396A9_2415_4B47_B265_D4EA5CD69BE4__INCLUDED_)
