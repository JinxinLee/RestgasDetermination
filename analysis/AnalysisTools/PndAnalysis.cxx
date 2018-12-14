// PndAnalysis
// Needs a FairRunAna set up in the macro for file & parameter I/O

#include "PndAnalysis.h"

#include <string>
#include <iostream>

using std::cout;
using std::endl;

//Root stuff
#include "TTree.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TParticle.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"

#include "RhoParticleSelectorBase.h"
#include "PndPidCandidate.h"

//RHO stuff
#include "RhoFactory.h"
#include "RhoCandidate.h"
#include "RhoCandList.h"
#include "RhoPdtLoader.h"

#include "FairTrackParP.h"
#include "FairTrackParH.h"
#include "FairGeanePro.h"
//#include "FairRunAna.h"
#include "FairField.h"

#include "PndTrack.h"
#include "PndPidCandidate.h"
#include "PndPidProbability.h"
#include "PndAnaPidSelector.h"
#include "PndAnaPidCombiner.h"
#include "PndMCTrack.h"
//#include "PndAnaCovTool.h" // using a cov matrix tool
#include "RhoCalculationTools.h"
#include "PndPidBremCorrected4Mom.h"

ClassImp ( PndAnalysis );

PndAnalysis::PndAnalysis ( TString tname1, TString tname2, TString algnamec, TString algnamen ) :
  fRootManager ( FairRootManager::Instance() ),
  fPidSelector ( 0 ),
  fEvtCount ( 0 ),
  fChainEntries ( 0 ),
  fEventRead ( false ),
  fBuildMcCands ( false ),
  fVerbose(0),
  fPhotosMax(0), fPhotosThresh(0.05),
  fChargedPidName ( algnamec ),
  fNeutralPidName ( algnamen ),
  fTracksName ( tname1 ),
  fTracksName2 ( tname2 ),
  fDefaultHypo ( 2 )
{
  if ( 0 == fRootManager ) {
    std::cout << "-E- PndAnalysis: RootManager not instantiated!" << std::endl;
    return;
  }

  Init();
}

PndAnalysis::~PndAnalysis()
{
  if ( 0!=fPidSelector ) {
    delete fPidSelector;
  }
}

TClonesArray* PndAnalysis::ReadTCA ( TString tcaname )
{
  TClonesArray* tca = ( TClonesArray* ) fRootManager->GetObject ( tcaname.Data() );
  if ( fVerbose>4 && ! tca ) { // the info is printed by the RootManager already.
    std::cout << "-I- PndAnalysis::ReadTCA(): No "<<tcaname.Data() <<" array found." << std::endl;
  }

  return tca;
}

