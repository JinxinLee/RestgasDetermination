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
   *  @param Mode = 1. - Elastic and inelastic interactions
   *  @param Mode = 2. - Only elastic scattering, no inelastic one
   **/
  PndFtfDirect(const char * configfile);
  PndFtfDirect(Double_t Mom, Int_t Mode, Long_t Seed, Double_t ThtMin);

  /** Destructor **/
  virtual ~PndFtfDirect();

  /** Generate one event using DPM
   ** @param primGen  pointer to the FairPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

 private:

  void InitZero(); // initialize numbers etc. to defaults
  void parseConfigfile(); // parse the configuration file
  void Setup(); // setup

  /**
   * P_lab(GeV/c)
  */
  float fMom;
  /**
  * 0. - No elastic scattering, only inelastic
  * 1. - Elastic and inelastic interactions
  * 2. - Only elastic scattering, no inelastic one
  */
  float  fMode;
  double fSeed;
  double fRsigma;
  float  fThtMin;



  std::ifstream* fin;
  Test30Material*  fmate;
  Test30Physics*   fphys;
  G4ParticleTable* fpartTable;

  CLHEP::RanluxEngine* fdefaultEngine;

   G4int     fverbose;
   G4bool    fsaverand;
//
   G4String  fnamePart;
   G4bool    fionParticle;
   G4double  fPlab;   // VU
   G4double  fenergy;
   G4double  fsigmae;
//
   G4int     fionZ, fionA;
   G4int     ftargetA;
   G4String  fnameMat;
//
   G4bool    fShen;
   G4String  fnameGen;
//
   G4int     fnevt;
   G4int     fmodu;
//
   G4double ftheStep;
   G4Material* fmaterial;
//
   G4bool fxsbgg;
//
//   // Track
   CLHEP::Hep3Vector* faPosition;
   G4double      faTime;
   CLHEP::Hep3Vector* faDirection;
   G4double fnx, fny, fnz;


  G4ExcitationHandler* ftheDeExcitation;
  G4Evaporation* ftheEvaporation;
  G4PreCompoundModel* fthePreCompound;

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

	  G4DynamicParticle* dParticle;
	  G4VProcess* proc;
	  G4Step* step;
	  G4Track* gTrack;
	  G4ParticleDefinition* part;


//  static const G4ParticleDefinition* gamma;
static const G4ParticleDefinition* electron;
static const G4ParticleDefinition* proton;
static const G4ParticleDefinition* neutron;
static const G4ParticleDefinition* pin;
static const G4ParticleDefinition* pip;
//  static const G4ParticleDefinition* pi0;
static const G4ParticleDefinition* deu;
static const G4ParticleDefinition* tri;
static const G4ParticleDefinition* he3;
static const G4ParticleDefinition* alp;
//static const G4ParticleDefinition* ion;

static const G4ParticleDefinition* anti_proton;
static const G4ParticleDefinition* anti_neutron;
static const G4ParticleDefinition* anti_deuteron;
static const G4ParticleDefinition* anti_triton;
static const G4ParticleDefinition* anti_He3;
static const G4ParticleDefinition* anti_alpha;



  ClassDef(PndFtfDirect,1);

};

#endif
