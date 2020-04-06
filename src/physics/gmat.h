#include"imat.h"

template<class T>
class gmat: public imat<T>
{
private:
    /* data */
public:
    gmat(int i,int j)
    {
        r=i; 
        c=j;
        mat.resize(i*j);
    }
   ~gmat();

    imat<T> operator +(const imat<T>& M) const
    {
        imat<T> ans(r,c);
        for (int i = 0; i < r*c; i++)
            ans(i)=mat[i]+M.mat[];
        return ans;
    }
};