void PndAnalysis::Init()
{
  Reset();
  fPidHypoStr[0] = "Electron";
  fPidHypoStr[1] = "Muon";
  fPidHypoStr[2] = "Pion";
  fPidHypoStr[3] = "Kaon";
  fPidHypoStr[4] = "Proton";
  fPidHypoStr[5] = "";
  fHypoPdg[0]=-11;
  fHypoPdg[1]=-13;
  fHypoPdg[2]=211;
  fHypoPdg[3]=321;
  fHypoPdg[4]=2212;
  fHypoPdg[5]=0;

  fNeutralCands = ReadTCA ( "PidNeutralCand" );
  //if ( !fNeutralCands ) {	}
  //fNeutralProbability = ReadTCA ( fNeutralPidName.Data()+fPidHypoStr[fDefaultHypo] );

  // List of branches to check by default
  TString branchnames1[4]= {fTracksName,"SttMvdGemGenTrack","BarrelGenTrack","SttMvdGenTrack"};
  TString branchnames2[4]= {fTracksName2,"FTSGenTrack","FtsIdealGenTrack","FTSTrkIdeal"};
  for (int i = 0; i < 6 ; i++) // 0-4 for PID hypothesis, 5 for fallback
  {
    //read pidcand arrays
    fTracks[i]=NULL; // initialize properly
    fTracks2[i]=NULL; // initialize properly
    fBremCorr[i]=NULL; // initialize properly
    fChargedProbability[i]=NULL; // initialize properly
    fChargedCands[i] = ReadTCA ( "PidChargedCand"+fPidHypoStr[i] );
    if ( fChargedCands[i] ) {
      fHypoFlagCharged[i] = true;
      std::cout<< "  ####################  read PidChargedCand with special tracking hypothesis: "<<("PidChargedCand"+fPidHypoStr[i]).Data()<<"  pointer "<<fChargedCands[i] <<std::endl;
    } else {
      fHypoFlagCharged[i] = false;
      //fChargedCands[i] = ReadTCA ( "PidChargedCand" );
      std::cout<< "  ####################  No PidChargedCand with "<<fPidHypoStr[i].Data()<<" tracking hypothesis" <<std::endl;
    }

    // Read Bremsstrahling Corrections and PID values
    if(fHypoFlagCharged[i]) {
      fBremCorr[i] = ReadTCA( "BremCorrected4Mom"+fPidHypoStr[i] );
      fChargedProbability[i] = ReadTCA ( fChargedPidName.Data()+fPidHypoStr[i] );
    }

    // load barrel tracks
    for(int k=0; k<4; k++) {
      if(fHypoFlagCharged[i]) {
        if(fVerbose>4) std::cout << "-I- PndAnalysis::Init(): br:"<<k<<" hyp:"<<i<<" Trying \""<<(branchnames1[k]+fPidHypoStr[i]).Data() <<"\" now.";
        fTracks[i] = ReadTCA ( branchnames1[k]+fPidHypoStr[i] );
      }
      if ( fTracks[i] ) {
        if(fVerbose>4) std::cout << " Succes reading tracking array \""<<(branchnames1[k]+fPidHypoStr[i]).Data() <<"\" with pointer "<<fTracks[i];
        break;
      }
    }
    if ( !fTracks[i] ) {
      std::cout << "-W- PndAnalysis::Init(): No barrel track inpt array." << std::endl;
    } else {
      std::cout<<" printing track array:";
      std::cout<<endl;
      fTracks[i]->Print();
    }

    // load forward tracks
    for(int k=0; k<4; k++) {
      if(fHypoFlagCharged[i]) {

        if(fVerbose>4) std::cout << "-I- PndAnalysis::Init(): Trying \""<<(branchnames2[k]+fPidHypoStr[i]).Data() <<"\" now.";
        fTracks2[i] = ReadTCA ( branchnames2[k]+fPidHypoStr[i] );
      }
      if ( fTracks2[i] ) break;
    }
    if ( !fTracks2[i] ) {
      std::cout << "-W- PndAnalysis::Init(): No forward track inpt array." << std::endl;
    } else {
      std::cout<<endl;
      fTracks2[i]->Print();
    }

  } // loop tracking hyp

  //Check if a list of pions exists. If not, set default hypo to something else
  if (!fHypoFlagCharged[2]) {
    if (fHypoFlagCharged[1]) {
      fDefaultHypo=1;
      std::cout<<"PndAnalysis::Init(): Default hypothesis is muons."<<std::endl;
    }
    else if (fHypoFlagCharged[0]) {
      fDefaultHypo=0;
      std::cout<<"PndAnalysis::Init(): Default hypothesis is electrons."<<std::endl;
    }
    else if (fHypoFlagCharged[3]) {
      fDefaultHypo=3;
      std::cout<<"PndAnalysis::Init(): Default hypothesis is Kaons."<<std::endl;
    }
    else if (fHypoFlagCharged[4]) {
      fDefaultHypo=4;
      std::cout<<"PndAnalysis::Init(): Default hypothesis is protons."<<std::endl;
    }
    else {
      std::cout<<"PndAnalysis::Init(): No Multikalman input branches exist, do fallback."<<std::endl;
      fDefaultHypo=5; // empty string in name arrays
    }
  } else {
    std::cout<<"PndAnalysis::Init(): Default hypothesis is pions."<<std::endl;
  }

  // -- MC Tracks
  fBuildMcCands = false;

  fMcCands = 0;

  if(fVerbose>4) std::cout << "-I- PndAnalysis::Init(): Trying mc stack now." << std::endl;
  fMcTracks = ( TClonesArray* ) fRootManager->GetObject ( "MCTrack" );

  if ( ! fMcTracks && fVerbose ) {
    std::cout << "-W- PndAnalysis::Init(): No \"MCTrack\" array found. No MC info available." << std::endl;
  } else {
    fBuildMcCands = true;
    fMcCands = new TClonesArray ( "RhoCandidate" );
    // next line commented by KG, 07/2012
    fRootManager->Register ( "PndMcTracks","PndMcTracksFolder", fMcCands, kFALSE );
  }

  //fChainEntries = ( fRootManager->GetInChain() )->GetEntries();
  fChainEntries = fRootManager->CheckMaxEventNo();

  //TODO default constructor here?
  fPidCombiner = new PndAnaPidCombiner();
  fPidSelector = new PndAnaPidSelector();

}


void PndAnalysis::Rewind()
{
  fEvtCount=0;
}

void PndAnalysis::Cleanup()
{
  // do a safe cleanup
  for (int i = 0; i<6; i++) {
    //fAllCandList[i].Cleanup();
    fChargedCandList[i].Cleanup();
  }
  fNeutralCandList.Cleanup();
  fMcCandList.Cleanup();
  RhoFactory::Instance()->Reset();

}

//void PndAnalysis::ReadCandidates()
//{
//ReadRecoCandidates();
//BuildMcCands();
//return;
//}

void PndAnalysis::GetEventInTask()
{
  Cleanup();
  ReadRecoCandidates();
  BuildMcCands();
  return;
}

Int_t PndAnalysis::GetEvent ( Int_t n )
{
  Cleanup();

  if ( n>=0 ) {
    fEvtCount=n+1;
  } else {
    fEvtCount++;
  }

  if ( fEvtCount>fChainEntries ) {
    fEvtCount=fChainEntries;
    Info("PndAnalysis::GetEvent()","Maximum number of entries in the file chain reached: %i.",fEvtCount);
    return 0;
  }
  fRootManager->ReadEvent ( fEvtCount-1 );

  //Printout
  if(fVerbose>4)
  {
    std::cout
      <<"-------->8-------->8-------->8-------->8-------->8-------->8-------->8-------->8"
      <<"\n"
      <<" No. -   Name  -    Flag - PidArr  - Trk Arr -Trk Arr2 -     PID -     Brem"
      <<std::endl;
    for (int i = 0; i < 6 ; i++) // 0-4 for PID hypothesis, 5 for fallback
    {
      std::cout
          <<std::setw(5)<<i
          <<std::setw(10)<<fPidHypoStr[i]
          <<std::setw(10)<<fHypoFlagCharged[i]
          <<std::setw(10)<<fChargedCands[i]
          <<std::setw(10)<<fTracks[i]
          <<std::setw(10)<<fTracks2[i]
          <<std::setw(10)<<fChargedProbability[i]
          <<std::setw(10)<<fBremCorr[i]
          <<std::endl;
    }
    std::cout
      <<"-------->8-------->8-------->8-------->8-------->8-------->8-------->8-------->8"
      <<std::endl;
  }

  ReadRecoCandidates();
  BuildMcCands();

  if(fVerbose) Info("PndAnalysis::GetEvent()","Finished loading event fEvtCount=%i.",fEvtCount);
  return fEvtCount;
}

