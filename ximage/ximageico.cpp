// xImageICO.cpp: implementation of the CxImageICO class.
//
//////////////////////////////////////////////////////////////////////
#include "ximageico.h"
#include "syslog.h"

#if CXIMAGE_SUPPORT_PNG
#include "ximagepng.h"
#include "memfile.h"
#endif
//////////////////////////////////////////////////////////////////////
// CIconDirentry
//////////////////////////////////////////////////////////////////////
class CIconDirentry{
public:
    BYTE  bWidth;
    BYTE  bHeight;
    BYTE  bColorCount;
    BYTE  bReserved;
    WORD  wPlanes;
    WORD  wBitCount;
    DWORD dwBytesInRes;
    DWORD dwImageOffset;
public:
    CIconDirentry();
    virtual ~CIconDirentry();
    int Init();
    int Destroy();
    int Copy(CIconDirentry *p);
    int InitBasic();
    int FromFile(CFileBase *pfile);
    int ToFile(CFileBase *pfile);
    static int Size();
};
CIconDirentry::CIconDirentry()
{
    this->InitBasic();
}
CIconDirentry::~CIconDirentry()
{
    this->Destroy();
}
int CIconDirentry::InitBasic()
{
    this->bWidth = 0;
    this->bHeight = 0;
    this->bColorCount = 0;
    this->bReserved = 0;
    this->wPlanes = 0;
    this->wBitCount = 0;
    this->dwBytesInRes = 0;
    this->dwImageOffset = 0;
    return OK;
}
int CIconDirentry::Init()
{
    this->InitBasic();
    //add your code
    return OK;
}
int CIconDirentry::Destroy()
{
    //add your code
    this->InitBasic();
    return OK;
}
int CIconDirentry::Copy(CIconDirentry *p)
{
    if(this == p)
        return OK;
    this->bWidth = p->bWidth;
    this->bHeight = p->bHeight;
    this->bColorCount = p->bColorCount;
    this->bReserved = p->bReserved;
    this->wPlanes = p->wPlanes;
    this->wBitCount = p->wBitCount;
    this->dwBytesInRes = p->dwBytesInRes;
    this->dwImageOffset = p->dwImageOffset;
    return OK;
}
int CIconDirentry::FromFile(CFileBase *pfile)
{
    pfile->Read(&this->bWidth,sizeof(BYTE));
    pfile->Read(&this->bHeight,sizeof(BYTE));
    pfile->Read(&this->bColorCount,sizeof(BYTE));
    pfile->Read(&this->bReserved,sizeof(BYTE));
    pfile->Read(&this->wPlanes,sizeof(WORD));
    pfile->Read(&this->wBitCount,sizeof(WORD));
    pfile->Read(&this->dwBytesInRes,sizeof(DWORD));
    pfile->Read(&this->dwImageOffset,sizeof(DWORD));
    return TRUE;
}
int CIconDirentry::ToFile(CFileBase *pfile)
{
    pfile->Write(&this->bWidth,sizeof(BYTE));
    pfile->Write(&this->bHeight,sizeof(BYTE));
    pfile->Write(&this->bColorCount,sizeof(BYTE));
    pfile->Write(&this->bReserved,sizeof(BYTE));
    pfile->Write(&this->wPlanes,sizeof(WORD));
    pfile->Write(&this->wBitCount,sizeof(WORD));
    pfile->Write(&this->dwBytesInRes,sizeof(DWORD));
    pfile->Write(&this->dwImageOffset,sizeof(DWORD));
    return TRUE;
}
int CIconDirentry::Size()
{
    int size = 0;
    size += sizeof(BYTE);
    size += sizeof(BYTE);
    size += sizeof(BYTE);
    size += sizeof(BYTE);
    size += sizeof(WORD);
    size += sizeof(WORD);
    size += sizeof(DWORD);
    size += sizeof(DWORD);
    return size;
}
//////////////////////////////////////////////////////////////////////
// CIconHeader
//////////////////////////////////////////////////////////////////////
class CIconHeader{
public:
    WORD          idReserved;
    WORD          idType;
    WORD          idCount;
public:
    CIconHeader();
    virtual ~CIconHeader();
    int Init();
    int Destroy();
    int Copy(CIconHeader *p);
    int InitBasic();
    int FromFile(CFileBase *pfile);
    int ToFile(CFileBase *pfile);
    static int Size();
};
CIconHeader::CIconHeader()
{
    this->InitBasic();
}
CIconHeader::~CIconHeader()
{
    this->Destroy();
}
int CIconHeader::InitBasic()
{
    this->idReserved = 0;
    this->idType = 0;
    this->idCount = 0;

    return OK;
}
int CIconHeader::Init()
{
    this->InitBasic();
    //add your code
    return OK;
}
int CIconHeader::Destroy()
{
    //add your code
    this->InitBasic();
    return OK;
}
int CIconHeader::Copy(CIconHeader *p)
{
    if(this == p)
        return OK;
    this->idReserved = p->idReserved;
    this->idType = p->idType;
    this->idCount = p->idCount;
    return OK;
}
int CIconHeader::FromFile(CFileBase *pfile)
{
    pfile->Read(&this->idReserved,sizeof(WORD));
    pfile->Read(&this->idType,sizeof(WORD));
    pfile->Read(&this->idCount,sizeof(WORD));
    return TRUE;
}
int CIconHeader::ToFile(CFileBase *pfile)
{
    pfile->Write(&this->idReserved,sizeof(WORD));
    pfile->Write(&this->idType,sizeof(WORD));
    pfile->Write(&this->idCount,sizeof(WORD));
    return TRUE;
}
int CIconHeader::Size()
{
    int size = 0;
    size += sizeof(WORD);
    size += sizeof(WORD);
    size += sizeof(WORD);
    return size;
}

