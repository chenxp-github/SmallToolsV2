#ifndef __REMOTE_DESKTOP_INPUT_H
#define __REMOTE_DESKTOP_INPUT_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "remote_desktop_mouse_input.h"
#include "remote_desktop_keybd_input.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"
#include "remote_desktop_common.h"

class CRemoteDesktopInput
/*##Begin Bases##*/
/*##End Bases##*/
{
/*##Begin Members##*/
public:
public:
    WEAK_REF_DEFINE();

public:
    uint32_t m_type;
    CRemoteDesktopMouseInput *m_mouseInput;
    CRemoteDesktopKeybdInput *m_keybdInput;
/*##End Members##*/

public:
    CRemoteDesktopInput();
    virtual ~CRemoteDesktopInput();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CRemoteDesktopInput *_p);
    int Comp(CRemoteDesktopInput *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    uint32_t GetType();
    CRemoteDesktopMouseInput* GetMouseInput();
    CRemoteDesktopKeybdInput* GetKeybdInput();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetType(uint32_t _type);
    status_t CreateMouseInput();
    status_t SetMouseInput(CRemoteDesktopMouseInput *_mouseinput);
    status_t CreateKeybdInput();
    status_t SetKeybdInput(CRemoteDesktopKeybdInput *_keybdinput);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/
};

#endif
