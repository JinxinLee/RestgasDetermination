//testing script for the spline stuff - needs to be compiled

//THIS MACRO HAS GROWN MUCH TOO BIG!!! 
//there need to be a couple of smaller ones. 
//Felix

#include "tpc/spacecharge/BiCubSpline.h"
#include "tpc/spacecharge/BSpline.h"
#include "tpc/spacecharge/BiCubSplineFitter.h"
#include "tpc/spacecharge/SplineTF2Interface.h"
#include "PndTpcLaserStat.h" 
#include "PndTpcDevmapCyl.h"
#include "PndTpcDevmapSLA.h"
#include "PndTpcGas.h"
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
#include "TClonesArray.h"
#include "TTree.h"
#include "TPolyMarker3D.h"
#include "TStopwatch.h"
#include "TFile.h"


void testBiCubSpline2(std::string origDevMap, std::string laserRecoOut, int knotsZ=7, int knotsR=4) {


TStopwatch timer;
timer.Start();

//TODO: URGENT!!! UNDERDSTAND INFLUENCE OF KNOTS ON FITTER STABILITY!!!
//INCREASING the # of knots kills the fitter, with or without solution error!
//see comment on stepx: this may have been the reason
//Update: knot area has to be safely bigger than data area...

int NknotsX = knotsZ;
int NknotsY = knotsR; 
int NlamdaX = NknotsX+8;
int NlamdaY = NknotsY+8;

// I dont get it

// 4 empty points left and right

// border points INSIDE the data range kills both fits:

// border points OUTSIDE the data range AND BOTH FITS ARE OK !!!!!!!!!!

// border points ON the edge of the data range: kills orig fit


// 2 MORE EMPTY POINTS (NlamdaX = NknotsX+10) points:

//  border points OUTSIDE data range kills the reco fit:
// in reco fitter: 
/*    BiCubSplineFitter::decompose(): performing QR-decomposition... success
      BiCubSplineFitter::solve(): solving... Error in <TDecompQRH::Solve(TVectorD &)>: R[40,40]=0.0000e+00 < 2.2204e-16
      something went wrong
*/

// same thing with border points ON the edge kills the orig fit, but the other one works
// in orig fitter:
/*    BiCubSplineFitter::decompose(): performing QR-decomposition... success
      BiCubSplineFitter::solve(): solving... Error in <TDecompQRH::Solve(TVectorD &)>: R[39,39]=0.0000e+00 < 2.2204e-16
      something went wrong
*/

// same thing for border points INSIDE the data range kills both fits
// EXCEPT WE MOVE FARTHER INSIDE 

/* conclusions:
   
   problem ON the data range is consistent. this way knots fall together with data points for 
   the orig fit.

*/


std::vector<double>* _kx = new std::vector<double>;
std::vector<double>* _ky = new std::vector<double>;


double lLx = -43.;
double uLx = 113.;
double lLy = 14.;
double uLy = 43.;

 double stepx = (uLx - lLx)/(NknotsX-1); 
 double stepy = (uLy - lLy)/(NknotsY-1); 

double kx,ky;

for(int i=0; i<NlamdaX; ++i){
  kx = lLx + (i-4)*stepx;
  _kx->push_back(kx); 
}
for(int j=0; j<NlamdaY; ++j){
  ky = lLy + (j-4)*stepy;
  _ky->push_back(ky); 
}

// std::cout<<"\nThe knots in x:"<<std::endl;
// for(unsigned int k=0; k<_kx->size(); k++)
//   std::cout<<_kx->at(k)<<"  ";

// std::cout<<"\nThe knots in y:"<<std::endl;
// for(unsigned int k=0; k<_ky->size(); k++)
//   std::cout<<_ky->at(k)<<"  ";
// std::cout<<std::endl;



// Cub-Spline testing ------------------------------------------
BiCubSpline* csp = new BiCubSpline(_kx, _ky);
BiCubSpline* csp2 = new BiCubSpline(_kx, _ky);


SplineTF2Interface* func = new SplineTF2Interface(csp);   
SplineTF2Interface* func2 = new SplineTF2Interface(csp2);   



//get Devmap-Data: ------------------------------------------------------
double minR;
double maxR;
int rBins;
double minZ;
double maxZ;
int zBins;

double zWidth;
double rWidth;


//read file
std::string filename = origDevMap;

std::ifstream infile(filename.c_str(), std::fstream::in);

TFile* laserfile = new TFile("DevMap_Efield_March_09_Bfield_Official_RECONST.root");

//read geometry
infile>>rBins>>zBins>>minR>>maxR>>minZ>>maxZ;

zWidth = (maxZ - minZ) / zBins;
rWidth = (maxR - minR) / rBins;


TGraph2D* lasergraph = (TGraph2D*)(((TCanvas*)laserfile->Get("c"))->FindObject("Graph2D"));
TH2D* laserhist = lasergraph->GetHistogram();
TPolyMarker3D* laserpoly=(TPolyMarker3D*)(((TCanvas*)laserfile->Get("c"))->FindObject("TPolyMarker3D"));

// TGraph2D* lasergraph = (TGraph2D*) laserfile->Get("devmap");
// TH2D* laserdens = (TH2D*)laserfile->Get("densmap");
// TH2D* laserhist = (TH2D*)((TGraph2D*)laserfile->Get("devmap"))->GetHistogram();
// TPolyMarker3D* laserpoly = (TPolyMarker3D*)laserfile->Get("raw_Poly");

std::vector<std::vector<double>*> devRdata;       //standard deviation data
 std::vector<std::vector<double>*> laserdata;

//needed later for comparison
int lbinsz = laserhist->GetNbinsX();
int lbinsr = laserhist->GetNbinsY();
double lzmin = laserhist->GetXaxis()->GetXmin();
double lzmax = laserhist->GetXaxis()->GetXmax();
double lrmin = laserhist->GetYaxis()->GetXmin();
double lrmax = laserhist->GetYaxis()->GetXmax();
double zstep = (lzmax-lzmin)/lbinsz;
double rstep = (lrmax-lrmin)/lbinsr;


//for(int j=0; j<laserpoly->GetN(); j++) {
//  //if(j%10==0) {	
//  double x, y, z;
//  laserpoly->GetPoint(j,x,y,z);
//  laserpolydata.push_back(new TVector3(x,y,z));
//  //}
//}

for(int r=0; r<lbinsr; r++) {
  for(int z=0; z<lbinsz; z++) {
    laserdata.push_back(new std::vector<double>(4,0.));
    (laserdata[laserdata.size()-1])->at(0) = lzmin+(z+0.5)*zstep;
    (laserdata[laserdata.size()-1])->at(1) = lrmin+(r+0.5)*rstep;
    (laserdata[laserdata.size()-1])->at(2) = laserhist->GetBinContent(z+1,r+1);
  }
}
 
//build data from original devmap ---------------------------------------------------
for (int nr=0; nr<rBins; nr++)
{
  for (int nz=0; nz<zBins; nz++)
  {
    double xDev,yDev,time,way,temp;
    infile>>xDev>>yDev>>time>>temp>>way;
    std::vector<double>* vec = new std::vector<double>(4,0);
    vec->at(0) = minZ + nz*zWidth;
    vec->at(1) = minR + nr*rWidth;
    vec->at(2) = xDev;
    devRdata.push_back(vec);
  }
}
infile.close();



//build data from reco file directly -------------------------------------------------

std::vector<std::vector<double>*> laserpolydata;  //raw reconstructed residual points
std::vector<std::vector<double>*> laserpolydata_noErr;  //no errors

TFile* laserrecofile = new TFile(laserRecoOut.c_str());
TTree* t=(TTree*)laserrecofile->Get("cbmsim");
TClonesArray* statA=new TClonesArray("PndTpcLaserStat");
t->SetBranchAddress("PndTpcLaserStat",&statA);
t->GetEntry(0);

int nr=statA->GetEntriesFast();
std::cout<<nr<<" residuals found."<<std::endl;
for(int i=0;i<nr;++i){   //loop over residuals
  PndTpcLaserStat* stat=(PndTpcLaserStat*)statA->At(i);
  if(stat==NULL){std::cout<<"Invalid stat object!"<<std::endl;continue;}
  
  TVector3 proj=stat->getProjection();
  TVector3 res=stat->getRes();
  TVector3 cl_pos=stat->getPos();
  TVector3 sig=stat->getResSig();
  TVector3 sigpP=stat->getProjectionSig();
          
  double r=proj.Perp();
  double z=proj.Z();
  double clr=cl_pos.Perp();
   
  TVector3 runit=proj;
  runit.SetZ(0);
  TVector3 resXY=res;
  resXY.SetZ(0);
  runit*=(1./runit.Mag());
  TVector3 sigRU = sigpP * (1./runit.Mag()); 
  sigRU.SetZ(0);
  double dr=res*runit;
  double sig_r=sqrt(pow(res.X()*sigRU.X(),2) + pow(res.Y()*sigRU.Y(),2) + pow(runit.X()*sig.X(),2) + pow(runit.Y()*sig.Y(),2));
  
    //if(z<105)
  std::vector<double>* vec = new std::vector<double>(4,0);
  std::vector<double>* vec2 = new std::vector<double>(4,0);
  vec->at(0)=z; vec->at(1)=r; vec->at(2)=dr; vec->at(3)=sig_r;
  vec2->at(0)=z; vec2->at(1)=r; vec2->at(2)=dr;
  laserpolydata.push_back(vec);
  laserpolydata_noErr.push_back(vec2);
}



//construct the TF2-----------------------------------------------------

TF2* f1 =new TF2("f1",func,&SplineTF2Interface::eval,lzmin,lzmax,lrmin,lrmax,
		 (NlamdaX-4)*(NlamdaY-4),"Function", "eval");
TF2* f2 =new TF2("f2",func2,&SplineTF2Interface::eval,lzmin,lzmax,lrmin,lrmax,
		 (NlamdaX-4)*(NlamdaY-4),"Function", "eval");


//try to build a BiCubSplineFitter ----------------------------------------------------

std::vector<std::vector<double>*> devRdata2 = std::vector<std::vector<double>*>(devRdata);

//TODO: understand why using the SAME set of data for both fits gives DIFFERENT results!
//apparantly 2 times of sorting CHANGES the data set!
//-> make data const and copy data in the fitter!!

BiCubSplineFitter* fitter = new BiCubSplineFitter(csp, &laserpolydata);
BiCubSplineFitter* fitter2 = new BiCubSplineFitter(csp2, &devRdata);



//solve the problem and draw -----------------------------------------------------------
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

f1->SetParameters(param);// THIS DOES NOT AFFECT CSP'S _COEFF VECTOR !!!
                         // and f1 does NOT take CSP'S coeff vector in the
                         // first place!!!! 

f1->SetNpx(50);
f1->SetNpy(50);

f2->SetParameters(param2);

f2->SetNpx(50);
f2->SetNpy(50);

// ---------------------- init Deviation Map as referencer -------------------------------------

 std::cout<<"test"<<std::endl;
 std::cout.flush();

PndTpcGas* gas = new PndTpcGas("tpc/NEON-90_CO2-10_B2_PRES1013.asc",400);
double vDrift = gas->VDrift();
 std::cout<<"\nVDrift from PndTpcGas: "<<vDrift<<std::endl;
PndTpcDevmapCyl* devmap = new PndTpcDevmapCyl(origDevMap.c_str(),vDrift);
 delete gas;

PndTpcDevmapSLA* devSLA = new PndTpcDevmapSLA(origDevMap.c_str(),vDrift);


TH2D* diff = new TH2D("diff",
		      "Abs. difference of original and reconstructed devmap",
		      lbinsz,lzmin,lzmax,lbinsr,lrmin,lrmax);
//compare to original devmap, not to fit
TH2D* diff_orig = new TH2D("diff_orig",
		      "Abs. difference of UNFIT original and reconstructed devmap",
		      lbinsz,lzmin,lzmax,lbinsr,lrmin,lrmax);

TH2D* diff_SLA = new TH2D("diff_SLA",
		      "Abs. difference of UNFIT original and SLA devmap",
		      lbinsz,lzmin,lzmax,lbinsr,lrmin,lrmax);

diff->SetStats(false); 
diff_orig->SetStats(false);
diff_SLA->SetStats(false);

TH1D* diff_hist = new TH1D("blob", "Abs. difference in histogram", 150, -0.15, 0.15);
TH1D* diff_hist_orig = new TH1D("blob2", "Abs. difference in histogram", 150, -0.15, 0.15);
TH1D* diff_hist_SLA = new TH1D("blob3", "Abs. difference in histogram", 150, -0.15, 0.15); 
 


 for(int r=0; r<50; r++)
   for(int z=0; z<50; z++) {
     if(z*zstep+lzmin>109.5 || r*rstep+lrmin> 41.5) continue;
     double d = f1->Eval((z+0.5)*zstep+lzmin,(r+0.5)*rstep+lrmin)-f2->Eval((z+0.5)*zstep+lzmin,(r+0.5)*rstep+lrmin);
     diff->SetBinContent(z+1, r+1,d);
     diff_hist->Fill(d);

     //from ORIGINAL devmap
     TVector3 dev_from_SLA = devSLA->value(TVector3(0.,(r+0.5)*rstep+lrmin, (z+0.5)*zstep+lzmin));
     TVector3 deviation = devmap->value(TVector3(0.,(r+0.5)*rstep+lrmin, (z+0.5)*zstep+lzmin));
     //dev_from_SLA.Print();
   
       
     double d2 = deviation.Y() - f1->Eval((z+0.5)*zstep+lzmin,(r+0.5)*rstep+lrmin);
     diff_orig->SetBinContent(z+1, r+1,d2);
     diff_hist_orig->Fill(d2);
    
     double d3 = deviation.Y() - dev_from_SLA.Y();
     diff_SLA->SetBinContent(z+1, r+1,d3);
     diff_hist_SLA->Fill(d3);
			       
     if(fabs(d) < 0.001) {
       std::cout<<d;
       if(f1->Eval(z*zstep+lzmin,r*rstep+lrmin)==0 && f1->Eval(z*zstep+lzmin,r*rstep+lrmin)==0)
	 std::cout<<"   z="<<z<<",  r="<<r;
       std::cout<<std::endl;
     }
   }
       
std::cout<<"\n**** fit complete ****"<<std::endl;

TCanvas* c1 = new TCanvas();

c1->Divide(2,2);
//c1->cd(1);
//f1->Draw("SURF1");

c1->cd(1);
//hist->Fit("f1","R");
f1->Draw("SURF1"); 

c1->cd(2);
f2->Draw("SURF1");

c1->cd(3);
diff->Draw("COLZ");

c1->cd(4);
diff_hist->Draw();

timer.Stop();
Double_t rtime = timer.RealTime();
Double_t ctime = timer.CpuTime();
printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}
