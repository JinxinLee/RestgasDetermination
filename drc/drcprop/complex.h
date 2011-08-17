//--------------------------------------------------
//  vs_complex.h 
//

/*! \brief Template class for complex numbers

  \class complex complex.h

 this routines are based on volya_complex.h (Volya at MSU)
 Version 1.0.1, he did
  
\verbatim
  -- Some modification for speed
  -- Some protections from overflow and underflow
  -- Solving of equations of order 2,3,4:
     Solve2, Solve3, Solve4;
\endverbatim

 this routine extends Volyas routine by using templates
 for more flexibility. Even it is tried to get as close 
 as possible to Volyas routine, it is not the same.

 Where a scalar x = {int, long int, float, double, long double}
 occured, those spezializations are implemented . But only where it
 is useful eg. complex<scalar> += x
 but not       x += complex<scalar> (integers are skipped)


*/
// 
// Carsten Schwarz (C.Schwarz@gsi.de)
// GSI
// Plankstr. 1
// 64291 Darmstadt
// Germany
//
// 21.7.99
//
//---------------------------------------------------

// $Id: complex.hxx,v 1.2 2003/05/01 13:11:48 carsten Exp $
// cvs log at end of file


#ifndef __COMPLEX_CXX__
#define __COMPLEX_CXX__  // endif at end of file




#ifndef small_epsilon
#define small_epsilon 1e-12
#endif

#ifndef PI
#define PI 3.141592653589793238462643
#endif

#include <cstdlib>
#include <cmath>
#include <cassert>
#include <iostream>


template <class scalar> class complex
{
  public:
  complex() 
  {
    re=0.0;
    im=0.0;
  }                                 
  complex(scalar r,scalar i) : re(r),im(i){}
  public:
  scalar re;
  scalar im;
};




//!specialization of complex numbers template
template <> class complex<float>
{
  public: 
  complex(float r=0.0, float i=0.0) : re(r), im(i) {}
  explicit complex(const complex<double>& a);
  explicit complex(const complex<long double>& a);
  public:
  float re,im;
};
//--------
//!specialization of complex numbers template
template <> class complex<double>
{
  public: 
  complex(double r=0.0, double i=0.0) : re(r), im(i) {}
  complex (const complex<float>& a);
  explicit complex (const complex<long double>& a);

  public:
  double re,im;
};
//--------
//!specialization of complex numbers template
template <> class complex<long double>
{
  public: 
  complex(long double r=0.0, long double i=0.0) : re(r), im(i) {}
  complex (const complex<float>& a);
  complex (const complex<double>& a);
  
  public:
  long double re,im;
};













//--------------------------------------------------------------
// += -= *= /=
//--------------------------------------------------------------
//                            complex<scalar> += complex<scalar> 
template <class scalar>
  inline complex<scalar>& operator += (complex<scalar>& a,
				       const complex<scalar>& b)
{
  a.re += b.re;
  a.im += b.im;
  return a;
}
//                            scalar += complex<scalar>
template <class scalar>
  inline scalar& operator += (float &a, 
			      const complex<scalar>& b)
{
  assert(fabs(b.im) < small_epsilon);
  a+=b.re;
  return a;
}
template <class scalar>
  inline scalar& operator += (double &a, 
			      const complex<scalar>& b)
{
  assert(fabs(b.im) < small_epsilon);
  a+=b.re;
  return a;
}
template <class scalar>
  inline scalar& operator += (long double &a, 
			      const complex<scalar>& b)
{
  assert(fabs(b.im) < small_epsilon);
  a+=b.re;
  return a;
}
//                            complex<scalar> += scalar
template <class scalar>
  inline complex<scalar>& operator += (complex<scalar>& a,
				       const int &b)
{
  a.re += b;
  return a;
}
template <class scalar>
  inline complex<scalar>& operator += (complex<scalar>& a,
				       const long int &b)
{
  a.re += b;
  return a;
}
template <class scalar>
  inline complex<scalar>& operator += (complex<scalar>& a,
				       const float &b)
{
  a.re += b;
  return a;
}
template <class scalar>
  inline complex<scalar>& operator += (complex<scalar>& a,
				       const double &b)
{
  a.re += b;
  return a;
}
template <class scalar>
  inline complex<scalar>& operator += (complex<scalar>& a,
				       const long double &b)
{
  a.re += b;
  return a;
}
//                            complex<scalar> -= complex<scalar> 
template <class scalar>
  inline complex<scalar>& operator -= (complex<scalar>& a,
				       const complex<scalar>& b)
{
  a.re -= b.re;
  a.im -= b.im;
  return a;
}
//                            scalar -= complex<scalar> 
template <class scalar>
  inline scalar& operator -= (float &a, 
			      const complex<scalar>& b)
{
  assert(fabs(b.im) < small_epsilon);
  a-=b.re;
  return a;
}
template <class scalar>
  inline scalar& operator -= (double &a, 
			      const complex<scalar>& b)
{
  assert(fabs(b.im) < small_epsilon);
  a-=b.re;
  return a;
}
template <class scalar>
  inline scalar& operator -= (long double &a, 
			      const complex<scalar>& b)
{
  assert(fabs(b.im) < small_epsilon);
  a-=b.re;
  return a;
}
//                            complex<scalar> -= scalar
template <class scalar>
  inline complex<scalar>& operator -= (complex<scalar>& a,
				       const int &b)
{
  a.re -= b;
  return a;
}
template <class scalar>
  inline complex<scalar>& operator -= (complex<scalar>& a,
				       const long int &b)
{
  a.re -= b;
  return a;
}
template <class scalar>
  inline complex<scalar>& operator -= (complex<scalar>& a,
				       const float &b)
{
  a.re -= b;
  return a;
}
template <class scalar>
  inline complex<scalar>& operator -= (complex<scalar>& a,
				       const double &b)
{
  a.re -= b;
  return a;
}
template <class scalar>
  inline complex<scalar>& operator -= (complex<scalar>& a,
				       const long double &b)
{
  a.re -= b;
  return a;
}
//                            complex<scalar> *= complex<scalar> 
template <class scalar>
  inline complex<scalar>& operator *= (complex<scalar>& a,
				       const complex<scalar>& b)
{
	scalar t1=a.re*b.re, t2=a.im*b.im;
	a.im = ((a.re+a.im)*(b.re+b.im)-t1-t2);
	a.re = (t1 - t2);
	return a;
}
//                            scalar *= complex<scalar> 
template <class scalar>
  inline scalar& operator *= (float &a, 
			      const complex<scalar>& b)
{
  assert(fabs(b.im) < small_epsilon);
  a *= b.re;
  return a;
}
template <class scalar>
  inline scalar& operator *= (double &a, 
			      const complex<scalar>& b)
{
  assert(fabs(b.im) < small_epsilon);
  a *= b.re;
  return a;
}
template <class scalar>
  inline scalar& operator *= (long double &a, 
			      const complex<scalar>& b)
{
  assert(fabs(b.im) < small_epsilon);
  a *= b.re;
  return a;
}
//                            complex<scalar> *= scalar
template <class scalar>
  inline complex<scalar>& operator *= (complex<scalar>& a,
				       const int &b)
{
  a.re *= b;
  a.im *= b;
  return a;
}
template <class scalar>
  inline complex<scalar>& operator *= (complex<scalar>& a,
				       const long int &b)
{
  a.re *= b;
  a.im *= b;
  return a;
}
template <class scalar>
  inline complex<scalar>& operator *= (complex<scalar>& a,
				       const float &b)
{
  a.re *= b;
  a.im *= b;
  return a;
}
template <class scalar>
  inline complex<scalar>& operator *= (complex<scalar>& a,
				       const double &b)
{
  a.re *= b;
  a.im *= b;
  return a;
}
template <class scalar>
  inline complex<scalar>& operator *= (complex<scalar>& a,
				       const long double &b)
{
  a.re *= b;
  a.im *= b;
  return a;
}

