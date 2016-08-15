#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trace.h"
#include "wrap-ogg.h"
#include "ogg-player.h"
#include "wrap-theora.h"

#define PHYSICAL_STREAM_READ_BLOCK_SIZE     8192

FILE* fd = NULL;

ogg_sync_state   sync_state_;
ogg_stream_state theora_stream_state_;
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

    /**
        - декодирование заголовков
            - получить пакет
            -
        Необходимо диамически формировать все логические потоки
        в зависимости от серийного номера считываемой страницы.
     **/

     ogg_pull_packet_from_logical_stream();


//    theora_decode_identification_header_packet();
//    theora_decode_comment_header_packet();
//    theora_decode_setup_header_packet();

    fclose(fd);

    return EXIT_SUCCESS;
}

void
ogg_init()
{
    TRACE_DEBUG("");
    memset(&sync_state_, 0x00, sizeof(ogg_sync_state));
    memset(&theora_stream_state_, 0x00, sizeof(ogg_stream_state));
    memset(&page_, 0x00, sizeof(ogg_page));
    memset(&packet_, 0x00, sizeof(ogg_packet));

    ogg_sync_init(&sync_state_);
}

void
ogg_read_block_into_physical_stream()
{
    char* buffer = w_ogg_sync_buffer(&sync_state_, PHYSICAL_STREAM_READ_BLOCK_SIZE);
    size_t bytes = fread(buffer, 1, PHYSICAL_STREAM_READ_BLOCK_SIZE, fd);
    TRACE_DEBUG("read %d byte(s)", bytes);
    w_ogg_sync_wrote(&sync_state_, bytes);
}

void
ogg_pull_page_from_physical_stream()
{
    while (w_ogg_sync_pageout(&sync_state_, &page_) != 1)
        ogg_read_block_into_physical_stream();

    TRACE_DEBUG("page  body_len: %ld  header_len: %ld", page_.body_len, page_.header_len);
}




void
ogg_init_logical_stream()
{
    ogg_pull_page_from_physical_stream();
    int serial_no = ogg_page_serialno(&page_);

    TRACE_DEBUG("page serial number: %d", serial_no);

    w_ogg_stream_init(&theora_stream_state_, serial_no);
    ogg_push_page_into_logical_stream();
}



void
ogg_push_page_into_logical_stream()
{
    TRACE_DEBUG("");
    w_ogg_stream_pagein(&theora_stream_state_, &page_);
}

void
ogg_pull_packet_from_logical_stream()
{
    TRACE_DEBUG("");
    while (w_ogg_stream_packetout(&theora_stream_state_, &packet_) != 1)
    {
        ogg_pull_page_from_physical_stream();
        ogg_push_page_into_logical_stream();
    }
}





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

void
theora_decode_header_packet()
{
    TRACE_DEBUG("");
    while (1)
    {

    ogg_pull_packet_from_logical_stream();
    int ret = w_theora_decode_header(&theora_info_, &theora_comment_, &packet_);
    if (ret == 0)
        return;

    }
}

void
theora_decode_identification_header_packet()
{
    TRACE_DEBUG("");
    ogg_init_logical_stream();
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
