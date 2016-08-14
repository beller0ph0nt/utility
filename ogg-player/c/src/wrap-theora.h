#ifndef __WRAP_THEORA_H
#define __WRAP_THEORA_H

#include <theora/theora.h>

int
w_theora_decode_header(theora_info* ci, theora_comment* cc, ogg_packet* op);

#endif // __WRAP_THEORA_H
