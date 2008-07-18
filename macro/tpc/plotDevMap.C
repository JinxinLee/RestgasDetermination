//small root script to plot the Deviation Map

{
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

gROOT->Reset();
gStyle->SetPalette(1);

//define variables
double minR;
double maxR;
int rBins;
double minZ;
double maxZ;
int zBins;

double temp;
double zWidth;

double PI = 3.14159265;
double v_Drift = 0.0027314;


//read file
std::cout<<"\n\nEnter name of Deviation-map-file: ";
std::string filename;
getline(std::cin,filename);
std::cout<<std::endl;
std::ifstream infile(filename.c_str(), std::fstream::in);
if (!infile.good())
{
  std::cerr<<"file not existing!!"<<std::endl;
  return(0);
}
//read geometry
infile>>rBins>>zBins>>minR>>maxR>>minZ>>maxZ;

zWidth = (maxZ - minZ) / zBins;

TH2D* histoX = new TH2D(filename.c_str(),"Drift in radial direction [cm]",
			zBins,minZ,maxZ,rBins,minR,maxR);
TH2D* histoY = new TH2D(filename.c_str(),"Drift perp. to radial-direction [cm]",
			zBins,minZ,maxZ,rBins,minR,maxR);
TH2D* histoT = new TH2D(filename.c_str(),"relative time-deviation",
			zBins,minZ,maxZ,rBins,minR,maxR);
TH2D* histoW = new TH2D(filename.c_str(),"relative total path deviation",
			zBins,minZ,maxZ,rBins,minR,maxR);
TH1D* histoV = new TH1D(filename.c_str(),"real drift velocity distribution",
			2000, 0.002,0.003);
			 


for (int nr=0; nr<rBins; nr++)
{
  for (int nz=0; nz<zBins; nz++)
  {
    double xDev,yDev,time,straight_time,way,straight_way,temp;
    infile>>xDev>>yDev>>time>>temp>>way;
    histoX->SetBinContent(nz+1,nr+1,xDev);
    // phidev = atan(yDev/xDev) * 180/PI;
    histoY->SetBinContent(nz+1,nr+1,yDev);
    straight_time = (nz + 0.5) * zWidth / v_Drift;
    histoT->SetBinContent(nz+1,nr+1,time/straight_time);
    straight_way = (nz + 0.5) * zWidth;
    histoW->SetBinContent(nz+1,nr+1,way/straight_way);
    histoV->Fill(way/time);           
  }
}
infile.close();
//draw
char* cfile = filename.c_str();
TCanvas* c = new TCanvas(cfile,cfile,700,1000);
gStyle->SetOptStat(0);
c->Divide(1,3,0.001,0.001,0);
c->cd(1);
histoX->GetXaxis()->SetTitle("z [cm]");
histoX->GetYaxis()->SetTitle("r [cm]");
histoX->Draw("SURF1");
c->cd(2);
histoY->GetXaxis()->SetTitle("z [cm]");
histoY->GetYaxis()->SetTitle("r [cm]");
histoY->Draw("SURF1");
c->cd(3);
histoT->GetXaxis()->SetTitle("z [cm]");
histoT->GetYaxis()->SetTitle("r [cm]");
histoT->Draw("COLZ");

c->Update();

// TCanvas* c2 = new TCanvas(cfile,cfile,700,1000);
// gStyle->SetOptStat(0);
// c2->Divide(1,3,0.001,0.001,0);
// c2->cd(1);
// histoW->GetXaxis()->SetTitle("z [cm]");
// histoW->GetYaxis()->SetTitle("r [cm]");
// histoW->Draw("COLZ");
// c2->cd(2);
// histoT->GetXaxis()->SetTitle("z [cm]");
// histoT->GetYaxis()->SetTitle("r [cm]");
// histoT->Draw("COLZ");
// c2->cd(3);
// histoV->GetXaxis()->SetTitle("v_Drift [cm/ns]");
// //histoV->GetYaxis()->SetTitle("r [cm]");
// histoV->Draw();

// c2->Update();



}