//                            complex<scalar> /= complex<scalar> 
template <class scalar>
  inline complex<scalar>& operator /= (complex<scalar>& a,
				       const complex<scalar>& b)
{
  scalar t1, t2, temp;
  if (fabs(b.re) >= fabs(b.im))
    {
      t1   = b.im / b.re;
      t2   = b.re + b.im * t1;
      temp = (a.re + a.im * t1) / t2;
      a.im = (a.im - a.re * t1) / t2;
      a.re = temp;
    }
  else
    {
      t1   = b.re / b.im;
      t2   = b.re * t1 + b.im;
      temp = (a.re * t1 + a.im) / t2;
      a.im = (a.im * t1 - a.re) / t2;
      a.re = temp;
    }
  return a;
}
//                            scalar /= complex<scalar> 
template <class scalar>
  inline scalar& operator /= (float &a, 
			      const complex<scalar>& b)
{
  assert(fabs(b.im) < small_epsilon);
  a /= b.re;
  return a;
}
template <class scalar>
  inline scalar& operator /= (double &a, 
			      const complex<scalar>& b)
{
  assert(fabs(b.im) < small_epsilon);
  a /= b.re;
  return a;
}
template <class scalar>
  inline scalar& operator /= (long double &a, 
			      const complex<scalar>& b)
{
  assert(fabs(b.im) < small_epsilon);
  a /= b.re;
  return a;
}
//                            complex<scalar> /= scalar
template <class scalar>
  inline complex<scalar>& operator /= (complex<scalar>& a,
				       const int &b)
{
  a.re /= b;
  a.im /= b;
  return a;
}
template <class scalar>
  inline complex<scalar>& operator /= (complex<scalar>& a,
				       const long int &b)
{
  a.re /= b;
  a.im /= b;
  return a;
}
template <class scalar>
  inline complex<scalar>& operator /= (complex<scalar>& a,
				       const float &b)
{
  a.re /= b;
  a.im /= b;
  return a;
}
template <class scalar>
  inline complex<scalar>& operator /= (complex<scalar>& a,
				       const double &b)
{
  a.re /= b;
  a.im /= b;
  return a;
}
template <class scalar>
  inline complex<scalar>& operator /= (complex<scalar>& a,
				       const long double &b)
{
  a.re /= b;
  a.im /= b;
  return a;
}

//--------------------------------------------------------------
// unary operators ! + -
//--------------------------------------------------------------
template <class scalar>
  inline complex<scalar> operator! (const complex<scalar>& a) //complex conj.
{return complex<scalar>(a.re,-a.im);}
template <class scalar>
  inline complex<scalar> operator+ (const complex<scalar>& a) //complex conj.
{return complex<scalar>(a.re,a.im);}
template <class scalar>
  inline complex<scalar> operator- (const complex<scalar>& a) //complex conj.
{return complex<scalar>(-a.re,-a.im);}

