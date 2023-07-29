#include "remote_desktop_pixel_buffer.h"
#include "sys_log.h"
#include "mem_tool.h"
#include "ximagehelper.h"
#include "remote_desktop_pixel_buffer_list.h"
#include "zlib_zlib.h"

CRemoteDesktopPixelBuffer::CRemoteDesktopPixelBuffer()
{
    this->InitBasic();
}

CRemoteDesktopPixelBuffer::~CRemoteDesktopPixelBuffer()
{
    this->Destroy();
}

status_t CRemoteDesktopPixelBuffer::InitBasic()
{
/*##Begin InitBasic##*/
    WEAK_REF_CLEAR();
    this->m_pixel_data = NULL;
    this->m_pixel_data_size = 0;
    this->m_byte_per_pixel = 0;
    this->m_width = 0;
    this->m_height = 0;
    this->m_offset_x = 0;
    this->m_offset_y = 0;
    this->m_flags = 0;
/*##End InitBasic##*/
    return OK;
}

status_t CRemoteDesktopPixelBuffer::Init()
{
    this->InitBasic();
/*##Begin Init##*/
/*##End Init##*/
    return OK;
}

status_t CRemoteDesktopPixelBuffer::Destroy()
{
    WEAK_REF_DESTROY();

	if(!IsGhost())
    {
		DEL_ARRAY(m_pixel_data);
	}

    this->InitBasic();
    return OK;
}

status_t CRemoteDesktopPixelBuffer::Copy(CRemoteDesktopPixelBuffer *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;
    this->AllocPixelData(_p->m_pixel_data_size);
	memcpy(m_pixel_data,_p->m_pixel_data,_p->m_pixel_data_size);
    this->m_byte_per_pixel = _p->m_byte_per_pixel;
    this->m_width = _p->m_width;
    this->m_height = _p->m_height;
    this->m_offset_x = _p->m_offset_x;
    this->m_offset_y = _p->m_offset_y;
	this->m_flags = _p->m_flags;
	this->SetIsGhost(false);
    return OK;
}

int CRemoteDesktopPixelBuffer::Comp(CRemoteDesktopPixelBuffer *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CRemoteDesktopPixelBuffer::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
    int i = 0;

    _buf->Log("pixel_data = [");
    _buf->IncLogLevel(1);
    for(i = 0; i < m_pixel_data_size; i++)
    {
        _buf->Log("%d,",m_pixel_data[i]);
    }
    _buf->IncLogLevel(-1);
    _buf->Log("]");
    _buf->Log("byte_per_pixel = %d",m_byte_per_pixel);
    _buf->Log("width = %d",m_width);
    _buf->Log("height = %d",m_height);
    _buf->Log("offset_x = %d",m_offset_x);
    _buf->Log("offset_y = %d",m_offset_y);
    _buf->Log("flags = %u",m_flags);
/*##End Print##*/
    return OK;
}

/*@@Begin Function GetPixelData@@*/
uint8_t* CRemoteDesktopPixelBuffer::GetPixelData()
{
    return m_pixel_data;
}
/*@@End  Function GetPixelData@@*/

/*@@Begin Function GetPixelDataSize@@*/
int CRemoteDesktopPixelBuffer::GetPixelDataSize()
{
    return m_pixel_data_size;
}
/*@@End  Function GetPixelDataSize@@*/

/*@@Begin Function GetPixelDataElem@@*/
uint8_t CRemoteDesktopPixelBuffer::GetPixelDataElem(int _index)
{
    ASSERT(this->m_pixel_data);
    ASSERT(_index >= 0 && _index < m_pixel_data_size);
    return m_pixel_data[_index];
}
/*@@End  Function GetPixelDataElem@@*/

/*@@Begin Function GetWidth@@*/
int CRemoteDesktopPixelBuffer::GetWidth()
{
    return m_width;
}
/*@@End  Function GetWidth@@*/

/*@@Begin Function GetHeight@@*/
int CRemoteDesktopPixelBuffer::GetHeight()
{
    return m_height;
}
/*@@End  Function GetHeight@@*/

/*@@Begin Function AllocPixelData@@*/
status_t CRemoteDesktopPixelBuffer::AllocPixelData(int _len)
{
    if(m_pixel_data_size == _len)
	{
		return OK;
	}

	if(!IsGhost())
    {
		DEL_ARRAY(this->m_pixel_data);
	}

    if(_len > 0)
    {
        NEW_ARRAY(this->m_pixel_data,uint8_t,_len);
        memset(this->m_pixel_data,0,sizeof(uint8_t)*_len);
		this->SetIsGhost(false);
    }
    this->m_pixel_data_size = _len;
    return OK;
}
/*@@End  Function AllocPixelData@@*/

