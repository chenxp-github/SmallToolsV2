#ifndef __X_FILE_SESSION_H
#define __X_FILE_SESSION_H

#include "minibson.h"
/*##Begin Inlcudes##*/
#include "x_opened_file_map.h"
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"

class CxFileSession
/*##Begin Bases##*/
/*##End Bases##*/
{
/*##Begin Members##*/
public:
    CMem m_name;
    CMem m_cur_dir;
    uint32_t m_cur_timer;
    CxOpenedFileMap m_opened_file_map;
/*##End Members##*/

public:
    CxFileSession();
    virtual ~CxFileSession();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CxFileSession *_p);
    int Comp(CxFileSession *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    CMem* GetName();
    const char* GetNameStr();
    CMem* GetCurDir();
    const char* GetCurDirStr();
    uint32_t GetCurTimer();
    CxOpenedFileMap* GetOpenedFileMap();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t SetName(CMem *_name);
    status_t SetName(const char *_name);
    status_t SetCurDir(CMem *_cur_dir);
    status_t SetCurDir(const char *_cur_dir);
    status_t SetCurTimer(uint32_t _cur_timer);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/
};

#endif
