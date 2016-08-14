#include <stdlib.h>

#include "trace.h"
#include "wrap-ogg.h"

char*
w_ogg_sync_buffer(ogg_sync_state *oy, long size)
{
    char* buffer = ogg_sync_buffer(oy, size);
    if (buffer == NULL)
    {
        TRACE_ERROR("ogg_sync_buffer failure");
        exit(EXIT_FAILURE);
    }

    return buffer;
}

void
w_ogg_sync_wrote(ogg_sync_state *oy, long bytes)
{
    if (ogg_sync_wrote(oy, bytes) == -1)
    {
        TRACE_ERROR("ogg_sync_wrote failure");
        exit(EXIT_FAILURE);
    }
}

int
w_ogg_sync_pageout(ogg_sync_state *oy, ogg_page *og)
{
    int ret = ogg_sync_pageout(oy, og);
    if (ret == -1)
    {
        TRACE_ERROR("ogg_sync_pageout failure");
        exit(EXIT_FAILURE);
    }

    return ret;
}

void
w_ogg_stream_init(ogg_stream_state *os,int serialno)
{
    int ret = ogg_stream_init(os, serialno);
    if (ret == -1)
    {
        TRACE_ERROR("ogg_stream_init failure");
        exit(EXIT_FAILURE);
    }
    else if (ret != 0)
        TRACE_WARNING("ogg_stream_init ret: %d", ret);
}

int
w_ogg_stream_pagein(ogg_stream_state *os, ogg_page *og)
{
    int ret = ogg_stream_pagein(os, og);
    if (ret == -1)
        TRACE_ERROR("ogg_stream_pagein incorrect page serial number");

    return ret;
}

int
w_ogg_stream_packetout(ogg_stream_state *os,ogg_packet *op)
{
    int ret = ogg_stream_packetout(os, op);
    if (ret == -1)
    {
        TRACE_ERROR("ogg_stream_packetout failure");
        exit(EXIT_FAILURE);
    }

    return ret;
}