/*@@Begin Function SetPixelData@@*/
status_t CRemoteDesktopPixelBuffer::SetPixelData(uint8_t _pixel_data[], int _len)
{
    ASSERT(_pixel_data);
    this->AllocPixelData(_len);
    memcpy(this->m_pixel_data,_pixel_data,sizeof(uint8_t)*_len);
    return OK;
}
/*@@End  Function SetPixelData@@*/

/*@@Begin Function SetPixelData_V2@@*/
status_t CRemoteDesktopPixelBuffer::SetPixelData(uint8_t *_pixel_data[], int _len)
{
    ASSERT(_pixel_data);
    this->AllocPixelData(_len);
    memcpy(this->m_pixel_data,_pixel_data,sizeof(uint8_t)*_len);
    return OK;
}
/*@@End  Function SetPixelData_V2@@*/

/*@@Begin Function SetPixelDataElem@@*/
status_t CRemoteDesktopPixelBuffer::SetPixelDataElem(int _index,uint8_t _pixel_data)
{
    ASSERT(this->m_pixel_data);
    ASSERT(_index >= 0 && _index < m_pixel_data_size);
    this->m_pixel_data[_index] = _pixel_data;
    return OK;
}
/*@@End  Function SetPixelDataElem@@*/

/*@@Begin Function SetWidth@@*/
status_t CRemoteDesktopPixelBuffer::SetWidth(int _width)
{
    this->m_width = _width;
    return OK;
}
/*@@End  Function SetWidth@@*/

/*@@Begin Function SetHeight@@*/
status_t CRemoteDesktopPixelBuffer::SetHeight(int _height)
{
    this->m_height = _height;
    return OK;
}
/*@@End  Function SetHeight@@*/


/*@@Begin Function GetBytePerPixel@@*/
uint8_t CRemoteDesktopPixelBuffer::GetBytePerPixel()
{
    return m_byte_per_pixel;
}
/*@@End  Function GetBytePerPixel@@*/

/*@@Begin Function SetBytePerPixel@@*/
status_t CRemoteDesktopPixelBuffer::SetBytePerPixel(uint8_t _byte_per_pixel)
{
    this->m_byte_per_pixel = _byte_per_pixel;
    return OK;
}
/*@@End  Function SetBytePerPixel@@*/

/*@@Begin Function GetOffsetX@@*/
int CRemoteDesktopPixelBuffer::GetOffsetX()
{
    return m_offset_x;
}
/*@@End  Function GetOffsetX@@*/

/*@@Begin Function GetOffsetY@@*/
int CRemoteDesktopPixelBuffer::GetOffsetY()
{
    return m_offset_y;
}
/*@@End  Function GetOffsetY@@*/

/*@@Begin Function SetOffsetX@@*/
status_t CRemoteDesktopPixelBuffer::SetOffsetX(int _offset_x)
{
    this->m_offset_x = _offset_x;
    return OK;
}
/*@@End  Function SetOffsetX@@*/

/*@@Begin Function SetOffsetY@@*/
status_t CRemoteDesktopPixelBuffer::SetOffsetY(int _offset_y)
{
    this->m_offset_y = _offset_y;
    return OK;
}
/*@@End  Function SetOffsetY@@*/

/*@@Begin Function GetFlags@@*/
uint32_t CRemoteDesktopPixelBuffer::GetFlags()
{
    return m_flags;
}
/*@@End  Function GetFlags@@*/

/*@@Begin Function SetFlags@@*/
status_t CRemoteDesktopPixelBuffer::SetFlags(uint32_t _flags)
{
    this->m_flags = _flags;
    return OK;
}
/*@@End  Function SetFlags@@*/
/*@@ Insert Function Here @@*/

status_t CRemoteDesktopPixelBuffer::Create(int width, int height, uint8_t bpp)
{
	ASSERT(bpp>=1&&bpp <=4);
	int size = bpp * width *height;
	this->SetWidth(width);
	this->SetHeight(height);
	this->SetBytePerPixel(bpp);
	this->AllocPixelData(size);
	return OK;
}

