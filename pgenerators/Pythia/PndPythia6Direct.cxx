#include <math.h>
#include "TROOT.h"
#include "FairPrimaryGenerator.h"
//#include "FairGenerator.h"
#include "TClonesArray.h"
#include "TDatabasePDG.h"

#include "PndPythia6Direct.h"


// -----   Default constructor   -------------------------------------------
PndPythia6Direct::PndPythia6Direct()
{
  fMom = 15.;
  fStoreTree=false;
  fPythia=TPythia6::Instance();
  SetPandaDefaults();
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void PndPythia6Direct::SetPandaDefaults() {
  std::cout<<"PndPythia6Direct::SetPandaDefaults()"<<std::endl;
  // Those are the FORTRAN settings. Let's access them via TPythia6
  //c----------  Choice of the nessesary process --------------------
  fPythia->SetMSEL(0);       // full User Access
  fPythia->SetMSUB(1,1);     // qQ-->A*/Z0*
  //c-------------------  Nessesary limits for PANDA ENERGY region ---------
  fPythia->SetPARP(2,0.5);   // Lowest inv. Mass (D=10.GeV)
  fPythia->SetPARP(111,0.1); // Minimum invar. mass of the remnant hadronic system (D=2.GeV)
  fPythia->SetCKIN(1,1.);    // limits for shat
  //fPythia->SetCKIN(2,-1.);   // no upper limit for S-HAT
  fPythia->SetCKIN(5,0.1);   // low cut limits for PT_-range
  fPythia->SetCKIN(6,0.1);   // low cut limits for intermediate mass
}

// -----   Default constructor   -------------------------------------------
Bool_t PndPythia6Direct::Init()
{
  std::cout<<"PndPythia6Direct::Init(): Beam Momentum "<<fMom<<std::endl;

  fPythia->Initialize("FIXT", "pbar", "p", fMom);
  fParticleList = (TClonesArray*)fPythia->GetListOfParticles();

  return kTRUE;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndPythia6Direct::~PndPythia6Direct()
{
  if(fPythia!=NULL) delete fPythia;
}
// -------------------------------------------------------------------------

// -----   Passing the event   ---------------------------------------------
Bool_t PndPythia6Direct::ReadEvent(FairPrimaryGenerator* primGen)
{
  static int evtn=0;
  if(fVerbose>0) std::cout<<"PndPythia6Direct::ReadEvent() No. "<<evtn<<std::endl;
  fPythia->GenerateEvent();
  if(fVerbose>2) std::cout<<"PndPythia6Direct::ReadEvent() Generation done."<<std::endl;
  double fX, fY, fZ, fT;
  double Px,Py,Pz, fE; // ,Pm[1000],Wh[1000];
  int Id,nFD,nLD,nMO,status;
  bool dotracking;
  //fParticleList=(TClonesArray*)fPythia->ImportParticles();
  int npart=fParticleList->GetEntriesFast();
  if(fVerbose>0) std::cout<<"PndPythia6Direct::ReadEvent() We have "<<npart<<" tracks."<<std::endl;
  for(int i=0; i<npart; ++i)
  {
    if(fVerbose>4) std::cout<<"PndPythia6Direct::ReadEvent() particle No. "<<i<<std::endl;
    fParticle=(TMCParticle*)fParticleList->At(i);
    if(fVerbose>4) std::cout<<"PndPythia6Direct::ReadEvent() particle pointer "<<fParticle<<std::endl;
    if(fParticle==NULL) continue;
    // add track
    nFD=fParticle->GetFirstChild();
    nLD=fParticle->GetLastChild();
    nMO=fParticle->GetParent();
    status=fParticle->GetKS();
      //Id=TDatabasePDG::Instance()->GetParticle(fParticle->GetName())->PdgCode();
    Id=fParticle->GetKF(); // "Flavour code" is pdg code
    if(fVerbose>4) {
      std::cout<<"PndPythia6Direct::ReadEvent() Particle:"
               //<<" pdg="<<TDatabasePDG::Instance()->GetParticle(fParticle->GetName())->PdgCode()
               <<" name=\""<<fParticle->GetName()<<"\""
               <<" nFD= "<<nFD
               <<" nLD= "<<nLD
               <<" nMO= "<<nMO
               <<" pdg= "<<Id
               <<" status= "<<status
               <<"\n                                       "
               <<" Time()*1000="<<fParticle->GetTime()*1000
               <<" Vx()*0.1= "<<fParticle->GetVx()*0.1
               <<" Vy()*0.1= "<<fParticle->GetVy()*0.1
               <<" Vz()*0.1= "<<fParticle->GetVz()*0.1
               <<"\n                                       "
               <<" Energy()= "<<fParticle->GetEnergy()
               <<" Px()= "<<fParticle->GetPx()
               <<" Py()= "<<fParticle->GetPy()
               <<" Pz()= "<<fParticle->GetPz()
               <<std::endl;
    }

    if(status==1 || fStoreTree )         //fStoreTree && status==11 && nFD!=nLD)
    {
      fT=fParticle->GetTime()*1000; //[mm/c]->s  with c in [m/s]           //   /(1000*TMath::C());  //mm - > s conversion
      fX=fParticle->GetVx()*0.1; // mm -> cm conversion
      fY=fParticle->GetVy()*0.1; // mm -> cm conversion
      fZ=fParticle->GetVz()*0.1; // mm -> cm conversion
      fE=fParticle->GetEnergy();
      Px=fParticle->GetPx();
      Py=fParticle->GetPy();
      Pz=fParticle->GetPz();
      if(fVerbose>0&&(evtn<10||(evtn%100)==0)) printf("- I -: new particle %d at: %f, %f, %f (%f)-> %f %f %f (%f) ID %d ##Daughters %d %d Mother %d\n",
            i,fX, fY, fZ, fT,Px, Py, Pz, fE, Id, nFD, nLD, nMO);
      if(fStoreTree) {
        // here we write all particles to the stack, but do processing only for final state particles
        dotracking = (status==1);
        primGen->AddTrack(Id, Px, Py, Pz, fX, fY, fZ, nMO,dotracking,fE,fT);
      } else {
        primGen->AddTrack(Id, Px, Py, Pz, fX, fY, fZ,-1,true,fE,fT);// set mother id to default: -1, do tracking: true
      }
    }
  }
  evtn++;
  return kTRUE;
}

// -------------------------------------------------------------------------
void PndPythia6Direct::Print() {
  //fPythia.settings.listAll();
}
// -------------------------------------------------------------------------

ClassImp(PndPythia6Direct)

