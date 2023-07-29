#ifndef __REMOTE_DESKTOP_KEYBD_INPUT_H
#define __REMOTE_DESKTOP_KEYBD_INPUT_H

#include "minibson.h"
/*##Begin Inlcudes##*/
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CRemoteDesktopKeybdInput
/*##Begin Bases##*/
/*##End Bases##*/
{
/*##Begin Members##*/
public:
public:
    WEAK_REF_DEFINE();

public:
    uint16_t m_wVk;
    uint16_t m_wScan;
    uint32_t m_dwFlags;
/*##End Members##*/

public:
    CRemoteDesktopKeybdInput();
    virtual ~CRemoteDesktopKeybdInput();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CRemoteDesktopKeybdInput *_p);
    int Comp(CRemoteDesktopKeybdInput *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    uint16_t GetWVk();
    uint16_t GetWScan();
    uint32_t GetDwFlags();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetWVk(uint16_t _wvk);
    status_t SetWScan(uint16_t _wscan);
    status_t SetDwFlags(uint32_t _dwflags);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/
};

#endif
