// -------------------------------------------------------------------------
// -----                PndFtfDirect source file                  -----
// -----                                                          -----
// -------------------------------------------------------------------------

#include <iostream>
#include "TClonesArray.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "TVector3.h"
#include "TParticle.h"
#include "PndFtfDirect.h"
#include "FairPrimaryGenerator.h"
#include "TRandom.h"

using namespace std;

//#include "cfortran.h"

#include "globals.hh"
#include "G4Version.hh"
#include "G4ios.hh"
#include <fstream>
#include <iomanip>
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"
#include "G4MaterialCutsCouple.hh"
#include "G4ElementVector.hh"
#include "Test30Material.hh"
#include "Test30Physics.hh"

#include "G4ProcessManager.hh"
#include "G4VParticleChange.hh"
#include "G4ParticleChange.hh"
#include "G4HadronCrossSections.hh"
#include "G4HadronicProcess.hh"
#include "G4ChargeExchangeProcess.hh"
#include "G4VCrossSectionDataSet.hh"
#include "G4ProtonInelasticCrossSection.hh"
#include "G4NeutronInelasticCrossSection.hh"
#include "G4HadronInelasticDataSet.hh"
#include "G4HadronElasticDataSet.hh"
#include "G4IonsShenCrossSection.hh"
#include "G4TripathiCrossSection.hh"
#include "G4TripathiLightCrossSection.hh"
#include "G4HadronElasticDataSet.hh"
#include "G4PiNuclearCrossSection.hh"
#include "G4IonProtonCrossSection.hh"

#include "G4BGGNucleonElasticXS.hh"
#include "G4BGGPionElasticXS.hh"
#include "G4BGGNucleonInelasticXS.hh"
#include "G4BGGPionInelasticXS.hh"

#include "G4HadronNucleonXsc.hh"             // Uzhi MK Cross-Sections
#include "G4VComponentCrossSection.hh"       // Uzhi
#include "G4ComponentAntiNuclNuclearXS.hh"   // Uzhi

#include "G4ParticleTable.hh"
#include "G4ParticleChange.hh"
#include "G4DynamicParticle.hh"
#include "G4AntiProton.hh"
#include "G4Neutron.hh"
#include "G4Proton.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"
#include "G4PionZero.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4Alpha.hh"
#include "G4He3.hh"
#include "G4Deuteron.hh"
#include "G4Triton.hh"
#include "G4IonTable.hh"
#include "G4DecayPhysics.hh"

#include "G4ForceCondition.hh"
#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4Step.hh"
#include "G4GRSVolume.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "G4ExcitationHandler.hh"
#include "G4PreCompoundModel.hh"
#include "G4Evaporation.hh"

#include "G4StateManager.hh"
#include "G4NistManager.hh"

//#include "Histo.hh"
#include "G4Timer.hh"

#include "G4ForceCondition.hh"
#include "G4TouchableHistory.hh"

#include "G4NucleiProperties.hh"
#include "G4ProductionCuts.hh"                    // Uzhi 1.02.13
#include "G4ProductionCutsTable.hh"               // Uzhi 1.02.13
#include "G4ChipsComponentXS.hh"                  // Uzhi 29.01.13
#include "UZHI_diffraction.hh"
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "TROOT.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TStopwatch.h"
#include "TParticle.h"

//  const G4ParticleDefinition* PndFtfDirect::gamma = G4Gamma::Gamma();
const G4ParticleDefinition* PndFtfDirect::electron = G4Electron::Electron();
const G4ParticleDefinition* PndFtfDirect::proton = G4Proton::Proton();
const G4ParticleDefinition* PndFtfDirect::neutron = G4Neutron::Neutron();
const G4ParticleDefinition* PndFtfDirect::pin = G4PionMinus::PionMinus();
const G4ParticleDefinition* PndFtfDirect::pip = G4PionPlus::PionPlus();
//  const G4ParticleDefinition* PndFtfDirect::pi0 = G4PionZero::PionZero();
const G4ParticleDefinition* PndFtfDirect::deu = G4Deuteron::DeuteronDefinition();
const G4ParticleDefinition* PndFtfDirect::tri = G4Triton::TritonDefinition();
const G4ParticleDefinition* PndFtfDirect::he3 = G4He3::He3Definition();
const G4ParticleDefinition* PndFtfDirect::alp = G4Alpha::AlphaDefinition();
//  const G4ParticleDefinition* PndFtfDirect::ion = G4GenericIon::GenericIon();

const G4ParticleDefinition* PndFtfDirect::anti_proton   = G4AntiProton::AntiProton();
const G4ParticleDefinition* PndFtfDirect::anti_neutron  = G4AntiNeutron::AntiNeutron();
const G4ParticleDefinition* PndFtfDirect::anti_deuteron = G4AntiDeuteron::AntiDeuteron();
const G4ParticleDefinition* PndFtfDirect::anti_triton   = G4AntiTriton::AntiTriton();
const G4ParticleDefinition* PndFtfDirect::anti_He3      = G4AntiHe3::AntiHe3();
const G4ParticleDefinition* PndFtfDirect::anti_alpha    = G4AntiAlpha::AntiAlpha();

