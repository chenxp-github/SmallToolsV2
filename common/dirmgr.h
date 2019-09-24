// DirMgr.h: interface for the CDirMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRMGR_H__F1C84966_D130_4527_A59B_A8B8AE067B79__INCLUDED_)
#define AFX_DIRMGR_H__F1C84966_D130_4527_A59B_A8B8AE067B79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mem.h"
#include "closure.h"

#define FN_SINGLE   0x00000001
#define FN_EXT      0x00000002
#define FN_PATH     0x00000004

#define FN_FILENAME (FN_SINGLE|FN_EXT)
#define FN_FULLNAME (FN_PATH|FN_FILENAME)

class CDirMgr{
public:
    enum{
        EVENT_BEGIN_DIR = 1,
        EVENT_END_DIR,
        EVENT_SINGLE_FILE,
    };
public: 
    static bool IsAbsPath(CFileBase *dir);
    static bool IsAbsPath(const char *dir);
    static status_t StdPathSplitor(CMem *str);
    static fsize_t GetFileSize(CMem *filename);
    static status_t IsPathSplitor(char ch);
    static status_t GetFileName(const char *line,CFileBase *file, uint32_t filter);
    static status_t GetFileName(CMem *fullname,CFileBase *file, uint32_t filter);
    static status_t SearchDir_Interruptable(CMem *dir, bool recursive, CClosure *closure, int *running);
    static status_t SearchDir(CMem *dir, bool recursive, CClosure *closure);
    static status_t SearchDir(const char *dir, bool recursive,CClosure *closure);
    static status_t GetCurDir(CMem *dir);
    static status_t ToAbsPath(CFileBase *file);
    static status_t ToAbsPath(CMem *prefix,CFileBase *file);
    static status_t ToAbsPath(const char *prefix,CFileBase *file);
    static status_t IsDirExist(CMem *dir);
    static status_t IsFileExist(CMem *file);
    static char GetPathSplitor();
    static status_t CreateDir(CMem *dir);
    static status_t DeleteFile(CMem *filename);
    static status_t DeleteDir(CMem *dir);
    static status_t DeleteDir(const char *dir);
    static status_t CreateDirSuper(CMem *dir);
    static status_t CreateFilePath(CMem *fullname);
    static status_t SaveFileSuper(CMem *fullname, CFileBase *data);
    static status_t CopyFileSuper(CMem *from, CMem *to);

#if _UNICODE_
    static wchar_t GetPathSplitorW();
    static status_t GetFileNameW(CMem *line,CFileBase *file, uint32_t filter);
    static status_t ToAbsPathW(CMem *prefix,CFileBase *file);
    static status_t ToAbsPathW(CFileBase *file);
    static status_t IsDirExistW(CMem *dir);
    static status_t GetCurDirW(CMem *dir);
#endif
};

#endif // !defined(AFX_DIRMGR_H__F1C84966_D130_4527_A59B_A8B8AE067B79__INCLUDED_)
