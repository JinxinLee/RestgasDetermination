#include "PndTpcPSAplot.h"
#include <iostream>
#include "TAxis.h"
#include "TGraph.h"

PndTpcPSAplot::PndTpcPSAplot(std::vector<PndTpcSample*>* samples,
					   std::vector<PndTpcDigi*>* alldigis,
					   std::vector<PndTpcSignal*>* signals,
					   const PndTpcFrontend* const ffrontend = NULL,
					   std::string name="") : fname(name) {


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
  
  fnDigiLines = ndigi;
  fnSigLines = nsig;



  fdigiLines = new TLine*[ndigi];
  if(signals != NULL) {
	fsigLines = new TLine*[nsig]; 
  }
  else {
	fsigLines = NULL;
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

  fsampG = new TGraph(nsamp,xSampG,ySampG);


  for(int iDigiG=0; iDigiG<ndigi; iDigiG++) {
	xDigiG[iDigiG] = (*digis)[iDigiG]->t();
	yDigiG[iDigiG] = (*digis)[iDigiG]->amp() / 10;
	fdigiLines[iDigiG] = new TLine((*digis)[iDigiG]->t(),0.,(*digis)[iDigiG]->t(),fsampG->GetYaxis()->GetXmax());
  }
  
  
  if(signals != NULL) {
	for(int iSig=0;iSig<nsig;iSig++) {
	  if( ((*signals)[iSig])->amp() > maxAmpSig) {
		maxAmpSig =  (*signals)[iSig]->amp();
	  }
	}
  
  
	for(int iSig=0;iSig<nsig;iSig++) {
	  fsigLines[iSig] = new TLine(ffrontend->ClockFine( ((*signals)[iSig])->t())  ,
								  0.,
								  ffrontend->ClockFine( ((*signals)[iSig])->t())  ,
								  ((*signals)[iSig])->amp() * maxAmpSamp/maxAmpSig);
	}
  }
  
  //  fsampG = new TGraph(nsamp,xSampG,ySampG);
  fdigiG = new TGraph(ndigi,xDigiG,yDigiG);
  //	  char buf[20];
  
}



PndTpcPSAplot::~PndTpcPSAplot(){
  delete fsampG;
  delete fdigiG;
  for(int i=0;i<fnDigiLines;i++) {
	delete fdigiLines[i];
  }
  for(int i=0;i<fnSigLines;i++) {
	delete fsigLines[i];
  }
}

void PndTpcPSAplot::Draw() {
  TCanvas *c1 = new TCanvas();

  //	sprintf(buf,"graph%i",i);
  //	graph->SetName(buf);
  fsampG->SetMarkerStyle(20);
  fsampG->SetMarkerSize(.5);
  fsampG->SetMarkerColor(kBlue);
  fsampG->SetFillColor(kBlue);
  fdigiG->SetMarkerStyle(21);
  fdigiG->SetMarkerSize(.75);
  fdigiG->SetMarkerColor(kRed);
  fsampG->SetTitle(fname.c_str());
  fsampG->GetXaxis()->SetTitle("time [100ns]");
  fsampG->GetYaxis()->SetTitle("arb. units");
  /*
  fsampG->GetYaxis()->SetTitleOffset(1.3);
  fsampG->GetXaxis()->SetTitleFont(102);
  fsampG->GetXaxis()->SetLabelFont(102);
  fsampG->GetYaxis()->SetTitleFont(102);
  fsampG->GetYaxis()->SetLabelFont(102);
  */
  fsampG->Draw("AB");
  fdigiG->Draw("P");
  if(fsigLines != NULL) {
	for(int iLines=0;iLines<fnSigLines;iLines++) {
	  fsigLines[iLines]->SetLineColor(kGreen);
	  fsigLines[iLines]->SetLineWidth(3);
	  fsigLines[iLines]->Draw("same");
	}
  }
  for(int iLines=0;iLines<fnDigiLines;iLines++) {
	fdigiLines[iLines]->SetLineColor(kRed);
	fdigiLines[iLines]->Draw("same");
  }
  gApplication->SetReturnFromRun(kTRUE);
  gSystem->Run();
  delete c1;

}