// -----   Default constructor   ------------------------------------------
PndFtfDirect::PndFtfDirect() :
fDoSetup(true),
fMom(0.),
fMode(0.),
fSeed(0.),
fRsigma(0.),
fThtMin(0.),
fin(0),
fmate(0),
fphys(0),
fpartTable(0),
fdefaultEngine(0),
fverbose(0),
fsaverand(false),
fNoElastics(false),
fnamePart("proton"),
fionParticle(false),
fPlab(100.),//*GeV;   // VU
fenergy(100.),//*GeV;
fsigmae(0.0),
fionZ(0),
fionA(0),
ftargetA(0),
fnameMat("G4_Al"),
fShen(false),
fnameGen("binary"),
fnevt(1000),
fmodu(1000),
ftheStep(0.01),//*micrometer;
fmaterial(0),
fxsbgg(true),
faPosition(0),
faTime(0.),
faDirection(0),
fnx(0.0),
fny(0.0),
fnz(0.0),
ftheDeExcitation(0),
ftheEvaporation(0),
fthePreCompound(0),
fsigTot(0),
fsigEl(0),
fsigIn(0),
fnpart(0),
factiveCnt(0),
dParticle(0),
proc(0),
step(0),
gTrack(0),
part(0)
{

}
// ------------------------------------------------------------------------
PndFtfDirect::PndFtfDirect(const PndFtfDirect& other) :
FairGenerator(other),
fDoSetup(true),
fMom(other.fMom),
fMode(other.fMode),
fSeed(other.fSeed),
fRsigma(other.fRsigma),
fThtMin(other.fThtMin),
fin(0),
fmate(0),
fphys(0),
fpartTable(other.fpartTable),
fdefaultEngine(0),
fverbose(other.fverbose),
fsaverand(other.fsaverand),
fNoElastics(other.fNoElastics),
fnamePart(other.fnamePart),
fionParticle(other.fionParticle),
fPlab(other.fPlab),//*GeV;   // VU
fenergy(other.fenergy),//*GeV;
fsigmae(other.fsigmae),
fionZ(other.fionZ),
fionA(other.fionA),
ftargetA(other.ftargetA),
fnameMat(other.fnameMat),
fShen(other.fShen),
fnameGen(other.fnameGen),
fnevt(other.fnevt),
fmodu(other.fmodu),
ftheStep(other.ftheStep),//*micrometer;
fmaterial(other.fmaterial),
fxsbgg(other.fxsbgg),
faPosition(0),
faTime(other.faTime),
faDirection(0),
fnx(other.fnx),
fny(other.fny),
fnz(other.fnz),
ftheDeExcitation(0),
ftheEvaporation(0),
fthePreCompound(0),
fsigTot(other.fsigTot),
fsigEl(other.fsigEl),
fsigIn(other.fsigIn),
fnpart(other.fnpart),
factiveCnt(other.factiveCnt),
dParticle(0),
proc(other.proc),
step(other.step),
gTrack(other.gTrack),
part(other.part)
{

}
// ------------------------------------------------------------------------
// -----   Standard constructor   -----------------------------------------

PndFtfDirect::PndFtfDirect(const char * configfile) :
fDoSetup(true),
fMom(0.),
fMode(0.),
fSeed(0.),
fRsigma(0.),
fThtMin(0.),
fin(0),
fmate(0),
fphys(0),
fpartTable(0),
fdefaultEngine(0),
fverbose(0),
fsaverand(false),
fNoElastics(false),
fnamePart("proton"),
fionParticle(false),
fPlab(100.),//*GeV;   // VU
fenergy(100.),//*GeV;
fsigmae(0.0),
fionZ(0),
fionA(0),
ftargetA(0),
fnameMat("G4_Al"),
fShen(false),
fnameGen("binary"),
fnevt(1000),
fmodu(1000),
ftheStep(0.01),//*micrometer;
fmaterial(0),
fxsbgg(true),
faPosition(0),
faTime(0.),
faDirection(0),
fnx(0.0),
fny(0.0),
fnz(0.0),
ftheDeExcitation(0),
ftheEvaporation(0),
fthePreCompound(0),
fsigTot(0),
fsigEl(0),
fsigIn(0),
fnpart(0),
factiveCnt(0),
dParticle(0),
proc(0),
step(0),
gTrack(0),
part(0)
{
  fdefaultEngine = new CLHEP::RanluxEngine( 1234567, 4 );
  faPosition = new CLHEP::Hep3Vector(0.,0.,0.);
  faDirection      = new CLHEP::Hep3Vector(0.0,0.0,1.0);


  CLHEP::HepRandom::setTheEngine( fdefaultEngine );
  G4cout << "========================================================" << G4endl;
  G4cout << "======              FTF Test Start              ========" << G4endl;
  G4cout << "========================================================" << G4endl;
  // -------------------------------------------------------------------
  // Control on input
  fin = new std::ifstream();
  G4String fname = configfile;
  fin->open(fname.c_str());
  if( !fin->is_open()) {
    G4cout << "Input file <" << fname << "> does not exist! Exit" << G4endl;
    exit(1);
  }

  InitZero();

  parseConfigfile();

  //Setup();

  // Calculate ThtMin first. For this we make a cut-off on the value of -t of 1e-2 GeV^2 (~100 MeV/c momentum)
  // This estimated from a parametrization found in thesis of Thomas Wuerschig (figure 6.4, page 121):
  // Roughly: 0.4 deg at 15 GeV/c and 4 deg at 1.5 GeV/c, lineair interpolation in double log-scale.
  //
  //  Double_t logangle = TMath::Log(0.4)+(TMath::Log(15.)-TMath::Log(Mom))*(TMath::Log(4)-TMath::Log(0.4))/(TMath::Log(15)-TMath::Log(1.5));
  //  Double_t ThtMin = TMath::Exp(logangle);
  //
}


