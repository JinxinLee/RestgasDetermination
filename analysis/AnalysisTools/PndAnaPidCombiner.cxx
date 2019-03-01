//
//  PndAnaPidCombiner.cxx
//  PANDAROOT
//
//  Created by Ralf Kliemt on 12/16/11.
//
//  This class reads the different information from the PID algorithms
//  and combines them according to the users specifications
//
//

#include "PndAnaPidCombiner.h"

#include <cmath>
#include "TDatabasePDG.h"

#include "RhoBase/RhoCandidate.h"
#include "RhoBase/RhoCandList.h"

#include "PndPidCandidate.h"

#include "TClonesArray.h"
#include "TPRegexp.h"

#include "PndPidProbability.h"


ClassImp ( PndAnaPidCombiner )

PndAnaPidCombiner::PndAnaPidCombiner ( const char* name, TString tcanames ) :
  TNamed ( name,"Panda PID Combiner" ) ,
  fRootManager ( 0 ),
  fPidArrays(),//FIXME: What should the initializing constructor contain here?
  fPidResult ( 0 ),
  fInitialized ( kFALSE )
{
  //  std::cout<< "PndAnaPidCombiner created with tcanames: "<<tcanames.Data()<<std::endl;
  if ( tcanames=="" ) {
    SetDefaults();
  } else {
    SetTcaNames ( tcanames );
  }

  fRootManager=FairRootManager::Instance();

  fPidResult = new PndPidProbability();
}

Bool_t PndAnaPidCombiner::Apply ( RhoCandList& tcl )
{
  Bool_t check = kTRUE;
  Bool_t chack = kTRUE;
  for ( int j=0; j<tcl.GetLength(); j++ ) {
    chack = Apply( tcl[j] );
    check = check && chack;
  }
  return check;
}

Bool_t PndAnaPidCombiner::Apply ( RhoCandidate* tc )
{
  // Apply the multiplied pdf's to the RhoCandidate
  // If on of the pdf's is not available, it is skipped
  Bool_t check=kTRUE;

  //TODO: Merge PID info now.
  fPidResult->Reset();
  // combine algorithms
  TClonesArray* aTca=0;
  PndPidProbability* aProb=0;
  Int_t trackIndex = tc->GetTrackNumber();
  //std::cout<<"PidCombiner: Try RhoCandidate uid:"<<tc->Uid()<<" trknr:"<<trackIndex<<std::endl;
  //std::cout<<tc<<std::endl;
  fPidResult->SetIndex(trackIndex);
  if ( fCurrentPidArrays.size()==0 || trackIndex<0 ) {
    ApplyFlat ( tc );
    return kFALSE;
  }

  for ( std::vector<TString>::iterator iter=fCurrentPidArrays.begin();
        iter!=fCurrentPidArrays.end(); iter++ ) {
    aTca=fPidArrays[*iter];
    //Info ( "Apply","try tca %s at %p",*iter.Data(),aTca );

    if ( 0==aTca ) {
      Error ( "Apply", "PID Probability array not found, skip setting pid for candidate %i.",trackIndex );
      check=kFALSE;
      continue;
    }

    if ( 0==aTca->GetEntriesFast() ) {
      Error ( "Apply", "PID Probability array '%s' at %p of size zero, skip setting pid for candidate %i.", aTca->GetName(), aTca, trackIndex );
      continue;
      check=kFALSE;
    }

    if ( trackIndex>=aTca->GetEntriesFast() ) {
      Error ( "Apply", "Index out of '%s' array (%p) bounds, skip setting pid for candidate %i.",aTca->GetName(),aTca,trackIndex );
      continue;
      check=kFALSE;
    }

    aProb= ( PndPidProbability* ) aTca->At ( trackIndex );

    if ( aProb == 0 ) {
      Error ( "Apply", "PID Probability object in array '%s' at %p not found, skip setting pid for candidate %i.",aTca->GetName(),aTca,trackIndex );
      continue;
      check=kFALSE;
    }

    if ( trackIndex!=aProb->GetIndex() ) {
      Error ( "Apply", "PID Probability object index (%i) is not the track index (%i). Is that bad?",aProb->GetIndex(),trackIndex );
      continue;
      check=kFALSE;
    }

    // catch Zeros to avoid NAN from Div/Zero
    if ( aProb->GetSumProb() == 0 ) {
      continue;
    }

    // avoid numerics with low numbers
    //aProb->NormalizeTo(1.);
    //now multiply
    *fPidResult *= *aProb;
  }
  //std::cout<<"PndAnaPidCombiner: ";  fPidResult->Print();
  //TODO renormalizing is done in the Pid object upon request ???
  //fPidResult->NormalizeTo(1.);
  //std::cout<<"PndAnaPisCombiner: scaled = ";  fPidResult->Print();
  // numbering see PndPidListMaker
  // No flux implemented! To come for each Detector!
  tc->SetPidInfo ( 0,fPidResult->GetElectronPidProb() );
  tc->SetPidInfo ( 1,fPidResult->GetMuonPidProb() );
  tc->SetPidInfo ( 2,fPidResult->GetPionPidProb() );
  tc->SetPidInfo ( 3,fPidResult->GetKaonPidProb() );
  tc->SetPidInfo ( 4,fPidResult->GetProtonPidProb() );
  //std::cout<<"PndAnaPidCombiner: "<<tc<<std::endl;

  return check;
}