uint8_t *CRemoteDesktopPixelBuffer::GetPixelBits(int x, int y)
{
	ASSERT(x>=0 && x<(int)m_width);
	ASSERT(y>=0 && y<(int)m_height);
	return this->GetPixelBits_Unsafe(x,y);
}

status_t CRemoteDesktopPixelBuffer::Crop(int x, int y, int w, int h, CRemoteDesktopPixelBuffer *out)
{
	ASSERT(out);
	
	ASSERT(x>=0 && x<m_width);
	ASSERT(y>=0 && y<m_height);
	if(w > m_width-x) w = m_width-x;
	if(h > m_height-y) h = m_height-y;
	out->Create(w,h,m_byte_per_pixel);
	int line_bytes = w*m_byte_per_pixel;

	for(int i = y; i < y+h; i++)
	{
		uint8_t *saddr = this->GetPixelBits_Unsafe(x,i);
		uint8_t *daddr = out->GetPixelBits_Unsafe(0,i-y);
		memcpy(daddr,saddr,line_bytes);
	}

	out->SetOffsetX(x);
	out->SetOffsetY(y);
	return OK;
}

status_t CRemoteDesktopPixelBuffer::Combine(CRemoteDesktopPixelBuffer *pixelBuf)
{
	ASSERT(pixelBuf);
	ASSERT(m_byte_per_pixel == pixelBuf->GetBytePerPixel());
	ASSERT(!this->IsDataCompressed());
	ASSERT(!pixelBuf->IsDataCompressed());

	int x = pixelBuf->GetOffsetX();
	int y = pixelBuf->GetOffsetY();
	int w = pixelBuf->GetWidth();
	int h = pixelBuf->GetHeight();

	ASSERT(x>=0 && x<m_width);
	ASSERT(y>=0 && y<m_height);
	if(w > m_width-x) w = m_width-x;
	if(h > m_height-y) h = m_height-y;
	int line_bytes = w*m_byte_per_pixel;
	for(int i = 0; i < h; i++)
	{
		uint8_t *saddr = pixelBuf->GetPixelBits_Unsafe(0,i);
		uint8_t *daddr = this->GetPixelBits_Unsafe(x,i+y);
		memcpy(daddr,saddr,line_bytes);
	}

	return OK;
}

status_t CRemoteDesktopPixelBuffer::ChangeBpp(uint8_t bpp,CRemoteDesktopPixelBuffer *out)
{
	ASSERT(out);	
	out->Create(m_width,m_height,bpp);
	int len = m_width*m_height;

	if(m_byte_per_pixel == bpp)
	{
		crt_memcpy(out->GetPixelData(),this->GetPixelData(),this->GetPixelDataSize());
		return OK;
	}

	if(m_byte_per_pixel == 3 && bpp == 1)
	{
		for(int i = 0; i < len; i++)
		{			
			uint8_t *p = m_pixel_data+i*3;
			out->m_pixel_data[i] = (p[0]&0xc0) | ((p[1]>>2)&0x38)|((p[2]>>5)&0x07);
		}
	}
	
	else if(m_byte_per_pixel == 1 && bpp == 3)
	{
		for(int i = 0; i < len; i++)
		{			
			uint8_t c = m_pixel_data[i];
			uint8_t *p = out->m_pixel_data+i*3;
			p[0] = (c&0xc0);
			p[1] = (c&0x38)<<2;
			p[2] = (c&0x07)<<5;
		}
	}
	
	else if(m_byte_per_pixel == 3 && bpp == 2)
	{
		for(int i = 0; i < len; i++)
		{			
			uint8_t  *p = m_pixel_data+i*3;
			uint16_t *d = (uint16_t*)(out->m_pixel_data+i*2); 
			*d = ((p[0]&0xf8)<<8)|((p[1]&0xfc)<<3)|((p[2]&0xf8)>>3);
		}
	}
	
	else if(m_byte_per_pixel == 2 && bpp == 3)
	{
		for(int i = 0; i < len; i++)
		{			
			uint16_t *s = (uint16_t*)(m_pixel_data + i*2);
			uint8_t *d = out->m_pixel_data + i*3;
			uint16_t c = *s;
			d[0] = (c>>8)&0xf8;
			d[1] = (c>>3)&0xfc;
			d[2] = (c<<3)&0xf8;
		}
	}

	else if(m_byte_per_pixel == 4 && bpp == 1)
	{
		for(int i = 0; i < len; i++)
		{			
			uint8_t *p = m_pixel_data+i*4;
			out->m_pixel_data[i] = (p[2]&0xc0) | ((p[1]>>2)&0x38)|((p[0]>>5)&0x07);
		}
	}

	else if(m_byte_per_pixel == 4 && bpp == 2)
	{
		for(int i = 0; i < len; i++)
		{			
			uint8_t  *p = m_pixel_data+i*4;
			uint16_t *d = (uint16_t*)(out->m_pixel_data+i*2); 
			*d = ((p[2]&0xf8)<<8)|((p[1]&0xfc)<<3)|((p[0]&0xf8)>>3);
		}
	}

	else if(m_byte_per_pixel == 4 && bpp == 3)
	{
		for(int i = 0; i < len; i++)
		{			
			uint8_t *src = m_pixel_data+i*4;
			uint8_t *dst = out->m_pixel_data+i*3;
			dst[2] = src[0];
			dst[1] = src[1];
			dst[0] = src[2];
		}
	}
	else
	{
		XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
			"not supported bpp changed(%d->%d)",m_byte_per_pixel,bpp
		);
		return ERROR;
	}
	return OK;
}

