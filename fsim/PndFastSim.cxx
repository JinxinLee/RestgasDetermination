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
#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "CbmMCTrack.h"
#include "CbmStack.h"
#include "CbmRun.h"

#include "PndFastSim.h"

//Rho includes
#include "RhoBase/TCandidate.h"
#include "RhoBase/TSimpleVertex.h"
#include "RhoBase/TRho.h"
#include "RhoBase/PndMicroCandidate.h"
#include "RhoBase/PndEventInfo.h"
#include "RhoBase/TCandList.h"
#include "RhoTools/TEventShape.h"
#include "RhoBase/TFactory.h"

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

using std::cout;
using std::endl;
using std::string;
using std::ifstream;

		
// -----   Default constructor   -------------------------------------------
PndFastSim::PndFastSim() :
  CbmTask("Panda Fast Simulation") { 
  //  fCandidates = new TClonesArray("TParticle");
  fRand=new TRandom3();
  fDetFac = new PndFsmDetFactory;
  fAddedDets=" ";
  fVb=0;
  fGenSplitOffs=false;
  fPropagate=false;
  fdbPdg = TRho::Instance()->GetPDG();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndFastSim::~PndFastSim() { 
    CbmRootManager *fManager =CbmRootManager::Instance();
   fManager->Write();

  if (fChargedCandidates) {fChargedCandidates->Delete(); delete fChargedCandidates;}
  if (fNeutralCandidates) {fNeutralCandidates->Delete(); delete fNeutralCandidates;}
  if (fMcCandidates) {fMcCandidates->Delete(); delete fMcCandidates;}
  if (fMicroCandidates) {fMicroCandidates->Delete(); delete fMicroCandidates;}
  if (fEventInfo) {fEventInfo->Delete(); delete fEventInfo;}

  if (fDetFac) delete fDetFac;
  if (fRand) delete fRand;
}
// -------------------------------------------------------------------------


void PndFastSim::Register() {
  //---

  fMcCandidates = new TClonesArray("TCandidate");
  CbmRootManager::Instance()->Register("PndMcTracks","FastSim", fMcCandidates, kTRUE);

  //fPndCandidates = new TClonesArray("TCandidate");
  //CbmRootManager::Instance()->Register("PndCandidates","FastSim", fPndCandidates, kTRUE);

  fChargedCandidates = new TClonesArray("TCandidate");
  CbmRootManager::Instance()->Register("PndChargedCandidates","FastSim", fChargedCandidates, kTRUE);

  fNeutralCandidates = new TClonesArray("TCandidate");
  CbmRootManager::Instance()->Register("PndNeutralCandidates","FastSim", fNeutralCandidates, kTRUE);

  fMicroCandidates = new TClonesArray("PndMicroCandidate");
  CbmRootManager::Instance()->Register("PndMicroCandidates","FastSim", fMicroCandidates, kTRUE);

  fEventInfo = new TClonesArray("PndEventInfo");
  CbmRootManager::Instance()->Register("PndEventSummary","FastSim", fEventInfo, kTRUE);

}



// -----   Public method Init   --------------------------------------------
InitStatus PndFastSim::Init() {
 
  if (fVb>3) cout << " Inside the Init function****" << endl;
  
  //CbmDetector::Initialize();
  //CbmRun* sim = CbmRun::Instance();
  //CbmRuntimeDb* rtdb=sim->GetRuntimeDb();
  
  // Get RootManager
  /* CbmRootManager* ioman = CbmRootManager::Instance();
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
  //d0            phi0            omega          
  fRho(0,0)= 1.0; fRho(0,1)=-0.9; fRho(0,2)= 0.2; // d0
  fRho(1,0)=-0.9; fRho(1,1)= 1.0; fRho(1,2)=-0.4; // phi0
  fRho(2,0)= 0.2; fRho(2,1)=-0.4; fRho(2,2)= 1.0; // omega
  //z0            tandip 
  fRho(3,3)= 1.0; fRho(3,4)=-0.9; // z0
  fRho(4,3)=-0.9; fRho(4,4)= 1.0; // tandip
  // and this is the square root of fRho 
  // needed to correlate the track params
  //d0            phi0               omega          
  fEta(0,0)= 1.0;                                       // d0
  fEta(1,0)=-0.9; fEta(1,1)= 0.4359;                    // phi0
  fEta(2,0)= 0.2; fEta(2,1)=-0.5047; fEta(2,2)= 0.8398; // omega
  //z0            tandip 
  fEta(3,3)= 1.0;                    // z0
  fEta(4,3)=-0.9; fEta(4,4)= 0.4359; // tandip

  // Create and register output array
  cout << "-I- PndFastSim: Intialization successfull" << endl;
  return kSUCCESS;
}

void PndFastSim::SetParContainers() {

  // Get run and runtime database
  CbmRunAna* run = CbmRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  CbmRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

 
}
// -------------------------------------------------------------------------

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

void PndFastSim::EnablePropagation(bool propagate) {
  fPropagate=propagate;
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
  if ((++evtcnt)%100==0)
    cout <<"evt: "<<evtcnt<<endl;
  
  CbmStack *fStack=(CbmStack*)gMC->GetStack();  
  int nTracks=fStack->GetNtrack();


   // Reset output array
  if (fMcCandidates->GetEntriesFast() != 0)  fMcCandidates->Clear("C");
  if (fChargedCandidates->GetEntriesFast() != 0)  fChargedCandidates->Clear("C");
  if (fNeutralCandidates->GetEntriesFast() != 0)  fNeutralCandidates->Clear("C");
  if (fMicroCandidates->GetEntriesFast() != 0) fMicroCandidates->Clear("C");
  if (fEventInfo->GetEntriesFast() != 0) fEventInfo->Clear("C");

  TClonesArray &mctracks = *fMcCandidates;
  TClonesArray &chrgCandidates= *fChargedCandidates;
  TClonesArray &neutCandidates= *fNeutralCandidates;
  TClonesArray &microCandidates = *fMicroCandidates;
  TClonesArray &evtInfo         = *fEventInfo;

  if (fVb)cout <<"number of tracks **** "<< nTracks <<endl;
  
  
  TCandList l;
  TLorentzVector McSumP4(0,0,0,0);
  TVector3 McAvgVtx(0,0,0);
  
  for (Int_t iPoint=0; iPoint<nTracks; iPoint++) {

	  // Get the MCTrack information
    Int_t mcsize = mctracks.GetEntriesFast();
    Int_t chcandsize = chrgCandidates.GetEntriesFast();
    Int_t neucandsize = neutCandidates.GetEntriesFast();
    Int_t miccandsize = microCandidates.GetEntriesFast();

    TParticle *t = fStack->GetParticle(iPoint);
    if (fVb>1) t->Print();

    TLorentzVector p4(t->Px(),t->Py(),t->Pz(),t->Energy());

    TVector3 stvtx(t->Vx(),t->Vy(),t->Vz());

    //TLorentzVector vtx(stvtx,t->T());
    
    double charge=fdbPdg->GetParticle(t->GetPdgCode())->Charge();
    if (fabs(charge)>2) charge/=3.;

    PndFsmTrack *ft=new PndFsmTrack(p4,stvtx,stvtx,charge,t->GetPdgCode(),iPoint+1);
    //PndFsmTrack ft(p4,stvtx,stvtx,charge,t->GetPdgCode(),iPoint+1);

    // store a plain copy of the mc track to the file
    TCandidate *pmc=new (mctracks[mcsize]) TCandidate(ft->p4(),ft->charge());
    pmc->SetMcIdx(iPoint);
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
      TSimpleVertex *svtx=new TSimpleVertex(ft->startVtx());
      
      TCandidate *tcand;
      PndMicroCandidate *micro;
      TLorentzVector miclv=ft->p4();
      TVector3 pos=ft->startVtx();
      
      // assign pion mass to all charged and 0 to all neutral cands
      if (fabs(ft->charge())>0.001) 
        miclv.SetVectM(miclv.Vect(),fdbPdg->GetParticle(211)->Mass());
      else
        miclv.SetVectM(miclv.Vect(),0.0);
      
      micro=new (microCandidates[miccandsize]) PndMicroCandidate((Int_t)ft->charge(),pos,miclv);

      micro->SetMcIndex(iPoint);
      micro->SetMvdMeanDEdx( ft->detResponse()->MvddEdx() );
      micro->SetMvdDEdxErr( ft->detResponse()->MvddEdxErr() );
      micro->SetSttMeanDEdx( ft->detResponse()->SttdEdx() );
      micro->SetSttDEdxErr( ft->detResponse()->SttdEdxErr() );
      micro->SetTpcMeanDEdx( ft->detResponse()->TpcdEdx() );
      micro->SetTpcDEdxErr( ft->detResponse()->TpcdEdxErr() );
      micro->SetTofM2( ft->detResponse()->m2() );
      micro->SetTofM2Err( ft->detResponse()->m2Err() );
      micro->SetBarrelDrcThetaC( ft->detResponse()->DrcBarrelThtc() );
      micro->SetBarrelDrcThetaCErr( ft->detResponse()->DrcBarrelThtcErr() );
      micro->SetBarrelDrcNumberOfPhotons(0);
      micro->SetDiscDrcThetaC( ft->detResponse()->DrcDiscThtc() );
      micro->SetDiscDrcThetaCErr( ft->detResponse()->DrcDiscThtcErr() );
      micro->SetDiscDrcNumberOfPhotons(0);
      micro->SetRichThetaC( ft->detResponse()->RichThtc() );
      micro->SetRichThetaCErr( ft->detResponse()->RichThtcErr() );
      micro->SetRichNumberOfPhotons(0);
      
      micro->SetElectronPidLH(ft->detResponse()->LHElectron());
      micro->SetMuonPidLH(ft->detResponse()->LHMuon());
      micro->SetPionPidLH(ft->detResponse()->LHPion());
      micro->SetKaonPidLH(ft->detResponse()->LHKaon());
      micro->SetProtonPidLH(ft->detResponse()->LHProton());
      
      if (fabs(ft->charge())>1e-6) 
        tcand=new (chrgCandidates[chcandsize]) TCandidate(ft->p4(),ft->charge(),svtx);
      else 
        tcand=new (neutCandidates[neucandsize]) TCandidate(ft->p4(),ft->charge(),svtx);

      double pidinfo[30];
    
      // the likelihood values;
      pidinfo[0] = ft->detResponse()->LHElectron();
      pidinfo[1] = ft->detResponse()->LHMuon(); 
      pidinfo[2] = ft->detResponse()->LHPion(); 
      pidinfo[3] = ft->detResponse()->LHKaon();
      pidinfo[4] = ft->detResponse()->LHProton();
      
      //the direct pid relevant measurements
      pidinfo[5] = ft->detResponse()->DrcBarrelThtc();
      pidinfo[6] = ft->detResponse()->DrcDiscThtc();
      pidinfo[7] = ft->detResponse()->RichThtc();
      pidinfo[8] = ft->detResponse()->m2();
      pidinfo[9] = ft->detResponse()->MvddEdx();
      pidinfo[10]= ft->detResponse()->SttdEdx();
      pidinfo[11]= ft->detResponse()->TpcdEdx();
    
      pidinfo[12] = ft->detResponse()->DrcBarrelThtcErr();
      pidinfo[13] = ft->detResponse()->DrcDiscThtcErr();
      pidinfo[14] = ft->detResponse()->RichThtcErr();
      pidinfo[15] = ft->detResponse()->m2Err();
      pidinfo[16] = ft->detResponse()->MvddEdxErr();
      pidinfo[17]= ft->detResponse()->SttdEdxErr();
      pidinfo[18]= ft->detResponse()->TpcdEdxErr();
      pidinfo[29]= (double) ft->pdt();
      //cout<<"********************** PID="<<ft->pdt()<<endl;
      
      tcand->SetMcIdx(iPoint);
      tcand->SetPidInfo(pidinfo);
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
        
      l.Add(*tcand);
    
      /*	
      tcand=new (pndCandidates[pndcandsize]) TCandidate(ft->p4(),ft->charge(),svtx);
      
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
          TVector3 pos(0,0,0);
          
          double mom   = fspo[type][0]->GetRandom();
          double dphi  = fspo[type][1]->GetRandom();
          double dtht  = fspo[type][2]->GetRandom();
          
          lv.SetPhi(lv.Phi()+dphi);
          lv.SetTheta(lv.Theta()+dtht);
          lv.SetRho(mom);
          lv.SetE(lv.P());
          
          PndMicroCandidate *micro=new (microCandidates[microCandidates.GetEntriesFast()])
              PndMicroCandidate(0,pos,lv);
          micro->SetMcIndex(-1);
          
          TCandidate *tcand=new (neutCandidates[neutCandidates.GetEntriesFast()]) TCandidate(lv,0.0);
          tcand->SetMcIdx(-1);
          tcand->SetType(22);
          
          l.Add(*tcand);
          
          //tcand=new (pndCandidates[pndCandidates.GetEntriesFast()]) TCandidate(lv,0.0);
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
  eventInfo->SetCharged(chrgCandidates.GetEntriesFast());
  eventInfo->SetNeutrals(neutCandidates.GetEntriesFast());
  
  TEventShape shape(l);
  eventInfo->SetEventShape(shape);
  
  TFactory::Instance()->Reset();

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

  for (FsmResponseList::iterator riter=responseList.begin(); riter!=responseList.end();riter++) 
  {
    PndFsmResponse *resp=*riter;
    if (resp) delete resp;
  }    
  responseList.clear();
 

  return cutAndSmear(t);

}

bool PndFastSim::SmearTrack(PndFsmTrack* t) {
  return smearTrack(t);
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
  double theta = t->p4().Theta();
  double p2 = t->p4().Mag2();
  double omega = t->GetHelixOmega();
  double tandip = t->GetHelixTanDip();

  double dE     = r->dE();
  double dp     = r->dp();
  double dtheta = r->dtheta();
  double dphi   = r->dphi();
  double dm     = r->dm();
  double m2     = r->m2();
  double MvddEdx =r->MvddEdx();
  double TpcdEdx =r->TpcdEdx();
  double SttdEdx =r->SttdEdx();
  TVector3 dV   = r->dV();

  //this removes candidates, which only have hit a PID-only device (like Cherenkov, TOF ...)
  if (fabs(charge)>1e-6 && fabs(dp)<1e-8) return false;
  if (fabs(charge)<1e-6 && fabs(dE)<1e-8) return false;
    
  // now produce some correlated errors 
  if (fPropagate && fabs(charge)>1e-6) {
    static TVectorD gaus(5);
    for (char p=0;p<5;p++)
      gaus(p)=fRand->Gaus();
//  gaus*=fEta;
    // calculate track par errors
    Float_t err[5];
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
      t->GetHelixParams()[p] += err[p] * gaus(p);
    // write scaled cov matrix
    for (char r=0;r<5;r++)
    for (char c=0;c<5;c++) 
      t->GetHelixCov()(r,c) = fRho(r,c)*fabs(err[r]*err[c]) * (r==c);
    t->Propagate();
    //debug
//  TMatrixD rho7(7,7);
//  for (char r=0;r<7;r++)
//  for (char c=0;c<7;c++) 
//    rho7(r,c)=t->Cov7()(r,c)/sqrt(t->Cov7()(r,r)*t->Cov7()(c,c));
//  t->print(std::cout);
//  rho7.Print();
    // uncharged particles remain uncorrelated
  } else {
    if (dE != 0.0)     smearEnergy(t,dE);
    if (dp != 0.0)     smearMomentum(t,dp);
    if (dtheta != 0.0) smearTheta(t,dtheta);
    if (dphi != 0.0)   smearPhi(t,dphi);
    if (dV.X() != 0.0 || dV.Y() != 0.0 || dV.Z() != 0.0) smearVertex(t,dV);
  }
  if (dm != 0.0)     smearM(t,dm);
  if( m2!=0.0)       smearM2(t,m2);           // mass^2 of track after tof
  if(MvddEdx!=0.0)   smearMvddEdx(t,MvddEdx); // dEdx of track after Mvd
  if(TpcdEdx!=0.0)   smearTpcdEdx(t,TpcdEdx); // dEdx of track after Tpc
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
  p4.SetVectM(p4.Vect()*(newP/p4.Vect().Mag()),t->p4().M());
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
PndFastSim::smearTpcdEdx(PndFsmTrack *t, double TpcdEdx)
{

  t->setTpcdEdX(TpcdEdx);
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

  dX dE;
  dX dp;
  dX dtheta;
  dX dphi;
  double dt=0.0;
  double dm=0.0;

  double m2=0;
  double MvddEdx=0;
  double TpcdEdx=0;
  double SttdEdx=0;
  double DrcDiscThtc=0;
  double DrcBarrelThtc=0;
  double RichThtc=0;

  double m2Err=0;
  double MvddEdxErr=0;
  double TpcdEdxErr=0;
  double SttdEdxErr=0;
  double DrcDiscThtcErr=0;
  double DrcBarrelThtcErr=0;
  double RichThtcErr=0;

  dX dVx;
  dX dVy;
  dX dVz;
  
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
      if (fabs(val = resp->dE()) > 1e-8)     dE += val;
      if (fabs(val = resp->dp()) > 1e-8)      dp += val;
      if (fabs(val = resp->dtheta())> 1e-8) dtheta += val;
      if (fabs(val = resp->dphi()) > 1e-8)   dphi += val;
      if (fabs(val = resp->dt()) > 1e-8)     dt += val*val;
      if (fabs(val = resp->dm()) > 1e-8)     dm +=val;
      if (fabs (val = resp->m2()) > 1e-11)    m2+=val;
      if (fabs (val = resp->MvddEdx()) > 1e-11)    MvddEdx+=val;
      if (fabs (val = resp->TpcdEdx()) > 1e-11)    TpcdEdx+=val;
      if (fabs (val = resp->SttdEdx()) > 1e-11)    SttdEdx+=val;
      if (fabs (val = resp->DrcDiscThtc()) > 1e-11)   DrcDiscThtc+=val;
      if (fabs (val = resp->DrcBarrelThtc()) > 1e-11)    DrcBarrelThtc+=val;
      if (fabs (val = resp->RichThtc()) > 1e-11)    RichThtc+=val;

      if (fabs (val = resp->m2Err()) > 1e-11)    m2Err+=val;
      if (fabs (val = resp->MvddEdxErr()) > 1e-11)    MvddEdxErr+=val;
      if (fabs (val = resp->TpcdEdxErr()) > 1e-11)    TpcdEdxErr+=val;
      if (fabs (val = resp->SttdEdxErr()) > 1e-11)    SttdEdxErr+=val;
      if (fabs (val = resp->DrcDiscThtcErr()) > 1e-11)   DrcDiscThtcErr+=val;
      if (fabs (val = resp->DrcBarrelThtcErr()) > 1e-11)    DrcBarrelThtcErr+=val;
      if (fabs (val = resp->RichThtcErr()) > 1e-11)    RichThtcErr+=val;

      if (fabs (val = resp->dV().X()) > 1e-11) dVx += val;
      if (fabs (val = resp->dV().Y()) > 1e-11) dVy += val;
      if (fabs (val = resp->dV().Z()) > 1e-11) dVz += val;

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

  LH_e  /= sumLH;
  LH_mu /= sumLH;
  LH_pi /= sumLH;
  LH_K  /= sumLH;
  LH_p  /= sumLH;
  
  allResponse->setDetected(detected);
  allResponse->setdE( dE );
  allResponse->setdp( dp );
  allResponse->setdtheta( dtheta );
  allResponse->setdphi( dphi );
  allResponse->setdt( sqrt(dt) );
  allResponse->setdm(dm);

  allResponse->setm2(m2, m2Err);
  allResponse->setMvddEdx(MvddEdx,MvddEdxErr);
  allResponse->setTpcdEdx(TpcdEdx,TpcdEdxErr);
  allResponse->setSttdEdx(SttdEdx,SttdEdxErr);
  
  allResponse->setDrcDiscThtc(DrcDiscThtc,DrcDiscThtcErr);
  allResponse->setDrcBarrelThtc(DrcBarrelThtc,DrcBarrelThtcErr);
  allResponse->setRichThtc(RichThtc,RichThtcErr);
  
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

PndFastSim::dX::dX() {
  n=0;
  b=0;
}

void PndFastSim::dX::operator += (double v) {
  n++;
  b+=1/v;
}

void PndFastSim::dX::operator = (double v) {
  n=1;
  b=1/v;
}

PndFastSim::dX::operator double() {
  return b>0 ? sqrt(n)/b : 0;
}

ClassImp(PndFastSim)
