/**
 * @author Manuel Zambrana <zambrana@kph.uni-mainz.de> and Dmitry Khaneftd <khaneftd@kph.uni-mainz.de>
 *
 * Mainz, May 2011
**/

//c/c++
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

//mz
#include "mzparameters.h"
#include "mzfunctions.h" 

//ranlux
#include "ranlxs.h"
#include "ranlxd.h"

void ranlxs(float r[], int n);
void ranlxd(double r[], int n);

void rlxs_init(int level, int seed);
void rlxd_init(int level, int seed);

using namespace std;

double mzdelta(int i, int k)
{
  /**
   * function mzdelta
   *
   * input: i,k integers
   * output: kroneker delta(i,k)
   */


  double d;

  if( k==i )
  {
    d=1.0;
  }
  else{
    d=0.0;
  }

  return d;
}

double mzvscalar(int n, double* p, double* q)
{
  /**
   * function mzvscalar
   *
   * input: int n, 4-vector p, 4-vector q
   *
   * output:
   *
   *    n=2 -> returns p[1]*q[1] + p[2]*q[2]
   *    n=3 -> returns p[1]*q[1] + p[2]*q[2] + p[3]*q[3]
   *    n=4 -> returns p[0]*q[0] -p[1]*q[1] -p[2]*q[2] -p[3]*q[3]
   */

  double s;


  if( !(n==2||n==3||n==4) ){
    printf("ERROR: function mzvscalar called with flag n out of range \n");
    printf("    => n should take the value 2,3 or 4 \n");
    printf("\n\n");
    exit(1);
  }


  if (n==2) {
    s = p[1]*q[1] + p[2]*q[2];
  }

  if (n==3) {
    s = p[1]*q[1] + p[2]*q[2] + p[3]*q[3];
  }

  if (n==4) {
    s = p[0]*q[0] -1.0*p[1]*q[1] -1.0*p[2]*q[2] -1.0*p[3]*q[3];
  }

  return s;
}

double mzvmod2(int n, double* p)
{
  /**
   * function mzvmod2
   *
   * input: int n, 4-vector p
   *
   * output:
   *
   *    n=2 -> returns p[1]*p[1] + p[2]*p[2]
   *    n=3 -> returns p[1]*p[1] + p[2]*p[2] + p[3]*p[3]
   *    n=4 -> returns p[0]*p[0]  -p[1]*p[1] -p[2]*p[2] -p[3]*p[3]
   */

  double m2;


  if( !(n==2||n==3||n==4) ){
    printf("ERROR: function mzvmod2 called with flag n out of range \n");
    printf("    => n should take the value 2,3 or 4 \n");
    printf("\n\n");
    exit(1);
  }


  m2 = mzvscalar(n,p,p);

  return m2;
}

void mzboost(int flag, double* p, double* q, double* q_prime)
{
  /**
   * function mzboost
   *
   * input:   flag=0 lab->CM, flag=1 CM->lab
   *          p 4-mom of CM particle (M!=0) in lab frame 
   *          q 4-mom of test particle in lab frame (flag=0) / CM frame (flag=1)
   *
   * output:  q_prime 4-mom of test particle in the CM frame (flag=0)  / lab frame (flag=1) 
   */

  int i,k;

  double m;
  double gamma;
  double p_uni[4];

  double L[4][4];
  double LI[4][4];

  double mztemp;



  //computing matrix L(p)

  m=pow(mzvmod2(4,p),0.5);
  gamma=p[0]/m;


  for (i=1; i<=3; i++) {
    p_uni[i]=p[i]/pow(mzvmod2(3,p),0.5);
  }


  L[0][0]=gamma;


  for (i=1; i<=3; i++) {
    L[0][i] = p_uni[i] * pow((gamma*gamma - 1.0),0.5);
    L[i][0] = L[0][i];
  }


  for (i=1; i<=3; i++) {
    for (k=1; k<=3; k++) {
      L[i][k] = mzdelta(i,k) + (gamma -1.0)*p_uni[i]*p_uni[k];
    }
  }




  //inverting matrix L(p)

  LI[0][0]=L[0][0];

  for (i=1; i<=3; i++) {
    LI[0][i] = -1.0*L[i][0];
    LI[i][0] = -1.0*L[0][i];
  }

  for (i=1; i<=3; i++) {
    for (k=1; k<=3; k++) {
      LI[i][k] = L[k][i];
    }
  }



  //boosting 4-mom q

  for (i=0; i<=3; i++) {
    mztemp = 0.0;
    for (k=0; k<=3; k++) {
      if (flag==0) mztemp = mztemp + LI[i][k]*q[k];
      if (flag==1) mztemp = mztemp + L[i][k]*q[k];
    }
    q_prime[i] =mztemp;
  }

}

