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
#include "THnSparse.h"
#include "TCanvas.h"
#include "TString.h"

#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <sstream>


void houghCPU() {


  bool CUT_CHAMBER=true;   //only collect hits with x>0;
  double CUT_DIST=1;      //cut on c
  
  unsigned int EVENT=6;
  double RIEMANNSCALING=40;

  TString dir = "../../DATA/";
  TString project = "Test10";

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

  int BIN_phi = 40;
  int BIN_theta = 40;
  
  int BIN_m = 100;
  int BIN_t = 100;
  int BIN_c = 40;

  double m_Max = 1.;
  double m_Min = -1.;
  double t_Max = 5.;
  double t_Min = -5.;
  double phi_Min = 0;
  double phi_Max = 180;

  
  TH3D* hitsReco  = new TH3D("bl","fd",100,-1,1,100,-1,1,100,0,1);
  TH3D* hitsMC  = new TH3D("bl31233","fd312321",100,-1,1,100,-1,1,100,0,1);
  TH3D* houghSpace  = new TH3D("bl21","Complete Hough Space",
			 BIN_phi,phi_Min,phi_Max,BIN_theta,0,180,100,-1,1);
  houghSpace->GetXaxis()->SetTitle("#Phi");
  houghSpace->GetYaxis()->SetTitle("#Theta");
  TH2D* hyperplane3D  = new TH2D("bl2","Sample of c(#Phi, #Theta)",
			      BIN_phi,phi_Min,phi_Max,BIN_theta,0,180);
  hyperplane3D->GetXaxis()->SetTitle("#Phi");
  hyperplane3D->GetYaxis()->SetTitle("#Theta");
  TH2D* phi_c  = new TH2D("phic","c-phi projection)",
			  BIN_phi,phi_Min,phi_Max,300,-CUT_DIST,CUT_DIST);
  TH2D* theta_c  = new TH2D("thetac","c-theta projection)",
			  BIN_theta,0,180,300,-CUT_DIST,CUT_DIST);

  TH2D* phi_c_mc  = new TH2D("phicMC","MC c-phi projection)",
			  BIN_phi/5,phi_Min,phi_Max,30,-CUT_DIST,CUT_DIST);
  TH2D* theta_c_mc  = new TH2D("thetacMC","MC c-theta projection)",
			  BIN_theta/5,0,180,30,-CUT_DIST,CUT_DIST);
  
  TH2D* houghRZ = new TH2D("vfg", "RZ hough space", 
			   BIN_m, m_Min, m_Max, BIN_t, t_Min, t_Max);
  houghRZ->GetXaxis()->SetTitle("m");
  houghRZ->GetYaxis()->SetTitle("t");
 
   //dimensions: phi, theta, c, m, t
  int bins[5] = {BIN_phi, BIN_theta, BIN_c, BIN_m, BIN_t};
  double mins[5] = {phi_Min,0,-1,m_Min,t_Min};
  double maxs[5] = {phi_Max, 180, 1, m_Max, t_Max};

  int CHUNKSIZE = 64000;
  
  THnSparse* fullHough =  new THnSparseF("fullHough", "5D Hough Space",
					 5, bins, mins, maxs);

  std::cout<<"\n &(*^*&^* CHUNK SIZE: "<<fullHough->GetChunkSize()
	   <<std::endl;
  

  std::vector<TVector3> riemannList;
  std::vector<TVector3> riemannListRZ;
  

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
	if(CUT_CHAMBER && pos.X() < 0.)
	  continue;
	thePoint->Position(pos);
	//pos.SetZ(0.);
	r = pos.Perp()/RIEMANNSCALING;
	phi = pos.Phi();
	
	double x_R = r * cos(phi)/(1+r*r);
	double y_R = r * sin(phi)/(1+r*r);
	double z_R = r*r/(1+r*r);
	hitsMC->Fill(x_R, y_R, z_R);
	
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
    if(CUT_CHAMBER && vec1.X() < 0.)
      continue;
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
    
    // if(phi<0) 
    //  phi += 180;      
           
    phi_c_mc->Fill(phi,c);
    theta_c_mc->Fill(theta,c);
        
  }
   

  // CLUSTER PART -----------------------------------------------------------



  //RIEMANN TRAFO ON CLUSTERS -----------------------------------------------
  //loop over clusters
  for(int cl=0; cl<nClusters; ++cl) {
    TVector3 pos = ((PndTpcCluster*)_clusters->At(cl))->pos();
    
    if(CUT_CHAMBER && pos.X() < 0.)
      continue;

    riemannListRZ.push_back(TVector3(pos.Perp(), 0., pos.Z()));

    //z = pos.Z()/15;
    //pos.SetZ(0.);
    r = pos.Perp()/RIEMANNSCALING;
    phi = pos.Phi();
    
    x_R = r * cos(phi)/(1+r*r);
    y_R = r * sin(phi)/(1+r*r);
    z_R = r*r/(1+r*r);

    //std::cout<<"x_R: "<<x_R<<"  y_R: "<<y_R<<"  z_R: "<<z_R<<std::endl;
    
    hitsReco->Fill(x_R, y_R, z_R);
    //surf->Fill(r, phi);
    pos.Print();
    riemannList.push_back(TVector3(x_R,y_R,z_R));
    
  }


  //ANALYSIS --------------------------------------------------------------

   double phiBinWidth=(double)180/BIN_phi;
   double thetaBinWidth=(double)180/BIN_theta;
  
   double mBinWidth = (m_Max-m_Min)/BIN_m;
   double tBinWidth = (t_Max-t_Min)/BIN_t;
  
   std::vector<TH2D*> histlist;
  