//////////////////////////////////////////////////////////////////////
// CxImageICO
//////////////////////////////////////////////////////////////////////
CxImageICO::CxImageICO()
{
    this->StartUp(CXIMAGE_FORMAT_ICO);
    m_dwImageOffset=0;
}

BOOL CxImageICO::Decode(CFileBase *hFile)
{
    ASSERT(hFile);
    
    hFile->Seek(0);
    fsize_t off = hFile->GetOffset(); //<yuandi>
    int page=info.nFrame;   //internal icon structure indexes
    
    // read the first part of the header
    CIconHeader icon_header;
    icon_header.FromFile(hFile);
    
    icon_header.idType = ntohs(icon_header.idType);
    icon_header.idCount = ntohs(icon_header.idCount);
    
    // check if it's an icon or a cursor
    if ((icon_header.idReserved == 0) && ((icon_header.idType == 1)||(icon_header.idType == 2))) {
        
        info.nNumFrames = icon_header.idCount;
        
        // load the icon descriptions
        CIconDirentry *icon_list;
        NEW_ARRAY(icon_list,CIconDirentry,icon_header.idCount);

        int c;
        for (c = 0; c < icon_header.idCount; c++) 
        {
            icon_list[c].FromFile(hFile);
            
            icon_list[c].wPlanes = ntohs(icon_list[c].wPlanes);
            icon_list[c].wBitCount = ntohs(icon_list[c].wBitCount);
            icon_list[c].dwBytesInRes = ntohl(icon_list[c].dwBytesInRes);
            icon_list[c].dwImageOffset = ntohl(icon_list[c].dwImageOffset);
        }
        
        if ((page>=0)&&(page<icon_header.idCount)){
            
            if (info.nEscape == -1) {
                // Return output dimensions only
                head.biWidth = icon_list[page].bWidth;
                head.biHeight = icon_list[page].bHeight;
#if CXIMAGE_SUPPORT_PNG
                if (head.biWidth==0 && head.biHeight==0)
                {   // Vista icon support
                    hFile->Seek(off + icon_list[page].dwImageOffset);
                    CxImagePNG png;
                    png.SetEscape(-1);
                    if (png.Decode(hFile)){
                        Transfer(&png,true);
                        info.nNumFrames = icon_header.idCount;
                    }
                }
#endif //CXIMAGE_SUPPORT_PNG
                DEL_ARRAY(icon_list);
                info.dwType = CXIMAGE_FORMAT_ICO;
                return TRUE;
            }
            
            // get the bit count for the colors in the icon <CoreyRLucier>
            BITMAPINFOHEADER bih;
            hFile->Seek(off + icon_list[page].dwImageOffset);
            
            if (icon_list[page].bWidth==0 && icon_list[page].bHeight==0)
            {   // Vista icon support
#if CXIMAGE_SUPPORT_PNG
                CxImagePNG png;
                if (png.Decode(hFile)){
                    Transfer(&png,true);
                    info.nNumFrames = icon_header.idCount;
                }
                SetType(CXIMAGE_FORMAT_ICO);
#endif //CXIMAGE_SUPPORT_PNG
            }
            else
            {   // standard icon
                hFile->Read(&bih,sizeof(BITMAPINFOHEADER));
                
                bihtoh(&bih);
                
                c = bih.biBitCount;
                
                // allocate memory for one icon
                Create(icon_list[page].bWidth,icon_list[page].bHeight, c, CXIMAGE_FORMAT_ICO);  //image creation
                
                // read the palette
                RGBQUAD pal[256];
                if (bih.biClrUsed)
                    hFile->Read(pal,bih.biClrUsed*sizeof(RGBQUAD));
                else
                    hFile->Read(pal,head.biClrUsed*sizeof(RGBQUAD));
                
                SetPalette(pal,head.biClrUsed); //palette assign
                
                //read the icon
                if (c<=24){
                    hFile->Read(info.pImage, head.biSizeImage);
                } else { // 32 bit icon
                    BYTE* buf;
                    MALLOC(buf,BYTE,4*head.biHeight*head.biWidth);
                    BYTE* src = buf;
                    hFile->Read(buf, 4*head.biHeight*head.biWidth);
#if CXIMAGE_SUPPORT_ALPHA
                    if (!AlphaIsValid()) AlphaCreate();
#endif //CXIMAGE_SUPPORT_ALPHA
                    for (long y = 0; y < head.biHeight; y++) {
                        BYTE* dst = GetBits(y);
                        for(long x=0;x<head.biWidth;x++){
                            *dst++=src[0];
                            *dst++=src[1];
                            *dst++=src[2];
#if CXIMAGE_SUPPORT_ALPHA
                            AlphaSet(x,y,src[3]);
#endif //CXIMAGE_SUPPORT_ALPHA
                            src+=4;
                        }
                    }
                    FREE(buf);
                }
                // apply the AND and XOR masks
                int maskwdt = ((head.biWidth+31) / 32) * 4; //line width of AND mask (always 1 Bpp)
                int masksize = head.biHeight * maskwdt;             //size of mask
                BYTE *mask;
                MALLOC(mask,BYTE,masksize);
                if (hFile->Read(mask, masksize)){
                    
                    BOOL bGoodMask=FALSE;
                    for (int im=0;im<masksize;im++){
                        if (mask[im]!=255){
                            bGoodMask=TRUE;
                            break;
                        }
                    }
                    
                    if (bGoodMask){
#if CXIMAGE_SUPPORT_ALPHA
                        BOOL bNeedAlpha = FALSE;
                        if (!AlphaIsValid()){
                            AlphaCreate();
                        } else { 
                            bNeedAlpha=TRUE; //32bit icon
                        }
                        int x,y;
                        for (y = 0; y < head.biHeight; y++) {
                            for (x = 0; x < head.biWidth; x++) {
                                if (((mask[y*maskwdt+(x>>3)]>>(7-x%8))&0x01)){
                                    AlphaSet(x,y,0);
                                    bNeedAlpha=TRUE;
                                }
                            }
                        }
                        if (!bNeedAlpha) AlphaDelete();
#endif //CXIMAGE_SUPPORT_ALPHA
                        
                        //check if there is only one transparent color
                        RGBQUAD cc,ct;
                        long* pcc = (long*)&cc;
                        long* pct = (long*)&ct;
                        int nTransColors=0;
                        int nTransIndex=0;
                        for (y = 0; y < head.biHeight; y++){
                            for (x = 0; x < head.biWidth; x++){
                                if (((mask[y*maskwdt+(x>>3)] >> (7-x%8)) & 0x01)){
                                    cc = GetPixelColor(x,y,FALSE);
                                    if (nTransColors==0){
                                        nTransIndex = GetPixelIndex(x,y);
                                        nTransColors++;
                                        ct = cc;
                                    } else {
                                        if (*pct!=*pcc){
                                            nTransColors++;
                                        }
                                    }
                                }
                            }
                        }
                        if (nTransColors==1){
                            SetTransColor(ct);
                            SetTransIndex(nTransIndex);
#if CXIMAGE_SUPPORT_ALPHA
                            AlphaDelete(); //because we have a unique transparent color in the image
#endif //CXIMAGE_SUPPORT_ALPHA
                        }
                        
                        // <vho> - Transparency support w/o Alpha support
                        if (c <= 8){ // only for icons with less than 256 colors (XP icons need alpha).
                            
                            // find a color index, which is not used in the image
                            // it is almost sure to find one, bcs. nobody uses all possible colors for an icon
                            
                            BYTE colorsUsed[256];
                            memset(colorsUsed, 0, sizeof(colorsUsed));
                            
                            for (y = 0; y < head.biHeight; y++){
                                for (x = 0; x < head.biWidth; x++){
                                    colorsUsed[BlindGetPixelIndex(x,y)] = 1;
                                }
                            }
                            
                            int iTransIdx = -1;
                            for (x = (int)(head.biClrUsed-1); x>=0 ; x--){
                                if (colorsUsed[x] == 0){
                                    iTransIdx = x; // this one is not in use. we may use it as transparent color
                                    break;
                                }
                            }
                            
                            // Go thru image and set unused color as transparent index if needed
                            if (iTransIdx >= 0){
                                BOOL bNeedTrans = FALSE;
                                for (y = 0; y < head.biHeight; y++){
                                    for (x = 0; x < head.biWidth; x++){
                                        // AND mask (Each Byte represents 8 Pixels)
                                        if (((mask[y*maskwdt+(x>>3)] >> (7-x%8)) & 0x01)){
                                            // AND mask is set (!=0). This is a transparent part
                                            SetPixelIndex(x, y, (BYTE)iTransIdx);
                                            bNeedTrans = TRUE;
                                        }
                                    }
                                }
                                // set transparent index if needed
                                if (bNeedTrans) SetTransIndex(iTransIdx);
#if CXIMAGE_SUPPORT_ALPHA
                                AlphaDelete(); //because we have a transparent color in the palette
#endif //CXIMAGE_SUPPORT_ALPHA
                            }
                        }
                    } else {
                        SetTransIndex(0); //empty mask, set black as transparent color
                        Negative();
                    }
                } 
                FREE(mask);
            }
            DEL_ARRAY(icon_list);
            // icon has been loaded successfully!
            return TRUE;
        }
        DEL_ARRAY(icon_list);
    }
    return FALSE;
    
}

