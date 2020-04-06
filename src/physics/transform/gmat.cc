#define TC template <class T>
#include "gmat.h"


gmat::gmat(/* args */)
{
}

gmat::~gmat()
{
}


TC gmat<T> operator +(const gmat<T>& M)
{
    gmat<T> ans(r,c);
    for (int i = 0; i < r*c; i++)
        ans(i)=mat[i]+M.mat[];
    return ans;
}

TC gmat<T> operator -(const gmat<T>& M)
{
    gmat<T> ans(r,c);
    for (int i = 0; i < r*c; i++)
        ans(i)=mat[i]-M.mat[i];
    return ans;
}

TC gmat<T> operator *(const gmat<T>& M)
{
    if(r==M.c&&c==M.r)
    {
        int R=r,
            C=M.c;

        gmat<T> ans(r,M.c);
        
        for (int i = 0; i < r; i++)
            for (int j = 0; j < M.c; i++)
            ans(i)=mat[i]+M.mat[];
        
        return ans;
    } else
    {
        // THROW ERROR
        return gmat<T> ans(0,0);
    }
}



    // OPERATORs
    void vect::zero() 
    {x = y = z = 0.0f;}
    // Unary minus 
    vect vect::operator -() const 
    { return vect(-x, -y, -z);}
    // Binary + and - add and subtract vectors
    
    gmat<T> operator +(const gmat<T>& M) const;
    gmat<T> operator -(const gmat<T>& M) const;	
    // Multiplication and division by scalar
    gmat<T> operator *(const gmat<T> &a) const;
    
    vect vect::operator /(float a) const 
    {	// NOTE: ADD TRY CATCH for /0
        float oneOverA = 1.0f / a; 
        return vect(x * oneOverA, y * oneOverA, z * oneOverA);
    }   