#ifndef __OGG_PLAYER_H
#define __OGG_PLAYER_H

#include "wrap-ogg.h"
#include "wrap-theora.h"

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
    ogg_logical_stream_t* logical_stream;
    theora_comment        comment;
    theora_info           info;
    theora_state          state;
} theora_t;

typedef struct
{
    FILE*                     fd;
    ogg_sync_state            physical_stream;
    ogg_logical_stream_pool_t logical_pool;

    ogg_logical_stream_t*     skeleton_logical_stream;
    theora_t                  theora;
    ogg_logical_stream_t*     vorbis_logical_stream;
} ogg_player_t;

void
display();

void
player_init(ogg_player_t* player);

void
player_free(ogg_player_t* player);

#define PHYSICAL_STREAM_READ_SUCCESS    0
#define PHYSICAL_STREAM_END             1

void
ogg_read_block_into_physical_stream(ogg_player_t* player);

#define PAGE_PULL_SUCCESS   0
#define PAGE_ENDED          1

void
ogg_pull_page_from_physical_stream(ogg_player_t* player, ogg_page* page);

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

//void
//theora_init();

int
is_theora_packet(ogg_packet* packet);

void
theora_decode_header_packet(theora_t* theora, ogg_packet* packet);

void
theora_decode_identification_header_packet(theora_t* theora, ogg_packet* packet);

void
theora_decode_comment_header_packet(theora_t* theora, ogg_packet* packet);

void
theora_decode_setup_header_packet(theora_t* theora, ogg_packet* packet);

unsigned char*
theora_create_rgb_buffer(uint32_t width, uint32_t height);

void
theora_yuv_to_rgb(yuv_buffer* yuv_buffer, unsigned char* rgb_buffer);

#endif // __OGG_PLAYER_H
