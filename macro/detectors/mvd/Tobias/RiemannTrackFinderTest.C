{
	
#include "TVector3.h"
#include "TRandom.h"
	
	TStopwatch timer;
	timer.Start();

  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  gSystem->Load("libriemann");
  
  double r1 = -2;
  TVector3 offset1(2,0,0);
  int nHits1 = 4;
  double dPhi1 = TMath::Pi()/36; // 10 degree
  
  double r2 = 2;
  TVector3 offset2(-2,0,0);
  int nHits2 = 4;
  double dPhi2 = -TMath::Pi()/36; // 10 degree
  
  CbmHit* hits[nHits1+nHits2];
  TRandom1 rand;
  
  for (int i = 0; i < nHits1; i++){
	  TVector3 dpos (0.001,0.001,0.001);
	  TVector3 pos (r1*TMath::Cos(dPhi1*i)+rand.Gaus(0,dpos[0]), r1*TMath::Sin(dPhi1*i)+rand.Gaus(0,dpos[1]), i+1 + rand.Gaus(0,dpos[2]));

	  pos += offset1;
	  PndSdsHit* detHit = new PndSdsHit(0,"",pos,dpos,i,0,0);
	  std::cout << i << ": " << detHit->GetX() << " " << detHit->GetY() << " " << detHit->GetZ() << std::endl;
	  	  
	  hits[i] = detHit;
  }
  
  for (int j = 0; j < nHits2; j++){
	  TVector3 dpos (0.001,0.001,0.001);
	  TVector3 pos (r2*TMath::Cos(dPhi2*j)+rand.Gaus(0,dpos[0]), r2*TMath::Sin(dPhi2*j)+rand.Gaus(0,dpos[1]), j+1 + rand.Gaus(0,dpos[2]));

	  pos += offset2;
	  PndSdsHit* detHit = new PndSdsHit(0,"",pos,dpos,j,0,0);
	  std::cout << nHits1 + j << ": " << detHit->GetX() << " " << detHit->GetY() << " " << detHit->GetZ() << std::endl;
	  	  
	  hits[nHits1 + j] = detHit;
  }
  
  PndRiemannTrackFinder findTracks;
  for (int k = 0; k < (nHits1 + nHits2); k++){
	  findTracks.AddHit(hits[k]);
  }
  findTracks.FindTracks(0.01);
  std::cout << "Found Tracks: " << findTracks.NTracks() << std::endl;
/* 
 TVectorT<double> dVec = track.orig();
 double x = dVec[0];
 double y = dVec[1];
 std::cout << "Riemann Fit r: " << track.r() << " offset: " << x << " " << y << std::endl;
 PndRiemannTrack track2;
 track2.init(x,y,track.r(),1,1);
 */
  
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  
  
}
