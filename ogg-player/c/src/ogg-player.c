#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vorbis/vorbisenc.h>

#include "trace.h"
#include "wrap-ogg.h"
#include "ogg-player.h"
#include "wrap-theora.h"
#include "ogg-skeleton.h"

#define PHYSICAL_STREAM_READ_BLOCK_SIZE     8192

FILE* fd = NULL;



ogg_sync_state   physical_stream_;

ogg_logical_stream_pool_t logical_pool_;

//ogg_logical_stream_t* logical_stream_pool_ = NULL;
//unsigned int          logical_stream_pool_size_ = 0;

ogg_stream_state theora_stream_state_;
//ogg_page         page_;
//ogg_packet       packet_;

theora_comment   theora_comment_;
theora_info      theora_info_;
theora_state     theora_state_;

yuv_buffer       yuv_frame_;

int
main(int argc, char** argv)
{
    if (argc != 2)
    {
        TRACE_INFO("usage: %s <file>", argv[0]);
        return EXIT_FAILURE;
    }

    ogg_player_t player;

    player.fd = fopen(argv[1], "r");

    ogg_init();
    ogg_sync_init(&player.physical_stream);

    theora_init();

    ogg_page     page;
    do
    {
        ogg_pull_page_from_physical_stream(&player, &page);
        ogg_push_page_into_logical_stream_pool(&logical_pool_, &page);
    }
    while (ogg_page_bos(&page));

    TRACE_INFO("********LOGICAL STREAMS*******");
    int i;
    for (i = 0; i < player.logical_pool.size; i++)
    {
        ogg_packet packet;
        ogg_pull_packet_from_logical_stream(&player, &player.logical_pool.stream[i].state, &packet);
        if (is_skeleton_packet(&packet))
            TRACE_INFO("index: %d serial: %d type: SKELETON", i, player.logical_pool.stream[i].serial_num);
        else if (theora_packet_isheader(&packet))
            TRACE_INFO("index: %d serial: %d type: THEORA", i, player.logical_pool.stream[i].serial_num);
        else if (vorbis_synthesis_idheader(&packet))
            TRACE_INFO("index: %d serial: %d type: VORBIS", i, player.logical_pool.stream[i].serial_num);
        else
            TRACE_INFO("index: %d serial: %d type: UNKNOWN", i, player.logical_pool.stream[i].serial_num);
    }
    TRACE_INFO("******************************");



//    theora_decode_identification_header_packet();
//    theora_decode_comment_header_packet();
//    theora_decode_setup_header_packet();

    fclose(player.fd);

    return EXIT_SUCCESS;
}


/*******
 * OGG *
 *******/


void
ogg_init()
{
    TRACE_DEBUG("");
//    memset(&physical_stream_, 0x00, sizeof(ogg_sync_state));
//    memset(&theora_stream_state_, 0x00, sizeof(ogg_stream_state));
//    memset(&page_, 0x00, sizeof(ogg_page));
//    memset(&packet_, 0x00, sizeof(ogg_packet));

//    ogg_sync_init(&physical_stream_);
}

void
ogg_read_block_into_physical_stream(ogg_player_t* player)
{
    char* buffer = w_ogg_sync_buffer(&player->physical_stream, PHYSICAL_STREAM_READ_BLOCK_SIZE);
    size_t bytes = fread(buffer, 1, PHYSICAL_STREAM_READ_BLOCK_SIZE, player->fd);
//    TRACE_DEBUG("read %d byte(s)", bytes);
    w_ogg_sync_wrote(&player->physical_stream, bytes);
}

void
ogg_pull_page_from_physical_stream(ogg_player_t* player,
                                   ogg_page*     page)
{
    while (w_ogg_sync_pageout(&player->physical_stream, page) != 1)
        ogg_read_block_into_physical_stream(player);

//    TRACE_DEBUG("page  body_len: %ld  header_len: %ld", page.body_len, page.header_len);
}

void
ogg_init_stream_in_logical_stream_pool(ogg_logical_stream_pool_t* pool,
                                       ogg_page*                  page,
                                       int                        page_serialno)
{
    int i = pool->size++;
    pool->stream = realloc(pool->stream, sizeof(ogg_logical_stream_t) * pool->size);
    pool->stream[i].serial_num = page_serialno;
    w_ogg_stream_init(&pool->stream[i].state, page_serialno);
    w_ogg_stream_pagein(&pool->stream[i].state, page);
}

void
ogg_push_page_into_logical_stream_pool(ogg_logical_stream_pool_t* pool,
                                       ogg_page*                  page)
{
    int serial_num = ogg_page_serialno(page);

    int i;
    for (i = 0; i < pool->size; i++)
        if (pool->stream[i].serial_num == serial_num)
        {
            w_ogg_stream_pagein(&pool->stream[i].state, page);
            return;
        }

    ogg_init_stream_in_logical_stream_pool(pool, page, serial_num);
}

void
ogg_pull_packet_from_logical_stream(ogg_player_t*     player,
                                    ogg_stream_state* logical_stream,
                                    ogg_packet*       packet)
{
    ogg_page page;
    while (w_ogg_stream_packetout(logical_stream, packet) != 1)
    {
        ogg_pull_page_from_physical_stream(player, &page);
        ogg_push_page_into_logical_stream_pool(&player->logical_pool ,&page);
    }
}


/**********
 * THEORA *
 **********/

void
theora_init()
{
    TRACE_DEBUG("");
    memset(&theora_info_, 0x00, sizeof(theora_info));
    memset(&theora_comment_, 0x00, sizeof(theora_comment));
    memset(&theora_state_, 0x00, sizeof(theora_state));
    memset(&yuv_frame_, 0x00, sizeof(yuv_buffer));

    theora_info_init(&theora_info_);
    theora_comment_init(&theora_comment_);
}

int
is_theora_packet(ogg_packet* packet)
{
    if (theora_packet_isheader(packet) == 1)
        return 1;
//    else if (theora_packet_iskeyframe(packet) == 1)
//        return 1;

    return 0;
}

void
theora_decode_header_packet()
{
    TRACE_DEBUG("");
//    while (1)
//    {

//    ogg_pull_packet_from_logical_stream();
//    int ret = w_theora_decode_header(&theora_info_, &theora_comment_, &packet_);
//    if (ret == 0)
//        return;

//    }
}

void
theora_decode_identification_header_packet()
{
    TRACE_DEBUG("");
//    ogg_init_logical_stream();
    theora_decode_header_packet();
}

void
theora_decode_comment_header_packet()
{
    TRACE_DEBUG("");
    theora_decode_header_packet();
}

void
theora_decode_setup_header_packet()
{
    TRACE_DEBUG("");
    theora_decode_header_packet();
}