PndFtfDirect::PndFtfDirect(const char * particle, const char * material, int targetA, const char * generator, double mom, int seed, bool noelastic) :
fDoSetup(true),
fMom(0.),
fMode(0.),
fSeed(0.),
fRsigma(0.),
fThtMin(0.),
fin(0),
fmate(0),
fphys(0),
fpartTable(0),
fdefaultEngine(0),
fverbose(0),
fsaverand(false),
fNoElastics(noelastic),
fnamePart("proton"),
fionParticle(false),
fPlab(100.),//*GeV;   // VU
fenergy(100.),//*GeV;
fsigmae(0.0),
fionZ(0),
fionA(0),
ftargetA(0),
fnameMat("G4_Al"),
fShen(false),
fnameGen("binary"),
fnevt(1000),
fmodu(1000),
ftheStep(0.01),//*micrometer;
fmaterial(0),
fxsbgg(true),
faPosition(0),
faTime(0.),
faDirection(0),
fnx(0.0),
fny(0.0),
fnz(0.0),
ftheDeExcitation(0),
ftheEvaporation(0),
fthePreCompound(0),
fsigTot(0),
fsigEl(0),
fsigIn(0),
fnpart(0),
factiveCnt(0),
dParticle(0),
proc(0),
step(0),
gTrack(0),
part(0)
{
  fdefaultEngine = new CLHEP::RanluxEngine( seed, 4 );
  faPosition = new CLHEP::Hep3Vector(0.,0.,0.);
  faDirection      = new CLHEP::Hep3Vector(0.0,0.0,1.0);


  CLHEP::HepRandom::setTheEngine( fdefaultEngine );
  G4cout << "========================================================" << G4endl;
  G4cout << "======              FTF Test Start              ========" << G4endl;
  G4cout << "========================================================" << G4endl;
  // -------------------------------------------------------------------
  // Control on input

  InitZero();

  fnamePart=particle;
  fionParticle= false;
  fnameMat=material;
  ftargetA=targetA;
  fnameGen=generator;
  if(fnameGen == "elastic" || fnameGen == "HElastic" || fnameGen == "DElastic") { fNoElastics=false; }
  fPlab=mom;
  fPlab *= GeV;

  //Setup();

}


//PndFtfDirect::PndFtfDirect(Double_t Mom, Int_t Mode, Long_t Seed, Double_t ThtMin)
//{
//  faPosition = new CLHEP::Hep3Vector(0.,0.,0.);
//  faDirection      = new CLHEP::Hep3Vector(0.0,0.0,1.0);
//  InitZero();
//
//  ////////////
//  fMom    = Mom;
//  fMode   = Mode;
//  fNoElastics = (fMode>0) ? false : true;
//  fSeed   = Seed;
//  fThtMin = ThtMin;
//
//  if (fSeed < 0)
//  {
//    Long_t iSeed = gRandom->GetSeed();
//    int a = iSeed/100000;
//    fSeed=iSeed - a*100000 + a/100000.;
//  }
//
//  //    fGasmode = 0;
//  fRsigma = 0.;
//  //    fDensityFunction = new TF1();
//
//  cout << "<I> PndFtfDirect initialization" << endl;
//  cout << "<I> Momentum  = " << fMom << endl;
//  cout << "<I> Seed      = " << fSeed << endl;
//  cout << "<I> Mode      = " << fMode << endl;
//  cout << "<I> Theta min = " << fThtMin<<endl;
//
//  ////////////
//
//  Setup();
//
//}
// ------------------------------------------------------------------------

// -----   Destructor   ---------------------------------------------------
PndFtfDirect::~PndFtfDirect() {
  //  delete pFrame;
  //  delete lFrame;
  //  delete sFrame;
  delete fmate;
  delete fin;
  delete fphys;
  delete faPosition;
  delete faDirection;
  fpartTable->DeleteAllParticles();
  if(dParticle) delete dParticle; // RK
                                  //f1.Write();

  G4cout << "###### End of PndFtfDirect #####" << G4endl;

}
// ------------------------------------------------------------------------


void PndFtfDirect::InitZero()
{
  std::cout<<"PndFtfDirect::InitZero()  $$$$$$$$$$$$$$$$$$$$$$$$$  "<<std::endl;
  //-----------------------------------------------------------------------
  // ------- Initialisation
  fverbose  = 0;
  fsaverand = false;

  fnamePart = "proton";
  fionParticle = false;
  fPlab     = 100.*GeV;   // VU
  fenergy   = 100.*GeV;
  fsigmae   = 0.0;

  fionZ = 0;
  fionA = 0;
  ftargetA  = 0;
  fnameMat  = "G4_Al";

  fShen     = false;
  fnameGen  = "binary";

  fnevt     = 1000;
  fmodu     = 1000;

  ftheStep   = 0.01*micrometer;
  fmaterial = 0;

  fxsbgg = true;

  // Track
  faPosition->set(0.,0.,0.);
  faTime     = 0. ;
  faDirection->set(0.0,0.0,1.0);
  fnx = 0.0, fny = 0.0, fnz = 0.0;

  G4cout.setf( std::ios::scientific, std::ios::floatfield );

  // -------------------------------------------------------------------
  //--------- Materials definition ---------
  fmate = new Test30Material();
  G4NistManager::Instance()->SetVerbose(0);

  //--------- Particles definition ---------
  fphys = new Test30Physics();


  //////G4DecayPhysics decays;
  //////decays.ConstructParticle();

  //////fpartTable = G4ParticleTable::GetParticleTable();
  //////fpartTable->SetReadiness();

  //--------- Geometry definition

  //G4double dimX = 100.0*cm;
  //G4double dimY = 100.0*cm;
  //G4double dimZ = 100.0*cm;

  //G4Box* sFrame = new G4Box ("Box",dimX, dimY, dimZ);
  //G4LogicalVolume* lFrame = new G4LogicalVolume(sFrame,fmaterial,"Box",0,0,0);
  //G4PVPlacement* pFrame = new G4PVPlacement(0,G4ThreeVector(),"Box",
  //                                            lFrame,0,false,0);

  // construct pre-compound and deexcitation
  ftheDeExcitation = new G4ExcitationHandler();
  ftheEvaporation = new G4Evaporation();
  ftheDeExcitation->SetEvaporation(ftheEvaporation);
  fthePreCompound = new G4PreCompoundModel(ftheDeExcitation);
  fphys->SetPreCompound(fthePreCompound);
  fphys->SetDeExcitation(ftheDeExcitation);
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  fsigTot = 0;
  fsigEl  = 0;
  fsigIn  = 0;

  fnpart=0;
  factiveCnt=0;

}