void PndAnaPidCombiner::ApplyFlat ( RhoCandidate* tc )
{
  tc->SetPidInfo ( 0,0.2 );
  tc->SetPidInfo ( 1,0.2 );
  tc->SetPidInfo ( 2,0.2 );
  tc->SetPidInfo ( 3,0.2 );
  tc->SetPidInfo ( 4,0.2 );
  return;
}

void PndAnaPidCombiner::SetDefaults()
{
  // Set list of names and weights to the default PANDA
  //TString names = "PidAlgoMvd;PidAlgoStt;PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc;PidAlgoMdtHardCuts;";
  //TString names = "PidAlgoIdealCharged";
  //TString names = "PidMvaChargedProbability";
  //SetTcaNames ( names );

  TString dummy("");
  SetTcaNames(dummy,dummy);
  return;
}

void PndAnaPidCombiner::SetTcaNames ( TString& names, TString postfix )
{
  fCurrentPidArrays.clear();
  // Tokenizer, cool thingy!
  TStringToken list ( names,";" );
  //use TString class part (inherited, Tokenizer stores data there)
  while ( list.NextToken() )
  {
    TString branch=(TString)list;
    if (branch == "") continue;
    TString full=branch+postfix;
    if (!fPidArrays[full])
    {
      //std::cout<<" -I- PndAnaPidCombiner::SetTcaNames(): try finding \""<<full.Data()<<"\""<<std::endl;
      TClonesArray * tmpar = ReadTCA(full.Data());
      if(tmpar) {
        fPidArrays[full]=tmpar;
        fRootManager->ReadBranchEvent(full.Data());
        // we have the branch now, lets use it
        fCurrentPidArrays.push_back (full);
        //std::cout<<" -I- PndAnaPidCombiner::SetTcaNames(): \""<<branch.Data()<<"\" + \""<<postfix.Data()<<"\" = \""<<full.Data()<<"\""<<std::endl;
      } else {
        // now there is no branch with the full name, let's try without the PID postfix
        // TODO here we would need to select one other branch, if available, before going to fallback
        // Users may use Multikalman for protons and pions only, but want to reconstruct muons
        if (!fPidArrays[branch])
        {
          TClonesArray * tmpar2 = ReadTCA(branch.Data());
          if(tmpar2)
          {
            fPidArrays[branch]=tmpar;
            fRootManager->ReadBranchEvent(branch.Data());
            // we have the backup branch now, lets use it
            fCurrentPidArrays.push_back (branch);
            std::cout<<" WARNING TO ANALYST: PndAnaPidCombiner::Init() could not find "<< full.Data()<<" we use a backup branch: "<<branch.Data()<<std::endl;
          } else {
            // now we even don't have a backup
            std::cout<<" WARNING TO ANALYST: PndAnaPidCombiner::Init() could not find "<< full.Data()<<" nor a backup branch "<<branch.Data()<<std::endl;
          }
        } else {
          // we have the backup branch already loaded, lets use it
          fCurrentPidArrays.push_back (branch);
        }
      }
    } else {
      //std::cout<<" -I- PndAnaPidCombiner::SetTcaNames(): we have alreasy a cached TClonesArray named \""<<full.Data()<<"\""<<std::endl;
      // we have the branch already loaded, lets use it
      fCurrentPidArrays.push_back(full);
    }
  }

  //std::cout<<"PidCombiner initialized."<<std::endl;
}

TClonesArray* PndAnaPidCombiner::ReadTCA ( const TString& tcaname )
{
  // Fetch a TCLonesArray from the framework by its root name
  if (tcaname == "") {
    Warning ( "PndAnaPidCombiner::ReadTCA()","Empty TCA name." );
    return NULL;
  }
  //std::cout<<" -I- PndAnaPidCombiner::ReadTCA(): Try fetching branch "<<tcaname.Data()<<" rootmanager="<<fRootManager<<std::endl;
  TClonesArray* tca = ( TClonesArray* ) fRootManager->GetObject ( tcaname.Data() );

  if ( ! tca ) { // this information is already provoded by the RootManager
    //Warning ( "PndAnaPidCombiner::ReadTCA()","No \"%s\" array found.",tcaname.Data() );
    return NULL;
  }

  return tca;
}