//---------------------------------------------------------------
// binary operators + - * /
template <class scalar> 
  inline complex<scalar> operator+ (const complex<scalar> &a, 
				    const complex<scalar> &b)
{return complex<scalar>(a.re+b.re, a.im+b.im);}
//..
template <class scalar> 
  inline complex<scalar> operator+ (const complex<scalar> &a, 
				    const int &b)
{return complex<scalar>(a.re+b, a.im);}
template <class scalar> 
  inline complex<scalar> operator+ (const complex<scalar> &a, 
				    const long int &b)
{return complex<scalar>(a.re+b, a.im);}
template <class scalar> 
  inline complex<scalar> operator+ (const complex<scalar> &a, 
				    const float &b)
{return complex<scalar>(a.re+b, a.im);}
template <class scalar> 
  inline complex<scalar> operator+ (const complex<scalar> &a, 
				    const double &b)
{return complex<scalar>(a.re+b, a.im);}
template <class scalar> 
  inline complex<scalar> operator+ (const complex<scalar> &a, 
				    const long double &b)
{return complex<scalar>(a.re+b, a.im);}
//..
template <class scalar>
  inline complex<scalar> operator+ (const int  &a, 
				    const complex<scalar> &b)
{return complex<scalar>(a+b.re, b.im);}
template <class scalar>
  inline complex<scalar> operator+ (const long int  &a, 
				    const complex<scalar> &b)
{return complex<scalar>(a+b.re, b.im);}
template <class scalar>
  inline complex<scalar> operator+ (const float  &a, 
				    const complex<scalar> &b)
{return complex<scalar>(a+b.re, b.im);}
template <class scalar>
  inline complex<scalar> operator+ (const double  &a, 
				    const complex<scalar> &b)
{return complex<scalar>(a+b.re, b.im);}
template <class scalar>
  inline complex<scalar> operator+ (const long double  &a, 
				    const complex<scalar> &b)
{return complex<scalar>(a+b.re, b.im);}
//---------
template <class scalar>
  inline complex<scalar> operator- (const complex<scalar> &a, 
				    const complex<scalar> &b)
{return complex<scalar>(a.re-b.re, a.im-b.im);}
//..
template <class scalar> 
  inline complex<scalar> operator- (const complex<scalar> &a, 
				    const int  &b)
{return complex<scalar>(a.re-b, a.im);}
template <class scalar> 
  inline complex<scalar> operator- (const complex<scalar> &a, 
				    const long int  &b)
{return complex<scalar>(a.re-b, a.im);}
template <class scalar> 
  inline complex<scalar> operator- (const complex<scalar> &a, 
				    const float  &b)
{return complex<scalar>(a.re-b, a.im);}
template <class scalar> 
  inline complex<scalar> operator- (const complex<scalar> &a, 
				    const double  &b)
{return complex<scalar>(a.re-b, a.im);}
template <class scalar> 
  inline complex<scalar> operator- (const complex<scalar> &a, 
				    const long double  &b)
{return complex<scalar>(a.re-b, a.im);}
//..
template <class scalar> 
  inline complex<scalar> operator- (const int &a, 
				    const complex<scalar> &b)
{return complex<scalar>(a-b.re, -b.im);}
template <class scalar> 
  inline complex<scalar> operator- (const long int &a, 
				    const complex<scalar> &b)
{return complex<scalar>(a-b.re, -b.im);}
template <class scalar> 
  inline complex<scalar> operator- (const float &a, 
				    const complex<scalar> &b)
{return complex<scalar>(a-b.re, -b.im);}
template <class scalar> 
  inline complex<scalar> operator- (const double &a, 
				    const complex<scalar> &b)
{return complex<scalar>(a-b.re, -b.im);}
template <class scalar> 
  inline complex<scalar> operator- (const long double &a, 
				    const complex<scalar> &b)
{return complex<scalar>(a-b.re, -b.im);}
//---------
template <class scalar>
  inline complex<scalar> operator* (const complex<scalar>& a,
				    const complex<scalar>& b)
{
    scalar t1 = a.re * b.re;
    scalar t2 = a.im * b.im;
    return complex<scalar>(t1-t2,
			   (a.re+a.im)*
			   (b.re+b.im)-
			   (t1+t2)
			   ); // why this trick? imag = a.r*b.i + b.r*a.i
}
//...
template <class scalar>
  inline complex<scalar> operator* (const complex<scalar>& a,
				    const int &b)
{return complex<scalar>(a.re*b, a.im*b);}
template <class scalar>
  inline complex<scalar> operator* (const complex<scalar>& a,
				    const long int &b)
{return complex<scalar>(a.re*b, a.im*b);}
template <class scalar>
  inline complex<scalar> operator* (const complex<scalar>& a,
				    const float &b)
{return complex<scalar>(a.re*b, a.im*b);}
template <class scalar>
  inline complex<scalar> operator* (const complex<scalar>& a,
				    const double &b)
{return complex<scalar>(a.re*b, a.im*b);}
template <class scalar>
  inline complex<scalar> operator* (const complex<scalar>& a,
				    const long double &b)
{return complex<scalar>(a.re*b, a.im*b);}
//...
template <class scalar>
  inline complex<scalar> operator* (const int &a,
				    const complex<scalar>& b)
{return complex<scalar>(a*b.re, a*b.im);}
template <class scalar>
  inline complex<scalar> operator* (const long int &a,
				    const complex<scalar>& b)
{return complex<scalar>(a*b.re, a*b.im);}
template <class scalar>
  inline complex<scalar> operator* (const float &a,
				    const complex<scalar>& b)
{return complex<scalar>(a*b.re, a*b.im);}
template <class scalar>
  inline complex<scalar> operator* (const double &a,
				    const complex<scalar>& b)
{return complex<scalar>(a*b.re, a*b.im);}
template <class scalar>
  inline complex<scalar> operator* (const long double &a,
				    const complex<scalar>& b)
{return complex<scalar>(a*b.re, a*b.im);}
//------------
template <class scalar>
  inline complex<scalar> operator/ (const complex<scalar>& a,
				    const complex<scalar>& b)
{
  scalar t1, t2;
  if (fabs(b.re) >= fabs(b.im))
    {
      t1 = b.im / b.re;
      t2 = b.re + b.im * t1;
      return complex<scalar>((a.re+a.im*t1)/t2, 
			     (a.im-a.re*t1)/t2);
    }
  else
    {
      t1 = b.re / b.im;
      t2 = b.re*t1 + b.im;
      return complex<scalar>((a.re*t1 + a.im) /t2, 
			     (a.im*t1 - a.re) /t2);
    }
}
//..
template <class scalar>
inline complex<scalar> operator/ (const int &a,
				  const complex<scalar>& b)
{return complex<scalar>(a,0)/b;}
template <class scalar>
inline complex<scalar> operator/ (const long int &a,
				  const complex<scalar>& b)
{return complex<scalar>(a,0)/b;}
template <class scalar>
inline complex<scalar> operator/ (const float &a,
				  const complex<scalar>& b)
{return complex<scalar>(a,0)/b;}
template <class scalar>
inline complex<scalar> operator/ (const double &a,
				  const complex<scalar>& b)
{return complex<scalar>(a,0)/b;}
template <class scalar>
inline complex<scalar> operator/ (const long double &a,
				  const complex<scalar>& b)
{return complex<scalar>(a,0)/b;}
//..
template <class scalar>
  inline complex<scalar> operator/ (const complex<scalar>& a,
				    const int &b)
{return complex<scalar>(a.re/b,a.im/b);}
template <class scalar>
  inline complex<scalar> operator/ (const complex<scalar>& a,
				    const long int &b)
{return complex<scalar>(a.re/b,a.im/b);}
template <class scalar>
  inline complex<scalar> operator/ (const complex<scalar>& a,
				    const float &b)
{return complex<scalar>(a.re/b,a.im/b);}
template <class scalar>
  inline complex<scalar> operator/ (const complex<scalar>& a,
				    const double &b)
{return complex<scalar>(a.re/b,a.im/b);}
template <class scalar>
  inline complex<scalar> operator/ (const complex<scalar>& a,
				    const long double &b)
{return complex<scalar>(a.re/b,a.im/b);}


