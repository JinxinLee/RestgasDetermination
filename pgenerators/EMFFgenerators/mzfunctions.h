/**
 * @author Manuel Zambrana <zambrana@kph.uni-mainz.de> and Dmitry Khaneftd <khaneftd@kph.uni-mainz.de>
 *
 * Mainz, May 2011
**/

#ifndef mzfunctions_h
#define mzfunctions_h 1

//mathematics, kinematics
double mzdelta(int i, int k);
double mzvscalar(int n, double* p, double* q);
double mzvmod2(int n, double* p);
void mzboost(int flag, double* p, double* q, double* q_prime);
double mzenergy(double m, double p_x, double p_y, double p_z);
double mzpolar(double* p);
double mzazimuthal(double* p);
double mzcospolar(double* p);
double mzangle(double* p, double* q);
double mzrnd(double a, double b);
double mzcomplexmod(double Re_z, double Im_z);
double mz_E_to_s(double E);
double mz_legendre_polynomial(int n, double x);
double mz_linear_extrapolation(double x1, double y1,
                               double x2, double y2,
                               double x);


#endif