FairMCEventHeader* PndAnalysis::GetEventHeader()
{
  if ( !fEventRead ) {
    fRootManager->ReadEvent ( fEvtCount-1 );
    fEventRead=kTRUE;
  }

  FairMCEventHeader*  evthead = ( FairMCEventHeader* ) FairRootManager::Instance()->GetObject ( "MCEventHeader." );

  return evthead;
}

Bool_t PndAnalysis::FillList ( RhoCandList& resultList, TString listkey, TString pidTcaNames, int trackHypothesis )
{
  // Reads the specified List for the current event
  resultList.Cleanup();

  // Select the right tracking hypothesis
  if(fHypoFlagCharged[fDefaultHypo]) {
    trackHypothesis=fDefaultHypo; // Pions are default
  }

  TString trkPostfix[6]= {"Electron","Muon","Pion","Kaon","Proton",""};
  if(0>trackHypothesis || 6<trackHypothesis) { //only for unsupportet track hyp. mumber
    for(int i=0; i<6; ++i) {
      if (fHypoFlagCharged[i] && listkey.Contains ( trkPostfix[i] ) ) {
        trackHypothesis=i;
        break;
      }
    }
  }
  if(fVerbose>4) cout<<"PndAnalysis::FillList() listkey=\""<<listkey<<"\" trackhypo="<<trackHypothesis<<" pidTcaNames=\""<<pidTcaNames.Data()<<"\"  trkPostfix=\""<<trkPostfix[trackHypothesis]<<"\""<<endl;

  // Set which PID information should be used.
  if ( pidTcaNames!="" ) {
    fPidCombiner->SetTcaNames ( pidTcaNames, trkPostfix[trackHypothesis] );
  } else {
    // FIXME This may cause problems, if defaults are not there
    fPidCombiner->SetDefaults(); // no pid array
  }

  // Get or build Monte-Carlo truth list
  if ( listkey=="McTruth" ) {
    return GetMcCandList(resultList);
  }

  if(fVerbose>4)Info("PndAnalysis::FillList","key=%s",listkey.Data());
  // acceleration: just give the large lists directly

  //if ( listkey=="Neutral" ) {
  if ( listkey.Contains ( "Neutral" ) ) {
    resultList=fNeutralCandList;
    return kTRUE;
  }
  //if ( listkey.Contains ( "Neutral" ) )
  //{ // The neutrals are all clusters, except for the ones which were close to a track. The hypothesis is used in extrapolating the track...
  ////fPidCombiner->Apply ( fNeutralCandList );
  //fPidSelector->Select ( fNeutralCandList,resultList );
  //return kTRUE;

  //}

  if ( listkey=="Charged" ) {
    resultList=fChargedCandList[trackHypothesis];
    fPidCombiner->Apply ( resultList );
    if(fVerbose>4)cout<<"trackhyp="<<trackHypothesis<<" list size after selection="<<resultList.GetLength()<<endl;
   return kTRUE;
  }

  const bool doBremCorr = listkey.Contains("Brem");
  if (doBremCorr) listkey.ReplaceAll("Brem","");

  // Real selection requested:
  // set the base list for the PID list maker
  Bool_t checkcrit = fPidSelector->SetCriterion ( listkey );
  if (!checkcrit) return kFALSE;

  if ( listkey.Contains ( "Electron" ) ||listkey.Contains ( "Muon" ) ||listkey.Contains ( "Pion" )
       || listkey.Contains ( "Kaon" ) ||listkey.Contains ( "Proton" )
       || listkey.Contains ( "Plus" ) ||listkey.Contains ( "Minus" ) ||listkey.Contains ( "Charged" ) ) {
    // We create a copy of all charged candidates

    resultList=fChargedCandList[trackHypothesis];
    if(fVerbose>4)cout<<"trackhyp="<<trackHypothesis<<" list size="<<resultList.GetLength()<<endl;
    // Correction for Bremsstrahlung, if desired
    if ( doBremCorr )
    {
      if (fBremCorr[trackHypothesis]==0) {
        if(fVerbose) Warning("PndAnalysis::FillList","Brem requested but no PndPidBremCorrected4Mom found on input file. Brem Correction can't be done.");
      } else {
        for (int j=0; j<resultList.GetLength(); ++j)
        {
          int trk_id = resultList[j]->GetTrackNumber();
          int nBremCorr = fBremCorr[trackHypothesis]->GetEntriesFast();
          if (nBremCorr!=resultList.GetLength())
            if(fVerbose)
              Warning("PndAnalysis::FillList","Warning: BermCorr list size diff. from chargeCandList");
          PndPidBremCorrected4Mom *bremCorr = (PndPidBremCorrected4Mom*) fBremCorr[trackHypothesis]->At(trk_id);
          resultList[j]->SetP3(bremCorr->GetMomentum());
        }
      }
    }
    fPidCombiner->Apply ( resultList );
    if(fVerbose>4)cout<<"trackhyp="<<trackHypothesis<<" list size after pid      ="<<resultList.GetLength()<<endl;
    resultList.Select(fPidSelector);
    if(fVerbose>4)cout<<"trackhyp="<<trackHypothesis<<" list size after selection="<<resultList.GetLength()<<endl;
    return kTRUE;
  }

  Error ( "FillList", "Unknown list key: %s",listkey.Data() );
  return kFALSE;
}

