#include "detector.h"
#include "../../src/TCalign.h"
#include "../../src/TCcluster.h"
#include "../../src/TCevent.h"
#include "alignment.h"
#include <TRandom3.h>
#include <iostream>
#include <TFile.h>
#include <algorithm>
#include <TROOT.h>
#include <TTree.h>
#include <string>
#include <vector>
#include "../../src/ConfigFile.h"
#include <boost/tokenizer.hpp>
using namespace std;

void failedConf(std::string);
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

void Alignment::readTracks(string tracks){
  TFile* file= TFile::Open(tracks.c_str());
  TTree *t =(TTree*)gROOT->FindObject("at2");
  if(t==NULL){
    t=(TTree*)gROOT->FindObject("at_tr");
  }
  int nEvt = t->GetEntries();
  TCevent *ev = 0;
  
  t->SetBranchAddress("event",&ev);
  int counter=0;
  for(int i=0;i<nEvt;++i){
    t->GetEntry(i);
    for(unsigned int j=0;j<ev->nTracks();++j){
      TCtrack track = *(ev->getTrack(j));

      
      bool a1 =false;
      bool a2 =false;
      bool a3 =false;
      bool a4 =false;
      bool a5 =false;
      bool a6 =false;
      bool a7 =false;
      bool a8 =false;
      
       for(int j=0;j<track.nCl();j++){
         int id = track.getCl(j).getId();
         if(id==1){
           a1=true;
         }
         if(id==2){
           a2=true;
         }
         if(id==3){
           a3=true;
         }
         if(id==4){
           a4=true;
         }
         if(id==5){
           a5=true;
         }
         if(id==6){
           a6=true;
         }
	 if(id==7){
	   a7=true;
	 }
	 if(id==8){
	   a8=true;
	 }
       }
       if(a1&&a2&&a3&&a4&&a5&&a6&&a7&&a8){
        tracks_real.push_back(track);
        ++counter;
       }
       

    }
    

  }

  cout<<"total tracks "<<counter<<endl;;  
  file->Close();
}

void Alignment::readDetectors(bool simulation_)
{
  TCalign *reader = TCalign::getInstance(infile);
  reader->clear();
  reader->read(infile);
  cout<<"infile "<<infile<<endl;
  std::vector<int> ids = reader->getLoadedIDs();
  
  for(unsigned int i=0;i<ids.size();i++){
    if(ids[i]>0&&ids[i]<100){
      TVector3 trans;
      TMatrixT<double> rot(3,3);
      double pitch,res, theta, phi, psi;
      
      reader->getConv(ids[i],trans,rot,pitch,theta,phi,psi,res);
      Detector* detector = new Detector(ids[i],
                                        trans.x(), trans.y(), trans.z(),
                                        trans.x(),trans.y(),trans.z(),
                                        theta,theta,
                                        pitch,
                                        0,0,0,0,
                                        res);
      detectors.push_back(detector);
    }
  }
  if(simulation){
    cout<<"simrealalign read-----------------------------------------------------***********"<<endl;
    reader->clear();
    reader->read("Alignmentfiles/simRealAlign.txt");
    assert(reader->getLoadedIDs().size()==ids.size());
    for(unsigned int i=0;i<ids.size();i++){
      TVector3 trans2;
      TMatrixT<double> rot2(3,3);
      double pitch2,res2, theta2, phi2, psi2;
      reader->getConv(ids[i],trans2,rot2,pitch2,theta2,phi2,psi2,res2);
      cout<<"theta2 "<<theta2<<endl;
      detectors[i]->setErrors(trans2[0],trans2[1],trans2[2],theta2);
    }
  }
  cout<<"before sort"<<endl;
  sort(detectors.begin(),detectors.end(),sortDetector());//sort based on z. not sure if it is necesary
  cout<<"after sort"<<endl;
} 

