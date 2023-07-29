#ifndef __REMOTE_DESKTOP_MOUSE_INPUT_H
#define __REMOTE_DESKTOP_MOUSE_INPUT_H

#include "minibson.h"
/*##Begin Inlcudes##*/
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CRemoteDesktopMouseInput
/*##Begin Bases##*/
/*##End Bases##*/
{
/*##Begin Members##*/
public:
public:
    WEAK_REF_DEFINE();

public:
    int m_dx;
    int m_dy;
    uint32_t m_mouseData;
    uint32_t m_dwFlags;
/*##End Members##*/

public:
    CRemoteDesktopMouseInput();
    virtual ~CRemoteDesktopMouseInput();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CRemoteDesktopMouseInput *_p);
    int Comp(CRemoteDesktopMouseInput *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    int GetDx();
    int GetDy();
    uint32_t GetMouseData();
    uint32_t GetDwFlags();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetDx(int _dx);
    status_t SetDy(int _dy);
    status_t SetMouseData(uint32_t _mousedata);
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
