#ifndef PNDMDTPOINT_H
#define PNDMDTPOINT_H


#include "TObject.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TMath.h"

#include "CbmMCPoint.h"

class PndMdtPoint : public CbmMCPoint 
{

 public:

  /** Default constructor **/
  PndMdtPoint();


  /** Constructor with arguments
   *@param Int_t fEventID		The ID of the event
   *@param Int_t fTrackID		The ID of the track
   *@param Int_t fTrackParentID		The ID of the parent track
   *@param Int_t fDetectorID		The ID of the detector
   *@param Int_t fPDG			The PDG code for the particle giving the current track
   *@param TLorentzVector fPos		The position of the hit [cm]
   *@param TLorentzVector fMom		The momentum of the particle giving the hit [GeV]
   *@param Double_t fELoss		contains the total energy at the layer [GeV] because it is omitted in pars
   **/

  /** Initializing constructor - see the above order **/  
  PndMdtPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
	     Double_t tof, Double_t length, Double_t eLoss, Int_t
	     MotherTrId, Int_t pdg, Int_t track_out);
  
  /** Copy constructor **/
 // PndMdtPoint(const PndMdtPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~PndMdtPoint();
  

  /** Accessors **/
 
  inline Int_t    GetTrackParentID()       const { return fTrackParentID;                  }; 
  inline Int_t    GetModule()              const { return fDetectorID<50 ? 1 : 2;          };
  inline Int_t    GetLayerID()             const { return fDetectorID<50 ? (Int_t)((fDetectorID-1)/2) : fDetectorID-51;};
  inline Int_t    GetSector(); 
  inline Int_t    GetPDG()                 const { return fPDG;                            };
  inline Int_t    GetTrackOut()            const { return fTrackOut;                       };
    
  /** Modifiers **/
  inline void SetTrackParentID(Int_t id)         { fTrackParentID = id;                               }; 
  inline void SetPDG(Int_t id)                   { fPDG           = id;                               };
  
  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;

   ClassDef(PndMdtPoint,1)
   
protected:
  Int_t fTrackParentID;		
  Int_t fPDG;			
  Int_t fTrackOut;
  
 
};

Int_t PndMdtPoint::GetSector()
{
  if (GetModule()==2) return -1;
  
  TVector3 pos(0.,0.,0.);
  Position(pos);
  Int_t sec = int((pos.Phi()*TMath::RadToDeg()+180+22.5)/45);
  if (sec==8) sec = 0;
  
  return sec;
}

#endif /* !PNDMDTPOINT_H */
