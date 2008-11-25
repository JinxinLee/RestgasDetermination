#include "detector.h"
#include "../../src/TCalign.h"
#include "../../src/TCcluster.h"
#include "alignment.h"
#include <TRandom3.h>
#include <iostream>
#include <TFile.h>
#include <algorithm>
#include <TROOT.h>
#include <TTree.h>

using namespace std;
class sortDetector{
public:
  bool operator()(Detector *a1, Detector *a2){
    return a1->getZ()<a2->getZ();
  }
};

void Alignment::generateTracks(int amount,
                               double length,
                               double startwidht,
                               double startheight,
                               double endheight,
                               double endwidth)
{
  TRandom3 random(0);    
  for(int i=0;i<amount;i++){
    double x0 =random.Rndm()*startwidht-startwidht/2;
    double y0 =random.Rndm()*startheight-startheight/2;
    double tx0 =((random.Rndm()*endwidth-endwidth/2)-x0)/length;
    double ty0 =((random.Rndm()*endheight-endheight/2)-y0)/length;
    tracks_sim.push_back(Track(x0,y0,0,tx0,ty0));
  }
}

void Alignment::readTracks(){
  TFile* file= TFile::Open("out.root");//temporary solution, should make a option file
  TTree *t =(TTree*)gROOT->FindObject("at");
  int nEvt = t->GetEntries();
  TCtrack *tr = 0;
  
  t->SetBranchAddress("track",&tr);
  
  for(int i=0;i<nEvt;++i){
    t->GetEntry(i);
    TCtrack track = *tr;
    tracks_real.push_back(track);
  }
  file->Close();
}

void Alignment::readDetectors(bool simulation_)
{
  TCalign* reader = TCalign::getInstance(infile);
  cout<<"infile "<<infile<<endl;
  std::vector<int> ids = reader->getLoadedIDs();
  
  for(unsigned int i=0;i<ids.size();i++){
    if(ids[i]>0&&ids[i]<20||ids[i]>50&&ids[i]<70){
      TVector3 trans;
      TMatrixT<double> rot(3,3);
      double pitch,res, theta, phi, psi;
      
      reader->getConv(ids[i],trans,rot,pitch,theta,phi,psi,res);
      Detector* detector = new Detector(ids[i],
                                        trans[0], trans[1], trans[2],
                                        trans[0],trans[1],trans[2],
                                        theta,theta,
                                        pitch,
                                        0,0,0,0,
                                        res);
      detectors.push_back(detector);
    }
  }
  if(simulation){
    reader->clear();
    reader->read("simRealAlign.txt");
    assert(reader->getLoadedIDs().size()==ids.size());
    for(unsigned int i=0;i<ids.size();i++){
      TVector3 trans2;
      TMatrixT<double> rot2(3,3);
      double pitch2,res2, theta2, phi2, psi2;
      reader->getConv(ids[i],trans2,rot2,pitch2,theta2,phi2,psi2,res2);
      detectors[i]->setErrors(trans2[0],trans2[1],trans2[2],theta2);
    }
  }
  sort(detectors.begin(),detectors.end(),sortDetector());//sort based on z. not sure if it is necesary
} 

Alignment::Alignment(bool simulation_,string infile_, string outfile_){
  infile=infile_;
  outfile=outfile_;
  simulation=simulation_;
  std::cout<<"Alignment-object ctor"<<std::endl;
  readDetectors(simulation);
  if(simulation){
    generateTracks(50000,100,2,2,2,2);
  }else{
    readTracks();
  }
}

