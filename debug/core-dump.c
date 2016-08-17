#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>

void
core_dump_1st()
{
    abort();
}

#define MEANING_OF_LIFE    42

void
core_dump_2nd()
{
    *((int*) 0) =  MEANING_OF_LIFE;
}

void
core_dump_3rd()
{
    kill(getpid(), SIGSEGV);
}

void
core_dump_4th()
{
    raise(SIGSEGV);
}

int
main()
{
    core_dump_1st();
    core_dump_2nd();
    core_dump_3rd();
    core_dump_4th();
}