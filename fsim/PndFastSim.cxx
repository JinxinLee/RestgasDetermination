/////////////////////////////////////////////////////////////
//
//  FastSim
//
//  Reader for McHit
////
///////////////////////////////////////////////////////////////

//C++ class headers
#include <string>
#include <fstream>

//CBM class headers
#include "FairRootManager.h"
//#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndMCTrack.h"
#include "PndStack.h"
#include "FairRun.h"

#include "PndFastSim.h"

//Rho includes
#include "RhoBase/RhoCandidate.h"
//#include "RhoBase/TSimpleVertex.h"
//#include "RhoBase/TRho.h"
#include "PndPidCandidate.h"
#include "PidData/PndPidCandidate.h"
#include "PidData/PndPidProbability.h"
#include "PndEventInfo.h"
#include "RhoBase/RhoCandList.h"
//#include "RhoTools/TEventShape.h"
#include "RhoBase/RhoFactory.h"

//ROOT class headers
#include "TClonesArray.h"
#include "TVector3.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TParticle.h"
#include "TRandom3.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TVirtualMC.h"
#include "TF1.h"

//Fast Sim class headers
#include "PndFsmTrack.h"
#include "PndFsmResponse.h"
#include "PndFsmAbsDet.h"
#include "PndFsmDetFactory.h"
#include "PndFsmRandom.h"

using std::cout;
using std::endl;
using std::string;
using std::ifstream;