Bool_t PndAnalysis::GetMcCandList(RhoCandList& l)
{
  l.Clear();
  // Put all candidates from the mctruth list to the candlist and set the mother-daughter relations
  if ( !fMcCands ) return kFALSE;

  RhoCandidate* truth=0;
  for (int i=0; i<fMcCands->GetEntriesFast(); i++)
  {
    // copy candidates via put
    truth = (RhoCandidate*) fMcCands->At(i);
    if(fVerbose>4) std::cout<<"PndAnalysis::GetMcCandList: mccand "<<i<<" :"<<truth<<" \t "<<*truth<<std::endl;
    l.Put(truth);
  }

  // now set genealogy inside the list
  RhoCandidate* truthmother=0;
  for (int k=0; k<l.GetLength(); k++)
  {
    // get mother track
    PndMCTrack* part = (PndMCTrack*) fMcTracks->At(k);
    Int_t mcMotherID = part->GetMotherID();
    if(mcMotherID<0) mcMotherID=part->GetSecondMotherID();

    // SetMotherLink does the deep mother-daughter relation
    if (mcMotherID<0) continue; // no mother there, go on...
    if (mcMotherID>=l.GetLength()) continue; // something bad hapened to the indices

    // do the linking
    truthmother = (RhoCandidate*) l[mcMotherID];
    l[k]->SetMotherLink(truthmother, false);
  }
  // And now we have to rapair the charges, because delta electrons are inside the MC list, but not the inons
  for (int k=0; k<l.GetLength(); k++)
  {
    TParticlePDG* ppdg = TDatabasePDG::Instance()->GetParticle(l[k]->PdgCode());
    double charge=0.0;
    if ( ppdg ) {
      charge=ppdg->Charge();
    } else if (fVerbose) {
      cout <<"-W- CreateMcCandidate: strange PDG code:"<<l[k]->PdgCode() <<endl;
    }
    if ( fabs(charge) >2 ) {
      charge/=3.;
    }
    l[k]->SetCharge(charge);
  }

  return kTRUE;
}

Int_t PndAnalysis::GetEntries()
{
  if ( fRootManager ) {
    return ( fRootManager->GetInChain() )->GetEntries();
  } else {
    return 0;
  }
}

void PndAnalysis::ReadRecoCandidates()
{
  UInt_t _uid=0;
  for(int i=0; i<6; i++) fChargedCandList[i].Cleanup();
  fNeutralCandList.Cleanup();

  // CHARGED
  int nEntries=0;
  for(int h=0; h<6; h++) {
    if ( fChargedCands[h] ) {
      nEntries=fChargedCands[h]->GetEntriesFast();
      break;
    }
  }
  if(0==nEntries&&fVerbose) Warning("PndAnalysis::ReadRecoCandidates()","No filled charged reco array found.");

  for ( Int_t i2=0; i2<nEntries; i2++ )
  {
    _uid++; // uid will start from (n_neutrals + 1) should be uniquie over all five pid hypotheses
    for(int i=0; i<6; i++)
    {
      if (!fHypoFlagCharged[i]) continue;
      PndPidCandidate* mic = ( PndPidCandidate* ) fChargedCands[i]->At ( i2 );
      RhoCandidate tc ( *mic,_uid );
      tc.SetTrackNumber ( i2 ); // Index for PID arrays
      if(i<5){tc.SetType( tc.Charge()*fHypoPdg[i] );}
      fChargedCandList[i].Add ( &tc );
      if(fVerbose>4) cout<<"Added Candidate to list i="<<i<<" with i2="<<i2<<" making the list to size "<<fChargedCandList[i].GetLength()<<endl;
    }
  }

  // NEUTRALS
  if ( fNeutralCands )
  {
    for ( Int_t i1=0; i1<fNeutralCands->GetEntriesFast(); i1++ )
    {
      _uid++; // uid will start from 1
      PndPidCandidate* mic = ( PndPidCandidate* ) fNeutralCands->At ( i1 );

      RhoCandidate tc ( *mic,_uid );

      tc.SetTrackNumber ( -1 );//(i1);
      tc.SetType( 22 );     // default PDG code for neutrals is gamma = 22
      fNeutralCandList.Add(&tc);
    }
  } else {
    if(fVerbose) Warning("PndAnalysis::ReadRecoCandidates()","No neutral reco array found.");
  }

  return;
}

