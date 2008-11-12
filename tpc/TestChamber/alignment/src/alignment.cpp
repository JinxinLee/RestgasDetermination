#include "detector.h"
#include "../../src/TCalign.h"
#include "alignment.h"
#include <TRandom3.h>
#include <iostream>
#include <TFile.h>
#include <algorithm>
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
	   tracks.push_back(Track(x0,y0,0,tx0,ty0));
       }
}

void Alignment::readTracks(){
    /*
      implemented when i get acces to pandaroot
    */
}

void Alignment::readDetectors(bool simulation_){
    TCalign* reader = TCalign::getInstance(infile);
   // std::cout<<"tcalign object created"<<std::endl;
    std::vector<int> ids = reader->getLoadedIDs();
 //   std::cout<<"ids vector size"<<ids.size()<<std::endl;
    
    for(unsigned int i=0;i<ids.size();i++){
      if(ids[i]>0&&ids[i]<20||ids[i]>50&&ids[i]<70){
        TVector3 trans;
        TMatrixT<double> rot(3,3);
        double pitch,res;
        double theta, phi, psi;
        TVector3 trans2;
        TMatrixT<double> rot2(3,3);
        double pitch2,res2;
        double theta2, phi2, psi2;
        
        std::cout<<"id: "<<ids[i]<<std::endl;
        reader->getConv(ids[i],trans,rot,pitch,theta,phi,psi,res);
        reader->getConv(ids[i]+20,trans2,rot2,pitch2,theta2,phi2,psi2,res2);
        cout<<"theta="<<theta;
        cout<<", x="<<trans[0]<<endl;
        
        Detector* detector = new Detector(ids[i],
                                          trans[0], trans[1], trans[2],
                                          trans2[0],trans2[1],trans2[2],
                                          theta,theta2,
                                          pitch,
                                          0,0,0,0,
                                          res);
        detectors.push_back(detector);
      }
    }
    sort(detectors.begin(),detectors.end(),sortDetector());
    cout<<endl;
    
    cout<<"size "<<detectors.size()<<endl;
}

