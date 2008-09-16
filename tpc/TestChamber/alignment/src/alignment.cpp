#include "detector.h"
#include "tcalign.h"
#include "alignment.h"
#include <TRandom3.h>
#include <iostream>
#include <TFile.h>

using namespace std;
void Alignment::generateTracks(int amount,
			      double length,
			      double startwidht,
			      double startheight,
			      double endheight,
			      double endwidth)
{
       TRandom3* random = new TRandom3(0);    
       for(int i=0;i<amount;i++){
	   double x0 =random->Rndm()*startwidht-startwidht/2;
	   double y0 =random->Rndm()*startheight-startheight/2;
	   double tx0 =(random->Rndm()*endwidth-endwidth/2)/length;
	   double ty0 =(random->Rndm()*endheight-endheight/2)/length;
	   tracks.push_back(Track(x0,y0,0,tx0,ty0));
       }
}

void Alignment::readTracks(){
    /*
      implemented when i get acces to pandaroot
    */
}

void Alignment::readDetectors(bool simulation_){
    TCalign* reader = TCalign::getInstance();
   // std::cout<<"tcalign object created"<<std::endl;
    std::vector<int> ids = reader->getLoadedIDs();
 //   std::cout<<"ids vector size"<<ids.size()<<std::endl;
    
    for(unsigned int i=0;i<ids.size();i++){
	TVector3 trans;
	TMatrixT<double> rot(3,3);
	double pitch;
	double theta, phi, psi;
	std::cout<<"id: "<<ids[i]<<std::endl;
	reader->getConv(ids[i],trans,rot,pitch,theta,phi,psi);
	cout<<theta<<endl;
// 	for(int a=0;a<3;a++){
// 	    for(int b=0;b<3;b++){
// 		cout<<a<<b<<": "<<rot(a,b)<<" ";
// 	    }
// 	    cout<<endl;
// 	}
	Detector* detector = new Detector(ids[i],
					  trans[0], trans[1], trans[2],
					  0,0,0,
					  theta,0,
					  pitch,
					  0,0,0,0,
					  0.001);
	detectors.push_back(detector);
    }
    for(unsigned int i=0;i<detectors.size()/2;i++){
	detectors[i]->setErrors(detectors[(i+detectors.size()/2)]);
    }
    int tmp_size = detectors.size()/2;
    for(int i=0;i<tmp_size;i++){
	detectors.pop_back();
    }
    cout<<"size "<<detectors.size()<<endl;
}

Alignment::Alignment(bool simulation_){
    simulation=simulation_;
    std::cout<<"Alignment-object creator"<<std::endl;
    readDetectors(simulation);
    generateTracks(5000,10,10,10,10);
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
    C_INITGL(NGLB,NPARTRCK,3,0);
    /*
      initializing the derivatives to zero
    */
    zerloc_(dergb,derlc);
    /*
      setting the iterate flag. Copied call to function from Compass,
      wil set me down and understand the parameters sendt two it soon
    */
    C_INITUN(11,10000.);

    for(unsigned int i = 0; i<detectors.size();i++){
	hists_det.push_back(new TH1D(TString::Format("dU_%i",i+1),TString::Format("dU_%i",i+1),10000,-3,3));
	hists_det2.push_back(new TH2D(TString::Format("dU_vs_U%i",i+1),TString::Format("dU_vs_U%i",i+1),2000,-10,10,1500,-1,1));
	
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
	/*
	  Fixing the first and the last detector in each projection.
	  Detector 0 and 4 is the first one. and detector 3 and 7 is
	  the second one.
	*/
	if(i==0||i==4||i==3||i==7){
	     {C_PARSIG(i*NPARPLAN+1,0.0);}	//!< fix all x 
	     {C_PARSIG(i*NPARPLAN+2,0.0);}	//!< fix all y 
	     {C_PARSIG(i*NPARPLAN+3,0.0);}	//!< fix all z 
	     {C_PARSIG(i*NPARPLAN+4,0.0);}	//
							//!< fix all thet
	}

    }
    
    for(unsigned int i=0;i<tracks.size();i++){
	double x0 = tracks[i].getX0();
	double y0 = tracks[i].getY0();
	double tx = tracks[i].getTx();
	double ty = tracks[i].getTy();
	
	for(unsigned int j=0;j<detectors.size();j++){
	    pair<double,double> hit = detectors[j]->getHitU(tracks[i]);
	    hists_det[j]->Fill(hit.second-hit.first);
	    hists_det2[j]->Fill(hit.first,(hit.second-hit.first));
	    float u_=(float)(hit.first);
	    double cosT_=detectors[j]->getCosT();
	    double sinT_=detectors[j]->getSinT();
	    double z_=detectors[j]->getZ();
	    float sigma_=(float)(detectors[j]->getSigma());

	    

/*
  calculate local derivatives, ie the derivatives with respect to
  track parameters. 
*/
	    double dudx  = cosT_;           //!< /d x
	    double dudy  = sinT_;           //!< /d y
	    double dudtx = cosT_*(z_);    //!< /d tx
	    double dudty = sinT_*(z_);    //!< /d ty
	    
	    //! store std local derivatives
	    derlc[0]= dudx;           
	    derlc[1]= dudtx;   
	    
	    
	    derlc[2]= dudy;        
	    derlc[3]= dudty; 
	    
	    //! calculate/store global derivatives   
	    dergb[NPARPLAN*j]=cosT_;                                                     //!< /d dx
	    dergb[NPARPLAN*j+1]=  sinT_;                                                  //!< /d dy
	    dergb[NPARPLAN*j+2]= cosT_*tx-sinT_*ty;                                       //!< /d dz
	    dergb[NPARPLAN*j+3]= -sinT_*( x0+tx*(z_) ) + cosT_*( y0+ty*(z_) );           //!< /d theta
	    
	    /*
	      Sending this information to millepede
	    */
	    equloc_(dergb,derlc,&u_,&sigma_);  //!< book local/global
					       //
					       //derivatives, measurement, error
	    //addition to equloc method. it resets dergb and derlc
	    //after beeing called. That is, the places with parameters
	    //is equal to zero
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

//open file for histograms
    
    TFile* file = new TFile("histograms.root","RECREATE");
    for(unsigned int i = 0; i<hists_det.size();i++){
	hists_det[i]->Write();
	hists_det2[i]->Write();
    }
    cout<<"Histograms written to file"<<endl;
    file->Close();
    delete file;
    for(unsigned int i = 0; i<hists_det.size();i++){
	delete hists_det[i];
	delete hists_det2[i];
    }
}