BOOL CxImageICO::Encode(CFileBase *hFile, BOOL bAppend, int nPageCount)
{
    if (EncodeSafeCheck(hFile)) return FALSE;

    hFile->SetSize(0);
#if CXIMAGE_SUPPORT_PNG == 0
    //check format limits
    if ((head.biWidth>255)||(head.biHeight>255)){
        strcpy(info.szLastError,"Can't save this image as icon");
        return FALSE;
    }
#endif

    //prepare the palette struct
    RGBQUAD* pal=GetPalette();
    if (head.biBitCount<=8 && pal==NULL) return FALSE;

    int maskwdt=((head.biWidth+31)/32)*4; //mask line width
    int masksize=head.biHeight * maskwdt; //size of mask
    int bitcount=head.biBitCount;
    int imagesize=head.biSizeImage;
#if CXIMAGE_SUPPORT_ALPHA
    if (AlphaIsValid() && head.biClrUsed==0){
        bitcount=32;
        imagesize=4*head.biHeight*head.biWidth;
    }
#endif

    //fill the icon headers
    int nPages = nPageCount;
    if (nPages<1) nPages = 1;

    CIconHeader icon_header;

    icon_header.idReserved  = 0;
    icon_header.idType = 1;
    icon_header.idCount = nPages;

    if (!bAppend)
        m_dwImageOffset = CIconHeader::Size() + nPages * CIconDirentry::Size();

    DWORD dwBytesInRes = sizeof(BITMAPINFOHEADER)+head.biClrUsed*sizeof(RGBQUAD)+imagesize+masksize;

    CIconDirentry icon_list;

    icon_list.bWidth = (BYTE)head.biWidth;
    icon_list.bHeight = (BYTE)head.biHeight;
    icon_list.bColorCount = (BYTE)head.biClrUsed;
    icon_list.bReserved = 0;
    icon_list.wPlanes = 0;
    icon_list.wBitCount = (WORD)bitcount;
    icon_list.dwBytesInRes = dwBytesInRes;
    icon_list.dwImageOffset = m_dwImageOffset;


    BITMAPINFOHEADER bi={
        sizeof(BITMAPINFOHEADER),
        head.biWidth,
        2*head.biHeight,
        1,
        (WORD)bitcount,
        0, imagesize,
        0, 0, 0, 0
    };

#if CXIMAGE_SUPPORT_PNG // Vista icon support
    CxImagePNG png;
    png.Copy(this);

    CMemFile memfile;
    if (head.biWidth>255 || head.biHeight>255){
        icon_list.bWidth = icon_list.bHeight = 0;
        memfile.Init();
        png.Encode(&memfile);
        icon_list.dwBytesInRes = dwBytesInRes = (DWORD)memfile.GetSize();
    }
#endif //CXIMAGE_SUPPORT_PNG

    if (!bAppend){
        icon_header.idType = ntohs(icon_header.idType);
        icon_header.idCount = ntohs(icon_header.idCount);
        icon_header.ToFile(hFile);
        icon_header.idType = ntohs(icon_header.idType);
        icon_header.idCount = ntohs(icon_header.idCount);
    }


    if ((bAppend && nPageCount==info.nNumFrames) || (!bAppend && nPageCount==0)){
        icon_list.wPlanes = ntohs(icon_list.wPlanes);
        icon_list.wBitCount = ntohs(icon_list.wBitCount);
        icon_list.dwBytesInRes = ntohl(icon_list.dwBytesInRes);
        icon_list.dwImageOffset = ntohl(icon_list.dwImageOffset);
        icon_list.ToFile(hFile);
        icon_list.wPlanes = ntohs(icon_list.wPlanes);
        icon_list.wBitCount = ntohs(icon_list.wBitCount);
        icon_list.dwBytesInRes = ntohl(icon_list.dwBytesInRes);
        icon_list.dwImageOffset = ntohl(icon_list.dwImageOffset);

        m_dwImageOffset += dwBytesInRes;            //update offset for next header
    }

    if ((bAppend && nPageCount<info.nNumFrames) || (!bAppend && nPageCount==0))
    {
#if CXIMAGE_SUPPORT_PNG
        if (icon_list.bWidth==0 && icon_list.bHeight==0) {  // Vista icon support
            hFile->WriteFile(&memfile,0,dwBytesInRes);
        } else
#endif //CXIMAGE_SUPPORT_PNG
        {   // standard icon
            bihtoh(&bi);
            hFile->Write(&bi,sizeof(BITMAPINFOHEADER));         //write the image header
            bihtoh(&bi);

            BOOL bTransparent = info.nBkgndIndex >= 0;
            RGBQUAD ct = GetTransColor();
            if (pal){
                if (bTransparent) SetPaletteColor((BYTE)info.nBkgndIndex,0,0,0,0);
                hFile->Write(pal,head.biClrUsed*sizeof(RGBQUAD)); //write palette
                if (bTransparent) SetPaletteColor((BYTE)info.nBkgndIndex,ct);
            }

#if CXIMAGE_SUPPORT_ALPHA
            if (AlphaIsValid() && head.biClrUsed==0){
                BYTE* buf;
                MALLOC(buf,BYTE,imagesize);
                BYTE* dst = buf;
                for (long y = 0; y < head.biHeight; y++) {
                    BYTE* src = GetBits(y);
                    for(long x=0;x<head.biWidth;x++){
                        *dst++=*src++;
                        *dst++=*src++;
                        *dst++=*src++;
                        *dst++=AlphaGet(x,y);
                    }
                }
                hFile->Write(buf,imagesize);
                FREE(buf);
            } else {
                hFile->Write(info.pImage,imagesize);    //write image
            }
#else
            hFile->Write(info.pImage,imagesize);    //write image
#endif

            //save transparency mask
            BYTE* mask;
            MALLOC(mask,BYTE,masksize);
            memset(mask,0,masksize);

            if (!mask) return FALSE;

            //prepare the variables to build the mask
            BYTE* iDst;
            int pos,i;
            RGBQUAD c={0,0,0,0};
            long* pc = (long*)&c;
            long* pct= (long*)&ct;
#if CXIMAGE_SUPPORT_ALPHA
            BOOL bAlphaPaletteIsValid = AlphaPaletteIsValid();
            BOOL bAlphaIsValid = AlphaIsValid();
#endif
            //build the mask
            for (int y = 0; y < head.biHeight; y++) {
                for (int x = 0; x < head.biWidth; x++) {
                    i=0;
#if CXIMAGE_SUPPORT_ALPHA
                    if (bAlphaIsValid && AlphaGet(x,y)==0) i=1;
                    if (bAlphaPaletteIsValid && BlindGetPixelColor(x,y,TRUE).rgbReserved==0) i=1;
#endif
                    c=GetPixelColor(x,y,FALSE);
                    if (bTransparent && *pc==*pct) i=1;
                    iDst = mask + y*maskwdt + (x>>3);
                    pos = 7-x%8;
                    *iDst &= ~(0x01<<pos);
                    *iDst |= ((i & 0x01)<<pos);
                }
            }
            //write AND/XOR masks
            hFile->Write(mask,masksize);
            FREE(mask);
        }
    }

    return TRUE;
}