Alignment::Alignment(bool simulation_,string infile_, string outfile_){
  infile=infile_;
  outfile=outfile_;
  simulation=simulation_;
  std::cout<<"Alignment-object ctor"<<std::endl;
  readDetectors(simulation);
  generateTracks(50000,100,2,2,2,2);
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

    for(unsigned int i = 0; i<detectors.size();i++){
      int id=detectors[i]->getId();
      hists_det.push_back(new TH1D(TString::Format("dU_%i",id),TString::Format("dU_%i",id),10000,-3,3));
      profiles_det.push_back(new TProfile(TString::Format("dU_vs_U%i",id),TString::Format("dU_vs_U%i",id),1000,-3,3,-1,1));
    }
/*
  Here you can choose which alignment to run
*/
    cout<<"Empty Histograms created"<<endl;
    cout<<"Starting to send data to Millepede"<<endl;
    bool alignX_ = true;
    bool alignY_ = true;
    bool alignZ_ = false;
    bool alignT_ = true;
    bool alignP_ = false;
/*
  for each detector i set the sigma for the detectors to zero for that
  parameter. This is picked up from the fortran code, and that
  parameter is excluded from the fit
*/
    for( unsigned int i=0; i<detectors.size(); i++) {
	if (!alignX_) {C_PARSIG(i*NPARPLAN+1,0.0);}	//!< fix all x 
	if (!alignY_) {C_PARSIG(i*NPARPLAN+2,0.0);}	//!< fix all y 
	if (!alignZ_) {C_PARSIG(i*NPARPLAN+3,0.0);}	//!< fix all z 
	if (!alignT_) {C_PARSIG(i*NPARPLAN+4,0.0);}    	//
							//!< fix all theta
        if (!alignP_) {C_PARSIG(i*NPARPLAN+5,0.0);}    	//!< fix all pitch
	/*
          fixing the first silicon detector with id 51 and 52
	*/
        int detID =detectors[i]->getId();
	if(detID==51||detID==52/*||detID==3||detID==4*/){
          cout<<"Fix detid"<<(detectors[i]->getId())<<endl;
	     C_PARSIG(i*NPARPLAN+1,0.0);	//!< fix all x 
	     C_PARSIG(i*NPARPLAN+2,0.0);	//!< fix all y 
             C_PARSIG(i*NPARPLAN+3,0.0);	//!< fix all z 
	     C_PARSIG(i*NPARPLAN+4,0.0);	//!< fix all thet
	     C_PARSIG(i*NPARPLAN+5,0.0);	//!< fix all pitch
	}
        
    }
    
    for(unsigned int i=0;i<tracks.size();i++){
	double x0 = tracks[i].getX0();
	double y0 = tracks[i].getY0();
	double tx = tracks[i].getTx();
	double ty = tracks[i].getTy();
	
	for(unsigned int j=0;j<detectors.size();j++){
	    pair<double,double> hit = detectors[j]->getHitU(tracks[i]);
	    hists_det[j]->Fill(hit.second-hit.first); /*u_rec - u_hit*/
	    profiles_det[j]->Fill(hit.first,(hit.second-hit.first)); 
	    float u_hit=(float)hit.first;
	    double cosT_=detectors[j]->getCosT();
	    double sinT_=detectors[j]->getSinT();
	    double z_=detectors[j]->getZ();
	    float sigma_=(float)detectors[j]->getSigma();
            double x=detectors[j]->getX();
            double y=detectors[j]->getY();
	    

/*
  calculate local derivatives, ie the derivatives with respect to
  track parameters. 
*/
	    double dudx  = cosT_;           //!< /d x
	    double dudy  = sinT_;           //!< /d y
	    double dudtx = cosT_*z_;    //!< /d tx
	    double dudty = sinT_*z_;    //!< /d ty
	    
	    //! store std local derivatives
	    derlc[0]= dudx;           
	    derlc[1]= dudtx;   
	    
	    
	    derlc[2]= dudy;        
	    derlc[3]= dudty; 
	    //cout<<"-cosT_: "<<-cosT_<<endl;
	    //! calculate/store global derivatives   
	    dergb[NPARPLAN*j]=-cosT_;                                                     //!< /d dx
	    dergb[NPARPLAN*j+1]=  sinT_;                                                  //!< /d dy
	    dergb[NPARPLAN*j+2]=  cosT_*tx - sinT_*ty;                                       //!< /d dz
	    dergb[NPARPLAN*j+3]= -sinT_*(x0+tx*(z_)-x) - cosT_*(y0+ty*(z_)-y);          //!< /d dtheta
            dergb[NPARPLAN*j+4]=0/*  cosT_*(x0+tx*(z_)-x) - sinT_*(y0+ty*(z_)-y)*/;              //!< /d dpitch
	    /*
	      Sending this information to millepede
	    */
	    equloc_(dergb,derlc,&u_hit,&sigma_);  //!< book local/global
					       //
					       //derivatives, measurement, error
	    //addition to equloc method. it resets dergb and derlc
	    //after beeing called. That is, the parameters are set to zero
	}
/*
  Doing a local fit
  That means fiting the track parameters and calculating submatrixes
  of the big matrix that needs to be inverted

*/
	fitloc_();
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
        cout<<"dTheta: "<<par[NPARPLAN*n+3]<<endl;
        cout<<"dPitch: "<<par[NPARPLAN*n+4]<<endl;
        cout<<"dx: "<<par[NPARPLAN*n]<<" \t\t dy: "<<par[NPARPLAN*n+1]<<"\t\t dz: "<<par[NPARPLAN*n+2]<<endl;
        reader->setConv(detectors[n]->getId(),trans,rot,pitch,theta,phi,psi,res);
    }
    reader->write(outfile);
//open file for histograms
    
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
