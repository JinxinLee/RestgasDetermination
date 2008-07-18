{
	
#include "TVector3.h"
	
	TStopwatch timer;
	timer.Start();

	  
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libPassive");
  gSystem->Load("libGen");
  gSystem->Load("libPGen");
  gSystem->Load("libMvd");
  gSystem->Load("libriemann");
  
  double r = 4.5;
  TVector3 offset(2.1,-3.2,0);
  int nHits = ;
  double dPhi = TMath::Pi()/18; // 10 degree
  
  CbmHit* hits[nHits];
  
  for (int i = 0; i < nHits; i++){
	  TVector3 pos (r*TMath::Cos(dPhi*i), r*TMath::Sin(dPhi*i), i+1);
	  TVector3 dpos (1,1,1);
	  pos += offset;
	  PndMvdHit* detHit = new PndMvdHit(0,"",pos,dpos,i,0,0);
	  std::cout << i << ": " << detHit->GetX() << " " << detHit->GetY() << " " << detHit->GetZ() << std::endl;
	  	  
	  hits[i] = detHit;
  }
  
  PndRiemannTrack track;
  std::cout << "Hits: " << std::endl;

  for (int j = 0; j < nHits; j++){
	  std::cout << j << ": " << hits[j]->GetX() << " " << hits[j]->GetY() << " " << hits[j]->GetZ() << std::endl;
	  PndRiemannHit* rHit = new PndRiemannHit(hits[j]);
	  track.addHit(rHit);
	  std::cout << "rHit: " << rHit->x().X() << " " << rHit->x().Y() << " " << rHit->x().Z() << std::endl;
  }
  track.refit();
  
 TVectorT<double> dVec = track.orig();
 double x = dVec[0];
 double y = dVec[1];
 std::cout << "Riemann Fit r: " << track.r()*100 << " offset: " << x*100 << " " << y*100 << std::endl;
  
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
  
}