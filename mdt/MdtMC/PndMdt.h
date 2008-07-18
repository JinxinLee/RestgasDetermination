
#ifndef PNDMDT_H
#define PNDMDT_H 1

#include "TROOT.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TString.h"

#include "TGeoManager.h"
#include "TGeoVolume.h"

#include "CbmRootManager.h"
#include "CbmDetector.h"
#include "CbmVolume.h"

#include "PndGeoMdtPar.h"
#include "PndMdtPoint.h"

class PndMdt : public CbmDetector
{
public:
    PndMdt();
    PndMdt(const char* name, Bool_t active);
    ~PndMdt();
    
// hit
    inline void Register() { CbmRootManager::Instance()->Register("MdtPoint","Mdt",fMdtCollection,kTRUE); };
    
    PndMdtPoint* AddHit();
    void ResetParameters();
    void CopyClones(TClonesArray*, TClonesArray*, Int_t);
    void Print() const;
    void Reset();

/** Accessors **/
    inline Int_t GetPosIndex()               const { return fPosIndex; };
    inline Int_t GetEventID()                const { return fEventID; };
    inline Int_t GetTrackID()                const { return fTrackID; };
    inline Int_t GetTrackParentID()          const { return fTrackParentID; };
    inline Int_t GetDetectorID()             const { return fDetectorID; };
    inline Int_t GetLayerID()                const { return fDetectorID<50 ? (Int_t)(fDetectorID/2) : fDetectorID-50;}; 
    inline Int_t GetModule()                 const { return fDetectorID<50 ? 1 : 2;             };
    inline Int_t GetPDG()                    const { return fPDG; };
    inline TLorentzVector GetPos()           const { return fPos; };
    inline TLorentzVector GetMom()           const { return fMom; };
    TClonesArray* GetCollection(Int_t iColl) const { if(iColl==0) return fMdtCollection; return NULL; };
    
/** Modifiers **/
    inline void SetPosIndex(Int_t pin)      { fPosIndex = pin; };
    inline void SetEventID(Int_t eid)       { fEventID = eid; };
    inline void SetTrackID(Int_t tid)       { fTrackID = tid; };
    inline void SetTrackParentID(Int_t tpi) { fTrackParentID = tpi; };
    inline void SetDetectorID(Int_t lid)    { fDetectorID = lid; };
    inline void SetPDG(Int_t pdg)           { fPDG = pdg; };
    inline void SetPos(TLorentzVector vect) { fPos.SetXYZT(vect.X(),vect.Y(),vect.Z(),vect.T()); };
    inline void SetMom(TLorentzVector vect) { fMom.SetXYZT(vect.X(),vect.Y(),vect.Z(),vect.T()); };

// geometry    
    void SetParFile(TString);
        
// detector
    inline void SetVerbosity(Bool_t verbosity) { fVerboseLevel = verbosity; };

    void ConstructGeometry();
    void Initialize();
    void BeginEvent();
    Bool_t ProcessHits(CbmVolume* vol);
    void EndOfEvent();

    
private:
// hit
    TClonesArray* fMdtCollection; //!
    Int_t fPosIndex;      
    Int_t fEventID;       
    Int_t fTrackID;       
    Int_t fTrackParentID; 
    Int_t fDetectorID;       
    Int_t fPDG;           
    TLorentzVector fPos;  
    TLorentzVector fMom;  

// geometry
    void SetBE();

    TString ffn;          
    Double_t displacement;
    
    struct {
	Double_t dx1;
	Double_t dx2;
	Double_t dy1;
	Double_t dy2;
	Double_t dz;
	Double_t x0;
	Double_t y0;
	Double_t z0;
    } be;                 //! barrel edge needed by mdt and mag

// detector
    Bool_t fVerboseLevel; 

    PndGeoMdtPar* par;    

    TString fVolumeName;  
    Int_t lEventID;       
    Int_t lTrackID;       

protected:
    ClassDef(PndMdt,1)

};

#endif /* !PNDMDT_H */
