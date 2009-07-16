#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TVector3.h"
#include "PndTpcCluster.h"
#include "PndTpcPoint.h"
//#include "FairMCPoint.h"
#include "TClonesArray.h"
#include "TH3D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TString.h"

#include <cmath>
#include <vector>
#include <map>


void houghCPU() {


  unsigned int EVENT=3;
  double RIEMANNSCALING=50;

  TString dir = "DATA/";
  TString project = "Test5";

  project=dir+project;
  TString mc_filename = project+".mc.root";
  TString reco_filename = project+".reco.root";

  
  TFile* reco_file =  new TFile(reco_filename);
  TTree* reco_tree = (TTree*)reco_file->Get("cbmsim");

  TFile* mc_file =  new TFile(mc_filename);
  TTree* mc_tree = (TTree*)mc_file->Get("cbmsim");

  TClonesArray* _clusters = new TClonesArray("PndTpcCluster");
  reco_tree->SetBranchAddress("PndTpcCluster", &_clusters);
  reco_tree->GetEntry(EVENT);

  TClonesArray* _points = new TClonesArray("PndTpcPoint");
  mc_tree->SetBranchAddress("PndTpcPoint", &_points);
  mc_tree->GetEntry(EVENT);
  
  int nClusters = _clusters->GetEntriesFast();
  int nPoints = _points->GetEntriesFast();
  
  double r, phi, z, x_R, y_R, z_R;

  int BIN_phi = 200;
  int BIN_theta = 200;
  
  TH3D* hist  = new TH3D("bl","fd",100,-1,1,100,-1,1,100,0,1);
  TH3D* hist_mc  = new TH3D("bl31233","fd312321",100,-1,1,100,-1,1,100,0,1);
  TH3D* blub  = new TH3D("bl21","Complete Hough Space",
			 BIN_phi,0,180,BIN_theta,0,180,100,-1,1);
  blub->GetXaxis()->SetTitle("#Phi");
  blub->GetYaxis()->SetTitle("#Theta");
  TH2D* histParam  = new TH2D("bl2","Sample of c(#Phi, #Theta)",
			      BIN_phi,0,180,BIN_theta,0,180);
  TH2D* phi_c  = new TH2D("phic","c-phi projection)",
			  BIN_phi,0,180,300,-1,1);
  TH2D* theta_c  = new TH2D("thetac","c-theta projection)",
			  BIN_theta,0,180,300,-1,1);

  TH2D* phi_c_mc  = new TH2D("phicMC","MC c-phi projection)",
			  BIN_phi/10,0,180,30,-1,1);
  TH2D* theta_c_mc  = new TH2D("thetacMC","MC c-theta projection)",
			  BIN_theta/10,0,180,30,-1,1);
  
  histParam->GetXaxis()->SetTitle("#Phi");
  histParam->GetYaxis()->SetTitle("#Theta");

  //  TH2D* surf  = new TH2D("surf","fdas2",100,0,2*TMath::Pi(),100,-1,1);

  std::vector<TVector3> riemannList;
  

  //FIND THE DIFFERENT MC TRACKS ----------------------------------------------
  std::map<int,int> trackIDs;
  for(unsigned int i=0; i<nPoints; ++i) {
    int ID = ((PndTpcPoint*) _points->At(i))->GetTrackID();
    trackIDs[ID]=1;
  }

  //SORT the hits and do RIEMANN TRAFO on MC hits
  std::vector<std::vector<TVector3>*> list;
  std::map<int,int>::iterator it;
  for(it = trackIDs.begin(); it!= trackIDs.end(); it++) {
    std::cout<<"Collecting Points with trackID "<<(*it).first<<std::endl;
    list.push_back(new std::vector<TVector3>);
    std::vector<TVector3>* theVec = list[list.size()-1];
    
    //loop over tpcpoints and do trafo 
    for(int p=0; p<nPoints; ++p) {
      PndTpcPoint* thePoint = (PndTpcPoint*)_points->At(p);
      if(thePoint->GetTrackID() == (*it).first) {
	TVector3 pos;
	thePoint->Position(pos);
	pos.SetZ(0.);
	r = pos.Mag()/RIEMANNSCALING;
	phi = pos.Phi();
	
	double x_R = r * cos(phi)/(1+r*r);
	double y_R = r * sin(phi)/(1+r*r);
	double z_R = r*r/(1+r*r);
	hist_mc->Fill(x_R, y_R, z_R);
	
	theVec->push_back(TVector3(x_R,y_R,z_R));
      }
    }
  }
  
  //BUILD ONE PLANE PER MC TRACK and get c, phi, theta.
  for(int t=0; t<list.size(); t++) {
    std::vector<TVector3>* theVec = list[t];
    int size = theVec->size();
    if(size < 10){
      std::cout<<"\nLess then 10 points in MC track ... aborting"<<std::endl;
      continue;
    }
    //get three points on spehere to span the plane
    TVector3 vec1, vec2, vec3, span1, span2, normal;
    vec1 = theVec->at(0); vec1.Print();
    vec3 = theVec->at(size-1); vec3.Print();
    vec2 = theVec->at(size/2); vec2.Print();
    span1 = vec3-vec1;
    span2 = vec2-vec1;
    normal = span1.Cross(span2);
    normal.SetMag(1.0);
    double c = normal*vec1;
    double phi=normal.Phi()*180/TMath::Pi();
    double theta=normal.Theta()*180/TMath::Pi();
    std::cout<<"MC-Track "<<t<<" has c: "<<c<<",   phi: "<<phi
	     <<",   theta: "<<theta<<std::endl;
    
    if(phi<0) 
      phi += 180;      
           
    phi_c_mc->Fill(phi,c);
    theta_c_mc->Fill(theta,c);
        
  }
  
  
  


  //RIEMANN TRAFO ON CLUSTERS -----------------------------------------------
  //loop over clusters
  for(int cl=0; cl<nClusters; ++cl) {
    TVector3 pos = ((PndTpcCluster*)_clusters->At(cl))->pos();
    //z = pos.Z()/15;
    pos.SetZ(0.);
    r = pos.Mag()/RIEMANNSCALING;
    phi = pos.Phi();
    
    x_R = r * cos(phi)/(1+r*r);
    y_R = r * sin(phi)/(1+r*r);
    z_R = r*r/(1+r*r);

    //std::cout<<"x_R: "<<x_R<<"  y_R: "<<y_R<<"  z_R: "<<z_R<<std::endl;
    
    hist->Fill(x_R, y_R, z_R);
    //surf->Fill(r, phi);

    riemannList.push_back(TVector3(x_R,y_R,z_R));
  }


  //ANALYSIS --------------------------------------------------------------

  double phiBinWidth=(double)180/BIN_phi;
  double thetaBinWidth=(double)180/BIN_theta;
  
  for(int rp=0; rp<riemannList.size(); ++rp) {
    TVector3 point = riemannList[rp];
    //TVector3 point = TVector3(1.,0.,0.);
    point.Print();
    for(int phi=0; phi<BIN_phi; ++phi) {
      TVector3 n = TVector3(1.,0.,0.);
      n.SetPhi((phi+0.5)*phiBinWidth*TMath::Pi()/180);
      for(int theta=0; theta<BIN_theta; ++theta) {
	n.SetTheta(((theta+0.5)*thetaBinWidth)*TMath::Pi()/180);
	n.SetMag(1.0);
	double c = point*n;
	if(rp==0) 
	  histParam->SetBinContent(phi+1,theta+1,fabs(c));
	blub->Fill((phi+0.5)*phiBinWidth,(theta+0.5)*thetaBinWidth,c);
	phi_c->Fill((phi+0.5)*phiBinWidth,c);
	theta_c->Fill((theta+0.5)*thetaBinWidth,c);
      }
    }
  }

  std::cout<<"phiBinWidth: "<<phiBinWidth<<std::endl;
  
  TCanvas* c =  new TCanvas();
  c->Divide(1,2);
  c->cd(1);
  hist->Draw();
  c->cd(2);
  hist_mc->Draw();
  TCanvas* c2 = new TCanvas();
  histParam->Draw("SURF1");
  TCanvas* c3 =  new TCanvas();
  blub->Draw();
  TCanvas* c4 = new TCanvas();
  c4->Divide(1,2);
  c4->cd(1);
  phi_c->Draw("COLZ");
  c4->cd(2);
  theta_c->Draw("COLZ");
  TCanvas* c5 = new TCanvas();
  c5->Divide(1,2);
  c5->cd(1);
  phi_c_mc->Draw("COLZ");
  c5->cd(2);
  theta_c_mc->Draw("COLZ");
 

  
  

}