void PndFtfDirect::Setup()
{
  std::cout<<"PndFtfDirect::Setup()  $$$$$$$$$$$$$$$$$$$$$$$$$  "<<std::endl;
  // Protect against unpurpousful multiple execution
  if (fDoSetup == false) return;
  fDoSetup=false;
  
  fpartTable = G4ParticleTable::GetParticleTable();
  
  //
  G4StateManager* g4State=G4StateManager::GetStateManager();
  if (! g4State->SetNewState(G4State_Init)) {
    G4cout << "error changing G4state"<< G4endl;;
  }
  //	  G4cout << "###### Start new run # " << run << "   for "
  //	  << fnevt << " events  #####" << G4endl;

  // -------- Target

  fmaterial = fmate->GetMaterial(fnameMat);
  if(!fmaterial) {
    G4cout << "Material <" << fnameMat
    << "> is not found out"
    << G4endl;
    exit(1);
  }
  const G4Element* elm = fmaterial->GetElement(0);

  // Uzhi 1.02.13 -----------------------
  G4ProductionCuts* cuts = new G4ProductionCuts();
  G4MaterialCutsCouple* couple = new G4MaterialCutsCouple(fmaterial,cuts);
  couple->SetIndex(0);
  G4ProductionCutsTable* pkt = G4ProductionCutsTable::GetProductionCutsTable();
  std::vector<G4double>* vc =
  const_cast<std::vector<G4double>*>(pkt->GetEnergyCutsVector(3));
  vc->push_back(0.0);
  // Uzhi 1.02.13 -----------------------

  // ------- Define target A
  G4int A = (G4int)(elm->GetN()+0.5);
  G4int Z = (G4int)(elm->GetZ()+0.5);

  // -------- Projectile
  part=0;
  if (!fionParticle) {
    part = (G4ParticleTable::GetParticleTable())->FindParticle(fnamePart);
    if(part) fenergy=std::sqrt(sqr(fPlab)+sqr(part->GetPDGMass()))-
      part->GetPDGMass();
  } else {
    part = (G4IonTable::GetIonTable())->GetIon(fionZ, fionA, 0.);
    if(part) fenergy=std::sqrt(sqr(fPlab)+sqr(940.*MeV))-940.*MeV;
  }
  if (!part) {
    G4cout << " Sorry, No definition for particle" <<fnamePart
    << " found" << G4endl;
  }

  if ( fionParticle ) {       // VU
    fenergy*=fionA;           // VU
  }

  if(dParticle) delete dParticle; // RK
		dParticle = new G4DynamicParticle(part,*faDirection,fenergy);

  // ------- Select model
  proc = fphys->GetProcess(fnameGen, part, fmaterial);      // Uzhi 1.02.13

  if(!proc) {
    G4cout << "For particle: " << part->GetParticleName()
    << " generator " << fnameGen << " is unavailable"<< G4endl;
    exit(1);
  }
  G4HadronicProcess* extraproc = 0;

  G4cout<<"targetA "<<ftargetA<<G4endl;
  if(ftargetA > 0) A = ftargetA;
  fphys->SetA(ftargetA);

  G4cout << "The particle:  " << part->GetParticleName() << G4endl;
  if(fverbose > 0) {
    G4cout << "The material:  " << fmaterial->GetName()
    << " Z= " << Z << " A= " << A<< G4endl;
    G4cout << "The step:      " << ftheStep/mm << " mm" << G4endl;
    G4cout << "The position:  " << (*faPosition)/mm << " mm" << G4endl;
    G4cout << "The direction: " << (*faDirection) << G4endl;
    G4cout << "The time:      " << faTime/ns << " ns" << G4endl;
  }

  G4cout <<G4endl<< "energy = " << fenergy/MeV << " MeV   "
  << " RMS(MeV)= " << fsigmae/MeV << " Plab "<<fPlab/GeV<<" GeV/c"<<G4endl;

  // -------- Normalisation

  G4double cross_sec = 0.0;
  G4double cross_secel = 0.0;
  G4double cross_inel = 0.0;

  if((part == anti_proton)   || (part == anti_neutron) ||
     (part == anti_deuteron) || (part == anti_triton)  ||
     (part == anti_He3)      || (part == anti_alpha)     )
  {
    G4VComponentCrossSection* cs =0;           // Uzhi
    cs=new G4ComponentAntiNuclNuclearXS();     // Uzhi

    cs->BuildPhysicsTable(*part);
    cross_sec = cs->GetTotalElementCrossSection(part, fenergy, Z, (G4double) A);
    cross_inel=cs->GetInelasticElementCrossSection(part, fenergy, Z, (G4double)A);
    cross_secel=cs->GetElasticElementCrossSection(part, fenergy, Z, (G4double)A);
  }
  else
  {
    G4VCrossSectionDataSet* cs = 0;

    if(fnameGen == "LElastic" || fnameGen == "BertiniElastic") {
      cs = new G4HadronElasticDataSet();
    } else if (fnameGen == "chargeex" ||
               fnameGen == "elastic" ||
               fnameGen == "HElastic" ||
               fnameGen == "DElastic") {

      if(part == proton || part == neutron) {
        if(fxsbgg) extraproc->AddDataSet(new G4BGGNucleonElasticXS(part));
      } else if(part == pip || part == pin) {
        if(fxsbgg) extraproc->AddDataSet(new G4BGGPionElasticXS(part));
      }

    } else if(part == proton && Z > 1 && fnameGen != "lepar") {
      if(fxsbgg) cs = new G4BGGNucleonInelasticXS(part);
      else      cs = new G4ProtonInelasticCrossSection();

    } else if(part == neutron && Z > 1 && fnameGen != "lepar") {
      if(fxsbgg) cs = new G4BGGNucleonInelasticXS(part);
      else      cs = new G4NeutronInelasticCrossSection();

    } else if((part == pin || part == pip) && Z > 1 && fnameGen != "lepar") {
      if(fxsbgg) cs = new G4BGGPionInelasticXS(part);
      else cs = new G4PiNuclearCrossSection();

    } else if( fionParticle && !fShen ) { // Uzhi
      G4double zz = G4double(Z);
      G4double aa = G4double(A);
      if(part == deu || part == tri ||part == he3 ||part == alp)
      {
        cs = new G4TripathiLightCrossSection();
        if(cs->IsIsoApplicable(dParticle,Z,A)) {
          G4cout << "Using Tripathi Light Cross section for Ions" << G4endl;
        } else {
          delete cs;
          cs = 0;
        }
      } else {
        if(!cs) {
          cs = new G4TripathiCrossSection();
          if(cs->IsIsoApplicable(dParticle,Z,A)) {
            G4cout << "Using Tripathi Cross section for Ions" << G4endl;
          } else {
            delete cs;
            cs = 0;
          }
        }
      }

      if(!cs) {
        cs = new G4IonsShenCrossSection();
        if(cs->IsElementApplicable(dParticle,Z,fmaterial)) {
          G4cout << "Using Shen Cross section for Ions" << G4endl;
        } else {
          G4cout << "ERROR: no cross section for ion Z= "
          << zz << " A= " << aa << G4endl;
          exit(1);
        }
      }
    } else {
      cs = new G4HadronInelasticDataSet();
      G4cout<<"Using G4HadronInelasticDataSet()"<<G4endl;
    }

    if(extraproc) {
      extraproc->PreparePhysicsTable(*part);
      extraproc->BuildPhysicsTable(*part);
      cross_sec = extraproc->GetMicroscopicCrossSection(dParticle,
                                                        elm, 0);     // Uzhi 0.0);

    } else if(cs) {
      cs->BuildPhysicsTable(*part);
      cross_sec = cs->GetCrossSection(dParticle, elm);
      G4cout<<"Try 1 cross_secel "<<cross_secel/millibarn<<G4endl;
    } else {
      cross_sec = (G4HadronCrossSections::Instance())->
      GetInelasticCrossSection(dParticle, Z,A);
      G4cout<<"Try 2 cross_sec "<<cross_sec/millibarn<<G4endl;
    }

    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    cs->BuildPhysicsTable(*part);
  } // -----------------------------------------

  // -------- Track
  // 	  G4Track* gTrack;
  gTrack = new G4Track(dParticle,faTime,*faPosition);
  G4TouchableHandle fpTouchable(new G4TouchableHistory());
  gTrack->SetTouchableHandle(fpTouchable);

  // -------- Step

  // 	  G4Step* step;
  step = new G4Step();
  step->SetTrack(gTrack);
  gTrack->SetStep(step);

  G4StepPoint *aPoint, *bPoint;
  aPoint = new G4StepPoint();
  aPoint->SetPosition(*faPosition);
  aPoint->SetMaterial(fmaterial);
  G4double safety = 10000.*cm;
  aPoint->SetSafety(safety);
  step->SetPreStepPoint(aPoint);

  bPoint = aPoint;
  G4ThreeVector bPosition = *faDirection*ftheStep;
  bPosition += *faPosition;
  bPoint->SetPosition(bPosition);
  step->SetPostStepPoint(bPoint);
  step->SetStepLength(ftheStep);

  //-------------------------------------------------------
  if(!G4StateManager::GetStateManager()->SetNewState(G4State_Idle))
    G4cout << "G4StateManager PROBLEM! " << G4endl;
  G4RotationMatrix* rot = new G4RotationMatrix();
  G4double phi0 = faDirection->phi();
  G4double theta0 = faDirection->theta();
  rot->rotateZ(-phi0);
  rot->rotateY(-theta0);

  if(fverbose > 0) {
    G4cout << "Test rotation= " << (*rot)*(*faDirection) << G4endl;
  }
  G4Timer* timer = new G4Timer();
  timer->Start();

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  G4cout << "cross(mb)in= " << cross_sec*1000./barn << G4endl
  << "cross(mb)el= " << cross_secel*1000./barn<<G4endl<<G4endl;

  cross_inel=cross_sec-cross_secel; // +++++++++++++++++++++++++

  cross_sec/=millibarn;   // Inel Cross section in mb
  cross_secel/=millibarn; // Elas Cross section in mb
  cross_inel/=millibarn;  // Inel Cross section in mb

  G4cout<<"Element A Z N: "<<A<<" "<<Z<<" "<<A-Z<<G4endl;
  G4cout<<"Proposed Xs (mb): Tot El In: "
  <<cross_sec<<" "<<cross_secel<<" "<<cross_inel<<G4endl;

  //---------------------------------------------------------------------------
  // Kossov cross sections      ---------------------------
  G4double chipsTot, chipsEl, chipsIn;

  static G4ChipsComponentXS* _instance = new G4ChipsComponentXS();
  G4ChipsComponentXS* CHIPSxsManager = _instance;

  G4bool CHIPapplic=true;                          //false;   Uzhi 29.01.13
  if(CHIPapplic)
  {
    chipsTot=CHIPSxsManager->GetTotalElementCrossSection(part,fenergy,Z,A-Z);
    chipsEl =CHIPSxsManager->GetElasticElementCrossSection(part,fenergy,Z,A-Z);
    chipsIn =CHIPSxsManager->GetInelasticElementCrossSection(part,fenergy,Z,A-Z);
    chipsTot/=millibarn; chipsEl/=millibarn; chipsIn/=millibarn;

    G4cout<<"CHIPS cross sections are used:----------------------"<<G4endl<<
    "Plab          Total        Elastic      Inelastic"   <<G4endl;
    G4cout<<" "<<fPlab/GeV<<" "<< chipsTot<<" "<<chipsEl<<" "<<chipsIn <<G4endl<<G4endl;

    fsigTot=chipsTot; fsigEl=chipsEl; fsigIn=chipsIn;
  } else
  {
    fsigTot = cross_sec;
    fsigEl  = cross_secel;
    fsigIn  = cross_inel;

    G4cout<<"Proposed Xs (mb) are used: Tot El In: "
    <<cross_sec<<" "<<cross_secel<<" "<<cross_inel<<G4endl;
  }

}

