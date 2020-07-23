// FileBase.h: interface for the CFileBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEBASE_H__BD546014_EE30_43B6_90BB_2EE15AD02697__INCLUDED_)
#define AFX_FILEBASE_H__BD546014_EE30_43B6_90BB_2EE15AD02697__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "cruntime.h"

//for vs2008 complie errors
#undef StrCpy
#undef StrCpyW
#undef StrCmp
#undef StrCmpW
#undef StrCat
#undef StrCatW

class CFileBase
{
/////////////////////////////////////////////////
#if _UNICODE_
/////////////////////////////////////////////////
public:
    const wchar_t *sp_chars_w;
public:
    fsize_t SplitW(CFileBase *file);
    fsize_t ReadLeftStrW(CFileBase *file,status_t skip_empty);
    status_t TrimW();
    status_t ReadKeyValueW(CFileBase *file_key, const wchar_t *split_chars, CFileBase *file_value);
    status_t ReadKeyValueW(const wchar_t *key, CFileBase *file_value);
    status_t StartWithW(CFileBase *file_str, status_t case_sensive, status_t skip_empty);
    status_t StartWithW(const wchar_t *str, status_t case_sensive, status_t skip_empty);
    static status_t InStrListW(const wchar_t *str, const wchar_t *str_list, status_t case_sensive);
    status_t ReadLineW(fsize_t line_index,CFileBase *file);
    wchar_t GetLastCharW(bool empty_included);
    status_t ReplaceStrW(const wchar_t *src_str, const wchar_t *des_str, status_t case_sensive, status_t word_only,CFileBase *new_file);
    fsize_t SearchStrW(const wchar_t *str, status_t case_sensive,status_t word_only);
    status_t ReadStringW_Reverse(CFileBase *file);
    fsize_t ReadWordW_Reverse(CFileBase *file);
    status_t ReadStringW(CFileBase *file);
    fsize_t ReadWordW(CFileBase *file);
    status_t SetSplitCharsW(const wchar_t *sp_str);
    status_t SkipEmptyCharsW_Reverse();
    status_t SkipEmptyCharsW();
    status_t ReverseW(fsize_t start, fsize_t end);
    fsize_t ReadWordWithEmptyCharW_Reverse(CFileBase *file);
    status_t IsSpCharW(wchar_t ch);
    fsize_t ReadWordWithEmptyCharW(CFileBase *file);
    status_t ReadLine_ReverseW(CFileBase *file);
    status_t ReadLine_ReverseW();
    status_t ReadLineW(CFileBase *file);
    wchar_t UnGetcW();
    status_t SetDefaultSpCharsW();
    status_t IsEmptyCharW(wchar_t uc);
    fsize_t PrintfW(const wchar_t *szFormat, ...);
    fsize_t PutsW(const wchar_t *str);
    fsize_t PutsW(CFileBase *file_str);
    status_t StrCatW(CFileBase *file);
    status_t StrCatW(const wchar_t *str);
    status_t StrLwrW();
    int StrCmpW(CFileBase *file);
    int StrCmpW(const wchar_t *str);
    int StrICmpW(CFileBase *file);
    int StrICmpW(const wchar_t *str);
    status_t StrCpyW(CFileBase *file);
    status_t StrCpyW(const wchar_t *str);   
    int_ptr_t StrLenW();
    status_t SetCharW(fsize_t index,wchar_t uc);
    wchar_t CharAtW(fsize_t index);
    status_t EndWithW(CFileBase *file_str, status_t case_sensive, status_t skip_empty);
    status_t EndWithW(const wchar_t *str, status_t case_sensive, status_t skip_empty);
/////////////////////////////////////////////////
#endif  //#if _UNICODE_
/////////////////////////////////////////////////
public:
    wchar_t GetcW();
    fsize_t PutcW(wchar_t uc);
/////////////////////////////////////////////////
#if _ASC_
/////////////////////////////////////////////////
public:
    char *file_name;
    char *sp_chars;
public:
    status_t StrLwr();
    status_t StrUpr();
    status_t ReadKeyValue(const char *key, CFileBase *file_value);
    status_t ReadKeyValue(CFileBase *file_key, const char *split_chars, CFileBase *file_value);
    fsize_t Split(CFileBase *file);
    status_t EndWith(CFileBase *file_str, status_t case_sensive, status_t skip_empty);
    status_t EndWith(const char *str, status_t case_sensive, status_t skip_empty);
    fsize_t Puts(CFileBase *file_str);
    fsize_t ReadLeftStr(CFileBase *file,status_t skip_empty);
    status_t Trim();
    status_t StartWith(const char *str,status_t case_sensive,status_t skip_empty);
    status_t StartWith(CFileBase *file_str,status_t case_sensive,status_t skip_empty);
    status_t StrCpy(const char *str);
    status_t StrCpy(CFileBase *file);
    int StrICmp(const char *str);
    int StrICmp(CFileBase *file);
    int StrCmp(CFileBase *file);    
    int StrCmp(const char *str);
    status_t StrCat(CFileBase *file);
    status_t StrCat(const char *str);
    int_ptr_t StrLen();
    static status_t InStrList(const char *str, const char *str_list, status_t case_sensive);
    status_t ReadQuoteStr(char escape_char,char end_char,CFileBase *file);
    int Getc_GB(char *letter);
    int Getc_Utf8(char *utf8);
    status_t ReadCStr(CFileBase *file);
    status_t ReplaceStr(const char *src_str,const char *des_str,status_t case_sensive,status_t word_only,CFileBase *new_file);
    fsize_t SearchStr(const char *str,status_t case_sensive,status_t word_only);
    status_t SetFileName(const char *fn);
    const char *GetFileName();
    char GetLastChar(bool empty_included);
    status_t ReadString_Reverse(CFileBase *file);
    status_t Reverse(fsize_t start,fsize_t end);
    virtual status_t SetChar(fsize_t index,char ch);
    virtual char CharAt(fsize_t index);
    fsize_t ReadWord_Reverse(CFileBase *file);
    fsize_t ReadWordWithEmptyChar_Reverse(CFileBase *file);
    status_t SkipEmptyChars_Reverse();
    status_t SetSplitChars(const char *sp_str);
    static status_t IsEmptyChar(char ch);
    virtual char UnGetc();
    status_t Dump();
    status_t IsSpChar(char ch);
    fsize_t Puts(const char *str);
    status_t ReadLine(CFileBase *file);
    status_t ReadLineWithEol(CFileBase *file,CFileBase *eol);
    status_t ReadString(CFileBase *file);
    fsize_t ReadWord(CFileBase *file);
    fsize_t ReadWordWithEmptyChar(CFileBase *file);
    status_t SetDefaultSpChars();
    status_t SkipEmptyChars();
    status_t ReadLine_Reverse();
    status_t ReadLine_Reverse(CFileBase *file);
    fsize_t Printf(const char *szFormat, ...);
    status_t LoadFile(const char *fn);
    fsize_t WriteToFile(const char *fn);
    fsize_t WriteToFile(const char *fn,fsize_t start,fsize_t wsize);
    fsize_t Log(const char *szFormat, ...);
    status_t Eol();    
    status_t Tab();
    int IncLogLevel(int inc);
    status_t SetLogLevel(int l);
    status_t GetLogLevel(); 
    int_ptr_t ReadZeroEndString(CFileBase *str);
/////////////////////////////////////////////////
#endif //#if _ASC_
/////////////////////////////////////////////////

/////////////////////////////////////////////////
//common part
/////////////////////////////////////////////////
public:
    WEAK_REF_DEFINE();
    void *user_data_;
    int log_tab_level_;
    bool is_sp_chars_malloc;
public:
    void* GetUserData();
    void SetUserData(void *ud);
    status_t LoadFile(FILE_HANDLE fp);
    fsize_t WriteToFile(FILE_HANDLE fp,fsize_t start,fsize_t wsize);
    virtual char Getc();
    virtual fsize_t Putc(char ch);
    fsize_t SearchBin(CFileBase *file_bin);
    fsize_t WriteFile(CFileBase *file, fsize_t start, fsize_t ws,void *buf,int_ptr_t buf_size);
    fsize_t WriteToFile(CFileBase *file,fsize_t start,fsize_t wsize,void *buf,int_ptr_t buf_size);
    fsize_t WriteToFile(CFileBase *file);
    fsize_t SeekEnd();
    status_t FillBlock(fsize_t fill_size,char fill_ch);
    status_t Destroy();
    status_t Init();
    status_t InitBasic();
    status_t ExpandFromOff(fsize_t exp_size);
    fsize_t SeekBack(fsize_t back_bytes);
    status_t Insert(CFileBase *file);
    status_t Insert(CFileBase *file,fsize_t start, fsize_t size);
    status_t Delete(fsize_t del_size);
    status_t DeleteLast(fsize_t bytes);
    bool IsEnd();
    fsize_t WriteFile(CFileBase *file);
    fsize_t WriteFile(CFileBase *file, fsize_t start, fsize_t ws);
    fsize_t WriteToFile(CFileBase *file,fsize_t wsize);
    fsize_t WriteToFile(CFileBase *file,fsize_t start,fsize_t wsize);
    CFileBase();
    virtual ~CFileBase();
    ////////////////////////////////////////////////////////
    virtual fsize_t GetOffset() = 0;
    virtual fsize_t GetSize() = 0;
    virtual int_ptr_t Read(void *buf,int_ptr_t n) = 0;
    virtual int_ptr_t Write(const void *buf,int_ptr_t n) = 0;
    virtual status_t SetSize(fsize_t ssize) = 0;
    virtual fsize_t Seek(fsize_t off) = 0;
    virtual status_t AddBlock(fsize_t bsize) = 0;
    virtual fsize_t GetMaxSize() = 0;
};

#endif // !defined(AFX_FILEBASE_H__BD546014_EE30_43B6_90BB_2EE15AD02697__INCLUDED_)
