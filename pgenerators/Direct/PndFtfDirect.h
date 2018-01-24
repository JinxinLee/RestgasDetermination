// -------------------------------------------------------------------------
// -----                 PndFtfDirect header file                  -----
// -----               Created 11/04/08  by M.Al-Turany              -----
// -------------------------------------------------------------------------

/** PndFtfDirect.h
 *@author R.Kliemt <r.kliemt@gsi.de>
 *
 The PndFtfDirect generates FTF events using the Geant4 Code
 and inserts the tracks into the PndStack via the FairPrimaryGenerator.
 Derived from FairGenerator.

 Caveat: There are some files incuded directly iside the code, as it was provided
 in the executable by the original authors. This should be restructured.
 **/

#ifndef PND_FTFDIRECT_H
#define PND_FTFDIRECT_H

#include "FairGenerator.h"
#include "TF1.h"

namespace CLHEP{
  class RanluxEngine;
  class Hep3Vector;
}
//#include "CLHEP/Vector/ThreeVector.h"
//typedef CLHEP::Hep3Vector G4ThreeVector;

//#include "Test30Material.hh"
//#include "Test30Physics.hh"

#include "G4Types.hh"
#include "G4String.hh"
//#include "G4ThreeVector.hh"
//#include "G4PhysicalConstants.hh"
//#include "G4SystemOfUnits.hh"

//#include "G4Evaporation.hh"
class G4Evaporation;
class G4ExcitationHandler;
class G4PreCompoundModel;
class G4ParticleTable;
class Test30Material;
class Test30Physics;

//class G4ThreeVector;
class G4RunManager;
class G4VUserPhysicsList;
class G4DynamicParticle;
class G4VProcess;
class G4Step;
class G4Track;
class G4ParticleDefinition;
class G4Material;
class TVirtualMCStack;
class FairPrimaryGenerator;

class PndFtfDirect : public FairGenerator
{

public:

  /** Default constructor (should not be used) **/
  PndFtfDirect();

  /** Standard constructor
   *  @param Mom in GeV/C
   *  @param Mode = 0. - No elastic scattering, only inelastic
   *  @param Mode = 1. - Elastic and inelastic interactions (default)
   **/
  PndFtfDirect(const char * configfile);
  PndFtfDirect(const PndFtfDirect&);
  PndFtfDirect(const char * particle, const char * material, int targetA, const char * generator, double mom, int seed=0, bool noelastic=false);

  /** Destructor **/
  virtual ~PndFtfDirect();

  /** Generate one event using DPM
   ** @param primGen  pointer to the FairPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);
  Bool_t ProcessEvent(FairPrimaryGenerator* primGen);
  void SetVerbose(int v=1) {fverbose=v;};
  void SetNoElastic(bool n=true){fNoElastics=n;};
  void LoadG4();

private:
  void InitZero(); // initialize numbers etc. to defaults
  void parseConfigfile(); // parse the configuration file
  void Setup(); // setup
  bool fDoSetup;
  /**
   * P_lab(GeV/c)
   */
  float fMom;
  /**
   * 0. - No elastic scattering, only inelastic
   * 1. - Elastic and inelastic interactions
   */
  float  fMode;
  double fSeed;
  double fRsigma;
  float  fThtMin;

  G4RunManager* fG4RunManager;              //!
  G4VUserPhysicsList* fG4VUserPhysicsList;  //!
  std::ifstream* fin;                       //!
  Test30Material*  fmate;                   //!
  Test30Physics*   fphys;                   //!
  G4ParticleTable* fpartTable;              //!

  CLHEP::RanluxEngine* fdefaultEngine;      //!

  G4int     fverbose;
  G4bool    fsaverand;
  G4bool    fNoElastics;
  //
  G4String  fnamePart;                      //!
  G4bool    fionParticle;
  G4double  fPlab;   // VU
  G4double  fenergy;
  G4double  fsigmae;
  //
  G4int     fionZ, fionA;
  G4int     ftargetA;
  G4String  fnameMat;                       //!
  //
  G4bool    fShen;
  G4String  fnameGen;                       //!
  //
  G4int     fnevt;
  G4int     fmodu;
  //
  G4double ftheStep;
  G4Material* fmaterial;                    //!
  //
  G4bool fxsbgg;
  //
  //   // Track
  CLHEP::Hep3Vector* faPosition;   //!
  G4double      faTime;
  CLHEP::Hep3Vector* faDirection;  //!
  G4double fnx, fny, fnz;


  G4ExcitationHandler* ftheDeExcitation;   //!
  G4Evaporation* ftheEvaporation;          //!
  G4PreCompoundModel* fthePreCompound;     //!

