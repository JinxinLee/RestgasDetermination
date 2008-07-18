//script for root to visualize output files of PndTpcSpaceChargeTask

{
#include <fstream>
#include <vector>
#include <string>

gROOT->Reset();
gStyle->SetPalette(1);

//define variables
double minR;
double segR;
int nSegR;
double minZ;
double segZ;
int nSegZ;
//read file
std::cout<<"Enter filename (output of simple_Drift.C): ";
std::string filename;
getline(std::cin,filename);
std::cout<<std::endl;
std::ifstream infile(filename.c_str(), std::fstream::in);
if (!infile.good())
{
  std::cerr<<"Not Existing"<<std::endl;
  return(0);
}
//read geometry
infile>>minR>>segR>>nSegR>>minZ>>segZ>>nSegZ;
//read charge density
double chargeDens[nSegR*nSegZ];
for (int nr=0; nr<nSegR; nr++)
  for (int nz=0; nz<nSegZ; nz++)
  {
    int index=nz+nr*nSegZ;
    infile>>chargeDens[index];
  }
infile.close();

std::string title = filename;
title.append(" [C/cm^3]");

//create axes
TH2D* histo = new TH2D(filename.c_str(),title.c_str(),
                       nSegZ,minZ,minZ+nSegZ*segZ,
		       nSegR,minR,minR+nSegR*segR);
for (int nr=0;nr<nSegR;nr++)
{
  for (int nz=0; nz<nSegZ; nz++)
  {
    int index=nz+nr*nSegZ;
    histo->SetBinContent(nz+1,nr+1,chargeDens[index]);
  }
}
//draw
TCanvas* c = new TCanvas(filename.c_str(),filename.c_str(),1);
gStyle->SetOptStat(0);
histo->GetXaxis()->SetTitle("z [cm]");
histo->GetYaxis()->SetTitle("r [cm]");
histo->Draw("COLZ");
c->Update();
}

    
