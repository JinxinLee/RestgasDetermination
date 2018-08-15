// $Id: PndCAPerformanceBase.cxx,v 1.11 2010/08/26 15:05:50 ikulakov Exp $
// **************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: Sergey Gorbunov <sergey.gorbunov@kip.uni-heidelberg.de> *
//                  Ivan Kisel <kisel@kip.uni-heidelberg.de>                *
//                  for The ALICE HLT Project.                              *
//                                                                          *
// Developed by:   Igor Kulakov <I.Kulakov@gsi.de>                          *
//                 Maksym Zyzak <M.Zyzak@gsi.de>                            *
//                                                                          *
// Permission to use, copy, modify and distribute this software and its     *
// documentation strictly for non-commercial purposes is hereby granted     *
// without fee, provided that the above copyright notice appears in all     *
// copies and that both the copyright notice and this permission notice     *
// appear in the supporting documentation. The authors make no claims       *
// about the suitability of this software for any purpose. It is            *
// provided "as is" without express or implied warranty.                    *
//                                                                          *
//***************************************************************************
#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE

#include "PndCACounters.h"

#include "PndCAPerformanceBase.h"
#include "PndCAMCTrack.h"
#ifndef HLTCA_STANDALONE
#include "PndCAMCPoint.h"
#endif
#include "PndCAGBTrack.h"
#include "PndCAGBTracker.h"



#include "TMath.h"
#include "TROOT.h"
#include "Riostream.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TStyle.h"

#include <string>
using std::string;


void PndCAPerformanceBase::SetNewEvent( const PndCAGBTracker * const tracker,
                                vector<PndCAHitLabel> *hitLabels,
                                vector<PndCAMCTrack> *mcTracks,
                                vector<PndCALocalMCPoint> *localMCPoints)
{
  fTracker = tracker;
  
  fHitLabels = hitLabels;
  fMCTracks = mcTracks;
  fLocalMCPoints = localMCPoints;
  
  nMCTracks = (*fMCTracks).size();

  fEff = PndCAEfficiencies();
  mcData.resize(0);
  recoData.resize(0);
} // void PndCAPerformanceBase::SetNewEvent

PndCAPerformanceBase::PndCAPerformanceBase():
  fStatNEvents(0),NHisto(0),fTracker(0),fHitLabels(0),fMCTracks(0),fLocalMCPoints(0),nRecoTracks(0),nMCTracks(0),fHistoDir(0)
{

}

PndCAPerformanceBase::~PndCAPerformanceBase()
{
  if (fHistoDir == 0) // don't write in file
    for( int i = 0; i < NHisto; i++ ){
      if (fHistos[i]) delete fHistos[i];
    }
}


void PndCAPerformanceBase::Exec( bool PrintFlag )
{
  assert( fTracker != 0 );

    // Efficiency
  CheckMCTracks();
  MatchTracks();
  EfficiencyPerformance();
  if (PrintFlag) PrintEfficiency();
  
    // Histos
  FillHistos();  
  
  fStatNEvents++;
} // Exec


void PndCAPerformanceBase::EfficiencyPerformance() // TODO add common parts of code
{
  fEff.IncNEvents();
  fEffStat += fEff;
}


TH1 *PndCAPerformanceBase::GetHisto(const char* name)
{
  int iHisto;
  for (iHisto = 0; iHisto < NHisto; iHisto++){
    if (string(fHistosInfo[iHisto].name) == string(name)){
      break;
    };
  }

  assert ( (iHisto != NHisto) || (string("") == string(" wrong histo name ")) );
  if (iHisto == NHisto){
    cout << "ERROR: wrong histo name: " << name << endl;
    exit(1);
  }
  
  return fHistos[iHisto];
}

#endif //DO_TPCCATRACKER_EFF_PERFORMANCE