  G4double fsigTot;
  G4double fsigEl;
  G4double fsigIn;

  int fnpart;
  //   Root initialization
  //TFile f1("FTF.root","RECREATE","ROOT_Tree");

  Int_t factiveCnt;
  //TTree* fTree = new TTree("data","FTF Background");
  //TClonesArray* fEvt;

  //fEvt=new TClonesArray("TParticle",100);
  //fTree->Branch("Npart",&activeCnt,"Npart/I");
  //fTree->Branch("Particles",&fEvt, 32000,99);

  G4DynamicParticle* dParticle;     //!
  G4VProcess* proc;                 //!
  G4Step* step;                     //!
  G4Track* gTrack;                  //!
  G4ParticleDefinition* part;       //!


  //G4ParticleDefinition* gamma;
  G4ParticleDefinition* electron;     //!
  G4ParticleDefinition* proton;       //!
  G4ParticleDefinition* neutron;      //!
  G4ParticleDefinition* pin;          //!
  G4ParticleDefinition* pip;          //!
  //G4ParticleDefinition* pi0;
  G4ParticleDefinition* deu;          //!
  G4ParticleDefinition* tri;          //!
  G4ParticleDefinition* he3;          //!
  G4ParticleDefinition* alp;          //!
  //G4ParticleDefinition* ion;

  G4ParticleDefinition* anti_proton;  //!
  G4ParticleDefinition* anti_neutron; //!
  G4ParticleDefinition* anti_deuteron;//!
  G4ParticleDefinition* anti_triton;  //!
  G4ParticleDefinition* anti_He3;     //!
  G4ParticleDefinition* anti_alpha;   //!

  void TestPointers()
  {
    std::cout<<"PndFtfDirect::fin               -> "<< fin << std::endl;
    std::cout<<"PndFtfDirect::fmate             -> "<< fmate << std::endl;
    std::cout<<"PndFtfDirect::fphys             -> "<< fphys << std::endl;
    std::cout<<"PndFtfDirect::fpartTable        -> "<< fpartTable << std::endl;
    std::cout<<"PndFtfDirect::fdefaultEngine    -> "<< fdefaultEngine << std::endl;
    std::cout<<"PndFtfDirect::fmaterial         -> "<< fmaterial << std::endl;
    std::cout<<"PndFtfDirect::faPosition        -> "<< faPosition << std::endl;
    std::cout<<"PndFtfDirect::faDirection       -> "<< faDirection << std::endl;
    std::cout<<"PndFtfDirect::ftheDeExcitation  -> "<< ftheDeExcitation << std::endl;
    std::cout<<"PndFtfDirect::ftheEvaporation   -> "<< ftheEvaporation << std::endl;
    std::cout<<"PndFtfDirect::fthePreCompound   -> "<< fthePreCompound << std::endl;
    std::cout<<"PndFtfDirect::dParticle         -> "<< dParticle << std::endl;
    std::cout<<"PndFtfDirect::proc              -> "<< proc << std::endl;
    std::cout<<"PndFtfDirect::step              -> "<< step << std::endl;
    std::cout<<"PndFtfDirect::gTrack            -> "<< gTrack << std::endl;
    std::cout<<"PndFtfDirect::part              -> "<< part << std::endl;
    std::cout<<"PndFtfDirect::electron          -> "<< electron << std::endl;
    std::cout<<"PndFtfDirect::proton            -> "<< proton << std::endl;
    std::cout<<"PndFtfDirect::neutron           -> "<< neutron << std::endl;
    std::cout<<"PndFtfDirect::pin               -> "<< pin << std::endl;
    std::cout<<"PndFtfDirect::pip               -> "<< pip << std::endl;
    std::cout<<"PndFtfDirect::deu               -> "<< deu << std::endl;
    std::cout<<"PndFtfDirect::tri               -> "<< tri << std::endl;
    std::cout<<"PndFtfDirect::he3               -> "<< he3 << std::endl;
    std::cout<<"PndFtfDirect::alp               -> "<< alp << std::endl;
    std::cout<<"PndFtfDirect::anti_proton       -> "<< anti_proton << std::endl;
    std::cout<<"PndFtfDirect::anti_neutron      -> "<< anti_neutron << std::endl;
    std::cout<<"PndFtfDirect::anti_deuteron     -> "<< anti_deuteron << std::endl;
    std::cout<<"PndFtfDirect::anti_triton       -> "<< anti_triton << std::endl;
    std::cout<<"PndFtfDirect::anti_He3          -> "<< anti_He3 << std::endl;
    std::cout<<"PndFtfDirect::anti_alpha        -> "<< anti_alpha << std::endl;
  };


  ClassDef(PndFtfDirect,1);

};

#endif