void PndAnalysis::BuildMcCands()
{
  int i;
  // Make Monte-carlo truth candidates by the reconstructed particles up to the initial state (if available)
  if ( !fBuildMcCands ) {
    if(fVerbose) Info("PndAnalysis::BuildMcCands","No mc to build...");
    return;
  }
  if ( !fMcCands ) {
    Warning("PndAnalysis::BuildMcCands","No array to store candidates...");
    return;
  }
  if ( fMcCands->GetEntriesFast() != 0 ) {
    fMcCands->Delete();
  }
  if ( fMcTracks == 0 ) {
    Error ( "BuildMcCands","MC track Array does not exist." );
    return;
  }

  //loop all MCTracks
  for (i=0; i<fMcTracks->GetEntriesFast(); i++)
  {
    // fetch particle properties
    PndMCTrack* part = (PndMCTrack*) fMcTracks->At(i);
    TLorentzVector p4 = part->Get4Momentum();
    TVector3    stvtx = part->GetStartVertex();
    TParticlePDG* ppdg = TDatabasePDG::Instance()->GetParticle(part->GetPdgCode());
    double charge=0.0;
    if ( ppdg ) {
      charge=ppdg->Charge();
    } else if (fVerbose) {
      cout <<"-W- CreateMcCandidate: strange PDG code:"<<part->GetPdgCode() <<endl;
    }
    if ( fabs(charge) >2 ) {
      charge/=3.;
    }
    // create mc candidate
    RhoCandidate* pmc=new ( (*fMcCands)[i] ) RhoCandidate(p4,charge);
    pmc->SetPos(stvtx);
    pmc->SetType(part->GetPdgCode()); //this overwrites our generator's mass information
    pmc->SetP4(p4);
    pmc->SetTrackNumber(i);
    pmc->SetLink(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId("MCTrack"), i));
  }

  //write correctly assigned copy of mc truth candidates
  GetMcCandList(fMcCandList);

  // Assign MC truth to reconstructed allCandnds
  //todo: make sure that this part is not broken
  RhoCandidate* truth=0;
  for(int icand=0; icand<fNeutralCandList.GetLength(); icand++) {
    RhoCandidate* currentcand=fNeutralCandList.Get(icand);
    //   get reco candidate
    PndPidCandidate* reco = currentcand->GetRecoCandidate();

    if(!reco) {
      if (fVerbose) Info("BuildMcCands","reco object to candidate %i (%p) missing.",icand,currentcand);
      continue;
    }
    // get the mctruth
    Int_t mcidx = reco->GetMcIndex();
    if (mcidx>fMcCandList.GetLength() || mcidx<0) continue;
    truth = fMcCandList[mcidx];
    currentcand->SetMcTruth(truth);
    if(fVerbose)Info("PndAnalysis::BuildMcCands()","Now setting truth index %i (%p) to candidate (uid=%i)", mcidx,truth,currentcand->Uid());
  }
  for(int ihyp=0; ihyp<6; ihyp++) {
    for(int icand=0; icand<fChargedCandList[ihyp].GetLength(); icand++) {
      RhoCandidate* currentcand=fChargedCandList[ihyp].Get(icand);
      //for(int icand=0;icand<fAllCandList[ihyp].GetLength();icand++){
      //RhoCandidate* currentcand=fAllCandList[ihyp].Get(icand);
      //   get reco candidate

      PndPidCandidate* reco = currentcand->GetRecoCandidate();

      if(!reco) {
        if (fVerbose) Info("BuildMcCands","reco object to candidate %i (%p) missing.",icand,currentcand);
        continue;
      }
      // get the mctruth
      Int_t mcidx = reco->GetMcIndex();
      if (mcidx>fMcCandList.GetLength() || mcidx<0) continue;
      truth = fMcCandList[mcidx];
      currentcand->SetMcTruth(truth);
      if(fVerbose)Info("PndAnalysis::BuildMcCands()","Now setting truth index %i (%p) to candidate (uid=%i)", mcidx,truth,currentcand->Uid());
    }

  }
}

Bool_t PndAnalysis::PropagateToIp ( RhoCandidate* cand )
{
  //Propagate from the tracks first parameter set to the POCA from (0,0,0)
  TVector3 ip( 0.,0.,0. );
  return PropagateToPoint ( cand, ip );
}

Bool_t PndAnalysis::PropagateToZAxis ( RhoCandidate* cand )
{
  FairTrackParP tStart = GetFirstPar(cand);
  return Propagator ( 2,tStart,cand );
}

Bool_t PndAnalysis::PropagateToPoint ( RhoCandidate* cand, TVector3 mypoint )
{
  //Propagate from the tracks first parameter set to the POCA from mypoint
  //The candidate is updated but the track not touched
  //Only the uncorrelated errors are propagated,
  //TODO: implement a real cov matrix
  FairTrackParP tStart = GetFirstPar(cand);
  return Propagator ( 1,tStart,cand,mypoint );
}

Bool_t PndAnalysis::PropagateToPlane(RhoCandidate* cand, TVector3 origin, TVector3 dj, TVector3 dk)
{
  //Propagate from the tracks first parameter set to a (detector) plane
  //The candidate is updated but the track not touched
  //Only the uncorrelated errors are propagated,
  //TODO: implement a real cov matrix
  FairTrackParP tStart = GetFirstPar(cand);
  return Propagator ( 3,tStart,cand,origin,kFALSE,kFALSE,dj,dk );
}

PndTrack* PndAnalysis::GetTrack ( RhoCandidate* cand )
{
  if ( !cand ) {
    Error ( "GetTrack","Candidate not found: %p",cand );
    return NULL;
  }

  PndPidCandidate* pidCand = (PndPidCandidate*)cand->GetRecoCandidate();

  if ( !pidCand ) {
    Error ( "GetTrack","PID Candidate not found: %p",pidCand );
    return NULL;
  }

  // TODO fallback should go away someday
  int hypid=5; // default is the fallback -
  for(int a=0; a<5; a++) {
    if(cand->PdgCode()==fHypoPdg[a]) {
      hypid=a;
      break;
    }
  }

  PndTrack* track = ( PndTrack* ) fTracks[hypid]->At ( pidCand->GetTrackIndex() );

  if ( !track ) {
    Warning ( "GetTrack","Could not find track object of index %d",pidCand->GetTrackIndex() );
    return NULL;
  }

  return track;
}