double mzenergy(double m, double p_x, double p_y, double p_z)
{
  /**
   *input: mass m, 3-mom components px, py, pz of a particle
   *
   *output: energy of the particle
   *
   */

  double E;

  E=pow(m*m +p_x*p_x +p_y*p_y +p_z*p_z,0.5);

  return E;
}

double mzpolar(double* p)
{
  /**
   * function mzpolar
   *
   * input: 4-mom p
   * output: polar angle theta in range [0,180]
   */

  double PI=acos(-1.0);

  double theta;

  double pmod;
  double costheta;

  pmod=pow(mzvmod2(3,p),0.5);
  
  if(pmod==0.0){
    cerr << "WARNING: mzpolar called with p[1]=p[2]=p[3]=0.0 " << endl;
    return 9999.;
  }

  costheta  = p[3]/pmod;

  if(costheta > 1.0) costheta=1.0;
  if(costheta < -1.0) costheta=-1.0;

  theta = acos(costheta) * 180.0 / PI ;


  return theta;
}


double mzazimuthal(double* p)
{
  /**
   * function mzazimuthal
   *
   * input: 4-mom p
   * output: azimuthal angle phi in range [0,180]
   */

 double PI=acos(-1.0);

 double phi;

 if ( (p[1]==0.) && (p[2]==0.0) ) {
   cerr << "WARNING: mzazimuthal called with p[1]=p[2]=0.0 " << endl;
   return 9999.;
 }


 phi=atan2(p[2],p[1])*180.0/PI;


 if (phi<0.0) phi = phi + 360.0;

 return phi;
}

double mzcospolar(double* p)
{
  /**
   * function mzcospolar
   *
   * input: 4-mom p
   * output: cos of polar angle theta
   */

  double costheta;
  
  double pmod;

  pmod=pow(mzvmod2(3,p),0.5);
  
  if(pmod==0.0){
    cerr << "WARNING: mzcospolar called with p[1]=p[2]=p[3]=0.0 " << endl;
    return 9999.;
  }

  costheta  = p[3]/pmod;

  return costheta;
}

double mzangle(double* p, double* q)
{
  /**
   * function mzangle
   *
   * input: 4-mom p, 4-mom q
   * output: angle between 3-dim components of p, q in range [0,180]   
   */


 double PI=acos(-1.0);

 double alpha;

 double scalar, pmod, qmod, cosalpha;

 pmod=pow(mzvmod2(3,p),0.5);
 qmod=pow(mzvmod2(3,q),0.5);

 scalar=mzvscalar(3,p,q);

 if(pmod==0.0){
    cerr << "WARNING: mzangle called with p[1]=p[2]=p[3]=0.0 " << endl;
    return 9999.;
 }

 if(qmod==0.0){
    cerr << "WARNING: mzangle called with q[1]=q[2]=q[3]=0.0 " << endl;
    return 9999.;
 }


 cosalpha=scalar/(pmod*qmod);

 if(cosalpha > 1.0) cosalpha=1.0;
 if(cosalpha < -1.0) cosalpha=-1.0;

 alpha=acos(cosalpha) * 180.0 / PI;


 return alpha;
}

double mzrnd(double a, double b){
  /**
   * function mzrnd
   *
   * arguments:
   *
   *   a = inf[a,b]
   *   b = sup[a,b]
   *
   * returns:
   *
   *   random number in interval [a,b]
   */


  double y;

  int dim=1;
  double rvec[dim];
  double x;

  //check arguments
  if(b<=a){
    printf("ERROR: function mzrnd called with arguments b<=a \n");
    printf("    => argument b should be larger than argument a \n");
    printf("\n\n");
    exit(1);
  }

  ranlxd(rvec,dim);  //generate random number in interval [0,1]
  x=rvec[0];
  y=(b-a)*x + a; 

  return y;
}

double mzcomplexmod(double Re_z, double Im_z){
  /**
   *  function mzcomplexmod
   *
   *  arguments:
   *
   *   Re_z = real part of complex number z
   *   Im_z = imaginary part of complex number z
   *
   *  returns:
   *
   *   modulus of complex number z
   */

  double y;

  y=pow(Re_z*Re_z + Im_z*Im_z,0.5);

  return y;
}

