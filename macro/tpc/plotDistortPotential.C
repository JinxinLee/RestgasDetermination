//script for root to visualize output files of CalcEField


#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>
#include "tpc/PndTpcEFieldCyl.h"
#include "TVector3.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TString.h"



void plotDistortPotential(TString filename) {


gROOT->Reset();
gStyle->SetPalette(1);

//define variables
double minR;
double segR;
int nSegR;
double minZ;
double segZ;
int nSegZ;

TH2D* histoR;
TH2D* histoZ;

//read file
std::ifstream infile(filename.Data(), std::fstream::in);

if (!infile.good())
{
  std::cerr<<"Not Existing"<<std::endl;
}
//read geometry
infile>>minR>>segR>>nSegR>>minZ>>segZ>>nSegZ;
//read el field and
//fill histogramms
//segR = ((nSegR-1)*segR)/nSegR;
//segZ = ((nSegZ-1)*segZ)/nSegZ;

histoR = new TH2D(filename.Data(),"Electro static potential [V]",
			nSegZ-1,minZ,minZ+(nSegZ)*segZ,nSegR-1,minR+segR,minR+(nSegR)*segR);

for (int nr=0; nr<nSegR; nr++)
{
  for (int nz=0; nz<nSegZ; nz++)
  {
    double pot;
    infile>>pot;
    histoR->SetBinContent(nz,nr,pot);
  }
}



infile.close();

//draw
const char* cfile = filename.Data();
TCanvas* c = new TCanvas(cfile,cfile);
gStyle->SetOptStat(0);
std::string fileR = filename.Data();
fileR.append("_r");
histoR->GetXaxis()->SetTitle("z [cm]");
histoR->GetYaxis()->SetTitle("r [cm]");
histoR->Draw("COLZ");
histoR->GetZaxis()->SetTitle("#Phi [V]");
histoR->Draw("COLZ");
}
