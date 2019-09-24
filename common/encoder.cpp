// Encoder.cpp: implementation of the CEncoder class.
//
//////////////////////////////////////////////////////////////////////
#include "encoder.h"
#include "mem.h"
#include "memfile.h"
#include "misc.h"
#include "syslog.h"
#include "mem_tool.h"

static const char * base64_alphabet = 
"ABCDEFGHIJKLMNOP"
"QRSTUVWXYZabcdef"
"ghijklmnopqrstuv"
"wxyz0123456789+/=";

struct _escape_table{
    const char *name;
    char ch;
};

static const struct _escape_table html_escape_table[]={
    {"&lt;",'<'},
    {"&gt;",'>'},
    {"&amp;",'&'},
    {"&apos;",'\''},
    {"&quot;",'\"'},
    {"&nbsp;",' '},
};

static const struct _escape_table xml_escape_table[]={
    {"&lt;",'<'},
    {"&gt;",'>'},
    {"&amp;",'&'},
    {"&apos;",'\''},
    {"&quot;",'\"'},
};

#define AUTO_MEM AUTO_LOCAL_MEM
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
status_t CEncoder::Base64_Encode_Interruptable(CFileBase *file_raw, CFileBase *file_base64, int *running)
{
    ASSERT(file_raw && file_base64);
    
    uint8_t buf[4],tmp[4]; 
    fsize_t left = file_raw->GetSize();
    
    file_raw->Seek(0);
    while(left > 0)
    {
        if(running && !(*running)) 
            return ERROR;

        file_raw->Read(buf,3);
        left -= 3;
        
        if(left <= -1) buf[2] = 0;
        if(left <= -2) buf[1] = 0;
        
        tmp[0] = (buf[0] >> 2) & 0x3f;
        tmp[1] = ((buf[0] << 4) | (buf[1] >> 4)) & 0x3f;
        tmp[2] = ((buf[1] << 2) | (buf[2] >> 6)) & 0x3f;
        tmp[3] = buf[2] & 0x3f;
        
        if(left <= -1) tmp[3] = 64;
        if(left <= -2) tmp[2] = 64;
        
        file_base64->Putc(base64_alphabet[tmp[0]]);
        file_base64->Putc(base64_alphabet[tmp[1]]);
        file_base64->Putc(base64_alphabet[tmp[2]]);
        file_base64->Putc(base64_alphabet[tmp[3]]);     
    }
    
    return OK;
}

status_t CEncoder::Base64_Encode(CFileBase *file_raw, CFileBase *file_base64)
{
    return Base64_Encode_Interruptable(file_raw,file_base64,NULL);
}

status_t CEncoder::Base64_Decode_Interruptable(CFileBase *file_base64, CFileBase *file_raw, int *running)
{
    ASSERT(file_raw && file_base64);
    
    uint8_t ch,buf[4],tmp[4];
    int i,k;
    char *p;

    file_base64->Seek(0);

    while(!file_base64->IsEnd())
    {
        if(running && !(*running))
            return ERROR;
       
        buf[0] = 64; buf[1] = 64; buf[2] = 64; buf[3] = 64;

        i = 0;
        while(!file_base64->IsEnd() && i<4)
        {
            ch = file_base64->Getc();
            p = crt_strchr(base64_alphabet,ch);
            if(p != NULL)
                buf[i++] =  p - base64_alphabet;;
        }

        k = 4;
        for(i = 1; i < 4; i++)
        {
            if(buf[i] == 64)
            {
                k = i;
                buf[i] = 0;
                break;
            }
        }

        tmp[0] = (buf[0] << 2) | (buf[1] >> 4);
        tmp[1] = (buf[1] << 4) | (buf[2] >> 2);
        tmp[2] = (buf[2] << 6) | buf[3];
        
        for( i = 0; i < k-1; i++)
        {
            file_raw->Putc(tmp[i]);
        }
    }

    return OK;
}

status_t CEncoder::Base64_Decode(CFileBase *file_base64, CFileBase *file_raw)
{
    return Base64_Decode_Interruptable(file_base64,file_raw,NULL);
}

