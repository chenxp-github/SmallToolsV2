#ifndef __T_REMOTE_DESKTOP_PIXEL_BUFFER_LIST_H
#define __T_REMOTE_DESKTOP_PIXEL_BUFFER_LIST_H

#include "mem.h"
#include "remote_desktop_pixel_buffer.h"
#include "minibson.h"


class CRemoteDesktopPixelBufferList{
public:
    WEAK_REF_DEFINE();

private:
    CRemoteDesktopPixelBuffer **m_Index;
    int m_Top;
    int m_Size;
public:
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
    CRemoteDesktopPixelBuffer* RemoveElem(int index);
    status_t InsElemPtr(int i, CRemoteDesktopPixelBuffer *node);
    CRemoteDesktopPixelBuffer* PopPtr();
    status_t AutoResize();
    status_t PushPtr(CRemoteDesktopPixelBuffer *node);
    status_t InitBasic();  
    int BSearchPos(CRemoteDesktopPixelBuffer *node,int order,int *find_flag);
    status_t InsOrderedPtr(CRemoteDesktopPixelBuffer *node,int order,int unique);
    status_t InsOrdered(CRemoteDesktopPixelBuffer *node,int order,int unique);
    status_t DelElem(int i);
    status_t InsElem(int i,CRemoteDesktopPixelBuffer *node);
    int BSearch(CRemoteDesktopPixelBuffer *node,int order);
    CRemoteDesktopPixelBuffer* BSearchNode(CRemoteDesktopPixelBuffer *node,int order);
    status_t Sort(int order);
    CRemoteDesktopPixelBuffer* GetElem(int index);
    CRemoteDesktopPixelBuffer* GetTopPtr();
    CRemoteDesktopPixelBuffer* Search(CRemoteDesktopPixelBuffer *node);
    int SearchPos(CRemoteDesktopPixelBuffer *node);
    status_t Clear();
    status_t DelTop();
    status_t Pop(CRemoteDesktopPixelBuffer *node);
    status_t Push(CRemoteDesktopPixelBuffer *node);
    int GetLen();
    bool IsEmpty();
    bool IsFull();

    int CompNode(CRemoteDesktopPixelBuffer *node1, CRemoteDesktopPixelBuffer *node2);
    status_t CopyNode(CRemoteDesktopPixelBuffer *dst, CRemoteDesktopPixelBuffer *src);
    status_t DelNode(CRemoteDesktopPixelBuffer *node);
    CRemoteDesktopPixelBuffer* CloneNode(CRemoteDesktopPixelBuffer *node);

    CRemoteDesktopPixelBufferList();
    ~CRemoteDesktopPixelBufferList();
    status_t Destroy();
    status_t Init(int init_size=256);
    status_t Print(CFileBase *_buf);
    status_t Copy(CRemoteDesktopPixelBufferList *stk);
    int Comp(CRemoteDesktopPixelBufferList *stk);
	status_t SaveAllTiles(const char *prefix);

	size_t GetTotalPixelDataSize();
	status_t CompressAll();
	status_t DecompressAll();
	status_t CopySingleScreen(CRemoteDesktopPixelBuffer *screen);
};


#endif

