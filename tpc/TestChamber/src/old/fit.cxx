#include "fit.h"
#include "StdoutKiller.h"
//#include "global.h"
//#include"unistd.h"
//#include"fcntl.h"

//#include "TRandom.h"

int nHits_g;
double x_g[MAXHITS];
double y_g[MAXHITS];
double z_g[MAXHITS];
double xerr_g[MAXHITS];
double yerr_g[MAXHITS];
double zerr_g[MAXHITS];


void residuals(double ax, double bx,
			   double ay, double by,
			   double x, double y, double z,
			   double &dx, double &dy, double &dz){

  double scalarproduct1 = x*ax + y*ay + z;//R*Q
  double scalarproduct2 = bx*ax + by*ay;//P*Q
  double scalarproduct3 = ax*ax + ay*ay + 1;//Q^2

  double factor = (scalarproduct1 - scalarproduct2)/scalarproduct3;

  dx = bx + factor*ax - x;
  dy = by + factor*ay - y;
  dz = factor - z;

}


void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{


//calculate chisquare
  Double_t chisq = 0;
  double dx,dy,dz;
  if(nHits_g > MAXHITS) {
	std::cerr << "nHits_g > MAXHITS" << std::endl;
	throw;
  }
  for (int i=0;i<nHits_g; i++) {
	residuals(par[0],par[1],par[2],par[3],
			  x_g[i],y_g[i],z_g[i],
			  dx,dy,dz);
	chisq += pow(dx/xerr_g[i],2.) + pow(dy/yerr_g[i],2.) + pow(dz/zerr_g[i],2.);
  }
  //float rand = gRandom->Uniform();  
  f = chisq;//+40000.*rand;
}


fitReturn_t doFit(fitArg_t fitArg){
  nHits_g = fitArg.nHits;
  for(int i=0;i<fitArg.nHits;i++) {
	x_g[i] = fitArg.x[i];
	y_g[i] = fitArg.y[i];
	z_g[i] = fitArg.z[i];

	xerr_g[i] = fitArg.xerr[i];
	yerr_g[i] = fitArg.yerr[i];
	zerr_g[i] = fitArg.zerr[i];
  }


  TMinuit *myMinuit = new TMinuit(6);  //initialize TMinuit with a maximum of 5 params
  myMinuit->SetFCN(fcn);
  
  Double_t arglist[10];
  Int_t ierflg = -1;
  
  arglist[0] = 1;
  {
	StdoutKiller killer;
	myMinuit->mnexcm("SET ERR", arglist ,1,ierflg);
  }

  double stepSizeFrac = 0.001;
  // Set starting values and step sizes for parameters
  Double_t vstart[4] = {fitArg.ax_s,fitArg.bx_s ,fitArg.ay_s ,fitArg.by_s };
  Double_t step[4] = {fabs(fitArg.ax_s*stepSizeFrac),
					  fabs(fitArg.bx_s*stepSizeFrac) ,
					  fabs(fitArg.ay_s*stepSizeFrac) ,
					  fabs(fitArg.by_s*stepSizeFrac) };
  {
	StdoutKiller killer;
	myMinuit->mnparm(0, "ax", vstart[0], step[0], 0, 0,ierflg);
	myMinuit->mnparm(1, "bx", vstart[1], step[1], 0,0,ierflg);
	myMinuit->mnparm(2, "ay", vstart[2], step[2], 0,0,ierflg);
	myMinuit->mnparm(3, "by", vstart[3], step[3], 0,0,ierflg);
  }

  // Now ready for minimization step
  arglist[0] = 500;
  arglist[1] = 1.;

  {
	StdoutKiller killer;
	myMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);
  }
  
  std::cout << "fit.cxx @@@@@@@@ Minuit fit status (0 is convergence): " << ierflg << std::endl;

  // Print results
  //Double_t amin,edm,errdef;
  //Int_t nvpar,nparx,icstat;
  //myMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
  //myMinuit->mnprin(3,amin);
  
  fitReturn_t fitReturn;
  double ax_f,ay_f,bx_f,by_f;
  double axerr_f,ayerr_f,bxerr_f,byerr_f;
  myMinuit->GetParameter(0,ax_f,axerr_f);
  myMinuit->GetParameter(1,bx_f,bxerr_f);
  myMinuit->GetParameter(2,ay_f,ayerr_f);
  myMinuit->GetParameter(3,by_f,byerr_f);

  delete myMinuit;  

  Double_t chisq = 0;
  double dx,dy,dz;
  for (int i=0;i<nHits_g; i++) {
	residuals(ax_f,bx_f,ay_f,by_f,
			  x_g[i],y_g[i],z_g[i],
			  dx,dy,dz);
	chisq += pow(dx/xerr_g[i],2.) + pow(dy/yerr_g[i],2.) + pow(dz/zerr_g[i],2.);
  }
  fitReturn.chi2 = chisq; 
  fitReturn.ax = ax_f; 
  fitReturn.bx = bx_f; 
  fitReturn.ay = ay_f; 
  fitReturn.by = by_f; 

  return fitReturn;
}
 
 