BOOL CxImageICO::Encode(CFileBase *hFile, CxImage **pImages, int nPageCount)
{
    cx_try
    {
        if (hFile==NULL) cx_throw("invalid file pointer");
        if (pImages==NULL || nPageCount<=0) cx_throw("multipage ICO, no images!");
        hFile->SetSize(0);
        
        int i;
        for (i=0; i<nPageCount; i++){
            if (pImages[i]==NULL)
                cx_throw("Bad image pointer");
            if (!(pImages[i]->IsValid()))
                cx_throw("Empty image");
        }
        
        CxImageICO ghost;
        for (i=0; i<nPageCount; i++){   //write headers
            ghost.Ghost(pImages[i]);
            ghost.info.nNumFrames = nPageCount;
            if (i==0) {
                if (!ghost.Encode(hFile,(BOOL)false,nPageCount))
                    cx_throw("Error writing ICO file header");
            }
            if (!ghost.Encode(hFile,(BOOL)true,nPageCount)) 
                cx_throw("Error saving ICO image header");
        }
        for (i=0; i<nPageCount; i++){   //write bodies
            ghost.Ghost(pImages[i]);
            ghost.info.nNumFrames = nPageCount;
            if (!ghost.Encode(hFile,true,i)) 
                cx_throw("Error saving ICO body");
        }
        
    } cx_catch {
        if (strcmp(message,"")) strcpy(info.szLastError,message);
        return false;
    }
    return true;
}
