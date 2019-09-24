// Misc.h: interface for the CMisc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MISC_H__886ED6AD_5EEC_4593_8333_9EB77699295F__INCLUDED_)
#define AFX_MISC_H__886ED6AD_5EEC_4593_8333_9EB77699295F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __cplusplus
extern "C" {
#endif

#include "cruntime.h"

int32_t str2int_32(const char *str);
bool_t is_float(const char *str);
uint32_t next_pot(uint32_t x);
int32_t find_shift(uint32_t size);
bool_t is_dec(const char *str);
bool_t is_hex(const char *str);
uint64_t hex2dec_64(const char *shex);
int32_t hex2dec_32(const char *shex);
int wild_match(const char *wild, const char *string);
void generate_uuid(char *uuid); 
status_t reshuffle(int *array, int size, int min);
uint32_t ac_x31_hash_string(const char *s);
uint32_t bob_32bit_integer_hash(uint32_t a);

#ifdef __cplusplus
}
#endif

#endif // !defined(AFX_MISC_H__886ED6AD_5EEC_4593_8333_9EB77699295F__INCLUDED_)
