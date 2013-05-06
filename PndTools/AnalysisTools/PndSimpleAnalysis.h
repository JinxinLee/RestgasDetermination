/******************************************************
Class PndSimpleAnalysis

Task to perform analysis (combinatorics, simple filetring)
by setting a configuration file

Writes out a TTree 'ntp'

Author: K.Goetzen, GSI, 06/2008

*******************************************************/

#ifndef PNDSIMPLEANALYSIS_H
#define PNDSIMPLEANALYSIS_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include <vector>
#include "RhoBase/RhoCandList.h"
#include "TLorentzVector.h"

#define fMaxEntries 1000

class TClonesArray;
class TObjectArray;
class TDatabasePDG;
class TH1F;
class TTree;

class RhoChargedParticleSelector;
class RhoNeutralParticleSelector;
class RhoPlusParticleSelector;
class RhoMinusParticleSelector;
class RhoMassParticleSelector;

class RhoSimpleElectronSelector;
class RhoSimpleMuonSelector;
class RhoSimplePionSelector;
class RhoSimpleKaonSelector;
class RhoSimpleProtonSelector;

class RhoCandidate;
//class RhoCandList;

class PndListDefiner;


class PndSimpleAnalysis : public FairTask
{

  public:
    typedef std::map<Int_t, Float_t> mapper;

    /** Default constructor **/
    PndSimpleAnalysis();

    PndSimpleAnalysis(std::string filename);

    /** Destructor **/
    ~PndSimpleAnalysis();


    /** Virtual method Init **/
    virtual InitStatus Init();


    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    virtual void Finish();
    //void CreateStructure();

  protected:

    int evcount;
    void PrintTree(RhoCandidate* tc, int level=0);
    void SetConfigFile(std::string filename="analysis.cfg");
    bool SetupAnalysis();
    void InitGenericLists();
    void FillGenericLists();
    void InitColumnNames();

    bool IsGenericListName(std::string n);
    int GetPdgCode(std::string name);
    int GetAntiPdgCode(std::string name);
    int GetAntiPdgCode(int pdgcode);

    bool ErrorMessage(int mid, int line=0, std::string arg="");

    // **** some basic selectors
    //
    //RhoChargedParticleSelector *chargedSel;
    RhoNeutralParticleSelector*        neutralSel;
    RhoPlusParticleSelector*           plusSel;
    RhoMinusParticleSelector*          minusSel;

    RhoSimpleElectronSelector* eSel;
    RhoSimpleMuonSelector*     muSel;
    RhoSimplePionSelector*     piSel;
    RhoSimpleKaonSelector*     kSel;
    RhoSimpleProtonSelector*   pSel;

    // **** mass selectors for the resonances/composites
    //


  private:

    /** Input array  **/
    TClonesArray* fChargedArray;
    TClonesArray* fNeutralArray;
    TClonesArray* fChargedProbability;
    TClonesArray* fNeutralProbability;

    TClonesArray* fMcArray;

    TClonesArray* fMicroArray;

    TDatabasePDG* fdbPdg;

    RhoCandList chargedCands;
    RhoCandList neutralCands;
    RhoCandList mcCands;

    std::vector<std::string>     fGenericListNames; // names of standard lists
    std::vector<PndListDefiner*> fListDefiners;   // mapping of tree structure
    std::map<std::string,int>    fListMap;      // maps list names to PndListDefiner index

    std::map<std::string,int>    fColKeyMap;      // maps column names for dumpout to int keys
    std::map<int, std::vector<std::string> > fColShortKeyMap;  // maps col index to a list of col indices

    std::string   fCfgFileName;

    TTree*   ntp;

    //int fMaxEntries;      // the maximum number of entries in column

    TLorentzVector fpInit;

    /** Geo file to use **/

    /** Get parameter containers **/
    virtual void SetParContainers();


    ClassDef(PndSimpleAnalysis,1);

};

#endif
