// -------------------------------------------------------------------------
// -----                  PndDchCylinderHit header file                -----
// -----                Created 26.10.2007  by A. Wronska              -----
// -------------------------------------------------------------------------

/** \class PndDchHit
 *  \author A.Wronska <aleksandra.wronska@uj.edu.pl>
 *  \date 26.10.2007
 *  \brief Class for 3d hits reconstructed in dch
 **/


#ifndef PNDDCHHIT_H
#define PNDDCHHIT_H

// Pnd includes
#include "FairHit.h"
// ROOT includes
#include "TVector3.h"

class PndDchHit : public FairHit
{

 public:

  /** Default constructor **/
  PndDchHit();

  /** Standard constructor
   *  \param trackID - Index of MC track
   *  \param detID - Detector ID
   *  \param pos - position in cm
   *  \param dpos - position uncertainty in cm
   *  \param index - index of corresponding MCpoint
   * **/
  PndDchHit(Int_t trackID, Int_t detID,
  	    TVector3 pos, TVector3 dpos, Int_t index);

  /** Destructor **/
  virtual ~PndDchHit();

  /** Modifiers **/
  void SetTrackID(Int_t id) { fTrackID = id; };
  
  /** Accessors **/
  Int_t     GetTrackID() const  { return fTrackID; }
  TVector3  GetPosition()	const { return TVector3(fX, fY, fZ); }
  

  /** Screen output **/
  virtual void Print(const Option_t* opt = 0) const;


 private:

  Int_t fTrackID;  ///< track ID from MC

  ClassDef(PndDchHit,1);

};

#endif