int CEncoder::UnicodeToUtf8(uint32_t uc, char *ret)
{
    ASSERT(ret);
    
    if (uc < 0x80)
    {
        ret[0] = uc;
        ret[1] = 0;
        return 1;
    }
    else if (uc < 0x800) 
    {
        ret[0] = (char)(0xC0 | (uc >> 6));
        ret[1] = (char)(0x80 | (uc & 0x3F));
        ret[2] = 0;
        return 2;
    }
    else if (uc < 0x10000) 
    {
        ret[0] = (char)(0xE0 | (uc >> 12));
        ret[1] = (char)(0x80 | ((uc >> 6) & 0x3F));
        ret[2] = (char)(0x80 | (uc & 0x3F));
        ret[3] = 0;
        return 3;
    }
    else if (uc < 0x200000) 
    {
        ret[0] = (char)(0xF0 | (uc >> 18));
        ret[1] = (char)(0x80 | ((uc >> 12) & 0x3F));
        ret[2] = (char)(0x80 | ((uc >> 6) & 0x3F));
        ret[3] = (char)(0x80 | (uc & 0x3F));
        ret[4] = 0;
        return 4;
    }
    return 0;
}

status_t CEncoder::UnicodeToUtf8(CFileBase *uni, CFileBase *utf8)
{
    ASSERT(uni && utf8);

    uint16_t ch;
    int ret;
    char t[8];


    utf8->SetSize(0);
    uni->Seek(0);

    while(!uni->IsEnd())
    {
        ch = uni->GetcW();
        ret = UnicodeToUtf8(ch,t);
        utf8->Write(t,ret);
    }

    return OK;
}

status_t CEncoder::UnicodeToUtf8(CFileBase *file)
{
    ASSERT(file);

    AUTO_MEM(mem,file->GetSize());
    mem.WriteFile(file);
    file->SetSize(0);
    return UnicodeToUtf8(&mem,file);
}

uint16_t CEncoder::GbCharToUnicode(const char *gb_char)
{
    return crt_gbk_char_to_unicode(gb_char);
}

void CEncoder::UnicodeToGbChar(uint16_t uc,char *gb)
{
    crt_unicode_char_to_gbk(uc,gb);
}

status_t CEncoder::GbToUnicode(CFileBase *file_gb, CFileBase *file_unicode)
{
    ASSERT(file_gb && file_unicode);

    char gb[4];
    uint16_t u;
    
    file_gb->Seek(0);
    file_unicode->SetSize(0);
    while( ! file_gb->IsEnd())
    {
        file_gb->Getc_GB(gb);
        u = CEncoder::GbCharToUnicode(gb);
        if(u != 0)
            file_unicode->Write(&u,sizeof(uint16_t));
    }   
    return OK;
}

status_t CEncoder::UnicodeToGb(CFileBase *file_unicode, CFileBase *file_gb)
{
    ASSERT(file_gb && file_unicode);

    char ch[4];
    uint16_t u;
    file_gb->SetSize(0);
    file_unicode->Seek(0);
    while( ! file_unicode->IsEnd())
    {
        file_unicode->Read(&u,sizeof(uint16_t));
        UnicodeToGbChar(u,ch);
        file_gb->Puts(ch);
    }
    return OK;
}

status_t CEncoder::GbToUnicode(CFileBase *file)
{
    ASSERT(file);
    AUTO_MEM(mem,file->GetSize());
    mem.WriteFile(file);
    file->SetSize(0);
    return GbToUnicode(&mem,file);
}

status_t CEncoder::UnicodeToGb(CFileBase *file)
{
    ASSERT(file);
    
    AUTO_MEM(mem,file->GetSize());

    mem.WriteFile(file);
    file->SetSize(0);

    return UnicodeToGb(&mem,file);
}

