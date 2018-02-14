// $Id: PndCAGlobalPerformance.cxx,v 1.11 2010/08/18 20:46:09 ikulakov Exp $
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
// List of most recent changes:                                             *
//                                                                          *
// 29-05-17 Adjustment of initialization for PANDA data (Irina Rostovtseva) *
//***************************************************************************

#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE

#include "PndCACounters.h"

#include "PndCATrackPerformanceBase.h"
#include "PndCAGlobalPerformance.h"


#include "PndCAGBHit.h"
#include "PndCAMCTrack.h"
#ifndef HLTCA_STANDALONE
#include "PndCAMCPoint.h"
#endif
#include "PndCAGBTrack.h"
#include "PndCAGBTracker.h"

//#include "PndCADisplay.h"

#include "TMath.h"
#include "TROOT.h"
#include "Riostream.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TStyle.h"

void PndCAGlobalPerformance::SetNewEvent(const PndCAGBTracker * const tracker,
                            vector<PndCAHitLabel> *hitLabels,
                            vector<PndCAMCTrack> *mcTracks,
                            vector<PndCALocalMCPoint> *localMCPoints)
{
  PndCATrackPerformanceBase::SetNewEvent(tracker, hitLabels, mcTracks, localMCPoints);


  nRecoTracks = fTracker->NTracks();


} // void PndCAGlobalPerformance::SetNewEvent

void PndCAGlobalPerformance::CheckMCTracks()
{
  for ( int imc = 0; imc < nMCTracks; imc++ ) (*fMCTracks)[imc].SetNHits( 0 );
  cout<<"nHits "<<fTracker->NHits()<<" n labels "<<(*fHitLabels).size()<<endl;
  
  for ( int ih = 0; ih < fTracker->NHits(); ih++ ) { // TODO: do we need to calculate consequtive hits??
    const PndCAGBHit &hit = fTracker->Hit( ih );
    const PndCAHitLabel &l = (*fHitLabels)[hit.ID()];
    if( hit.IsLeft() ){
      cout<<" mirrored hit in array, something wrong!! "<<endl;
      continue;
    }
    //cout<<ih<<" "<<hit.ID()<<" "<<l.fLab[0]<<" "<<l.fLab[1]<<" "<<l.fLab[2]<<endl;
    if ( l.fLab[0] >= 0 ) (*fMCTracks)[l.fLab[0]].SetNHits( (*fMCTracks)[l.fLab[0]].NHits() + 1 );
    if ( l.fLab[1] >= 0 ) (*fMCTracks)[l.fLab[1]].SetNHits( (*fMCTracks)[l.fLab[1]].NHits() + 1 );
    if ( l.fLab[2] >= 0 ) (*fMCTracks)[l.fLab[2]].SetNHits( (*fMCTracks)[l.fLab[2]].NHits() + 1 );
  }
  mcData.resize(nMCTracks);
  for ( int imc = 0; imc < nMCTracks; imc++ ) {
    PndCAMCTrack &mc = (*fMCTracks)[imc];
    PndCAPerformanceMCTrackData &mcTrackData = mcData[imc];
    mc.SetSet( 0 );
    mc.SetNReconstructed( 0 );
    mc.SetNTurns( 1 );
    if ( mc.NHitRows() >= PParameters::MinimumHitsForMCTrack)
    //if ( mc.NHits() == mc.NHitRows() )
//    if ( mc.NMCPoints() >= PParameters::MinimumMCPointsForMCTrack )
    if ( mc.NMCRows() >= PParameters::MinimumMCPointsForMCTrack )
      mcTrackData.SetAsReconstructable();

     // sets of tracks 0-OutSet, 1-ExtraSet, 2-RefSet, 3-ExtraSecSet, 4-ExtraPrimSet, 5-RefSecSet, 6-RefPrimSet, 7-LongRefPrimSet
    if ( mc.P() >= PParameters::ExtraThreshold ) {
      if ( mc.P() >= PParameters::RefThreshold ) {
        mc.SetSet( 2 );
        mcTrackData.SetSet( 2 );
        if ( mc.MotherId() != -1 ) {
          mc.SetSet( 5 );
          mcTrackData.SetSet( 5 );
          
        }
        else {
          mc.SetSet( 6 );
          mcTrackData.SetSet( 6 );
          if ( mc.NHits() == fTracker->NStations() ) {
            mc.SetSet( 7 );
            mcTrackData.SetSet( 7 );
          }
        }
      }
      else{
        mc.SetSet( 1 );
        mcTrackData.SetSet( 1 );
        if ( mc.MotherId() != -1 ) {
          mc.SetSet( 3 );
          mcTrackData.SetSet( 3 );
          
        }
        else {
          mc.SetSet( 4 );
          mcTrackData.SetSet( 4 );
        }
      }
    }
  } // for iMC

} // void PndCAGlobalPerformance::CheckMCTracks()


