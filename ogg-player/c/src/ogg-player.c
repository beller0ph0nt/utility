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

int
main(int argc, char** argv)
{
    if (argc != 2)
    {
        TRACE_INFO("usage: %s <file>", argv[0]);
        return EXIT_FAILURE;
    }

    ogg_player_t player;
    player_init(&player);

    player.fd = fopen(argv[1], "r");

    ogg_page   page;

    do
    {
        ogg_pull_page_from_physical_stream(&player, &page);
        ogg_push_page_into_logical_stream_pool(&player.logical_pool, &page);
    }
    while (ogg_page_bos(&page));

    ogg_packet packet;

    TRACE_INFO("********LOGICAL STREAMS*******");
    int i;
    for (i = 0; i < player.logical_pool.size; i++)
    {
        ogg_pull_packet_from_logical_stream(&player,
                                            &player.logical_pool.stream[i].state,
                                            &packet);
        if (is_skeleton_packet(&packet))
        {
            TRACE_INFO("index: %d serial: %d type: SKELETON",
                       i,
                       player.logical_pool.stream[i].serial_num);
            player.skeleton_logical_stream = &player.logical_pool.stream[i];
        }
        else if (theora_packet_isheader(&packet))
        {
            TRACE_INFO("index: %d serial: %d type: THEORA",
                       i,
                       player.logical_pool.stream[i].serial_num);
            player.theora.logical_stream = &player.logical_pool.stream[i];
            theora_decode_identification_header_packet(&player.theora, &packet);
        }
        else if (vorbis_synthesis_idheader(&packet))
        {
            TRACE_INFO("index: %d serial: %d type: VORBIS",
                       i,
                       player.logical_pool.stream[i].serial_num);
            player.vorbis_logical_stream = &player.logical_pool.stream[i];
        }
        else
            TRACE_INFO("index: %d serial: %d type: UNKNOWN",
                       i,
                       player.logical_pool.stream[i].serial_num);
    }
    TRACE_INFO("******************************");

    ogg_pull_packet_from_logical_stream(&player,
                                        &player.theora.logical_stream->state,
                                        &packet);
    theora_decode_comment_header_packet(&player.theora, &packet);

    ogg_pull_packet_from_logical_stream(&player,
                                        &player.theora.logical_stream->state,
                                        &packet);
    theora_decode_setup_header_packet(&player.theora, &packet);


    yuv_buffer yuv;
//    float* rgb_buffer = theora_create_rgb_buffer(player.theora.info.frame_width,
//                                                 player.theora.info.frame_height);
    while (!feof(player.fd))
    {
        ogg_pull_packet_from_logical_stream(&player,
                                            &player.theora.logical_stream->state,
                                            &packet);
        theora_decode_packetin(&player.theora.state, &packet);
        theora_decode_YUVout(&player.theora.state, &yuv);

        TRACE_INFO("y   width:  %d  height: %d  stride: %d"
                   "    "
                   "yu  width:  %d  height: %d  stride: %d",
                   yuv.y_width,
                   yuv.y_height,
                   yuv.y_stride,
                   yuv.uv_width,
                   yuv.uv_height,
                   yuv.uv_stride);

        break;
//        theora_yuv_to_rgb(&yuv, rgb_buffer);
    }



    fclose(player.fd);

    player_free(&player);

    return EXIT_SUCCESS;
}

/**********
 * PLAYER *
 **********/

void
player_init(ogg_player_t* player)
{
    memset(player, 0, sizeof(ogg_player_t));

    ogg_sync_init(&player->physical_stream);

    theora_info_init(&player->theora.info);
    theora_comment_init(&player->theora.comment);
}

void
player_free(ogg_player_t* player)
{
    theora_comment_clear(&player->theora.comment);
    theora_info_clear(&player->theora.info);

    int i;
    for (i = 0; i < player->logical_pool.size; i++)
        ogg_stream_clear(&player->logical_pool.stream[i].state);

    free(player->logical_pool.stream);

	ogg_sync_clear(&player->physical_stream);
}

/*******
 * OGG *
 *******/

void
ogg_read_block_into_physical_stream(ogg_player_t* player)
{
    char* buffer = w_ogg_sync_buffer(&player->physical_stream,
                                     PHYSICAL_STREAM_READ_BLOCK_SIZE);
    size_t bytes = fread(buffer, 1, PHYSICAL_STREAM_READ_BLOCK_SIZE, player->fd);
//    TRACE_DEBUG("read %d byte(s)", bytes);
    w_ogg_sync_wrote(&player->physical_stream, bytes);
}

void
ogg_pull_page_from_physical_stream(ogg_player_t* player, ogg_page* page)
{
    while (w_ogg_sync_pageout(&player->physical_stream, page) != 1)
        ogg_read_block_into_physical_stream(player);

//    TRACE_DEBUG("page  body_len: %ld  header_len: %ld",
//                page->body_len,
//                page->header_len);
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
ogg_push_page_into_logical_stream_pool(ogg_logical_stream_pool_t* pool, ogg_page* page)
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
theora_decode_header_packet(theora_t* theora, ogg_packet* packet)
{
    w_theora_decode_header(&theora->info, &theora->comment, packet);
}

void
theora_decode_identification_header_packet(theora_t* theora, ogg_packet* packet)
{
    TRACE_DEBUG("");
    theora_decode_header_packet(theora, packet);
}

void
theora_decode_comment_header_packet(theora_t* theora, ogg_packet* packet)
{
    TRACE_DEBUG("");
    theora_decode_header_packet(theora, packet);
}

void
theora_decode_setup_header_packet(theora_t* theora, ogg_packet* packet)
{
    TRACE_DEBUG("");
    theora_decode_header_packet(theora, packet);
}

float*
theora_create_rgb_buffer(uint32_t width, uint32_t height)
{
    float* buffer = calloc(width * height, 3);

    return buffer;
}

void
theora_yuv_to_rgb(yuv_buffer* yuv_buffer, float* rgb_buffer)
{
//    int y;
//    for (y = 0; y < yuv_buffer.y_height; y++)
//    {
//        int y_row_offset = yuv_buffer.y_stride * y;
//        int uv_row_offset = (yuv_buffer.uv_stride / 2) * y;
//
//        int x;
//        for (x = 0; x < yuv_buffer.y_width; x++)
//        {
//            int nHX = x / 2;
//
////            BYTE nY = *(BYTE*) (yuv_buffer.y + nYShift + x);
////            BYTE nU = *(BYTE*) (yuv_buffer.u + nUVShift + nHX);
////            BYTE nV = *(BYTE*) (yuv_buffer.v + nUVShift + nHX);
//
//            unsigned char Y = yuv_buffer.y[y_row_offset + x];
//            unsigned char U = yuv_buffer.u[uv_row_offset + x];
//            unsigned char V = yuv_buffer.v[uv_row_offset + x];
//
//            float r = nY + 1.371f * (nV - 128);
//            float g = nY - 0.698f * (nV - 128) - 0.336f * (nU - 128);
//            float b = nY + 1.732f * (nU - 128);
//
//            int index = (y * yuv_buffer.y_width + x) * 4;
//            frame[index + 0] = ClampFloatToByte(r);
//            frame[index + 1] = ClampFloatToByte(g);
//            frame[index + 2] = ClampFloatToByte(b);
//            frame[index + 3] = 255;
//        }
//    }
}


/**********
 * VORBIS *
 **********/
