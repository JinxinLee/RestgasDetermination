//testing script for the spline stuff - needs to be compiled


//THIS MACRO HAS GROWN MUCH TOO BIG!!! 
//there need to be a couple of smaller ones. 
//Felix

#include "tpc/spacecharge/BiCubSpline.h"
#include "tpc/spacecharge/BSpline.h"
#include "tpc/spacecharge/BiCubSplineFitter.h"
#include "tpc/spacecharge/SplineTF2Interface.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include "TF2.h"
#include "TH2D.h"
#include "TGraph2D.h"
#include "TArrayD.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TPolyMarker3D.h"
#include "TStopwatch.h"
#include "TFile.h"


void testBiCubSpline() {


TStopwatch timer;
timer.Start();

//TODO: URGENT!!! UNDERDSTAND INFLUENCE OF KNOTS ON FITTER STABILITY!!!
//INCREASING the # of knots kills the fitter, with or without solution error!
//see comment on stepx: this may have been the reason

int NknotsX = 3;
int NknotsY = 7; 
int NlamdaX = NknotsX+8;
int NlamdaY = NknotsY+8;
std::vector<double>* _kx = new std::vector<double>;
std::vector<double>* _ky = new std::vector<double>;

std::vector<TVector3*> data;  

std::vector<std::vector<double>*>* coeff=new std::vector<std::vector<double>*>;
 double lLx = -48;
 double uLx = 130;
 double lLy = 15;
 double uLy = 42;
double stepx = (uLx - lLx + 1)/NknotsX;  //"+1": mke sure last valid knot is slightly 
double stepy = (uLy - lLy + 1)/NknotsY;  //larger than uLx or uLy

double kx,ky;

for(int i=0; i<NlamdaX; ++i){
  kx = lLx + (i-3)*stepx;
  _kx->push_back(kx); 
}
for(int j=0; j<NlamdaY; ++j){
  ky = lLy + (j-3)*stepy;
  _ky->push_back(ky); 
}

// std::cout<<"\nThe knots in x:"<<std::endl;
// for(unsigned int k=0; k<_kx->size(); k++)
//   std::cout<<_kx->at(k)<<"  ";

// std::cout<<"\nThe knots in y:"<<std::endl;
// for(unsigned int k=0; k<_ky->size(); k++)
//   std::cout<<_ky->at(k)<<"  ";
// std::cout<<std::endl;

for(int i=0; i<NlamdaY-4; i++) {
  coeff->push_back(new std::vector<double>(NlamdaX-4, 1.));
}


// Cub-Spline testing ------------------------------------------
BiCubSpline* csp = new BiCubSpline(_kx, _ky, coeff);
BiCubSpline* csp2 = new BiCubSpline(_kx, _ky, coeff);


SplineTF2Interface* func = new SplineTF2Interface(csp);   
SplineTF2Interface* func2 = new SplineTF2Interface(csp2);   


//construct the TF2-----------------------------------------------------

TF2* f2 =new TF2("f2",func,&SplineTF2Interface::eval,-40,110,15,42,
		 (NlamdaX-4)*(NlamdaY-4),"Function", "eval");
TF2* f3 =new TF2("f3",func2,&SplineTF2Interface::eval,-38,105,16,39,
		 (NlamdaX-4)*(NlamdaY-4),"Function", "eval");


//get Devmap-Data: ------------------------------------------------------
double minR;
double maxR;
int rBins;
double minZ;
double maxZ;
int zBins;

double temp;
double zWidth;
double rWidth;


//read file
std::string filename = "DevMap_Efield_March_09_Bfield_Official.dat";

std::ifstream infile(filename.c_str(), std::fstream::in);

TFile* laserfile = new TFile("DevMap_Efield_March_09_Bfield_Official_RECONST.root");


//read geometry
infile>>rBins>>zBins>>minR>>maxR>>minZ>>maxZ;

zWidth = (maxZ - minZ) / zBins;
rWidth = (maxR - minR) / rBins;

TH2D* histoX = new TH2D(filename.c_str(),"Drift in radial direction [cm]",
			zBins,minZ,maxZ,rBins,minR,maxR);
TH2D* histoY = new TH2D(filename.c_str(),"Drift perp. radial-direction [cm]",
			zBins,minZ,maxZ,rBins,minR,maxR);

TGraph2D* lasergraph = (TGraph2D*)(((TCanvas*)laserfile->Get("c"))->FindObject("Graph2D"));
TH2D* laserhist = lasergraph->GetHistogram();
TPolyMarker3D* laserpoly=(TPolyMarker3D*)(((TCanvas*)laserfile->Get("c"))->FindObject("TPolyMarker3D"));

// TH2D* laserhist = (TH2D*)((TGraph2D*)laserfile->Get("devmap"))->GetHistogram();
// TGraph2D* lasergraph = (TGraph2D*) laserfile->Get("devmap");
// TH2D* laserdens = (TH2D*)laserfile->Get("densmap");
// TPolyMarker3D* laserpoly = (TPolyMarker3D*)laserfile->Get("raw_Poly");


std::vector<std::vector<double>*> devRdata;       //standard deviation data
std::vector<std::vector<double>*> laserdata;      //reconstructed dev data
std::vector<std::vector<double>*> laserpolydata;  //raw reconstructed residual points

// std::vector<TVector3*> devRdata;       //standard deviation data
// std::vector<TVector3*> laserdata;      //reconstructed dev data
// std::vector<TVector3*> laserpolydata;  //raw reconstructed residual points


int lbinsz = laserhist->GetNbinsX();
int lbinsr = laserhist->GetNbinsY();

double lzmin = laserhist->GetXaxis()->GetXmin();
double lzmax = laserhist->GetXaxis()->GetXmax();
double lrmin = laserhist->GetYaxis()->GetXmin();
double lrmax = laserhist->GetYaxis()->GetXmax();

 std::cout<<"\nLaserDevMap source histogram:"<<std::endl;
 std::cout<<"zmin: "<<lzmin<<"   zmax: "<<lzmax<<std::endl;
 std::cout<<"rmin: "<<lrmin<<"   rmax: "<<lrmax<<std::endl;

double zstep = (lzmax-lzmin)/lbinsz;
double rstep = (lrmax-lrmin)/lbinsr;

for(int j=0; j<laserpoly->GetN(); j++) {
  //if(j%10==0) {	
  double x, y, z;
  laserpoly->GetPoint(j,x,y,z);
  laserpolydata.push_back(new std::vector<double>(4,0));
  (laserpolydata[laserpolydata.size()-1])->at(0) = x;
  (laserpolydata[laserpolydata.size()-1])->at(1) = y;
  (laserpolydata[laserpolydata.size()-1])->at(2) = z;
  //}
}

int counter=0;
TGraph2D* graph = new TGraph2D(laserpolydata.size());
for (int i=0; i<laserpolydata.size(); i++) {
  std::vector<double>* temp=laserpolydata.at(i);
  graph->SetPoint(i,temp->at(0), temp->at(1), temp->at(2));
  if(temp->at(0) > 110 || temp->at(0)<(-40) || temp->at(1) > 42 || temp->at(1) < 15)
    counter++;
}

std::cout<<"\n"<<counter<<" values of "<<laserpolydata.size()<<" where invalid"<<std::endl;

// for(int r=0; r<lbinsr; r++) {
//   for(int z=0; z<lbinsz; z++) 
//     laserdata.push_back(new TVector3(lzmin+z*zstep, lrmin+r*rstep,
// 				     laserhist->GetBinContent(z+1,r+1)));
// }

for (int nr=0; nr<rBins; nr++)
{
  for (int nz=0; nz<zBins; nz++)
  {
    double xDev,yDev,time,way,temp;
    infile>>xDev>>yDev>>time>>temp>>way;
    devRdata.push_back(new std::vector<double>(4,0));
    (devRdata[devRdata.size()-1])->at(0) = minZ + nz*zWidth;
    (devRdata[devRdata.size()-1])->at(1) = minR + nr*rWidth;
    (devRdata[devRdata.size()-1])->at(2) = xDev;
    //devRdata.push_back(new TVector3(minZ + nz*zWidth, minR + nr*rWidth, xDev));
    histoX->SetBinContent(nz+1,nr+1,xDev);
    histoY->SetBinContent(nz+1,nr+1,yDev);
  }
}
infile.close();


//try to build a BiCubSplineFitter ---------------------------------------

BiCubSplineFitter* fitter = new BiCubSplineFitter(csp, &devRdata);
BiCubSplineFitter* fitter2 = new BiCubSplineFitter(csp2, &laserpolydata);


//take a look at the matrices --------------------------------------------
// TMatrixD A = TMatrixD(* (fitter->getSpMatrix()));

// int ncols = A.GetNcols();
// int nrows = A.GetNrows();

// TH2D* histmat = new TH2D("bloek", "bloek", ncols,0,ncols, nrows, 0, nrows);
//  for(int i=0; i<nrows; i++) {
//    for(int j=0; j<ncols; j++) 
//      histmat->SetBinContent(j+1, i+1, A[i][j]);
//  }

// TMatrixD Af = TMatrixD(* (fitter->getFixedMatrix()));

// int ncolsf = Af.GetNcols();
// int nrowsf = Af.GetNrows();

//TH2D* histmatf = new TH2D("blaegh","blaeghk",ncolsf,0,ncolsf,nrowsf,0,nrowsf);
//  for(int i=0; i<nrowsf; i++) {
//    for(int j=0; j<ncolsf; j++) 
//      histmatf->SetBinContent(j+1, i+1, Af[i][j]);
//  }

//--------------------------------------------------------------------


//solve the problem and draw -----------------------------------------
fitter->decompose();
TVectorD solution = fitter->solve();
fitter2->decompose();
TVectorD solution2 = fitter2->solve();

//extract the right number of elements
TArrayD result = TArrayD((NlamdaX-4)*(NlamdaY-4));
for(int i=0; i<(NlamdaX-4)*(NlamdaY-4); i++)
  result[i] = solution[i];

TArrayD result2 = TArrayD((NlamdaX-4)*(NlamdaY-4));
for(int i=0; i<(NlamdaX-4)*(NlamdaY-4); i++)
  result2[i] = solution2[i];


double* param = result.GetArray();
double* param2 = result2.GetArray();

f2->SetParameters(param);// THIS DOES NOT AFFECT CSP'S _COEFF VECTOR !!!
                           // and f2 does NOT take CSP'S coeff vector in the
                           // first place!!!! 

f2->SetNpx(50);
f2->SetNpy(50);

f3->SetParameters(param2);

f3->SetNpx(50);
f3->SetNpy(50);

TH2D* diff = new TH2D("diff",
		      "difference of original and reconstructed devmap",
		      lbinsz,lzmin,lzmax,lbinsr,lrmin,lrmax);

TH2D* rel_diff = new TH2D("diff",
		      "rel difference of original and reconstructed devmap",
		      lbinsz,lzmin,lzmax,lbinsr,lrmin,lrmax);

for(int r=0; r<50; r++)
  for(int z=0; z<50; z++) {
    diff->SetBinContent(z+1, r+1,
			f2->Eval(z*zstep+lzmin,r*rstep+lrmin) - 
			f3->Eval(z*zstep+lzmin,r*rstep+lrmin));
    if(fabs(f3->Eval(z*zstep+lzmin,r*rstep+lrmin)) >0.03 ) 
      rel_diff->SetBinContent(z+1, r+1,
			      (fabs(f2->Eval(z*zstep+lzmin,r*rstep+lrmin) / 
			      f3->Eval(z*zstep+lzmin,r*rstep+lrmin))));
  }
     

std::cout<<"\n**** fit complete ****"<<std::endl;

TCanvas* c1 = new TCanvas();

c1->Divide(1,3);
//c1->cd(1);
//f2->Draw("SURF1");

c1->cd(1);
//hist->Fit("f2","R");
f2->Draw("SURF1"); 

c1->cd(2);
f3->Draw("SURF1");

c1->cd(3);
diff->Draw("COLZ"); 

timer.Stop();
Double_t rtime = timer.RealTime();
Double_t ctime = timer.CpuTime();
printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}