//------------------------------------------------------------------
// stream operators
//------------------------------------------------------------------
template <class scalar>
  std::ostream& operator << (std::ostream &stream, const complex<scalar> &a)
{
  stream<<"("<<a.re<<","<<a.im<<")";
  return stream;
}
template <class scalar>
  std::istream& operator >> (std::istream &stream, complex<scalar> &a)
{
  stream>>a.re>>a.im;
  return stream;
}
//---------------------------------------------------------------------
// bool operators == !=
//---------------------------------------------------------------------
template <class scalar>
  inline bool operator == (const complex<scalar>& a,
			   const complex<scalar>& b)
{
  return (a.re==b.re) && (a.im==b.im);
}
//...
template <class scalar>
  inline bool operator == (const int &a,
			   const complex<scalar>& b)
{
  return (a==b.re) && (b.im<small_epsilon);
}
template <class scalar>
  inline bool operator == (const long int &a,
			   const complex<scalar>& b)
{
  return (a==b.re) && (b.im<small_epsilon);
}
template <class scalar>
  inline bool operator == (const float &a,
			   const complex<scalar>& b)
{
  return (a==b.re) && (b.im<small_epsilon);
}
template <class scalar>
  inline bool operator == (const double &a,
			   const complex<scalar>& b)
{
  return (a==b.re) && (b.im<small_epsilon);
}
template <class scalar>
  inline bool operator == (const long double &a,
			   const complex<scalar>& b)
{
  return (a==b.re) && (b.im<small_epsilon);
}
//...
template <class scalar>
  inline bool operator == (const complex<scalar>& a,
			   const int &b)
{
  return (a.re==b) && (a.im<small_epsilon);
}
template <class scalar>
  inline bool operator == (const complex<scalar>& a,
			   const long int &b)
{
  return (a.re==b) && (a.im<small_epsilon);
}
template <class scalar>
  inline bool operator == (const complex<scalar>& a,
			   const float &b)
{
  return (a.re==b) && (a.im<small_epsilon);
}
template <class scalar>
  inline bool operator == (const complex<scalar>& a,
			   const double &b)
{
  return (a.re==b) && (a.im<small_epsilon);
}
template <class scalar>
  inline bool operator == (const complex<scalar>& a,
			   const long double &b)
{
  return (a.re==b) && (a.im<small_epsilon);
}
//...
template <class scalar>
  inline bool operator != (const complex<scalar>& a,
			   const complex<scalar>& b)
{
  return (a.re!=b.re) || (a.im!=b.im);
}
//...
template <class scalar>
  inline bool operator != (const int &a,
			   const complex<scalar>& b)
{
  return (a!=b.re) || (b.im>=small_epsilon);
}
template <class scalar>
  inline bool operator != (const long int &a,
			   const complex<scalar>& b)
{
  return (a!=b.re) || (b.im>=small_epsilon);
}
template <class scalar>
  inline bool operator != (const float &a,
			   const complex<scalar>& b)
{
  return (a!=b.re) || (b.im>=small_epsilon);
}
template <class scalar>
  inline bool operator != (const double &a,
			   const complex<scalar>& b)
{
  return (a!=b.re) || (b.im>=small_epsilon);
}
template <class scalar>
  inline bool operator != (const long double &a,
			   const complex<scalar>& b)
{
  return (a!=b.re) || (b.im>=small_epsilon);
}
//...
template <class scalar>
  inline bool operator != (const complex<scalar>& a,
			   const int &b)
{
  return (a.re!=b) || (a.im>=small_epsilon);
}
template <class scalar>
  inline bool operator != (const complex<scalar>& a,
			   const long int &b)
{
  return (a.re!=b) || (a.im>=small_epsilon);
}
template <class scalar>
  inline bool operator != (const complex<scalar>& a,
			   const float &b)
{
  return (a.re!=b) || (a.im>=small_epsilon);
}
template <class scalar>
  inline bool operator != (const complex<scalar>& a,
			   const double &b)
{
  return (a.re!=b) || (a.im>=small_epsilon);
}
template <class scalar>
  inline bool operator != (const complex<scalar>& a,
			   const long double &b)
{
  return (a.re!=b) || (a.im>=small_epsilon);
}
//---------------------------------------------------------------------
// bool operators >= <=
//---------------------------------------------------------------------
template <class scalar>
  inline bool operator >= (const complex<scalar>& a,
			   const complex<scalar>& b)
{
  return (abs(a) >= abs(b));
}
//...
template <class scalar>
  inline bool operator >= (const int &a,
			   const complex<scalar>& b)
{
  return (a >= abs(b));
}
template <class scalar>
  inline bool operator >= (const long int &a,
			   const complex<scalar>& b)
{
  return (a >= abs(b));
}
template <class scalar>
  inline bool operator >= (const float &a,
			   const complex<scalar>& b)
{
  return (a >= abs(b));
}
template <class scalar>
  inline bool operator >= (const double &a,
			   const complex<scalar>& b)
{
  return (a >= abs(b));
}
template <class scalar>
  inline bool operator >= (const long double &a,
			   const complex<scalar>& b)
{
  return (a >= abs(b));
}
//...
template <class scalar>
  inline bool operator >= (const complex<scalar>& a,
			   const int &b)
{
  return (abs(a) >= b);
}
template <class scalar>
  inline bool operator >= (const complex<scalar>& a,
			   const long int &b)
{
  return (abs(a) >= b);
}
template <class scalar>
  inline bool operator >= (const complex<scalar>& a,
			   const float &b)
{
  return (abs(a) >= b);
}
template <class scalar>
  inline bool operator >= (const complex<scalar>& a,
			   const double &b)
{
  return (abs(a) >= b);
}
template <class scalar>
  inline bool operator >= (const complex<scalar>& a,
			   const long double &b)
{
  return (abs(a) >= b);
}
//...
template <class scalar>
  inline bool operator <= (const complex<scalar>& a,
			   const complex<scalar>& b)
{
  return (abs(a) <= abs(b));
}
//...
template <class scalar>
  inline bool operator <= (const int &a,
			   const complex<scalar>& b)
{
  return (a <= abs(b));
}
template <class scalar>
  inline bool operator <= (const long int &a,
			   const complex<scalar>& b)
{
  return (a <= abs(b));
}
template <class scalar>
  inline bool operator <= (const float &a,
			   const complex<scalar>& b)
{
  return (a <= abs(b));
}
template <class scalar>
  inline bool operator <= (const double &a,
			   const complex<scalar>& b)
{
  return (a <= abs(b));
}
template <class scalar>
  inline bool operator <= (const long double &a,
			   const complex<scalar>& b)
{
  return (a <= abs(b));
}
//...
template <class scalar>
  inline bool operator <= (const complex<scalar>& a,
			   const int &b)
{
  return (abs(a) <= b);
}
template <class scalar>
  inline bool operator <= (const complex<scalar>& a,
			   const long int &b)
{
  return (abs(a) <= b);
}
template <class scalar>
  inline bool operator <= (const complex<scalar>& a,
			   const float &b)
{
  return (abs(a) <= b);
}
template <class scalar>
  inline bool operator <= (const complex<scalar>& a,
			   const double &b)
{
  return (abs(a) <= b);
}
template <class scalar>
  inline bool operator <= (const complex<scalar>& a,
			   const long double &b)
{
  return (abs(a) <= b);
}
//---------------------------------------------------------------------
// bool operators > <
//---------------------------------------------------------------------
template <class scalar>
  inline bool operator > (const complex<scalar>& a,
			   const complex<scalar>& b)
{
  return (abs(a) > abs(b));
}
//...
template <class scalar>
  inline bool operator > (const int &a,
			   const complex<scalar>& b)
{
  return (a > abs(b));
}
template <class scalar>
  inline bool operator > (const long int &a,
			   const complex<scalar>& b)
{
  return (a > abs(b));
}
template <class scalar>
  inline bool operator > (const float &a,
			   const complex<scalar>& b)
{
  return (a > abs(b));
}
template <class scalar>
  inline bool operator > (const double &a,
			   const complex<scalar>& b)
{
  return (a > abs(b));
}
template <class scalar>
  inline bool operator > (const long double &a,
			   const complex<scalar>& b)
{
  return (a > abs(b));
}
//...
template <class scalar>
  inline bool operator > (const complex<scalar>& a,
			   const int &b)
{
  return (abs(a) > b);
}
template <class scalar>
  inline bool operator > (const complex<scalar>& a,
			   const long int &b)
{
  return (abs(a) > b);
}
template <class scalar>
  inline bool operator > (const complex<scalar>& a,
			   const float &b)
{
  return (abs(a) > b);
}
template <class scalar>
  inline bool operator > (const complex<scalar>& a,
			   const double &b)
{
  return (abs(a) > b);
}
template <class scalar>
  inline bool operator > (const complex<scalar>& a,
			   const long double &b)
{
  return (abs(a) > b);
}
//...
template <class scalar>
  inline bool operator < (const complex<scalar>& a,
			   const complex<scalar>& b)
{
  return (abs(a) < abs(b));
}
//...
template <class scalar>
  inline bool operator < (const int &a,
			   const complex<scalar>& b)
{
  return (a < abs(b));
}
template <class scalar>
  inline bool operator < (const long int &a,
			   const complex<scalar>& b)
{
  return (a < abs(b));
}
template <class scalar>
  inline bool operator < (const float &a,
			   const complex<scalar>& b)
{
  return (a < abs(b));
}
template <class scalar>
  inline bool operator < (const double &a,
			   const complex<scalar>& b)
{
  return (a < abs(b));
}
template <class scalar>
  inline bool operator < (const long double &a,
			   const complex<scalar>& b)
{
  return (a < abs(b));
}
//...
template <class scalar>
  inline bool operator < (const complex<scalar>& a,
			   const int &b)
{
  return (abs(a) < b);
}
template <class scalar>
  inline bool operator < (const complex<scalar>& a,
			   const long int &b)
{
  return (abs(a) < b);
}
template <class scalar>
  inline bool operator < (const complex<scalar>& a,
			   const float &b)
{
  return (abs(a) < b);
}
template <class scalar>
  inline bool operator < (const complex<scalar>& a,
			   const double &b)
{
  return (abs(a) < b);
}
template <class scalar>
  inline bool operator < (const complex<scalar>& a,
			   const long double &b)
{
  return (abs(a) < b);
}