status_t CEncoder::Utf8ToUnicode(CFileBase *utf8, CFileBase *unicode)
{
    ASSERT(utf8 && unicode);
    
    uint16_t b0,b1,b2,uc;
    
    utf8->Seek(0);
    unicode->SetSize(0);

    while(!utf8->IsEnd())
    {
        b0 = (uint8_t)(utf8->Getc());
        if(b0 < 0x80)
        {
            uc = b0;
        }
        else
        {
            if((b0 & 0xe0)==0xc0)
            {
                b1 = (uint8_t)utf8->Getc();
                uc = ((b0 & ~0xe0) << 6) + ((b1 & ~0xc0) );
            }
            else
            {               
                if((b0 & 0xf0)==0xe0)
                {
                    b1 = (uint8_t)utf8->Getc();
                    b2 = (uint8_t)utf8->Getc();
                    uc=((b0&~0xf0)<<12) + ((b1&~0xc0)<<6) + ((b2&~0xc0));
                }
                else
                {
                    utf8->Getc();   
                    utf8->Getc();
                    utf8->Getc();
                    uc = '?';
                }
            }
        }
        unicode->Write(&uc,sizeof(uint16_t));
    }   
    return OK;
}

status_t CEncoder::Utf8ToUnicode(CFileBase *file)
{
    ASSERT(file);
    AUTO_MEM(mem,file->GetSize());
    mem.WriteFile(file);
    file->SetSize(0);
    return Utf8ToUnicode(&mem,file);
}

status_t CEncoder::UrlEncode(const char *url_str, CFileBase *file_enc)
{
    CMem mem(url_str);
    return UrlEncode(&mem,file_enc);
}

