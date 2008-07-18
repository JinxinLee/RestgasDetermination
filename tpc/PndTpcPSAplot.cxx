#include "PndTpcPSAplot.h"
#include <iostream>
#include "TAxis.h"
#include "TGraph.h"

PndTpcPSAplot::PndTpcPSAplot(std::vector<PndTpcSample*>* samples,
					   std::vector<PndTpcDigi*>* alldigis,
					   std::vector<PndTpcSignal*>* signals,
					   const PndTpcFrontend* const _frontend = NULL,
					   std::string name="") : _name(name) {


  std::vector<PndTpcDigi*>* digis = new std::vector<PndTpcDigi*>;
  int currentPadId = samples->at(0)->padId();
  for (unsigned int i=0;i<alldigis->size();++i) {
	if(alldigis->at(i)->padId() == currentPadId) {
	  digis->push_back(alldigis->at(i));
	}
  }


  int nsamp = samples->size();
  int ndigi = digis->size();
  int nsig;
  if(signals != NULL) {
	nsig  = signals->size();
  }
  else {
	nsig = 0;
  }
  int xSampG[nsamp];
  int ySampG[nsamp];
  double xDigiG[ndigi];
  double yDigiG[ndigi];
  
  _nDigiLines = ndigi;
  _nSigLines = nsig;



  _digiLines = new TLine*[ndigi];
  if(signals != NULL) {
	_sigLines = new TLine*[nsig]; 
  }
  else {
	_sigLines = NULL;
  }
  
  int maxAmpSamp=0;
  double maxAmpSig=0.;
  for(int iSampG=0; iSampG<nsamp; iSampG++) {
	xSampG[iSampG] = (*samples)[iSampG]->t();
	ySampG[iSampG] = (*samples)[iSampG]->amp();
	if((*samples)[iSampG]->amp() > maxAmpSamp) {
	  maxAmpSamp = (*samples)[iSampG]->amp();
	}
  }

  _sampG = new TGraph(nsamp,xSampG,ySampG);


  for(int iDigiG=0; iDigiG<ndigi; iDigiG++) {
	xDigiG[iDigiG] = (*digis)[iDigiG]->t();
	yDigiG[iDigiG] = (*digis)[iDigiG]->amp() / 10;
	_digiLines[iDigiG] = new TLine((*digis)[iDigiG]->t(),0.,(*digis)[iDigiG]->t(),_sampG->GetYaxis()->GetXmax());
  }
  
  
  if(signals != NULL) {
	for(int iSig=0;iSig<nsig;iSig++) {
	  if( ((*signals)[iSig])->amp() > maxAmpSig) {
		maxAmpSig =  (*signals)[iSig]->amp();
	  }
	}
  
  
	for(int iSig=0;iSig<nsig;iSig++) {
	  _sigLines[iSig] = new TLine(_frontend->ClockFine( ((*signals)[iSig])->t())  ,
								  0.,
								  _frontend->ClockFine( ((*signals)[iSig])->t())  ,
								  ((*signals)[iSig])->amp() * maxAmpSamp/maxAmpSig);
	}
  }
  
  //  _sampG = new TGraph(nsamp,xSampG,ySampG);
  _digiG = new TGraph(ndigi,xDigiG,yDigiG);
  //	  char buf[20];
  
}



PndTpcPSAplot::~PndTpcPSAplot(){
  delete _sampG;
  delete _digiG;
  for(int i=0;i<_nDigiLines;i++) {
	delete _digiLines[i];
  }
  for(int i=0;i<_nSigLines;i++) {
	delete _sigLines[i];
  }
}

void PndTpcPSAplot::Draw() {
  TCanvas *c1 = new TCanvas();

  //	sprintf(buf,"graph%i",i);
  //	graph->SetName(buf);
  _sampG->SetMarkerStyle(20);
  _sampG->SetMarkerSize(.5);
  _sampG->SetMarkerColor(kBlue);
  _sampG->SetFillColor(kBlue);
  _digiG->SetMarkerStyle(21);
  _digiG->SetMarkerSize(.75);
  _digiG->SetMarkerColor(kRed);
  _sampG->SetTitle(_name.c_str());
  _sampG->GetXaxis()->SetTitle("time [100ns]");
  _sampG->GetYaxis()->SetTitle("arb. units");
  /*
  _sampG->GetYaxis()->SetTitleOffset(1.3);
  _sampG->GetXaxis()->SetTitleFont(102);
  _sampG->GetXaxis()->SetLabelFont(102);
  _sampG->GetYaxis()->SetTitleFont(102);
  _sampG->GetYaxis()->SetLabelFont(102);
  */
  _sampG->Draw("AB");
  _digiG->Draw("P");
  if(_sigLines != NULL) {
	for(int iLines=0;iLines<_nSigLines;iLines++) {
	  _sigLines[iLines]->SetLineColor(kGreen);
	  _sigLines[iLines]->SetLineWidth(3);
	  _sigLines[iLines]->Draw("same");
	}
  }
  for(int iLines=0;iLines<_nDigiLines;iLines++) {
	_digiLines[iLines]->SetLineColor(kRed);
	_digiLines[iLines]->Draw("same");
  }
  gApplication->SetReturnFromRun(kTRUE);
  gSystem->Run();
  delete c1;

}