//---------------------------------------------------------------------
// functions
//---------------------------------------------------------------------
//-----------------------------------------------------
// real(), imag(), conj(), abs(), arg(), phase(), Arg()
//-----------------------------------------------------
template <class scalar> 
  inline const scalar          real(const complex<scalar>& a) 
{return a.re;}
template <class scalar> 
  inline const scalar          imag(const complex<scalar>& a) 
{return a.im;}
template <class scalar> 
  inline const complex<scalar> conj(const complex<scalar>& a) 
{return complex<scalar>(a.re,-a.im);}
template <class scalar> 
  inline const scalar           abs(const complex<scalar>& a)
{
  if (a.im == 0) return fabs(a.re);
  if (a.re == 0) return fabs(a.im);
  scalar rr = fabs(a.re);
  scalar ri = fabs(a.im);
  return (rr >= ri) ?
    (rr * sqrt (1 + (ri*ri/(rr*rr)))) :
    (ri * sqrt (1 + (rr*rr/(ri*ri)))) ;
}
template <class scalar> 
  inline const scalar   arg(const complex<scalar>& a) 
{
  return atan2(a.im,a.re);
}
template <class scalar> 
  inline const scalar phase(const complex<scalar>& a) 
{
  return atan2(a.im,a.re);
}
template <class scalar> 
  inline const scalar Arg(const complex<scalar>& a) 
{
  return atan2(a.im,a.re);
}
//-----------------------------------------------------------------
// sqr(), 
// complex^int 
// complex^(long int) 
// complex^scalar 
// complex^complex
// pow(complex,int)  
// pow(complex,long int)  
// pow(complex,scalar)  
// pow(complex,complex)  
//
// results checked with mathematica: fun(0.5+i*0.4)
//-------
template <class scalar> 
  inline complex<scalar> sqr(complex<scalar> z) {return z*z;}