// -----   Public method ReadEvent   --------------------------------------
Bool_t PndFtfDirect::ReadEvent(FairPrimaryGenerator* primGen)
{
  Setup();

  // capsulate to get each event somthing sensible, even if it "failed".
  // I.e. when elastics are produced  by the generator and shall not be stored.
  int tryno=0;
  while (!ProcessEvent(primGen))
  {
    tryno++;
    if(fverbose>=1) cout<<"ProcessEvent() try number "<<tryno<<" failed, try again..."<<endl;
  }
  return kTRUE;
}

Bool_t PndFtfDirect::ProcessEvent(FairPrimaryGenerator* primGen)
{
  G4int Ntotal=fnevt;
  G4int Nfault=0;
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //-------------------------------------------------------

  const G4DynamicParticle* sec = 0;
  G4ParticleDefinition* pd;
  G4ThreeVector  mom;
  G4LorentzVector labv, fm;
  G4double e, px, py, pz,  theta; //pt, //[R.K. 01/2017] unused variable?
  G4VParticleChange* aChange = 0;
  TLorentzVector Mom;
  TLorentzVector V(0,0,0,0);

  //  G4double E=fenergy+part->GetPDGMass();                                  // Elab Proj
		//  G4double SqrtS=std::sqrt(sqr(part->GetPDGMass())+sqr(938.)+2.*938.*E); // per  Proj+N
    //  G4double Ycms=0.5*std::log((E+fPlab)/(E-fPlab));                         //      Proj+N

		// -------- Event loop
    // 	  G4cout<<"Events start "<<fnevt<<G4endl;
    //  G4int Ninelast=fnevt;                              // Uzhi
    //=================================================================
    // 	  for (G4int iter=0; iter<fnevt; ++iter) {
    // 		//=================================================================
    // 		if(fverbose > 0) G4cout<<"Start events loop***********************"<<G4endl;

  // 		if(fverbose>=1 || iter == fmodu*(iter/fmodu)) {
  // 		  G4cout << "### " << iter << "-th event start " <<fPlab/GeV<<G4endl;
  // 		}

  if(fsaverand) {fdefaultEngine->saveStatus("initial.conf");}

  G4double e0 = fenergy;
  do {
    if(fsigmae > 0.0) e0 = G4RandGauss::shoot(fenergy,fsigmae);
  } while (e0 < 0.0);

  dParticle->SetKineticEnergy(e0);

  gTrack->SetStep(step);
  gTrack->SetKineticEnergy(e0);
  G4double amass = fphys->GetNucleusMass();
  // note: check of 4-momentum balance for CHIPS is not guranteed due to
  // unknown isotope
  aChange = proc->PostStepDoIt(*gTrack,*step);

  G4double mass = part->GetPDGMass();

  if ( fionParticle )
  {
    e0/=fionA; G4double mass_N=938.*MeV;                              // Init 4-mom
    labv = G4LorentzVector(0.0, 0.0, std::sqrt(e0*(e0 + 2.*mass_N)), //   NN
                           e0 + mass_N +  mass_N);
  } else
  {
    labv = G4LorentzVector(0.0, 0.0, std::sqrt(e0*(e0 + 2.*mass)),   //   hA
                           e0 + mass + amass);
  }

  G4ThreeVector bst = labv.boostVector();          // To CMS NN in AA or hA
                                                   //------------
  G4LorentzVector labNN(0.0, 0.0, std::sqrt(e0*(e0 + 2.*mass)),e0 + mass + amass);
  G4ThreeVector boostNN = labNN.boostVector();
  //------------

  // take into account local energy deposit
  G4double de = aChange->GetLocalEnergyDeposit();
  G4LorentzVector dee = G4LorentzVector(0.0, 0.0, 0.0, de);
  labv -= dee;

  G4int n = aChange->GetNumberOfSecondaries();     // Multiplicity of prod. part.
  fnpart = n;
  //      fEvt->Clear();
  Int_t cnt = 0;

  if(fverbose>=1) G4cout<<" Uzhi ------------ N prod. part "<<n<<G4endl;
  //++++++++++++++++ Variables for each event +++++++++++++++++++++++++++++
  if((fverbose > 0) && (n < 2)) {G4cout<<"Multiplicity of produced < 2!!!"<<G4endl;}
  if(n < 2) {Nfault++; Ntotal--;}

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  bool flag_good=true;
  if(fNoElastics&&(n==2)) {
	const G4DynamicParticle *sec1 = aChange->GetSecondary(0)->GetDynamicParticle();
	const G4DynamicParticle *sec2 = aChange->GetSecondary(1)->GetDynamicParticle();
    G4ParticleDefinition *pd1  = sec1->GetDefinition();
    G4ParticleDefinition *pd2  = sec2->GetDefinition();
    int id1 = pd1->GetPDGEncoding();
    int id2 = pd2->GetPDGEncoding();
	// now we know it's fully elastic!
    if(abs(id1)==2212 && abs(id2)==2212)
	{
		flag_good=false;
		delete aChange->GetSecondary(0);
		delete aChange->GetSecondary(1);
	}
  } else { // now inelastic
    for(G4int i=0; i<n; ++i)              // Loop over produced particles
    {
      sec = aChange->GetSecondary(i)->GetDynamicParticle();
      pd  = sec->GetDefinition();
      G4String pname=pd->GetParticleName();

      if(fverbose>=1) G4cout<<" Part  "<<i<<" "<<pname
        <<" "<<sec->Get4Momentum()/GeV
        <<sec->Get4Momentum().mag()/GeV<<G4endl;

      fm  = sec->Get4Momentum();

      mom = sec->GetMomentum();
      //      G4double mas = pd->GetPDGMass();
      //  G4double p = mom.mag();

      labv -= fm;   // For checking energy-momentum conservation

      // electron can come only from internal conversion
      // its mass should be added to initial state
      if(pd == electron) {

        labv += G4LorentzVector(0.0,0.0,0.0,electron_mass_c2);
      }

      px = mom.x()/GeV;
      py = mom.y()/GeV;
      pz = mom.z()/GeV;
      //pt = std::sqrt(px*px +py*py)/GeV; //[R.K. 01/2017] unused variable?
      e  = fm.e()/GeV; // - m;
      theta = mom.theta();
      if(std::abs(pd->GetBaryonNumber()) < 2)
      {
        int id = pd->GetPDGEncoding();
        Mom.SetPxPyPzE(px,py,pz,e);
        //      TParticle  fparticle(id,1,0,0,0,0,Mom,V);
        //      new((*fEvt)[cnt++]) TParticle(fparticle);
        // add track
        printf("- I -: new particle with: %i, %f, %f, %f ...\n", id, px, py, pz);
        primGen->AddTrack(id, px, py, pz, 0.,0.,0.); //fX, fY, fZ);

      }
      theta=theta*180./pi;
      fm.boost(-bst);

    //        G4double costcm = std::cos(fm.theta());
      de += e;

    //  delete sec;
      delete aChange->GetSecondary(i);

    } //     end of the loop on particles
  }
  if(flag_good){ // inelastic!
    if(fverbose > 0)
      G4cout << "We have an inelastic event." << G4endl;
    factiveCnt = cnt;
    //fTree->Fill();
  }
  else{
    if(fverbose > 0)
      G4cout << "We have skipped an elastic event." << G4endl;
    Nfault++;
    Ntotal--;
    //    iter--; // elastic!
  }

  if(fverbose > 0)
    G4cout << "Energy/Momentum balance= " << labv << G4endl;

  aChange->Clear();

  if(fverbose > 0)
  {
    G4cout << "End event =====================================" <<fPlab<< G4endl; // Uzhi
    G4int Uzhi_i;                                                        // Uzhi
    G4cin >> Uzhi_i;                                                     // Uzhi
  }
  if(!flag_good) return kFALSE;
  return kTRUE;

}
// End of the event loop ------------------------------------
//		timer->Stop();
//		G4cout << "  "  << *timer << G4endl;
//		delete timer;