FairTrackParP PndAnalysis::GetFirstPar ( RhoCandidate* cand )
{
  if ( !cand ) {
    Error ( "GetFirstPar","Candidate not found: %p",cand );
    FairTrackParP dummy;
    return dummy;
  }
  PndTrack* track = GetTrack(cand);

  if ( !track ) {
    Warning ( "GetFirstPar","Could not find track object " );
    FairTrackParP dummy;
    return dummy;
  }

  FairTrackParP tStart = track->GetParamFirst();

  return tStart;
}

Bool_t PndAnalysis::ResetDaughters ( RhoCandidate* cand )
{
  Bool_t success=kTRUE;

  for ( Int_t daug =0; daug<cand->NDaughters(); daug++ ) {
    RhoCandidate* a=cand->Daughter ( daug );
    success = success && ResetCandidate ( a );
  }

  return success;
}

Bool_t PndAnalysis::ResetCandidate ( RhoCandidate* cand )
{
  FairTrackParP firstpar = GetFirstPar ( cand );
  Double_t globalCov[6][6];
  firstpar.GetMARSCov ( globalCov );
  TMatrixD err ( 6,6 );

  for ( Int_t ii=0; ii<6; ii++ ) for ( Int_t jj=0; jj<6; jj++ ) {
      err[ii][jj]=globalCov[ii][jj];
    }

  if(fVerbose>3){ std::cout<<"MARS cov (px,py,pz,E,x,y,z): ";err.Print();}
  TLorentzVector lv = cand->P4();

  TMatrixD covPosMom = RhoCalculationTools::GetConverted7 ( RhoCalculationTools::GetFitError ( lv, err ) );

  if(fVerbose>3){ std::cout<<"covPosMom (x,y,z,px,py,pz,E): ";covPosMom.Print();}

  cand->SetPosition ( firstpar.GetPosition() );

  cand->SetP3 ( firstpar.GetMomentum() ); // implicitly uses the candidates mass to set P4

  cand->SetCov7 ( covPosMom );

  return kTRUE;
}


