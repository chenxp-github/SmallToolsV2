#ifndef __FTPCONFIGLIST_H
#define __FTPCONFIGLIST_H

#include "ftpconfig.h"
#include "ftputils.h"

class CFtpConfigList{
public:
	CFtpConfig **mIndex;
	int32_t mTop;
	int32_t mSize;
	void *param_comp[MAX_CALLBACK_PARAMS];
	status_t (*callback_comp)(void **p);	
public:		
	CFtpConfig * SearchByUserNameAndPassword(CFtpConfig *cfg);
	static status_t comp_user_and_pass(void **p);
	static status_t def_comp(void**p);
	CFtpConfig * RemoveElem(int32_t index);
	status_t InsertElemPtr(int32_t i, CFtpConfig *node);
	CFtpConfig * PopPtr();
	status_t AutoResize();
	status_t PushPtr(CFtpConfig *node);
	status_t InitBasic();  
	int32_t BSearch_Pos(CFtpConfig *node,int32_t order,int32_t *find_flag);
	status_t InsOrderedPtr(CFtpConfig *str,int32_t order,int32_t unique);
	status_t InsOrdered(CFtpConfig *str,int32_t order,int32_t unique);
	status_t DelElem(int32_t i);
	status_t InsertElem(int32_t i,CFtpConfig *node);
	status_t BSearch(CFtpConfig *node,int32_t order);
	CFtpConfig * BSearch_CFtpConfig(CFtpConfig *node,int32_t order);
	status_t Sort(int32_t order);
	CFtpConfig * GetElem(int32_t index);
	CFtpConfig * GetTopPtr();
	CFtpConfig * Search(CFtpConfig *node);
	int32_t Search_Pos(CFtpConfig *node);
	CFtpConfigList();
	status_t Clear();
	status_t DelTop();
	status_t Destroy();
	status_t Init(int32_t init_size);
	status_t IsEmpty();
	status_t IsFull();
	status_t Pop(CFtpConfig *node);
	status_t Print();
	status_t Push(CFtpConfig *node);
	int32_t GetLen();
	~CFtpConfigList();
};

#endif
