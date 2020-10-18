// Xml.h: interface for the CXml class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XML_H__FA31F9F4_F031_4B0A_8B37_3ABD6AB598ED__INCLUDED_)
#define AFX_XML_H__FA31F9F4_F031_4B0A_8B37_3ABD6AB598ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mem.h"
#include "memfile.h"
#include "xmlreader.h"
#include "file.h"

#define XML_STR(s) ((s)==NULL?"":(s))
#define XML_STRING_TO_DOUBLE(s) atof(s)
#define XML_STRING_TO_INT32(s) atoi(s)
#define XML_STRING_TO_BOOLEAN(s) (strcmp(s,"true")?false:true)

class CXmlNode{
public:
    WEAK_REF_DEFINE();
public:
    CMem *mem_name;
    CMemFile *mf_attrib;
    CMemFile *mf_value;
    status_t value_type;
    CXmlNode *parent;
    CXmlNode *child;
    CXmlNode *next;
    CXmlNode *tail;
public:
    CXmlNode();
    ~CXmlNode();
    status_t Init();
    status_t Destroy();
    status_t Print();
    static CXmlNode *Alloc();
    static status_t Free(CXmlNode *node);
    status_t AddChild(CXmlNode *node);
    CXmlNode *GetChild(int i);
    CXmlNode *GetChild();
    CXmlNode *GetNext();
    CXmlNode *GetParent();
    status_t AddAttrib(CFileBase *file);
    status_t AddAttrib(const char *attrib, const char *val);
    status_t SetName(const char *name);
    status_t SetName(CMem *name);
    status_t AddValueData(const char *str);
    status_t AddValueData(CFileBase *file);
    status_t SetValueType(int type);
    status_t WriteToFile(CFileBase *file);
    status_t GetPathToStk(CMemStk *ss);
    status_t GetPath(CFileBase *path);
    CXmlNode *GetNodeByPath(const char *path);
    const char *GetName();
    status_t GetAttrib(const char *name, CFileBase *val);
    status_t RestartAttrib();
    status_t GetNextAttrib(CFileBase *name, CFileBase *val);
    CXmlNode *GetChildByName(const char *child_name);
    CMemFile *GetValue();
    status_t GetStringValue(CMem *val);
    int32_t GetInt32Value();
    int64_t GetInt64Value();
    uint32_t GetUInt32Value();
    uint64_t GetUInt64Value();
    int8_t GetInt8Value();
    int16_t GetInt16Value();
    uint8_t GetUInt8Value();
    uint16_t GetUInt16Value();
    bool GetBooleanValue();
    double GetDoubleValue();
    float GetFloatValue();
};

class CXml{
public:
    WEAK_REF_DEFINE();
public:
    CXmlNode *root;
    bool interrupt_parsing;
public:
    status_t LoadXml(const char *fn);
    CXmlNode *GetNodeByPath(const char *path);
    status_t WriteToFile(const char *fn,CMem *header);
    status_t WriteToFile(CFileBase *mf,CMem *header);
    status_t LoadXml(CFileBase *file);
    CXml();
    ~CXml();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    CXmlNode *GetRoot();
    status_t AddRoot(CXmlNode *node);
    status_t InterruptParsing();
    bool IsInterrupted();
};

#endif // !defined(AFX_XML_H__FA31F9F4_F031_4B0A_8B37_3ABD6AB598ED__INCLUDED_)