status_t CEncoder::UrlEncode(CFileBase *url_utf8, CFileBase *url_enc)
{
    ASSERT(url_utf8 && url_enc);
    
    url_utf8->Seek(0);
    url_enc->SetSize(0);

    char ch,utf8[8];
    int i,n;

    while(!url_utf8->IsEnd())
    {
        n = url_utf8->Getc_Utf8(utf8);
        ch = utf8[0];
        if(     (ch >= '0' && ch <= '9') 
            ||  (ch >= 'a' && ch <= 'z')
            ||  (ch >= 'A' && ch <= 'Z')
            ||  ch == '.' || ch== '_' || ch == '&')
        {
            url_enc->Putc(ch);
        }
        else
        {
            for(i = 0; i < n; i++)
                url_enc->Printf("%%%x",(uint8_t)utf8[i]);
        }
    }
    return OK;
}
status_t CEncoder::UrlDecode(CFileBase *url_enc, CFileBase *out_utf8)
{
    ASSERT(url_enc && out_utf8);
    
    url_enc->Seek(0);
    out_utf8->SetSize(0);

    char ch,hex[8];
    while(!url_enc->IsEnd())
    {
        ch = url_enc->Getc();
        if(ch == 0)break;
        if(ch =='%')
        {
            hex[0] = url_enc->Getc();
            hex[1] = url_enc->Getc();
            hex[2] = 0;
            out_utf8->Putc(hex2dec_32(hex));            
        }
        else
        {
            out_utf8->Putc(ch);
        }
    }
    
    return OK;
}   
void CEncoder::md5_core(uint32_t *A,uint32_t *B,uint32_t *C,uint32_t *D,uint32_t *x)
{                
    uint32_t a,b,c,d;
 
    a = *A; b = *B; c = *C; d = *D;

    a=b+(((((a+(((b)&(c))|((~b)&(d)))+x[0]+0xd76aa478))<<(7))|(((a+(((b)&(c))|((~b)&(d)))+x[0]+0xd76aa478))>>(32-(7)))));
    d=a+(((((d+(((a)&(b))|((~a)&(c)))+x[1]+0xe8c7b756))<<(12))|(((d+(((a)&(b))|((~a)&(c)))+x[1]+0xe8c7b756))>>(32-(12)))));
    c=d+(((((c+(((d)&(a))|((~d)&(b)))+x[2]+0x242070db))<<(17))|(((c+(((d)&(a))|((~d)&(b)))+x[2]+0x242070db))>>(32-(17)))));
    b=c+(((((b+(((c)&(d))|((~c)&(a)))+x[3]+0xc1bdceee))<<(22))|(((b+(((c)&(d))|((~c)&(a)))+x[3]+0xc1bdceee))>>(32-(22)))));
    a=b+(((((a+(((b)&(c))|((~b)&(d)))+x[4]+0xf57c0faf))<<(7))|(((a+(((b)&(c))|((~b)&(d)))+x[4]+0xf57c0faf))>>(32-(7)))));
    d=a+(((((d+(((a)&(b))|((~a)&(c)))+x[5]+0x4787c62a))<<(12))|(((d+(((a)&(b))|((~a)&(c)))+x[5]+0x4787c62a))>>(32-(12)))));
    c=d+(((((c+(((d)&(a))|((~d)&(b)))+x[6]+0xa8304613))<<(17))|(((c+(((d)&(a))|((~d)&(b)))+x[6]+0xa8304613))>>(32-(17)))));
    b=c+(((((b+(((c)&(d))|((~c)&(a)))+x[7]+0xfd469501))<<(22))|(((b+(((c)&(d))|((~c)&(a)))+x[7]+0xfd469501))>>(32-(22)))));
    a=b+(((((a+(((b)&(c))|((~b)&(d)))+x[8]+0x698098d8))<<(7))|(((a+(((b)&(c))|((~b)&(d)))+x[8]+0x698098d8))>>(32-(7)))));
    d=a+(((((d+(((a)&(b))|((~a)&(c)))+x[9]+0x8b44f7af))<<(12))|(((d+(((a)&(b))|((~a)&(c)))+x[9]+0x8b44f7af))>>(32-(12)))));
    c=d+(((((c+(((d)&(a))|((~d)&(b)))+x[10]+0xffff5bb1))<<(17))|(((c+(((d)&(a))|((~d)&(b)))+x[10]+0xffff5bb1))>>(32-(17)))));
    b=c+(((((b+(((c)&(d))|((~c)&(a)))+x[11]+0x895cd7be))<<(22))|(((b+(((c)&(d))|((~c)&(a)))+x[11]+0x895cd7be))>>(32-(22)))));
    a=b+(((((a+(((b)&(c))|((~b)&(d)))+x[12]+0x6b901122))<<(7))|(((a+(((b)&(c))|((~b)&(d)))+x[12]+0x6b901122))>>(32-(7)))));
    d=a+(((((d+(((a)&(b))|((~a)&(c)))+x[13]+0xfd987193))<<(12))|(((d+(((a)&(b))|((~a)&(c)))+x[13]+0xfd987193))>>(32-(12)))));
    c=d+(((((c+(((d)&(a))|((~d)&(b)))+x[14]+0xa679438e))<<(17))|(((c+(((d)&(a))|((~d)&(b)))+x[14]+0xa679438e))>>(32-(17)))));
    b=c+(((((b+(((c)&(d))|((~c)&(a)))+x[15]+0x49b40821))<<(22))|(((b+(((c)&(d))|((~c)&(a)))+x[15]+0x49b40821))>>(32-(22)))));
    a=b+(((((a+(((b)&(d))|((c)&(~d)))+x[1]+0xf61e2562))<<(5))|(((a+(((b)&(d))|((c)&(~d)))+x[1]+0xf61e2562))>>(32-(5)))));
    d=a+(((((d+(((a)&(c))|((b)&(~c)))+x[6]+0xc040b340))<<(9))|(((d+(((a)&(c))|((b)&(~c)))+x[6]+0xc040b340))>>(32-(9)))));
    c=d+(((((c+(((d)&(b))|((a)&(~b)))+x[11]+0x265e5a51))<<(14))|(((c+(((d)&(b))|((a)&(~b)))+x[11]+0x265e5a51))>>(32-(14)))));
    b=c+(((((b+(((c)&(a))|((d)&(~a)))+x[0]+0xe9b6c7aa))<<(20))|(((b+(((c)&(a))|((d)&(~a)))+x[0]+0xe9b6c7aa))>>(32-(20)))));
    a=b+(((((a+(((b)&(d))|((c)&(~d)))+x[5]+0xd62f105d))<<(5))|(((a+(((b)&(d))|((c)&(~d)))+x[5]+0xd62f105d))>>(32-(5)))));
    d=a+(((((d+(((a)&(c))|((b)&(~c)))+x[10]+0x02441453))<<(9))|(((d+(((a)&(c))|((b)&(~c)))+x[10]+0x02441453))>>(32-(9)))));
    c=d+(((((c+(((d)&(b))|((a)&(~b)))+x[15]+0xd8a1e681))<<(14))|(((c+(((d)&(b))|((a)&(~b)))+x[15]+0xd8a1e681))>>(32-(14)))));
    b=c+(((((b+(((c)&(a))|((d)&(~a)))+x[4]+0xe7d3fbc8))<<(20))|(((b+(((c)&(a))|((d)&(~a)))+x[4]+0xe7d3fbc8))>>(32-(20)))));
    a=b+(((((a+(((b)&(d))|((c)&(~d)))+x[9]+0x21e1cde6))<<(5))|(((a+(((b)&(d))|((c)&(~d)))+x[9]+0x21e1cde6))>>(32-(5)))));
    d=a+(((((d+(((a)&(c))|((b)&(~c)))+x[14]+0xc33707d6))<<(9))|(((d+(((a)&(c))|((b)&(~c)))+x[14]+0xc33707d6))>>(32-(9)))));
    c=d+(((((c+(((d)&(b))|((a)&(~b)))+x[3]+0xf4d50d87))<<(14))|(((c+(((d)&(b))|((a)&(~b)))+x[3]+0xf4d50d87))>>(32-(14)))));
    b=c+(((((b+(((c)&(a))|((d)&(~a)))+x[8]+0x455a14ed))<<(20))|(((b+(((c)&(a))|((d)&(~a)))+x[8]+0x455a14ed))>>(32-(20)))));
    a=b+(((((a+(((b)&(d))|((c)&(~d)))+x[13]+0xa9e3e905))<<(5))|(((a+(((b)&(d))|((c)&(~d)))+x[13]+0xa9e3e905))>>(32-(5)))));
    d=a+(((((d+(((a)&(c))|((b)&(~c)))+x[2]+0xfcefa3f8))<<(9))|(((d+(((a)&(c))|((b)&(~c)))+x[2]+0xfcefa3f8))>>(32-(9)))));
    c=d+(((((c+(((d)&(b))|((a)&(~b)))+x[7]+0x676f02d9))<<(14))|(((c+(((d)&(b))|((a)&(~b)))+x[7]+0x676f02d9))>>(32-(14)))));
    b=c+(((((b+(((c)&(a))|((d)&(~a)))+x[12]+0x8d2a4c8a))<<(20))|(((b+(((c)&(a))|((d)&(~a)))+x[12]+0x8d2a4c8a))>>(32-(20)))));
    a=b+(((((a+((b)^(c)^(d))+x[5]+0xfffa3942))<<(4))|(((a+((b)^(c)^(d))+x[5]+0xfffa3942))>>(32-(4)))));
    d=a+(((((d+((a)^(b)^(c))+x[8]+0x8771f681))<<(11))|(((d+((a)^(b)^(c))+x[8]+0x8771f681))>>(32-(11)))));
    c=d+(((((c+((d)^(a)^(b))+x[11]+0x6d9d6122))<<(16))|(((c+((d)^(a)^(b))+x[11]+0x6d9d6122))>>(32-(16)))));
    b=c+(((((b+((c)^(d)^(a))+x[14]+0xfde5380c))<<(23))|(((b+((c)^(d)^(a))+x[14]+0xfde5380c))>>(32-(23)))));
    a=b+(((((a+((b)^(c)^(d))+x[1]+0xa4beea44))<<(4))|(((a+((b)^(c)^(d))+x[1]+0xa4beea44))>>(32-(4)))));
    d=a+(((((d+((a)^(b)^(c))+x[4]+0x4bdecfa9))<<(11))|(((d+((a)^(b)^(c))+x[4]+0x4bdecfa9))>>(32-(11)))));
    c=d+(((((c+((d)^(a)^(b))+x[7]+0xf6bb4b60))<<(16))|(((c+((d)^(a)^(b))+x[7]+0xf6bb4b60))>>(32-(16)))));
    b=c+(((((b+((c)^(d)^(a))+x[10]+0xbebfbc70))<<(23))|(((b+((c)^(d)^(a))+x[10]+0xbebfbc70))>>(32-(23)))));
    a=b+(((((a+((b)^(c)^(d))+x[13]+0x289b7ec6))<<(4))|(((a+((b)^(c)^(d))+x[13]+0x289b7ec6))>>(32-(4)))));
    d=a+(((((d+((a)^(b)^(c))+x[0]+0xeaa127fa))<<(11))|(((d+((a)^(b)^(c))+x[0]+0xeaa127fa))>>(32-(11)))));
    c=d+(((((c+((d)^(a)^(b))+x[3]+0xd4ef3085))<<(16))|(((c+((d)^(a)^(b))+x[3]+0xd4ef3085))>>(32-(16)))));
    b=c+(((((b+((c)^(d)^(a))+x[6]+0x04881d05))<<(23))|(((b+((c)^(d)^(a))+x[6]+0x04881d05))>>(32-(23)))));
    a=b+(((((a+((b)^(c)^(d))+x[9]+0xd9d4d039))<<(4))|(((a+((b)^(c)^(d))+x[9]+0xd9d4d039))>>(32-(4)))));
    d=a+(((((d+((a)^(b)^(c))+x[12]+0xe6db99e5))<<(11))|(((d+((a)^(b)^(c))+x[12]+0xe6db99e5))>>(32-(11)))));
    c=d+(((((c+((d)^(a)^(b))+x[15]+0x1fa27cf8))<<(16))|(((c+((d)^(a)^(b))+x[15]+0x1fa27cf8))>>(32-(16)))));
    b=c+(((((b+((c)^(d)^(a))+x[2]+0xc4ac5665))<<(23))|(((b+((c)^(d)^(a))+x[2]+0xc4ac5665))>>(32-(23)))));
    a=b+(((((a+((c)^((b)|(~d)))+x[0]+0xf4292244))<<(6))|(((a+((c)^((b)|(~d)))+x[0]+0xf4292244))>>(32-(6)))));
    d=a+(((((d+((b)^((a)|(~c)))+x[7]+0x432aff97))<<(10))|(((d+((b)^((a)|(~c)))+x[7]+0x432aff97))>>(32-(10)))));
    c=d+(((((c+((a)^((d)|(~b)))+x[14]+0xab9423a7))<<(15))|(((c+((a)^((d)|(~b)))+x[14]+0xab9423a7))>>(32-(15)))));
    b=c+(((((b+((d)^((c)|(~a)))+x[5]+0xfc93a039))<<(21))|(((b+((d)^((c)|(~a)))+x[5]+0xfc93a039))>>(32-(21)))));
    a=b+(((((a+((c)^((b)|(~d)))+x[12]+0x655b59c3))<<(6))|(((a+((c)^((b)|(~d)))+x[12]+0x655b59c3))>>(32-(6)))));
    d=a+(((((d+((b)^((a)|(~c)))+x[3]+0x8f0ccc92))<<(10))|(((d+((b)^((a)|(~c)))+x[3]+0x8f0ccc92))>>(32-(10)))));
    c=d+(((((c+((a)^((d)|(~b)))+x[10]+0xffeff47d))<<(15))|(((c+((a)^((d)|(~b)))+x[10]+0xffeff47d))>>(32-(15)))));
    b=c+(((((b+((d)^((c)|(~a)))+x[1]+0x85845dd1))<<(21))|(((b+((d)^((c)|(~a)))+x[1]+0x85845dd1))>>(32-(21)))));
    a=b+(((((a+((c)^((b)|(~d)))+x[8]+0x6fa87e4f))<<(6))|(((a+((c)^((b)|(~d)))+x[8]+0x6fa87e4f))>>(32-(6)))));
    d=a+(((((d+((b)^((a)|(~c)))+x[15]+0xfe2ce6e0))<<(10))|(((d+((b)^((a)|(~c)))+x[15]+0xfe2ce6e0))>>(32-(10)))));
    c=d+(((((c+((a)^((d)|(~b)))+x[6]+0xa3014314))<<(15))|(((c+((a)^((d)|(~b)))+x[6]+0xa3014314))>>(32-(15)))));
    b=c+(((((b+((d)^((c)|(~a)))+x[13]+0x4e0811a1))<<(21))|(((b+((d)^((c)|(~a)))+x[13]+0x4e0811a1))>>(32-(21)))));
    a=b+(((((a+((c)^((b)|(~d)))+x[4]+0xf7537e82))<<(6))|(((a+((c)^((b)|(~d)))+x[4]+0xf7537e82))>>(32-(6)))));
    d=a+(((((d+((b)^((a)|(~c)))+x[11]+0xbd3af235))<<(10))|(((d+((b)^((a)|(~c)))+x[11]+0xbd3af235))>>(32-(10)))));
    c=d+(((((c+((a)^((d)|(~b)))+x[2]+0x2ad7d2bb))<<(15))|(((c+((a)^((d)|(~b)))+x[2]+0x2ad7d2bb))>>(32-(15)))));
    b=c+(((((b+((d)^((c)|(~a)))+x[9]+0xeb86d391))<<(21))|(((b+((d)^((c)|(~a)))+x[9]+0xeb86d391))>>(32-(21)))));

    *A += a; *B += b; *C += c; *D += d;
}

