{

#include "TH2D.h"

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  // Load this example libraries
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libField");
  gSystem->Load("libPassive");

  gSystem->Load("libMvd");
  gSystem->Load("libEmc");
  gSystem->Load("libDrcProp");
  gSystem->Load("libDrc");
  gSystem->Load("libGen");
  gSystem->Load("libgenfit");
  gSystem->Load("libtpc");
  gSystem->Load("librecotasks");

gROOT->Reset();
gStyle->SetPalette(1);

double _tpcMinR=15;	
double _tpcMaxR=45;         
double _tpcMinZ=-45;        
double _tpcMaxZ=115;

int rBins = 50;
int zBins = 200;

double xWidth = (_tpcMaxR - _tpcMinR) / rBins;
double zWidth = (_tpcMaxZ - _tpcMinZ) / zBins;

double bFieldX;

PndMultiField* bField= new PndMultiField();

PndTransMap* map= new PndTransMap("TransMap", "R");
PndDipoleMap* map1= new PndDipoleMap("DipoleMap", "R");
PndSolenoidMap* map2= new PndSolenoidMap("SolenoidMap", "R");
bField->AddField(map);
bField->AddField(map1);
bField->AddField(map2);
bField->Init();


double minR = 0;
double maxR = 0;  //max and min values for scale fitting;

TH2D* histo = new TH2D("B-Field-Histo","radial B-Field [T]",
                       zBins,_tpcMinZ,_tpcMaxZ,
		       rBins,_tpcMinR,_tpcMaxR);

for (int nr=0; nr<rBins; nr++)
{
  for (int nz=0; nz<zBins; nz++)
  {
    double X = _tpcMinR + (nr + 0.5) * xWidth;
    double Z = _tpcMinZ + (nz + 0.5) * zWidth;
    bFieldX = bField->GetBx(X,0,Z) * 0.1;  
    if(bFieldX < minR)          
      minR = bFieldX;
    if(bFieldX > maxR)
      maxR = bFieldX;
    histo->SetBinContent(nz+1,nr+1,bFieldX);
  }
}

TH2D* histo2 = new TH2D("B-Field-Histo","z-component of B-Field [T]",
                       zBins,_tpcMinZ,_tpcMaxZ,
		       rBins,_tpcMinR,_tpcMaxR);


for (int nr=0; nr<rBins; nr++)
{
  for (int nz=0; nz<zBins; nz++)
  {
    double X = _tpcMinR + (nr + 0.5) * xWidth;
    double Z = _tpcMinZ + (nz + 0.5) * zWidth;
    bFieldX = bField->GetBz(X,0,Z) * 0.1;  
    histo2->SetBinContent(nz+1,nr+1,bFieldX);
  }
}


double contR[20];
double scaleR =(maxR - minR)/20 ;
for(int i=0; i<20; i++) 
  contR[i] = i*scaleR + minR;
  
//fix the scale:
histo->SetContour(20,contR);



TCanvas* c = new TCanvas("multifield_bfield","Multifield B-Field",1);
gStyle->SetOptStat(0);
c->Divide(1,2);

c->cd(1);
histo->GetXaxis()->SetTitle("z [cm]");
histo->GetYaxis()->SetTitle("r [cm]");
histo->Draw("COLZ");

c->cd(2);
histo2->GetXaxis()->SetTitle("z [cm]");
histo2->GetYaxis()->SetTitle("r [cm]");
histo2->Draw("COLZ");

c->Update();

}
