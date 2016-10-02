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
    bool is_leftmost_digit();
    bool is_rightmost_digit();
    bool is_all_digits_different();
    bool is_last_digit_combination();

    void next_digit_combination();

    void reset_digit_combination();
    void reset_digit();

    void left_digit_shift();

private:
    int  _n;
    int  _k;
    int  _digit;
    int* _combination;
    bool _is_last;
};
