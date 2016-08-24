#ifndef __OGG_PLAYER_H
#define __OGG_PLAYER_H

#include "wrap-ogg.h"

typedef struct
{
    ogg_stream_state state;
    int              serial_num;
} ogg_logical_stream_t;

typedef struct
{
    ogg_logical_stream_t* stream;
    unsigned int          size;
} ogg_logical_stream_pool_t;

typedef struct
{
    FILE*                     fd;
    ogg_sync_state            physical_stream;
    ogg_logical_stream_pool_t logical_pool;
    ogg_logical_stream_t*     skeleton_logical_stream;
    ogg_logical_stream_t*     theora_logical_stream;
    ogg_logical_stream_t*     vorbis_logical_stream;
} ogg_player_t;

//void
//ogg_init();

#define PHYSICAL_STREAM_READ_SUCCESS    0
#define PHYSICAL_STREAM_END             1

void
ogg_read_block_into_physical_stream(ogg_player_t* player);

#define PAGE_PULL_SUCCESS   0
#define PAGE_ENDED          1

void
ogg_pull_page_from_physical_stream(ogg_player_t* player,
                                   ogg_page*     page);

void
ogg_init_stream_in_logical_stream_pool(ogg_logical_stream_pool_t* pool,
                                       ogg_page*                  page,
                                       int                        page_serialno);

void
ogg_push_page_into_logical_stream_pool(ogg_logical_stream_pool_t* pool,
                                       ogg_page*                  page);

void
ogg_pull_packet_from_logical_stream(ogg_player_t*     player,
                                    ogg_stream_state* logical_stream,
                                    ogg_packet*       packet);

void
theora_init();

int
is_theora_packet(ogg_packet* packet);

void
theora_decode_header_packet();

void
theora_decode_identification_header_packet();

void
theora_decode_comment_header_packet();

void
theora_decode_setup_header_packet();

#endif // __OGG_PLAYER_H
