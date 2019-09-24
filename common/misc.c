// Misc.c: implementation of the CMisc class.
//
//////////////////////////////////////////////////////////////////////
#include "misc.h"
#include "syslog.h"
#include "time.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int32_t hex2dec_32(const char *shex)
{
    int32_t idec,i,mid;   
    int32_t len;    
    
    if(shex[0] == '0' && (shex[1] == 'x' || shex[1] == 'X'))
        shex += 2;

    while(shex[0] == '0')
        shex ++;

    len = crt_strlen(shex);
    mid = 0;   idec = 0;   
    
    for( i=0; i<len; i++ )   
    {   
        if( shex[i]>='0'&&shex[i]<='9' )   
            mid = shex[i]-'0';   
        else if( shex[i]>='a'&&shex[i]<='f' )   
            mid   =   shex[i]   -'a'   +10;   
        else if( shex[i]>='A'&&shex[i]<='F' )   
            mid = shex[i]   -'A'   +10;   
        else   
            return   0;   
        
        mid = mid << ((len-i-1)<<2);   
        idec  = idec | mid;         
    }   
    
    return idec;   
}

int wild_match(const char *wild,const char *string) 
{
    const char *cp = NULL, *mp = NULL;
    
    while ((*string) && (*wild != '*')) 
    {
        if ((*wild != *string) && (*wild != '?'))
        {
            return 0;
        }
        wild++;
        string++;
    }
    
    while (*string) 
    {
        if (*wild == '*') 
        {
            if (!*++wild) 
            {
                return 1;
            }
            mp = wild;
            cp = string+1;
        } 
        else if ((*wild == *string) || (*wild == '?')) 
        {
            wild++;
            string++;
        }
        else
        {
            wild = mp;
            string = cp++;
        }
    }
    
    while (*wild == '*')
    {
        wild++;
    }
    return !*wild;
}

uint64_t hex2dec_64(const char *shex)
{   
    uint64_t  idec,mid;
    int32_t i,len;
    
    while(shex[0] == '0')
        shex ++;

    len = crt_strlen( shex );   
    
    ASSERT(len <= 16);
    
    mid   =   0;   idec   =   0;   

    for(   i=0;i<len;i++   )   
    {   
        if(   shex[i]>='0'&&shex[i]<='9'   )   
            mid   =   shex[i]-'0';   
        else   if(   shex[i]>='a'&&shex[i]<='f'   )   
            mid   =   shex[i]   -'a'   +10;   
        else   if(   shex[i]>='A'&&shex[i]<='F'   )   
            mid   =   shex[i]   -'A'   +10;   
        else   
            return   0;   
        
        mid  = mid << ((len-i-1)<<2);   
        idec   = idec | mid;   
        
    }

    return   idec;   
}   


bool_t is_dec(const char *str)
{
    if(str[0] == 0)
        return FALSE;

    while(*str)
    {
        if(*str <'0' || *str > '9')
            return FALSE;
        str ++;
    }

    return TRUE;
}

bool_t is_hex(const char *str)
{
    if(str[0] == 0)
        return FALSE;

    if(str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
        str += 2;

    while(*str)
    {
        if(!((*str>='0' && *str <='9') || (*str >='a' && *str <= 'f') || (*str >='A' && *str <= 'F')))
            return FALSE;
        str ++;
    }

    return TRUE;
}

bool_t is_float(const char *str)
{    
    int32_t dot = 0;
    if(str[0] == 0)return FALSE;

    if(str[0] == '-' || str[0] =='+')
        str++;

    while(*str)
    {
        if(*str == '.')
            {str++; dot++;}
        if(*str <'0' || *str > '9')
            return FALSE;
        str++;
    }
    if(dot > 1) return FALSE;
    return TRUE;
}

int32_t find_shift(uint32_t size)
{
    int32_t i;
    uint32_t t = 1;
    for(i = 1; i < 32; i++)
    {
        t = t << 1;
        if(t == size)
            return i;
    }
    
    return -1;
}

uint32_t next_pot(uint32_t x)
{
    x = x - 1;
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >>16);
    return x + 1;
}

void generate_uuid(char *uuid)
{
    const char *chars = "0123456789abcdef";
    int i,r;
    
    crt_memset(uuid,0,64);

    uuid[8] = uuid[13] = uuid[18] = uuid[23] = '-';
    uuid[14] = '4'; // version 4
    
    for (i = 0; i < 36; i++)
    {
        if (!uuid[i])
        {
            r = (int)(((double)rand()/RAND_MAX) * 16);
            uuid[i] = chars[(i == 19) ? (r & 0x3) | 0x8 : r & 0xf];
        }
    }   
}

status_t reshuffle(int *array, int size, int min)
{
    int i,t,pos;
    ASSERT(array && size > 0);
    
    for(i = 0; i < size; i++)
        array[i] = min+i;

    for(i = 0; i < size; i++)
    {
        pos = rand()%(size-i);
        t = array[pos];
        array[pos] = array[size-i-1];
        array[size-i-1] = t;
    }

    return OK;
}

int32_t str2int_32(const char *str)
{
    ASSERT(str);
    if(is_dec(str))
        return atoi(str);
    if(is_hex(str))
        return hex2dec_32(str);
    return 0;
}

uint32_t ac_x31_hash_string(const char *s)
{
    uint32_t h = (uint32_t)*s;
    if (h) for (++s ; *s; ++s) h = (h << 5) - h + (int32_t)*s;
    return h;
}

//Bob Jenkins' 32 bit integer hash function
uint32_t bob_32bit_integer_hash(uint32_t a)
{
    a = (a+0x7ed55d16) + (a<<12);
    a = (a^0xc761c23c) ^ (a>>19);
    a = (a+0x165667b1) + (a<<5);
    a = (a+0xd3a2646c) ^ (a<<9);
    a = (a+0xfd7046c5) + (a<<3);
    a = (a^0xb55a4f09) ^ (a>>16); 
    return a;
}
