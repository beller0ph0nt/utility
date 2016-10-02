#include <Permutation.h>

int
main(int argc, char** argv)
{
    Permutation p(4);

    while (p.is_last() == false)
    {
        p.show();
        p.next();
    }

    return 0;
}
