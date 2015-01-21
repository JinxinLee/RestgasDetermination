#ifndef PNDMDTPOINT_H
#define PNDMDTPOINT_H


#include "TObject.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TMath.h"

#include "FairMCPoint.h"

class PndMdtPoint : public FairMCPoint 
{

 public:

  /** Default constructor **/
  PndMdtPoint();


  /** Constructor with arguments
   *@param Int_t fEventID		The ID of the event
   *@param Int_t fTrackID		The ID of the track
   *@param Int_t fDetectorID		The ID of the detector
   *@param TLorentzVector fPosIn        The position of the hit [cm] at the exitentrance point
   *@param TLorentzVector fMomIn	The momentum of the particle giving the hit [GeV] at the exitentrance point
   *@param TLorentzVector fPos		The position of the hit [cm] at the exit point
   *@param TLorentzVector fMom		The momentum of the particle giving the hit [GeV] at the exit point
   *@param Double_t fELoss		contains the total energy at the layer [GeV] because it is omitted in pars

   **/

  /** Initializing constructor - see the above order **/  
  PndMdtPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
	      Double_t tof, Double_t length, Double_t eLoss,
	      TVector3 pos_in, TVector3 mom_in);
  
  /** Copy constructor **/
 // PndMdtPoint(const PndMdtPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~PndMdtPoint();
  

  /** Accessors **/
 
  inline TVector3 GetPosIn()            const { return fPosIn;                          };
  inline TVector3 GetMomIn()            const { return fMomIn;                          }; 
  inline TVector3 GetPosOut()           const { return TVector3(fX, fY, fZ);            };
  inline TVector3 GetMomOut()           const { return TVector3(fPx, fPy, fPz);         };
  
  Short_t GetModule()      const { return (fDetectorID/1000000);};
  Short_t GetSector()      const { return ((fDetectorID/100000)%10);};
  Short_t GetLayerID()     const { return ((fDetectorID/1000)%100);};
  Short_t GetBox()         const { return ((fDetectorID/10)%100);};
  Short_t GetWire()        const { return (fDetectorID%10);};
  
  /** Modifiers **/
  
  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;

   ClassDef(PndMdtPoint,2)
   
protected:
  
  TVector3 fPosIn;
  TVector3 fMomIn;
 
};


#endif /* !PNDMDTPOINT_H */