// -----   Default constructor   -------------------------------------------
PndFastSim::PndFastSim() :
FairTask("Panda Fast Simulation") {
  //  fCandidates = new TClonesArray("TParticle");
  fRand=PndFsmRandom::Instance();
  fDetFac = new PndFsmDetFactory;
  fAddedDets=" ";
  fVb=0;
  fGenSplitOffs=false;
  fPropagate=false;
  fdbPdg = TDatabasePDG::Instance();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndFastSim::~PndFastSim() {
  FairRootManager *fManager =FairRootManager::Instance();
  fManager->Write();

  //if (fChargedCandidates) {fChargedCandidates->Delete(); delete fChargedCandidates;}
  //if (fNeutralCandidates) {fNeutralCandidates->Delete(); delete fNeutralCandidates;}
  if (fMcCandidates) {fMcCandidates->Delete(); delete fMcCandidates;}
  //if (fMicroCandidates) {fMicroCandidates->Delete(); delete fMicroCandidates;}
  if (fPidChargedCand) {fPidChargedCand->Delete(); delete fPidChargedCand;}
  if (fPidNeutralCand) {fPidNeutralCand->Delete(); delete fPidNeutralCand;}

  if (fPidChargedProb) {fPidChargedProb->Delete(); delete fPidChargedProb;}
  if (fPidNeutralProb) {fPidNeutralProb->Delete(); delete fPidNeutralProb;}

  if (fEventInfo) {fEventInfo->Delete(); delete fEventInfo;}

  if (fDetFac) delete fDetFac;
  if (fRand) delete fRand;
}
// -------------------------------------------------------------------------


void PndFastSim::Register() {
  //---

  fMcCandidates = new TClonesArray("RhoCandidate");
  FairRootManager::Instance()->Register("PndMcTracks","FastSim", fMcCandidates, kTRUE);

  //fPndCandidates = new TClonesArray("RhoCandidate");
  //FairRootManager::Instance()->Register("PndCandidates","FastSim", fPndCandidates, kTRUE);

  //fChargedCandidates = new TClonesArray("RhoCandidate");
  //FairRootManager::Instance()->Register("PndChargedCandidates","FastSim", fChargedCandidates, kTRUE);

  //fNeutralCandidates = new TClonesArray("RhoCandidate");
  //FairRootManager::Instance()->Register("PndNeutralCandidates","FastSim", fNeutralCandidates, kTRUE);

  //fMicroCandidates = new TClonesArray("PndPidCandidate");
  //FairRootManager::Instance()->Register("PndPidCandidates","FastSim", fMicroCandidates, kTRUE);

  fPidChargedCand = new TClonesArray("PndPidCandidate");
  FairRootManager::Instance()->Register("PidChargedCand","FastSim", fPidChargedCand, kTRUE);

  fPidNeutralCand = new TClonesArray("PndPidCandidate");
  FairRootManager::Instance()->Register("PidNeutralCand","FastSim", fPidNeutralCand, kTRUE);

  fPidChargedProb = new TClonesArray("PndPidProbability");
  FairRootManager::Instance()->Register("PidChargedProbability","FastSim", fPidChargedProb, kTRUE);

  fPidNeutralProb = new TClonesArray("PndPidProbability");
  FairRootManager::Instance()->Register("PidNeutralProbability","FastSim", fPidNeutralProb, kTRUE);

  fEventInfo = new TClonesArray("PndEventInfo");
  FairRootManager::Instance()->Register("PndEventSummary","FastSim", fEventInfo, kTRUE);

  for (FsmAbsDetList::iterator iter=fDetList.begin();iter!=fDetList.end(); iter++)
  {
    PndFsmAbsDet *det=*iter;
    if(!det->doesPid()) continue;
    TString detname = det->detName();
    TString arrayname=detname;
    arrayname+="Probability";
    TClonesArray* tmparray = new TClonesArray("PndPidProbability");
    FairRootManager::Instance()->Register(arrayname.Data(),"FastSim", tmparray, kTRUE);
    fPidArrayList[detname]=tmparray;
    //std::cout<<"registered pid TCA with name "<<detname.Data()<<std::endl;
  }
}



// -----   Public method Init   --------------------------------------------
InitStatus PndFastSim::Init() {

  if (fVb>3) cout << " Inside the Init function****" << endl;

  //FairDetector::Initialize();
  //FairRun* sim = FairRun::Instance();
  //FairRuntimeDb* rtdb=sim->GetRuntimeDb();

  // Get RootManager
  /* FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
  cout << "-E- PndFastSim::Init: "
  << "RootManager not instantiated!" << endl;
  return kFATAL;
}
*/
  Register();
  if (fVb)
  {
    cout <<"\n\n******   DETECTOR SETUP SUMMARY    ***************\n\n";
    for (FsmAbsDetList::iterator iter=fDetList.begin();iter!=fDetList.end(); iter++)
    {
      cout<<endl<<"----------------------------"<<endl;
      PndFsmAbsDet *det=*iter;
      det->print(std::cout);
    }
    cout <<"\n\n******   DETECTOR SETUP SUMMARY    ***************\n\n";
  }

  evtcnt=0;

  // for the time being, this is the matrix of correlation
  // coefficients, it is later scaled by the track errors
  //d0                 phi0                omega               z0                  dandip
  fRho(0,0)= 1;        fRho(0,1)=-0.9243;  fRho(0,2)= 0.3087;  fRho(0,3)=-0.01743; fRho(0,4)= 0.009165; // d0
  fRho(1,0)=-0.9243;   fRho(1,1)= 1;       fRho(1,2)=-0.4189;  fRho(1,3)=-0.00552; fRho(1,4)=-0.00873;  // phi0
  fRho(2,0)= 0.3087;   fRho(2,1)=-0.4189;  fRho(2,2)= 1;       fRho(2,3)= 0.02094; fRho(2,4)=-0.01742;  // omega
  fRho(3,0)=-0.01743;  fRho(3,1)=-0.00552; fRho(3,2)= 0.02094; fRho(3,3)= 1;       fRho(3,4)=-0.9327;   // z0
  fRho(4,0)= 0.009165; fRho(4,1)=-0.00873; fRho(4,2)=-0.01742; fRho(4,3)=-0.9327;  fRho(4,4)= 1;        // tandip
  // and this is the square root of fRho
  // needed to correlate the track params
  //d0                 phi0                 omega                z0                  dandip
  fEta(0,0)= 1;                                                                                       // d0
  fEta(1,0)=-0.9243;   fEta(1,1)= 0.3817;                                                             // phi0
  fEta(2,0)= 0.3087;   fEta(2,1)=-0.35;     fEta(2,2)= 0.8844;                                        // omega
  fEta(3,0)=-0.01743;  fEta(3,1)=-0.05667;  fEta(3,2)= 0.007335; fEta(3,3)= 0.9982;                   // z0
  fEta(4,0)= 0.009165; fEta(4,1)=-6.781e-4; fEta(4,2)=-0.02316;  fEta(4,3)=-0.9341; fEta(4,4)=0.3562; // tandip

  // Create and register output array
  cout << "-I- PndFastSim: Intialization successfull" << endl;
  return kSUCCESS;
}

void PndFastSim::SetParContainers() {

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");


}
// -------------------------------------------------------------------------

void PndFastSim::SetSeed(unsigned int seed) {
  fRand->SetSeed(seed);
}

// -----   Enable split off parametrization   ------------------------------

bool PndFastSim::EnableSplitoffs(std::string fname)
{
  if (fname=="")
  {
    cout <<" -W-  (PndFastSim::EnableSplitoffs) - no filename given; no split offs will be produced."<<endl;
    return false;
  }


  /*
  prepare the split off parametrization
  expected in the parameter file:
  - four blocks (mom, phi, tht, multiplicity) with the parametrization structured like
  * <n_i>   <lowlimit>   <uplimit>   <model-string(in ROOT TF1 format)>
  * 5 lines of parameter sets with info for pid=11,13,211,321,2212
  <pid> <p1> <p2> ... <p_n_i>

  for the moment momentum independent
    models:
    momentum     : expo(0)+expo(2) = 4 params
    tht+phi      : gaus(0)+gaus(3) = 6 params
    multiplicity : expo(0)+gaus(2) = 5 params
    */

  ifstream pars(fname.data());

  if ( (pars.rdstate() & ifstream::failbit ) != 0 )
  {
    cout << " -W-  (PndFastSim::EnableSplitoffs) -  Error opening '"<<fname<<"'; no split offs will be produced."<<endl;
    return false;
  }


  int i,j,k;
  char tmp[50];

  string model[4];
  int numpars[4];
  double rangeup[4],rangelow[4];
  double curpar;


  //loop over blocks
  for (i=0;i<4;i++)
  {
    pars>>model[i]>>numpars[i]>>rangelow[i]>>rangeup[i];
    if (fVb) cout <<" -I-  (PndFastSim::EnableSplitoffs)  split off model #"<<i<<": '"<<model[i]<<"' with "<<numpars[i]<<" params."<<endl;

    //loop over pid
    for (j=0;j<5;j++)
    {
      sprintf(tmp,"f%d%d",j,i);
      fspo[j][i]=new TF1(tmp,model[i].data());

      //loop over params
      pars>>curpar;   // read the dummy PID value
      for (k=0;k<numpars[i];k++)
      {
        pars>>curpar;
        fspo[j][i]->SetParameter(k,curpar);
      }

      fspo[j][i]->SetRange(rangelow[i],rangeup[i]);
      if (fVb) fspo[j][i]->Print();
    }
  }

  if (fVb) cout <<" -I-  (PndFastSim::EnableSplitoffs) - Successfully read "<<fname<<endl;

  pars.close();

  fGenSplitOffs=true;

  return true;
}

// -------------------------------------------------------------------------

// -----   Enable track propagation   --------------------------------------

void PndFastSim::EnablePropagation(bool propagate, bool tostartvtx, bool usecovmatrix, double tolerance) {
  fPropagate=propagate;
  fToStartVtx=tostartvtx;
  fUseCovMatrix=usecovmatrix;
  fTolerance=tolerance;
}

// -------------------------------------------------------------------------

// -----   Add a detector to the setup   -----------------------------------
bool PndFastSim::AddDetector(std::string name, std::string params)
{
  if (name =="")
  {
    cout <<" -W-  (PndFastSim::AddDetector) - No name given, no detector added!"<<endl;

  }

  if (fAddedDets.find(" "+name+" ")!=std::string::npos)
  {
    std::cout <<" -W-  (PndFastSim::AddDetector) - Detector <"<<name<<"> already appended - skipping!"<<std::endl;
    return false;
  }
  fAddedDets.append(std::string(name+" "));

  PndFsmAbsDet *det=0;
  det=fDetFac->create(name,params);
  if (!det) return false;

  fDetList.push_back(det);
  std::cout<<" -I- (PndFastSim::AddDetector) - Added detector "<<name<<" with params <"<<params<<">"<<std::endl;
  return true;
}

bool PndFastSim::AddDetector(PndFsmAbsDet* det)
{
  if (det) {
    fDetList.push_back(det);
    fAddedDets.append(det->detName()+" ");
    std::cout<<" -I- (PndFastSim::AddDetector) - Added detector "<<det->detName()<<std::endl;
  }
  return det;
}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndFastSim::Exec(Option_t* opt)
{
  int nCharged = 0;
  int nNeutral = 0;

  if ((++evtcnt)%100==0)
    cout <<"evt: "<<evtcnt<<endl;

  PndStack *fStack=(PndStack*)gMC->GetStack();
  int nTracks=fStack->GetNtrack();


  // Reset output array
  if (fMcCandidates->GetEntriesFast() != 0)  fMcCandidates->Clear("C");
  if (fPidChargedCand->GetEntriesFast() != 0)  fPidChargedCand->Clear("C");
  if (fPidNeutralCand->GetEntriesFast() != 0)  fPidNeutralCand->Clear("C");
  if (fPidChargedProb->GetEntriesFast() != 0)  fPidChargedProb->Clear("C");
  if (fPidNeutralProb->GetEntriesFast() != 0)  fPidNeutralProb->Clear("C");
  //if (fMicroCandidates->GetEntriesFast() != 0) fMicroCandidates->Clear("C");
  if (fEventInfo->GetEntriesFast() != 0) fEventInfo->Clear("C");
  for(std::map<TString,TClonesArray*>::iterator iter=fPidArrayList.begin();iter!=fPidArrayList.end();iter++)
  {
    if(iter->second->GetEntriesFast() != 0) iter->second->Clear("C");
  }

  TClonesArray &mctracks       = *fMcCandidates;

  TClonesArray &chrgCandidates = *fPidChargedCand;  // Charged Candidates
  TClonesArray &neutCandidates = *fPidNeutralCand;  // Neutral Candidates

  TClonesArray &chrgProbs      = *fPidChargedProb;  // PID for charged Cands
  TClonesArray &neutProbs      = *fPidNeutralProb;  // PID for neutral Cands

  //TClonesArray &microCandidates = *fMicroCandidates;
  TClonesArray &evtInfo         = *fEventInfo;

  if (fVb)cout <<"number of tracks **** "<< nTracks <<endl;


  RhoCandList l;
  TLorentzVector McSumP4(0,0,0,0);
  TVector3 McAvgVtx(0,0,0);

  for (Int_t iTrack=0; iTrack<nTracks; iTrack++) {

    // Get the MCTrack information
    Int_t mcsize = mctracks.GetEntriesFast();
    Int_t chcandsize = chrgCandidates.GetEntriesFast();
    Int_t neucandsize = neutCandidates.GetEntriesFast();
    //Int_t miccandsize = microCandidates.GetEntriesFast();

    TParticle *t = fStack->GetParticle(iTrack);
    if (fVb>1) t->Print();

    TLorentzVector p4(t->Px(),t->Py(),t->Pz(),t->Energy());

    TVector3 stvtx(t->Vx(),t->Vy(),t->Vz());

    //TLorentzVector vtx(stvtx,t->T());
    TParticlePDG* part = fdbPdg->GetParticle(t->GetPdgCode());
    double charge(0);//safety against unknown pdgcodes, might scrw up the charge
    if(part) charge=part->Charge();
    if (fabs(charge)>2) charge/=3.;

    PndFsmTrack *ft=new PndFsmTrack(p4,stvtx,stvtx,charge,t->GetPdgCode(),iTrack+1);
    //PndFsmTrack ft(p4,stvtx,stvtx,charge,t->GetPdgCode(),iPoint+1);

    // store a plain copy of the mc track to the file
    RhoCandidate *pmc=new (mctracks[mcsize]) RhoCandidate(ft->p4(),ft->charge());
    pmc->SetMcTruth(pmc);;
    pmc->SetPos(ft->startVtx());
    pmc->SetType(t->GetPdgCode());
    // write some ideal pid lhs
    switch(abs(t->GetPdgCode())) {
      case 11: pmc->SetPidInfo(0, 1); break;
      case 13: pmc->SetPidInfo(1, 1); break;
      case 211: pmc->SetPidInfo(2, 1); break;
      case 321: pmc->SetPidInfo(3, 1); break;
      case 2212: pmc->SetPidInfo(4, 1); break;
    }

    McSumP4+=ft->p4();
    McAvgVtx+=ft->startVtx();

    // smear and cut the track according to the detector setup
    if (smearTrack(ft)) {
      RhoVector3Err *svtx=new RhoVector3Err(ft->startVtx());

      //RhoCandidate *tcand;

      //PndPidCandidate *micro;
      TLorentzVector miclv=ft->p4();
      TVector3 pos=ft->startVtx();

      // assign pion mass to all charged and 0 to all neutral cands
      if (fabs(ft->charge())>0.001)
      {
        miclv.SetVectM(miclv.Vect(),fdbPdg->GetParticle(211)->Mass());
        nCharged++;
      }
      else
      {
        miclv.SetVectM(miclv.Vect(),0.0);
        nNeutral++;
      }

      /*
      micro=new (microCandidates[miccandsize]) PndPidCandidate((Int_t)ft->charge(),pos,miclv);

      micro->SetMcIndex(iPoint);
      micro->SetMvdDEDX( ft->detResponse()->MvddEdx() );
      //micro->SetMvdDEdxErr( ft->detResponse()->MvddEdxErr() );
      micro->SetSttMeanDEDX( ft->detResponse()->SttdEdx() );
      //micro->SetSttDEdxErr( ft->detResponse()->SttdEdxErr() );
      micro->SetTofM2( ft->detResponse()->m2() );
      micro->SetTofM2Err( ft->detResponse()->m2Err() );
      micro->SetDrcThetaC( ft->detResponse()->DrcBarrelThtc() );
      micro->SetDrcThetaCErr( ft->detResponse()->DrcBarrelThtcErr() );
      micro->SetDrcNumberOfPhotons(0);
      micro->SetDiscThetaC( ft->detResponse()->DrcDiscThtc() );
      micro->SetDiscThetaCErr( ft->detResponse()->DrcDiscThtcErr() );
      micro->SetDiscNumberOfPhotons(0);
      micro->SetRichThetaC( ft->detResponse()->RichThtc() );
      micro->SetRichThetaCErr( ft->detResponse()->RichThtcErr() );
      micro->SetRichNumberOfPhotons(0);

      micro->SetElectronPidLH(ft->detResponse()->LHElectron());
      micro->SetMuonPidLH(ft->detResponse()->LHMuon());
      micro->SetPionPidLH(ft->detResponse()->LHPion());
      micro->SetKaonPidLH(ft->detResponse()->LHKaon());
      micro->SetProtonPidLH(ft->detResponse()->LHProton());

      if (fPropagate && fabs(ft->charge())>1e-6) {
  micro->SetCov7( ft->Cov7() );
    }
    */


      PndPidCandidate *pidCand;
      PndPidProbability *pidProb;

      if (fabs(ft->charge())>1e-6)
      {
        pidCand = new (chrgCandidates[chcandsize]) PndPidCandidate((Int_t)ft->charge(),pos,miclv);
        pidProb = new (chrgProbs[chcandsize]) PndPidProbability();
      }
      else
      {
        pidCand = new (neutCandidates[neucandsize]) PndPidCandidate((Int_t)ft->charge(),pos,miclv);
        pidProb = new (neutProbs[neucandsize]) PndPidProbability();
      }

      pidCand->SetMcIndex(iTrack);
      pidCand->SetMvdDEDX( ft->detResponse()->MvddEdx() );
      //pidCand->SetMvdDEdxErr( ft->detResponse()->MvddEdxErr() );
      pidCand->SetSttMeanDEDX( ft->detResponse()->SttdEdx() );
      //pidCand->SetSttDEdxErr( ft->detResponse()->SttdEdxErr() );
      pidCand->SetTofM2( ft->detResponse()->m2() );
      //pidCand->SetTofM2Err( ft->detResponse()->m2Err() );
      pidCand->SetDrcThetaC( ft->detResponse()->DrcBarrelThtc() );
      pidCand->SetDrcThetaCErr( ft->detResponse()->DrcBarrelThtcErr() );
      pidCand->SetDrcNumberOfPhotons(0);
      pidCand->SetDiscThetaC( ft->detResponse()->DrcDiscThtc() );
      pidCand->SetDiscThetaCErr( ft->detResponse()->DrcDiscThtcErr() );
      pidCand->SetDiscNumberOfPhotons(0);
      pidCand->SetRichThetaC( ft->detResponse()->RichThtc() );
      pidCand->SetRichThetaCErr( ft->detResponse()->RichThtcErr() );
      pidCand->SetRichNumberOfPhotons(0);
      pidCand->SetEmcCalEnergy(ft->detResponse()->EmcEcal() );
	  pidCand->SetMuoIron(ft->detResponse()->MuoIron() );
      pidProb->SetElectronPdf(ft->detResponse()->LHElectron());
      pidProb->SetMuonPdf(ft->detResponse()->LHMuon());
      pidProb->SetPionPdf(ft->detResponse()->LHPion());
      pidProb->SetKaonPdf(ft->detResponse()->LHKaon());
      pidProb->SetProtonPdf(ft->detResponse()->LHProton());
      pidProb->SetIndex(chcandsize);

      /*
      pidCand->SetElectronPidLH(ft->detResponse()->LHElectron());
      pidCand->SetMuonPidLH(ft->detResponse()->LHMuon());
      pidCand->SetPionPidLH(ft->detResponse()->LHPion());
      pidCand->SetKaonPidLH(ft->detResponse()->LHKaon());
      pidCand->SetProtonPidLH(ft->detResponse()->LHProton());
      */

      if (fPropagate && fabs(ft->charge())>1e-6) {
        pidCand->SetCov7( ft->Cov7() );
      }

      /*
      if (fabs(ft->charge())>1e-6)
      tcand=new (chrgCandidates[chcandsize]) RhoCandidate(ft->p4(),ft->charge(),svtx);
      else
        tcand=new (neutCandidates[neucandsize]) RhoCandidate(ft->p4(),ft->charge(),svtx);


      // the likelihood values;
      tcand->SetPidInfo( 0, ft->detResponse()->LHElectron());
      tcand->SetPidInfo( 1, ft->detResponse()->LHMuon());
      tcand->SetPidInfo( 2, ft->detResponse()->LHPion());
      tcand->SetPidInfo( 3, ft->detResponse()->LHKaon());
      tcand->SetPidInfo( 4, ft->detResponse()->LHProton());

      //the direct pid relevant measurements
      tcand->SetPidInfo( 5, ft->detResponse()->DrcBarrelThtc());
      tcand->SetPidInfo( 6, ft->detResponse()->DrcDiscThtc());
      tcand->SetPidInfo( 7, ft->detResponse()->RichThtc());
      tcand->SetPidInfo( 8, ft->detResponse()->m2());
      tcand->SetPidInfo( 9, ft->detResponse()->MvddEdx());
      tcand->SetPidInfo(10, ft->detResponse()->SttdEdx());

      tcand->SetPidInfo(12,  ft->detResponse()->DrcBarrelThtcErr());
      tcand->SetPidInfo(13, ft->detResponse()->DrcDiscThtcErr());
      tcand->SetPidInfo(14, ft->detResponse()->RichThtcErr());
      tcand->SetPidInfo(15, ft->detResponse()->m2Err());
      tcand->SetPidInfo(16, ft->detResponse()->MvddEdxErr());
      tcand->SetPidInfo(17, ft->detResponse()->SttdEdxErr());
      tcand->SetPidInfo(29, (double) ft->pdt());
      //cout<<"********************** PID="<<ft->pdt()<<endl;

      tcand->SetMcIdx(iPoint);
      tcand->SetType(ft->pdt());

      if (fPropagate && fabs(charge)>1e-6) {
  tcand->SetCov7( ft->Cov7() );
  micro->SetCov7( ft->Cov7() );
    }

    // as default set pion mass for charged tracks and 0 for neutral cands
    if (fabs(tcand->Charge())>0.001) {
  tcand->SetMass(fdbPdg->GetParticle(211)->Mass());
    } else
      tcand->SetMass(0.0);
    */

      RhoCandidate tcand(ft->p4(),ft->charge(),svtx);

      l.Add(&tcand);

      /*
      tcand=new (pndCandidates[pndcandsize]) RhoCandidate(ft->p4(),ft->charge(),svtx);

      tcand->SetMcIdx(iPoint);
      tcand->SetPidInfo(pidinfo);

      if (fabs(tcand->Charge())>0.001)
      tcand->SetMass(TDatabasePDG::Instance()->GetParticle(211)->Mass());
      else
        tcand->SetMass(0.0);
      */
      delete svtx;

      // shall we add some parametrized split offs?
      if (fGenSplitOffs)
      {
        int type=0;
        int abslid = abs(ft->pdt());

        if (abslid == 11) type=0;
        else if (abslid == 211) type=2;
        else if (abslid == 321) type=3;
        else if (abslid == 2212) type=4;
        else continue;

        //number of split offs?
        int numSP=(int)fspo[type][3]->GetRandom();

        if (fVb) cout <<" -I- (PndFastSim::Exec) - creating "<<numSP
          <<" split offs for particle with type "<<type<<endl;

        for (int i=0;i<numSP;i++)
        {
          TLorentzVector lv=ft->p4();
          TVector3 fPos(0.,0.,0.);
          RhoVector3Err *svtx2=new RhoVector3Err(fPos);

          double mom   = fspo[type][0]->GetRandom();
          double dphi  = fspo[type][1]->GetRandom();
          double dtht  = fspo[type][2]->GetRandom();

          lv.SetPhi(lv.Phi()+dphi);
          lv.SetTheta(lv.Theta()+dtht);
          lv.SetRho(mom);
          lv.SetE(lv.P());

          /*
          PndPidCandidate *micro=new (microCandidates[microCandidates.GetEntriesFast()])
          PndPidCandidate(0,fPos,lv);
          micro->SetMcIndex(-1);
          */


          neucandsize = neutCandidates.GetEntriesFast();

          pidCand = new (neutCandidates[neucandsize]) PndPidCandidate((Int_t)0,fPos,lv);
          pidProb = new (neutProbs[neucandsize]) PndPidProbability();
          pidCand->SetMcIndex(-1);

          RhoCandidate tCand(lv,0.0,svtx2);
          tCand.SetType(22);
          nNeutral++;


          l.Add(&tCand);

          delete svtx2;
          //tcand=new (pndCandidates[pndCandidates.GetEntriesFast()]) RhoCandidate(lv,0.0);
          //tcand->SetMcIdx(-1);

        } // split off loop

      }// generate split offs

    }// smeartrack
    delete ft;
  }//trackloop

  McAvgVtx*=1./(double)nTracks;

  PndEventInfo *eventInfo=new (evtInfo[evtInfo.GetEntriesFast()]) PndEventInfo();

  eventInfo->SetIPTruth(McAvgVtx);
  eventInfo->SetCmFrame(McSumP4);
  eventInfo->SetCharged(nCharged);
  eventInfo->SetNeutrals(nNeutral);

  //  TEventShape shape(l);
  //  eventInfo->SetEventShape(shape);

  RhoFactory::Instance()->Reset();

}
// -------------------------------------------------------------------------

bool PndFastSim::smearTrack(PndFsmTrack *t)
{


  FsmResponseList responseList;

  for (FsmAbsDetList::iterator iter=fDetList.begin();iter!=fDetList.end(); iter++)
  {
    PndFsmAbsDet *det=*iter;
    //if (!det) {cout <<"--------------------------> outch"<<endl;continue;}

    PndFsmResponse *respo=det->respond(t);

    if (respo) responseList.push_back(respo);
  }

  PndFsmResponse *allResponse=sumResponse(responseList);

  t->setDetResponse(allResponse);

  bool success = cutAndSmear(t);

  for (FsmResponseList::iterator riter=responseList.begin(); riter!=responseList.end();riter++)
  {
    PndFsmResponse *resp=*riter;
    if (!resp) continue;
    if(success&&resp->detector()->doesPid())
    { //save PID information only if particle is stored
      TString detname = resp->detector()->detName();
      //std::cout<<"try PID array "<<detname.Data()<<std::endl;
      TClonesArray* myPidarray = fPidArrayList[detname];
      int npid = myPidarray->GetEntriesFast();
      PndPidProbability *pidProb=new((*myPidarray)[npid]) PndPidProbability();
      double rawLHe  = resp->LHElectron();
      double rawLHmu = resp->LHMuon();
      double rawLHpi = resp->LHPion();
      double rawLHK  = resp->LHKaon();
      double rawLHp  = resp->LHProton();

      double sumRaw = rawLHe+rawLHmu+rawLHpi+rawLHK+rawLHp;
      if (sumRaw!=0.)
      {
        pidProb->SetElectronPdf(rawLHe);
        pidProb->SetMuonPdf(rawLHmu);
        pidProb->SetPionPdf(rawLHpi);
        pidProb->SetKaonPdf(rawLHK);
        pidProb->SetProtonPdf(rawLHp);
      } else {
        pidProb->SetElectronPdf(0.2);
        pidProb->SetMuonPdf(0.2);
        pidProb->SetPionPdf(0.2);
        pidProb->SetKaonPdf(0.2);
        pidProb->SetProtonPdf(0.2);
      }
    }
    // and now clean up!
    delete resp;
  }
  responseList.clear();

  return success;
}

bool
PndFastSim::cutAndSmear(PndFsmTrack *t)
{
  return cutAndSmear(t, t->detResponse());
}

//-----------------------------------------------------------------------

bool
PndFastSim::cutAndSmear(PndFsmTrack *t, PndFsmResponse *r)
{
  if( !(r->detected()) ) return false;

  double charge = t->charge();
  double dE     = r->dE();
  double dp     = r->dp();
  double dtheta = r->dtheta();
  double dphi   = r->dphi();
  double dm     = r->dm();
  double m2     = r->m2();
  double MvddEdx =r->MvddEdx();
  double SttdEdx =r->SttdEdx();
  TVector3 dV   = r->dV();

  //this removes candidates, which only have hit a PID-only device (like Cherenkov, TOF ...)
  if (fabs(charge)>1e-6 && fabs(dp)<1e-8) return false;
  if (fabs(charge)<1e-6 && fabs(dE)<1e-8) return false;

  // now produce some correlated errors
  if (fPropagate && fabs(charge)>1e-6) {
    t->HelixRep(t->startVtx() );
    double p2 = t->p4().Vect().Mag2();
    double omega = t->GetHelixOmega();
    double tandip = t->GetHelixTanDip();
    double theta = t->p4().Theta();

    static TVectorD gaus(5);
    for (char p=0;p<5;p++)
      gaus[p]=fRand->Gaus();
    if (fUseCovMatrix)
      gaus*=fEta;
    // calculate track par errors
    double err[5];
    // d0 (guessed), phi0, z0
    err[0] = 0.5*(dV.X()+dV.Y());
    err[1] = dphi;
    err[3] = dV.Z();
    // omega needs some error propagation (momentum and theta are uncorrelated)
    err[2] = omega * sqrt( dp*dp/p2 + pow(dtheta*tandip,2) );
    // as well as tandip
    err[4] = dtheta/pow(sin(theta),2);
    // smear track pars
    for (char p=0;p<5;p++)
      t->GetHelixParams()[p] += err[p] * gaus[p];
    // write scaled cov matrix
    for (char ir=0;ir<5;ir++)
      for (char c=0;c<5;c++)
        t->GetHelixCov()(ir,c) = fRho(ir,c)*fabs(err[ir]*err[c])*(fUseCovMatrix||ir==c);
      t->Propagate( fToStartVtx ? t->startVtx() : TVector3(0,0,0), fTolerance);
    // uncharged particles remain uncorrelated
  } else {
    if (dE != 0.0)     smearEnergy(t,dE);
    if (dp != 0.0)     smearMomentum(t,dp);
    if (dtheta != 0.0) smearTheta(t,dtheta);
    if (dphi != 0.0)   smearPhi(t,dphi);
    if ( fabs(charge)>1e-6 && (dV.X() != 0.0 || dV.Y() != 0.0 || dV.Z() != 0.0)) smearVertex(t,dV);
  }
  if (dm != 0.0)     smearM(t,dm);
  if( m2!=0.0)       smearM2(t,m2);           // mass^2 of track after tof
    if(MvddEdx!=0.0)   smearMvddEdx(t,MvddEdx); // dEdx of track after Mvd
      if(SttdEdx!=0.0)   smearSttdEdx(t,SttdEdx); // dEdx of track after Stt
        return true;
}

//-----------------------------------------------------------------------

void
PndFastSim::smearEnergy(PndFsmTrack *t, double dE)
{
  TLorentzVector p4=t->p4();
  double m=t->p4().M();

  //  if (t->pdt()->lundId()==22) //gammas are always on mass shell
  if (fabs(t->charge()) < 0.1 ) //gammas are always on mass shell
  {
    //    double rnd = pRand->Gaus(0.,dE);
    double rnd = fRand->Gaus(0. , dE);

    double newE = rnd + p4.E();

    p4.SetVectM(p4.Vect()*(newE/p4.E()) , 0.0);
  }
  else
  {
    double newE = fRand->Gaus(0.0,dE) + p4.E();
    double newP = sqrt(newE*newE - m*m);

    p4.SetE(newE);
    p4.SetVectMag(p4.Vect(),newP);
  }

  t->setP4(p4);
}

void
PndFastSim::smearMomentum(PndFsmTrack *t, double dp)
{
  TLorentzVector p4=t->p4();
  double newP = p4.Vect().Mag() + fRand->Gaus(0.0,dp);
  p4.SetVectM(p4.Vect().Unit()*newP,t->p4().M());
  t->setP4(p4);
}

void
PndFastSim::smearTheta(PndFsmTrack *t, double dtheta)
{
  TLorentzVector p4=t->p4();
  double newTheta = p4.Theta() + fRand->Gaus(0.,dtheta);
  p4.SetTheta(newTheta);

  t->setP4(p4);
}

void
PndFastSim::smearPhi(PndFsmTrack *t, double dphi)
{
  TLorentzVector p4=t->p4();
  double newPhi = p4.Phi() + fRand->Gaus(0.,dphi);
  p4.SetPhi(newPhi);

  t->setP4(p4);
}

void
PndFastSim::smearM(PndFsmTrack *t, double dm)
{
  TLorentzVector p4=t->p4();
  //  double newM = p4.m() + RandGauss::shoot(0.,dm);
  double newM=p4.M() + dm;

  p4.SetVectM(p4.Vect(),newM);

  t->setP4(p4);
}

void
PndFastSim::smearM2(PndFsmTrack *t, double m2)
{

  t->setMass2(m2);
}

void
PndFastSim::smearMvddEdx(PndFsmTrack *t, double MvddEdx)
{

  t->setMvddEdX(MvddEdx);
}

void
PndFastSim::smearSttdEdx(PndFsmTrack *t, double SttdEdx)
{

  t->setSttdEdX(SttdEdx);
}

void
PndFastSim::smearVertex(PndFsmTrack *t, TVector3 dV)
{
  TVector3 vtx = t->startVtx();

  double newX = vtx.X() + fRand->Gaus( 0. , dV.X() );
  double newY = vtx.Y() + fRand->Gaus( 0. , dV.Y() );
  double newZ = vtx.Z() + fRand->Gaus( 0. , dV.Z() );

  vtx.SetX(newX);
  vtx.SetY(newY);
  vtx.SetZ(newZ);

  t->setStartVtx(vtx);
}

//-----------------------------------------------------------------------


PndFsmResponse*
PndFastSim::sumResponse(FsmResponseList respList)
{

  bool detected=false;

  double dE=0.0;
  double dp=0.0;
  double dtheta=0.0;
  double dphi=0.0;
  double dt=0.0;
  double dm=0.0;

  double m2=0;
  double MvddEdx=0;
  double SttdEdx=0;
  double DrcDiscThtc=0;
  double DrcBarrelThtc=0;
  double RichThtc=0;
  double EmcEcal=0;
  double MuoIron=0;

  double m2Err=0;
  double MvddEdxErr=0;
  double SttdEdxErr=0;
  double DrcDiscThtcErr=0;
  double DrcBarrelThtcErr=0;
  double RichThtcErr=0;

  double dVx=0.0;
  double dVy=0.0;
  double dVz=0.0;

  double LH_e=1.0;
  double LH_mu=1.0;
  double LH_pi=1.0;
  double LH_K=1.0;
  double LH_p=1.0;

  double val=0.0;

  PndFsmResponse *allResponse=new PndFsmResponse();

  if (fVb>3) cout <<" *** SINGLE RESPONSES ***"<<endl<<"--------------------------------------"<<endl;

  for (FsmResponseList::iterator riter=respList.begin(); riter!=respList.end();riter++)
  {
    PndFsmResponse *resp=*riter;

    if ( fVb>3 ) resp->print(cout);

    detected = detected | resp->detected();

    if (resp->detected())
    {
      if (fabs(val = resp->dE()) > 1e-8)     dE += 1/(val*val);
      if (fabs(val = resp->dp()) > 1e-8)      dp += 1/(val*val);
      if (fabs(val = resp->dtheta())> 1e-8) dtheta += 1/(val*val);
      if (fabs(val = resp->dphi()) > 1e-8)   dphi += 1/(val*val);

      if (fabs(val = resp->dt()) > 1e-8)     dt += val*val;
      if (fabs(val = resp->dm()) > 1e-8)     dm =val;
      if (fabs (val = resp->m2()) > 1e-11)    m2=val;
      if (fabs (val = resp->MvddEdx()) > 1e-11)    MvddEdx=val;
      if (fabs (val = resp->SttdEdx()) > 1e-11)    SttdEdx=val;
      if (fabs (val = resp->DrcDiscThtc()) > 1e-11)   DrcDiscThtc=val;
      if (fabs (val = resp->DrcBarrelThtc()) > 1e-11)    DrcBarrelThtc=val;
      if (fabs (val = resp->RichThtc()) > 1e-11)    RichThtc=val;
      if (fabs (val = resp->EmcEcal()) > 1e-11)    EmcEcal=val;
      if (fabs (val = resp->MuoIron()) > 1e-11)    MuoIron=val;

      if (fabs (val = resp->m2Err()) > 1e-11)    m2Err=val;
      if (fabs (val = resp->MvddEdxErr()) > 1e-11)    MvddEdxErr=val;
      if (fabs (val = resp->SttdEdxErr()) > 1e-11)    SttdEdxErr=val;
      if (fabs (val = resp->DrcDiscThtcErr()) > 1e-11)   DrcDiscThtcErr=val;
      if (fabs (val = resp->DrcBarrelThtcErr()) > 1e-11)    DrcBarrelThtcErr=val;
      if (fabs (val = resp->RichThtcErr()) > 1e-11)    RichThtcErr=val;

      if (fabs (val = resp->dV().X()) > 1e-11) dVx += 1/(val*val);
      if (fabs (val = resp->dV().Y()) > 1e-11) dVy += 1/(val*val);
      if (fabs (val = resp->dV().Z()) > 1e-11) dVz += 1/(val*val);

      double rawLHe  = resp->LHElectron();
      double rawLHmu = resp->LHMuon();
      double rawLHpi = resp->LHPion();
      double rawLHK  = resp->LHKaon();
      double rawLHp  = resp->LHProton();

      double sumRaw = rawLHe+rawLHmu+rawLHpi+rawLHK+rawLHp;

      if (sumRaw>0) {
        rawLHe  /= sumRaw;
        rawLHmu /= sumRaw;
        rawLHpi /= sumRaw;
        rawLHK  /= sumRaw;
        rawLHp  /= sumRaw;
        LH_e  *= rawLHe;
        LH_mu *= rawLHmu;
        LH_pi *= rawLHpi;
        LH_K  *= rawLHK;
        LH_p  *= rawLHp;
      } else {
        LH_e  *= 0.2;
        LH_mu *= 0.2;
        LH_pi *= 0.2;
        LH_K  *= 0.2;
        LH_p  *= 0.2;
      }

      //here a weighted Likelihood evaluation has to be done

      /*
      if (val = rawLHe)  LH_e  == 0.0 ? LH_e  = val : LH_e  *= val;
      if (val = rawLHmu) LH_mu == 0.0 ? LH_mu = val : LH_mu *= val;
      if (val = rawLHpi) LH_pi == 0.0 ? LH_pi = val : LH_pi *= val;
      if (val = rawLHK)  LH_K  == 0.0 ? LH_K  = val : LH_K  *= val;
      if (val = rawLHp)  LH_p  == 0.0 ? LH_p  = val : LH_p  *= val;
      */
    }
  }

  double sumLH = LH_e + LH_mu + LH_pi + LH_K + LH_p;

  if (sumLH>0) {
    LH_e  /= sumLH;
    LH_mu /= sumLH;
    LH_pi /= sumLH;
    LH_K  /= sumLH;
    LH_p  /= sumLH;
  } else {
    LH_e  = 0.2;
    LH_mu = 0.2;
    LH_pi = 0.2;
    LH_K  = 0.2;
    LH_p  = 0.2;
  }

  allResponse->setDetected(detected);
  allResponse->setdE( dE>0. ? 1/sqrt(dE) : 0.0 );
  allResponse->setdp( dp>0. ? 1/sqrt(dp) : 0.0 );
  allResponse->setdtheta( dtheta>0. ? 1/sqrt(dtheta) : 0.0 );
  allResponse->setdphi( dphi>0. ? 1/sqrt(dphi) : 0.0 );
  allResponse->setdt( sqrt(dt) );
  allResponse->setdm(dm);

  allResponse->setm2(m2, m2Err);
  allResponse->setMvddEdx(MvddEdx,MvddEdxErr);
  allResponse->setSttdEdx(SttdEdx,SttdEdxErr);

  allResponse->setDrcDiscThtc(DrcDiscThtc,DrcDiscThtcErr);
  allResponse->setDrcBarrelThtc(DrcBarrelThtc,DrcBarrelThtcErr);
  allResponse->setRichThtc(RichThtc,RichThtcErr);
  allResponse->setEmcEcal(EmcEcal);
  allResponse->setMuoIron(MuoIron);

  if (dVx > 0.) dVx=1./sqrt(dVx); else dVx = 0.0;
  if (dVy > 0.) dVy=1./sqrt(dVy); else dVy = 0.0;
  if (dVz > 0.) dVz=1./sqrt(dVz); else dVz = 0.0;

  allResponse->setdV( dVx , dVy , dVz );

  allResponse->setLHElectron(LH_e);
  allResponse->setLHMuon(LH_mu);
  allResponse->setLHPion(LH_pi);
  allResponse->setLHKaon(LH_K);
  allResponse->setLHProton(LH_p);


  if (fVb>2) cout <<"--------------------------------------"<<endl<<"*** OVERALL RESPONSE ***"<<endl<<"--------------------------------------" <<endl;
  if (fVb>2) allResponse->print(cout);
  if (fVb>2) cout <<"--------------------------------------"<<endl;

  return allResponse;
}

//-----------------------------------------------------------------------

TMatrixD PndFastSim::fRho(5,5);
TMatrixD PndFastSim::fEta(5,5);

ClassImp(PndFastSim)
