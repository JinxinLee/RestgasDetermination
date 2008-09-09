#include"fft.h"

#include <stdio.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>


void fft(int n,double* in,double* out){
  double workspace[n];
  for(int i=0;i<n;++i)workspace[i]=in[i];


  gsl_fft_real_wavetable * real;
  gsl_fft_halfcomplex_wavetable * hc;
  gsl_fft_real_workspace * work;
     
  work = gsl_fft_real_workspace_alloc (n);
  real = gsl_fft_real_wavetable_alloc (n);
     
  gsl_fft_real_transform (workspace, 1, n, 
			  real, work);
     
  gsl_fft_real_wavetable_free (real);
  gsl_fft_real_workspace_free (work);
  for(int i=0;i<n;++i)out[i]=workspace[i];

}
