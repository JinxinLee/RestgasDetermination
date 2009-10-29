
//STL and C includes
#include <iostream>
#include <string>
#include <signal.h>

//ROOT headers
#include <TGraph.h>
#include <TChain.h>
#include <TFile.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TProfile.h>
#include <TApplication.h>
#include <TVector3.h>
#include <TRandom3.h>
#include <TSystem.h>
#include <TROOT.h>
//TestChamber includes

#include "TCcluster.h"
#include "TCalign.h"
#include "TCtrack.h"
#include "TCevent.h"
#include "ConfigFile.h"
#include "histContainers.h"


static bool controlC=false;
static int controlCs=0;
using std::cout;
using std::endl;
using std::string;
const double posTPC=108.0;

void signalHandler2(int signal) {
  if (signal==SIGINT) {
    cout<<"CTRL + C pressed saving and exiting, press again to abort"<<endl;
    controlC = true;
    if (controlCs!=0) {
      cout<<"aborting"<<endl;
      exit(1);
    }
    controlCs++;
    
  }
}

void failedConf(std::string var){
  std::cerr << "Reading parameter " << var << " from conf file failed ->abort"
	    << std::endl;
  throw;
}


int main(int argc,char **argv){
  using namespace std;
  signal(SIGINT, signalHandler2);
  if(!(argc==2)){
    cerr<<"Wrong number of arguments, "<<argc<<endl
        <<"Syntax should be ./tracker configFile"<<endl;
    throw;
  }
  TApplication theApp("theApp",NULL,NULL);
  //reading configFile
  //  TCanvas * c=NULL;
  string infilePath;
  string tpcFilePath;
  string outFilePath;
  string alignmentFilePath;
  int zRangeMin = 0, zRangeMax=0, zDivision=1;

  string histFilePath;
  string mode;
  //trackHistContainer* histCont =new trackHistContainer();
  ConfigFile cf( argv[1] );
  
  bool disp=false;
  bool unbiased=false;

  if(!(cf.readInto(infilePath , "inFile") )) failedConf("infile");
  if(!(cf.readInto(tpcFilePath , "tpcFile") )) failedConf("tpcfile");
  if(!(cf.readInto(outFilePath , "outFile") )) failedConf("alignmentFile");
  if(!(cf.readInto(alignmentFilePath , "alignmentFile") )) failedConf("alignmentFile");
  if(!(cf.readInto(histFilePath , "histFile") )) failedConf("histFile");
  if(!(cf.readInto(mode , "mode") )) failedConf("mode");
  if(!(cf.readInto(zRangeMin , "zRangeMin") )) failedConf("zRangeMin");
  if(!(cf.readInto(zRangeMax , "zRangeMax") )) failedConf("zRangeMax"); 
  if(!(cf.readInto(zDivision , "zDivision") )) failedConf("zDivision");
  if(mode=="disp"){
    disp=true;
  }else if(mode=="unbiasedFit"){
    disp=false;
    unbiased=true;

  }

  TH2D* correlationX = new TH2D("corX","u_TPC vs x_track",5000,-6.4,-2.5,5000,-3.5,3.5);
  TH2D* correlationY = new TH2D("corY","u_TPC vs x_track",5000,-3,3,5000,-4.5,4.5);

  TH2D* hitMap = new TH2D("hitmap","hitmap",1000,-10,10,1000,-10,10);
  TH2D* hitMapTrack = new TH2D("hitmapTr","hitmapTr",1000,-10,10,1000,-10,10);

  TH2D* Drift = new TH2D("DriftSpace_vs_clustersize","DriftSpace_vs_clustersize",1000,-10,10,100,0,100);

  std::vector <TH1D*> resU;
  std::vector <TH1D*> resV;

  TH1D* resUAll = new TH1D("Resolution_vs_Z","Resolution_U_vs_Z",zDivision,zRangeMin,zRangeMax);
  TH1D* resVAll = new TH1D("Resolution_V_vs_Z","Resolution_V_vs_Z",zDivision,zRangeMin,zRangeMax);
  
  for (int i = 0; i<zDivision;i++)
    {
      char buf[20];
      sprintf(buf,"ResidualsTPC_U_%d",i);
      TH1D* tmpU=new TH1D(buf,buf,1000,-10,10);
      resU.push_back(tmpU);
      
      sprintf(buf,"ResidualsTPC_V_%d",i);
      TH1D* tmpV=new TH1D(buf,buf,1000,-10,10);
      resV.push_back(tmpV);
      
    }


  TProfile* correlationUvXp= new TProfile("corUvXp","corUvXp",1000,-10,10,-10,10);
  TProfile* correlationVvYp= new TProfile("corVvYp","corVvYp",1000,-10,10,-10,10);

  TProfile* correlationXvUp= new TProfile("corXvUp","corXvup",1000,-10,10,-10,10);
  TProfile* correlationYvVp= new TProfile("corYvVp","corYvVp",1000,-10,10,-10,10);
  TFile* fileTelescope=new TFile(infilePath.c_str());
  if(fileTelescope==NULL){
    cout<<"telescope file not found "<<infilePath<<endl;
  }
  TFile* fileTPC=new TFile(tpcFilePath.c_str());
  if(fileTPC==NULL){
    cout<<"tpc file not found "<<tpcFilePath<<endl;
  }
  TTree *telescopeTree = (TTree*)fileTelescope->Get("at_pr");
  TTree *tpcTree = (TTree*)fileTPC->Get("at");
  TCevent *telescopeEvent = new TCevent();
  TCevent *tpcEvent = new TCevent();
  //  TCtrack *tpcTrack=new TCtrack(); 
  TCevent *outEvent = new TCevent();
   if(telescopeTree==NULL){
    cout<<"telescopeTree not found"<<endl;
  }
  if(tpcTree==NULL){
    cout<<"tpcTree not found"<<endl;
  }
  TBranch *branchTelescopeEvent=telescopeTree->GetBranch("event");
  TBranch *branchTpcEvent=tpcTree->GetBranch("event");
  //  TBranch *branchTpcTrack=tpcTree->GetBranch("track");
  branchTelescopeEvent->SetAddress(&telescopeEvent);
  branchTpcEvent->SetAddress(&tpcEvent);
  // branchTpcTrack->SetAddress(&tpcTrack);
  
  int nTelescopeEvents=telescopeTree->GetEntries();
  int nTpcEvents=tpcTree->GetEntries();

  TFile* outFile = new TFile(outFilePath.c_str(),"RECREATE");
  TTree* eventTreeOut = new TTree("at_tr","testBench analysis tree");
  eventTreeOut->Branch("event","TCevent",&outEvent,32000,99);
  int totClusters=0;

  TCalign* a = TCalign::getInstance(alignmentFilePath);
  a->clear();
  a->read(alignmentFilePath);
  int tpcEventNr=0;
  
  for(int i_bt=0;i_bt<nTelescopeEvents;i_bt++) {
    if(controlC){
      break;
    }
    if(i_bt>20000){
         break;
    }
    //cout<<"bla"<<endl;
    telescopeTree->GetEntry(i_bt);
    cout<<"telescope "<<i_bt<<": "<<telescopeEvent->getEventNumber()<<endl;
    //    bool found=false
    
    
    for(int i_tpc=0;i_tpc<nTpcEvents;++i_tpc){
      if(i_tpc<tpcEventNr){
        i_tpc=tpcEventNr;
      }
      //  <<i_bt<<" "<<i_tpc<<" ";
      tpcTree->GetEntry(i_tpc);
      //      cout<<"TPC "<<tpcEvent->getEventNumber()<<endl;
      if(telescopeEvent->getEventNumber()==tpcEvent->getEventNumber()){
        tpcEventNr=i_tpc;
        cout<<"found corresponding events "<<tpcEvent->getEventNumber()<<endl;
        for(unsigned int iTr=0;iTr<telescopeEvent->nTracks();++iTr) {
          TCtrack* telTrack = telescopeEvent->getTrack(iTr);
          if(telTrack->fit(1,2,3,4,5,6,7,8)){
            //          cout<<x<<", "<<y<<endl;
            for(unsigned int iCl=0;iCl<tpcEvent->nClusters();++iCl){
	       TCcluster cl=tpcEvent->getCluster(iCl);
              // cout<<cl.posUVW().x()<<", "<<cl.posUVW().y()<<endl;
	      
	      if(cl.posUVW().z()<zRangeMax && cl.posUVW().z()>zRangeMin){
		
		double y=telTrack->getAy()*(posTPC - cl.posUVW().z())+telTrack->getBy();
		double x=telTrack->getAx()*(posTPC - cl.posUVW().z())+telTrack->getBx();
		

                correlationX->Fill(x,cl.posUVW().x());
                correlationY->Fill(y,cl.posUVW().y());
                correlationUvXp->Fill(x,cl.posUVW().x());
                correlationVvYp->Fill(y,cl.posUVW().y());

                correlationXvUp->Fill(cl.posUVW().x(),x);
                correlationYvVp->Fill(cl.posUVW().y(),y);
		int zone = zDivision*(cl.posUVW().z()-zRangeMin)/(zRangeMax-zRangeMin); //zone in the drift space
		assert(zone<zDivision);
		resU.at(zone)->Fill(cl.posUVW().x()-x);
		resV.at(zone)->Fill(cl.posUVW().y()-y);

		Drift->Fill(cl.posUVW().z(),cl.nRaw());
		
                hitMap->Fill(cl.posUVW().x(),cl.posUVW().y());
                hitMapTrack->Fill(x,y);
              }
            }
            //          cout<<endl<<endl;
          }
        }
        break;
      }else if(telescopeEvent->getEventNumber()<tpcEvent->getEventNumber()){
        i_tpc=nTpcEvents;
      }

      //      cout<<i_tpc<<endl;
    }
    cout<<endl; 
    /*
    outEvent->clear();
    //    cout<<inEvent->getEventNumber()<<endl;
    outEvent->setEventNumber(inEvent->getEventNumber());
    if(controlC){ 
      cout<<"jumping out of for loop"<<endl;
      break;
    }
    if(i_ev%250==0){
      cout<<i_ev<<" n clusters "<<totClusters<<endl;
    }
    if(!unbiased){
      for(unsigned int iTr=0;iTr<inEvent->nTracks();++iTr) {
        TCtrack* track = inEvent->getTrack(iTr);
        TCtrack* outTrack=new TCtrack;//cout<<track->nClFit()<<endl;
        
        for(unsigned int i=0;i<track->nCl();++i){
          TCcluster cl=track->getCl(i);
          outTrack->addCluster(cl);
        }
        if(outTrack->nClFit()>5&&outTrack->fit(1,2,3,4,5,6,7,8)){
          histCont->fillRes(outTrack);
          outEvent->addTrack(outTrack);
          //	cout<<"test"<<endl;
        }else{
          delete outTrack;
        }
      }
      eventTreeOut->Fill();
    }else{
      for(unsigned int iTr=0;iTr<inEvent->nTracks();++iTr) {
        TCtrack* track = inEvent->getTrack(iTr);
        
        for(int i=1;i<9;++i){
          TCtrack* outTrack=new TCtrack;//cout<<track->nClFit()<<endl;
          for(unsigned int iCl=0;iCl<track->nCl();++iCl){
            TCcluster cl=track->getCl(iCl);
            if(cl.getId()==i){
              // cout<<"i no fit "<<i<<endl;
              cl.setFit(false);
            }
            outTrack->addCluster(cl);
          }
          
          if(outTrack->nClFit()>6&&outTrack->fit(1,2,3,4,5,6,7,8)){
            histCont->fillRes(outTrack,true);
            outEvent->addTrack(outTrack);
            //	cout<<"test"<<endl;
          }else{
            delete outTrack;
          }
        }
      }
      eventTreeOut->Fill();
    }
    */
  }
  // eventTreeOut->Write();
  outFile->Close();
  fileTelescope->Close();
  fileTPC->Close();
  {
    TFile* histFile=new TFile(histFilePath.c_str(),"RECREATE");
    histFile->cd();

    Drift->Write();

    //**FITTING
    TDirectoryFile  * resU_=new TDirectoryFile("res_","res_");
    resU_->cd();

    resVAll->SetOption("E");    
    resUAll->SetOption("E");    
    for (int iR=0;iR<zDivision;iR++)
      { 
	TF1 * g1 =  new TF1("g1","gaus(0)");
	double center = resU.at(iR)->GetBinCenter(resU.at(iR)->GetMaximumBin());
	double sigma = 0.4;
	g1->SetParameter(0,resU.at(iR)->GetMaximum());//height
	g1->SetParameter(1,center);//mean
	g1->SetParameter(2,sigma);//exp sigma
	resU.at(iR)->Fit(g1,"Q","",center-3*sigma,center+3*sigma);
	
	TF1 * g2 =  new TF1("g2","gaus(0)+gaus(3)");
	g2->SetParameter(0,g1->GetParameter(0));//height
	g2->SetParameter(1,g1->GetParameter(1));//mean
	g2->SetParameter(2,g1->GetParameter(2));//exp sigma
	g2->SetParameter(3,0.2*g1->GetParameter(0));//height
	g2->SetParameter(4,g1->GetParameter(1));//mean
	g2->SetParameter(5,5*g1->GetParameter(2));//exp sigma
	resU.at(iR)->Fit(g2,"Q","",center-10*sigma,center+10*sigma);
	
	TF1 * g3 =  new TF1("g3","gaus(0)+gaus(3)+pol0(6)");
	g3->SetParameter(0,g2->GetParameter(0));//height
	g3->SetParameter(1,g2->GetParameter(1));//mean
	g3->SetParameter(2,g2->GetParameter(2));//exp sigma
	g3->SetParameter(3,g2->GetParameter(3));//height
	g3->SetParameter(4,g2->GetParameter(4));//mean
	g3->SetParameter(5,g2->GetParameter(5));//exp sigma
	
	g3->SetLineColor(600);
	g3->SetLineWidth(2);
	resU.at(iR)->Fit(g3,"Q","",center-10*sigma,center+10*sigma);
	resU.at(iR)->Write();
	
	resUAll->SetBinContent(iR+1,abs(g3->GetParameter(2)));
	resUAll->SetBinError(iR+1,0.054146);//pitch/srqt(12)
	

	delete g1;
	delete g2;
	delete g3;

	
	TF1 * g1V =  new TF1("g1V","gaus(0)");
	double centerV = resV.at(iR)->GetBinCenter(resV.at(iR)->GetMaximumBin());
	double sigmaV = 0.4;
	g1V->SetParameter(0,resV.at(iR)->GetMaximum());//height
	g1V->SetParameter(1,centerV);//mean
	g1V->SetParameter(2,sigmaV);//exp sigma
	resV.at(iR)->Fit(g1V,"Q","",centerV-3*sigmaV,centerV+3*sigmaV);

	TF1 * g2V =  new TF1("g2V","gaus(0)+gaus(3)");
	g2V->SetParameter(0,g1V->GetParameter(0));//height
	g2V->SetParameter(1,g1V->GetParameter(1));//mean
	g2V->SetParameter(2,g1V->GetParameter(2));//exp sigma
	g2V->SetParameter(3,0.2*g1V->GetParameter(0));//height
	g2V->SetParameter(4,g1V->GetParameter(1));//mean
	g2V->SetParameter(5,5*g1V->GetParameter(2));//exp sigma
	resV.at(iR)->Fit(g2V,"Q","",centerV-10*sigmaV,centerV+10*sigmaV);
	
	TF1 * g3V =  new TF1("g3V","gaus(0)+gaus(3)+pol0(6)");
	g3V->SetParameter(0,g2V->GetParameter(0));//height
	g3V->SetParameter(1,g2V->GetParameter(1));//mean
	g3V->SetParameter(2,g2V->GetParameter(2));//exp sigma
	g3V->SetParameter(3,g2V->GetParameter(3));//height
	g3V->SetParameter(4,g2V->GetParameter(4));//mean
	g3V->SetParameter(5,g2V->GetParameter(5));//exp sigma

	g3V->SetLineColor(600);//blue style
	g3V->SetLineWidth(2);

	resV.at(iR)->Fit(g3V,"Q","",centerV-10*sigmaV,centerV+10*sigmaV);
	
	resV.at(iR)->Write();

	
	resVAll->SetBinContent(iR+1,abs(g3V->GetParameter(2)));
	resVAll->SetBinError(iR+1,0.031);//pitch/sqrt(12)

	delete g1V;
	delete g2V;
	delete g3V;
      }

    histFile->cd();
    resUAll->Write();
    resVAll->Write();
    correlationX->Write();
    correlationY->Write();
    correlationXvUp->Write();
    correlationYvVp->Write();
    correlationUvXp->Write();
    correlationVvYp->Write();

    hitMap->Write();
    hitMapTrack->Write();
 //    gApplication->SetReturnFromRun(kTRUE);
//     gSystem->Run();
//     gROOT->Reset();
      
    histFile->Close();
  
    delete histFile;
  }
  //histCont->write(histFilePath);
  //  delete histCont;
  
}
