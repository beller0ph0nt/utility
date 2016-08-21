#include <string.h>

#include "ogg-skeleton.h"

int
is_skeleton_packet(ogg_packet* packet)
{
    if (is_1st_skeletoh_packet(packet) ||
        is_2nd_skeleton_packet(packet))
        return 1;

    return 0;
}

int
is_1st_skeletoh_packet(ogg_packet* packet)
{
    if (strcmp((char*) packet->packet, SKELETON_1ST_HEADER_IDENT) == 0)
        return 1;

    return 0;
}

int
is_2nd_skeleton_packet(ogg_packet* packet)
{
    if (strcmp((char*) packet->packet, SKELETON_2ND_HEADER_IDENT) == 0)
        return 1;

    return 0;
}