void PndCAGlobalPerformance::MatchTracks()
{
  cout<<"n reco trcks: "<<nRecoTracks<<" "<<fTracker->NTracks()<<endl;
  recoData.resize(nRecoTracks);
  for ( int itr = 0; itr < nRecoTracks; itr++ ) {
    int traLabels = -1;
    double traPurity = 0;
    const PndCAGBTrack &tCA = fTracker->Track( itr );
    const int nhits = tCA.NHits();
    int *lb = new int[nhits*3];
    int nla = 0;
    for ( int ihit = 0; ihit < nhits; ihit++ ) {
      const int index = fTracker->TrackHit( tCA.FirstHitRef() + ihit );
      const PndCAHitLabel &l = (*fHitLabels)[fTracker->Hit( index ).ID()];
      if ( l.fLab[0] >= 0 ) lb[nla++] = l.fLab[0];
      if ( l.fLab[1] >= 0 ) lb[nla++] = l.fLab[1];
      if ( l.fLab[2] >= 0 ) lb[nla++] = l.fLab[2];
    }
    std::sort( lb, lb + nla );
    int labmax = -1, labcur = -1, lmax = 0, lcurr = 0;
    for ( int i = 0; i < nla; i++ ) {
      if ( lb[i] != labcur ) {
        if ( labcur >= 0 && lmax < lcurr ) {
          lmax = lcurr;
          labmax = labcur;
        }
        labcur = lb[i];
        lcurr = 0;
      }
      lcurr++;
    }
    if ( labcur >= 0 && lmax < lcurr ) {
      lmax = lcurr;
      labmax = labcur;
    }
    lmax = 0;
    for ( int ihit = 0; ihit < nhits; ihit++ ) {
      const int index = fTracker->TrackHit( tCA.FirstHitRef() + ihit );
      const PndCAHitLabel &l = (*fHitLabels)[fTracker->Hit( index ).ID()];
      if ( l.fLab[0] == labmax || l.fLab[1] == labmax || l.fLab[2] == labmax
         ) lmax++;
    }
    traLabels = labmax;
    traPurity = ( ( nhits > 0 ) ? double( lmax ) / double( nhits ) : 0 );
    if ( lb ) delete[] lb;

    recoData[itr].SetMCTrack(traLabels, traPurity, nhits);
    if ( recoData[itr].IsReco(PParameters::MinTrackPurity, PParameters::MinimumHitsForRecoTrack) ) mcData[traLabels].AddReconstructed(itr);
  } // for iReco
} // void PndCAGlobalPerformance::MatchTracks()


