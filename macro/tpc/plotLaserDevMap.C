#include "TFile.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TPolyMarker3D.h"
#include "TF2.h"
#include "TH2D.h"
#include "PndTpcLaserStat.h"
#include "TVector3.h"
#include "TGraph2D.h"
#include <iostream>
#include <vector>
#include <cmath>


int npar=6;

double fitfcn(double* x, double* par){
  return par[0]/x[1]+par[1]/x[1]*(x[0]+40)+par[2]*x[1]+par[3]*x[0]+par[4]*x[0]*x[0]*x[0]+par[5]*x[0]*x[0];
}

void
plotLaserDevMap(TString file, int id){
  TFile* infile=TFile::Open(file);

  TTree* t=(TTree*)infile->Get("cbmsim");

  TClonesArray* statA=new TClonesArray("PndTpcLaserStat");
  //TClonesArray* trkA=new TClonesArray("

  t->SetBranchAddress("PndTpcLaserStat",&statA);
  //t->SetBranchAddress("PndTpcLaserTrack",&trkA);

  // loop over residuals
  t->GetEntry(0);
  
  std::vector<std::vector<double> >* _data=new std::vector<std::vector<double> >;
  
  // 14*27 bins
  int nbinsr=14;double rbin=27./(double)nbinsr;
  int nbinsz=20;double zbin=150./(double)nbinsz;
  int nbins=nbinsr*nbinsz;
  TGraph2D* devmap=new TGraph2D(nbins);
  TH2D* densmap=new TH2D("density","laser hit density",nbinsz,-39.5,109.5,
			 nbinsr,15.5,41.5);

  
  std::vector<double> val(nbins,0);
  std::vector<int> entrycount(nbins,0);

  // loop over residuals
  int nr=statA->GetEntriesFast();
  TPolyMarker3D* raw=new TPolyMarker3D(nr);
  std::cout<<nr<<" residuals found."<<std::endl;
  for(int i=0;i<nr;++i){
    PndTpcLaserStat* stat=(PndTpcLaserStat*)statA->At(i);
    if(stat==NULL){std::cout<<"Invalid stat object!"<<std::endl;continue;}

    //if(stat->getLaserID()%40!=id)continue;

    TVector3 proj=stat->getProjection();
    TVector3 res=stat->getRes();
    TVector3 cl_pos=stat->getPos();
    TVector3 sig=stat->getResSig();
    
      
    double r=proj.Perp();
    double z=proj.Z();
    double clr=cl_pos.Perp();
    
    
    double sig_r=std::sqrt((cl_pos.X()/clr*sig.X())*(cl_pos.X()/clr*sig.X())+(cl_pos.Y()/clr*sig.Y())*(cl_pos.Y()/clr*sig.Y()));
    
    
    densmap->Fill(z,r);

    TVector3 runit=proj;
    runit.SetZ(0);
    TVector3 resXY=res;
    resXY.SetZ(0);
    runit*=(1./runit.Mag());
    double dr=res*runit;
    
    
    int binz=(int)floor((z+40.)/zbin);if(binz>=nbinsz)continue;
    int binr=(int)floor((r-15.)/rbin);if(binr>=nbinsr)continue;
    int bin=binz+binr*nbinsz;

    int c=entrycount[bin];
    val[bin]=(val[bin]*c+dr)/(c+1);
    entrycount[bin]=c+1;
    
    //if(z<105)
    raw->SetNextPoint(z,r,dr);
    std::vector<double>vec = std::vector<double>(4,0);
    vec[0]=z; vec[1]=r; vec[2]=dr; vec[3]=sig_r;
    _data->push_back(vec);


  }// end loop over residuals

  // fill graph
  int counter=0;
  for(int j=0;j<nbinsr;++j){
    double r=15+j*rbin;
    for(int k=0;k<nbinsz;++k){
      double z=-40+k*zbin;
      std::cout<<z<<"  "<<r<<"  "<<val[counter]<<std::endl;
      devmap->SetPoint(counter,z+0.5*zbin,r+0.5*rbin,val[counter]);
      counter++;
    }
  }
 
  TCanvas* c=new TCanvas("c","c",10,10,800,800);
  c->Divide(1,3);
  c->cd(1);

  devmap->Draw("SURF1");

  //  TF2* f=new TF2("fcn",fitfcn,-40,150,15,42,npar);

  double params[7]={1.,1.,1.,1.,1.,1.,1.};
  // f->SetParameters(params);
  //f->FixParameter(0,0);
  //devmap->Fit("fcn","N0");

  c->cd(2);
  densmap->Draw("COLZ");

  c->cd(3);
  raw->Draw();

  //infile->Close();
  
  // TFile* outfile=new TFile("laserdevmap.root","RECREATE");
  // devmap->Write("devmap");
  // raw->Write("raw_Poly");
  // _data.Write("_data");
  // outfile->Close();
  
}
