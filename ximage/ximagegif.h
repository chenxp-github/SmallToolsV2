// xImageGIF.h: interface for the CxImageGIF class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XIMAGEGIF_H__86DE3AD3_049C_4864_AFFC_88794D9029DD__INCLUDED_)
#define AFX_XIMAGEGIF_H__86DE3AD3_049C_4864_AFFC_88794D9029DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "filebase.h"
#include "ximage.h"

typedef short int       code_int;   

#define OUT_OF_MEMORY -10
#define BAD_CODE_SIZE -20
#define READ_ERROR -1
#define WRITE_ERROR -2
#define OPEN_ERROR -3
#define CREATE_ERROR -4
#define MAX_CODES   4095
#define GIFBUFTAM 16383
#define TRANSPARENCY_CODE 0xF9

//LZW GIF Image compression
#define MAXBITSCODES    12
#define HSIZE  5003     /* 80% occupancy */
#define MAXCODE(n_bits) (((code_int) 1 << (n_bits)) - 1)
#define HashTabOf(i)    htab[i]
#define CodeTabOf(i)    codetab[i]

class struct_gifgce{
public:
    BYTE flags;
    WORD delaytime;
    BYTE transpcolindex;
public:
    struct_gifgce();
    int Copy(struct_gifgce *p);
    int InitBasic();
    int FromFile(CFileBase *pfile);
    int ToFile(CFileBase *pfile);
    static int Size();
};

class struct_dscgif{
public:
    char header[6];
    WORD scrwidth;
    WORD scrheight;
    char pflds;
    char bcindx;
    char pxasrat;
public:
    struct_dscgif();
    int Copy(struct_dscgif *p);
    int InitBasic();
    int FromFile(CFileBase *pfile);
    int ToFile(CFileBase *pfile);
    static int Size();
};

class struct_image{
public:
    WORD l;
    WORD t;
    WORD w;
    WORD h;
    BYTE pf;
public:
    struct_image();
    int Copy(struct_image *p);
    int InitBasic();
    int FromFile(CFileBase *pfile);
    int ToFile(CFileBase *pfile);
    static int Size();
};

class struct_TabCol{
public:
    short colres;
    short sogct;
    rgb_color paleta[256];
public:
    struct_TabCol();
    int Copy(struct_TabCol *p);
    int InitBasic();
    int FromFile(CFileBase *pfile);
    int ToFile(CFileBase *pfile);
    static int Size();
};

class struct_RLE{
public:
    int rl_pixel;
    int rl_basecode;
    int rl_count;
    int rl_table_pixel;
    int rl_table_max;
    int just_cleared;
    int out_bits;
    int out_bits_init;
    int out_count;
    int out_bump;
    int out_bump_init;
    int out_clear;
    int out_clear_init;
    int max_ocodes;
    int code_clear;
    int code_eof;
    unsigned int obuf;
    int obits;
    unsigned char oblock[256];
    int oblen;
public:
    struct_RLE();
    int Copy(struct_RLE *p);
    int InitBasic();
    int FromFile(CFileBase *pfile);
    int ToFile(CFileBase *pfile);
    static int Size();
};

class CxImageGIF : public CxImage
{
public:
    struct_gifgce gifgce;
    
    int             curx, cury;
    long             CountDown;
    unsigned long    cur_accum;
    int              cur_bits;
    int interlaced, iypos, istep, iheight, ipass;
    int ibf;
    int ibfmax;

    BYTE buf[GIFBUFTAM + 1];
    
    short curr_size;                     /* The current code size */
    short clear;                         /* Value for a clear code */
    short ending;                        /* Value for a ending code */
    short newcodes;                      /* First available code */
    short top_slot;                      /* Highest code for current size */
    short slot;                          /* Last read code */

    short navail_bytes;              /* # bytes left in block */
    short nbits_left;                /* # bits left in current BYTE */
    BYTE b1;                           /* Current BYTE */
    BYTE byte_buff[257];               /* Current block */
    BYTE *pbytes;                      /* Pointer to next BYTE in block */

    BYTE stack[MAX_CODES + 1];            /* Stack for storing pixels */
    BYTE suffix[MAX_CODES + 1];           /* Suffix table */
    WORD prefix[MAX_CODES + 1];           /* Prefix linked list */

    long htab [HSIZE];
    unsigned short codetab [HSIZE];
    int n_bits;             /* number of bits/code */
    code_int maxcode;       /* maximum code, given n_bits */
    code_int free_ent;      /* first unused entry */
    int clear_flg;
    int g_init_bits;
    CFileBase* g_outfile;
    int ClearCode;
    int EOFCode;

    int a_count;
    char accum[256];

    char m_comment[256];
    int m_loops;
public:
    BOOL DecodeExtension(CFileBase *fp);
    int out_line(CImageIterator* iter, unsigned char *pixels, int linelen);
    short get_next_code(CFileBase* file);
    short init_exp(short size);
    fsize_t seek_next_image(CFileBase* fp, fsize_t position);
    int get_byte(CFileBase* file);
    short decoder(CFileBase* file, CImageIterator* iter, short linewidth, int &bad_code_count);
    int get_num_frames(CFileBase *fp,struct_TabCol* TabColSrc,struct_dscgif* dscgif);
    void GifMix(CxImage *imgsrc2, struct_image *imgdesc);
    void EncodeLoopExtension(CFileBase *fp);
    void cl_hash(long hsize);
    void compressLZW( int init_bits, CFileBase* outfile);
    void flush_char();
    void char_out(int c);
    void output( code_int  code);
    void compressNONE( int init_bits, CFileBase* outfile);
    void EncodeBody(CFileBase *fp, BOOL bLocalColorMap);
    void EncodeExtension(CFileBase *fp);
    void EncodeComment(CFileBase *fp);
    void Putword(int w, CFileBase *fp );
    void EncodeHeader(CFileBase *fp);
    BOOL EncodeRGB(CFileBase *fp);
    BYTE GetPixelIndex(long x,long y);
    int GifNextPixel( );
    CxImageGIF();
    virtual ~CxImageGIF();

    BOOL Decode(CFileBase * fp);
#if CXIMAGE_SUPPORT_ENCODE
    BOOL Encode(CFileBase * fp);
    BOOL Encode(CFileBase * fp, CxImage ** pImages, int pagecount, BOOL bLocalColorMap = false, BOOL bLocalDispMeth = false, int *running = NULL);
#endif // CXIMAGE_SUPPORT_ENCODE

    void SetLoops(int loops);
    long GetLoops();
    void SetComment(const char* sz_comment_in);
    void GetComment(char* sz_comment_out);

    void compressRLE( int init_bits, CFileBase* outfile);
    void rle_clear(struct_RLE* rle);
    void rle_flush(struct_RLE* rle);
    void rle_flush_withtable(int count, struct_RLE* rle);
    void rle_flush_clearorrep(int count, struct_RLE* rle);
    void rle_flush_fromclear(int count,struct_RLE* rle);
    void rle_output_plain(int c,struct_RLE* rle);
    void rle_reset_out_clear(struct_RLE* rle);
    unsigned int rle_compute_triangle_count(unsigned int count, unsigned int nrepcodes);
    unsigned int rle_isqrt(unsigned int x);
    void rle_write_block(struct_RLE* rle);
    void rle_block_out(unsigned char c, struct_RLE* rle);
    void rle_block_flush(struct_RLE* rle);
    void rle_output(int val, struct_RLE* rle);
    void rle_output_flush(struct_RLE* rle);
};

#endif // !defined(AFX_XIMAGEGIF_H__86DE3AD3_049C_4864_AFFC_88794D9029DD__INCLUDED_)