void PndCAGlobalPerformance::EfficiencyPerformance( )
{
  for ( int iRTr = 0; iRTr < nRecoTracks; iRTr++ ) {
    if (  recoData[iRTr].IsGhost(PParameters::MinTrackPurity) )
      fEff.ghosts++;
  }

  for ( int iMCTr = 0; iMCTr < nMCTracks; iMCTr++ ) {
    PndCAPerformanceMCTrackData &mc = mcData[iMCTr];
    if ( !mc.IsReconstructable() ) continue;
    const bool reco = mc.IsReconstructed();
    const int nclones = mc.GetNClones();

    bool killed = 0; // isn't used
    double ratio_length = 0;
    double ratio_fakes  = 0;
    const vector<int>& rTIds = mc.RecoTrackIds();
    if (reco){
      double mc_length = (*fMCTracks)[iMCTr].NMCRows();
      for (unsigned int irt = 0; irt < rTIds.size(); irt++) {
        PndCAPerformanceRecoTrackData& rd = recoData[rTIds[irt]];
        ratio_length += static_cast<double>( rd.NHits() )*rd.GetPurity() / mc_length;
        ratio_fakes += 1 - rd.GetPurity();
      }
    }
    
    if ( mc.GetSet() == 0){ // rest, out track
      fEff.Inc(reco, killed, ratio_length, ratio_fakes, nclones, "rest");
    }
    else{ // good
      fEff.Inc(reco, killed, ratio_length, ratio_fakes, nclones, "total");
      switch( mc.GetSet() ){
        case 1:
          fEff.Inc(reco, killed, ratio_length, ratio_fakes, nclones, "extra");
          break;
        case 2:
          fEff.Inc(reco, killed, ratio_length, ratio_fakes, nclones, "ref");
          break;
        case 3:
          fEff.Inc(reco, killed, ratio_length, ratio_fakes, nclones, "extra");
          fEff.Inc(reco, killed, ratio_length, ratio_fakes, nclones, "extra_sec");
          break;
        case 4:
          fEff.Inc(reco, killed, ratio_length, ratio_fakes, nclones, "extra");
          fEff.Inc(reco, killed, ratio_length, ratio_fakes, nclones, "extra_prim");
          break;
        case 5:
          fEff.Inc(reco, killed, ratio_length, ratio_fakes, nclones, "ref");
          fEff.Inc(reco, killed, ratio_length, ratio_fakes, nclones, "ref_sec");
          break;
        case 6:
          fEff.Inc(reco, killed, ratio_length, ratio_fakes, nclones, "ref");
          fEff.Inc(reco, killed, ratio_length, ratio_fakes, nclones, "ref_prim");
          break;
            break;
        case 7:
          fEff.Inc(reco, killed, ratio_length, ratio_fakes, nclones, "ref");
          fEff.Inc(reco, killed, ratio_length, ratio_fakes, nclones, "ref_prim");
          fEff.Inc(reco, killed, ratio_length, ratio_fakes, nclones, "ref_prim_long");
          break;
      }
    }
  } // for iMCTr
  PndCATrackPerformanceBase::EfficiencyPerformance();
} // void PndCAGlobalPerformance::EfficiencyPerformance( )

