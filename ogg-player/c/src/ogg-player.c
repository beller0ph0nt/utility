#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/gl.h>
//#include <GL/glut.h>

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
    unsigned char* rgb_buffer = \
        theora_create_rgb_buffer(player.theora.info.frame_width,
                                 player.theora.info.frame_height);
    while (!feof(player.fd))
    {
        ogg_pull_packet_from_logical_stream(&player,
                                            &player.theora.logical_stream->state,
                                            &packet);
        theora_decode_packetin(&player.theora.state, &packet);
        theora_decode_YUVout(&player.theora.state, &yuv);

//        TRACE_INFO("y   width:  %d  height: %d  stride: %d"
//                   "    "
//                   "uv  width:  %d  height: %d  stride: %d\n"
//                   "uv.width  / y.width  = %f\n"
//                   "uv.height / y.height = %f",
//                   yuv.y_width,
//                   yuv.y_height,
//                   yuv.y_stride,
//                   yuv.uv_width,
//                   yuv.uv_height,
//                   yuv.uv_stride,
//                   (float) (yuv.uv_width * 1.0 / yuv.y_width),
//                   (float) (yuv.uv_height * 1.0 / yuv.y_height));
//
//        break;
        theora_yuv_to_rgb(&yuv, rgb_buffer);


        glTexImage2D(GL_TEXTURE_RECTANGLE_NV,
                     0,
                     GL_RGB,
                     yuv.y_width,
                     yuv.y_height,
                     0,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     rgb_buffer);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        double mv[16] = { 1,  0,  0,  0,
                          0,  1,  0,  0,
                          0,  0, -1,  0,
                          0,  0,  0,  1 };
        glLoadMatrixd(&mv[0]);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_TEXTURE_RECTANGLE_NV);
        glColor4f(1, 1, 1, 1);
        glTranslatef(50, 50, 0);
        glScalef(1, 1, 1);
        glBegin(GL_QUADS);

        glTexCoord2i(0, yuv.y_height - 1);
        glVertex3i(0, 0, 9);

        glTexCoord2i(0, 0);
        glVertex3i(0, yuv.y_height, 9);

        glTexCoord2i(yuv.y_width - 1, 0);
        glVertex3i(yuv.y_width, yuv.y_height, 9);

        glTexCoord2i(yuv.y_width - 1, yuv.y_height - 1);
        glVertex3i(yuv.y_width, 0, 9);

        glEnd();

        glFinish();
//        SwapBuffers(hDC);

//        break;
    }

    free(rgb_buffer);



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

unsigned char*
theora_create_rgb_buffer(uint32_t width, uint32_t height)
{
    unsigned char* buffer = calloc(width * height, 3);

    return buffer;
}

unsigned char
clamp(float x, int min, int max)
{
    if (x > max)
        x = max;

    if (x < min)
        x = min;

    return (unsigned char) x;
}

void
theora_yuv_to_rgb(yuv_buffer* yuv_buffer, unsigned char* rgb_buffer)
{
    int y_row;
    for (y_row = 0; y_row < yuv_buffer->y_height; y_row++)
    {
        int y_row_offset = yuv_buffer->y_stride * y_row;
        int uv_row_offset = yuv_buffer->uv_stride * y_row / 2;

        int y_col;
        for (y_col = 0; y_col < yuv_buffer->y_width; y_col++)
        {
            int uv_col = y_col / 2;

            unsigned char Y = yuv_buffer->y[y_row_offset + y_col];
            unsigned char U = yuv_buffer->u[uv_row_offset + uv_col];
            unsigned char V = yuv_buffer->v[uv_row_offset + uv_col];

            float r = Y + 1.371f * (V - 128);
            float g = Y - 0.698f * (V - 128) - 0.336f * (U - 128);
            float b = Y + 1.732f * (U - 128);

            int index = (y_row * yuv_buffer->y_width + y_col) * 3;
            rgb_buffer[index + 0] = clamp(r, 0, 255);
            rgb_buffer[index + 1] = clamp(g, 0, 255);
            rgb_buffer[index + 2] = clamp(b, 0, 255);
        }
    }
}


/**********
 * VORBIS *
 **********/