//already defined in physfun.cxx
//template <class scalar>
//inline scalar sqr(scalar z){return z*z;}

template <class scalar> 
  complex<scalar> operator ^ (const complex<scalar>& a, 
			      const int n)
{
  complex<scalar> c(1,0);
  if (n==0) return c;
  if (n>0) 
    {
      for (int i=0;i<n;i++) 
	{
	  c*=a;
	}
      
    }
  else 
    {
      for (int j=0;j>n;j--) 
	{
	  c*=a;
	}
    }
  if (n>0) 
    {
      return c;
    }
  else 
    {
      return 1/c;
    }
}
template <class scalar> 
  complex<scalar> operator ^ (const complex<scalar>& a, 
			      const long int n)
{
  complex<scalar> c(1,0);
  if (n==0) return c;
  if (n>0) 
    {
      for (int i=0;i<n;i++) 
	{
	  c*=a;
	}
      
    }
  else 
    {
      for (int j=0;j>n;j--) 
	{
	  c*=a;
	}
    }
  if (n>0) 
    {
      return c;
    }
  else 
    {
      return 1/c;
    }
}

template <class scalar> 
  complex<scalar> operator ^ (const complex<scalar>& a,
			      const float &r)
{
  return exp(r*log(a));
}
template <class scalar> 
  complex<scalar> operator ^ (const complex<scalar>& a,
			      const double &r)
{
  return exp(r*log(a));
}
template <class scalar> 
  complex<scalar> operator ^ (const complex<scalar>& a,
			      const long double &r)
{
  return exp(r*log(a));
}
template <class scalar> 
  complex<scalar> operator ^ (const complex<scalar>& a,
			      const complex<scalar>& b)
{
  return  exp(b*log(a));
}
template <class scalar> 
  complex<scalar> pow(const complex<scalar>& a,
		      const int& i)
{
  return a^i;
}
template <class scalar> 
  complex<scalar> pow(const complex<scalar>& a,
		      const long int& i)
{
  return a^i;
}
template <class scalar> 
  complex<scalar> pow(const complex<scalar>& a,
		      const float &r)
{
  return a^r;
}
template <class scalar> 
  complex<scalar> pow(const complex<scalar>& a,
		      const double &r)
{
  return a^r;
}
template <class scalar> 
  complex<scalar> pow(const complex<scalar>& a,
		      const long double &r)
{
  return a^r;
}
template <class scalar> 
  complex<scalar> pow(const complex<scalar>& a,
		      const complex<scalar>& b)
{
  return a^b;
}
//----------------------------------------------------------------------
// sqrt(z)
// results checked with mathematica: fun(0.5+i*0.4)
//----------------------------------------------------------------------
template <class scalar>
  complex<scalar> sqrt(const complex<scalar>& a)
{
  if ((a.re>=0) && (fabs(a.im)<small_epsilon))
    {
      return complex<scalar>(sqrt(a.re),0);
    }
  

  scalar rr = fabs(a.re);
  scalar ii = fabs(a.im);
  scalar w = (rr >= ii) ?
    sqrt (rr/2 * (    1 + sqrt (1 + sqr(a.im/a.re)))):
    sqrt (ii/2 * (rr/ii + sqrt (1 + sqr(a.re/a.im))));
  complex<scalar> c;
  if (a.re >= 0)
    {
      c.re = w;
      c.im = a.imag()/(2*w);
    }
  else
    {
      c.re = ii / (2*w);
      c.im = (a.im >= 0) ? w : -w;
    }
  return (c.re>=0) ? c : -c;
}
template <class scalar>
  complex<scalar> root (const complex<scalar>& z, int n, int k)
{
  scalar c=exp(log(abs(z))/n);
  scalar t=(Arg(z)+2*PI*k)/n;
  return complex<scalar>(c*cos(t), c*sin(t));
}
//----------------------------------------------------------------------
// exp() log()
//----------------------------------------------------------------------
template <class scalar>
  complex<scalar> exp (const complex<scalar>& a)
{
	scalar t=exp(a.re);
	return complex<scalar>(t*cos(a.im), t*sin(a.im));
}
template <class scalar>
  complex<scalar> log (const complex<scalar>& a)
{  
  assert(abs(a)!=0);
  return complex<scalar>(log(abs(a)),Arg(a));
}
//----------------------------------------------------------------------
// sin() cos() tan() cot()
// results checked with mathematica: fun(0.5+i*0.4)
//----------------------------------------------------------------------
template <class scalar>
  complex<scalar> sin (const complex<scalar>& a)
{
  return complex<scalar>(sin(a.re)*cosh(a.im), 
			 cos(a.re)*sinh(a.im));
}

