// xImageJPG.cpp: implementation of the CxImageJPG class.
//
//////////////////////////////////////////////////////////////////////
#include "ximagejpg.h"
#include "jpeg_morecfg.h"
#include "syslog.h"

#define eBufSize 4096

#define NUM_FORMATS 12

#define FMT_BYTE       1 
#define FMT_STRING     2
#define FMT_USHORT     3
#define FMT_ULONG      4
#define FMT_URATIONAL  5
#define FMT_SBYTE      6
#define FMT_UNDEFINED  7
#define FMT_SSHORT     8
#define FMT_SLONG      9
#define FMT_SRATIONAL 10
#define FMT_SINGLE    11
#define FMT_DOUBLE    12

/* Describes tag values */

#define TAG_EXIF_VERSION      0x9000
#define TAG_EXIF_OFFSET       0x8769
#define TAG_INTEROP_OFFSET    0xa005

#define TAG_MAKE              0x010F
#define TAG_MODEL             0x0110

#define TAG_ORIENTATION       0x0112
#define TAG_XRESOLUTION       0x011A
#define TAG_YRESOLUTION       0x011B
#define TAG_RESOLUTIONUNIT    0x0128

#define TAG_EXPOSURETIME      0x829A
#define TAG_FNUMBER           0x829D

#define TAG_SHUTTERSPEED      0x9201
#define TAG_APERTURE          0x9202
#define TAG_BRIGHTNESS        0x9203
#define TAG_MAXAPERTURE       0x9205
#define TAG_FOCALLENGTH       0x920A

#define TAG_DATETIME_ORIGINAL 0x9003
#define TAG_USERCOMMENT       0x9286

#define TAG_SUBJECT_DISTANCE  0x9206
#define TAG_FLASH             0x9209

#define TAG_FOCALPLANEXRES    0xa20E
#define TAG_FOCALPLANEYRES    0xa20F
#define TAG_FOCALPLANEUNITS   0xa210
#define TAG_EXIF_IMAGEWIDTH   0xA002
#define TAG_EXIF_IMAGELENGTH  0xA003

/* the following is added 05-jan-2001 vcs */
#define TAG_EXPOSURE_BIAS     0x9204
#define TAG_WHITEBALANCE      0x9208
#define TAG_METERING_MODE     0x9207
#define TAG_EXPOSURE_PROGRAM  0x8822
#define TAG_ISO_EQUIVALENT    0x8827
#define TAG_COMPRESSION_LEVEL 0x9102

#define TAG_THUMBNAIL_OFFSET  0x0201
#define TAG_THUMBNAIL_LENGTH  0x0202

struct jpg_error_mgr {
    struct jpeg_error_mgr pub;  /* "public" fields */
//  jmp_buf setjmp_buffer;      /* for return to caller */
    char* buffer;               /* error message <CSC>*/
};
typedef jpg_error_mgr *jpg_error_ptr;

static const int BytesPerFormat[] = {0,1,1,2,4,8,1,1,2,4,8,4,8};

static void ima_jpeg_error_exit (j_common_ptr cinfo)
{
//  jpg_error_ptr myerr = (jpg_error_ptr) cinfo->err;
//  myerr->pub.format_message (cinfo, myerr->buffer);
//  longjmp(myerr->setjmp_buffer, 1);
}
//////////////////////////////////////////////////////////////////////
// CxFileJpg
//////////////////////////////////////////////////////////////////////
CxFileJpg::CxFileJpg(CFileBase* pFile)
{
    m_pFile = pFile;
    
    init_destination = InitDestination;
    empty_output_buffer = EmptyOutputBuffer;
    term_destination = TermDestination;
    
    init_source = InitSource;
    fill_input_buffer = FillInputBuffer;
    skip_input_data = SkipInputData;
    resync_to_restart = jpeg_resync_to_restart; // use default method
    term_source = TermSource;
    next_input_byte = NULL; //* => next byte to read from buffer 
    bytes_in_buffer = 0;    //* # of bytes remaining in buffer 
    this->Init();   
}

CxFileJpg::~CxFileJpg()
{
    this->Destroy();
}

int CxFileJpg::Init()
{
    NEW_ARRAY(m_pBuffer,BYTE,eBufSize);
    return OK;
}
int CxFileJpg::Destroy()
{
    DEL_ARRAY(m_pBuffer);
    return OK;
}
void CxFileJpg::InitDestination(j_compress_ptr cinfo)
{
    CxFileJpg* pDest = (CxFileJpg*)cinfo->dest;
    pDest->next_output_byte = pDest->m_pBuffer;
    pDest->free_in_buffer = eBufSize;
}

boolean CxFileJpg::EmptyOutputBuffer(j_compress_ptr cinfo)
{
    CxFileJpg* pDest = (CxFileJpg*)cinfo->dest;

    if (pDest->m_pFile->Write(pDest->m_pBuffer,eBufSize)!=(size_t)eBufSize)
        ERREXIT(cinfo, JERR_FILE_WRITE);

    pDest->next_output_byte = pDest->m_pBuffer;
    pDest->free_in_buffer = eBufSize;
    return TRUE;
}

void CxFileJpg::TermDestination(j_compress_ptr cinfo)
{
    CxFileJpg* pDest = (CxFileJpg*)cinfo->dest;
    size_t datacount = eBufSize - pDest->free_in_buffer;
    /* Write any data remaining in the buffer */
    if (datacount > 0) {
        if (!pDest->m_pFile->Write(pDest->m_pBuffer,datacount))
            ERREXIT(cinfo, JERR_FILE_WRITE);
    }
    //pDest->m_pFile->Flush();
    /* Make sure we wrote the output file OK */
    //if (pDest->m_pFile->Error()) ERREXIT(cinfo, JERR_FILE_WRITE);

    return;
}

