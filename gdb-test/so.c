#include <stdio.h>

void
lib_func()
{
    int a = 13;

    asm("int $3");

    printf("a = %d\n", a);
}