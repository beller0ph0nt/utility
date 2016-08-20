#include <stdlib.h>

size_t
get_chunk_size()
{
    2 * sizeof(size_t)
}

void*
get_chunk(void *ptr)
{
    size_t chunk_size = get_chunk_size;
    char* chunk = (char*)ptr - chunk_size;

    return chunck;
}

void
print_chunk(void* ptr)
{
    char* chunk = get_chunk(ptr);

    int i;
    size_t chunk_size = get_chunk_size;
    printf("chunck:");
    for (i = 0; i < chunk_size; i++)
        printf(" 0x%02h", chunk[i]);
}

int
main(int argc, char** argv)
{
    return EXIT_SUCCESS;
}