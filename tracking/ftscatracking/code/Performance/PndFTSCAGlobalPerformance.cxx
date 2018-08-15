// $Id: PndFTSCAGlobalPerformance.cxx,v 1.11 2010/08/18 20:46:09 ikulakov Exp $
// **************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: Sergey Gorbunov <sergey.gorbunov@kip.uni-heidelberg.de> *
//                  Ivan Kisel <kisel@kip.uni-heidelberg.de>                *
//                  for The ALICE HLT Project.                              *
//                                                                          *
// Developed by:   Igor Kulakov <I.Kulakov@gsi.de>                          *
//                 Mykhailo Pugach <M.Pugach@gsi.de>                        *
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

#define USE_CA_FIT

#include "PndFTSCounters.h"

#include "PndFTSCAPerformance.h"
#include "PndFTSCATrackPerformanceBase.h"
#include "PndFTSCAGlobalPerformance.h"


#include "PndFTSCAGBHit.h"
#include "PndFTSCAMCTrack.h"
#ifndef HLTCA_STANDALONE
#include "PndFTSCAMCPoint.h"
#endif
#include "PndFTSCAGBTrack.h"
#include "PndFTSCAGBTracker.h"

#include "PndFTSCADisplay.h"

#ifdef DRAW
#define DRAW_GLOBALPERF
#endif

#ifdef DRAW_GLOBALPERF
#include "PndFTSCADisplay.h"
#include "PndFTSCAPerformance.h"
#endif

#include "TMath.h"
#include "TROOT.h"
#include "Riostream.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TStyle.h"

void PndFTSCAGlobalPerformance::SetNewEvent(const PndFTSCAGBTracker * const tracker,
                            PndFTSResizableArray<PndFTSCAHitLabel> *hitLabels,
                            PndFTSResizableArray<PndFTSCAMCTrack> *mcTracks,
                            PndFTSResizableArray<PndFTSCALocalMCPoint> *localMCPoints)
{
  PndFTSCATrackPerformanceBase::SetNewEvent(tracker, hitLabels, mcTracks, localMCPoints);


  nRecoTracks = fTracker->NTracks();


} // void PndFTSCAGlobalPerformance::SetNewEvent

