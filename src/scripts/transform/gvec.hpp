#pragma once

#include "gmat.hpp"

// class gvec a simple 3D gvecor class
template<class T>
class gvec: public gmat<T> 
{ // float: Data Type 
public:
	//CON/DE-STRUCTOR
    gvec(int j,int i=1)
    {
        this->r=i; 
        this->c=j;
        this->mat.resize(i*j);
    }
    gvec(){};
   ~gvec(){};
	
	// Boolian Operations 

	bool operator ==(gvec& vec) 
	{
		if(this->size()!= vec.size())
			return false;
		for (int i = 0; i < vec.size(); i++)
			if(vec[i]!=this->mat[i])
				return false;
		return true;
	}
	bool operator !=(gvec& vec)
	{
		if(this->size()!= vec.size())
			return true;
		for (int i = 0; i < vec.size(); i++)
			if(vec[i]!=this->mat[i])
				return true;
		return false;
	}
	gvec operator *(const gmat<T>& M) const
    {
        if(this->c==M.r)
        {
            int R=this->r,
                C=M.c;
            gvec<T> res(this->c);
                    res.zero();
            
            for (int i = 0; i < this->r; i++)
                for (int j = 0; j < M.c; j++)
                    for (int k = 0; k < this->c; k++)
                        res(i,j)+=this->mat[i*this->c+k]*M.mat[k*M.c+j];

            return res;
        } 
        else {
            // THROW ERROR
            gvec<T> res(0,0);
            return res;
        }
    }
	/*T operator * (gvec<T>& vec)
	{
		if(this->size()!= vec.size())
		{
			// throw error
			return -1;
		}	
		T res=0;
		for (int i = 0; i < vec.size(); i++)
			res+= vec[i]*this->mat[i];
		return res;
	}*/

	gvec<T> operator ^(const gvec<T> vec)
	{
		if(this->size()!= vec.size())
		{
			// throw error
			gvec<T> res(0);
			return res;
		}
		gvec<T> res(vec.size());
		for (int i = 0; i < vec.size(); i++)
		{
			for (int j = 0; j < vec.size(); j++)
			{
				res[i]+=this->mat[i];
			}
		}
		return res;
	}
	// Set the gvecor to zero
	
	// Unary minus returns the negative of the gvecor
	/*gvec<T> operator -() const
	{
		gvec<T> res(this->size()) 
	}*/
};

	/*
	// Binary + and - add and subtract gvecors
	gvec operator +(const gvec& a) const;
	gvec operator -(const gvec& a) const;
	// Multiplication and division by scalar
	gvec operator *(float a) const;
	gvec operator /(float a) const;

	// Assignment operations 
	gvec& operator = (const gvec& a);
	gvec& operator +=(const gvec& a);
	gvec& operator -=(const gvec& a);
	gvec& operator *=(float a);
	gvec& operator /=(float a);

	// normalize the gvecor 
	void normalize();
	// rotate around it
	void rotate(gvec& v){}
	// gvecor dot product 
	float operator *(const gvec& a) const;
};
 
/* * * * * * * * * * * * 
 * Nonmember functions *
 * * * * * * * * * * * */
/*
// Compute the magnitude of a gvecor gvecorMag
extern float norm(const gvec& a);
// Compute the cross product of two gvecors
extern gvec cross(const gvec& a, const gvec& b);
// Scalar on the left multiplication, for symmetry
extern gvec operator *(float k, const gvec& v);
// Compute the distance between two points
extern float dist(const gvec& a, const gvec& b);

//Global variables


 // We provide a global zero gvecor constant
extern const gvec kZerogvecor;
*/