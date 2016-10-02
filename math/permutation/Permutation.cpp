/**
    n! / (n - k)!
**/
#include <iostream>
using namespace std;

#include <Permutation.h>

Permutation::Permutation(int n)
{
    _n = n;
    _digit = _n - 1;
    _is_last = false;
    _combination = new int[_n];
}

Permutation::~Permutation()
{
    delete[] _combination;
}

bool
Permutation::is_leftmost_digit()
{
    return (_digit == 0) ? true : false;
}

bool
Permutation::is_rightmost_digit()
{
    return (_digit == _n - 1) ? true : false;
}

bool
Permutation::is_last_digit_combination()
{
    return (_combination[_digit] < _n) ? false : true;
}

bool
Permutation::is_all_digits_different()
{
    for (int i = 0; i < _n; i++)
        for (int j = i + 1; j < _n; j++)
            if (_combination[i] == _combination[j])
                return false;

    return true;
}

void
Permutation::next_digit_combination()
{
    _combination[_digit]++;
}

void
Permutation::reset_digit_combination()
{
    _combination[_digit] = 0;
}

void
Permutation::reset_digit()
{
    _digit = _n - 1;
}

void
Permutation::left_digit_shift()
{
    _digit--;
}

void
Permutation::next()
{
    if (is_last_digit_combination() == false)
    {
        next_digit_combination();

        if (is_rightmost_digit() == false)
            reset_digit();

        if (is_all_digits_different() == false)
            next();
    }
    else if (is_leftmost_digit() == false)
    {
        reset_digit_combination();
        left_digit_shift();
        next();
    }
    else
    {
        _is_last = true;
    }
}

void
Permutation::show()
{
    for (int i = 0; i < _n; i++)
        cout << " " << _combination[i];
    cout << endl;
}

bool
Permutation::is_last()
{
    return _is_last;
}
