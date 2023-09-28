#ifndef __FTPFILEINFOLIST_H
#define __FTPFILEINFOLIST_H

#include "ftpfileinfo.h"

#define CALLBACK_DEFINE(name) \
void *param_##name[16];\
status_t (*callback_##name)(void **p)\

class CFtpFileInfoList{
public:
	CALLBACK_DEFINE(comp);
	CFtpFileInfo **mIndex;
	int mTop;
	int mSize;
public:		
	int SearchOnlyByName(const char *name);
	status_t RemoveDirPrefix(const char *dir);
	status_t LoadFromXml(const char *filename);
	status_t LoadFromXml(CFileBase *file);
	status_t SaveToXml(const char *filename);
	status_t SaveToXml(CFileBase *file);
	int CompNode(CFtpFileInfo *ftpfileinfo1, CFtpFileInfo *ftpfileinfo2);
	status_t CopyNode(CFtpFileInfo *dst, CFtpFileInfo *src);
	status_t DelNode(CFtpFileInfo *ftpfileinfo);
	CFtpFileInfo * CloneNode(CFtpFileInfo *ftpfileinfo);
	status_t Copy(CFtpFileInfoList *stk);
	CFtpFileInfo * RemoveElem(int index);
	status_t InsertElemPtr(int i, CFtpFileInfo *ftpfileinfo);
	CFtpFileInfo * PopPtr();
	status_t AutoResize();
	status_t PushPtr(CFtpFileInfo *ftpfileinfo);
	status_t InitBasic();  
	int BSearchPos(CFtpFileInfo *ftpfileinfo,int order,int *find_flag);
	status_t InsOrderedPtr(CFtpFileInfo *ftpfileinfo,int order,int unique);
	status_t InsOrdered(CFtpFileInfo *ftpfileinfo,int order,int unique);
	status_t DelElem(int i);
	status_t InsertElem(int i,CFtpFileInfo *ftpfileinfo);
	int BSearch(CFtpFileInfo *ftpfileinfo,int order);
	CFtpFileInfo * BSearchNode(CFtpFileInfo *ftpfileinfo,int order);
	status_t Sort(int order);
	CFtpFileInfo * GetElem(int index);
	CFtpFileInfo * GetTopPtr();
	CFtpFileInfo * Search(CFtpFileInfo *ftpfileinfo);
	int SearchPos(CFtpFileInfo *ftpfileinfo);
	CFtpFileInfoList();
	status_t Clear();
	status_t DelTop();
	status_t Destroy();
	status_t Init(int init_size);
	bool IsEmpty();
	bool IsFull();
	status_t Pop(CFtpFileInfo *ftpfileinfo);
	status_t Print();
	status_t Push(CFtpFileInfo *ftpfileinfo);
	int GetLen();
	~CFtpFileInfoList();
	
	status_t SearchDir(const char *dir, bool recursive);
	status_t GetFileList(const char *dir,bool is_abs_path, bool utf8=false);
};

#endif