void PndFTSCAGlobalPerformance::CheckMCTracks()
{

  for ( int imc = 0; imc < nMCTracks; imc++ ) (*fMCTracks)[imc].SetNHits( 0 );

  for ( int ih = 0; ih < fTracker->NHits(); ih++ ) { // TODO: do we need to calculate consequtive hits??
    const PndFTSCAGBHit &hit = fTracker->Hit( ih );
    const PndFTSCAHitLabel &l = (*fHitLabels)[hit.ID()];
    /*if( hit.IsLeft() ){
      cout<<" mirrored hit in array, something wrong!! "<<endl;
      continue;
    }*/
    //cout<<ih<<" "<<hit.ID()<<" "<<l<<endl;
    if ( l.fLab[0] >= 0 ) (*fMCTracks)[l.fLab[0]].SetNHits( (*fMCTracks)[l.fLab[0]].NHits() + 1 );
    if ( l.fLab[1] >= 0 ) (*fMCTracks)[l.fLab[1]].SetNHits( (*fMCTracks)[l.fLab[1]].NHits() + 1 );
    if ( l.fLab[2] >= 0 ) (*fMCTracks)[l.fLab[2]].SetNHits( (*fMCTracks)[l.fLab[2]].NHits() + 1 );
  }
  
  /*for ( int ih = 0; ih < (*fHitLabels).Size(); ih++ ) { // TODO: do we need to calculate consequtive hits??
    const PndFTSCAHitLabel &l = (*fHitLabels)[ih];
    if ( l.fLab[0] >= 0 ) (*fMCTracks)[l.fLab[0]].SetNHits( (*fMCTracks)[l.fLab[0]].NHits() + 1 );
    if ( l.fLab[1] >= 0 ) (*fMCTracks)[l.fLab[1]].SetNHits( (*fMCTracks)[l.fLab[1]].NHits() + 1 );
    if ( l.fLab[2] >= 0 ) (*fMCTracks)[l.fLab[2]].SetNHits( (*fMCTracks)[l.fLab[2]].NHits() + 1 );
  }
  */
  mcData.resize(nMCTracks);
  //cout<<"nMCTracks "<<nMCTracks<<endl;
  for ( int imc = 0; imc < nMCTracks; imc++ ) {
    PndFTSCAMCTrack &mc = (*fMCTracks)[imc];
    //cout<<"MCTrack PDG NHits x y "<<mc.PDG()<<" "<<mc.NHits()<<" "<<mc.X()<<" "<<mc.Y()<<endl;
    PndFTSCAPerformanceMCTrackData &mcTrackData = mcData[imc];
    mc.SetSet( 0 );
    mc.SetNReconstructed( 0 );
    mc.SetNTurns( 1 );
    if ( ( mc.NHitRows() >= PParameters::MinimumHitsForMCTrack ) &&
#ifndef DRIFT_TUBES
           // ( mc.NHits() == mc.NHitRows() ) &&
#else
         ( mc.NHitContRows() >= PParameters::MinimumHitsForMCTrack ) &&
         ( mc.NMCContRows() >= PParameters::MinimumMCPointsForMCTrack ) &&
#endif
           // ( mc.NMCPoints() >= PParameters::MinimumMCPointsForMCTrack ) &&
         ( mc.NMCRows() >= PParameters::MinimumMCPointsForMCTrack ) &&
         ( mc.IsForwardTrack() )

       )
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

} // void PndFTSCAGlobalPerformance::CheckMCTracks()


void PndFTSCAGlobalPerformance::MatchTracks()
{
  recoData.resize(nRecoTracks);
  for ( int itr = 0; itr < nRecoTracks; itr++ ) {
    int traLabels = -1;
    double traPurity = 0;
    const PndFTSCAGBTrack &tCA = fTracker->Track( itr );
    const int nhits = tCA.NHits();
    int *lb = new int[nhits*3];
    int nla = 0;
    for ( int ihit = 0; ihit < nhits; ihit++ ) {
      const int index = fTracker->TrackHit( tCA.FirstHitRef() + ihit );
      const PndFTSCAHitLabel &l = (*fHitLabels)[fTracker->Hit( index ).ID()];
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
      const PndFTSCAHitLabel &l = (*fHitLabels)[fTracker->Hit( index ).ID()];
      if ( l.fLab[0] == labmax || l.fLab[1] == labmax || l.fLab[2] == labmax
         ) lmax++;
    }
    traLabels = labmax;
    traPurity = ( ( nhits > 0 ) ? double( lmax ) / double( nhits ) : 0 );
    if ( lb ) delete[] lb;

    recoData[itr].SetMCTrack(traLabels, traPurity, nhits);
    if ( recoData[itr].IsReco(PParameters::MinTrackPurity, PParameters::MinimumHitsForRecoTrack) ) mcData[traLabels].AddReconstructed(itr);
  } // for iReco
} // void PndFTSCAGlobalPerformance::MatchTracks()


void PndFTSCAGlobalPerformance::EfficiencyPerformance( )
{
  for ( int iRTr = 0; iRTr < nRecoTracks; iRTr++ ) {
    if (  recoData[iRTr].IsGhost(PParameters::MinTrackPurity) )
      fEff.ghosts++;
  }

  for ( int iMCTr = 0; iMCTr < nMCTracks; iMCTr++ ) {
    PndFTSCAPerformanceMCTrackData &mc = mcData[iMCTr];
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
        PndFTSCAPerformanceRecoTrackData& rd = recoData[rTIds[irt]];
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
            break; ///??????
        case 7:
          fEff.Inc(reco, killed, ratio_length, ratio_fakes, nclones, "ref");
          fEff.Inc(reco, killed, ratio_length, ratio_fakes, nclones, "ref_prim");
          fEff.Inc(reco, killed, ratio_length, ratio_fakes, nclones, "ref_prim_long");
          break;
      }
    }
  } // for iMCTr

  PndFTSCATrackPerformanceBase::EfficiencyPerformance();
} // void PndFTSCAGlobalPerformance::EfficiencyPerformance( )

