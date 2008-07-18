// -------------------------------------------------------------------------
// -----                      CbmMCTrack header file                   -----
// -----                  Created 03/08/04  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmMCTrack.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Data class for storing Monte Carlo tracks processed by the CbmStack.
 ** A MCTrack can be a primary track put into the simulation or a
 ** secondary one produced by the transport through decay or interaction.
 **/


#ifndef CBMMCTRACK_H
#define CBMMCTRACK_H 1


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

class CbmMCTrack : public TObject
{

 public:


  /**  Default constructor  **/
  CbmMCTrack();


  /**  Standard constructor  **/
  CbmMCTrack(Int_t pdgCode, Int_t motherID, TVector3 startVvertex, 
	     Double_t startTime, TVector3 momentum, Int_t nPoint=0);


  /**  Copy constructor  **/
  CbmMCTrack(const CbmMCTrack& track);


  /**  Constructor from TParticle  **/
  CbmMCTrack(TParticle* particle);


  /**  Destructor  **/
  virtual ~CbmMCTrack();


  /**  Output to screen  **/
  void Print(Int_t iTrack) const;


  /**  Accessors  **/
  Int_t    GetPdgCode()     const { return fPdgCode; }
  Int_t    GetMotherID()    const { return fMotherID; }
  TVector3 GetStartVertex() const { return TVector3(fStartX, fStartY, 
						    fStartZ); }
  Double_t GetStartTime()   const { return fStartT; }
  TVector3 GetMomentum()    const { return TVector3(fPx, fPy, fPz); }
  /*
  Int_t GetStsPoints()  const { return   (fPoints &  15        )        ; }
  Int_t GetRichPoints() const { return ( (fPoints & (15 <<  4) ) >>  4 ); }
  */
  Int_t GetMdtPoints()  const { return ( (fPoints & (15 <<  6) ) >>  6 ); }
  Int_t GetDchPoints()  const { return ( (fPoints & (63 << 10) ) >> 10 ); }
  Int_t GetEmcPoints()  const { return ( (fPoints & (15 << 16) ) >> 16 ); }
  Int_t GetSttPoints()  const { return ( (fPoints & (15 << 20) ) >> 20 ); }

  /**  Modifiers  **/

  void SetMotherID(Int_t id) { fMotherID = id; }
  /*
  void SetStsPoints(Int_t np);
  void SetRichPoints(Int_t np);
  */
  void SetMdtPoints(Int_t np);
  void SetDchPoints(Int_t np);
  void SetEmcPoints(Int_t np);
  void SetSttPoints(Int_t np);

  /**  Add one detector point to the fPoint variable  **/
 /*
  void AddStsPoint()  { SetStsPoints(  GetStsPoints()  + 1 ); }
  void AddRichPoint() { SetRichPoints( GetRichPoints() + 1 ); }
 */
  void AddMdtPoint()  { SetMdtPoints(  GetMdtPoints()  + 1 ); }
  void AddDchPoint()  { SetDchPoints(  GetDchPoints()  + 1 ); }
  void AddEmcPoint()  { SetEmcPoints(  GetEmcPoints()  + 1 ); }
  void AddSttPoint()  { SetSttPoints(  GetSttPoints()  + 1 ); }

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
   **  xxx:  Bit  0 -  3
   **  xxx: Bit  4 -  7
   **  MDT:  Bit  6 - 9
   **  DCH:  Bit 10 - 15
   **  EMC:  Bit 16 - 19
   **  STT:  Bit 20 - 23
   **  The respective point numbers can be accessed and modified 
   **  with the inline functions. The number of points must of course
   **  not exceed 15 (4 bits). There is room for two more subdetectors 
   **  in this variable.
   **/
  Int_t fPoints;

 
  ClassDef(CbmMCTrack,1);

};

/*
inline void CbmMCTrack::SetStsPoints(Int_t np) {
  if (np >= 0) fPoints = ( fPoints & ( ~ 15      ) ) |   np;
}

inline void CbmMCTrack::SetRichPoints(Int_t np) {
  if (np >= 0) fPoints = ( fPoints & ( ~(15<< 4) ) ) | ( np <<  4 );
}
*/

inline void CbmMCTrack::SetMdtPoints(Int_t np) {
  if (np >= 0) fPoints = ( fPoints & ( ~(15<< 6) ) ) | ( np <<  6 );
}

inline void CbmMCTrack::SetDchPoints(Int_t np) {
  if (np >= 0) fPoints = ( fPoints & ( ~(63<<10) ) ) | ( np << 10 );
}

inline void CbmMCTrack::SetEmcPoints(Int_t np) {
  if (np >= 0) fPoints = ( fPoints & ( ~(15<<16) ) ) | ( np << 16 );
}

inline void CbmMCTrack::SetSttPoints(Int_t np) {
  if (np >= 0) fPoints = ( fPoints & ( ~(15<<20) ) ) | ( np << 20 );
}

#endif
