#include "so.h"

int
main(int argc, char** argv)
{
    lib_func();

    while (1)
    {
        sleep(1);
    }

    lib_func();

    return 0;
}