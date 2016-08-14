#include <stdlib.h>

#include "trace.h"
#include "wrap-theora.h"

int
w_theora_decode_header(theora_info* ci, theora_comment* cc, ogg_packet* op)
{
    int ret = theora_decode_header(ci, cc, op);
    if (ret == OC_BADHEADER)
    {
        TRACE_ERROR("theora_decode_header failure (OC_BADHEADER:%d)", ret);
        exit(EXIT_FAILURE);
    }
    else if (ret == OC_VERSION)
    {
        TRACE_ERROR("theora_decode_header failure (OC_VERSION:%d)", ret);
        exit(EXIT_FAILURE);
    }
    else if (ret == OC_NEWPACKET)
    {
        TRACE_ERROR("theora_decode_header failure (OC_NEWPACKET:%d)", ret);
        exit(EXIT_FAILURE);
    }
    else if (ret == OC_NOTFORMAT)
    {
        TRACE_WARNING("theora_decode_header (OC_NOTFORMAT:%d)", ret);
    }

    return ret;
}
