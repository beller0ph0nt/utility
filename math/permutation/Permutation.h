#include <PermutationInterface.h>

class Permutation : public IPermutation
{
public:
    Permutation(int n);
    ~Permutation();

    void next();
    void show();
    bool is_last();

private:
    int  _n;
    int  _k;
    int  _index;
    int* _combo;
};