//		if(fverbose > 0) {
//		  G4cout << "###### End of run # " << run << "     ######" << G4endl;
//		}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ Uzhi

//} while(end);
//}  // End of job ----------------------------------------------
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//	  return kTRUE;

//	}
// ------------------------------------------------------------------------

//ClassImp(PndFtfDirect)


void PndFtfDirect::parseConfigfile()
{

  G4bool end = true;
  G4String line, line1;

  do {
    if( fin->eof() ) {
      end = false;
      break;
    }
    (*fin) >> line;
    if(fverbose > 0) G4cout << "Next line " << line << G4endl;
    if(line == "#particle") {
      (*fin) >> fnamePart;
      fionParticle= false;                      // Uzhi 1.02.13
    } else if(line == "#ion") {
      fionParticle= true;
      fnamePart="GenericIon";
      (*fin) >> fionA >> fionZ;
    } else if(line == "#Plab(GeV/c)") {
      (*fin) >> fPlab;
      fPlab *= GeV;
    } else if(line == "#sigmae(GeV)") {
      (*fin) >> fsigmae;
      fsigmae *= GeV;
    } else if(line == "#events") {
      (*fin) >> fnevt;
    } else if(line == "#step(mm)") {
      (*fin) >> ftheStep;
      ftheStep *= mm;
    } else if(line == "#print") {
      (*fin) >> fmodu;
    } else if(line == "#material") {
      (*fin) >> fnameMat;
    } else if(line == "#targetA") {
      (*fin) >> ftargetA;
    } else if(line == "#Shen") {
      fShen = true;
    } else if(line == "#noElastics") {
      fNoElastics = true;
    } else if(line == "#generator") {
      fnameGen = "";
      (*fin) >> fnameGen;
      if (fnameGen == "") {
        G4cout << "Generator name is empty! " << G4endl;
        continue;
      }

      if(fnameGen == "elastic" || fnameGen == "HElastic" ||
         fnameGen == "DElastic") { fNoElastics=false; break; }
      char* c = getenv(fnameGen);
      if(!c) {
        G4cout << "Generator <" << fnameGen << "> is not included in the "
        << " list SetModels.csh, so is ignored!"
        << G4endl;
        G4cout << "Use #run command to overcome this limitation " << G4endl;
        continue;
      }
      G4String ss(c);
      if(ss=="1") { break; }
    } else if(line == "#run") {
      break;
    } else if(line == "#verbose") {
      (*fin) >> fverbose;
      G4NistManager::Instance()->SetVerbose(fverbose);
    } else if(line == "#position(mm)") {
      (*fin) >> fnx >> fny >> fnz;
      faPosition->set(fnx*mm, fny*mm, fnz*mm);
    } else if(line == "#direction") {
      (*fin) >> fnx >> fny >> fnz;
      if(fnx*fnx + fny*fny + fnz*fnz > 0.0) {
        faDirection->set(fnx, fny, fnz);
        *faDirection = faDirection->unit();
      }
    } else if(line == "#time(ns)") {
      (*fin) >> faTime;
      faTime *= ns;
    } else if(line == "#saverand") {
      fsaverand = true;
    } else if(line == "#initrand") {
      G4String sss("");
      (*fin) >> sss;
      fdefaultEngine->restoreStatus(sss);
      if(fverbose>0) G4cout << "Random Engine restored from file <"
        << sss << ">" << G4endl;
      CLHEP::HepRandom::showEngineStatus();
    } else if(line == "#xs_ghad") {
      fxsbgg = false;
    } else if(line == "#exit") {
      end = false;
      break;
    } else if(line == "#GEMEvaporation") {
      G4cout<<"### GEM evaporation is set"<<G4endl;
      ftheEvaporation->SetGEMChannel();
    } else if(line == "#DefGEMEvaporation") {
      G4cout<<"### Combined Default+GEM evaporation is set"<<G4endl;
      ftheEvaporation->SetCombinedChannel();
    } else if(line == "#Evaporation") {
      G4cout<<"### Default evaporation is set"<<G4endl;
      ftheEvaporation->SetDefaultChannel();
    } else if(line == "#XSoption") {
      G4int OPTxs;
      (*fin)>>OPTxs;
      if (OPTxs< 0 || OPTxs >4  ){
        G4cout << "### WArning: BAD CROSS SECTION OPTION for PreCompound model "
        << OPTxs << " ignored" << G4endl;
      } else {

        fthePreCompound->SetOPTxs(OPTxs);
        ftheDeExcitation->SetOPTxs(OPTxs);
        G4cout<<" Option for inverse cross sections : OPTxs="<<OPTxs<<G4endl;
      }
    } else if(line == "#UseSuperImposedCoulombBarrier") {
      G4cout<<" Coulomb Barrier has been overimposed to ALL inverse cross sections"
      <<G4endl;
      fthePreCompound->UseSICB();
      ftheDeExcitation->UseSICB();

    } else if(line == "#UseNeverGoBack") {
      G4cout<<" Never Go Back hypothesis has been assumed at preequilibrium"
      <<G4endl;
      fthePreCompound->UseNGB();
    } else if(line == "#UseSoftCutOff") {
      G4cout<<" Soft Cut Off  hypothesis has been assumed at preequilibrium"
      <<G4endl;
      fthePreCompound->UseSCO();

    } else if(line == "#UseCEMTransitions") {
      G4cout<<" Transition probabilities at preequilibrium based on CEM model"
      <<G4endl;
      fthePreCompound->UseCEMtr();

    } else if(line == "#MFenergy(MeV)") {
      G4double tmin;
      (*fin) >> tmin;
      G4cout<<" Min energy for multi-fragmentation is set to " << tmin
      << " MeV" << G4endl;
      ftheDeExcitation->SetMinEForMultiFrag(tmin*MeV);
    } else if(line == "#FermiBreakUp") {
      G4cout<<"### Max A and Z energy for fermiBreakUp  are set to 17 and 9"
      << G4endl;
      ftheDeExcitation->SetMaxAandZForFermiBreakUp(17,9);
    }
  } while(end);

  //  if(!end) break;

}