status_t CEncoder::Md5Encode_Interruptable(CFileBase *file, uint32_t *md5_value, int *running)
{
    ASSERT(file && md5_value);

    uint32_t A,B,C,D;
    uint32_t buf[16],flen[2];
    fsize_t i,len;

    A=0x67452301,B=0xefcdab89,C=0x98badcfe,D=0x10325476;
    crt_memset(buf,0,64);

    len = (file->GetSize());

    flen[1]= (uint32_t)(len>>29);
    flen[0]= (uint32_t)((len&(0x20000000-1))<<3);

    file->Seek(0);  
    file->Read(buf,64);
    for(i=0;i<(len>>6);i++)
    {      
        if(running && !(*running))
            return ERROR;
        md5_core(&A,&B,&C,&D,buf);
        crt_memset(buf,0,64);
        file->Read(buf,64);
    }

    ((uint8_t*)buf)[len&63]=128;

    if((len&63)>55)
    {
        md5_core(&A,&B,&C,&D,buf);
        crt_memset(buf,0,64);
    }

    crt_memcpy(buf+14,flen,8);
    
    md5_core(&A,&B,&C,&D,buf);
    md5_value[0] = (A<<24)|((A<<8)&0xff0000)|((A>>8)&0xff00)|(A>>24);
    md5_value[1] = (B<<24)|((B<<8)&0xff0000)|((B>>8)&0xff00)|(B>>24);
    md5_value[2] = (C<<24)|((C<<8)&0xff0000)|((C>>8)&0xff00)|(C>>24);
    md5_value[3] = (D<<24)|((D<<8)&0xff0000)|((D>>8)&0xff00)|(D>>24);

    return OK;
}

