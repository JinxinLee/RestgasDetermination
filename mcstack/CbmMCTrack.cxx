// -------------------------------------------------------------------------
// -----                      CbmMCTrack source file                   -----
// -----                  Created 03/08/04  by V. Friese               -----
// -------------------------------------------------------------------------


#include <iostream>

#include "CbmMCTrack.h"

using namespace std;
// -----   Default constructor   -------------------------------------------
CbmMCTrack::CbmMCTrack() {
 fPdgCode = fMotherID = fPoints = 0;
  fStartX = fStartY  = fStartZ = fStartT = 0.;
  fPx = fPy = fPz = 0.;

}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMCTrack::CbmMCTrack(Int_t pdgCode, Int_t motherID, TVector3 startVertex, 
		       Double_t startTime, TVector3 momentum, Int_t nPoint){
  fPdgCode  = pdgCode;
  fMotherID = motherID;
  fStartX   = startVertex.X();
  fStartY   = startVertex.Y();
  fStartZ   = startVertex.Z();
  fStartT   = startTime;
  fPx       = momentum.X();
  fPy       = momentum.Y();
  fPz       = momentum.Z();
  if (nPoint >= 0) fPoints = nPoint;
  else             fPoints = 0;



}
// -------------------------------------------------------------------------



// -----   Copy constructor   ----------------------------------------------
CbmMCTrack::CbmMCTrack(const CbmMCTrack& track) { 
  *this = track;
}
// -------------------------------------------------------------------------



// -----   Constructor from TParticle   ------------------------------------
CbmMCTrack::CbmMCTrack(TParticle* part) {
  fPdgCode  = part->GetPdgCode();
  fMotherID = part->GetMother(0);
  fStartX   = part->Vx();
  fStartY   = part->Vy();
  fStartZ   = part->Vz();
  fStartT   = part->T()*1e09;
  fPx       = part->Px();
  fPy       = part->Py();
  fPz       = part->Pz();
  fPoints   = Int_t(part->GetMother(1));
}
// -------------------------------------------------------------------------


  
// -----   Destructor   ----------------------------------------------------
CbmMCTrack::~CbmMCTrack() { }
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmMCTrack::Print(Int_t trackID) const {
  cout << "Track " << trackID << ", mother : " << fMotherID << ", Type "
       << fPdgCode << ", momentum (" << fPx << ", " << fPy << ", " << fPz
       << ") GeV" << endl;
}
// -------------------------------------------------------------------------

TLorentzVector CbmMCTrack::Get4Momentum() const {

    Double_t mass=0.0;
    Double_t ene=0.0;
    TParticlePDG*
    fParticlePDG = TDatabasePDG::Instance()->GetParticle(fPdgCode);

    if (fParticlePDG)
     mass    = fParticlePDG->Mass();

    if ( mass >= 0 ) {
	ene  = TMath::Sqrt(mass*mass + fPx*fPx +fPy*fPy +fPz*fPz);
    }

   return TLorentzVector(fPx,fPy,fPz,ene);
}

ClassImp(CbmMCTrack)
