#include <stdio.h>

int pid;

int main() {
    pid = getpid();
    printf("pid is %d\n", pid);

    __asm__(
        "movl $20, %eax    \n"
        "call *%gs:0x10    \n"   /* offset 0x10 is not fixed across the systems */
        "movl %eax, pid    \n"
    );
    printf("pid is %d\n", pid);

    __asm__(
        "movl $20, %eax     \n"
        "int  $0x80         \n"
        "movl %eax, pid     \n"
    );
    printf("pid is %d\n", pid);

    return 0;
}