Bool_t PndAnalysis::Propagator ( int mode, FairTrackParP& tStart, RhoCandidate* cand, TVector3 mypoint, Bool_t skipcov, Bool_t overwrite, TVector3 planej, TVector3 planek )
{
  //Propagate from the tracks first parameter set to the POCA from mypoint
  //The candidate is updated but the track not touched
  //Only the uncorrelated errors are propagated,
  //TODO: implement a real cov matrix

  Bool_t rc = kFALSE;
  FairGeanePro* geaneProp = new FairGeanePro();
  Int_t pdgcode = cand->PdgCode();

  if ( fVerbose>0 ) {
    cout<<"Try mode "<<mode<<" with pdgCode "<<pdgcode<<endl;
  }

  if ( fVerbose>2 ) {
    std::cout<<"Start Params are:"<<std::endl;
    tStart.Print();
  }

  Double_t startCov[6][6];

  tStart.GetMARSCov ( startCov );
  TMatrixD errst ( 6,6 );

  for ( Int_t ii=0; ii<6; ii++ ) for ( Int_t jj=0; jj<6; jj++ ) {
      errst[ii][jj]=startCov[ii][jj];
    }

  if ( fVerbose>2 ) {
    std::cout<<"Start MARS cov: ";
    errst.Print();
  }

  if ( 1==mode ) { // to point
    geaneProp->BackTrackToVertex(); //set where to propagate
    geaneProp->SetPoint ( mypoint );
  } else if ( 2==mode ) { // to line
    geaneProp->PropagateToPCA ( 2, -1 );// track back to z axis
    TVector3 ex1 ( 0.,0.,-50. ); // virtual wire of arbitrarily chosen size
    TVector3 ex2 ( 0.,0.,100. );
    geaneProp->SetWire ( ex1,ex2 );
  } else if ( 3==mode ) { // to plane
    geaneProp->PropagateToPlane(mypoint,planej,planek);
  } else {
    Error ( "Propagator()","Use mode 1 (to a TVector3) or mode 2 (to z axis) or mode 3 (to plane). (Mode=%i)",mode );
    return kFALSE;
  }

  if(skipcov) geaneProp->PropagateOnlyParameters();

  FairTrackParH* myResult=0;
  // now we propagate
  if(mode==3) {
    FairTrackParP* tResult = new FairTrackParP();
    rc = geaneProp->Propagate ( &tStart, tResult,pdgcode );
    myResult = new FairTrackParH(*tResult);
  } else {
    myResult = new FairTrackParH();
    FairTrackParH* myStart = new FairTrackParH ( tStart );
    rc = geaneProp->Propagate ( myStart, myResult,pdgcode );
  }

  if ( !rc ) {
    if ( fVerbose>0 ) {
      Warning ( "Propagator()","Geane propagation failed" );
    }
    return kFALSE;
  }

  int ierr=0;
  TVector3 di = myResult->GetMomentum();
  di.SetMag ( 1. );
  TVector3 dj = di.Orthogonal();
  TVector3 dk = di.Cross ( dj );
  FairTrackParP* myParab = new FairTrackParP ( myResult, dj, dk, ierr );

  TVector3 pos( myResult->GetX(),myResult->GetY(),myResult->GetZ() ); // I want to be sure...
  cand->SetPosition( pos );
  cand->SetP3( myResult->GetMomentum() ); // implicitly uses the candidates mass to set P4

  //printout for checks
  if ( fVerbose>1 ) {
    TVector3 vecdiff=tStart.GetPosition() - myResult->GetPosition();
    std::cout<<"position start     :";
    tStart.GetPosition().Print();
    std::cout<<"position ip        :";
    myResult->GetPosition().Print();
    std::cout<<"position difference:";
    vecdiff.Print();
    vecdiff=tStart.GetMomentum()-myResult->GetMomentum();
    std::cout<<"momentum start     :";
    tStart.GetMomentum().Print();
    std::cout<<"momentum ip        :";
    myResult->GetMomentum().Print();
    std::cout<<"momentum difference:";
    vecdiff.Print();
  }

  if(kFALSE==skipcov) {
    Double_t globalCov[6][6];
    myParab->GetMARSCov ( globalCov );
    TMatrixD err ( 6,6 );

    for ( Int_t ii=0; ii<6; ii++ ) for ( Int_t jj=0; jj<6; jj++ ) {
        err[ii][jj]=globalCov[ii][jj];
      }

    if ( fVerbose>2 ) {
      std::cout<<"MARS cov (px,py,pz,E,x,y,z): ";
      err.Print();
    }

    TLorentzVector lv = cand->P4();

    TMatrixD covPosMom = RhoCalculationTools::GetConverted7 ( RhoCalculationTools::GetFitError ( lv, err ) );

    if ( fVerbose>2 ) {
      std::cout<<"covPosMom (x,y,z,px,py,pz,E): ";
      covPosMom.Print();
    }

    cand->SetCov7 ( covPosMom );
  }
  //  rc = RhoCalculationTools::FillHelixParams(cand,skipcov);
  //  if (!rc) {Warning("Propagator()","P7toHelix failed"); return kFALSE;}

  if ( fVerbose>2 ) {
    std::cout<<" :::::::::::  Printout in PndAnalysis::Propagator() :::::::::::  "<<std::endl;

    //std::cout<<"Start Params:"<<std::endl;
    //myStart->Print();

    std::cout<<"SC system params:"
             <<"\nq/p    = "<<myResult->GetQp()
             <<"\nLambda = "<<myResult->GetLambda()
             <<"\nPhi    = "<<myResult->GetPhi()
             <<"\nX_sc   = "<<myResult->GetX_sc()
             <<"\nY_sc   = "<<myResult->GetY_sc()
             <<"\nZ_sc   = "<<myResult->GetZ_sc()
             <<std::endl;

    std::cout<<"some values:"
             <<"\n Z0 ?= z_sc / cos(lambda)  = "<< myResult->GetZ_sc() / cos ( myResult->GetLambda() )
             <<"\n Z0 ?= sqrt(x_sc^2+z_sc^2) = "<<sqrt ( myResult->GetX_sc() *myResult->GetX_sc() +myResult->GetZ_sc() *myResult->GetZ_sc() )
             <<std::endl;
  }

  // COMMENT:
  // When taking the Trackparams in the POCA to the z-axis, the SC system from GEANE matches the common helix params easier, i.e:
  // D0 = y_sc and Z0 = sqrt(x_sc^2 + z_sc^2) = z_sc*tan(Lambda)

  if(overwrite) {
    if ( fVerbose>1 ) {
      Info ( "Propagator  ","overwriting start parameter state with result");
    }
    tStart.SetTrackPar(myParab->GetV(), myParab->GetW(),
                       myParab->GetTV(), myParab->GetTW(),
                       myParab->GetQp(), myParab->GetCov(),
                       myParab->GetOrigin(),
                       myParab->GetIVer(),
                       myParab->GetJVer(),
                       myParab->GetKVer(),
                       myParab->GetSPU()
                      );
  }

  if ( fVerbose>1 ) {
    Info ( "Propagator  ","Succsess=%i",rc );
  }

  return kTRUE;
}



//////////////////////////////////////////////////////
//  MC Truth matching                               //
//////////////////////////////////////////////////////



Bool_t PndAnalysis::McTruthMatch(RhoCandidate* cand, Int_t level, bool verbose)
{
  return MctMatch(cand,fMcCandList,level,verbose);
}

Int_t PndAnalysis::McTruthMatch(RhoCandList& list, Int_t level, bool verbose)
{
  Int_t ifound = 0;
  for(int icand=0; icand<list.GetLength(); icand++) {
    if( true == MctMatch(list[icand],fMcCandList,level,verbose) ) {
      ifound++;
    }
  }
  return ifound;
}

