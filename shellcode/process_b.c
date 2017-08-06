#include <stdio.h>
#include <string.h>

void vuln(char* str)
{
    char buf[5] = { 0 };
    strncpy(buf, str, strlen(str));
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("usage: %s <str>\n", argv[0]);
        return 0;
    }

    printf("process B str: %s\n", argv[1]);
    vuln(argv[1]);

    return 0;
}