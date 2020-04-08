#pragma once

#include <vector>
template<class T>
class imat
{
private:
    
public:
    // DATA MEMBER
    int r,c;
    std::vector<T> mat;
    // ACCESSORS
    T& operator () (int i, int j)
    {
        return mat[i*c+j];
    }
    
    // CONSTRUCTOR
    imat(int R, int C):
        r(R), 
        c(C)
    {
        mat.resize(R*C);
    }
    imat(){};
   ~imat(){};
};

