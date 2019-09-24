// Encoder.h: interface for the CEncoder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENCODER_H__E56A70D1_E004_4747_A94E_1F9E2BBD47B1__INCLUDED_)
#define AFX_ENCODER_H__E56A70D1_E004_4747_A94E_1F9E2BBD47B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "filebase.h"

#define ENCODING_GBK        1
#define ENCODING_UTF8       2   
#define ENCODING_UNICODE    3

class CEncoder{
public:
    static status_t ToLocalEncoding(int from,CFileBase *file);
    static status_t EncodingConvert(int from, int to, CFileBase *file);
    static status_t EncodingConvert(int from, int to,CFileBase *inFile, CFileBase *outFile);
    static status_t EscapeXmlStr(CFileBase *str);
    static status_t UnEscapeXmlStr(CFileBase *str);
    static status_t UnEscapeHtmlStr(CFileBase *str);
    static status_t EscapeHtmlStr(CFileBase *str);
    static status_t unescape_str(int option,CFileBase *file);
    static status_t escape_str(int option,CFileBase *file);
    static const char *escape_char(int option,char ch);
    static char un_escape_char(int option,const char *esc_str);
    static status_t UnicodeToUtf8(CFileBase *file);
    static status_t Md5Encode_Interruptable(CFileBase *file, CFileBase *md5_str, int *running);
    static status_t Md5Encode_Interruptable(CFileBase *file,uint32_t *md5_value,int *running);
    static status_t Md5Encode(CFileBase *file, CFileBase *md5_str);
    static status_t Md5Encode(CFileBase *file,uint32_t *md5_value);
    static void md5_core(uint32_t *A,uint32_t *B,uint32_t *C,uint32_t *D,uint32_t *x);
    static status_t UrlDecode(CFileBase *url_enc,CFileBase *out_utf8);
    static status_t UrlEncode(CFileBase *url_utf8, CFileBase *url_enc);
    static status_t UrlEncode(const char *url_str,CFileBase *file_enc);
    static status_t Utf8ToUnicode(CFileBase *file);
    static status_t Utf8ToUnicode(CFileBase *utf8,CFileBase *unicode);
    static status_t UnicodeToUtf8(CFileBase *uni, CFileBase *utf8);
    static status_t UnicodeToGb(CFileBase *file);
    static status_t GbToUnicode(CFileBase *file);
    static status_t UnicodeToGb(CFileBase *file_unicode,CFileBase *file_gb);
    static uint16_t GbCharToUnicode(const char *gb_char);
    static void UnicodeToGbChar(uint16_t uc,char *gb);
    static status_t GbToUnicode(CFileBase *file_gb,CFileBase *file_unicode);
    static int UnicodeToUtf8(uint32_t uc, char *ret);
    static status_t Base64_Decode(CFileBase *file_base64,CFileBase *file_raw);
    static status_t Base64_Decode_Interruptable(CFileBase *file_base64, CFileBase *file_raw, int *running);
    static status_t Base64_Encode(CFileBase *file_raw,CFileBase *file_base64);
    static status_t Base64_Encode_Interruptable(CFileBase *file_raw, CFileBase *file_base64, int *running);
};

#endif // !defined(AFX_ENCODER_H__E56A70D1_E004_4747_A94E_1F9E2BBD47B1__INCLUDED_)