void PndFTSCAGlobalPerformance::FillHistos()
{
  PndFTSCATrackPerformanceBase::FillHistos();

  const int NMCTracks = (*fMCTracks).Size();
  vector<int> mcTrackNRecoHits;
  vector<int> nHitsVsRow;
  vector<int> nMCPointsVsRow;
  const int Multiplicity = (*fMCTracks).Size();
  
  mcTrackNRecoHits.resize(NMCTracks, 0);
  nHitsVsRow.resize(fTracker->NStations());
  nMCPointsVsRow.resize(fTracker->NStations());
  for(int iH=0; iH < fTracker->NHits(); iH++){
    const PndFTSCAGBHit &hit = fTracker->Hit( iH );
    
    nHitsVsRow[hit.IRow()]++;
    
    const PndFTSCAHitLabel &l = (*fHitLabels)[hit.ID()];
    if ( l.fLab[0] >= 0 ) mcTrackNRecoHits[l.fLab[0]]++;
    if ( l.fLab[1] >= 0 ) mcTrackNRecoHits[l.fLab[1]]++;
    if ( l.fLab[2] >= 0 ) mcTrackNRecoHits[l.fLab[2]]++;
  }
  for(int i=0; i < NMCTracks; i++){
    PndFTSCAMCTrack &mcT = (*fMCTracks)[i];
        
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

  for(int i=0; i < (*fLocalMCPoints).Size(); i++){
    nMCPointsVsRow[(*fLocalMCPoints)[i].IRow()]++;
  }
  for(int i=0; i < fTracker->NStations(); i++){
    if ( nMCPointsVsRow[i] > 0 ) {
      GetHisto("nHitsOverNMCPointsVsRow")->Fill( i, float(nHitsVsRow[i])/float(nMCPointsVsRow[i]) );
      GetHisto("nHitsOverNMCPointsVsNMCTracks")->Fill( Multiplicity, float(nHitsVsRow[i])/float(nMCPointsVsRow[i]) );
    }
  }

  for(int iRTr=0; iRTr < nRecoTracks; iRTr++){  // TODO: make common
    PndFTSCAPerformanceRecoTrackData &recoD = recoData[iRTr];

    const PndFTSCAGBTrack &recoTr = fTracker->Track( iRTr );  // TODO: make common
    PndFTSCAMCTrack &mcTr = (*fMCTracks)[ recoD.GetMCTrackId() ];

      // if ( mcTr.MotherId() == -1 ) continue;
      // if ( (*fMCTracks)[mcTr.MotherId()].PDG() != 310 ) continue;
      // if ( mcTr.NHits() != fTracker->NStations() ) continue;
      // if ( abs(mcTr.Pt()) < 1 ) continue;
      // if ( abs(mcTr.DzDs()) < 0.5 ) continue;
    
#ifdef USE_CA_FIT // use 3 iterational fit, which ends on inner station
//    PndFTSCATrackParam param = recoTr.OuterParam();
   PndFTSCATrackParam param = recoTr.InnerParam();
#else
    PndFTSCATrackParam param = recoTr.InnerParam();
//    PndFTSCATrackParam param = recoTr.OuterParam();
#endif

#if !defined(PANDA_FTS)
    double RecoPt  = 1. / fabs(param.QPt());
    double RecoMom = RecoPt * sqrt(1. + param.DzDs()*param.DzDs());
#endif
      //     fNVsMom->Fill( param.Y());
      //     fLengthVsMom->Fill( param.Y(), t.NHits());
    double prob = param.Chi2() != -1 ? TMath::Prob( param.Chi2(), param.NDF() ) : -1;
    
    GetHisto("purity")->Fill( recoData[iRTr].GetPurity() );
    if (  recoD.IsGhost(PParameters::MinTrackPurity) ) {
#if !defined(PANDA_FTS)
      GetHisto("ghostsRMom")->Fill( RecoMom );
      GetHisto("ghostsRPt")->Fill( RecoPt );
      GetHisto("ghostsMCPt")->Fill( mcTr.Pt() );
      GetHisto("ghostsLengthAndRMom")->Fill( recoTr.NHits(), RecoMom );
#endif
      GetHisto("ghostsLength")->Fill( recoTr.NHits() );
      GetHisto("ghostsMCMom")->Fill( mcTr.P() );
      GetHisto("ghostsLengthAndMCMom")->Fill( recoTr.NHits(), mcTr.P() );
      GetHisto("ghostsChi2")->Fill( param.Chi2() );
      GetHisto("ghostsProb")->Fill( prob );
    }
    else {
#if !defined(PANDA_FTS)
      GetHisto("recosRMom")->Fill( RecoMom );
      GetHisto("recosRPt")->Fill( RecoPt );
      GetHisto("recosMCPt")->Fill( mcTr.Pt() );
      GetHisto("recosLengthAndRMom")->Fill( recoTr.NHits() , RecoMom );
#endif
      GetHisto("recosLength")->Fill( recoTr.NHits() );
      GetHisto("recosMCMom")->Fill( mcTr.P() );
      GetHisto("recosLengthAndMCMom")->Fill( recoTr.NHits() , mcTr.P() );
//       GetHisto("recosChi2")->Fill( param.Chi2()/param.NDF() );
//       GetHisto("recosProb")->Fill( prob );
      if ( abs(mcTr.P()) > PParameters::RefThreshold )
        GetHisto("recosRefProb")->Fill( prob );
      if( mcTr.P() > 0.5 ) GetHisto("nHitsRecoTOverNHitsMCT")->Fill( float(recoTr.NHits()) / mcTrackNRecoHits[recoD.GetMCTrackId()] );
    }
  }


/*
 for(int iRTr=0; iRTr < nRecoTracks; iRTr++){  // TODO: make common
    PndFTSCAPerformanceRecoTrackData &recoD = recoData[iRTr];

    const PndFTSCAGBTrack &recoTr = fTracker->Track( iRTr );  // TODO: make common
    PndFTSCAMCTrack &mcTr = (*fMCTracks)[ recoD.GetMCTrackId() ];

      // if ( mcTr.MotherId() == -1 ) continue;
      // if ( (*fMCTracks)[mcTr.MotherId()].PDG() != 310 ) continue;
      // if ( mcTr.NHits() != fTracker->NStations() ) continue;
      // if ( abs(mcTr.Pt()) < 1 ) continue;
      // if ( abs(mcTr.DzDs()) < 0.5 ) continue;

*/
  
  // global tracker performance
  {
    //cout<<"checkpoint #0 \n";


    for ( int itr = 0; itr < nRecoTracks; itr++ ) {
      const int iMC = recoData[itr].GetMCTrackId();
      if ( recoData[itr].IsGhost(PParameters::MinTrackPurity) ) continue;
      PndFTSCAMCTrack &mc = (*fMCTracks)[iMC];

        // if ( mc.MotherId() == -1 ) continue;
        // if ( (*fMCTracks)[mc.MotherId()].PDG() != 310 ) continue;
        // if ( mc.NHits() != fTracker->NStations() ) continue;

#ifdef PANDA_FTS
////05.03          if ( abs(mc.P()) < 1 ) continue;
#endif
        // if ( abs(mc.DzDs()) < 0.5 ) continue;
      //cout<<"checkpoint #1 \n";
      int nFirstMC = mc.FirstMCPointID();
      int nMCPoints = mc.NMCPoints();

      PndFTSCALocalMCPoint *points = &((*fLocalMCPoints).Data()[nFirstMC]);

      const PndFTSCAGBTrack &t = fTracker->Track( itr );
#ifdef USE_CA_FIT
//      PndFTSCATrackParam p = t.OuterParam();
        PndFTSCATrackParam p = t.InnerParam();
#else
//      PndFTSCATrackParam p = t.OuterParam();
      PndFTSCATrackParam p = t.InnerParam();
#endif
////05.03        if( !p.IsValid() ) continue;
      //cout<<"checkpoint #2 \n";
      int nHits = t.NHits();
      if(nHits < 6) continue;
// int nHits = t.NHits();
// std::cout << "hits:" << std::endl;
// for(int iH=0; iH<nHits; iH++)
//   std::cout << fTracker->Hit( fTracker->TrackHit(t.FirstHitRef() + iH) ).X() << " " <<
//                fTracker->Hit( fTracker->TrackHit(t.FirstHitRef() + iH) ).Y() << " " << 
//                fTracker->Hit( fTracker->TrackHit(t.FirstHitRef() + iH) ).Z() << " " << std::endl; 
// std::cout << "MCPoints:" << std::endl;
// for(int iMCPoint=0; iMCPoint<nMCPoints; iMCPoint++)
//   std::cout << points[iMCPoint].X() << " " <<
//                points[iMCPoint].Y() << " " << 
//                points[iMCPoint].Z() << " " << std::endl; 
// 
// std::cout << "track:" << std::endl;
// std::cout << p.X() << " " << p.Y() << " " << p.Z() << std::endl;


  //21.09 take mc-value for the last hit
  const int hitIndex = fTracker->TrackHit( t.FirstHitRef() /*+nHits-1*/ );

  const PndFTSCAGBHit &hit = fTracker->Hit( hitIndex );

  //   int trackID = recoD.GetMCTrackId();

  int trackID = iMC;
  //cout << " iMC  " << trackID << "  hit. Track_ID " <<   hit.Track_ID << endl;

  if (trackID!=hit.Track_ID) continue;


  int nHits_track = t.NHits();

  //cout << "nHits_track " << nHits_track << endl;

  if (nHits_track < 22) continue;

  float Tx_mc = hit.point_Px/hit.point_Pz;
  float Ty_mc = hit.point_Py/hit.point_Pz;
  
  /*
  cout << "  p.Chi2() " << p.Chi2() << " p.NDF() " << p.NDF() << endl;
  cout<<"p.X1() p.X2() p.Tx1() p.Tx2() p.QP()"<<p.X1()<<" "<<p.X2()<<" "<<p.Tx1()<<" "<<p.Tx2()<<" " << p.QP() << endl;
  cout << " hit.point_X " << hit.point_X <<  " hit.point_Y " << hit.point_Y << " Tx_mc " << Tx_mc <<  endl;
  cout << " p.X1() - hit.point_X " << p.X1() - hit.point_X << endl;
  cout << " p.Tx1() - Tx_mc   " << p.Tx1() - Tx_mc  << endl;*/

  if(p.Chi2() < 0 || p.NDF() <= 0) continue;


  //cout << "p.Chi2() " << p.Chi2() << " p.NDF() " << p.NDF() << endl; 
  while ( true ) {
     
	//GetHisto("resX1")->Fill( p.X1() - mcX1 );

	GetHisto("resX1")->Fill( p.X1() - hit.point_X );
//       if ( p.Err2X1() > 0 ) GetHisto("pullX1")->Fill( ( p.X1() - mcX1 ) / TMath::Sqrt( p.Err2X1() ) );
	if ( p.Err2X1() > 0 ) 
	  GetHisto("pullX1")->Fill( ( p.X1() - hit.point_X ) / TMath::Sqrt( p.Err2X1() ) );
	
       GetHisto("resX2")->Fill( p.X2() - hit.point_Y );
       if ( p.Err2X2() > 0 ) 
	 GetHisto("pullX2")->Fill( ( p.X2() - hit.point_Y ) / TMath::Sqrt( p.Err2X2() ) );

#ifdef PANDA_FTS
       // double qP = -p.QP(); // TODO: undestand why
       double qP = p.QP(); // TODO: undestand why

//       GetHisto("resTx1")->Fill( p.Tx1() - mcTx1 );

       GetHisto("resTx1")->Fill( p.Tx1() - Tx_mc );       
       GetHisto("resTx2")->Fill( p.Tx2() - Ty_mc );
       if(CAMath::Abs(qP) > 1.e-7)
       {
         GetHisto("resP")->Fill( (qP - hit.point_Qp)/hit.point_Qp );
       }
       //compare in terminal the mc and reco parameter values (dbg)
       /*cout<<"p.X1() p.X2() p.Tx1() p.Tx2() p.QP()"<<p.X1()<<" "<<p.X2()<<" "<<p.Tx1()<<" "<<p.Tx2()<<" " << p.QP() << endl;
       cout<<"mcX mcY mcTx mcTy mcQP "<<hit.point_X<<" "<<hit.point_Y<<" "<<Tx_mc<<" "<<Ty_mc<<" " << hit.point_Qp << endl;
       cout << "p.X1() - mcX " << p.X1() - hit.point_X << endl;
       cout << "p.Tx1() - mcTx " << p.Tx1() - Tx_mc << endl;*/

//        cout << "err " <<TMath::Sqrt( p.Err2X1())<< " " <<TMath::Sqrt( p.Err2X2()) <<" "<<  TMath::Sqrt( p.Err2Tx1() ) << " " << TMath::Sqrt( p.Err2Tx2() ) << " " << 
//        TMath::Sqrt(p.Err2QP()) << std::endl;
       
//       if ( p.Err2Tx1() > 0 ) GetHisto("pullTx1")->Fill( ( p.Tx1() - mcTx1 ) / TMath::Sqrt( p.Err2Tx1() ) );

      if ( p.Err2Tx1() > 0 ) 
	GetHisto("pullTx1")->Fill( ( p.Tx1() - Tx_mc) / TMath::Sqrt( p.Err2Tx1() ) );

       if ( p.Err2Tx2() > 0 ) 
	 GetHisto("pullTx2")->Fill( ( p.Tx2() - Ty_mc ) / TMath::Sqrt( p.Err2Tx2() ) );
       if(CAMath::Abs(qP) > 1.e-7 && p.Err2QP() > 0 ) 
	 GetHisto("pullQP")->Fill( (qP - hit.point_Qp)/TMath::Sqrt(p.Err2QP()) );

	if ( p.Chi2() < 1.e-7 ) continue;


       double prob = p.Chi2() != -1 ? TMath::Prob( p.Chi2(), p.NDF() ) : -1;
       GetHisto("recosChi2")->Fill( p.Chi2()/p.NDF() );
       GetHisto("recosProb")->Fill( prob );
#else // PANDA_FTS
              
       double qPt = p.QPt(); 

       GetHisto("resSinPhi")->Fill( p.SinPhi() - mcSinPhi );
       GetHisto("resDzDs")->Fill( p.DzDs() - mcDzDs );
       if(CAMath::Abs(qPt) > 1.e-7){
        GetHisto("resPt")->Fill( (qPt - mcQPt)/mcQPt );
        GetHisto("resPtVsP")->Fill( TMath::Abs(mcQP), (1.f/qPt - 1.f/mcQPt)*mcQPt );
        GetHisto("resPVsP")->Fill( TMath::Abs(mcQP), (TMath::Abs(1.f/p.QP()) - TMath::Abs(1.f/mcQP))*TMath::Abs(mcQP) );
          //          cout << setprecision(5) << qPt << " " << mcQPt  << " " << TMath::Sqrt(p.Err2QPt()) << endl;
      }

       if ( p.Err2SinPhi() > 0 ) GetHisto("pullSinPhi")->Fill( ( p.SinPhi() - mcSinPhi ) / TMath::Sqrt( p.Err2SinPhi() ) );
       if ( p.Err2DzDs() > 0 ) GetHisto("pullDzDs")->Fill( ( p.DzDs() - mcDzDs ) / TMath::Sqrt( p.Err2DzDs() ) );
       if(CAMath::Abs(qPt) > 1.e-7 && p.Err2QPt()>0 ) GetHisto("pullQPt")->Fill( (qPt - mcQPt)/TMath::Sqrt(p.Err2QPt()) );
#endif // PANDA_FTS
        
       break;
      }
    }

  }

    // hits pulls
  { // TODO
    const int nHits = fTracker->NHits();
    for ( int ih = 0; ih < nHits; ih++ ) {
      const PndFTSCAGBHit &hit = fTracker->Hit( ih );
      float x0HLoc, x1HLoc, x2HLoc;
      PndFTSCAParameters::GlobalToCALocal( hit.X(), hit.Y(), hit.Z(), hit.Angle(), x0HLoc, x1HLoc, x2HLoc );
        
      const int iMCP = PndFTSCAPerformance::Instance().GetMCPoint(hit);
      if (iMCP == -1) continue;
      const PndFTSCALocalMCPoint& point = fLocalMCPoints->Data()[iMCP];

      float x0PLoc, x1PLoc, x2PLoc;
      PndFTSCAParameters::GlobalToCALocal( point.X(), point.Y(), point.Z(), hit.Angle(), x0PLoc, x1PLoc, x2PLoc );
      float px0PLoc, px1PLoc, px2PLoc;
      PndFTSCAParameters::GlobalToCALocal( point.Px(), point.Py(), point.Pz(), hit.Angle(), px0PLoc, px1PLoc, px2PLoc );
      const float t1 = px1PLoc/px0PLoc, t2 = px2PLoc/px0PLoc;
      x1PLoc += t1*( x0HLoc - x0PLoc );
      x2PLoc += t2*( x0HLoc - x0PLoc );
      x0PLoc =  x0HLoc;
      
      GetHisto("resXHit")->Fill( x0HLoc - x0PLoc );
      GetHisto("resYHit")->Fill( x1HLoc - x1PLoc );
      GetHisto("resZHit")->Fill( x2HLoc - x2PLoc );
      GetHisto("resXHitVsZ")->Fill( hit.Z(), x0HLoc - x0PLoc );
      GetHisto("resYHitVsZ")->Fill( hit.Z(), x1HLoc - x1PLoc );
      GetHisto("resZHitVsZ")->Fill( hit.Z(), x2HLoc - x2PLoc );
      GetHisto("resXHitVsX")->Fill( hit.X(), x0HLoc - x0PLoc );
      GetHisto("resYHitVsX")->Fill( hit.X(), x1HLoc - x1PLoc );
      GetHisto("resZHitVsX")->Fill( hit.X(), x2HLoc - x2PLoc );

      GetHisto("xMCPoint")->Fill( point.X() );
      GetHisto("rMCPoint")->Fill( sqrt(point.Y()*point.Y() + point.X()*point.X()) );

        // get errors

      // GetHisto("pullXHit")->Fill( hit.X() - point.X() );
      GetHisto("pullYHit")->Fill( (x1HLoc - x1PLoc)/sqrt(hit.Err2X1()) );
      GetHisto("pullZHit")->Fill( (x2HLoc - x2PLoc)/sqrt(hit.Err2X2()) );

#ifdef DRIFT_TUBES
      const float dist = hit.DistanceFromWireToPoint( point.X(), point.Y(), point.Z() );
      GetHisto("resRHit")->Fill( hit.R() - dist );
      GetHisto("pullRHit")->Fill( (hit.R() - dist)/sqrt(hit.Err2R()) );
#endif
    }
  }







} // void PndFTSCAGlobalPerformance::FillHistos()

void PndFTSCAGlobalPerformance::Draw()
{
#ifdef DRAW_GLOBALPERF
  // PndFTSCAPerformance::Instance().Init();
  PndFTSCAPerformance& gbPerfo = PndFTSCAPerformance::Instance();
  PndFTSCADisplay &disp = PndFTSCADisplay::Instance();
  // disp.SetGB( gbPerfo.GetTracker() );
  disp.ClearView();
  disp.SetTPC( fTracker->GetParameters() );
  //disp.DrawTPC();
  disp.DrawGBHits( *gbPerfo.GetTracker(), kGreen+2, 0.8 );

  for ( int imc = 0; imc < nMCTracks; imc++ ) {
    PndFTSCAPerformanceMCTrackData &mc = mcData[imc];
    bool doDraw = true;
#if !(defined(PANDA_STT) || defined(PANDA_FTS))
    //doDraw &= (mc.GetSet() == 7); // long ref prim
    doDraw &= (*fMCTracks)[imc].P() > 1;
    //doDraw &= mc.IsReconstructable();
      // doDraw &= mc.IsReconstructed();
    if ( doDraw ) disp.DrawMCTrack( imc, kRed+2, 0 );
#else
    doDraw &= mc.IsReconstructable();
    if ( doDraw ) disp.DrawMCTrack( imc, kRed+2, 0 );
#endif
  }

  cout << "NRecoTracks = " <<  nRecoTracks << endl;
   for( int iT = 0; iT < nRecoTracks; ++iT ) {
    PndFTSCAPerformanceRecoTrackData &r = recoData[iT];
    const bool IsGhost = r.IsGhost(PParameters::MinTrackPurity);
    if ( IsGhost ) {
      const int imc = r.GetMCTrackId();
      PndFTSCAPerformanceMCTrackData &mc = mcData[imc];
      UNUSED_PARAM1(mc);
      bool doDraw = true;
#if !(defined(PANDA_STT) || defined(PANDA_FTS))
      //doDraw &= (mc.GetSet() == 7); // long ref prim
      //doDraw &= (*fMCTracks)[imc].P() > 1;
      //doDraw &= mc.IsReconstructable();
      if ( doDraw ) disp.DrawRecoTrack( iT, kGreen+1, 0 );

#else
      //doDraw &= (mc.GetSet() == 7); // long ref prim
      //doDraw &= (*fMCTracks)[imc].P() > 1;
      //doDraw &= mc.IsReconstructable();
      if ( doDraw ) disp.DrawRecoTrack( iT, kGreen+1, 0.4 );
#endif
      //disp.Ask();
    }
    else {
      const int imc = r.GetMCTrackId();
      PndFTSCAPerformanceMCTrackData &mc = mcData[imc];
      UNUSED_PARAM1(mc);
      bool doDraw = true;
        //doDraw &= (mc.GetSet() == 7); // long ref prim
#if !(defined(PANDA_STT) || defined(PANDA_FTS))
      doDraw &= (*fMCTracks)[imc].P() > 1;
      //doDraw &= mc.IsReconstructable();
        // doDraw &= mc.IsReconstructed();
      if ( doDraw ) disp.DrawRecoTrack( iT, kBlue+1, 0 );
#else
      //doDraw &= (*fMCTracks)[imc].P() > 1;
      //doDraw &= mc.IsReconstructable();
        // doDraw &= mc.IsReconstructed();
      if ( doDraw ) disp.DrawRecoTrack( iT, kGreen+1, 0.4 );
#endif
    }
  } 

  disp.SaveCanvasToFile( "DrawGlobalPerformance.pdf" );
  disp.Ask();
  
#endif // DRAW_GLOBALPERF
} // void PndFTSCAGlobalPerformance::Draw()

#endif //DO_TPCCATRACKER_EFF_PERFORMANCE