void CxFileJpg::InitSource(j_decompress_ptr cinfo)
{
    CxFileJpg* pSource = (CxFileJpg*)cinfo->src;
    pSource->m_bStartOfFile = TRUE;
}

boolean CxFileJpg::FillInputBuffer(j_decompress_ptr cinfo)
{
    size_t nbytes;
    CxFileJpg* pSource = (CxFileJpg*)cinfo->src;
    nbytes = pSource->m_pFile->Read(pSource->m_pBuffer,eBufSize);
    if (nbytes <= 0){
        if (pSource->m_bStartOfFile)    //* Treat empty input file as fatal error 
            ERREXIT(cinfo, JERR_INPUT_EMPTY);
        WARNMS(cinfo, JWRN_JPEG_EOF);
        // Insert a fake EOI marker 
        pSource->m_pBuffer[0] = (JOCTET) 0xFF;
        pSource->m_pBuffer[1] = (JOCTET) JPEG_EOI;
        nbytes = 2;
    }
    pSource->next_input_byte = pSource->m_pBuffer;
    pSource->bytes_in_buffer = nbytes;
    pSource->m_bStartOfFile = FALSE;
    return TRUE;
}

void CxFileJpg::SkipInputData(j_decompress_ptr cinfo, long num_bytes)
{
    CxFileJpg* pSource = (CxFileJpg*)cinfo->src;
    if (num_bytes > 0){
        while (num_bytes > (long)pSource->bytes_in_buffer){
            num_bytes -= (long)pSource->bytes_in_buffer;
            FillInputBuffer(cinfo);
            // note we assume that fill_input_buffer will never return FALSE,
            // so suspension need not be handled.
        }
        pSource->next_input_byte += (size_t) num_bytes;
        pSource->bytes_in_buffer -= (size_t) num_bytes;
    }
}

void CxFileJpg::TermSource(j_decompress_ptr /*cinfo*/)
{
    return;
}
//////////////////////////////////////////////////////////////////////
// CxExifInfo
//////////////////////////////////////////////////////////////////////
CxExifInfo::CxExifInfo(EXIFINFO* info)
{
    this->Init(info);
}

CxExifInfo::~CxExifInfo()
{   
    this->Destroy();
}

