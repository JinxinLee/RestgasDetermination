
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
  PndMdtPoint(Int_t,Int_t,Int_t,Int_t,Int_t,TLorentzVector,TLorentzVector);
  
  /** Copy constructor **/
  PndMdtPoint(const PndMdtPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~PndMdtPoint();
  

  /** Accessors **/
  inline Int_t    GetEventID()             const { return fEventID;                        };
//  inline Int_t    GetTrackID()             const { return fTrackID;                        }; 
  inline Int_t    GetTrackParentID()       const { return fTrackParentID;                  }; 
  inline Int_t    GetDetectorID()          const { return fDetectorID;                     };
  inline Int_t    GetModule()              const { return fDetectorID<50 ? 1 : 2;          };
  inline Int_t    GetLayerID()             const { return fDetectorID<50 ? (Int_t)((fDetectorID-1)/2) : fDetectorID-51;};
  inline Int_t    GetSector(); 
  inline Int_t    GetPDG()                 const { return fPDG;                            };
  inline Double_t GetX()                   const { return fPos.X();                        };
  inline Double_t GetY()                   const { return fPos.Y();                        };
  inline Double_t GetZ()                   const { return fPos.Z();                        };
  inline Double_t GetT()                   const { return fPos.T();                        };
  inline Double_t GetPx()                  const { return fMom.X();                        };
  inline Double_t GetPy()                  const { return fMom.Y();                        };
  inline Double_t GetPz()                  const { return fMom.Z();                        };
  inline Double_t GetEnergy()              const { return fMom.T();                        };
  inline TLorentzVector GetPosition()            { return fPos;                            };
  inline TLorentzVector GetMomentum()            { return fMom;                            };
/* stupid inheritance */
  inline void Position(TVector3& pos)            { pos.SetXYZ(fPos.X(),fPos.Y(),fPos.Z()); };
  inline void Momentum(TVector3& mom)            { mom.SetXYZ(fMom.X(),fMom.Y(),fMom.Z()); };
   
  /** Modifiers **/
  inline void SetEventID(Int_t id)               { fEventID       = id;                               };
//  inline void SetTrackID(Int_t id)               { fTrackID       = id;                               }; 
  inline void SetTrackParentID(Int_t id)         { fTrackParentID = id;                               }; 
  inline void SetDetectorID(Int_t id)            { fDetectorID    = id;                               }; 
  inline void SetPDG(Int_t id)                   { fPDG           = id;                               };
  inline void SetX(Double_t val)                 { fPos.SetX(val);                                    };
  inline void SetY(Double_t val)                 { fPos.SetY(val);                                    };
  inline void SetZ(Double_t val)                 { fPos.SetZ(val);                                    };
  inline void SetT(Double_t val)                 { fPos.SetT(val);                                    };
  inline void SetPx(Double_t val)                { fMom.SetX(val);                                    };
  inline void SetPy(Double_t val)                { fMom.SetY(val);                                    };
  inline void SetPz(Double_t val)                { fMom.SetZ(val);                                    };
  inline void SetEnergy(Double_t val)            { fMom.SetT(val);                                    };
  inline void SetPosition(TLorentzVector vect)   { fPos.SetXYZT(vect.X(),vect.Y(),vect.Z(),vect.T()); };
  inline void SetMomentum(TLorentzVector vect)   { fMom.SetXYZT(vect.X(),vect.Y(),vect.Z(),vect.T()); };
  
  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;


protected:
  Int_t fEventID;		
//  Int_t fTrackID;		
  Int_t fTrackParentID;		
  Int_t fPDG;			
  TLorentzVector fPos;          
  TLorentzVector fMom;          
      
  ClassDef(PndMdtPoint,1)
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