template <class scalar>
  complex<scalar> cos (const complex<scalar>& a)
{
  return complex<scalar> (cos(a.re)*cosh(a.im), 
			  -sin(a.re)*sinh(a.im));
}

template <class scalar>
  complex<scalar> tan (const complex<scalar>& a)
{
  return sin(a)/cos(a);
}

template <class scalar>
  complex<scalar> cot (const complex<scalar>& a)
{
	return cos(a)/sin(a);
}
template <class scalar>
  complex<scalar> sec (const complex<scalar>& a)
{
	return 1/cos(a);
}
template <class scalar>
  complex<scalar> csc (const complex<scalar>& a)
{
	return 1/sin(a);
}
//---------------------------------------------------------------------
// sinh cosh tanh coth sech csch
// results checked with mathematica: fun(0.5+i*0.4)
//---------------------------------------------------------------------
template <class scalar>
  complex<scalar> sinh (const complex<scalar>& a)
{
  return complex<scalar>(sinh(a.re)*cos(a.im), 
			 cosh(a.re)*sin(a.im));
}
template <class scalar>
  complex<scalar> cosh (const complex<scalar>& a)
{
  return complex<scalar>(cosh(a.re)*cos(a.im), 
			 sinh(a.re)*sin(a.im));
}
template <class scalar>
  complex<scalar> tanh (const complex<scalar>& a)
{
  return sinh(a)/cosh(a);
}
template <class scalar>
  complex<scalar> coth (const complex<scalar>& a)
{
  return cosh(a)/sinh(a);
}
template <class scalar>
  complex<scalar> sech (const complex<scalar>& a)
{
  return 1/cosh(a);
}
template <class scalar>
  complex<scalar> csch (const complex<scalar>& a)
{
  return 1/sinh(a);
}
//---------------------------------------------------------------------
// asin acos atan acot asec acsc
// results checked with mathematica: fun(0.5+i*0.4)
//---------------------------------------------------------------------  
template <class scalar>
  complex<scalar> asin(const complex<scalar> &a)
{
  const complex<scalar> ImUnit(0,1);
  return -ImUnit * log(ImUnit*a + sqrt(1-sqr(a)));
}
template <class scalar>
  complex<scalar> acos(const complex<scalar> &a)
{
  const complex<scalar> ImUnit(0,1);
  return -ImUnit * log(a + ImUnit*sqrt(1-sqr(a)));
}
template <class scalar>
  complex<scalar> atan(const complex<scalar> &a)
{
  const complex<scalar> ImUnit(0,1);
  return  ImUnit/2 * log((ImUnit+a)/(ImUnit-a));
}
template <class scalar>
  complex<scalar> acot(const complex<scalar> &a)
{
  const complex<scalar> ImUnit(0,1);
  return ImUnit/2 * log((a-ImUnit)/(a+ImUnit));
}
template <class scalar>
  complex<scalar> asec(const complex<scalar> &a)
{
  return acos(1/a);
}
template <class scalar>
  complex<scalar> acsc(const complex<scalar> &a)
{
  return asin(1/a);
}
//--------------------------------------------------------------------
// asinh acosh atanh acoth asech acsch
// results checked with mathematica: fun(0.5+i*0.4)
//--------------------------------------------------------------------
template <class scalar>
  complex<scalar> asinh(const complex<scalar> &a)
{
  return log(a + sqrt(sqr(a)+1));
}
template <class scalar>
  complex<scalar> acosh(const complex<scalar> &a)
{
  return log(a + sqrt(sqr(a)-1));
}
template <class scalar>
  complex<scalar> atanh(const complex<scalar> &a)
{
  return log((1+a)/(1-a))/2;
}
template <class scalar>
  complex<scalar> acoth(const complex<scalar> &a)
{
  return log((a+1)/(a-1))/2;
}
template <class scalar>
  complex<scalar> asech(const complex<scalar> &a)
{
  return acosh(1/a);
}
template <class scalar>
  complex<scalar> acsch(const complex<scalar> &a)
{
  return asinh(1/a);
}
//---------------------------------------------------------------------
// divers
//---------------------------------------------------------------------
template <class scalar>
  complex<scalar> Polar(scalar a,scalar b)
{
  return complex<scalar>(a*cos(b),a*sin(b));
}
//----------------------------------------------------------------------
// solve Equations z^2 + b*z + c = 0
//----------------------------------------------------------------------
template <class scalar>
  void Solve2 (complex<scalar>* z, 
	       const complex<scalar>& b, 
	       const complex<scalar>& c)
{  
  complex<scalar> t = sqrt(sqr(b)-4*c);
  complex<scalar> p = !b * t;
  complex<scalar> q = (p.re >= 0) ? 
    (-(b + t) / 2) : (-(b - t) / 2);
  z[0] = q;
  z[1] = c/q;
}
template <class scalar>
  complex<scalar> Solve2(const complex<scalar>& b, 
			 const complex<scalar>& c, 
			 int RootNumber)
{
  assert((RootNumber >=0) && (RootNumber <= 1));
  
  complex<scalar> z[2];
  Solve2(z,b,c);
  return z[RootNumber];
}
//----------------------------------------------------------------------
// solve Equations z^3 + a2*z^2 + a1*z + a0 = 0
//----------------------------------------------------------------------
template <class scalar>
  void Solve3(complex<scalar>* z, 
	      const complex<scalar>& a2, 
	      const complex<scalar>& a1, 
	      const complex<scalar>& a0)
{
  const complex<scalar> ImUnit(0,1);
  complex<scalar> q, r, t, a, b, zero(0,0);
  q = (sqr(a2) - 3*a1) / 9;
  r = (2*(a2^3)-9*a2*a1+27*a0)/54;
  t = sqrt(sqr(r) - (q^3));
  a = ((!r * t) >=0.0) ? 
    -((r+t)^(1.0/3)) : -((r-t)^(1.0/3));
  b = ((a == zero) ? zero : (q/a));
  z[0] = -(a+b)/2-a2/3+ImUnit*sqrt(3)*(a-b)/2;
  z[1] = -(a+b)/2-a2/3-ImUnit*sqrt(3)*(a-b)/2;
  z[2] = a + b - a2/scalar(3);
}
template <class scalar>
  complex<scalar> Solve3(const complex<scalar>& a2, 
			 const complex<scalar>& a1, 
			 const complex<scalar>& a0, 
			 int RootNumber)
{
  assert((RootNumber >= 0) && (RootNumber <= 2));

    
  complex<scalar> z[3];
  Solve3 (z,a2,a1,a0);
  return z[RootNumber];
}
//----------------------------------------------------------------------
// solve Equations z^4 + a3*z^3 + a2*z^2 + a1*z + a0 = 0
//----------------------------------------------------------------------
template <class scalar>
  void Solve4(complex<scalar>* z, 
	      const complex<scalar>& a3, 
	      const complex<scalar>& a2, 
	      const complex<scalar>& a1, 
	      const complex<scalar>& a0)
{
  complex<scalar> u[3], t1, t2, t;
  Solve3 (u,-a2,(a1*a3 - 4*a0),
	  -(sqr(a1)+a0*sqr(a3)-4*a0*a2));
  t = *u;
  t1 = sqrt(sqr(a3)/4 - a2 + t);
  t2 = sqrt(sqr(t) /4 - a0);
  Solve2 (      z, (a3/2 - t1), (t/2 + t2));
  Solve2 (&(z[2]), (a3/2 + t1), (t/2 - t2));
}
template <class scalar>
  complex<scalar> Solve4(const complex<scalar>& a3, 
			 const complex<scalar>& a2, 
			 const complex<scalar>& a1, 
			 const complex<scalar>& a0, 
			 int RootNumber)
{
  assert((RootNumber >= 0) && (RootNumber <= 3));

  complex<scalar> z[4];
  Solve4 (z,a3,a2,a1,a0);
  return z[RootNumber];
}

#endif  // __COMPLEX_CXX__ //


// $Log: complex.hxx,v $
// Revision 1.2  2003/05/01 13:11:48  carsten
// headerfiles without .h
//
// Revision 1.1  2002/04/12 11:56:14  carsten
// moved from indracorr
//
// Revision 1.1  2001/10/09 19:50:49  carsten
// added
//
// Revision 1.4  2001/10/09 19:48:22  carsten
// added comments
//