status_t CRemoteDesktopPixelBuffer::ChangeBpp(uint8_t bpp)
{
	if(m_byte_per_pixel == bpp)
		return OK;
	CRemoteDesktopPixelBuffer tmp;
	tmp.Init();
	status_t r = this->ChangeBpp(bpp,&tmp);
	this->AllocPixelData(0);
	m_pixel_data = tmp.m_pixel_data;
	m_pixel_data_size = tmp.m_pixel_data_size;
	tmp.m_pixel_data = NULL;
	tmp.m_pixel_data_size = 0;
	m_byte_per_pixel = bpp;
	return r;
}

//slow
status_t CRemoteDesktopPixelBuffer::SaveToXimage(CxImage *img)
{
	ASSERT(img);
	ASSERT(!IsDataCompressed());

	CRemoteDesktopPixelBuffer *pbuf = this;

	CRemoteDesktopPixelBuffer tmp_buf;
	tmp_buf.Init();
	if(m_byte_per_pixel != 3)
	{		
		this->ChangeBpp(3,&tmp_buf);
		pbuf = &tmp_buf;
	}

	if(img->GetWidth() != this->GetWidth() || img->GetHeight()!=this->GetHeight())
	{
		SAVE_WEAK_REF_ID(*img,w);
		img->DestroyAll();
		img->Init();
		img->Create(m_width,m_height,24,CXIMAGE_FORMAT_BMP);
		RESTORE_WEAK_REF_ID(*img,w);
	}

	for(int j = 0; j <m_height; j++)
	{
		for(int i = 0; i < m_width; i++)
		{
			uint8_t *p = pbuf->GetPixelBits_Unsafe(i,j);					
			BYTE* iDst = img->info.pImage + j*img->info.dwEffWidth + i*3;
			iDst[0] = p[2];
			iDst[1] = p[1];
			iDst[2] = p[0];

		}
	}

	return OK;
}

status_t CRemoteDesktopPixelBuffer::SaveToXimage(const char *fn)
{
	CxImage tmp;
	tmp.Init();

	if(this->SaveToXimage(&tmp))
	{
		return CxImageHelper::SaveImage(&tmp,fn);
	}
	return ERROR;
}

status_t CRemoteDesktopPixelBuffer::LoadFromXimage(CxImage *img)
{
	ASSERT(img);
	
	int w = img->GetWidth();
	int h = img->GetHeight();

	this->Create(w,h,3);

	for(int j = 0; j < h; j++)
	{
		for(int i = 0; i < w; i++)
		{
			RGBQUAD c = img->GetPixelColor(i,j,FALSE);
			uint8_t *p = this->GetPixelBits_Unsafe(i,j);
			p[0] = c.rgbRed;
			p[1] = c.rgbGreen;
			p[2] = c.rgbBlue;
		}
	}

	return OK;
}

status_t CRemoteDesktopPixelBuffer::LoadFromXimage(const char *fn)
{
	ASSERT(fn);
	CxImage tmp;
	if(CxImageHelper::LoadImage(fn,&tmp))
	{
		return this->LoadFromXimage(&tmp);
	}
	return ERROR;
}

