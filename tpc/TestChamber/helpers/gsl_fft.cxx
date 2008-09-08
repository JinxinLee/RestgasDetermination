//g++ `root-config --glibs --cflags` -I/sw/include/gsl/ -lgsl -lgslcblas gsl_fft.cxx -o gsl_fft

#include <stdio.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>
     
#include "TStyle.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TTree.h"
#include "TVector3.h"
#include "TMatrixT.h"
#include "TApplication.h"
#include "TObject.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TF1.h"


int
main (void)
{
  TApplication theApp("myApp",NULL,NULL);

  int i, n = 100;
  double data[n];
     
  double xvals[n];
  for(int i=0;i<n;++i){
    xvals[i]=i;
  }


  gsl_fft_real_wavetable * real;
  gsl_fft_halfcomplex_wavetable * hc;
  gsl_fft_real_workspace * work;
     
  for (i = 0; i < n; i++)
    {
      data[i] = 0.0;
    }
     
  for (i = n / 3; i < 2 * n / 3; i++)
    {
      data[i] = 1.0;
    }
     
  printf ("{");
  for (i = 0; i < n; i++)
    {
      printf ("%d,", i);
    }
  printf ("};");
  printf ("\n");
  printf ("{");
  for (i = 0; i < n; i++)
    {
      printf ("%e,", data[i]);
    }
  printf ("};");
  
 
  printf ("\n");
     
  work = gsl_fft_real_workspace_alloc (n);
  real = gsl_fft_real_wavetable_alloc (n);
     
  gsl_fft_real_transform (data, 1, n, 
			  real, work);
     
  gsl_fft_real_wavetable_free (real);
     
  double data2[n];
  for (i = 0; i < n; i++)
    {
      data2[i]=data[i];
    }

  for (i = 11; i < n; i++)
    {
      data2[i] = 0;
    }
     
  hc = gsl_fft_halfcomplex_wavetable_alloc (n);
     
  gsl_fft_halfcomplex_inverse (data2, 1, n, 
			       hc, work);
  gsl_fft_halfcomplex_wavetable_free (hc);
     
  printf ("{");
  for (i = 0; i < n; i++)
    {
      printf ("%d,", i);
    }
  printf ("};");
  printf ("\n");
  printf ("{");
  for (i = 0; i < n; i++)
    {
      printf ("%e,", data2[i]);
    }
  printf ("};");
  
  gsl_fft_real_workspace_free (work);
  TGraph* graph1=new TGraph(n,xvals,data2);
  graph1->Draw("A*L");
  theApp.Run();



  return 0;
}
