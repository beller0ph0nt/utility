#ifndef __WRAP_OGG_H
#define __WRAP_OGG_H

#include <ogg/ogg.h>

char*
w_ogg_sync_buffer(ogg_sync_state *oy, long size);

void
w_ogg_sync_wrote(ogg_sync_state *oy, long bytes);

int
w_ogg_sync_pageout(ogg_sync_state *oy, ogg_page *og);

void
w_ogg_stream_init(ogg_stream_state *os,int serialno);

int
w_ogg_stream_pagein(ogg_stream_state *os, ogg_page *og);

int
w_ogg_stream_packetout(ogg_stream_state *os,ogg_packet *op);

#endif // __WRAP_OGG_H