bool CRemoteDesktopPixelBuffer::IsSame(CRemoteDesktopPixelBuffer *pixelBuf)
{
	ASSERT(pixelBuf);
	if(m_width != pixelBuf->GetWidth())
		return false;
	if(m_height != pixelBuf->GetHeight())
		return false;
	if(m_byte_per_pixel != pixelBuf->m_byte_per_pixel)
		return false;
	return memcmp(m_pixel_data,pixelBuf->m_pixel_data,m_pixel_data_size) == 0;
}

status_t CRemoteDesktopPixelBuffer::SaveBson(CMiniBson *_bson)
{
    ASSERT(_bson);
	/*##Begin SaveBson_1##*/
    _bson->PutUInt8("byte_per_pixel",m_byte_per_pixel);
    _bson->PutUInt32("flags",m_flags);
    _bson->PutInt32("height",m_height);
    _bson->PutInt32("offset_x",m_offset_x);
    _bson->PutInt32("offset_y",m_offset_y);
    if(this->m_pixel_data)
    {
        _bson->PutBinary("pixel_data",m_pixel_data,sizeof(uint8_t)*m_pixel_data_size);
    }
    _bson->PutInt32("width",m_width);
/*##End SaveBson_1##*/
    return OK;
}

status_t CRemoteDesktopPixelBuffer::SaveBson(CMem *_mem)
{
    ASSERT(_mem);
    CMiniBson _bson;
    _bson.Init();
    _bson.SetRawBuf(_mem);
    _bson.StartDocument();
    this->SaveBson(&_bson);
    _bson.EndDocument();
    _mem->SetSize(_bson.GetDocumentSize());
    return OK;
}

status_t CRemoteDesktopPixelBuffer::LoadBson(CMiniBson *_bson)
{
    ASSERT(_bson);
	/*##Begin LoadBson_1##*/
    BSON_CHECK(_bson->GetUInt8("byte_per_pixel",&m_byte_per_pixel));
    BSON_CHECK(_bson->GetUInt32("flags",&m_flags));
    BSON_CHECK(_bson->GetInt32("height",&m_height));
    BSON_CHECK(_bson->GetInt32("offset_x",&m_offset_x));
    BSON_CHECK(_bson->GetInt32("offset_y",&m_offset_y));
    /*******pixel_data start********/{
    CMem _tmp_bin;
    if(_bson->GetBinary("pixel_data",&_tmp_bin))
    {
        int _tmp_len = (int)_tmp_bin.GetSize()/sizeof(uint8_t);
        this->SetPixelData((uint8_t*)_tmp_bin.GetRawBuf(),_tmp_len);
    }
    /*******pixel_data end********/}
    BSON_CHECK(_bson->GetInt32("width",&m_width));
/*##End LoadBson_1##*/
    return OK;
}

status_t CRemoteDesktopPixelBuffer::LoadBson(CFileBase *_file)
{
    ASSERT(_file);
    CMiniBson _bson;
    _bson.Init();
    _bson.LoadBson(_file);
    _bson.ResetPointer();
    return this->LoadBson(&_bson);
}

/*@@ Insert Function Here @@*/

status_t CRemoteDesktopPixelBuffer::Diff(CRemoteDesktopPixelBuffer *pixelBuf, int tileWidth, int tileHeight,CRemoteDesktopPixelBufferList *outList)
{
	ASSERT(pixelBuf && outList);
	ASSERT(m_width == pixelBuf->GetWidth());
	ASSERT(m_height == pixelBuf->GetHeight());
	ASSERT(!IsDataCompressed());
	ASSERT(!pixelBuf->IsDataCompressed());
		
	CRemoteDesktopPixelBufferList this_tiles, that_tiles;

	this_tiles.Init();
	that_tiles.Init();

	this->ToTiles(tileWidth,tileHeight,&this_tiles);
	pixelBuf->ToTiles(tileWidth,tileHeight,&that_tiles);

	ASSERT(this_tiles.GetLen() == that_tiles.GetLen());

	outList->Clear();

	for(int i = 0; i < this_tiles.GetLen(); i++)
	{
		CRemoteDesktopPixelBuffer *p1 = this_tiles.GetElem(i);
		CRemoteDesktopPixelBuffer *p2 = that_tiles.GetElem(i);

		if(!p1->IsSame(p2))
		{
			outList->Push(p2);
		}
	}

	return OK;
}