void PndCAGlobalPerformance::FillHistos()
{
  PndCATrackPerformanceBase::FillHistos();

    //

  const int NMCTracks = (*fMCTracks).size();
  vector<int> mcTrackNRecoHits;
  vector<int> nHitsVsRow;
  vector<int> nMCPointsVsRow;
  const int Multiplicity = (*fMCTracks).size();
  
  mcTrackNRecoHits.resize(NMCTracks, 0);
  nHitsVsRow.resize(fTracker->NStations());
  nMCPointsVsRow.resize(fTracker->NStations());
  for(int iH=0; iH < fTracker->NHits(); iH++){
    const PndCAGBHit &hit = fTracker->Hit( iH );
    
    nHitsVsRow[hit.IRow()]++;
    
    const PndCAHitLabel &l = (*fHitLabels)[hit.ID()];
    if ( l.fLab[0] >= 0 ) mcTrackNRecoHits[l.fLab[0]]++;
    if ( l.fLab[1] >= 0 ) mcTrackNRecoHits[l.fLab[1]]++;
    if ( l.fLab[2] >= 0 ) mcTrackNRecoHits[l.fLab[2]]++;
  }

  for(int i=0; i < NMCTracks; i++){
    PndCAMCTrack &mcT = (*fMCTracks)[i];
        
    GetHisto("mcTrackNRecoHits")->Fill( mcTrackNRecoHits[i] );
    GetHisto("nMCPointsVsMCMom")->Fill( mcT.P(), mcT.NMCPoints() );
    
    if ( mcT.NMCPoints() > 0 ) {
      double mcEx = mcT.Px();
      double mcEy = mcT.Py();
      double mcEz = mcT.Pz();
      double mcEt = TMath::Sqrt( mcEx * mcEx + mcEy * mcEy );

      const double dZdS = mcEz/mcEt;

      GetHisto("nHitsOverNMCPointsVsMCMom")->Fill( mcT.P(), float(mcTrackNRecoHits[i])/float(mcT.NMCPoints()) );
      GetHisto("nHitsOverNMCPointsVsMCDzDs")->Fill( dZdS, float(mcTrackNRecoHits[i])/float(mcT.NMCPoints()) );
    }
  }

  for(unsigned int i=0; i < (*fLocalMCPoints).size(); i++){
    nMCPointsVsRow[(*fLocalMCPoints)[i].IRow()]++;
  }
  for(int i=0; i < fTracker->NStations(); i++){
    if ( nMCPointsVsRow[i] > 0 ) {
      GetHisto("nHitsOverNMCPointsVsRow")->Fill( i, float(nHitsVsRow[i])/float(nMCPointsVsRow[i]) );
      GetHisto("nHitsOverNMCPointsVsNMCTracks")->Fill( Multiplicity, float(nHitsVsRow[i])/float(nMCPointsVsRow[i]) );
    }
  }

  for(int iRTr=0; iRTr < nRecoTracks; iRTr++){  // TODO: make common
    PndCAPerformanceRecoTrackData &recoD = recoData[iRTr];

    const PndCAGBTrack &recoTr = fTracker->Track( iRTr );  // TODO: make common
    PndCAMCTrack &mcTr = (*fMCTracks)[ recoD.GetMCTrackId() ];

#ifdef USE_CA_FIT // use 3 iterational fit, which ends on inner station
//    PndCATrackParam param = recoTr.OuterParam();
   PndCATrackParam param = recoTr.InnerParam();
#else
    PndCATrackParam param = recoTr.InnerParam();
//    PndCATrackParam param = recoTr.OuterParam();
#endif

    double RecoPt  = 1. / fabs(param.QPt());
    double RecoMom = RecoPt * sqrt(1. + param.DzDs()*param.DzDs());
      //     fNVsMom->Fill( param.GetY());
      //     fLengthVsMom->Fill( param.GetY(), t.NHits());
    double prob = param.GetChi2() != -1 ? TMath::Prob( param.GetChi2(), param.GetNDF() ) : -1;
    
    GetHisto("purity")->Fill( recoData[iRTr].GetPurity() );
    if (  recoD.IsGhost(PParameters::MinTrackPurity) ) {
      GetHisto("ghostsLength")->Fill( recoTr.NHits() );
      GetHisto("ghostsRMom")->Fill( RecoMom );
      GetHisto("ghostsMCMom")->Fill( mcTr.P() );
      GetHisto("ghostsRPt")->Fill( RecoPt );
      GetHisto("ghostsMCPt")->Fill( mcTr.Pt() );
      GetHisto("ghostsLengthAndMCMom")->Fill( recoTr.NHits(), mcTr.P() );
      GetHisto("ghostsLengthAndRMom")->Fill( recoTr.NHits(), RecoMom );
      GetHisto("ghostsChi2")->Fill( param.GetChi2() );
      GetHisto("ghostsProb")->Fill( prob );
    }
    else {
      GetHisto("recosLength")->Fill( recoTr.NHits() );
      GetHisto("recosRMom")->Fill( RecoMom );
      GetHisto("recosMCMom")->Fill( mcTr.P() );
      GetHisto("recosRPt")->Fill( RecoPt );
      GetHisto("recosMCPt")->Fill( mcTr.Pt() );
      GetHisto("recosLengthAndMCMom")->Fill( recoTr.NHits() , mcTr.P() );
      GetHisto("recosLengthAndRMom")->Fill( recoTr.NHits() , RecoMom );
      GetHisto("recosChi2")->Fill( param.GetChi2() );
      GetHisto("recosProb")->Fill( prob );
      if ( abs(mcTr.P()) > PParameters::RefThreshold )
        GetHisto("recosRefProb")->Fill( prob );
      if( mcTr.P() > 0.5 ) GetHisto("nHitsRecoTOverNHitsMCT")->Fill( float(recoTr.NHits()) / mcTrackNRecoHits[recoD.GetMCTrackId()] );
    }
  }  
} // void PndCAGlobalPerformance::FillHistos()

#endif //DO_TPCCATRACKER_EFF_PERFORMANCE