Bool_t PndAnalysis::MctMatch ( RhoCandidate* c, RhoCandList& mct, Int_t level, bool verbose )
{
  Int_t nd  = c->NDaughters();
  Int_t pdg = c->PdgCode();
  //Int_t nmct = mct.GetLength(); //[R.K. 01/2017] unused variable?

  if ( 0==nd ) { // final state particle
    RhoCandidate* mccnd = c->GetMcTruth();
    if ( !mccnd ) {
      if(verbose) Info("PndMcTruthMatch::MctMatch","rejected final state by nonexistent mc truth pointer");
      return false;
    }
    if ( mccnd->PdgCode() == pdg ) {
      if(verbose) Info("PndMcTruthMatch::MctMatch","accepted final state by PDG code (pdg=%i)",pdg);
      //if(verbose) std::cout<<*c<<std::endl;
      return true;
    } else {
      if(verbose) Info("PndMcTruthMatch::MctMatch","rejected final state by PDG Code (pdg=%i|mcpdg=%i)",pdg, mccnd->PdgCode());
      return false;
    }
  }

  // check recursively whether all daughter trees match
  for ( Int_t i=0; i<nd; i++ ) {
    if ( !MctMatch (  c->Daughter ( i ) , mct, level, verbose ) ) {
      if(verbose) Info("PndMcTruthMatch::MctMatch","rejected composite (pdg=%i) by non-matching daughter: idau=%i",pdg,i);
      return false;
    }
  }

  // ***
  // *** MATCH LEVEL 0 reached: only PID of final state particles are matched
  // ***
  if ( 0==level ) {
    return true;
  }
  //if(verbose)std::cout<<"going Level 1"<<std::endl;
  // reset mc truth pointer
  RhoCandidate* mccnd = c->GetMcTruth();
  if(mccnd) {
    if(verbose) Warning("PndMcTruthMatch::MctMatch","Existing MC truth found. Will reset it now.");
    c->SetMcTruth(0);
    //return false;
  }

  // find this particle's truth in the mc decay tree
  RhoCandidate* dauzero = c->Daughter(0);
  if (!dauzero) {
    if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by not existing daughter zero");
    return false;
  }
  RhoCandidate* mcdauzero = dauzero->GetMcTruth();
  if (!mcdauzero) {
    if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by not existing MC truth of daughter zero");
    return false;
  }
  RhoCandidate* mcdauzeromother=mcdauzero->TheMother();
  if (!mcdauzeromother) {
    if(verbose) {
      Info("PndMcTruthMatch::MctMatch","rejected by not existing mother of MC truth of daughter zero");
      cout <<*mcdauzero<<endl;
    }

    return false;
  }

  //now check the tree structure:

  int  ndaudiff  = mcdauzeromother->NDaughters() - nd;

  // we still might accept if only photos photons are missing in the reco tree
  // if overall difference is small enough, we check further below what particles are missing
  if( ndaudiff<0 || ndaudiff>fPhotosMax )
  {
    if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by differing daughter count: cand:%i mc:%i",c->NDaughters(),mcdauzeromother->NDaughters());
    return false;
  }
  // count daughter photons (pdg==22) from MC mother with E < photos_thresh
  int nphall = 0;
  if (ndaudiff>0)
    for (int idau=0; idau<mcdauzeromother->NDaughters(); ++idau)
      if (mcdauzeromother->Daughter(idau)->PdgCode()==22 && mcdauzeromother->Daughter(idau)->E()<fPhotosThresh) nphall++;

  // reco'd photons with E<photos_thresh
  int nphreco = 0;
  //  now if all daughters MC-Mother is the same
  for(int idau=1; idau<nd; idau++) {
    // look if all daughters mc mothers are the same
    RhoCandidate* dau = c->Daughter(idau);
    if (!dau) {
      if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by not existing daughter %i",idau);
      return false;
    }
    RhoCandidate* mcdau = dau->GetMcTruth();
    if (!mcdau) {
      if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by not existing MC truth of daughter %i",idau);
      //if(verbose) std::cout<<*dau<<std::endl;
      return false;
    }
    RhoCandidate* mcdaumother=mcdau->TheMother();
    if (!mcdaumother) {
      if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by not existing mother of MC truth of daughter %i",idau);
      return false;
    }
    if(mcdaumother!=mcdauzeromother) {
      if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by mc mother of daughter %i(%p) is different to mc mother of daughter zero(%p) -> Tree does not match"
                         ,idau,mcdaumother,mcdauzeromother);
      return false;
    }

    // daughter was correctly matched; now check for low energetic photon for photos accept
    if (ndaudiff>0 && mcdau->PdgCode()==22 && mcdau->E()<fPhotosThresh) nphreco++;
  }
  // difference in #phot<max allowed and has to be exactly the total number of particle difference
  if ( (nphall-nphreco)>fPhotosMax || (nphall-nphreco)!=ndaudiff )
  {
    if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by differing daughter count not being photos photons: cand:%i mc:%i",c->NDaughters(),mcdauzeromother->NDaughters());
    return false;
  }

  // ***
  // *** MATCH LEVEL 1: PID of the leaves and tree topology are matched
  // ***
  if ( 1==level ) {
    if (verbose) Info("PndMcTruthMatch::MctMatch","accepted composite(pdg=%i) on match-level 1",pdg);
    c->SetMcTruth(mcdauzeromother);
    if (verbose)  cout <<*c->GetMcTruth()<<endl;
    return true;
  }
  //if(verbose)std::cout<<"going Level 2"<<std::endl;
  // check whether all daughter's mother has correct PDG code
  if ( pdg != mcdauzeromother->PdgCode() ) {
    if(verbose) Info("PndMcTruthMatch::MctMatch","rejected by nonmatching pdg code in tree (pdgcode|mcpdgcode) (%i|%i)",pdg,mcdauzeromother->PdgCode());
    return false;
  }
  // ***
  // *** MATCH LEVEL 2: PID of leaves, tree topology and intermediate particle types are matched
  // ***
  if (verbose) Info("PndMcTruthMatch::MctMatch","accepted composite(pdg=%i) on match-level 2", pdg);
  c->SetMcTruth(mcdauzeromother);
  if (verbose) cout <<*c->GetMcTruth()<<endl;
  return true;  // c's tree matches!
}









