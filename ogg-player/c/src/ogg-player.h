#ifndef __OGG_PLAYER_H
#define __OGG_PLAYER_H

#include "wrap-ogg.h"

typedef struct
{
    ogg_stream_state stream;
    int              serial_num;
} logical_stream_t;

void
ogg_init();

void
ogg_read_block_into_physical_stream();

void
ogg_pull_page_from_physical_stream();

void
ogg_init_logical_stream();

void
ogg_push_page_into_logical_stream();

void
ogg_pull_packet_from_logical_stream();

void
theora_init();

void
theora_decode_header_packet();

void
theora_decode_identification_header_packet();

void
theora_decode_comment_header_packet();

void
theora_decode_setup_header_packet();

#endif // __OGG_PLAYER_H
