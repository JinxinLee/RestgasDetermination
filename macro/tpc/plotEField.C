//script for root to visualize output files of CalcEField
//NEEDS TO BE COMPILED WITH ".L scriptname+"

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



void plotEField() {


gROOT->Reset();
gStyle->SetPalette(1);

//define variables
double nomEField;
double minR;
double segR;
int nSegR;
double minZ;
double segZ;
int nSegZ;

TH2D* histoR;
TH2D* histoZ;

//read file
std::cout<<"Enter filename: ";
std::string filename;
getline(std::cin,filename);
std::cout<<std::endl;
std::string answer;
std::cout<<"Do you want to use the original binning? (y/n): ";
getline(std::cin, answer);
std::cout.flush();

std::ifstream infile(filename.c_str(), std::fstream::in);

while(answer!="n" && answer!= "y") {
  std::cout<<"\ninvalid input! Please repeat: ";
  getline(std::cin, answer);
}
 
if(answer=="y") {
if (!infile.good())
{
  std::cerr<<"Not Existing"<<std::endl;
}
//read geometry
infile>>nomEField>>minR>>segR>>nSegR>>minZ>>segZ>>nSegZ;
//read el field and
//fill histogramms

// WATCH OUT FOR PROPER VALUES IN YOUR FIELD FILES!!!

// segR = ((nSegR-1)*segR)/nSegR;
// segZ = ((nSegZ-1)*segZ)/nSegZ;

histoR = new TH2D(filename.c_str(),"Electric field in r direction [V/cm]",
			nSegZ,minZ,minZ+nSegZ*segZ,nSegR,minR,minR+nSegR*segR);
histoZ = new TH2D(filename.c_str(),"Electric field in z direction [V/cm]",
			nSegZ,minZ,minZ+nSegZ*segZ,nSegR,minR,minR+nSegR*segR);
for (int nr=0; nr<nSegR; nr++)
{
  for (int nz=0; nz<nSegZ; nz++)
  {
    double eFieldR; 
    double eFieldZ;
    infile>>eFieldR>>eFieldZ;
    histoR->SetBinContent(nz+1,nr+1,eFieldR);
    histoZ->SetBinContent(nz+1,nr+1,eFieldZ);
  }
}

}

if(answer=="n") {

std::ifstream infile(filename.c_str(), std::fstream::in);
if (!infile.good())
{
  std::cerr<<"Not Existing"<<std::endl;
}

//read geometry
infile>>nomEField>>minR>>segR>>nSegR>>minZ>>segZ>>nSegZ;

// eval max coordinates:
double maxR = minR + segR*(nSegR-1);
double maxZ = minZ + segZ*(nSegZ-1);

//get new binning:
std::cout<<"\nEnter bins in r direction: ";
getline(std::cin, answer);
nSegR=atoi(answer.c_str());
std::cout<<"Enter bins in z direction: ";
getline(std::cin, answer);
nSegZ=atoi(answer.c_str());

//eval new binning
segR = (maxR - minR)/nSegR;
segZ = (maxZ - minZ)/nSegZ;


//create and fill histograms
histoR = new TH2D(filename.c_str(),"Electric field in r direction [V/cm]",
			nSegZ,minZ,minZ+(nSegZ-1)*segZ,nSegR,minR,minR+(nSegR-1)*segR);
histoZ = new TH2D(filename.c_str(),"Electric field in z direction [V/cm]",
			nSegZ,minZ,minZ+(nSegZ-1)*segZ,nSegR,minR,minR+(nSegR-1)*segR);

PndTpcEFieldCyl* field = new PndTpcEFieldCyl(filename.c_str());

for (int nr=0; nr<nSegR; nr++)
{
  for (int nz=0; nz<nSegZ; nz++)
  {
    double eFieldR; 
    double eFieldZ;
    TVector3 pos = TVector3(minR + (nr+0.5)*segR, 0., minZ + (nz+0.5)*segZ);
    TVector3 value=field->value(pos);
    eFieldR=value.X();
    eFieldZ=value.Z();
    histoR->SetBinContent(nz+1,nr+1,eFieldR);
    histoZ->SetBinContent(nz+1,nr+1,eFieldZ);
  }
}
}

infile.close();

//draw
const char* cfile = filename.c_str();
TCanvas* c = new TCanvas(cfile,cfile,700,1000);
gStyle->SetOptStat(0);
c->Divide(1,2,0.001,0.001,0);
c->cd(1);
std::string fileR = filename;
fileR.append("_r");
histoR->GetXaxis()->SetTitle("z [cm]");
histoR->GetYaxis()->SetTitle("r [cm]");
histoR->Draw("COLZ");
std::string fileZ = filename;
fileZ.append("_z");
c->cd(2);
histoZ->GetXaxis()->SetTitle("z [cm]");
histoZ->GetYaxis()->SetTitle("r [cm]");
histoZ->Draw("COLZ");
c->Update();

}
