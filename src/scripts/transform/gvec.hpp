#pragma once
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include "gmat.hpp"

template<class T>
class gvec
{
private:
    unsigned int r,c;
    std::vector<T> data;
public:
    gvec(int R, int C=1): r(R), c(C) 
    {data.resize(r*c,0);}
    //gvec(int Z): r(1), c(Z) 
    //{data.resize(r*c,0);}
    gvec(){}
   ~gvec(){};
    
    // ACCESSORs

    T& operator [] (int i)
    {return data[i];}
    const T& operator [] (int i) const
    {return data[i];}
    T& operator () (int i, int j)
    {return data[i*c+j];}
    const T& operator () (int i, int j) const
    {return data[i*c+j];}

    // GETTERs
    
    int getRow()const{return r;}
    int getCol()const{return c;}
    int size()const{return r*c;}
    
    // SETTERs

    void resize(int rr, int cc)
    {
        r=rr; 
        c=cc;
        data.resize(r*c);
    }

    void resize(int zz)
    {
        r=zz; 
        c=1;
        data.resize(r*c);
    }
    
    // OPERATORs
    
    gvec<T> operator +(const gvec<T>& M) const
    {
        gvec res(*this);
        int i=0;
        for(T& j:res.data)
            j+=M[i++];

        return res;
    }

    gvec<T> operator -(const gvec<T>& M) const
    {
        gvec res(*this);
        int i=0;
        for(T& j:res.data)
            j-=M[i++];

        return res;
    }

    gvec<T> operator -() const
    {
        gvec res(*this);
        for(T& j:res.data)
            j=-j;

        return res;
    }

    T operator *(const gvec& M) const
    {
        T res=0;
        int i=M.size();
        
        while(i--)
            res+=data[i]*M[i];
        
        return res;
    }
    
    gvec<T> operator *(const gmat<T>& M) const
    {
        if(c!=M.getRow())
            return gvec<T>(0,0);

        gvec<T> res(r,M.getCol());
                
        for (int i = 0; i < r; i++)
            for (int j = 0; j < M.getCol(); j++)
                for (int k = 0; k < c; k++)
                    res(i,j)+=data[k]*M(k,j);

        return res;
    }

    gvec<T> operator *(const T& n) const
    {
        gvec res(*this);
        int i=0;
        for(T& j:res.data)
            j*=n;

        return res;
    }

    void operator *=(T& n)
    {
        int i=0;
        for(T& j:data)
            j*=n;

    }

    void operator =(const gvec& M)
    {   
        r=M.getRow();
        c=M.getCol();
        (*this).resize(r,c);
        
        int i=0;
        for(T& j:data)
            j=M[i++];
    }

    void operator = (const T& n)
    {
        for(T& j:data)
            j=n;
    }
    // FUNCTIONS

    gvec<T> transpose()
    {
        gvec<T> res(c,r);

        int i=0;
        for(T& j:res.data)
            j=data[i++];
       
        return res;
    }
    
    T norm()
	{
		T res=0;
		for(T i:data)
			res+=i*i;
	}

	void normalize() 
    { 
        T n = norm(); 
        if (n > 0) { 
            T factor = 1 / sqrt(n);
			for(auto& i:data)
				i*=factor;
        } 
        //return *this; 
    } 

    gvec<T> cross (gvec<T> v) const
    { 	//y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x
		gvec<T> res(r*c);
		res[0]= data[1] * v[2] - data[2] * v[1];
		res[1]= data[2] * v[0] - data[0] * v[2];
		res[2]= data[0] * v[1] - data[1] * v[0];
		res[3]= 0;
		return res;
	}

    friend std::ostream& operator << (std::ostream &s, const gvec<T> &v)
    {   
        return s << '[' << v[0] << ' ' << v[1] << ' ' << v[2] << ']';
    }
};