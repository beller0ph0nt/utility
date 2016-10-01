/**
    n! / (n - k)!
**/

Permutation::Permutation(int n):_n(n), _index(n - 1)
{
    _combo = new int[n];

//    for (int i = 0; i < _n; i++)
//        _combo[i] = 0;
}

Permutation::~Permutation()
{
    delete[] _combo;
}

void Permutation::next()
{
    _combo[_index]++;

    if (_combo[_index] < _n)
        return;



}

void Permutation::show()
{
}

bool Permutation::is_last()
{
}
