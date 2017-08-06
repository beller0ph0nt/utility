#include <unistd.h>

char  path[] = "./process_b";
char* args[] = { "process_b", "asdasdasd", NULL };

int main(int argc, char** argv)
{
//    execve(path, args, NULL);

/*
   0x080483ee <+3>:    ba 00 00 00 00  mov    $0x0,%edx
   0x080483f3 <+8>:    b9 28 a0 04 08  mov    $0x804a028,%ecx
   0x080483f8 <+13>:   bb 1c a0 04 08  mov    $0x804a01c,%ebx
   0x080483fd <+18>:   b8 0b 00 00 00  mov    $0xb,%eax
   0x08048402 <+23>:   cd 80           int    $0x80

    __asm__(
        "movl   $0,%edx    \n"
        "movl   $args,%ecx \n"
        "movl   $path,%ebx \n"
        "movl   $11,%eax   \n"
        "int    $0x80      \n"
    );
*/

/*
=> 0x080483ee <+3>:     31 d2           xor    %edx,%edx
   0x080483f0 <+5>:     b9 28 a0 04 08  mov    $0x804a028,%ecx
   0x080483f5 <+10>:    bb 1c a0 04 08  mov    $0x804a01c,%ebx
   0x080483fa <+15>:    31 c0           xor    %eax,%eax
   0x080483fc <+17>:    b0 0b           mov    $0xb,%al
   0x080483fe <+19>:    cd 80           int    $0x80

=> 0x080483ee <+3>: 8d 55 f8    lea    -0x8(%ebp),%edx
=> 0x080483ee <+3>: 8d 5d 08    lea    0x8(%ebp),%ebx

*/
    __asm__(
        "leal    8(%ebp),%ebx  \n"
        "xor     %edx,%edx  \n"
        "movl    $args,%ecx \n"
        "movl    $path,%ebx \n"
        "xor     %eax,%eax  \n"
        "movb    $11,%al    \n"
        "int     $0x80      \n"
    );

/*
    __asm__(
        "movl    $0,    %edx   \n"
        "movl    $args, %ecx   \n"
        "movl    $path, %ebx   \n"
        "movl    $11,   %eax   \n"
        "call   *%gs:0x10   \n"
    );
*/
    return 0;
}