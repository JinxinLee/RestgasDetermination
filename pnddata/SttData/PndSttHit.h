/** CbmStsMapsHit
 ** Class for MAPS detector hit
 **@author Michael Deveaux <m.deveaux@gsi.de>
 ** Acknowledgements to M. Al-Turany, D. Bertini, G. Gaycken
 ** Version beta 0.1 (02.02.2005)
 ** Slight modifications by V. Friese to match coding conventions
 **
 ** Meaning of RefIndex:  Index of corresponding MCPoint
 **                       -1 if fake or background hit
 **
 ** Meaning of Flag:       0 = Hit ok
 **                       -1 : Hit lost due to detection inefficiency
 **/

#ifndef PNDSTTHIT_H
#define PNDSTTHIT_H 1


#include "TVector3.h"
#include "FairHit.h"

class PndSttHit : public FairHit 
{

 public:    

  /** Default constructor **/
  PndSttHit();


  /** Standard constructor 
  *@param detID     Detector unique volume ID
  *@param pos       Position coordinates [cm]
  *@param dpos      Errors in position coordinates [cm]
  *@param index     Index of corresponding MCPoint
  *@param isochrone The radial measurement
  *@param isoerror  The erroon on the radial measurement
  **/
  PndSttHit(Int_t detID, TVector3& pos, TVector3& dpos,
	    Int_t index, Int_t flag, Double_t isochrone,
	    Double_t isoerror, TVector3 wireDir);
  PndSttHit(Int_t detID, TVector3& pos, TVector3& dpos, Int_t index); 

  // stt1----
  PndSttHit(Int_t detID, TVector3& pos, TVector3& dpos, Int_t index, Int_t trackID, Double_t p, Double_t rr, Double_t rt, Double_t isochroneError,TVector3 wireDir);
  //---------
  
  /** Destructor **/
  virtual ~PndSttHit();    


  /** Output to screen (not yet implemented) **/
  virtual void Print(const Option_t* opt = 0) const {;}


  /** Public method Clear
   ** Resets the isochrone and it's error to 0
   **/
  void Clear();


  /** Accessors **/
  Double_t GetIsochrone()        const { return fIsochrone;                 }; 
  Double_t GetIsochroneError()   const { return fIsochroneError;            }; 
  Double_t GetRadial()           const { return fRadial;                    };
  TVector3 GetWireDirection()    const { return fWireDirection;             };

  Double_t GetPulse()            const {return fPulse;                      };
  Double_t GetXint()             const { return fXint;                      };
  Double_t GetYint()             const { return fYint;                      };
  Double_t GetZint()             const { return fZint;                      };
  Double_t GetdEdx()             const { return fdEdx;                      };
  Double_t GetDepCharge()        const { return fDepCharge;                 };
  Double_t GetEnergyLoss()       const { return fELoss;                     };
  Double_t GetTubeHalfLength()   const { return fHalfLength ;               };

  /** Modifiers **/
  void SetIsochrone(Double_t isochrone)           { fIsochrone = isochrone; };
  void SetIsochroneError(Double_t isochroneError) { fIsochroneError = isochroneError; };
  void SetRadial(Double_t newRadial)              { fRadial = newRadial; };
  void SetWireDirection(TVector3 newWire)         { fWireDirection = newWire; }

  void SetAssigned()  {fAssigned = kTRUE;} 
  Bool_t IsAssigned() const {return fAssigned;}
 
  void SetXint(Double_t x) { fXint = x; }
  void SetYint(Double_t y) { fYint = y; }
  void SetZint(Double_t z) { fZint = z; }

  void SetTrackID(Int_t trackid)     { fTrackID = trackid;}

  void SetDepCharge(Double_t depcharge)       { fDepCharge = depcharge; }
  void SetEnergyLoss(Double_t eloss)          { fELoss = eloss; }
  void SetdEdx(Double_t dedx)                 { fdEdx = dedx; }
  void SetTubeHalfLength(Double_t halflength) { fHalfLength = halflength; }

 // tube ID // CHECK added
  void SetTubeID(Int_t tubeid) { fTubeID = tubeid; }
  Int_t GetTubeID() { return fTubeID; }

 protected:
 
  /** This variable contains the radial distance to the wire **/    
  Double_t fIsochrone;
  /** This variable contains the error on the radial distance to the wire **/    
  Double_t fIsochroneError;
  /** This variable contains the position calculated along the circle in the x-y plane **/
  Double_t fRadial;
  /** This variables contain the direction of the wire **/
  TVector3 fWireDirection;

  Bool_t fAssigned;

  // stt1
  Double_t fPulse; 
  Double_t fRsim; 
  Double_t fRtrue; 
  //  fFlag;  
  //  fnam;   
  Int_t fTrackID; 
  //  fEventID; 
  /*  tube_c;    */
  /*   tube_max;  */
  /*   tube_min;  */
  Double_t fXint, fYint, fZint;      // Position of intersections (will work in reco)

  Double_t fDepCharge;  // deposit charge (arbitrary units)
  Double_t fdEdx;       // hit dEdx
  Double_t fHalfLength; // tube half length
  Double_t fELoss;      // hit energy loss 
  Int_t fTubeID; // CHECK added


 ClassDef(PndSttHit,1);
};


#endif
