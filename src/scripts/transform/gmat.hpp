#pragma once

#include <vector>

template<class T>//,int R, int C>
class gmat
{
private:
    unsigned int r,c;
    std::vector<std::vector<T>> data;
public:
    gmat(int R, int C): r(R), c(C) 
    {data.resize(R,std::vector<T>(C,0));}
    gmat(){}
   ~gmat(){};
    
    // ACCESSORs

    std::vector<T>& operator [] (int i)
    {return data[i];}
    const std::vector<T>& operator [] (int i) const
    {return data[i];}
    T& operator () (int i, int j)
    {return data[i][j];}
    const T& operator () (int i, int j) const
    {return data[i][j];}

    // GETTERs
    
    int getRow()const{return r;}
    int getCol()const{return c;}
    int size()const{return r*c;}

    // SETTERs

    void resize(int rr, int cc)
    {
        r=rr; 
        c=cc;
        data.resize(rr);
        for(auto& vt:data)
            vt.resize(cc);
    }
    
    // OPERATORs
    
    gmat<T> operator +(const gmat<T>& M) const
    {
        gmat res(*this);

        for (int i = 0; i < r; i++)
            for (int j = 0; j < c; j++)
                res(i,j)+=M(i,j);
            
        return res;
    }

    gmat<T> operator -(const gmat<T>& M) const
    {
        gmat res(*this);

        for (int i = 0; i < r; i++)
            for (int j = 0; j < c; j++)
                res(i,j)-=M(i,j);
            
        return res;
    }

    gmat operator *(const gmat& M) const
    {
        if(c!=M.getRow())
            return *this;

        gmat<T> res(r,M.getCol());
                
        for (int i = 0; i < r; i++)
            for (int j = 0; j < M.getCol(); j++)
                for (int k = 0; k < c; k++)
                    res(i,j)+=data[i][k]*M(k,j);

        return res;
    }
    
    gmat<T> operator *(const T& n) const
    {
        gmat<T> res(*this);
        
        for (int i = 0; i < r; i++)
            for (int j = 0; j < c; j++)
                res(i,j)*=n;
        
        return res;
    }

    void operator =(const gmat& M)
    {
        r=M.getRow();
        c=M.getCol();
        (*this).resize(M.getRow(),M.getCol());
        
        for (int i = 0; i < r; i++)
            for (int j = 0; j < c; j++)
                data[i][j]=M(i,j);
    }

    void operator = (const T& n)
    {
        for (int i = 0; i < r; i++)
            for (int j = 0; j < c; j++)
                data[i][j]=n;
    }
    // FUNCTIONS

    void fill(T n)
	{
        for (int i = 0; i < r; i++)
            for (int j = 0; j < c; j++)
                data[i][j]=n;
	}
	
	void identity()
	{
        (*this).fill(0);
		int i = r>c ? c:r;
		while(i--) data[i][i]=1;
	}

    gmat<T> transpose()
    {
        gmat<T> res(c,r);

        for (int i = 0; i < r; i++)
            for (int j = 0; j < c; j++)
                res(j,i)=data[i][j];
        
        return res;
    }

    gmat inverse() const
	{
        int n = c>r ? r:c;
        
        gmat M(*this); 
        gmat I(*this);
        I.identity();

		float r;
		int i,j,k;
        
        // ADD PIVOTING HERE

		// Applying Gauss Jordan Elimination
        for(i=0;i<n;i++)
        {
            for(j=0;j<n;j++)
            { 
                if(j!=i)
                {
                    r=M(j,i)/M(i,i);
                    for(k=0;k<n;k++)
                    {
                        M(j,k)-=M(i,k)*r;
                        I(j,k)-=I(i,k)*r;
                    }       
                }
            }
        }

		// M -> unity I -> inv
		for(i=0;i<n;i++)
        {   
            r=M(i,i);
            for(j=0;j<n;j++)
            {
                I[i][j]=I[i][j]/r;
            }
        }

		return I;
	}
  
};
/*
using tmat = gmat<float,4,4>;
using tvec = gmat<float,1,4>;
using rmat = gmat<float,3,3>;
using rvec = gmat<float,1,3>;
*/