int CxExifInfo::Init(EXIFINFO* info)
{
    if (info) {
        m_exifinfo = info;
        freeinfo = FALSE;
    } else {
        NEW(m_exifinfo,EXIFINFO);
        memset(m_exifinfo,0,sizeof(EXIFINFO));
        freeinfo = TRUE;
    }

    m_szLastError[0]='\0';
    ExifImageWidth = MotorolaOrder = 0;
    SectionsRead=0;
    memset(&Sections, 0, MAX_SECTIONS * sizeof(Section_t));
    return OK;
}
int CxExifInfo::Destroy()
{
    for(int i=0;i<MAX_SECTIONS;i++) 
    {
        FREE(Sections[i].Data);
    }

    if (freeinfo)
    {
        DEL( m_exifinfo );
    }
    
    return OK;
}
BOOL CxExifInfo::DecodeExif(CFileBase * hFile, int nReadMode)
{
    int a;
    int HaveCom = FALSE;

    a = hFile->Getc();

    if (a != 0xff || hFile->Getc() != M_SOI){
        return FALSE;
    }

    for(;;){
        int itemlen;
        int marker = 0;
        int ll,lh, got;
        BYTE * Data;

        if (SectionsRead >= MAX_SECTIONS){
            strcpy(m_szLastError,"Too many sections in jpg file");
            return FALSE;
        }

        for (a=0;a<7;a++){
            marker = hFile->Getc();
            if (marker != 0xff) break;

            if (a >= 6){
                printf("too many padding bytes\n");
                return FALSE;
            }
        }

        if (marker == 0xff){
            // 0xff is legal padding, but if we get that many, something's wrong.
            strcpy(m_szLastError,"too many padding bytes!");
            return FALSE;
        }

        Sections[SectionsRead].Type = marker;

        // Read the length of the section.
        lh = hFile->Getc();
        ll = hFile->Getc();

        itemlen = (lh << 8) | ll;

        if (itemlen < 2){
            strcpy(m_szLastError,"invalid marker");
            return FALSE;
        }

        Sections[SectionsRead].Size = itemlen;
        
        MALLOC(Data,BYTE,itemlen);

        if (Data == NULL){
            strcpy(m_szLastError,"Could not allocate memory");
            return FALSE;
        }
        Sections[SectionsRead].Data = Data;

        // Store first two pre-read bytes.
        Data[0] = (BYTE)lh;
        Data[1] = (BYTE)ll;

        got = hFile->Read(Data+2, itemlen-2); // Read the whole section.
        if (got != itemlen-2){
            strcpy(m_szLastError,"Premature end of file?");
            return FALSE;
        }
        SectionsRead += 1;

        switch(marker){

            case M_SOS:   // stop before hitting compressed data 
                // If reading entire image is requested, read the rest of the data.
                if (nReadMode & EXIF_READ_IMAGE){
                    int cp, ep, size;
                    // Determine how much file is left.
                    cp = (int)hFile->GetOffset();
                    hFile->SeekEnd();
                    ep = (int)hFile->GetOffset();
                    hFile->Seek(cp);

                    size = ep-cp;
                    MALLOC(Data,BYTE,size);
                    if (Data == NULL){
                        strcpy(m_szLastError,"could not allocate data for entire image");
                        return FALSE;
                    }

                    got = hFile->Read(Data, size);
                    if (got != size){
                        strcpy(m_szLastError,"could not read the rest of the image");
                        return FALSE;
                    }

                    Sections[SectionsRead].Data = Data;
                    Sections[SectionsRead].Size = size;
                    Sections[SectionsRead].Type = PSEUDO_IMAGE_MARKER;
                    SectionsRead ++;
                }
                return TRUE;

            case M_EOI:   // in case it's a tables-only JPEG stream
                printf("No image in jpeg!\n");
                return FALSE;

            case M_COM: // Comment section
                if (HaveCom || ((nReadMode & EXIF_READ_EXIF) == 0)){
                    // Discard this section.
                    SectionsRead--;
                    FREE(Sections[SectionsRead].Data);
                }else{
                    process_COM(Data, itemlen);
                    HaveCom = TRUE;
                }
                break;

            case M_JFIF:
                // Regular jpegs always have this tag, exif images have the exif
                // marker instead, althogh ACDsee will write images with both markers.
                // this program will re-create this marker on absence of exif marker.
                // hence no need to keep the copy from the file.
                SectionsRead--;
                FREE(Sections[SectionsRead].Data);
                break;

            case M_EXIF:
                // Seen files from some 'U-lead' software with Vivitar scanner
                // that uses marker 31 for non exif stuff.  Thus make sure 
                // it says 'Exif' in the section before treating it as exif.
                if ((nReadMode & EXIF_READ_EXIF) && memcmp(Data+2, "Exif", 4) == 0){
                    m_exifinfo->IsExif = process_EXIF((BYTE *)Data+2, itemlen);
                }else{
                    // Discard this section.
                    SectionsRead--;
                    FREE(Sections[SectionsRead].Data);
                }
                break;

            case M_SOF0: 
            case M_SOF1: 
            case M_SOF2: 
            case M_SOF3: 
            case M_SOF5: 
            case M_SOF6: 
            case M_SOF7: 
            case M_SOF9: 
            case M_SOF10:
            case M_SOF11:
            case M_SOF13:
            case M_SOF14:
            case M_SOF15:
                process_SOFn(Data, marker);
                break;
            default:
                // Skip any other sections.
                //if (ShowTags) printf("Jpeg section marker 0x%02x size %d\n",marker, itemlen);
                break;
        }
    }
    return TRUE;
}
BOOL CxExifInfo::process_EXIF(unsigned char * CharBuf, unsigned int length)
{
    m_exifinfo->FlashUsed = 0; 
    /* If it's from a digicam, and it used flash, it says so. */
    m_exifinfo->Comments[0] = '\0';  /* Initial value - null string */

    ExifImageWidth = 0;

    {   /* Check the EXIF header component */
        static const unsigned char ExifHeader[] = "Exif\0\0";
        if (memcmp(CharBuf+0, ExifHeader,6)){
            strcpy(m_szLastError,"Incorrect Exif header");
            return FALSE;
        }
    }

    if (memcmp(CharBuf+6,"II",2) == 0){
        MotorolaOrder = 0;
    }else{
        if (memcmp(CharBuf+6,"MM",2) == 0){
            MotorolaOrder = 1;
        }else{
            strcpy(m_szLastError,"Invalid Exif alignment marker.");
            return FALSE;
        }
    }

    /* Check the next two values for correctness. */
    if (Get16u(CharBuf+8) != 0x2a){
        strcpy(m_szLastError,"Invalid Exif start (1)");
        return FALSE;
    }

    int FirstOffset = Get32u(CharBuf+10);
    /* <Richard Collins> 
    if (FirstOffset < 8 || FirstOffset > 16){
        // I used to ensure this was set to 8 (website I used indicated its 8)
        // but PENTAX Optio 230 has it set differently, and uses it as offset. (Sept 11 2002)
        strcpy(m_szLastError,"Suspicious offset of first IFD value");
        return FALSE;
    }*/

    unsigned char * LastExifRefd = CharBuf;

    /* First directory starts 16 bytes in.  Offsets start at 8 bytes in. */
    if (!ProcessExifDir(CharBuf+14, CharBuf+6, length-6, m_exifinfo, &LastExifRefd))
        return FALSE;

    /* <Richard Collins> give a chance for a second directory */
    if (FirstOffset > 8) {
        if (!ProcessExifDir(CharBuf+14+FirstOffset-8, CharBuf+6, length-6, m_exifinfo, &LastExifRefd))
            return FALSE;
    }

    /* This is how far the interesting (non thumbnail) part of the exif went. */
    // int ExifSettingsLength = LastExifRefd - CharBuf;

    /* Compute the CCD width, in milimeters. */
    if (m_exifinfo->FocalplaneXRes != 0){
        m_exifinfo->CCDWidth = (float)(ExifImageWidth * m_exifinfo->FocalplaneUnits / m_exifinfo->FocalplaneXRes);
    }

    return TRUE;
}
int CxExifInfo::Get16m(void * Short)
{
    return (((unsigned char *)Short)[0] << 8) | ((unsigned char *)Short)[1];
}

int CxExifInfo::Get16u(void * Short)
{
    if (MotorolaOrder){
        return (((unsigned char *)Short)[0] << 8) | ((unsigned char *)Short)[1];
    }else{
        return (((unsigned char *)Short)[1] << 8) | ((unsigned char *)Short)[0];
    }
}
long CxExifInfo::Get32s(void * Long)
{
    if (MotorolaOrder){
        return  ((( char *)Long)[0] << 24) | (((unsigned char *)Long)[1] << 16)
              | (((unsigned char *)Long)[2] << 8 ) | (((unsigned char *)Long)[3] << 0 );
    }else{
        return  ((( char *)Long)[3] << 24) | (((unsigned char *)Long)[2] << 16)
              | (((unsigned char *)Long)[1] << 8 ) | (((unsigned char *)Long)[0] << 0 );
    }
}