Alignment::Alignment(string conffile){
  std::cout << "Opening config file..." << std::endl;
  ConfigFile cf( conffile.c_str() );
  string tracks;
  alignU_=false;
  alignZ_=false;
  alignT_=false;
  alignP_=false;
  if(!(cf.readInto(infile , "Alignment input") )) failedConf("Alignment input");
  if(!(cf.readInto(outfile , "Alignment output") )) failedConf("Alignment output");
  if(!(cf.readInto(simulation, "Simulation") )) failedConf("Simulation");
  if(!simulation){
    cf.readInto(tracks, "Track file");
  }else{
    tracks="";
  }
  if(!cf.readInto(histogramFile, "HistogramFile")){
    histogramFile="histogramfailed.root";
  }
  cf.readInto(alignU_, "AlignU");
  cf.readInto(alignZ_, "AlignZ");
  cf.readInto(alignT_, "AlignT");
  cf.readInto(alignP_, "AlignP");
  cout<<"align p "<<alignP_<<endl;
  using namespace std;
  using namespace boost;
  /*
    Loading detectorIDs for Fixing
  */
  
  /*
    first detectors where u is fixed-----------------------------------------
  */
  string s = "this is,  a test";
  cf.readInto(s,"fixU");
  tokenizer<> tok(s);  
  cout<<"fixing u"<<endl;
  for(tokenizer<>::iterator beg=tok.begin();beg!=tok.end();++beg){
    std::istringstream istr1(*beg);
    int number;
    istr1>>number;
    cout<<number<<endl;
    lockedU.push_back(number);
  }
  /*
    first detectors where Z is fixed-----------------------------------------
  */
  s="";
  cf.readInto(s,"fixZ");
  tokenizer<> tok2(s);
  cout<<"fixing z"<<endl;
  for(tokenizer<>::iterator beg2=tok2.begin();beg2!=tok2.end();++beg2){
    std::istringstream istr1(*beg2);
    int number;
    istr1>>number;
    cout<<number<<endl;
    lockedZ.push_back(number);
  }
  /*
    first detectors where T is fixed-----------------------------------------
  */
  s="";
  cf.readInto(s,"fixT");
  tokenizer<> tok3(s);
  cout<<"fixing T"<<endl;
  for(tokenizer<>::iterator beg3=tok3.begin();beg3!=tok3.end();++beg3){
    std::istringstream istr1(*beg3);
    int number;
    istr1>>number;
    cout<<number<<endl;
    lockedT.push_back(number);
  }
  /*
    first detectors where p is fixed-----------------------------------------
  */
  s="";
  cf.readInto(s,"fixP");
  tokenizer<> tok4(s);
  cout<<"fixing P"<<endl;
  for(tokenizer<>::iterator beg4=tok4.begin();beg4!=tok4.end();++beg4){
    std::istringstream istr1(*beg4);
    int number;
    istr1>>number;
    cout<<number<<endl;
    lockedP.push_back(number);
  }

  std::cout<<"Alignment-object ctor"<<std::endl;
  readDetectors(simulation);
  if(simulation){
    generateTracks(50000,150,2,2,2,2);
  }else{
    cout<<"reading tracks"<<endl;
    readTracks(tracks);
  }
  cout<<"al ctor done"<<endl;
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
  C_INITUN(11,1000000000.0);
  
  for(unsigned int i = 0; i<detectors.size();i++){
    int id=detectors[i]->getId();
    hists_det.push_back(new TH1D(TString::Format("dU_%i",id),TString::Format("dU_%i",id),3000,-10,10));
    profiles_det.push_back(new TProfile(TString::Format("dU_vs_U%i",id),TString::Format("dU_vs_U%i",id),1000,10,10,-10,10));
    hists_det_testX.push_back(new TH2D(TString::Format("U_vs_X%i",id),TString::Format("Track_x"),200,0,25,200,-5,10));
    hists_det_testY.push_back(new TH2D(TString::Format("U_vs_Y%i",id),TString::Format("Track_y"),200,0,25,200,-5,10));
  }
  cout<<endl<<"marker1"<<endl;
  /*
    Here you can choose which alignment to run
  */
  cout<<"Empty Histograms created"<<endl;
  cout<<"Starting to send data to Millepede"<<endl;

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
      Fixing detector coordinates given from config file
    */
    int detID =detectors[i]->getId();
    if(find(lockedU.begin(),lockedU.end(),detID)!=lockedU.end()){
      C_PARSIG(i*NPARPLAN+1,0.0);	//!< fix u
      cout<<"Fixed U for "<<detID<<endl;
    }
    if(find(lockedZ.begin(),lockedZ.end(),detID)!=lockedZ.end()){
      C_PARSIG(i*NPARPLAN+2,0.0);	//!< fix Z
      cout<<"Fixed Z for "<<detID<<endl;
    }
    if(find(lockedP.begin(),lockedP.end(),detID)!=lockedP.end()){
      C_PARSIG(i*NPARPLAN+4,0.0);	//!< fix Pitch 
      cout<<"Fixed P for "<<detID<<endl;
    }
    if(find(lockedT.begin(),lockedT.end(),detID)!=lockedT.end()){
      C_PARSIG(i*NPARPLAN+3,0.0);	//!< fix Theta 
      cout<<"Fixed Theta for "<<detID<<endl;
    }
          
    
  }
  cout<<endl<<"marker 2 before iteration"<<endl;
  if(simulation){
    cout<<"simulation"<<endl;
    cout<<"tracks length "<<tracks_sim.size()<<endl;
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
        double z_=detectors[j]->getZ();

        double cosT_=detectors[j]->getCosT();
        double sinT_=detectors[j]->getSinT();
        float sigma_=(float)detectors[j]->getSigma();

        float u_hit=(float)hit.first+x*cosT_+y*sinT_ ;


        //        int detID = detectors[j]->getId();
        
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
        dergb[NPARPLAN*j+1]=  cosT_*tx + sinT_*ty;                                       //!< /d dz
        dergb[NPARPLAN*j+2]= -sinT_*(x0+tx*(z_)-x) + cosT_*(y0+ty*(z_)-y);          //!< /d dtheta
        dergb[NPARPLAN*j+3]=  cosT_*(x0+tx*(z_)-x) + sinT_*(y0+ty*(z_)-y);              //!< /d dpitch
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
      double tx = tracks_real[i].getAx();
      double y0 = tracks_real[i].getBy();
      double ty = tracks_real[i].getAy();
      /*
      cout<<"*************************************************************************************"<<endl;
      cout<<"Track parameters for track "<<i<<endl;
      cout<<"x0 = "<<x0<<"  tx = "<<tx<<endl;
      cout<<"y0 = "<<y0<<"  ty = "<<ty<<endl;
      cout<<"--------------------------------------------"<<endl;
      */
      for(unsigned int j=0;j<detectors.size();j++){  
        double x=detectors[j]->getX();
        double y=detectors[j]->getY();
        double z_=detectors[j]->getZ();
        float sigma_=(float)detectors[j]->getSigma();
        double cosT_=detectors[j]->getCosT();
        double sinT_=detectors[j]->getSinT();
        
        int detID = detectors[j]->getId();
        TCcluster hit_cluster = tracks_real[i].getClById(detID,0);
        float u_hit=(float)hit_cluster.posUVW().x()+x*cosT_+y*sinT_ ;
        /*
        cout<<"Detector parameters for detector "<<detID<<endl;
        cout<<"cos(phi) = "<<cosT_<<"  sin(phi) = "<<sinT_<<endl;
        cout<<"x = "<<x<<"  y = "<<y<<"  z = "<<z_<<endl;
        cout<<"Hit point in detector u = "<<hit_cluster.posUVW().x()<<endl;
        cout<<"_________________________________________"<<endl;
        */
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
        dergb[NPARPLAN*j+1]=  cosT_*tx + sinT_*ty;                                       //!< /d dz
        dergb[NPARPLAN*j+2]= -sinT_*(x0+tx*(z_)-x) + cosT_*(y0+ty*(z_)-y);          //!< /d dtheta
        dergb[NPARPLAN*j+3]=  cosT_*(x0+tx*(z_)-x) + sinT_*(y0+ty*(z_)-y);              //!< /d dpitch
        /*
          Sending this information to millepede
        */
        equloc_(dergb,derlc,&u_hit,&sigma_);  //!< book local/global


        TVector3 resid = hit_cluster.getRes();
        hists_det[j]->Fill(resid.x()); /*u_rec - u_hit*/
        profiles_det[j]->Fill(hit_cluster.posUVW()[0],resid(0)); 
        hists_det_testX[j]->Fill((x0+tx*z_),hit_cluster.posUVW()[0]);
        hists_det_testY[j]->Fill((y0+ty*z_),hit_cluster.posUVW()[0]);

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
  TCalign* reader = TCalign::getInstance(infile);
  reader->clear();
  reader->read(infile);
  std::vector<int> ids = reader->getLoadedIDs();
  for(unsigned int n=0;n<detectors.size();n++){
    TVector3 trans;
    TMatrixT<double> rot(3,3);
    double pitch,res;
    double theta, phi, psi;
    cout<<endl<<"ID:"<<detectors[n]->getId()<<endl;
    reader->getConv(detectors[n]->getId(),trans,rot,pitch,theta,phi,psi,res);
    double du, dx, dy,dTheta,dz,dp;
    du=par[NPARPLAN*n+0];
    dz=par[NPARPLAN*n+1];
    dTheta=par[NPARPLAN*n+2];
    dp=par[NPARPLAN*n+3];
    dx = du*cos(theta);
    dy = du*sin(theta);

    theta+=dTheta;
    trans[0]+=dx;
    trans[1]+=dy;
    trans[2]+=dz;
    pitch=pitch*(1+dp);

    cout<<"dTheta: "<<dTheta<<endl;
    cout<<"dPitch: "<<dp<<endl;
    cout<<"du: "<<du<<"\t\t dx: "<<dx<<"\t\t dy: "<<dy<<"\t\t dz: "<<dz<<endl;
    rot=TCalign::eulerMatrix(theta,phi,psi);
    reader->setConv(detectors[n]->getId(),trans,rot,pitch,theta,phi,psi,res);
  }
  reader->write(outfile);
  //open file for histograms
  
  TFile* file = new TFile(histogramFile.c_str(),"RECREATE");
  for(unsigned int i = 0; i<hists_det.size();i++){
    hists_det[i]->Write();
    profiles_det[i]->Write();
    hists_det_testX[i]->Write();
    hists_det_testY[i]->Write();
  }
  cout<<"Histograms written to file"<<endl;
  file->Close();
  delete file;
  for(unsigned int i = 0; i<hists_det.size();i++){
    delete hists_det[i];
    delete profiles_det[i];
    delete hists_det_testX[i];
    delete hists_det_testY[i];
  }

  
}

void failedConf(std::string var) {
  std::cerr << "Reading parameter " << var << " from conf file failed ->abort"
			<< std::endl;
  throw;
}
