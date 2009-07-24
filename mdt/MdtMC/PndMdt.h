
#ifndef PNDMDT_H
#define PNDMDT_H 1

#include "TROOT.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TString.h"

#include "TGeoManager.h"
#include "TGeoVolume.h"

#include "FairRootManager.h"
#include "FairDetector.h"
#include "FairVolume.h"

#include "PndGeoMdtPar.h"
#include "PndMdtPoint.h"

class PndMdt : public FairDetector
{
public:
    PndMdt();
    PndMdt(const char* name, Bool_t active);
    ~PndMdt();
    
    void SetMdtVersion(TString);
    inline void SetMdtMagnet(bool opt=false) { mdtMagnet = opt; return; };
    inline void SetMdtMF(bool opt=false) { mdtMF = opt; return; };
    inline void SetMdtMFIron(bool opt=false) { mdtMFI = opt; return; };

// hit
    inline void Register() { FairRootManager::Instance()->Register("MdtPoint","Mdt",fMdtCollection,kTRUE); };
    
   
    void ResetParameters();
    void CopyClones(TClonesArray*, TClonesArray*, Int_t);
    void Print() const;
    void Reset();

/** Accessors **/
    inline Int_t GetPosIndex()            const { return fPosIndex;              };
    inline Int_t GetLayerID()             const { return fDetectorID<200 ? fDetectorID%8 : fDetectorID<300 ? (fDetectorID-200)%8 : (fDetectorID-300)%8; };
    inline Int_t GetModule()              const { return fDetectorID<200 ? 1 : fDetectorID<300 ? 2 : 3; };

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
    Bool_t ProcessHits(FairVolume* vol);
    void EndOfEvent();

    static Int_t fTrkIn; 
    static TLorentzVector fPos_In;
    static TLorentzVector fMom_In;

	ClassDef(PndMdt,1)
    
private:
    TString version;
    void PndMdtMagnet(); //!
    void PndMdtMuonFilter(); //!
    void PndMdtMFIron(); //!
    void ConstructGeometryTo(); //!
    void ConstructGeometryDu(); //!
    Bool_t ProcessHitsTo(FairVolume* vol); //!
    Bool_t ProcessHitsDu(FairVolume* vol); //!

// hit
    TClonesArray* fMdtCollection; //!
    Int_t fPosIndex;      
    Int_t fDetectorID;               
    TLorentzVector fPos;  
    TLorentzVector fMom;  

// detector
    Bool_t fVerboseLevel; 
    PndGeoMdtPar* par;   //! 
    TString ffn;
    bool mdtMagnet; //!
    bool mdtMF; //!
    bool mdtMFI; //!

};

#endif /* !PNDMDT_H */