double mz_E_to_s(double E){
  /**
   * function mz_E_to_s
   *
   * arguments:
   *
   *   E: pbar energy in lab (GeV)
   *
   *
   * returns:
   *
   *   cm-energy square s of pbar p system
   *
   */

  double s;

  double M=mp;

  double p_p[4];           //lab frame: 4-mom p    (proton target)
  double pbar_p[4];        //lab frame: 4-mom pbar (anti-proton beam)
  double pbarp_p[4];       //lab frame: 4-mom pbarpsystem
  double P;                //lab frame: pz pbar


  //check argument range
  if(E<M){
    printf("ERROR: function mz_E_to_s called with argument E out of range \n");
    printf("    => E should be larger than the proton mass M = %5.3f GeV\n",M);
    printf("\n\n");
    exit(1);
  }


  //4-mom of p (lab frame)
  p_p[0]=M;
  p_p[1]=0.;
  p_p[2]=0.;
  p_p[3]=0.;


  //4-mom of pbar (lab frame)
  P=pow((E*E-M*M),0.5);

  pbar_p[0]=E;
  pbar_p[1]=0.;
  pbar_p[2]=0.;
  pbar_p[3]=P;


  //4-mom pbarpsystem (lab frame)
  for(int i=0; i<4; i++){
    pbarp_p[i]=p_p[i]+pbar_p[i];
  }

  s=mzvmod2(4,pbarp_p);

  return s;
}

double mz_legendre_polynomial(int n, double x){
  /**
   * function  mz_legendre_polynomial
   *
   * INPUT: 
   *
   *   n -> polynomial degree, [0,10] 
   *   x -> independent variable, [-1,1]
   *
   * returns:
   *
   * P_{n}(x)
   *
   */

  double y;

  //checking parameter range...
  if( !( (n>=0)&&(n<=10) ) ){
    printf("ERROR: function  mz_legendre_polynomial called with n out of range \n");
    printf("    => n should be contained in the interval [0,10]\n");
    printf("\n\n");
    exit(1);
  }

  if( !( (x>=-1.0)&&(x<=1.0) ) ){
    printf("ERROR: function  mz_legendre_polynomial called with x out of range \n");
    printf("    => x should be contained in the interval [-1.0,1.0]\n");
    printf("\n\n");
    exit(1);
  }


  if(n==0){
    y=1.0;
  }

  if(n==1){
    y=x;
  }

  if(n==2){
    y=(1.0/2.0)*(3.0*pow(x,2.0) -1.0);
  }

  if(n==3){
    y=(1.0/2.0)*(5.0*pow(x,3.0) -3.0*x);
  }

  if(n==4){
    y=(1.0/8.0)*(35.0*pow(x,4.0) -30.0*pow(x,2.0) +3.0);
  }

  if(n==5){
    y=(1.0/8.0)*(63.0*pow(x,5.0) -70.0*pow(x,3.0) +15.0*x);
  }

  if(n==6){
    y=(1.0/16.0)*(231.0*pow(x,6.0) -315.0*pow(x,4.0) 
                 +105.0*pow(x,2.0) -5.0);
  }

  if(n==7){
    y=(1.0/16.0)*(429.0*pow(x,7.0) -693.0*pow(x,5.0) 
                  +315.0*pow(x,3.0) -35.0*x);
  }

  if(n==8){
    y=(1.0/128.0)*(6435.0*pow(x,8.0) -12012.0*pow(x,6.0)
                   +6930.0*pow(x,4.0) -1260.0*pow(x,2.0) 
                   +35.0);
  }

  if(n==9){
    y=(1.0/128.0)*(12155.0*pow(x,9.0) -25740.0*pow(x,7.0) 
                   +18018.0*pow(x,5.0) -4620.0*pow(x,3.0) 
                   +315.0*x);
  }

  if(n==10){
    y=(1.0/256.0)*(46189.0*pow(x,10.0) -109395.0*pow(x,8.0)
		   +90090.0*pow(x,6.0) -30030*pow(x,4.0)
                   +3465.0*pow(x,2.0) -63.0);
  }

  return y;
}

double mz_linear_extrapolation(double x1, double y1,
                               double x2, double y2,
                               double x)
{
  /**
   * funtion mz_linear_extrapolation
   *
   * INPUT:
   *
   *  (x1,y1) first point in (x,y)-plane
   *  (x2,y2) second point in (x,y)-plane
   *
   *  x independent variable
   *
   *
   * RETURNS:
   *
   *  y=f(x), with f the linear function atisfying y1=f(x1) and y2=f(x2) 
   *
   */

  double y;
  double a,b;

  //checking parameter range...
  if(x2==x1){
    printf("ERROR: function  mz_linear_extrapolation called with x_2=x_1 \n");
    printf("    => argument x_2 should be different from argument x_1 \n");
    printf("\n\n");
    exit(1);
  }

  a = (y1-y2)/(x1-x2);
  b = y1-a*x1;

  y = a*x+b;

  return y;
}