status_t CRemoteDesktopPixelBuffer::ToTiles(int tileWidth, int tileHeight, CRemoteDesktopPixelBufferList *tiles)
{
	ASSERT(tileWidth >= 8 && tileHeight>=8);
	ASSERT(tiles);

	int cols = m_width / tileWidth;
	int rows = m_height / tileHeight;

	if(tileWidth*cols < m_width)
		cols++;
	if(tileHeight*rows < m_height)
		rows++;

	for(int i = 0; i < rows; i++)
	{
		for(int j = 0; j < cols; j++)
		{
			int offsetx = j*tileWidth;
			int offsety = i*tileHeight;
			
			CRemoteDesktopPixelBuffer *pbuf;
			NEW(pbuf,CRemoteDesktopPixelBuffer);
			pbuf->Init();
			if(this->Crop(offsetx,offsety,tileWidth,tileHeight,pbuf))
			{
				tiles->PushPtr(pbuf);
			}
		}
	}

	return OK;
}

status_t CRemoteDesktopPixelBuffer::ApplyPatch(CRemoteDesktopPixelBufferList *patch)
{
	ASSERT(patch);

	for(int i = 0; i < patch->GetLen(); i++)
	{
		CRemoteDesktopPixelBuffer *p = patch->GetElem(i);
		ASSERT(p);
		this->Combine(p);
	}
	return OK;
}


status_t CRemoteDesktopPixelBuffer::CompressData()
{	
	ASSERT(!IsGhost());
	ASSERT(!IsDataCompressed());
	
	uint8_t *tmpBuf = NULL;

	unsigned long bufSize = (m_pixel_data_size+12)*3/2;
	NEW_ARRAY(tmpBuf,uint8_t,bufSize);
	
	int r = compress2(tmpBuf,&bufSize,m_pixel_data,m_pixel_data_size,Z_BEST_COMPRESSION);
	ASSERT(r == Z_OK);

	DEL_ARRAY(m_pixel_data);
	m_pixel_data_size = 0;

	m_pixel_data = tmpBuf;
	m_pixel_data_size = bufSize;
		
	this->SetIsDataCompressed(true);
	return OK;
}

status_t CRemoteDesktopPixelBuffer::DecompressData()
{
	ASSERT(IsDataCompressed());
    unsigned long size = m_byte_per_pixel*m_width*m_height;
	uint8_t *p;
	NEW_ARRAY(p,uint8_t,size);
	unsigned long outSize = size;
	ASSERT(uncompress(p,&outSize,m_pixel_data,m_pixel_data_size) == Z_OK);
	ASSERT(outSize == size);
	DEL_ARRAY(m_pixel_data);
	m_pixel_data = p;
	m_pixel_data_size = size;
	this->SetIsDataCompressed(false);
	return OK;
}

status_t CRemoteDesktopPixelBuffer::Transfer(CRemoteDesktopPixelBuffer *from)
{	
	ASSERT(from);
	
	if(from->IsGhost())
	{
		return this->Ghost(from);
	}
	
	this->AllocPixelData(0);

	m_pixel_data = from->m_pixel_data;
	m_pixel_data_size = from->m_pixel_data_size;
	from->m_pixel_data = NULL;
	from->m_pixel_data_size = 0;
    this->m_byte_per_pixel = from->m_byte_per_pixel;
    this->m_width = from->m_width;
    this->m_height = from->m_height;
    this->m_offset_x = from->m_offset_x;
    this->m_offset_y = from->m_offset_y;
	this->m_flags = from->m_flags;
	return OK;
}

status_t CRemoteDesktopPixelBuffer::Ghost(CRemoteDesktopPixelBuffer *from)
{
	ASSERT(from);
	this->AllocPixelData(0); //free pixel data
	m_pixel_data = from->m_pixel_data;
	m_pixel_data_size = from->m_pixel_data_size;
    this->m_byte_per_pixel = from->m_byte_per_pixel;
    this->m_width = from->m_width;
    this->m_height = from->m_height;
    this->m_offset_x = from->m_offset_x;
    this->m_offset_y = from->m_offset_y;
	this->m_flags = from->m_flags;
	this->SetIsGhost(true);
	return OK;	
}


bool CRemoteDesktopPixelBuffer::IsSizeOrBppChanged(CRemoteDesktopPixelBuffer *pbuf)
{
	ASSERT(pbuf);
	if(pbuf->GetWidth() != this->GetWidth())
		return true;
	if(pbuf->GetHeight() != this->GetHeight())
		return true;
	if(pbuf->GetBytePerPixel() != this->GetBytePerPixel())
		return true;
	return false;
}

