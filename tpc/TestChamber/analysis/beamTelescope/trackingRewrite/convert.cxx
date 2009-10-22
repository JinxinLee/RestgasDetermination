//STL and C headers
#include <list>
#include <iostream>
#include <sstream> 
#include <string>
#include <signal.h>
//boost headers
#include <boost/tokenizer.hpp>
//ROOT headers
#include <TChain.h>
#include <TFile.h>
#include <TApplication.h>
#include <TVector3.h>
#include <TRandom3.h>
#include <TH2D.h>
//normal includes
#include "../Hits.h"
#include "ConfigFile.h"
#include "TCevent.h"
using namespace std;

static bool controlC=false;
static int controlCs=0;

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

  string infilePath;
  string outFilePath;
  string alignmentFilePath;
  string histFilePath;
  bool gemMonErrGM=true;
  bool gemMonErrSI=false;

  ConfigFile cf( argv[1] );
  
  if(!(cf.readInto(infilePath , "inFile") )) failedConf("infile");
  if(!(cf.readInto(outFilePath , "outFile") )) failedConf("alignmentFile");
  if(!(cf.readInto(alignmentFilePath , "alignmentFile") )) failedConf("alignmentFile");
  if(!(cf.readInto(histFilePath , "histFile") )) failedConf("histFile");
  if(!(cf.readInto(gemMonErrGM , "gemMonErrGM") )) failedConf("gemMonErrGM");
  if(!(cf.readInto(gemMonErrSI , "gemMonErrSI") )) failedConf("gemMonErrSI");
  
  TFile::Open(infilePath.c_str());
  TTree *inTree = (TTree*)gROOT->FindObject("Hits");

  vector<CsGEMPlane*> detectorPlanes;
  vector<double> noiseThreshold; //in units of sigma
  vector<double> a1a2Threshold;
  vector<double> a0a2Threshold;
  vector<double> cutMaxU;
  vector<double> cutMinU;
  vector<double> multipleScatterWidth;
  vector<TH1D*> ampSpectra;
  vector<TH1D*> ampSpectra2;
  vector<TH1D*> ampSpectraSi1;
  vector<TH1D*> ampSpectraSi2;
  vector<TH1D*> clErr1;
  vector<TH1D*> clErr2;
  vector<TH1D*> timeErr1;
  vector<TH1D*> timeErr2;
  vector<TH1D*> time1;
  vector<TH1D*> time2;
  vector<TH2D*> clErrVclSz1;
  vector<TH2D*> clErrVclSz2;
  vector<TH1I*> clusterSize1;
  vector<TH1I*> clusterSize2;
  vector<TH1I*> clusterMultip1;
  vector<TH1I*> clusterMultip2;
  vector<TH1I*> hitStrip1;
  vector<TH1I*> hitStrip2;
  
  
  for(int i=0;i<8;++i){
    char buf[10];
    sprintf(buf,"%i",(i+1));
    string name1="ampSpectra";
    name1+=buf;
    detectorPlanes.push_back(new CsGEMPlane());
    ampSpectra.push_back(new TH1D(name1.c_str(),name1.c_str(),1200,0,2400));
    name1+="_2";
    ampSpectra2.push_back(new TH1D(name1.c_str(),name1.c_str(),1200,0,2400));
    name1="clusterSize";
    name1+=buf;
    clusterSize1.push_back(new TH1I(name1.c_str(),name1.c_str(),20,0,20));
    name1+="_2";
    clusterSize2.push_back(new TH1I(name1.c_str(),name1.c_str(),20,0,20));
    name1="clusterMult";
    name1+=buf;
    clusterMultip1.push_back(new TH1I(name1.c_str(),name1.c_str(),40,0,40));
    name1+="_2";
    clusterMultip2.push_back(new TH1I(name1.c_str(),name1.c_str(),40,0,40));
    name1="hitStrip";
    name1+=buf;
    hitStrip1.push_back(new TH1I(name1.c_str(),name1.c_str(),384,0,384));
    name1+="_2";
    hitStrip2.push_back(new TH1I(name1.c_str(),name1.c_str(),384,0,384));
    name1="amp/sigma";
    name1+=buf;
    ampSpectraSi1.push_back(new TH1D(name1.c_str(),name1.c_str(),1200,0,1200));
    name1+="_2";
    ampSpectraSi2.push_back(new TH1D(name1.c_str(),name1.c_str(),1200,0,1200));
    name1="clErr";
    name1+=buf;
    clErr1.push_back(new TH1D(name1.c_str(),name1.c_str(),4000,0,0.5));
    name1+="_2";
    clErr2.push_back(new TH1D(name1.c_str(),name1.c_str(),4000,0,0.5));
    name1="clErrVclSz";
    name1+=buf;
    clErrVclSz1.push_back(new TH2D(name1.c_str(),name1.c_str(),1000,0,0.5,20,0,20));
    name1+="_2";
    clErrVclSz2.push_back(new TH2D(name1.c_str(),name1.c_str(),1000,0,0.5,20,0,20));
    name1="timeErr";
    name1+=buf;
    timeErr1.push_back(new TH1D(name1.c_str(),name1.c_str(),4000,0,100));
    name1+="_2";
    timeErr2.push_back(new TH1D(name1.c_str(),name1.c_str(),4000,0,100));
    name1="time";
    name1+=buf;
    time1.push_back(new TH1D(name1.c_str(),name1.c_str(),2000,0,100));
    name1+="_2";
    time2.push_back(new TH1D(name1.c_str(),name1.c_str(),2000,0,100));
    /*
      Reading cuts from configfile
     */
    {
      string s = "";
      ostringstream oss;
      oss<<(i+1);
      string name="cut"+oss.str();
      if(!(cf.readInto(s, name.c_str()) )) failedConf(name.c_str());
      istringstream iss(s);
      double noise,a0a2,a1a2,minU,maxU;
      iss>>noise>>a1a2>>a0a2>>minU>>maxU;
      cout<<i<<" noise "<<noise
          <<" a0a2 "<<a0a2
          <<" a1a2 "<<a1a2
          <<" minU "<<minU
          <<" maxU "<<maxU<<endl;
      noiseThreshold.push_back(noise);
      a1a2Threshold.push_back(a1a2);
      a0a2Threshold.push_back(a0a2);
      cutMinU.push_back(minU);
      cutMaxU.push_back(maxU);
    }
    {
      string s = "";
      ostringstream oss;
      oss<<(i+1);
      string name="mScat"+oss.str();
      double cut=0;
      if((cf.readInto(cut, name.c_str()) )){
        multipleScatterWidth.push_back(cut);
        cout<<name<<" "<<cut<<endl;
      }else{
        multipleScatterWidth.push_back(0);
        cout<<name<<" "<<cut<<endl;
      }
    } 
  }
  
  TBranch *branchGM01X1 =inTree->GetBranch("GM01X1__.");
  TBranch *branchGM01Y1 =inTree->GetBranch("GM01Y1__.");
  TBranch *branchGM02X1 =inTree->GetBranch("GM02X1__.");
  TBranch *branchGM02Y1 =inTree->GetBranch("GM02Y1__.");
  TBranch *branchSI01X1 =inTree->GetBranch("SI01X1__.");
  TBranch *branchSI01Y1 =inTree->GetBranch("SI01Y1__.");
  TBranch *branchSI02X1 =inTree->GetBranch("SI02X1__.");
  TBranch *branchSI02Y1 =inTree->GetBranch("SI02Y1__.");  
  
  branchGM01X1->SetAddress(&(detectorPlanes.at(0)));
  branchGM01Y1->SetAddress(&(detectorPlanes.at(1)));
  branchSI01X1->SetAddress(&(detectorPlanes.at(2)));
  branchSI01Y1->SetAddress(&(detectorPlanes.at(3)));
  branchSI02X1->SetAddress(&(detectorPlanes.at(4)));
  branchSI02Y1->SetAddress(&(detectorPlanes.at(5)));
  branchGM02X1->SetAddress(&(detectorPlanes.at(6)));
  branchGM02Y1->SetAddress(&(detectorPlanes.at(7)));
  

  int nEvents=inTree->GetEntries();

  TCevent* outEvent = new TCevent();
  /*
    outTree will contain events with clusters, no track candidates.
  */
  TFile* outFile = new TFile(outFilePath.c_str(),"RECREATE");
  TTree* outTree = new TTree("at_cl","testbench elsa analysis tree");
  outTree->Branch("event","TCevent",&outEvent,32000,99);

  TCalign* a = TCalign::getInstance(alignmentFilePath);
  a->clear();
  a->read(alignmentFilePath);
  
  vector<int> nClusters;
  vector<int> nCuts;
  int totClusters=0;
  int totCut=0;
  int posCut=0;
  for(int i=0;i<8;++i){
    nClusters.push_back(0);
    nCuts.push_back(0);
  }
  TRandom3 rand(0);
  for(int i_ev=0;i_ev<nEvents;i_ev++) {

    if(controlC){ 
      cout<<"jumping out of for loop"<<endl;
      break;
    }
    if(i_ev%500==0){
      cout<<i_ev<<" n clusters "<<totClusters<<" totCut "<<totCut<<" pos cut "<<posCut<<endl;
    }
    
    inTree->GetEntry(i_ev);
    outEvent->Clear();
    outEvent->setEventNumber(detectorPlanes.at(0)->GetHeader()->GetEvtNumInRun());
    //    cout<<outEvent->getEventNumber()<<endl;
    vector<TCcluster> eventClusters;
    for(unsigned int i=0;i<detectorPlanes.size();++i){
      double pitch=a->getPitch(i+1);
      double res=a->getRes(i+1);
      //cout<<detectorPlanes.at(i)->GetName()<<endl;
      //if(i==0||i==1||i==6||i==7){
      //GEM clusters, clustering in GEMmonitor is okey
      int nclus1=0;
      int nclus2=0;
      for(list<CsGEMCluster*>::const_iterator it = (detectorPlanes.at(i)->GetClusters()).begin();it!=(detectorPlanes.at(i)->GetClusters()).end();++it){
        //cout<<"test"<<endl;
        vector<float> amps =(*it)->GetAmp();
        double amp2=amps.at(2);
        double amp1=amps.at(1);
        double amp0=amps.at(0);
        nclus1++;
        

        clusterSize1.at(i)->Fill((*it)->GetSize());
        hitStrip1.at(i)->Fill((*it)->GetPosition());
        double ratioA1A2=(amp1+rand.Uniform(-0.5,0.5))/(amp2);
        double ratioA0A2=(amp0+rand.Uniform(-0.5,0.5))/(amp2);
        double noise=(*it)->GetNoise();
        double time=0;
        double timeErr=0;
        if((*it)->GetSize()>1){
          ampSpectraSi1.at(i)->Fill(amp2/noise);  
          ampSpectra.at(i)->Fill(amp2);
        }
        if((*it)->GetTime(time,timeErr)){
          //cout<<"time "<<time<<" err "<<timeErr<<endl;
        }
        clErr1.at(i)->Fill((*it)->GetPositionErr());
        clErrVclSz1.at(i)->Fill((*it)->GetPositionErr(),(*it)->GetSize());
        timeErr1.at(i)->Fill(timeErr);
        time1.at(i)->Fill(time);
        if((ratioA0A2<a0a2Threshold.at(i))&&(ratioA1A2<a1a2Threshold.at(i))&&(amp2>noiseThreshold.at(i)*noise)){
          double x=((*it)->GetPosition())*pitch;
          //          cout<<i<<" u "<<x<<" ";
          if(x>=cutMinU.at(i)&&x<=cutMaxU.at(i)){
            //cout<<" not cut "<<endl;
            time2.at(i)->Fill(time);
            timeErr2.at(i)->Fill(timeErr);
            clErrVclSz2.at(i)->Fill((*it)->GetPositionErr(),(*it)->GetSize());
            clErr2.at(i)->Fill((*it)->GetPositionErr());
            nclus2++;
            if((*it)->GetSize()>1){
              ampSpectraSi2.at(i)->Fill(amp2/noise);
              ampSpectra2.at(i)->Fill(amp2);
            }
            hitStrip2.at(i)->Fill((*it)->GetPosition());
            TVector3 pos(x,0,0);
            TVector3 err(0,0,0);
            if(i>1&&i<7){
              if(!gemMonErrSI){
                double resEst=TMath::Sqrt(res*res+multipleScatterWidth.at(i)*multipleScatterWidth.at(i));
                err=TVector3(resEst,0.5,0.1);
              }else{
                double resEst=TMath::Sqrt((((*it)->GetPositionErr())*pitch)*(((*it)->GetPositionErr())*pitch)
                                          +multipleScatterWidth.at(i)*multipleScatterWidth.at(i));
                err=TVector3(resEst,0.5,0.1);
              }
            }else{
              if(!gemMonErrGM){
                double resEst=TMath::Sqrt(res*res+multipleScatterWidth.at(i)*multipleScatterWidth.at(i));
                err=TVector3(resEst,0.5,0.1);
              }else{
                double resEst=TMath::Sqrt((((*it)->GetPositionErr())*pitch)*(((*it)->GetPositionErr())*pitch)
                                          +multipleScatterWidth.at(i)*multipleScatterWidth.at(i));
                err=TVector3(resEst,0.5,0.1);
              }
            }
            TCcluster _c(pos,err,amp2,(i+1));
            eventClusters.push_back(_c);
            nClusters.at(i)++;
            totClusters++;
           
            clusterSize2.at(i)->Fill((*it)->GetSize());
            //	    cout<<i+1<<" x "<<_c.posXYZ().x()<<" y "<<_c.posXYZ().y()<<endl;
          }else{
            posCut++;
            totCut++;
          }
	  
        }else{
          nCuts.at(i)++;
          totCut++;
        }
      }//end looping over clusters
      // }//end looping over gem clusters
      clusterMultip1.at(i)->Fill(nclus1);
      clusterMultip2.at(i)->Fill(nclus2);
    }//end looping over detector planes
    /*
      string tmp;
      cout<<"bla enter"<<endl;
      cin>>tmp;
    */
    outEvent->addClusters(eventClusters);
    outTree->Fill();
  }//eventloop end
  outEvent->Clear();
  outFile->Write();
  outFile->Close();
  for(unsigned int i=0;i<detectorPlanes.size();++i){
    delete detectorPlanes.at(i);
  }
  detectorPlanes.clear();
  a->clear();
  TFile* histFile=new TFile(histFilePath.c_str(),"RECREATE");
  for(unsigned int i=0;i<8;++i){
    ampSpectra.at(i)->Write();
    ampSpectra2.at(i)->Write();
    clusterSize1.at(i)->Write();
    clusterSize2.at(i)->Write();
    clusterMultip1.at(i)->Write();
    clusterMultip2.at(i)->Write();
    hitStrip1.at(i)->Write();
    hitStrip2.at(i)->Write();
    ampSpectraSi1.at(i)->Write();
    ampSpectraSi2.at(i)->Write();
    clErr1.at(i)->Write();
    clErr2.at(i)->Write();
    timeErr1.at(i)->Write();
    timeErr2.at(i)->Write();
    time1.at(i)->Write();
    time2.at(i)->Write();
    clErrVclSz1.at(i)->Write();
    clErrVclSz2.at(i)->Write();

  }
  histFile->Close();
  delete histFile;
}

