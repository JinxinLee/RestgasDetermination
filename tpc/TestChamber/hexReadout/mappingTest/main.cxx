#include<iostream>
#include<cassert>
#include"tpcEvent.h"
#include"TApplication.h"
#include"TGraph.h"
#include"TCanvas.h"
#include"TSystem.h"

#include "SignalAnalysis.h"

int main(int argc,char **argv){
  assert(argc==2);

  TApplication theApp("theApp",NULL,NULL);

  TFile::Open(argv[1]);
  TTree *tree = (TTree*)gROOT->FindObject("datatree");

  tpcEvent e(tree);
  while(e.loadNext()){
    /*
    
    double maxAmp=-1.E10;
    int maxSample=-1;
    for(int i=0;i<e.tNumberOfSamples;++i){
      double amp=1024.-e.tAmp[i];
      if(amp>maxAmp && !(e.tChannelId[i]>30&&e.tChannelId[i]<40)){
	maxAmp=amp;
	maxSample=i;
      }
    }
    int chId = e.tChannelId[maxSample];
    UInt_t amps[255];
    int counter=0;
    for(int i=0;i<e.tNumberOfSamples;++i){
      assert(counter<256);
      if(e.tChannelId[i]==chId) amps[counter++]=1024.-e.tAmp[i];
    }


    std::vector<pulse_t*> pulses = psa(amps,254);
    for(int i=0;i<pulses.size();++i){
      std::cout << "PSA:" << std::endl;
      std::cout << pulses.at(i)->time << " " << pulses.at(i)->time << std::endl;
      std::cout << pulses.at(i)->samples.size() << std::endl;
    }

    assert(counter==255);
    double time[255];
    for(int i=0;i<255;++i){
      time[i]=i;
    }
    static int rootCounter = 0;
    char buf2[50];
    sprintf(buf2,"graph%d",rootCounter++);
    TCanvas *c = new TCanvas();
    c->SetName(buf2);

    double Damps[256];
    for(int i=0;i<256;++i)Damps[i]=amps[i];
    TGraph *g = new TGraph(255,time,Damps);
    char buf[50];
    sprintf(buf,"channel %d",chId);
    sprintf(buf2,"graph%d",rootCounter++);
    g->SetName(buf2);
    g->SetTitle(buf);
    g->Draw("A*");
    gApplication->SetReturnFromRun(kTRUE);
    gSystem->Run();
    gROOT->Reset();
    delete g;
    delete c;
    std::cout << chId << " " << maxAmp << std::endl;
    */

    
    UInt_t amps[76][255];
    std::vector<pulse_t*> pulses[76];
    int counter[76];
    for(int i=0;i<76;++i) counter[i]=0;
    int nPulses=0;
    int nChannels=0;
    for(int i=0;i<e.tNumberOfSamples;++i){
      int channelId = e.tChannelId[i];
      amps[channelId][counter[channelId]++] = 1024.-e.tAmp[i];
    }

    for(int i=0;i<76;++i) {
      pulses[i] = psa(amps[i],254);
      for(int j=0;j<pulses[i].size();++j){
	pulses[i].at(j)->ch = i;
      }
      nPulses+=pulses[i].size();
      if(pulses[i].size()>0)nChannels+=1;
    }
    double x[nPulses];
    double y[nPulses];
    int pulsCounter = 0;

    for(int j=0;j<76;++j){
      for(int k=0;k<pulses[j].size();++k){
	x[pulsCounter] = pulses[j].at(k)->startTime;
	y[pulsCounter++] = j;
      }
    }

    if(nPulses>0){
      static int rootCounter = 0;
      char buf2[50];
      sprintf(buf2,"graph%d",rootCounter++);
      TCanvas *c = new TCanvas();
      c->SetName(buf2);
      TGraph *g = new TGraph(nPulses,x,y);
      char buf[50];
      static int eventCounter(0);
      sprintf(buf,"event %d",eventCounter++);
      sprintf(buf2,"graph%d",rootCounter++);
      g->SetName(buf2);
      g->SetTitle(buf);
      g->Draw("A*");
      gApplication->SetReturnFromRun(kTRUE);
      gSystem->Run();
      gROOT->Reset();
      delete g;
      delete c;
    }
    std::cout << nPulses << " " << nChannels <<  std::endl;
    

  }


}
