#include<iostream>
#include<sstream>
#include<string>


#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TVector3.h"
#include "TMatrixT.h"
#include "TApplication.h"
#include "TObject.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TF1.h"


#include "dataReader.h"
#include "Pedestals.h"
#include "Calib.h"
#include "ConfigFile.h"

#include "PndTpcSample.h"
#include "PndTpcPSAplot.h"
#include "PndTpcSimplePSAStrategy.h"
#include "PndTpcPSA_TOT1.h"
#include "PndTpcDigiMapper.h"
#include "PndTpcCluster.h"
#include "PndTpcClusterFinder.h"
#include "PndTpcClusterFinderSimple.h"


#include"TCalign.h"
#include"TCcluster.h"
#include"TCabsPatternReco.h"
#include"TChoughPatternReco.h"
#include"TCtrack.h"

using namespace std;



void failedConf(std::string);

int main(int argc, char* argv[]) {
  TCtrack *outputTrack = new TCtrack();

  if(argc != 4) {
	cerr << "NumArgs is not 3. Args: infile outfile configfile" << endl;
	throw;
  }

  string infile(argv[1]);
  string outfile(argv[2]);
  string conffile(argv[3]);
  std::cout << "Opening config file..." << std::endl;
  ConfigFile cf( conffile.c_str() );

  Calib::getInstance()->setKey(infile);
  std::string pedFileName(Calib::getInstance()->get_pedFile());
  new Pedestals(pedFileName);
  std::map<int,double> ped_m;
  std::map<int,double> ped_s;
  Pedestals::getPedestals(ped_m,ped_s);
  
  Calib* calib = Calib::getInstance();


  TFile* file = TFile::Open(infile.c_str());
  if(file == NULL) {
	cerr << "Input file couldn't be loaded -> abort" <<endl;
	throw;
  }

  TTree* intree = (TTree*)file->Get("datatree_m");
  if(intree == NULL) {
	cerr << "Tree couldn't be loaded -> abort" <<endl;
	throw;
  }

  //PndTpcAbsPSAStrategy*_psa= new PndTpcSimplePSAStrategy(10); // threshold (in adc channels)
  PndTpcAbsPSAStrategy*_psa= new PndTpcPSA_TOT1();
  
  dataReader myReader(intree);

  TCevt* event;
  McIdCollection dummyID;
  McId dummy1(1,1);
  dummyID.AddID(dummy1);

  TApplication theApp("myApp",NULL,NULL);

  gROOT->Macro("macro/christian_style.C");

  PndTpcGas * _gas= new PndTpcGas("ARGON-70_CO2-30_B0.1_PRES1013.asc",250);

  PndTpcGem *_gem=new PndTpcGem(5000,           // Gain
				0.02);          // Spread

  PndTpcPadShapePool *_padShapes = new PndTpcPadShapePool("TestChamberPad.dat",
							  *_gem,
							  0.5, // lookup range
							  0.02, // Lookup Step
							  0.01); // LookupIntegrationStep

  PndTpcPadPlane *_padPlane= new PndTpcPadPlane("padplane.dat", _padShapes);



  PndTpcDigiMapper* mapper = PndTpcDigiMapper::getInstance(false);
  double _zGem=0.;
  double _t0=0.;
  double _sampleFreq = 10.;
  mapper->init(_padPlane,_gem,_gas,_zGem,_t0,_sampleFreq);

  std::vector<PndTpcDigi*> digis;

  int clust_choice;  
  int timewindow_chr,timewindow_seb,detId;
  if(!(cf.readInto(clust_choice , "CLUSTERING") )) failedConf("CLUSTERING");
  if(!(cf.readInto( timewindow_chr, "TIMEWINDOW_CLUST_CHR") )) failedConf("TIMEWINDOW_CLUST_CHR");
  if(!(cf.readInto( timewindow_seb, "TIMEWINDOW_CLUST_SEB") )) failedConf("TIMEWINDOW_CLUST_CHR");
  if(!(cf.readInto( detId, "TPCDETID") )) failedConf("TPCDETID");

  std::vector<PndTpcCluster*> *clusters = new std::vector<PndTpcCluster*>;
  PndTpcAbsClusterFinder *clusterfinder;
  if(clust_choice == 1) {
    clusterfinder = new PndTpcClusterFinderSimple(PndTpcDigiMapper::getInstance()->getPadPlane(),clusters,timewindow_chr);
  }
  else {
    if(clust_choice == 2) {
      clusterfinder = new PndTpcClusterFinder(PndTpcDigiMapper::getInstance()->getPadPlane(),clusters,timewindow_seb);
    }
    else {
      std::cerr << "Wrong value for clusterfinder was given: " << clust_choice << " ->abort" << std::endl;
      throw;
    }
  }
  clusterfinder->saveRaw();
  clusterfinder->checkConsistency();
  
  double ellipse_ay,ellipse_by;
  int minselhits,visflag;
  if(!(cf.readInto( ellipse_ay, "ELLIPSE_AY") )) failedConf("ELLIPSE_AY");
  if(!(cf.readInto( ellipse_by, "ELLIPSE_BY") )) failedConf("ELLIPSE_BY");
  
  TFile* rootOutfile = new TFile(outfile.c_str(),"RECREATE");
  rootOutfile->cd();
  TTree* outTree = new TTree("at","TPC test chamber analysis tree");
  outTree->Branch("track","TCtrack",&outputTrack,32000,99);
  
  while(1) {
    outputTrack->clear();
    event = myReader.readEvent();
    if(event == NULL) {
      break;
    }
    if( event->Nchan(ped_s) > 64) {
      std::cout << "more than 64 pads fired -> skipping" << std::endl;
      continue;
    }
    cout << "Processing Event with trigger ID: " << event->trigger << endl;
    
    int maxtrigger;
    if(!(cf.readInto( maxtrigger, "MAXTRIGGER") )) failedConf("MAXTRIGGER");
    if(event->trigger>maxtrigger)break;
    
    //clean up
    for(unsigned int i=0;i<digis.size();i++) {
      delete digis.at(i);
    }
    
    digis.clear();
    for(unsigned int i=0;i<clusters->size();i++) {
      delete clusters->at(i);
    }
    clusters->clear();
    clusterfinder->reset();
    
    //Pulse Shape Analysis
    for(int iPad=0;iPad<128;iPad++) {
      
      //make PndTpcSamples for signals in event tree
      std::vector<PndTpcSample*> samplelist;
      int nsamp=0;
      for(int iSample=0;iSample<150;iSample++) {
	if(event->time[iPad][iSample]>0 && 
	   event->charge[iPad][iSample] < 100000 && 
	   event->charge[iPad][iSample]>=0) {
	  PndTpcSample* aSample = new PndTpcSample(event->time[iPad][iSample],event->charge[iPad][iSample],iPad,dummyID);
	  samplelist.push_back(aSample);
	  if(event->charge[iPad][iSample] > ped_s[iPad]*4.) nsamp++;
	}
      }
      
      
      
      if(nsamp>3) {
	_psa->Process(samplelist,digis,ped_s[iPad]*4.);	 
	std::ostringstream mytitle;
	mytitle << "Pulse Shape Analysis -- padID " << iPad;
	PndTpcPSAplot myplot(&samplelist,&digis,NULL,NULL,mytitle.str());
	int psaplot;
	if(!(cf.readInto( psaplot, "PSAPLOT") )) failedConf("PSAPLOT");
	if(psaplot) myplot.Draw();
      }
      //clean up
      for(unsigned int i=0;i<samplelist.size();i++) {
	delete samplelist.at(i);
      }
      samplelist.clear();
    } // end of PSA
    
    
    //Clustering (at the moment: one cluster per pad and digi)
    clusterfinder->process(digis);
    int nclust = clusters->size();
    if(nclust<2){
      continue;//discard event
    }
    //NEW



    std::vector<TCcluster> TCc;
    for(int i=0;i<nclust;++i){
      TCcluster _c(*(clusters->at(i)),detId);
      TCc.push_back(_c);
    }

    TCabsPatternReco *pr = new TChoughPatternReco();
    pr->process(TCc);
    delete pr;	



    outputTrack->addClusters(TCc);

    outputTrack->fit(detId);
    //outputTrack->draw();
    rootOutfile->cd();
    outTree->Fill();
    
  }// event loop
  
  rootOutfile->cd();
  outTree->Write();
  rootOutfile->Close();
}


void failedConf(std::string var) {
  std::cerr << "Reading parameter " << var << " from conf file failed ->abort"
			<< std::endl;
  throw;
}

 
 
