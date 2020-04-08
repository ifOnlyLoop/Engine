#include"imat.hpp"
#include<algorithm>

//#define tmat(M) gmat<float> M(4,4)

template<class T>
class gmat: public imat<T>
{
private:
    /* data */
public:
    //CON/DE-STRUCTOR
    gmat(int i,int j)
    {
        this->r=i; 
        this->c=j;
        this->mat.resize(i*j);
        (*this).identity();
    }
    gmat(){};
   ~gmat(){};

    T& operator [] (const int i) 
	{	
		this->mat[i];
	}
    // OPERATORs
    gmat<T> operator +(const gmat<T>& M) const
    {
        gmat<T> ans(this->r,this->c);
        for (int i = 0; i < this->r*this->c; i++)
            ans(i)=this->mat[i]+M.mat[i];
        return ans;
    }
    gmat<T> operator -(const gmat<T>& M) const
    {
        gmat<T> ans(this->r,this->c);
        for (int i = 0; i < this->r*this->c; i++)
            ans(i)=this->mat[i]-M.mat[i];
        return ans;
    }
    gmat operator *(const gmat& M) const
    {
        if(this->c==M.r)
        {
            int R=this->r,
                C=M.c;
            gmat<T> res(this->r,M.c);
                    res.zero();
            
            for (int i = 0; i < this->r; i++)
                for (int j = 0; j < M.c; j++)
                    for (int k = 0; k < this->c; k++)
                        res(i,j)+=this->mat[i*this->c+k]*M.mat[k*M.c+j];

            return res;
        } 
        else {
            // THROW ERROR
            gmat<T> res(0,0);
            return res;
        }
    }
    void operator =(const gmat<T>& M)
    {
        this->r=M.r;
        this->c=M.c;
        this->resize(this->r*this->c);
        int j=0;
        for(auto i:M.mat)
            this->mat[j++]=i;
    }

    // Util
    int size()
    {
        return this->r*this->c;   
    }
    void resize(int z)
    {
        this->mat.resize(z);
    }
    void zero()
	{
		for(auto& i:this->mat)
			i=0;
	}
	void ones()
	{
		for(auto& i:this->mat)
			i=1;
	}
	void identity()
	{
		int i = this->r > this->c? this->c:this->r;
		while(i--) this->mat[i*this->c+i]=1;
	}
    gmat<T> transpose()
    {
        gmat<T> res(this->c,this->r);
        for (int i = 0; i < this->c; i++)
        {
            for (int j = 0; j < this->r; j++)
            {
                res(i,j)=this->mat[j*this->c+i];
            }
        }
        return res;
    }
    gmat inverse() const
	{
        int n = this->c > this->r? 
                this->r : this->c;
        
        gmat<T> M(*this), 
                I(n,n);

		float r;
		int i,j,k;
        
        // ADD PIVOTING HERE

		// Applying Gauss Jordan Elimination
        for(i=0;i<n;i++)
        {
            for(j=0;j<n;j++) if(j!=i)
            {
                r=M(j,i)/M(i,i);
                for(k=0;k<n;k++)
                {
                    M(j,k)-=M(i,k)*r;
                    I(j,k)-=I(i,k)*r;
                }       
            }
        }
		
		for(i=0;i<n;i++)
        {   // M -> unity I -> inv
            r=M(i,i);
            for(j=0;j<n;j++)
            {
                I(i,j)=I(i,j)/r;
            }
        }
		return I;
	}

};

//using tmat = gmat<float>(4,4);