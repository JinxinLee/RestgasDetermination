{
	
#include "TVector3.h"
#include "TRandom.h"
	
	TStopwatch timer;
	timer.Start();

  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  gSystem->Load("libriemann");
  
  double r = -2;
  TVector3 offset(2,0,0);
  int nHits = 4;
  double dPhi = TMath::Pi()/36; // 10 degree
  
  CbmHit* hits[nHits];
  TRandom1 rand;
  
  for (int i = 0; i < nHits; i++){
	  TVector3 dpos (0.001,0.001,0.001);
	  TVector3 pos (r*TMath::Cos(dPhi*i)+rand.Gaus(0,dpos[0]), r*TMath::Sin(dPhi*i)+rand.Gaus(0,dpos[1]), i+1 + rand.Gaus(0,dpos[2]));

	  pos += offset;
	  PndSdsHit* detHit = new PndSdsHit(0,"",pos,dpos,i,0,0);
	  std::cout << i << ": " << detHit->GetX() << " " << detHit->GetY() << " " << detHit->GetZ() << std::endl;
	  	  
	  hits[i] = detHit;
  }
  
  PndRiemannTrack track;
  std::cout << "Hits: " << std::endl;

  for (int j = 0; j < nHits-1; j++){
	  std::cout << j << ": " << hits[j]->GetX() << " " << hits[j]->GetY() << " " << hits[j]->GetZ() << std::endl;
	  PndRiemannHit* rHit = new PndRiemannHit(hits[j]);
	  track.addHit(rHit);
	  std::cout << "rHit: " << rHit->x().X() << " " << rHit->x().Y() << " " << rHit->x().Z() << std::endl;
  }
  track.refit();
  std::cout << "Distance: " << track.dist(new PndRiemannHit(hits[nHits-1])) << std::endl;
  
 TVectorT<double> dVec = track.orig();
 double x = dVec[0];
 double y = dVec[1];
 std::cout << "Riemann Fit r: " << track.r() << " offset: " << x << " " << y << std::endl;
 PndRiemannTrack track2;
 track2.init(x,y,track.r(),1,1);
 
 TVectorT<double> vecN = track.n();
 std::cout << "Backcalc fn: " << vecN[0] << " " << vecN[1] << " " << vecN[2] << " fc: " << track.c() << std::endl;
  
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
  
}
