#ifndef __XIMAGEHELPER_H
#define __XIMAGEHELPER_H

#include "ximage.h"
#include "mem.h"
#include "rect.h"

class CxImageHelper{
public:
    static float CalcuSimilarity(CxImage *img1, CxImage *img2, int threshold, CxImage *diff_img);
    static status_t Convert9Png(CxImage *nineImg, CxImage *outImg, int *info, int *info_size);

#if CXIMAGE_SUPPORT_WINDOWS
    static status_t DrawString(CxImage *img, LOGFONTW *log_font, CRect *layout,COLORREF color, const wchar_t *text);
    static status_t MeasureTextSize(const wchar_t *text, LOGFONTW *log_font, int *w, int *h);
#endif

    static status_t ImageToRawRgba(CxImage *img, CMem *rgba);
    static status_t SaveImage(CxImage *img, const char *filename);
    static status_t SaveImage(CxImage *img, const char *type,CFileBase *dst);
    static status_t LoadImage(const char *filename, CxImage *out);
    static status_t LoadImage(CFileBase *file, CxImage *out);
};

#endif
