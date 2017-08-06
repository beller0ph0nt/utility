#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <trace.h>
#include <proto.h>
#include <common-lib.h>

void read_string(int sock, string_t* string)
{
    TRACE_DEBUG("read string");
    read(sock, &string->len, sizeof(int32_t));
    string->str = malloc(string->len);
    read(sock, string->str, string->len);
    TRACE_DEBUG("len: %d str: %s", string->len, string->str);
}

void fill_string(string_t* string, const char* s)
{
    TRACE_DEBUG("fill string");
    string->len = strlen(s) + 1;
    string->str = malloc(string->len);
    memcpy(string->str, s, string->len);
}

void clean_string(string_t* string)
{
    TRACE_DEBUG("clean string");
    free(string->str);
    string->len = 0;
}