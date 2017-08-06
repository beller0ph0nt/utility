#include <unistd.h>

int main(int argc, char** argv)
{
    char* a[] = { "process_b", "asdasdasd", NULL };
//    execv("./process_b", a);
    execve("./process_b", a, NULL);

    return 0;
}