//   for(int rp=0; rp<riemannList.size(); ++rp) {
//     TVector3 point = riemannList[rp];
//     //TVector3 point = TVector3(1.,0.,0.);
//     point.Print();
//     TH2D* theHist;
//     for(int phi=0; phi<BIN_phi; ++phi) {
//       TVector3 n = TVector3(1.,0.,0.);
//       n.SetPhi((phi+0.5)*phiBinWidth*TMath::Pi()/180);
//       for(int theta=0; theta<BIN_theta; ++theta) {
// 	if(theta%10==0) {
// 	  std::string name = "phic_";
// 	  std::stringstream s;
// 	  s<<theta;
// 	  name.append(s.str());
// 	  if(rp==0&&phi==0){
// 	    histlist.push_back(new TH2D(name.c_str(),"c-phi projection)",
// 				      BIN_phi,0,180,200,-CUT_DIST,CUT_DIST));
// 	    (histlist[histlist.size()-1])->GetXaxis()->SetTitle("#Phi");
// 	  }
// 	}
// 	theHist=histlist[(int)theta/10];
// 	n.SetTheta(((theta+0.5)*thetaBinWidth)*TMath::Pi()/180);
// 	n.SetMag(1.0);
// 	double c = point*n;
// 	if(rp==0) 
// 	  hyperplane3D->SetBinContent(phi+1,theta+1,fabs(c));
// 	houghSpace->Fill((phi+0.5)*phiBinWidth,(theta+0.5)*thetaBinWidth,c);
// 	phi_c->Fill((phi+0.5)*phiBinWidth,c);
// 	theHist->Fill((phi+0.5)*phiBinWidth,c);	
// 	theta_c->Fill((theta+0.5)*thetaBinWidth,c);	
//       }
//     }
    
//     TVector3 pointRZ = riemannListRZ[rp];
//     double perp = pointRZ.X();
//     double z = pointRZ.Z();
//     for(int m=0; m<BIN_m; ++m) {
//       double M = (m+0.5)*mBinWidth + m_Min;
//       double t = perp * M * (-1.) + z;
//       houghRZ->Fill(M,t);
//     }
//   }


  for(int rp=0; rp<riemannList.size(); ++rp) {
    TVector3 point = riemannList[rp];
    //TVector3 point = TVector3(1.,0.,0.);
    point.Print();
    TH2D* theHist;

    TVector3 pointRZ = riemannListRZ[rp];
    double perp = pointRZ.X();
    double z = pointRZ.Z();

    int count = 0;
    
    for(int t=0; t<BIN_t; ++t) {
      
      double T = (t+0.5)*tBinWidth + t_Min;
      double M = (T-z) / (perp*(-1.));
      
      houghRZ->Fill(M,T);
            
      for(int phi=0; phi<BIN_phi; ++phi) {
	TVector3 n = TVector3(1.,0.,0.);
	n.SetPhi((phi+0.5)*phiBinWidth*TMath::Pi()/180 + phi_Min);
	for(int theta=0; theta<BIN_theta; ++theta) {
	  if(theta%10==0) {
	    std::string name = "phic_";
	    std::stringstream s;
	    s<<theta;
	    name.append(s.str());
	    if(rp==0 && phi==0 && count==0){
	      histlist.push_back(new TH2D(name.c_str(),"c-phi projection)",
					  BIN_phi,phi_Min,phi_Max,200
					  ,-CUT_DIST,CUT_DIST));
	      (histlist[histlist.size()-1])->GetXaxis()->SetTitle("#Phi");
	    }
	  }
	  theHist=histlist[(int)theta/10];
	  n.SetTheta(((theta+0.5)*thetaBinWidth)*TMath::Pi()/180);
	  n.SetMag(1.0);
	  double c = point*n;
	  if(rp==0) 
	    hyperplane3D->SetBinContent(phi+1,theta+1,fabs(c));
	  if(count==0) {
	    houghSpace->Fill((phi+0.5)*phiBinWidth+phi_Min,
			     (theta+0.5)*thetaBinWidth,c);
	    phi_c->Fill((phi+0.5)*phiBinWidth+phi_Min,c);
	    theHist->Fill((phi+0.5)*phiBinWidth+phi_Min,c);	
	    theta_c->Fill((theta+0.5)*thetaBinWidth,c);	
	  }
	  double x[5] = {(phi+0.5)*phiBinWidth+phi_Min,
			 (theta+0.5)*thetaBinWidth,c,
			 M, T};
	  
	  fullHough->Fill(x);
	}
      }
      count++;
    } 
  }


  
      
  TCanvas* c =  new TCanvas();
  c->Divide(1,2);
  c->cd(1);
  hitsReco->Draw();
  c->cd(2);
  hitsMC->Draw();
  //TCanvas* c2 = new TCanvas();
  // hyperplane3D->Draw("SURF1");
  TCanvas* c3 =  new TCanvas();
  houghSpace->Draw();
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
  TCanvas* c6 = new TCanvas();
  c6->Divide(5,4);
  for(int c=0; c<histlist.size(); c++) {
    c6->cd(c+1);
    (histlist[c])->Draw("COLZ");
  }
  TCanvas* c7 = new TCanvas();
  houghRZ->Draw("COLZ");

  TFile* outfile = new TFile("outputSHORT.root", "RECREATE");

  int nbins=fullHough->GetNbins();
  for(unsingend int ib=0;ib<nbins;++ib){
    fullHough->GetBinContent(linidx, (Int_t*)coord).
      

      }
 After the call, coord will contain the bin coordinate of each axis for the bin
 with linear index linidx. A possible call would be
   cout << hs.GetBinContent(0, coord);
   cout <<" is the content of bin [x = " << coord[0] "
        << " | y = " << coord[1] << "]" << endl;


  fullHough->Write();
  outfile->Close();
}
