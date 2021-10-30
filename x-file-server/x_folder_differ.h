#ifndef __X_FOLDER_DIFFER_H
#define __X_FOLDER_DIFFER_H

/*##Begin Inlcudes##*/
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"
#include "x_file_info_list.h"

class CxFolderDiffer
/*##Begin Bases##*/
/*##End Bases##*/
{
/*##Begin Members##*/
/*##End Members##*/
public:
    uint32_t m_flags;    
public:
    CxFolderDiffer();
    virtual ~CxFolderDiffer();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

/*##Begin Getter_H##*/
/*##End Getter_H##*/
/*##Begin Setter_H##*/
/*##End Setter_H##*/

    static status_t ReadFinalLink(const char *name, CMem *out);    
    status_t GetFolderInfos(const char *folder, CxFileInfoList *list);
    static int64_t GetFileModifyTime(CMem *fn);
    status_t SaveFolderSnapshot(CxFileInfoList *list, const char *fn);
    status_t LoadFolderSnapshot(const char *bson_fn, CxFileInfoList *list);

    status_t CompTwoFolder(CxFileInfoList *left, CxFileInfoList *right, CxFileInfoList *create_list, CxFileInfoList *delete_list);

    FLAG_FUNC(m_flags,FollowLinks,0x00000001);
    FLAG_FUNC(m_flags,Recursive,0x00000002);
};

#endif