unsigned long CxExifInfo::Get32u(void * Long)
{
    return (unsigned long)Get32s(Long) & 0xffffffff;
}
BOOL CxExifInfo::ProcessExifDir(unsigned char * DirStart, unsigned char * OffsetBase, unsigned ExifLength,
                           EXIFINFO * const m_exifinfo, unsigned char ** const LastExifRefdP, int NestingLevel)
{
    int de;
    int a;
    int NumDirEntries;
    unsigned ThumbnailOffset = 0;
    unsigned ThumbnailSize = 0;

    if (NestingLevel > 4){
        strcpy(m_szLastError,"Maximum directory nesting exceeded (corrupt exif header)");
        return FALSE;
    }

    NumDirEntries = Get16u(DirStart);

    if ((DirStart+2+NumDirEntries*12) > (OffsetBase+ExifLength)){
        strcpy(m_szLastError,"Illegally sized directory");
        return FALSE;
    }

    for (de=0;de<NumDirEntries;de++){
        int Tag, Format, Components;
        unsigned char * ValuePtr;
            /* This actually can point to a variety of things; it must be
               cast to other types when used.  But we use it as a byte-by-byte
               cursor, so we declare it as a pointer to a generic byte here.
            */
        int ByteCount;
        unsigned char * DirEntry;
        DirEntry = DirStart+2+12*de;

        Tag = Get16u(DirEntry);
        Format = Get16u(DirEntry+2);
        Components = Get32u(DirEntry+4);

        if ((Format-1) >= NUM_FORMATS) {
            /* (-1) catches illegal zero case as unsigned underflows to positive large */
            strcpy(m_szLastError,"Illegal format code in EXIF dir");
            return FALSE;
        }

        ByteCount = Components * BytesPerFormat[Format];

        if (ByteCount > 4){
            unsigned OffsetVal;
            OffsetVal = Get32u(DirEntry+8);
            /* If its bigger than 4 bytes, the dir entry contains an offset.*/
            if (OffsetVal+ByteCount > ExifLength){
                /* Bogus pointer offset and / or bytecount value */
                strcpy(m_szLastError,"Illegal pointer offset value in EXIF.");
                return FALSE;
            }
            ValuePtr = OffsetBase+OffsetVal;
        }else{
            /* 4 bytes or less and value is in the dir entry itself */
            ValuePtr = DirEntry+8;
        }

        if (*LastExifRefdP < ValuePtr+ByteCount){
            /* Keep track of last byte in the exif header that was
               actually referenced.  That way, we know where the
               discardable thumbnail data begins.
            */
            *LastExifRefdP = ValuePtr+ByteCount;
        }

        /* Extract useful components of tag */
        switch(Tag){

            case TAG_MAKE:
                strncpy(m_exifinfo->CameraMake, (char*)ValuePtr, 31);
                break;

            case TAG_MODEL:
                strncpy(m_exifinfo->CameraModel, (char*)ValuePtr, 39);
                break;

            case TAG_EXIF_VERSION:
                strncpy(m_exifinfo->Version,(char*)ValuePtr, 4);
                break;

            case TAG_DATETIME_ORIGINAL:
                strncpy(m_exifinfo->DateTime, (char*)ValuePtr, 19);
                break;

            case TAG_USERCOMMENT:
                // Olympus has this padded with trailing spaces. Remove these first. 
                for (a=ByteCount;;){
                    a--;
                    if (((char*)ValuePtr)[a] == ' '){
                        ((char*)ValuePtr)[a] = '\0';
                    }else{
                        break;
                    }
                    if (a == 0) break;
                }

                /* Copy the comment */
                if (memcmp(ValuePtr, "ASCII",5) == 0){
                    for (a=5;a<10;a++){
                        char c;
                        c = ((char*)ValuePtr)[a];
                        if (c != '\0' && c != ' '){
                            strncpy(m_exifinfo->Comments, (char*)ValuePtr+a, 199);
                            break;
                        }
                    }
                    
                }else{
                    strncpy(m_exifinfo->Comments, (char*)ValuePtr, 199);
                }
                break;

            case TAG_FNUMBER:
                /* Simplest way of expressing aperture, so I trust it the most.
                   (overwrite previously computd value if there is one)
                   */
                m_exifinfo->ApertureFNumber = (float)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_APERTURE:
            case TAG_MAXAPERTURE:
                /* More relevant info always comes earlier, so only
                 use this field if we don't have appropriate aperture
                 information yet. 
                */
                if (m_exifinfo->ApertureFNumber == 0){
                    m_exifinfo->ApertureFNumber = (float)exp(ConvertAnyFormat(ValuePtr, Format)*log(2.0f)*0.5);
                }
                break;

            case TAG_BRIGHTNESS:
                m_exifinfo->Brightness = (float)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_FOCALLENGTH:
                /* Nice digital cameras actually save the focal length
                   as a function of how farthey are zoomed in. 
                */

                m_exifinfo->FocalLength = (float)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_SUBJECT_DISTANCE:
                /* Inidcates the distacne the autofocus camera is focused to.
                   Tends to be less accurate as distance increases.
                */
                m_exifinfo->Distance = (float)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_EXPOSURETIME:
                /* Simplest way of expressing exposure time, so I
                   trust it most.  (overwrite previously computd value
                   if there is one) 
                */
                m_exifinfo->ExposureTime = 
                    (float)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_SHUTTERSPEED:
                /* More complicated way of expressing exposure time,
                   so only use this value if we don't already have it
                   from somewhere else.  
                */
                if (m_exifinfo->ExposureTime == 0){
                    m_exifinfo->ExposureTime = (float)
                        (1/exp(ConvertAnyFormat(ValuePtr, Format)*log(2.0f)));
                }
                break;

            case TAG_FLASH:
                if ((int)ConvertAnyFormat(ValuePtr, Format) & 7){
                    m_exifinfo->FlashUsed = 1;
                }else{
                    m_exifinfo->FlashUsed = 0;
                }
                break;

            case TAG_ORIENTATION:
                m_exifinfo->Orientation = (int)ConvertAnyFormat(ValuePtr, Format);
                if (m_exifinfo->Orientation < 1 || m_exifinfo->Orientation > 8){
                    strcpy(m_szLastError,"Undefined rotation value");
                    m_exifinfo->Orientation = 0;
                }
                break;

            case TAG_EXIF_IMAGELENGTH:
            case TAG_EXIF_IMAGEWIDTH:
                /* Use largest of height and width to deal with images
                   that have been rotated to portrait format.  
                */
                a = (int)ConvertAnyFormat(ValuePtr, Format);
                if (ExifImageWidth < a) ExifImageWidth = a;
                break;

            case TAG_FOCALPLANEXRES:
                m_exifinfo->FocalplaneXRes = (float)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_FOCALPLANEYRES:
                m_exifinfo->FocalplaneYRes = (float)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_RESOLUTIONUNIT:
                switch((int)ConvertAnyFormat(ValuePtr, Format)){
                    case 1: m_exifinfo->ResolutionUnit = 1.0f; break; /* 1 inch */
                    case 2: m_exifinfo->ResolutionUnit = 1.0f; break;
                    case 3: m_exifinfo->ResolutionUnit = 0.3937007874f;    break;  /* 1 centimeter*/
                    case 4: m_exifinfo->ResolutionUnit = 0.03937007874f;   break;  /* 1 millimeter*/
                    case 5: m_exifinfo->ResolutionUnit = 0.00003937007874f;  /* 1 micrometer*/
                }
                break;

            case TAG_FOCALPLANEUNITS:
                switch((int)ConvertAnyFormat(ValuePtr, Format)){
                    case 1: m_exifinfo->FocalplaneUnits = 1.0f; break; /* 1 inch */
                    case 2: m_exifinfo->FocalplaneUnits = 1.0f; break;
                    case 3: m_exifinfo->FocalplaneUnits = 0.3937007874f;    break;  /* 1 centimeter*/
                    case 4: m_exifinfo->FocalplaneUnits = 0.03937007874f;   break;  /* 1 millimeter*/
                    case 5: m_exifinfo->FocalplaneUnits = 0.00003937007874f;  /* 1 micrometer*/
                }
                break;

                // Remaining cases contributed by: Volker C. Schoech <schoech(at)gmx(dot)de>

            case TAG_EXPOSURE_BIAS:
                m_exifinfo->ExposureBias = (float) ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_WHITEBALANCE:
                m_exifinfo->Whitebalance = (int)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_METERING_MODE:
                m_exifinfo->MeteringMode = (int)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_EXPOSURE_PROGRAM:
                m_exifinfo->ExposureProgram = (int)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_ISO_EQUIVALENT:
                m_exifinfo->ISOequivalent = (int)ConvertAnyFormat(ValuePtr, Format);
                if ( m_exifinfo->ISOequivalent < 50 ) m_exifinfo->ISOequivalent *= 200;
                break;

            case TAG_COMPRESSION_LEVEL:
                m_exifinfo->CompressionLevel = (int)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_XRESOLUTION:
                m_exifinfo->Xresolution = (float)ConvertAnyFormat(ValuePtr, Format);
                break;
            case TAG_YRESOLUTION:
                m_exifinfo->Yresolution = (float)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_THUMBNAIL_OFFSET:
                ThumbnailOffset = (unsigned)ConvertAnyFormat(ValuePtr, Format);
                break;

            case TAG_THUMBNAIL_LENGTH:
                ThumbnailSize = (unsigned)ConvertAnyFormat(ValuePtr, Format);
                break;

        }

        if (Tag == TAG_EXIF_OFFSET || Tag == TAG_INTEROP_OFFSET){
            unsigned char * SubdirStart;
            unsigned Offset = Get32u(ValuePtr);
            if (Offset>8){
                SubdirStart = OffsetBase + Offset;
                if (SubdirStart < OffsetBase || 
                    SubdirStart > OffsetBase+ExifLength){
                    strcpy(m_szLastError,"Illegal subdirectory link");
                    return FALSE;
                }
                ProcessExifDir(SubdirStart, OffsetBase, ExifLength, m_exifinfo, LastExifRefdP, NestingLevel+1);
            }
            continue;
        }
    }


    {
        /* In addition to linking to subdirectories via exif tags,
           there's also a potential link to another directory at the end
           of each directory.  This has got to be the result of a
           committee!  
        */
        unsigned char * SubdirStart;
        unsigned Offset;
        Offset = Get16u(DirStart+2+12*NumDirEntries);
        if (Offset){
            SubdirStart = OffsetBase + Offset;
            if (SubdirStart < OffsetBase 
                || SubdirStart > OffsetBase+ExifLength){
                strcpy(m_szLastError,"Illegal subdirectory link");
                return FALSE;
            }
            ProcessExifDir(SubdirStart, OffsetBase, ExifLength, m_exifinfo, LastExifRefdP, NestingLevel+1);
        }
    }


    if (ThumbnailSize && ThumbnailOffset){
        if (ThumbnailSize + ThumbnailOffset <= ExifLength){
            /* The thumbnail pointer appears to be valid.  Store it. */
            m_exifinfo->ThumbnailPointer = OffsetBase + ThumbnailOffset;
            m_exifinfo->ThumbnailSize = ThumbnailSize;
        }
    }

    return TRUE;
}
double CxExifInfo::ConvertAnyFormat(void * ValuePtr, int Format)
{
    double Value;
    Value = 0;

    switch(Format){
        case FMT_SBYTE:     Value = *(signed char *)ValuePtr;  break;
        case FMT_BYTE:      Value = *(unsigned char *)ValuePtr;        break;

        case FMT_USHORT:    Value = Get16u(ValuePtr);          break;
        case FMT_ULONG:     Value = Get32u(ValuePtr);          break;

        case FMT_URATIONAL:
        case FMT_SRATIONAL: 
            {
                int Num,Den;
                Num = Get32s(ValuePtr);
                Den = Get32s(4+(char *)ValuePtr);
                if (Den == 0){
                    Value = 0;
                }else{
                    Value = (double)Num/Den;
                }
                break;
            }

        case FMT_SSHORT:    Value = (signed short)Get16u(ValuePtr);  break;
        case FMT_SLONG:     Value = Get32s(ValuePtr);                break;

        /* Not sure if this is correct (never seen float used in Exif format)
         */
        case FMT_SINGLE:    Value = (double)*(float *)ValuePtr;      break;
        case FMT_DOUBLE:    Value = *(double *)ValuePtr;             break;
    }
    return Value;
}
void CxExifInfo::process_COM (const BYTE * Data, int length)
{
    int ch;
    char Comment[MAX_COMMENT+1];
    int nch;
    int a;

    nch = 0;

    if (length > MAX_COMMENT) length = MAX_COMMENT; // Truncate if it won't fit in our structure.

    for (a=2;a<length;a++){
        ch = Data[a];

        if (ch == '\r' && Data[a+1] == '\n') continue; // Remove cr followed by lf.

        if (isprint(ch) || ch == '\n' || ch == '\t'){
            Comment[nch++] = (char)ch;
        }else{
            Comment[nch++] = '?';
        }
    }

    Comment[nch] = '\0'; // Null terminate

    //if (ShowTags) printf("COM marker comment: %s\n",Comment);

    strcpy(m_exifinfo->Comments,Comment);
}
void CxExifInfo::process_SOFn (const BYTE * Data, int marker)
{
    int data_precision, num_components;

    data_precision = Data[2];
    m_exifinfo->Height = Get16m((void*)(Data+3));
    m_exifinfo->Width = Get16m((void*)(Data+5));
    num_components = Data[7];

    if (num_components == 3){
        m_exifinfo->IsColor = 1;
    }else{
        m_exifinfo->IsColor = 0;
    }

    m_exifinfo->Process = marker;

    //if (ShowTags) printf("JPEG image is %uw * %uh, %d color components, %d bits per sample\n",
    //               ImageInfo.Width, ImageInfo.Height, num_components, data_precision);
}
BOOL CxExifInfo::EncodeExif(CFileBase * hFile)
{
    int a;

    if (FindSection(M_SOS)==NULL){
        strcpy(m_szLastError,"Can't write exif : didn't read all");
        return FALSE;
    }

    // Initial static jpeg marker.
    hFile->Putc((char)0xff);
    hFile->Putc((char)0xd8);
    
    if (Sections[0].Type != M_EXIF && Sections[0].Type != M_JFIF){
        // The image must start with an exif or jfif marker.  If we threw those away, create one.
        static BYTE JfifHead[18] = {
            0xff, M_JFIF,
            0x00, 0x10, 'J' , 'F' , 'I' , 'F' , 0x00, 0x01, 
            0x01, 0x01, 0x01, 0x2C, 0x01, 0x2C, 0x00, 0x00 
        };
        hFile->Write(JfifHead, 18);
    }

    // Write all the misc sections
    for (a=0;a<SectionsRead-1;a++){
        hFile->Putc((char)0xff);
        hFile->Putc((char)(Sections[a].Type));
        hFile->Write(Sections[a].Data, Sections[a].Size);
    }

    // Write the remaining image data.
    hFile->Write(Sections[a].Data, Sections[a].Size);

    return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
void CxExifInfo::DiscardAllButExif()
{
    Section_t ExifKeeper;
    Section_t CommentKeeper;
    int a;

    memset(&ExifKeeper, 0, sizeof(ExifKeeper));
    memset(&CommentKeeper, 0, sizeof(ExifKeeper));

    for (a=0;a<SectionsRead;a++){
        if (Sections[a].Type == M_EXIF && ExifKeeper.Type == 0){
            ExifKeeper = Sections[a];
        }else if (Sections[a].Type == M_COM && CommentKeeper.Type == 0){
            CommentKeeper = Sections[a];
        }else{
            FREE(Sections[a].Data);
            Sections[a].Data = 0;
        }
    }
    SectionsRead = 0;
    if (ExifKeeper.Type){
        Sections[SectionsRead++] = ExifKeeper;
    }
    if (CommentKeeper.Type){
        Sections[SectionsRead++] = CommentKeeper;
    }
}

void* CxExifInfo::FindSection(int SectionType)
{
    int a;
    for (a=0;a<SectionsRead-1;a++){
        if (Sections[a].Type == SectionType){
            return &Sections[a];
        }
    }
    // Could not be found.
    return NULL;
}
//////////////////////////////////////////////////////////////////////
// CxImageJPG
//////////////////////////////////////////////////////////////////////
CxImageJPG::CxImageJPG()
{
    this->StartUp(CXIMAGE_FORMAT_JPG);
    m_exif = NULL;
    memset(&m_exifinfo, 0, sizeof(EXIFINFO));
}

CxImageJPG::~CxImageJPG()
{
    CxImage::Destroy();
    this->Destroy();
}

int CxImageJPG::Destroy()
{
    DEL(m_exif);
    return OK;
}

BOOL CxImageJPG::DecodeExif(CFileBase *hFile)
{
    ASSERT(hFile);
    
    NEW(m_exif,CxExifInfo(&m_exifinfo));

    if (m_exif){
        fsize_t pos=hFile->GetOffset();
        m_exif->DecodeExif(hFile);
        hFile->Seek(pos);
        return m_exif->m_exifinfo->IsExif;
    } else {
        return FALSE;
    }
}

#if CXIMAGE_SUPPORT_DECODE
BOOL CxImageJPG::Decode(CFileBase * hFile)
{
    ASSERT(hFile);
    
    hFile->Seek(0);

    BOOL is_exif = FALSE;

#if CXIMAGEJPG_SUPPORT_EXIF
    is_exif = DecodeExif(hFile);
#endif

    CImageIterator iter(this);
    struct jpeg_decompress_struct cinfo;
    
    struct jpg_error_mgr jerr;
    jerr.buffer=info.szLastError;

    JSAMPARRAY buffer;  
    int row_stride;     

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = ima_jpeg_error_exit;

    jpeg_create_decompress(&cinfo);

    CxFileJpg src(hFile);
    cinfo.src = &src;
    
    jpeg_read_header(&cinfo, TRUE);

    if ((GetCodecOption(CXIMAGE_FORMAT_JPG) & DECODE_GRAYSCALE) != 0)
        cinfo.out_color_space = JCS_GRAYSCALE;
    if ((GetCodecOption(CXIMAGE_FORMAT_JPG) & DECODE_QUANTIZE) != 0) {
        cinfo.quantize_colors = TRUE;
        cinfo.desired_number_of_colors = GetJpegQuality();
    }
    if ((GetCodecOption(CXIMAGE_FORMAT_JPG) & DECODE_DITHER) != 0)
        cinfo.dither_mode = m_nDither;
    if ((GetCodecOption(CXIMAGE_FORMAT_JPG) & DECODE_ONEPASS) != 0)
        cinfo.two_pass_quantize = FALSE;
    if ((GetCodecOption(CXIMAGE_FORMAT_JPG) & DECODE_NOSMOOTH) != 0)
        cinfo.do_fancy_upsampling = FALSE;
    cinfo.scale_denom = GetJpegScale();
    if (info.nEscape == -1) {
        jpeg_calc_output_dimensions(&cinfo);
        head.biWidth = cinfo.output_width;
        head.biHeight = cinfo.output_height;
        info.dwType = CXIMAGE_FORMAT_JPG;
        jpeg_destroy_decompress(&cinfo);
        return TRUE;
    }
    jpeg_start_decompress(&cinfo);
    Create(cinfo.output_width, cinfo.output_height, 8*cinfo.output_components, CXIMAGE_FORMAT_JPG);

    ASSERT(pDib);

    if (is_exif){
#if CXIMAGEJPG_SUPPORT_EXIF
    if ((m_exifinfo.Xresolution != 0.0) && (m_exifinfo.ResolutionUnit != 0))
        SetXDPI((long)(m_exifinfo.Xresolution/m_exifinfo.ResolutionUnit));
    if ((m_exifinfo.Yresolution != 0.0) && (m_exifinfo.ResolutionUnit != 0))
        SetYDPI((long)(m_exifinfo.Yresolution/m_exifinfo.ResolutionUnit));
#endif
    } else {
        switch (cinfo.density_unit) {
        case 0: 
            if((cinfo.Y_density > 0) && (cinfo.X_density > 0)){
                SetYDPI((long)(GetXDPI()*(float(cinfo.Y_density)/float(cinfo.X_density))));
            }
            break;
        case 2: 
            SetXDPI((long)floor(cinfo.X_density * 2.54 + 0.5));
            SetYDPI((long)floor(cinfo.Y_density * 2.54 + 0.5));
            break;
        default:
            SetXDPI(cinfo.X_density);
            SetYDPI(cinfo.Y_density);
        }
    }
    if (cinfo.out_color_space==JCS_GRAYSCALE){
        SetGrayPalette();
        head.biClrUsed =256;
    } else {
        if (cinfo.quantize_colors){
            SetPalette(cinfo.actual_number_of_colors, cinfo.colormap[0], cinfo.colormap[1], cinfo.colormap[2]);
            head.biClrUsed=cinfo.actual_number_of_colors;
        } else {
            head.biClrUsed=0;
        }
    }
    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)
        ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
    iter.Upset();
    while (cinfo.output_scanline < cinfo.output_height) {
        ASSERT(!(info.nEscape));
        
        (void) jpeg_read_scanlines(&cinfo, buffer, 1);
        if ((cinfo.num_components==4)&&(cinfo.quantize_colors==FALSE)){
            BYTE k,*dst,*src;
            dst=iter.GetRow();
            src=buffer[0];
            for(long x3=0,x4=0; x3<(long)info.dwEffWidth && x4<row_stride; x3+=3, x4+=4){
                k=src[x4+3];
                dst[x3]  =(BYTE)((k * src[x4+2])/255);
                dst[x3+1]=(BYTE)((k * src[x4+1])/255);
                dst[x3+2]=(BYTE)((k * src[x4+0])/255);
            }
        } else {
            iter.SetRow(buffer[0], row_stride);
        }
            iter.PrevRow();
    }
    (void) jpeg_finish_decompress(&cinfo);
    if ((cinfo.num_components==3)&&(cinfo.quantize_colors==FALSE)){
        BYTE* r0=GetBits(0);
        for(long y=0;y<head.biHeight;y++){
            ASSERT(!(info.nEscape));
            RGBtoBGR(r0,3*head.biWidth);
            r0+=info.dwEffWidth;
        }
    }
    jpeg_destroy_decompress(&cinfo);
    return TRUE;
}
#endif  //CXIMAGE_SUPPORT_DECODE

