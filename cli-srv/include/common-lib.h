#ifndef __COMMON_LIB_H
#define __COMMON_LIB_H

#include <proto.h>

void read_string(int sock, string_t* string);
void fill_string(string_t* string, const char* s);
void clean_string(string_t* string);

#endif // __COMMON_LIB_H
