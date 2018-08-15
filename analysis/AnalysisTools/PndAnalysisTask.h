#ifndef PNDANALYSISTASK_H
#define PNDANALYSISTASK_H 1


#include "FairTask.h"
#include <map>
#include <string>

class TClonesArray;
class TObjectArray;
class TH1F;

#include "RhoSelector/RhoMassParticleSelector.h"
#include "RhoSelector/RhoNeutralParticleSelector.h"
#include "RhoSelector/RhoPlusParticleSelector.h"
#include "RhoSelector/RhoMinusParticleSelector.h"
#include "RhoSelector/RhoSimpleElectronSelector.h"
#include "RhoSelector/RhoSimpleKaonSelector.h"
#include "RhoSelector/RhoSimpleMuonSelector.h"
#include "RhoSelector/RhoSimplePionSelector.h"
#include "RhoSelector/RhoSimpleProtonSelector.h"

class PndAnalysisTask : public FairTask
{

  public:
    typedef std::map<Int_t, Float_t> mapper;

    /** Default constructor **/
    PndAnalysisTask();


    /** Destructor **/
    ~PndAnalysisTask();


    /** Virtual method Init **/
    virtual InitStatus Init();


    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    virtual void Finish();
    //void CreateStructure();

  protected:

    int evcount;



    // **** some basic selectors
    //
    //RhoChargedParticleSelector *chargedSel;
    RhoNeutralParticleSelector* neutralSel;
    RhoPlusParticleSelector*    plusSel;
    RhoMinusParticleSelector*   minusSel;

    // **** mass selectors for the resonances/composites
    //
    RhoMassParticleSelector* phiMSel;
    RhoMassParticleSelector* pi0MSel;
    RhoMassParticleSelector* dsMSel;
    RhoSimpleKaonSelector* kSel;
    RhoSimplePionSelector* piSel;

    /**book all the histograms**/
    TH1F* phimass;
    TH1F* pi0mass;
    TH1F* dsmass;
    TH1F* ds0mass;
    TH1F* ppmass;

    TH1F* nmult;

  private:

    /** Input array of TpcLheTrack **/
    TClonesArray* fChargedArray;
    TClonesArray* fNeutralArray;


    /** Geo file to use **/

    /** Get parameter containers **/
    virtual void SetParContainers();


    ClassDef(PndAnalysisTask,1);

};

#endif