#if CXIMAGE_SUPPORT_ENCODE
BOOL CxImageJPG::Encode(CFileBase * hFile)
{
    ASSERT(hFile);
    hFile->SetSize(0);

    if (EncodeSafeCheck(hFile)) return FALSE;

    if (head.biClrUsed!=0 && !IsGrayScale())
    {
        strcpy(info.szLastError,"JPEG can save only RGB or GreyScale images");
        return FALSE;
    }   
    fsize_t pos=hFile->GetOffset();
    struct jpeg_compress_struct cinfo;
    struct jpg_error_mgr jerr;
    jerr.buffer=info.szLastError;
    int row_stride;     
    JSAMPARRAY buffer;      
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = ima_jpeg_error_exit;

    jpeg_create_compress(&cinfo);
    CxFileJpg dest(hFile);
    cinfo.dest = &dest;
    cinfo.image_width = GetWidth();     
    cinfo.image_height = GetHeight();
    if (IsGrayScale()){
        cinfo.input_components = 1;         
        cinfo.in_color_space = JCS_GRAYSCALE; 
    } else {
        cinfo.input_components = 3;     
        cinfo.in_color_space = JCS_RGB; 
    }
    jpeg_set_defaults(&cinfo);
    if ((GetCodecOption(CXIMAGE_FORMAT_JPG) & ENCODE_ARITHMETIC) != 0)
        cinfo.arith_code = TRUE;
    if ((GetCodecOption(CXIMAGE_FORMAT_JPG) & ENCODE_OPTIMIZE) != 0)
        cinfo.optimize_coding = TRUE;
    if ((GetCodecOption(CXIMAGE_FORMAT_JPG) & ENCODE_GRAYSCALE) != 0)
        jpeg_set_colorspace(&cinfo, JCS_GRAYSCALE);
    if ((GetCodecOption(CXIMAGE_FORMAT_JPG) & ENCODE_SMOOTHING) != 0)
        cinfo.smoothing_factor = m_nSmoothing;
    jpeg_set_quality(&cinfo, GetJpegQuality(), (GetCodecOption(CXIMAGE_FORMAT_JPG) & ENCODE_BASELINE) != 0);
    if ((GetCodecOption(CXIMAGE_FORMAT_JPG) & ENCODE_PROGRESSIVE) != 0)
        jpeg_simple_progression(&cinfo);
#ifdef C_LOSSLESS_SUPPORTED
    if ((GetCodecOption(CXIMAGE_FORMAT_JPG) & ENCODE_LOSSLESS) != 0)
        jpeg_simple_lossless(&cinfo, m_nPredictor, m_nPointTransform);
#endif
        cinfo.comp_info[0].h_samp_factor = 2;
        cinfo.comp_info[0].v_samp_factor = 2;
        cinfo.comp_info[1].h_samp_factor = 1;
        cinfo.comp_info[1].v_samp_factor = 1;
        cinfo.comp_info[2].h_samp_factor = 1;
        cinfo.comp_info[2].v_samp_factor = 1;
    if ((GetCodecOption(CXIMAGE_FORMAT_JPG) & ENCODE_SUBSAMPLE_422) != 0){
        cinfo.comp_info[0].h_samp_factor = 2;
        cinfo.comp_info[0].v_samp_factor = 1;
        cinfo.comp_info[1].h_samp_factor = 1;
        cinfo.comp_info[1].v_samp_factor = 1;
        cinfo.comp_info[2].h_samp_factor = 1;
        cinfo.comp_info[2].v_samp_factor = 1;
    }
    if ((GetCodecOption(CXIMAGE_FORMAT_JPG) & ENCODE_SUBSAMPLE_444) != 0){
        cinfo.comp_info[0].h_samp_factor = 1;
        cinfo.comp_info[0].v_samp_factor = 1;
        cinfo.comp_info[1].h_samp_factor = 1;
        cinfo.comp_info[1].v_samp_factor = 1;
        cinfo.comp_info[2].h_samp_factor = 1;
        cinfo.comp_info[2].v_samp_factor = 1;
    }
    cinfo.density_unit=1;
    cinfo.X_density=(unsigned short)GetXDPI();
    cinfo.Y_density=(unsigned short)GetYDPI();
    jpeg_start_compress(&cinfo, TRUE);
    row_stride = info.dwEffWidth;   
    buffer = (*cinfo.mem->alloc_sarray)
        ((j_common_ptr) &cinfo, JPOOL_IMAGE, 8+row_stride, 1);
    CImageIterator iter(this);
    iter.Upset();
    while (cinfo.next_scanline < cinfo.image_height) {
        iter.GetRow(buffer[0], row_stride);
        if (head.biClrUsed==0){              
            RGBtoBGR(buffer[0], row_stride); 
        }
        iter.PrevRow();
        (void) jpeg_write_scanlines(&cinfo, buffer, 1);
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
#if CXIMAGEJPG_SUPPORT_EXIF
    if (m_exif && m_exif->m_exifinfo->IsExif){
        m_exif->DiscardAllButExif();
        hFile->Seek(pos);
        m_exif->DecodeExif(hFile,EXIF_READ_IMAGE);
        hFile->Seek(pos);
        m_exif->EncodeExif(hFile);
    }
#endif
    return TRUE;
}
#endif 
