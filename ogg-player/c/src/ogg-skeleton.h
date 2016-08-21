#ifndef __OGG_SKELETON_H
#define __OGG_SKELETON_H

#include "wrap-ogg.h"

#define SKELETON_1ST_HEADER_IDENT   "fishead"
#define SKELETON_2ND_HEADER_IDENT   "fisbone"

typedef struct
{
    uint16_t major;
    uint16_t minor;
} skeleton_ver_t;

typedef struct
{
    uint64_t numerator;
    uint64_t denomenator;
} skeleton_time_t;

typedef struct
{
    char            ident[sizeof(SKELETON_1ST_HEADER_IDENT)];
    skeleton_ver_t  ver;
    skeleton_time_t presentation_time;
    skeleton_time_t base_time;
    char            utc[20];
} skeleton_head_t;

int
is_skeleton_packet(ogg_packet* packet);

int
is_1st_skeletoh_packet(ogg_packet* packet);

int
is_2nd_skeleton_packet(ogg_packet* packet);

#endif // __OGG_SKELETON_H
