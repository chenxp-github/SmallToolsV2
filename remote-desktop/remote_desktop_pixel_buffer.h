#ifndef __REMOTE_DESKTOP_PIXEL_BUFFER_H
#define __REMOTE_DESKTOP_PIXEL_BUFFER_H

/*##Begin Inlcudes##*/
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"
#include "ximage.h"
#include "minibson.h"

class CRemoteDesktopPixelBufferList;
class CRemoteDesktopPixelBuffer
/*##Begin Bases##*/
/*##End Bases##*/
{
/*##Begin Members##*/
public:
public:
    WEAK_REF_DEFINE();

public:
    uint8_t *m_pixel_data;
    int m_pixel_data_size;
    uint8_t m_byte_per_pixel;
    int m_width;
    int m_height;
    int m_offset_x;
    int m_offset_y;
    uint32_t m_flags;
/*##End Members##*/

public:
	FLAG_FUNC(m_flags,IsDataCompressed,0x00000001);
	FLAG_FUNC(m_flags,IsGhost,0x80000000);

    CRemoteDesktopPixelBuffer();
    virtual ~CRemoteDesktopPixelBuffer();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CRemoteDesktopPixelBuffer *_p);
    int Comp(CRemoteDesktopPixelBuffer *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
    uint8_t* GetPixelData();
    int GetPixelDataSize();
    uint8_t GetPixelDataElem(int _index);
    uint8_t GetBytePerPixel();
    int GetWidth();
    int GetHeight();
    int GetOffsetX();
    int GetOffsetY();
    uint32_t GetFlags();
/*##End Getter_H##*/
/*##Begin Setter_H##*/
    status_t AllocPixelData(int _len);
    status_t SetPixelData(uint8_t _pixel_data[], int _len);
    status_t SetPixelData(uint8_t *_pixel_data[], int _len);
    status_t SetPixelDataElem(int _index,uint8_t _pixel_data);
    status_t SetBytePerPixel(uint8_t _byte_per_pixel);
    status_t SetWidth(int _width);
    status_t SetHeight(int _height);
    status_t SetOffsetX(int _offset_x);
    status_t SetOffsetY(int _offset_y);
    status_t SetFlags(uint32_t _flags);
/*##End Setter_H##*/

/*##Begin Bson##*/
    status_t SaveBson(CMiniBson *_bson);
    status_t SaveBson(CMem *_mem);
    status_t LoadBson(CMiniBson *_bson);
    status_t LoadBson(CFileBase *_file);
/*##End Bson##*/

	status_t Create(int width, int height, uint8_t bpp);
	uint8_t *GetPixelBits(int x, int y);	
	status_t Crop(int x, int y, int w, int h, CRemoteDesktopPixelBuffer *out);
	status_t Combine(CRemoteDesktopPixelBuffer *pixelBuf);
	status_t ChangeBpp(uint8_t bpp,CRemoteDesktopPixelBuffer *out);	
	status_t ChangeBpp(uint8_t bpp);	
	status_t SaveToXimage(CxImage *img);
	status_t SaveToXimage(const char *fn);
	status_t LoadFromXimage(CxImage *img);
	status_t LoadFromXimage(const char *fn);
	bool IsSame(CRemoteDesktopPixelBuffer *pixelBuf);	
	status_t Diff(CRemoteDesktopPixelBuffer *pixelBuf, int tileWidth, int tileHeight,CRemoteDesktopPixelBufferList *outList);
	status_t ToTiles(int tileWidth, int tileHeight, CRemoteDesktopPixelBufferList *tiles);
	status_t ApplyPatch(CRemoteDesktopPixelBufferList *patch);

	status_t CompressData();
	status_t DecompressData();	
	status_t Transfer(CRemoteDesktopPixelBuffer *from);
	status_t Ghost(CRemoteDesktopPixelBuffer *from);
	bool IsSizeOrBppChanged(CRemoteDesktopPixelBuffer *pbuf);

    inline uint8_t *GetPixelBits_Unsafe(int x, int y)
    {
        return m_pixel_data +((y*m_width + x)*m_byte_per_pixel);
    }
};

#endif
