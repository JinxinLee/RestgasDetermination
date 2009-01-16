
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
    
    void SetMdtVersion(TString);

// hit
    inline void Register() { CbmRootManager::Instance()->Register("MdtPoint","Mdt",fMdtCollection,kTRUE); };
    
   
    void ResetParameters();
    void CopyClones(TClonesArray*, TClonesArray*, Int_t);
    void Print() const;
    void Reset();

/** Accessors **/
    inline Int_t GetPosIndex()               const { return fPosIndex; };
    inline Int_t GetLayerID()                const { return fDetectorID<50 ? (Int_t)(fDetectorID/2) : fDetectorID-50;}; 
    inline Int_t GetModule()                 const { return fDetectorID<50 ? 1 : 2;             };

    TClonesArray* GetCollection(Int_t iColl) const ;
    
/** Modifiers **/
    inline void SetPosIndex(Int_t pin)      { fPosIndex = pin; };
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

    static Int_t fTrkIn; 
    static TLorentzVector fPos_In;
    static TLorentzVector fMom_In;

	ClassDef(PndMdt,1)
    
private:
    TString version;
    void ConstructGeometryTo();
    void ConstructGeometryDu();
    Bool_t ProcessHitsTo(CbmVolume* vol);
    Bool_t ProcessHitsDu(CbmVolume* vol);

// hit
    TClonesArray* fMdtCollection; //!
    Int_t fPosIndex;      
    Int_t fDetectorID;               
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
    PndGeoMdtPar* par;   //! 
    TString fVolumeName;  
};

#endif /* !PNDMDT_H */
