// -------------------------------------------------------------------------
// -----                      PndMCTrack header file                   -----
// -----                  Created 03/08/04  by V. Friese               -----
// -------------------------------------------------------------------------


/** PndMCTrack.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Data class for storing Monte Carlo tracks processed by the PndStack.
 ** A MCTrack can be a primary track put into the simulation or a
 ** secondary one produced by the transport through decay or interaction.
 **/


#ifndef CBMMCTRACK_H
#define CBMMCTRACK_H 1

#include "PndDetectorList.h"
#include "TObject.h"
#include "TParticle.h"
#include "TVector3.h"
#include "TLorentzVector.h"

#ifndef ROOT_TParticlePDG
#include "TParticlePDG.h"
#endif
#ifndef ROOT_TDatabasePDG
#include "TDatabasePDG.h"
#endif

class PndMCTrack : public TObject
{

 public:


  /**  Default constructor  **/
  PndMCTrack();


  /**  Standard constructor  **/
  PndMCTrack(Int_t pdgCode, Int_t motherID, TVector3 startVvertex, 
	     Double_t startTime, TVector3 momentum, Int_t  nPoint=0);


  /**  Copy constructor  **/
  PndMCTrack(const PndMCTrack& track);


  /**  Constructor from TParticle  **/
  PndMCTrack(TParticle* particle);


  /**  Destructor  **/
  virtual ~PndMCTrack();


  /**  Output to screen  **/
  void Print(Int_t iTrack) const;


  /**  Accessors  **/
  Int_t    GetPdgCode()     const { return fPdgCode; }
  Int_t    GetMotherID()    const { return fMotherID; }
  TVector3 GetStartVertex() const { return TVector3(fStartX, fStartY,fStartZ);}
  Double_t GetStartTime()   const { return fStartT; }
  TVector3 GetMomentum()    const { return TVector3(fPx, fPy, fPz); }
  Double_t GetPt()       const { return TMath::Sqrt(fPx*fPx+fPy*fPy); }
  /*
  Int_t GetStsPoints()  const { return   (fPoints &  15        )        ; }
  */
   
  Int_t  GetNPoints(DetectorId detId)  const;  
  /**  Modifiers  **/
  
  void SetMotherID(Int_t id) { fMotherID = id; }
  /*
  void SetStsPoints(Int_t np);
  */ 
 
  
  /**  Add one detector point to the fPoint variable  **/
 /*
  void AddStsPoint()  { SetStsPoints(  GetStsPoints()  + 1 ); }
 */ 
 
  
  void SetNPoints(Int_t iDet, Int_t  np);
  
  TLorentzVector Get4Momentum() const;


private:

  /**  PDG particle code  **/
  Int_t  fPdgCode;

  /** Momentum components at production [GeV]  **/
  Double32_t fPx, fPy, fPz;

  /**  Index of mother track. Zero for primary particles.  **/
  Int_t  fMotherID;

  /** Coordinates of start vertex [cm, ns]  **/
  Double32_t fStartX, fStartY, fStartZ, fStartT;

  /**  Bitvector representing the number of MCPoints for this track in 
   **  each subdetector. The detectors are represented by
   **  This goes to fPoints:
   **  DRC:  Bit  0 -  1  (2 bit max. value 3)
   **  MDT:  Bit  2 -  4  (3 bit max. value 7)
   **  MVD:  Bit  5 -  7  (3 bit max. value 7)
   **  DCH:  Bit  8 - 10  (3 bit max. value 7)
   **  EMC:  Bit 11 - 13  (3 bit max. value 7)
   **  STT:  Bit 14 - 17  (4 bit max. value 15)
   **  TOF:  Bit 18 - 21  (3 bit max. value 7)
   **  GEM:  Bit 22 - 24  (3 bit max. value 7)
   **  DSK:  Bit 25 - 26  (2 bit max. value 3)
   **  HYP:  Bit 27 - 28  (2 bit max. value 3)
   **  RPC:  Bit 29 - 30  (3 bit max. value 3)
   **  The respective point numbers can be accessed and modified **/

  Int_t  fPoints;

  ClassDef(PndMCTrack,1);

};



#endif