status_t CEncoder::Md5Encode(CFileBase *file, uint32_t *md5_value)
{
    return Md5Encode_Interruptable(file,md5_value,NULL);
}

status_t CEncoder::Md5Encode_Interruptable(CFileBase *file, CFileBase *md5_str, int *running)
{
    ASSERT(file && md5_str);    
    uint32_t val[8];
    status_t ret = Md5Encode_Interruptable(file,val,running);
    md5_str->SetSize(0);
    md5_str->Printf("%08x%08x%08x%08x",val[0],val[1],val[2],val[3]);
    return ret;
}

status_t CEncoder::Md5Encode(CFileBase *file, CFileBase *md5_str)
{
    return Md5Encode_Interruptable(file,md5_str,NULL);
}

char CEncoder::un_escape_char(int option,const char *esc_str)
{
    int size;
    const struct _escape_table *escape_table;

    if(option == 1)
    {
        size = sizeof(html_escape_table) / sizeof(html_escape_table[0]);
        escape_table = html_escape_table;
    }
    else
    {
        size = sizeof(xml_escape_table) / sizeof(xml_escape_table[0]);
        escape_table = xml_escape_table;
    }

    for(int i = 0; i < size; i++)
    {
        if(crt_strcmp(escape_table[i].name,esc_str) == 0)
        {
            return escape_table[i].ch;
        }
    }
    return 0;
}

