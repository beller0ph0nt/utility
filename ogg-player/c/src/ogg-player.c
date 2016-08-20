#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vorbis/vorbisenc.h>

#include "trace.h"
#include "wrap-ogg.h"
#include "ogg-player.h"
#include "wrap-theora.h"

#define PHYSICAL_STREAM_READ_BLOCK_SIZE     8192

FILE* fd = NULL;

//ogg_sync_state   sync_state_;
ogg_sync_state   physical_stream_;
ogg_stream_state theora_stream_state_;

ogg_logical_stream_t* logical_stream_ = NULL;
unsigned int          logical_stream_count_ = 0;

ogg_page         page_;
ogg_packet       packet_;

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

    fd = fopen(argv[1], "r");

    ogg_init();
    theora_init();

    while (!feof(fd))
    {
        ogg_pull_page_from_physical_stream(&physical_stream_, &page_);
        ogg_push_page_into_logical_stream(&page_);
//        ogg_pull_packet_from_logical_stream();
    }

    TRACE_INFO("********LOGICAL STREAMS*******");
    int i;
    for (i = 0; i < logical_stream_count_; i++)
    {
        ogg_pull_packet_from_logical_stream(&logical_stream_[i].stream, &packet_);
        if (theora_packet_isheader(&packet_))
            TRACE_INFO("index: %d serial: %d type: THEORA", i, logical_stream_[i].serial_num);
        else if (vorbis_synthesis_idheader(&packet_))
            TRACE_INFO("index: %d serial: %d type: VORBIS", i, logical_stream_[i].serial_num);
        else
            TRACE_INFO("index: %d serial: %d type: UNKNOWN", i, logical_stream_[i].serial_num);

    }
    TRACE_INFO("******************************");



//    theora_decode_identification_header_packet();
//    theora_decode_comment_header_packet();
//    theora_decode_setup_header_packet();

    fclose(fd);

    return EXIT_SUCCESS;
}


/*******
 * OGG *
 *******/


void
ogg_init()
{
    TRACE_DEBUG("");
//    memset(&sync_state_, 0x00, sizeof(ogg_sync_state));
//    memset(&physical_stream_, 0x00, sizeof(ogg_sync_state));
//    memset(&theora_stream_state_, 0x00, sizeof(ogg_stream_state));
//    memset(&page_, 0x00, sizeof(ogg_page));
//    memset(&packet_, 0x00, sizeof(ogg_packet));

//    ogg_sync_init(&sync_state_);
    ogg_sync_init(&physical_stream_);
}

void
ogg_read_block_into_physical_stream(ogg_sync_state* physical_stream)
{
    char* buffer = w_ogg_sync_buffer(physical_stream, PHYSICAL_STREAM_READ_BLOCK_SIZE);
    size_t bytes = fread(buffer, 1, PHYSICAL_STREAM_READ_BLOCK_SIZE, fd);
//    TRACE_DEBUG("read %d byte(s)", bytes);
    w_ogg_sync_wrote(physical_stream, bytes);
}

void
ogg_pull_page_from_physical_stream(ogg_sync_state* physical_stream,
                                   ogg_page*       page)
{
    while (w_ogg_sync_pageout(physical_stream, page) != 1)
        ogg_read_block_into_physical_stream(physical_stream);

//    TRACE_DEBUG("page  body_len: %ld  header_len: %ld", page.body_len, page.header_len);
}




//void
//ogg_init_logical_stream()
//{
//    ogg_pull_page_from_physical_stream();
//    int serial_no = ogg_page_serialno(&page_);
//
//    TRACE_DEBUG("page serial number: %d", serial_no);
//
//    w_ogg_stream_init(&theora_stream_state_, serial_no);
//    ogg_push_page_into_logical_stream();
//}



void
ogg_push_page_into_logical_stream(ogg_page* page)
{
//    TRACE_DEBUG("");

    int i;
    int serial_num = ogg_page_serialno(page);
    for (i = 0; i < logical_stream_count_; i++)
    {
        if (logical_stream_[i].serial_num == serial_num)
        {
            w_ogg_stream_pagein(&logical_stream_[i].stream, page);
//            TRACE_DEBUG("add page to logical stream serial: %d", serial_num);
            return;
        }
    }

    i = logical_stream_count_++;
    logical_stream_ = realloc(logical_stream_, sizeof(ogg_logical_stream_t) * logical_stream_count_);
    logical_stream_[i].serial_num = serial_num;
    w_ogg_stream_init(&logical_stream_[i].stream, serial_num);
    w_ogg_stream_pagein(&logical_stream_[i].stream, page);
}

void
ogg_pull_packet_from_logical_stream(ogg_stream_state* logical_stream, ogg_packet* packet)
{
//    TRACE_DEBUG("");
    while (w_ogg_stream_packetout(logical_stream, packet) != 1)
    {
        ogg_pull_page_from_physical_stream(&physical_stream_, &page_);
        ogg_push_page_into_logical_stream(&page_);
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
    while (1)
    {

//    ogg_pull_packet_from_logical_stream();
    int ret = w_theora_decode_header(&theora_info_, &theora_comment_, &packet_);
    if (ret == 0)
        return;

    }
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
