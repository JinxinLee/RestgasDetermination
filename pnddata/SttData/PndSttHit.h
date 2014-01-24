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
#include "PndTrack.h"

class PndSttHit : public FairHit 
{

 public:    

  /** Default constructor **/
  PndSttHit();


  /** Standard constructor 
  *@param detID     Detector unique volume ID
  *@param tubeID    Unique tube ID
  *@param mcindex   Index of corresponding MCPoint
  *@param pos       Position coordinates of the tube [cm]
  *@param dpos      Errors in position coordinates [cm]
  *@param isochrone The radial measurement
  *@param isoerror  The erroon on the radial measurement
  *@param chDep     Deposited charge (arbitrary unit)
  **/

  // THIS ONE!
  PndSttHit(Int_t detID, Int_t tubeID, Int_t mcindex, TVector3& pos, TVector3& dpos, Double_t p, Double_t isochrone, Double_t isochroneError, Double_t chDep);


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
  Double_t GetPulse()            const {return fPulse;                      };
  Double_t GetDepCharge()        const { return fDepCharge;                 };
  Double_t GetEnergyLoss()       const { return fDepCharge/1e6;             };

  /** Modifiers **/
  void SetIsochrone(Double_t isochrone)           { fIsochrone = isochrone; };
  void SetIsochroneError(Double_t isochroneError) { fIsochroneError = isochroneError; };
  void SetDepCharge(Double_t depcharge)       { fDepCharge = depcharge; }

  // tube ID // CHECK added
  void SetTubeID(Int_t tubeid) { fTubeID = tubeid; }
  Int_t GetTubeID() const { return fTubeID; }

  // computation of dE/dx
  Double_t ComputedEdx(PndTrack *track, Double_t tuberadius);

	virtual bool equal(FairTimeStamp* data) {
	  PndSttHit* myDigi = dynamic_cast <PndSttHit*> (data);
	  if (myDigi != 0){
	    if (fTubeID == myDigi->GetTubeID())
						return true;
	  }
	  return false;
	}

	virtual bool operator<(const PndSttHit& myDigi) const{
		if (fTubeID < myDigi.GetTubeID()) 		return true;
		else return false;
	}

  friend std::ostream& operator<< (std::ostream& out, PndSttHit& digi){
	  out << "PndSttHit in Tube: " << digi.GetTubeID() << " Isochrone: " << digi.GetIsochrone() << " +/- " << digi.GetIsochroneError()
		  << " Charge: " << digi.GetDepCharge() << " Pulse: " << digi.GetPulse() << std::endl;
	  return out;
  }

 protected:
  
  /** This variable contains the radial distance to the wire **/    
  Double_t fIsochrone;
  /** This variable contains the error on the radial distance to the wire **/    
  Double_t fIsochroneError;

  /** time pulse **/
  Double_t fPulse; 

 /**  deposit charge (arbitrary units) **/
  Double_t fDepCharge; 

  /** tube id **/
  Int_t fTubeID; // CHECK added


 ClassDef(PndSttHit,1);
};


#endif
