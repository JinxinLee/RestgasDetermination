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
std::cout<<"Enter directory name (GEANT settings) ";
std::string setting;

 std::string basedir = "/afs/e18/panda/DATA/fboehmer/dipl_data/SpaceCharge/";

std::string filename;

getline(std::cin,setting);
std::cout<<std::endl;

 std::string file07 = basedir;
 std::string file13 = basedir;

 file07.append("07_01_2009/");
 file13.append("13_01_2009/");

 file07.append(setting);
 file13.append(setting);

 file07.append("/Drifted_SpaceCharge");
 file13.append("/Drifted_SpaceCharge");

 file07.append(setting);
 file13.append(setting);

 file07.append("07.dat");
 file13.append("13.dat");

std::ifstream infile07(file07.c_str(), std::fstream::in);
std::ifstream infile13(file13.c_str(), std::fstream::in);

if (!infile07.good() || !infile13.good())
{
  std::cerr<<"Not Existing"<<std::endl;
  return(0);
}

 std::cout<<"\nreading geometry data from file "<<file07.c_str()<<std::endl;
//read geometry
infile07>>minR>>segR>>nSegR>>minZ>>segZ>>nSegZ;
infile13>>minR>>segR>>nSegR>>minZ>>segZ>>nSegZ;


//read charge density
double chargeDens07[nSegR*nSegZ];
double chargeDens13[nSegR*nSegZ];

for (int nr=0; nr<nSegR; nr++)
  for (int nz=0; nz<nSegZ; nz++)
  {
    int index=nz+nr*nSegZ;
    infile07>>chargeDens07[index];
    infile13>>chargeDens13[index];
  }

infile07.close();
infile13.close();

std::string title07 = setting;
std::string title13 = setting;

 title07.append("_07");
 title13.append("_13");

title07.append(" [C/cm^3]");
title13.append(" [C/cm^3]");

//create axes
TH2D* histo07 = new TH2D(file07.c_str(),title07.c_str(),
                       nSegZ,minZ,minZ+nSegZ*segZ,
		       nSegR,minR,minR+nSegR*segR);

TH2D* histo13 = new TH2D(file13.c_str(),title13.c_str(),
                       nSegZ,minZ,minZ+nSegZ*segZ,
		       nSegR,minR,minR+nSegR*segR);

for (int nr=0;nr<nSegR;nr++)
{
  for (int nz=0; nz<nSegZ; nz++)
  {
    int index=nz+nr*nSegZ;
    histo07->SetBinContent(nz+1,nr+1,chargeDens07[index]);
    histo13->SetBinContent(nz+1,nr+1,chargeDens13[index]);
  }
}
//draw
TCanvas* c = new TCanvas(setting.c_str(),setting.c_str(),1);
gStyle->SetOptStat(0);
c->Divide(1,2);
c->cd(1);
histo07->GetXaxis()->SetTitle("z [cm]");
histo07->GetYaxis()->SetTitle("r [cm]");
histo07->Draw("COLZ");
c->cd(2);
histo13->GetXaxis()->SetTitle("z [cm]");
histo13->GetYaxis()->SetTitle("r [cm]");
histo13->Draw("COLZ");
c->Update();
}

    
