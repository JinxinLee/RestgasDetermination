// -------------------------------------------------------------------------
// -----                  PndRichHitProducer source file               -----
// -----                  Created 11/06/08  by  S.Spataro              -----
// -------------------------------------------------------------------------

#include "PndRichHitProducer.h"

#include "PndRichHit.h"
#include "PndRichPDHit.h"
#include "PndRichPDPoint.h"
#include "PndRichBarPoint.h"

#include "FairRootManager.h"
#include "FairDetector.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TVector3.h"
#include "TRandom.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndRichHitProducer::PndRichHitProducer() :
  FairTask("Rich Hit Producer") { 
  fPosResolution = -1.;
  fGeoVersion = 313;
  fPhDetNoise = kFALSE;
  fNumRand = 0;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndRichHitProducer::~PndRichHitProducer() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndRichHitProducer::Init() {
  
  cout << "-I- PndRichHitProducer::Init: "
       << "INITIALIZATION *********************" << endl;
  
  //FairRun* sim = FairRun::Instance(); //[R.K. 01/2017] unused variable?
  //FairRuntimeDb* rtdb=sim->GetRuntimeDb(); //[R.K. 01/2017] unused variable?
   
  fGeo = new PndRichGeo();
  fGeo->init(fGeoVersion);
  cout << "-I- PndRichHitProducer::Init: "
       << "fGeoVersion = " << fGeoVersion << endl;
    
  cout << "-I- PndRichHitProducer::Init: ";
   if (fPhDetNoise)
       cout << "Photodetector noise is switched on! " << endl;
   else
       cout << "Photodetector noise is switched off! " << endl;
    
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndRichHitProducer::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  
  // Get input array
  fPDPointArray = (TClonesArray*) ioman->GetObject("RichPDPoint");
  if ( ! fPDPointArray ) {
    cout << "-W- PndRichHitProducer::Init: "
	 << "No RichPDPoint array!" << endl;
    return kERROR;
  }

  fBarPointArray = (TClonesArray*) ioman->GetObject("RichBarPoint");
  if ( ! fBarPointArray ) {
    cout << "-W- PndRichHitProducer::Init: "
	 << "No RichBarPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fPDHitArray = new TClonesArray("PndRichPDHit");
  
  ioman->Register("RichPDHit","PndRich",fPDHitArray,kTRUE);
 
  fHitArray = new TClonesArray("PndRichHit");
  
  ioman->Register("RichHit","PndRich",fHitArray,kTRUE);
 
  if (fPosResolution>0.)
     cout << "-I- PndRichHitProducer::Init: "
	  << "Hit Position smearing: " << fPosResolution << " [cm]" << endl;
  
  cout << "-I- PndRichHitProducer: Intialization successfull" << endl;

   // pde_dpc3200_22.dat
   std::string workdir(getenv( "VMCWORKDIR" ));
   std::string effFileName(workdir+"/rich/pde_dpc3200_22.dat");
   std::ifstream from( effFileName.c_str() );
   Double_t wli, pdei;
   from >> wli >> pdei;
   while( !from.eof() ) {
      fWlPhoton.push_back(wli); // nm
      fPDE.push_back(pdei); // %
      from >> wli >> pdei;
   };

  fRichResolution = new PndRichResolution();
         
  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndRichHitProducer::Exec(Option_t* opt) {
  
  // Reset output array
  if ( ! fPDHitArray ) Fatal("Exec", "No HitArray");
  
  fPDHitArray->Clear();

   //pixels of PhDet
   UInt_t iXmax = fGeo->phDetNPixelMaxX();
   UInt_t iYmax = fGeo->phDetNPixelMaxY();
   UInt_t map[iXmax][iYmax];
   for (Int_t ix=0; ix<iXmax; ix++)
      for (Int_t iy=0; iy<iYmax; iy++)
         map[ix][iy] = 0;
  
  // Loop over RichPDpoints
  Int_t nPoints = fPDPointArray->GetEntriesFast();
  PndRichPDPoint *point = 0;
  TVector3 pos, mom;
  TVector3 sig(fPosResolution, fPosResolution, fPosResolution);
   Double_t k = 2*3.1415927*197.3269602e-9;

  for (Int_t iPoint=0; iPoint<nPoints; iPoint++) {
    point  = (PndRichPDPoint*) fPDPointArray->At(iPoint);
    point->Momentum(mom);
    Double_t wl = k/mom.Mag();
    if ( wl>=fWlPhoton.front() && wl<fWlPhoton.back() ) {
       UInt_t ind = (UInt_t)(wl-fWlPhoton.front());
       Double_t eff1 = fPDE[ind];
       Double_t eff2 = fPDE[ind+1];
       Double_t wl1 = fWlPhoton[ind];
       Double_t wl2 = fWlPhoton[ind+1];
       Double_t eff = eff1+(eff2-eff1)*(wl-wl1)/(wl2-wl1);
       if ( gRandom->Uniform(0.,100.)<eff*0.526316/1.7 ) { // 0.526316 - geometrical occupancy, 1.7 - measured difference MC-EXP
          point->Position(pos);
          Double_t t = gRandom->Gaus(point->GetTime(),0.05); //ns
          if (fPhDetNoise) { // add noise
             std::vector<Double_t> tn = PhDetNoise();
             for (Int_t i=0; i<tn.size(); i++)
                if (t-tn.at(i)<720&&t>tn.at(i)) t = tn.at(i);
          }
          TVector3 posl = fGeo->PhDetPositionLocal(pos);
          TVector3 posd = fGeo->PhDetPositionGlobal( fGeo->PositionDiscretization( posl ) );
          AddPDHit(point->GetDetectorID(), posd, sig, iPoint, t );
          UInt_t ix = fGeo->IndexX(posl);
          UInt_t iy = fGeo->IndexY(posl);
          if ( ix<iXmax && iy<iYmax )
             map[ix][iy] = 1;
       }
    }
  } // Loop over MCPoints
   if (fPhDetNoise) {
      for (Int_t ix=0; ix<iXmax; ix++)
         for (Int_t iy=0; iy<iYmax; iy++)
            if (!map[ix][iy]) {
               std::vector<Double_t> tn = PhDetNoise();
               if (tn.size()&&tn.back()>-50) {
                  pos = fGeo->PixelPositionGlobal(ix,iy);
                  AddPDHit(0, pos, sig, 0, tn.back() );
               }
            }
   }
  
  fHitArray->Clear();
  
   // Loop over RichBarPoints
   Int_t nBarPoints = fBarPointArray->GetEntriesFast();
   PndRichBarPoint *hit = 0;

   Int_t detID, sensorId;//, index; //[R.K. 01/2017] unused variable?
   TVector3 dpos(0., 0., 0.);
   // thetaC containe value of beta 
   Double_t thetaC, errThetaC;
   dbpoint pnt;
   for (Int_t iBarPoint=0; iBarPoint<nBarPoints; iBarPoint++) {
      hit = (PndRichBarPoint*) fBarPointArray->At(iBarPoint);
      hit->Position(pos);
      hit->Momentum(mom);
      
      pnt.mass = hit->GetMass();
      pnt.beta = mom.Mag()/sqrt(mom.Mag()*mom.Mag()+pnt.mass*pnt.mass);
      pnt.x = pos.X();
      pnt.y = pos.Y();
      pnt.theta = mom.Theta();
      pnt.phi = mom.Phi();

      if ( gRandom->Uniform() < fRichResolution->Efficiency(pnt) ) { // efficiency of reconstruction
         errThetaC = fRichResolution->Sigma(pnt); // sigma of beta
         thetaC = gRandom->Gaus( hit->GetThetaC(), errThetaC );
         AddHit(detID, sensorId, pos, dpos, thetaC, errThetaC, iBarPoint);
      }
   } // Loop over MCPoints
   
}
// -------------------------------------------------------------------------

std::vector<Double_t> PndRichHitProducer::PhDetNoise()
{
   std::vector<Double_t> tn;
   Double_t td = 720; //ns
   Double_t fn = 0.25e5; //Hz
   Double_t ts = -2000;//start time of noise simulation, in ns
   Double_t tstop = 50;
   Double_t t = 0;
   while(ts<tstop) {
      t += gRandom->Exp(1e9/fn-td);
      fNumRand++;
      if (t>td) {
         ts += t;
         t = 0;
         if (ts<tstop) tn.push_back(ts);
      }
   }
   return tn;
}

// -----   Private method AddHit   --------------------------------------------
PndRichPDHit* PndRichHitProducer::AddPDHit(Int_t detID, TVector3& pos, TVector3& dpos, 
                                                Int_t index, Double_t time ){
  // It fills the PndRichPDHit category
 
  Int_t sensorId;
  Double_t timeThreshold;
  TClonesArray& clref = *fPDHitArray;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndRichPDHit(detID, sensorId, pos, dpos, time, timeThreshold, index);
}
// ----

// -----   Private method AddHit   --------------------------------------------
PndRichHit* PndRichHitProducer::AddHit(Int_t detID, Int_t sensorId,
                                            TVector3& pos, TVector3& dpos,
                                            Double_t thetaC, Double_t errThetaC,
                                            Int_t index){

  TClonesArray& clref = *fHitArray;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndRichHit(detID, sensorId, pos, dpos, thetaC, errThetaC, index);
}
// ----

void PndRichHitProducer::FinishEvent()
{
}

void PndRichHitProducer::FinishTask()
{
}

ClassImp(PndRichHitProducer)
