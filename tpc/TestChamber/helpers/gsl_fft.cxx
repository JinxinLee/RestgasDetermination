//g++ `root-config --glibs --cflags` -I/sw/include/gsl/ -lgsl -lgslcblas gsl_fft.cxx -o gsl_fft

#include <stdio.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>
     
#include "TStyle.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TMath.h"
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
  double data2[n];
  double data_initial[n];
  double data_fft[n];
  double data_final[n];

  double xvals[n];
  for(int i=0;i<n;++i){
    xvals[i]=i;
  }


  gsl_fft_real_wavetable * real;
  gsl_fft_halfcomplex_wavetable * hc;
  gsl_fft_real_workspace * work;
     
  for (i = 0; i < n; i++)
    {
      //      data[i] = 1.0;
      data[i] = cos(80./n *TMath::Pi()*i)+sin(30./n *TMath::Pi()*i)+sin(10./n *TMath::Pi()*i);

    }
     
  //  for (i = n / 3; i < 2 * n / 3; i++)
//   for (i = 20; i < 40; i++)
//     {
//       data[i] = 3.0;
//     }
     
  for (i = 0; i < n; i++)
    {
      data_initial[i]=data[i];
    }
  

  /*
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
  */
     
  work = gsl_fft_real_workspace_alloc (n);
  real = gsl_fft_real_wavetable_alloc (n);
     
  gsl_fft_real_transform (data, 1, n, 
			  real, work);
     
  gsl_fft_real_wavetable_free (real);
     

  for (i = 0; i < n; i++)
    {
      data2[i]=data[i];
      data_fft[i]=data[i];
    }

  for (i = 100; i < n; i++)
    {
      data2[i] = 0;
    }
     
  hc = gsl_fft_halfcomplex_wavetable_alloc (n);
     
  gsl_fft_halfcomplex_inverse (data2, 1, n, 
			       hc, work);
  gsl_fft_halfcomplex_wavetable_free (hc);
     
  for (i = 0; i < n; i++)
    {
      data_final[i]=data2[i];
    }
  


  /*
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
  */

  double data_self[n];
  for(int j=0; j<n;++j){

    data_self[j]=0.;
    for(int k=0;k<n;++k){
      data_self[j]+=data_fft[k]*TMath::Cos(-2.*TMath::Pi()*j*k/((double)n));
    }

  }

  
  gsl_fft_real_workspace_free (work);
  TCanvas c1("c1","",500,500);
  TGraph* graph_initial=new TGraph(n,xvals,data_initial);
  TGraph* graph_final=new TGraph(n,xvals,data_final);
  TGraph* graph_self=new TGraph(n,xvals,data_self);
  graph_initial->SetMarkerStyle(20);
  graph_final->SetMarkerStyle(24);
  graph_self->SetMarkerStyle(20);
  graph_initial->SetMarkerSize(0.8);
  graph_final->SetMarkerSize(1.8);
  graph_self->SetMarkerSize(0.8);
  graph_initial->SetMarkerColor(kBlue);
  graph_final->SetMarkerColor(kRed);
  graph_self->SetMarkerColor(kGreen);
  graph_self->SetTitle("time");
  graph_final->SetTitle("time");
  //  graph_self->Draw("APL");
  graph_final->Draw("APL");
  graph_initial->Draw("PL");

  TCanvas c2("c2","",550,0,500,500);
  TGraph* graph_fft=new TGraph(n,xvals,data_fft);
  graph_fft->SetMarkerStyle(20);
  graph_fft->SetMarkerSize(0.8);
  graph_fft->SetMarkerColor(kBlue);
  graph_fft->SetTitle("frequency");
  graph_fft->Draw("APL");


  theApp.Run();



  return 0;
}
