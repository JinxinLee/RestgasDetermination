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
	     MotherTrId, Int_t pdg, TVector3 pos_in, TVector3 mom_in);
  
  /** Copy constructor **/
 // PndMdtPoint(const PndMdtPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~PndMdtPoint();
  

  /** Accessors **/
 
  inline Int_t    GetTrackParentID()    const { return fTrackParentID;                  }; 
  inline Int_t GetLayerID()             const { return fDetectorID<200 ? (fDetectorID - fDetectorID%8)/8 : 
						       fDetectorID<300 ? (fDetectorID-200 - (fDetectorID-200)%8)/8 : 
						       (fDetectorID-300 - (fDetectorID-300)%8)/8; };
  inline Int_t GetModule()              const { return fDetectorID<200 ? 1 : fDetectorID<300 ? 2 : 3; };
  inline Int_t    GetSector(); 
  inline Int_t    GetPDG()              const { return fPDG;                            };
  inline TVector3 GetPosIn()            const { return fPosIn;                          };
  inline TVector3 GetMomIn()            const { return fMomIn;                          };
    
  /** Modifiers **/
  inline void SetTrackParentID(Int_t id)         { fTrackParentID = id;                               }; 
  inline void SetPDG(Int_t id)                   { fPDG           = id;                               };
  
  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;

   ClassDef(PndMdtPoint,1)
   
protected:
  Int_t fTrackParentID;		
  Int_t fPDG;			
  TVector3 fPosIn;
  TVector3 fMomIn;
 
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