void Alignment::doFit(){
  cout<<"NGLB "<<NGLB<<"NPARTRCK "<<NPARTRCK<<endl;
  /*
    initializing Millepede
    NGLB is the number of global parameters, ie. Alignment parameters
    NPARTRCK is the number of local paramters, ie Track parameters
    The two last numbers is #sigmas for cut, and verbosity.
    negative value means no output, 0 means some, and 1 means a lot
  */
  C_INITGL(NGLB,NPARTRCK,3,1);
  /*
    initializing the derivatives to zero
  */
  zerloc_(dergb,derlc);
  /*
    setting the iterate flag. Copied call to function from Compass,
    wil set me down and understand the parameters sendt two it soon
  */
  C_INITUN(11,100000.0);
  if(simulation){  
    for(unsigned int i = 0; i<detectors.size();i++){
      int id=detectors[i]->getId();
      hists_det.push_back(new TH1D(TString::Format("dU_%i",id),TString::Format("dU_%i",id),10000,-3,3));
      profiles_det.push_back(new TProfile(TString::Format("dU_vs_U%i",id),TString::Format("dU_vs_U%i",id),1000,-3,3,-1,1));
    }
  }
  cout<<endl<<"marker1"<<endl;
  /*
    Here you can choose which alignment to run
  */
  cout<<"Empty Histograms created"<<endl;
  cout<<"Starting to send data to Millepede"<<endl;
  bool alignU_ = true;
  bool alignZ_ = false;
  bool alignT_ = true;
  bool alignP_ = false;
  /*
    for each detector i set the sigma for the detectors to zero for that
    parameter. This is picked up from the fortran code, and that
    parameter is excluded from the fit
  */
  
  for( unsigned int i=0; i<detectors.size(); i++) {
    if (!alignU_) {C_PARSIG(i*NPARPLAN+1,0.0);}	//!< fix all u 
    if (!alignZ_) {C_PARSIG(i*NPARPLAN+2,0.0);}	//!< fix all z 
    if (!alignT_) {C_PARSIG(i*NPARPLAN+3,0.0);} //!< fix all theta
    if (!alignP_) {C_PARSIG(i*NPARPLAN+4,0.0);}  //!< fix all pitch
    /*
      fixing the first silicon detector with id 51 and 52
    */
    int detID =detectors[i]->getId();
    if(detID==3||detID==4||detID==7||detID==8){
      cout<<"Fix detid"<<(detectors[i]->getId())<<endl;
      C_PARSIG(i*NPARPLAN+1,0.0);	//!< fix all u 
      C_PARSIG(i*NPARPLAN+2,0.0);	//!< fix all z 
      C_PARSIG(i*NPARPLAN+4,0.0);	//!< fix all pitch
      //      if(detID==3||detID==4){
      C_PARSIG(i*NPARPLAN+3,0.0);	//!< fix all theta
        //}
      
    }   
  }
  cout<<endl<<"marker 2 before iteration"<<endl;
  if(simulation){
    cout<<"simulation"<<endl;
    for(unsigned int i=0;i<tracks_sim.size();i++){
      double x0 = tracks_sim[i].getX0();
      double y0 = tracks_sim[i].getY0();
      double tx = tracks_sim[i].getTx();
      double ty = tracks_sim[i].getTy();
      
      for(unsigned int j=0;j<detectors.size();j++){  
        pair<double,double> hit = detectors[j]->getHitU(tracks_sim[i]);
        
        hists_det[j]->Fill(hit.second-hit.first); /*u_rec - u_hit*/
        profiles_det[j]->Fill(hit.first,(hit.second-hit.first)); 
        
        double x=detectors[j]->getX();
        double y=detectors[j]->getY();
        double cosT_=detectors[j]->getCosT();
        double sinT_=detectors[j]->getSinT();
        
        float u_hit=(float)hit.first+x*cosT_+y*sinT_ ;
        double z_=detectors[j]->getZ();
        float sigma_=(float)detectors[j]->getSigma();
        int detID = detectors[j]->getId();
        
        /*
          calculate local derivatives, ie the derivatives with respect to
          track parameters. 
        */
        double dudx0  = cosT_;           //!< /d x0
        double dudy0  = sinT_;           //!< /d y0
        double dudtx = cosT_*z_;    //!< /d tx
        double dudty = sinT_*z_;    //!< /d ty
        
        //! store std local derivatives
        derlc[0]= dudx0;           
        derlc[1]= dudtx;   
        derlc[2]= dudy0;        
        derlc[3]= dudty; 
        
        //! calculate/store global derivatives   
        dergb[NPARPLAN*j]=-1;                                                     //!< /d du
        dergb[NPARPLAN*j+1]=  cosT_*tx - sinT_*ty;                                       //!< /d dz
        dergb[NPARPLAN*j+2]= -sinT_*(x0+tx*(z_)-x) - cosT_*(y0+ty*(z_)-y);          //!< /d dtheta
        dergb[NPARPLAN*j+3]=0/*  cosT_*(x0+tx*(z_)-x) - sinT_*(y0+ty*(z_)-y)*/;              //!< /d dpitch
        /*
          Sending this information to millepede
          
        */
        equloc_(dergb,derlc,&u_hit,&sigma_);  //!< book local/global
      }
      /*
        Doing a local fit
        That means fiting the track parameters and calculating submatrixes
        of the big matrix that needs to be inverted
      */
      fitloc_();
    }
    cout<<"after loop sim"<<endl;
  }else{
    cout<<"real "<<endl;
    for(unsigned int i=0;i<tracks_real.size();i++){
      double x0 = tracks_real[i].getBx();
      double y0 = tracks_real[i].getBy();
      double tx = tracks_real[i].getAx();
      double ty = tracks_real[i].getAy();
      
      for(unsigned int j=0;j<detectors.size();j++){  
        double x=detectors[j]->getX();
        double y=detectors[j]->getY();
        double cosT_=detectors[j]->getCosT();
        double sinT_=detectors[j]->getSinT();
        int detID = detectors[j]->getId();
        TCcluster hit_cluster = tracks_real[i].getClById(detID,0);
        float u_hit=(float)hit_cluster.posUVW()[0]+x*cosT_+y*sinT_ ;
        double z_=detectors[j]->getZ();
        float sigma_=(float)detectors[j]->getSigma();
        /*
          calculate local derivatives, ie the derivatives with respect to
          track parameters. 
        */
        double dudx0  = cosT_;           //!< /d x0
        double dudy0  = sinT_;           //!< /d y0
        double dudtx = cosT_*z_;    //!< /d tx
        double dudty = sinT_*z_;    //!< /d ty
        
        //! store std local derivatives
        derlc[0]= dudx0;           
        derlc[1]= dudtx;   
        derlc[2]= dudy0;        
        derlc[3]= dudty; 
        //! calculate/store global derivatives   
        dergb[NPARPLAN*j]=-1;                                                     //!< /d du
        dergb[NPARPLAN*j+1]=  cosT_*tx - sinT_*ty;                                       //!< /d dz
        dergb[NPARPLAN*j+2]= -sinT_*(x0+tx*(z_)-x) - cosT_*(y0+ty*(z_)-y);          //!< /d dtheta
        dergb[NPARPLAN*j+3]=0/*  cosT_*(x0+tx*(z_)-x) - sinT_*(y0+ty*(z_)-y)*/;              //!< /d dpitch
        /*
          Sending this information to millepede
        */
        equloc_(dergb,derlc,&u_hit,&sigma_);  //!< book local/global
      }
      /*
        Doing a local fit
        That means fiting the track parameters and calculating submatrixes
        of the big matrix that needs to be inverted
      */
      fitloc_();
    }
  }
  cout<<"Local fits done, doing global fit"<<endl;
  
  fitglo_(par); //!< minimize alignment parameters 
  
  C_PRTGLO(20); //!< Dump to screen
  TCalign* reader = TCalign::getInstance();
  std::vector<int> ids = reader->getLoadedIDs();
  for(unsigned int n=0;n<detectors.size();n++){
    TVector3 trans;
    TMatrixT<double> rot(3,3);
    double pitch,res;
    double theta, phi, psi;
    cout<<endl<<"ID:"<<detectors[n]->getId()<<endl;
    reader->getConv(detectors[n]->getId(),trans,rot,pitch,theta,phi,psi,res);
    trans[0]+=par[NPARPLAN*n+0];
    trans[1]+=par[NPARPLAN*n+1];
    trans[2]+=par[NPARPLAN*n+2];
    theta+=par[NPARPLAN*n+3];
    pitch+=par[NPARPLAN*n+4];
    cout<<"dTheta: "<<par[NPARPLAN*n+2]<<endl;
    cout<<"dPitch: "<<par[NPARPLAN*n+3]<<endl;
    cout<<"du: "<<par[NPARPLAN*n]<<"\t\t dz: "<<par[NPARPLAN*n+1]<<endl;
    reader->setConv(detectors[n]->getId(),trans,rot,pitch,theta,phi,psi,res);
  }
  reader->write(outfile);
  //open file for histograms
  if(simulation){
    TFile* file = new TFile("histograms.root","RECREATE");
    for(unsigned int i = 0; i<hists_det.size();i++){
      hists_det[i]->Write();
      profiles_det[i]->Write();
    }
    cout<<"Histograms written to file"<<endl;
    file->Close();
    delete file;
    for(unsigned int i = 0; i<hists_det.size();i++){
      delete hists_det[i];
      delete profiles_det[i];
    }
  }
}