const char * CEncoder::escape_char(int option,char ch)
{
    int size;
    const struct _escape_table *escape_table;

    if(option == 1)
    {
        size = sizeof(html_escape_table) / sizeof(html_escape_table[0]);
        escape_table = html_escape_table;
    }
    else
    {
        size = sizeof(xml_escape_table) / sizeof(xml_escape_table[0]);
        escape_table = xml_escape_table;
    }

    for(int i = 0; i < size; i++)
    {
        if(escape_table[i].ch == ch)
        {
            return escape_table[i].name;
        }
    }
    return NULL;
}

status_t CEncoder::escape_str(int option,CFileBase *file)
{
    ASSERT(file);
    AUTO_MEM(mem,file->GetSize());
    mem.StrCpy(file);
    mem.Seek(0);
    file->SetSize(0);
    while(!mem.IsEnd())
    {
        char ch = mem.Getc();
        if(ch == 0) break;
        const char *esc = escape_char(option,ch);
        if(esc == NULL)
            file->Putc(ch);
        else
            file->Puts(esc);
    }
    return OK;
}

status_t CEncoder::unescape_str(int option,CFileBase *file)
{
    ASSERT(file);
    AUTO_MEM(mem,file->GetSize());
    mem.StrCpy(file);
    mem.Seek(0);    
    file->SetSize(0);

    char ch,ch2;
    char buf[16];
    fsize_t old_off;
    int i;
    bool find;

    while(!mem.IsEnd())
    {
        ch = mem.Getc();
        if(ch == 0) break;

        if(ch == '&')
        {
            old_off = mem.GetOffset();
            buf[0] = ch;
            find = false;
            for(i = 0; i < 5; i++)
            {
                ch2 = mem.Getc();
                buf[i+1] = ch2;
                if(ch2 == ';')
                {
                    buf[i+2] = 0;
                    find = true;
                    break;
                }
            }

            if(!find)
            {
                file->Putc(ch);
                mem.Seek(old_off);
            }
            else
            {
                ch2 = un_escape_char(option,buf);
                if(ch2 == 0)
                {
                    file->Putc(ch);
                    mem.Seek(old_off);
                }
                else
                {
                    file->Putc(ch2);
                }
            }
        }
        else
        {
            file->Putc(ch);
        }
    }
    return OK;
}

status_t CEncoder::EscapeHtmlStr(CFileBase *str)
{
    ASSERT(str);
    return escape_str(1,str);
}

status_t CEncoder::UnEscapeHtmlStr(CFileBase *str)
{
    ASSERT(str);
    return unescape_str(1,str);
}
status_t CEncoder::EscapeXmlStr(CFileBase *str)
{
    ASSERT(str);
    return escape_str(2,str);
}

status_t CEncoder::UnEscapeXmlStr(CFileBase *str)
{
    ASSERT(str);
    return unescape_str(2,str);
}

status_t CEncoder::EncodingConvert(int from, int to, CFileBase *inFile, CFileBase *outFile)
{
    ASSERT(inFile && outFile);

    if(from == to)
    {
        if(inFile != outFile)
            outFile->WriteFile(inFile);
        return OK;
    }

    AUTO_MEM(tmp,inFile->GetSize()*sizeof(uint16_t));
    
    if(from  == ENCODING_UNICODE)
    {
        tmp.WriteFile(inFile);
    }
    else if(from == ENCODING_GBK)
    {
        GbToUnicode(inFile,&tmp);
    }
    else if(from == ENCODING_UTF8)
    {
        Utf8ToUnicode(inFile,&tmp);
    }
    
    if(to == ENCODING_UNICODE)
    {
        outFile->SetSize(0);
        outFile->WriteFile(&tmp);
    }
    else if(to == ENCODING_GBK)
    {
        UnicodeToGb(&tmp,outFile);
    }
    else if(to == ENCODING_UTF8)
    {
        UnicodeToUtf8(&tmp,outFile);
    }
    return OK;
}

status_t CEncoder::EncodingConvert(int from, int to, CFileBase *file)
{
    return EncodingConvert(from,to,file,file);
}

status_t CEncoder::ToLocalEncoding(int from, CFileBase *file)
{
    return EncodingConvert(from,LOCAL_ENCODING,file);
}
