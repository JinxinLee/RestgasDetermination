// **************************************************************************
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

#include "PndFTSTopoPerformance.h"

#include "PndFTSCounters.h"

#include "PndFTSCATrackPerformanceBase.h"
#include "PndFTSCAPerformance.h"
#include "PndFTSCAGlobalPerformance.h"

#ifdef DRAW
#include "PndFTSCADisplay.h"
#define MAIN_DRAW
#endif //DRAW

#include "PndFTSCAGBHit.h"
#include "PndFTSCAMCTrack.h"
#ifndef HLTCA_STANDALONE
#include "PndFTSCAMCPoint.h"
#endif

#include "PndFTSCAGBTrack.h"
#include "PndFTSCAGBTracker.h"
#include "PndFTSCAGBTracker.h"
#include "PndFTSTopoReconstructor.h"

#include "PndFTSCADisplay.h"

#include "KFParticleSIMD.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"

#include "TMath.h"
#include "TROOT.h"
#include "Riostream.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TStyle.h"

PndFTSTopoPerformance::PndFTSTopoPerformance():fTopoReconstructor(0),fPrimVertices(),fFindParticlesMode(3)
{
}

void PndFTSTopoPerformance::SetNewEvent(
                            const PndFTSCAGBTracker * const tracker,
                            PndFTSResizableArray<PndFTSCAHitLabel> *hitLabels,
                            PndFTSResizableArray<PndFTSCAMCTrack> *mcTracks,
                            PndFTSResizableArray<PndFTSCALocalMCPoint> *localMCPoints)
{
  assert( PndFTSCAPerformance::Instance().GetSubPerformance("Global Performance") != 0 );
  
  PndFTSParticlePerformanceBase::SetNewEvent(tracker, hitLabels, mcTracks, localMCPoints);

  nRecoTracks = fTracker->NTracks();
} // void PndFTSTopoPerformance::SetNewEvent

void PndFTSTopoPerformance::SetNewEvent2( const PndFTSTopoReconstructor * const TopoReconstructor)
{
  assert( fTracker != 0 ); // should be called after SetNewEvent
  
  fTopoReconstructor = TopoReconstructor;
} // void PndFTSTopoPerformance::SetNewEvent2

void PndFTSTopoPerformance::CheckMCTracks()
{
  mcData = PndFTSCAPerformance::Instance().GetSubPerformance("Global Performance")->GetMCData();

    // find prim vertex
  PndFTSCAMCTrack *iPrimMC = (*fMCTracks).Data();
  for( ;(iPrimMC->MotherId() != -1) && (iPrimMC < (*fMCTracks).Data() + nMCTracks); iPrimMC++);
  if (iPrimMC < (*fMCTracks).Data() + nMCTracks) {
    PndFTSCAMCVertex primVertex;
    primVertex.SetX( iPrimMC->X() );
    primVertex.SetY( iPrimMC->Y() );
    primVertex.SetZ( iPrimMC->Z() );
    fPrimVertices.push_back(primVertex);
  }
} // void PndFTSTopoPerformance::CheckMCTracks()

void PndFTSTopoPerformance::GetMCParticles()
{
  // convert MC tracks into KF MC Particles

  vMCParticles.clear();

  // all MC tracks are copied into KF MC Particles
  for(int iMC=0; iMC < nMCTracks; iMC++)
  {
    PndFTSCAMCTrack &mtra = (*fMCTracks)[iMC];
    KFMCParticle part;
    part.SetMCTrackID( iMC );
    part.SetMotherId ( mtra.MotherId() );
    part.SetPDG      ( mtra.PDG() );
    vMCParticles.push_back( part );
  }
    // find relations between mother and daughter MC particles
  const unsigned int nMCParticles = vMCParticles.size();
  for ( unsigned int iP = 0; iP < nMCParticles; iP++ ) {
    KFMCParticle &part = vMCParticles[iP];
    for(unsigned int iP2 = 0; iP2 < nMCParticles; iP2++) {
      KFMCParticle &part2 = vMCParticles[iP2];

      if(part.GetMotherId() == part2.GetMCTrackID()) {
        part2.AddDaughter(iP);
      }
    }
  }

  for(unsigned int iMC=0; iMC < vMCParticles.size(); iMC++)
  {
    KFMCParticle &part = vMCParticles[iMC];
    part.SetMCTrackID( iMC );
//     if(part.NDaughters() > 0 && part.GetPDG() == 310)
//     {
//       std::cout << iMC << " " << part.GetPDG() << " "<<std::endl;
//       for(int iD=0; iD<part.NDaughters(); iD++)
//       {
//         int dId = part.GetDaughterIds()[iD];
//         PndFTSCAMCTrack &mtra = (*fMCTracks)[vMCParticles[dId].GetMCTrackID()];
//         std::cout << "   d" <<iD<<" " << dId << " " << vMCParticles[dId].GetPDG() <<" " << mtra.X() << " " << mtra.Y() << " " << mtra.Z() << " " << mtra.Px() << " " << mtra.Py() << " " << mtra.Pz() <<std::endl;;
//       }
//     }
  }
}

void PndFTSTopoPerformance::FindReconstructableMCParticles()
{
  const unsigned int nMCParticles = vMCParticles.size();

  for ( unsigned int iP = 0; iP < nMCParticles; iP++ ) {
    KFMCParticle &part = vMCParticles[iP];
    CheckMCParticleIsReconstructable(part);
  }
}

void PndFTSTopoPerformance::CheckMCParticleIsReconstructable(KFMCParticle &part)
{
  if ( part.IsReconstructable() ) return;

    // tracks
  if ( /*part.NDaughters() == 0*/ part.GetPDG() ==  -211 ||
                                  part.GetPDG() ==   211 ||
                                  part.GetPDG() ==  2212 ||
                                  part.GetPDG() == -2212 ||
                                  part.GetPDG() ==   321 ||
                                  part.GetPDG() ==  -321 ||
                                  part.GetPDG() ==    11 ||
                                  part.GetPDG() ==   -11 ||
                                  part.GetPDG() ==    13 ||
                                  part.GetPDG() ==   -13 ) { // TODO other particles

    switch ( fFindParticlesMode ) {
      case 1:
        part.SetAsReconstructable();
        break;
      case 2:
        {
          int iMCPart = part.GetMCTrackID();
          PndFTSCAPerformanceMCTrackData &mc = 
            PndFTSCAPerformance::Instance().GetSubPerformance("Global Performance")->GetMCData()[iMCPart];

          if(mc.IsReconstructable())
            part.SetAsReconstructable();
        }
        break;
      case 3:
        {
          int iMCPart = part.GetMCTrackID();

          PndFTSCAPerformanceMCTrackData &mc = 
            PndFTSCAPerformance::Instance().GetSubPerformance("Global Performance")->GetMCData()[iMCPart];

          if(mc.IsReconstructed())
            part.SetAsReconstructable();
        }
        break;
      default:
        part.SetAsReconstructable();
    };
  }
    //  mother particles
  else
  {
    for(int iPart=0; iPart<fParteff.nParticles; iPart++)
    {
      if(part.GetPDG() == fParteff.partPDG[iPart])
      {
        const unsigned int nDaughters = fParteff.partDaughterPdg[iPart].size();
        if( part.GetDaughterIds().size() != nDaughters ) return;
        vector<int> pdg(nDaughters);

        for(unsigned int iD=0; iD<nDaughters; iD++)
          pdg[iD] = vMCParticles[part.GetDaughterIds()[iD]].GetPDG();

        vector<bool> isDaughterFound(nDaughters);
        for(unsigned int iDMC=0; iDMC<nDaughters; iDMC++)
          isDaughterFound[iDMC] = 0;

        bool isReco = 1;
        for(unsigned int iDMC=0; iDMC<nDaughters; iDMC++)
          for(unsigned int iD=0; iD<nDaughters; iD++)
            if(pdg[iD] == fParteff.partDaughterPdg[iPart][iDMC]) isDaughterFound[iDMC] = 1;

        for(unsigned int iDMC=0; iDMC<nDaughters; iDMC++)
          isReco = isReco && isDaughterFound[iDMC];

        if(!isReco) return;
      }
    }


    const vector<int>& dIds = part.GetDaughterIds();
    const unsigned int nD = dIds.size();
    bool reco = 1;
    for ( unsigned int iD = 0; iD < nD && reco; iD++ ) {
      KFMCParticle &dp = vMCParticles[dIds[iD]];
      CheckMCParticleIsReconstructable(dp);
      reco &= dp.IsReconstructable();
    }
    if (reco) part.SetAsReconstructable();
  }
}

void PndFTSTopoPerformance::MatchParticles()
{
    // get all reco particles ( temp )
  MCtoRParticleId.clear();
  RtoMCParticleId.clear();
  MCtoRParticleId.resize(vMCParticles.size());
  RtoMCParticleId.resize(fTopoReconstructor->GetParticles().size() );
  
    // match tracks ( particles which are direct copy of tracks )
  for( unsigned int iRP = 0; iRP < fTopoReconstructor->GetParticles().size(); iRP++ ) {
//    CbmKFParticle &rPart = fTopoReconstructor->GetParticles()[iRP];
    const KFParticle &rPart = fTopoReconstructor->GetParticles()[iRP];

    if (rPart.NDaughters() != 1) continue;
    
    const int rTrackId = rPart.DaughterIds()[0];
    const int mcTrackId = recoData[rTrackId].GetMCTrackId();

// std::cout << "reco " << rPart.X() << " " << rPart.Y() << " " << rPart.Z() << " " << rPart.Px() << " " << rPart.Py() << " " << rPart.Pz() <<" " << rPart.Q()<< std::endl;
// 
// PndFTSCAMCTrack mcTrack = (*PndFTSCAPerformance::Instance().GetSubPerformance("Global Performance")->fMCTracks)[mcTrackId];
// 
//   PndFTSCALocalMCPoint *points = &((*(PndFTSCAPerformance::Instance().GetSubPerformance("Global Performance")->fLocalMCPoints)).Data()[mcTrack.FirstMCPointID()]);
// 
// std::cout << "mc " << points[0].X() << " " << points[0].Y() << " " << points[0].Z() << " " << points[0].Px() << " " << points[0].Py() << " " << points[0].Pz() <<" " << mcTrack.PDG()<< std::endl;

// #ifdef DRAW
//   if( mcTrackId == 2346 || mcTrackId == 2347 )
//   {
//     if(iRP==0)
//     {
//       PndFTSCADisplay::Instance().ClearView();
//       PndFTSCADisplay::Instance().SetTPCView();
//       PndFTSCADisplay::Instance().DrawTPC();
//     }
// 
//     PndFTSCADisplay::Instance().DrawGBTrack( rTrackId, kBlue, 2. );
// 
//     PndFTSCADisplay::Instance().SpecDrawMCTrackPointsGlobal( (*PndFTSCAPerformance::Instance().GetSubPerformance("Global Performance")->fMCTracks)[mcTrackId], PndFTSCAPerformance::Instance().GetSubPerformance("Global Performance")->fLocalMCPoints, kRed, 0.3 );
// 
//     PndFTSCADisplay::Instance().DrawGBPoint( rPart.X(), rPart.Y(), rPart.Z(), kGreen );
//     KFParticle rPart1 = rPart;
//     const Double_t vtx[3] = {0,0,18};
//     rPart1.TransportToPoint(vtx);
//     PndFTSCADisplay::Instance().DrawGBPoint( rPart1.X(), rPart1.Y(), rPart1.Z(), kMagenta );
//     const KFParticle &mPart = fTopoReconstructor->GetParticles()[2];
//     PndFTSCADisplay::Instance().DrawGBPoint( mPart.X(), mPart.Y(), mPart.Z(), kOrange );
// 
//     PndFTSCADisplay::Instance().Ask();
//   }
// #endif

    if ( recoData[rTrackId].IsGhost(PParameters::MinTrackPurity) ) continue;

    for ( unsigned int iMP = 0; iMP < vMCParticles.size(); iMP++ ) {
      KFMCParticle &mPart = vMCParticles[iMP];
      if ( mPart.GetMCTrackID() == mcTrackId ) { // match is found
        if( mPart.GetPDG() == rPart.GetPDG() ) {
          MCtoRParticleId[iMP].ids.push_back(iRP);
          RtoMCParticleId[iRP].ids.push_back(iMP);
        }
        else {
          MCtoRParticleId[iMP].idsMI.push_back(iRP);
          RtoMCParticleId[iRP].idsMI.push_back(iMP);
        }
// KFMCParticle &mmPart = vMCParticles[mPart.GetMotherId()];
// if(mmPart.GetPDG()==310) std::cout << "!!!!! " << rTrackId << " " << iMP << " " << mcTrackId<< std::endl;
      }
    }
  }

    // match created mother particles
  for( unsigned int iRP = 0; iRP < fTopoReconstructor->GetParticles().size(); iRP++ ) {
//    CbmKFParticle &rPart = fTopoReconstructor->GetParticles()[iRP];
    const KFParticle &rPart = fTopoReconstructor->GetParticles()[iRP];
    const unsigned int NRDaughters = rPart.NDaughters();
    if (NRDaughters < 2) continue;
    
    unsigned int iD = 0;
    int mmId = -2; // mother MC id
    {
      const int rdId = rPart.DaughterIds()[iD];
      if ( !RtoMCParticleId[rdId].IsMatched() ) continue;
      const int mdId = RtoMCParticleId[rdId].GetBestMatch();
      mmId = vMCParticles[mdId].GetMotherId();
    }
    iD++;
    for ( ; iD < NRDaughters; iD++ ) {
      const int rdId = rPart.DaughterIds()[iD];
      if ( !RtoMCParticleId[rdId].IsMatched() ) break;
      const int mdId = RtoMCParticleId[rdId].GetBestMatch();
      if( vMCParticles[mdId].GetMotherId() != mmId ) break;
    }
    if ( iD == NRDaughters && mmId > -1 ) { // match is found and it is not primary vertex
      KFMCParticle &mmPart = vMCParticles[mmId];
      if( mmPart.GetPDG()     == rPart.GetPDG()     &&
          mmPart.NDaughters() == rPart.NDaughters() ) {
        MCtoRParticleId[mmId].ids.push_back(iRP);
        RtoMCParticleId[iRP].ids.push_back(mmId);
      }
      else {
        MCtoRParticleId[mmId].idsMI.push_back(iRP);
        RtoMCParticleId[iRP].idsMI.push_back(mmId);
      }
    }
  }
}

void PndFTSTopoPerformance::MatchTracks()
{
  recoData = PndFTSCAPerformance::Instance().GetSubPerformance("Global Performance")->GetRecoData();
  GetMCParticles();
  FindReconstructableMCParticles();
  MatchParticles();
  CalculateEfficiency();

} // void PndFTSTopoPerformance::MatchTracks()

void PndFTSTopoPerformance::CalculateEfficiency()
{
  KFPartEfficiencies partEff; // efficiencies for current event

  const int NRP = fTopoReconstructor->GetParticles().size();
  for ( int iP = 0; iP < NRP; ++iP ) {
//    const CbmKFParticle &part = fPF->GetParticles()[iP];
    const KFParticle &part = fTopoReconstructor->GetParticles()[iP];
    const int pdg = part.GetPDG();

    const bool isBG = RtoMCParticleId[iP].idsMI.size() != 0;
    const bool isGhost = !RtoMCParticleId[iP].IsMatched();

    for(int iPart=0; iPart<fParteff.nParticles; iPart++)
      if ( pdg == fParteff.partPDG[iPart] )
        partEff.IncReco(isGhost, isBG, fParteff.partName[iPart].Data());
  }

  const int NMP = vMCParticles.size();
  for ( int iP = 0; iP < NMP; ++iP ) {
    const KFMCParticle &part = vMCParticles[iP];
    if ( !part.IsReconstructable() ) continue;
    const int pdg = part.GetPDG();
    const int mId = part.GetMotherId();

    const bool isReco = MCtoRParticleId[iP].ids.size() != 0;
    const int nClones = MCtoRParticleId[iP].ids.size() - 1;

    for(int iPart=0; iPart<fParteff.nParticles; iPart++)
    {
      if ( pdg == fParteff.partPDG[iPart] ) {
        partEff.Inc(isReco, nClones, fParteff.partName[iPart].Data());
        if ( mId == -1 )
          partEff.Inc(isReco, nClones, (fParteff.partName[iPart]+"_prim").Data());
        else
          partEff.Inc(isReco, nClones, (fParteff.partName[iPart]+"_sec").Data());
      }
    }
  }

  fNEvents++;

  fParteff += partEff;

  partEff.CalcEff();
  fParteff.CalcEff();

    //   cout.precision(3);
  //if(fNEvents%100 == 0)
  {
    cout << " ---- KF Particle finder --- " << endl;
    // cout << "L1 STAT    : " << fNEvents << " EVENT "               << endl << endl;
    //partEff.PrintEff();
    // cout << endl;
    cout << "ACCUMULATED STAT    : " << fNEvents << " EVENTS "               << endl << endl;
    fParteff.PrintEff();

    cout<<endl;
      // cout<<"CA Track Finder: " << L1_CATIME/L1_fNEvents << " s/ev" << endl << endl;
  }
}

void PndFTSTopoPerformance::FillHistos()
{
  PndFTSParticlePerformanceBase::FillHistos();

  const int nParticles = fTopoReconstructor->fRTrackIds.size();

  for(int iP = 0; iP < nParticles; iP++){

    const int itr = fTopoReconstructor->fRTrackIds[iP];
    const int iMC = recoData[itr].GetMCTrackId();


    if ( recoData[itr].IsGhost(PParameters::MinTrackPurity) ) continue;

    PndFTSCAMCTrack &mc = (*fMCTracks)[iMC];

    if(mc.MotherId() < 0)
    {
      continue;
    }
//     PndFTSCAMCTrack &mmc = (*fMCTracks)[mc.MotherId()];

      //      const PndFTSCAGBTrack &t = fTracker->Track( itr );
      //      PndFTSCATrackParam p = t.Param();
    KFParticle &p = fTopoReconstructor->fKFPTopoReconstructor->GetParticle(iP);

    float mcParPoint[6]={0};

    for(;;){ // technical DO // track pulls and residuals at track end
        // find point which corresponds to our track end - mcPoint
      int nFirstMC = mc.FirstMCPointID();
      int nMCPoints = mc.NMCPoints();
      PndFTSCALocalMCPoint *points = &((*fLocalMCPoints).Data()[nFirstMC]);
        
      int MCindex=-1;
      PndFTSCALocalMCPoint mcPoint;
      for(int iMCPoint=0; iMCPoint<nMCPoints; iMCPoint++)
      {
          // get global CS
        mcPoint = points[iMCPoint];
          
          // check if the point correspond to our track end
        if (fabs(p.X() - mcPoint.X()) < 2.f) {
          if (fabs(p.Y() - mcPoint.Y()) < 2.f) {
            if (fabs(p.Z() - mcPoint.Z()) < 2.f) {
              MCindex = iMCPoint;
              break;
            }
          }
        }
      }
      if(MCindex == -1) break;

        // get mc info
      const float mcX =  mcPoint.X();
      const float mcY =  mcPoint.Y();
      const float mcZ =  mcPoint.Z();
      const float mcPx = mcPoint.Px();
      const float mcPy = mcPoint.Py();
      const float mcPz = mcPoint.Pz();

      mcParPoint[0] = mcX;
      mcParPoint[1] = mcY;
      mcParPoint[2] = mcZ;
      mcParPoint[3] = mcPx;
      mcParPoint[4] = mcPy;
      mcParPoint[5] = mcPz;


      if ( CAMath::Sqrt( mcPx*mcPx + mcPy*mcPy /*+ mcPz*mcPz*/ ) < 0.010 ) break;
      
      KFParticle &pTmp = p;
      
      GetHisto("resX")->Fill ( pTmp.GetX()  - mcX  );
      GetHisto("resY")->Fill ( pTmp.GetY()  - mcY  );
      GetHisto("resZ")->Fill ( pTmp.GetZ()  - mcZ  );
      GetHisto("resPx")->Fill( pTmp.GetPx() - mcPx );
      GetHisto("resPy")->Fill( pTmp.GetPy() - mcPy );
      GetHisto("resPz")->Fill( pTmp.GetPz() - mcPz );

      if ( pTmp.GetErrX()  > 0 ) GetHisto("pullX")->Fill ( ( pTmp.GetX()  - mcX  ) / pTmp.GetErrX()  );
      if ( pTmp.GetErrY()  > 0 ) GetHisto("pullY")->Fill ( ( pTmp.GetY()  - mcY  ) / pTmp.GetErrY()  );
      if ( pTmp.GetErrZ()  > 0 ) GetHisto("pullZ")->Fill ( ( pTmp.GetZ()  - mcZ  ) / pTmp.GetErrZ()  );
      if ( pTmp.GetErrPx() > 0 ) GetHisto("pullPx")->Fill( ( pTmp.GetPx() - mcPx ) / pTmp.GetErrPx() );
      if ( pTmp.GetErrPy() > 0 ) GetHisto("pullPy")->Fill( ( pTmp.GetPy() - mcPy ) / pTmp.GetErrPy() );
      if ( pTmp.GetErrPz() > 0 ) GetHisto("pullPz")->Fill( ( pTmp.GetPz() - mcPz ) / pTmp.GetErrPz() );

      break;
    }

    for(;;){ // technical DO // track pulls and residuals at vertex
        // get mc info


      const float mcX =  mc.X();
      const float mcY =  mc.Y();
      const float mcZ =  mc.Z();
      const float mcPx = mc.Px();
      const float mcPy = mc.Py();
      const float mcPz = mc.Pz();

        // transport to vertex
      KFParticle pTmp = p;
      const double vertex[3] = { mcX, mcY, mcZ };
      pTmp.TransportToDS(  pTmp.GetDStoPoint(vertex) );

      int mcMotherId = mc.MotherId();
      int mcMotherPDG = 0;
      if(mcMotherId>-1)
      {
        PndFTSCAMCTrack &mcMother = (*fMCTracks)[mcMotherId];
        mcMotherPDG = mcMother.PDG();
      }
// if(mcMotherPDG!=310) break;
// #ifdef DRAW
// 
//       if(mcMotherPDG==310)
//       {
//         PndFTSCADisplay::Instance().ClearView();
//         PndFTSCADisplay::Instance().DrawTPC();
// 
// //         PndFTSCADisplay::Instance().SpecDrawMCTrackPointsGlobal( (*PndFTSCAPerformance::Instance().GetSubPerformance("Global Performance")->fMCTracks)[iMC], PndFTSCAPerformance::Instance().GetSubPerformance("Global Performance")->fLocalMCPoints, kRed, 0.3 );
//     // 
//         float param[8] = {mcX, mcY, mcZ, mcPx, mcPy, mcPz, 0, 0};
//         PndFTSCADisplay::Instance().DrawGBPoint( mcX, mcY, mcZ, kBlack );
//         PndFTSCADisplay::Instance().DrawGBPoint( pTmp.X(), pTmp.Y(), pTmp.Z(), kGreen );
// 
//         float vtx[3] = {mcX, mcY, mcZ};
//         for(int iP=0; iP<8; iP++)
//           param[iP] = p.Parameters()[iP];
// 
// std::cout << "Rec ";
// for(int iP=0; iP<8; iP++)
// std::cout << param[iP] << " ";
// std::cout << std::endl;
//         double b[3];
//         p.GetFieldValue(vertex, b);
//           PndFTSCADisplay::Instance().DrawParticleGlobal( param, p.Q(), 0, p.GetDStoPoint(vertex)*10, b[2], kBlue, 2 );
// 
//         double mcQ = 0;
//         for( int iP = 0; iP < mc.NMCPoints(); iP++ ) {
//           PndFTSCALocalMCPoint mcPoint =  (*fLocalMCPoints).Data()[mc.FirstMCPointID()+iP];
// 
//           double mcX0 =  mcPoint.X();
//           double mcY0 =  mcPoint.Y();
//           double mcZ  =  mcPoint.Z();
//           mcQ = mcPoint.QP()/CAMath::Abs(mcPoint.QP());
//           PndFTSCADisplay::Instance().DrawGBPoint((float)mcX0, (float)mcY0, (float)mcZ, kRed, (Size_t)1);
//         }
// 
//         const float kCLight = 0.000299792458;
//         float bq = b[2]*mcQ*kCLight;
//         float pt2 = mcParPoint[3]*mcParPoint[3] + mcParPoint[4]*mcParPoint[4];
// 
//         float dx = vtx[0] - mcParPoint[0];
//         float dy = vtx[1] - mcParPoint[1]; 
//         float a = dx*mcParPoint[3]+dy*mcParPoint[4];
//         float dS = 0;
//         dS = atan2( bq*a, pt2 + bq*(dy*mcParPoint[3] -dx*mcParPoint[4]) )/bq;
//         for(int i=0; i<6; i++)
//           param[i] = mcParPoint[i];
//         PndFTSCADisplay::Instance().DrawParticleGlobal( param, mcQ, 0, dS*10, b[2], kGreen, 2 );
// 
// std::cout << "MC  ";
// for(int iP=0; iP<8; iP++)
// std::cout << param[iP] << " ";
// std::cout << std::endl;
// std::cout << "URA!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << b[2] << std::endl;
//         PndFTSCADisplay::Instance().Ask();
//       }
// #endif

//      if ( mcX*mcX+mcY*mcY < 50*50 ) break; // dbg // TPC-vertices only
//      if ( pTmp.GetPt() < 1.f ) break; // dbg
//      if ( mc.MotherId() != -1 ) break; // dbg // primary only !!
      
        // fill histos

      if ( CAMath::Sqrt( mcPx*mcPx + mcPy*mcPy /*+ mcPz*mcPz*/ ) < 0.010 ) break;
        //        if ( TMath::Abs( qPt ) > 1.e-4 ) pt = 1. / TMath::Abs( qPt );

      // std::cout << " MC " << mcX << " " << mcY << std::endl;
      // std::cout << " diff " << pTmp.GetX()  - mcX << " " << pTmp.GetY()  - mcY << std::endl;
      GetHisto("resAtProdPointX")->Fill ( pTmp.GetX()  - mcX  );
      GetHisto("resAtProdPointY")->Fill ( pTmp.GetY()  - mcY  );
      GetHisto("resAtProdPointZ")->Fill ( pTmp.GetZ()  - mcZ  );
      GetHisto("resAtProdPointPx")->Fill( pTmp.GetPx() - mcPx );
      GetHisto("resAtProdPointPy")->Fill( pTmp.GetPy() - mcPy );
      GetHisto("resAtProdPointPz")->Fill( pTmp.GetPz() - mcPz );
        
      if ( pTmp.GetErrX()  > 0 ) GetHisto("pullAtProdPointX")->Fill ( ( pTmp.GetX()  - mcX  ) / pTmp.GetErrX()  );
      if ( pTmp.GetErrY()  > 0 ) GetHisto("pullAtProdPointY")->Fill ( ( pTmp.GetY()  - mcY  ) / pTmp.GetErrY()  );
      if ( pTmp.GetErrZ()  > 0 ) GetHisto("pullAtProdPointZ")->Fill ( ( pTmp.GetZ()  - mcZ  ) / pTmp.GetErrZ()  );
      if ( pTmp.GetErrPx() > 0 ) GetHisto("pullAtProdPointPx")->Fill( ( pTmp.GetPx() - mcPx ) / pTmp.GetErrPx() );
      if ( pTmp.GetErrPy() > 0 ) GetHisto("pullAtProdPointPy")->Fill( ( pTmp.GetPy() - mcPy ) / pTmp.GetErrPy() );
      if ( pTmp.GetErrPz() > 0 ) GetHisto("pullAtProdPointPz")->Fill( ( pTmp.GetPz() - mcPz ) / pTmp.GetErrPz() );

      break;
    }

    for(;;){ // technical DO // vertex pulls and residuals
      if (fPrimVertices.size() == 0) break; // no prim tracks
      
        // get mc info
      const float mcX =  fPrimVertices[0].X();
      const float mcY =  fPrimVertices[0].Y();
      const float mcZ =  fPrimVertices[0].Z();

        // prim vertex
      KFParticle &pTmp = fTopoReconstructor->fKFPTopoReconstructor->GetPrimVertex();

        // fill histos

      GetHisto("resVertexX")->Fill ( pTmp.GetX()  - mcX  );
      GetHisto("resVertexY")->Fill ( pTmp.GetY()  - mcY  );
      GetHisto("resVertexZ")->Fill ( pTmp.GetZ()  - mcZ  );
        
      if ( pTmp.GetErrX()  > 0 ) GetHisto("pullVertexX")->Fill ( ( pTmp.GetX()  - mcX  ) / pTmp.GetErrX()  );
      if ( pTmp.GetErrY()  > 0 ) GetHisto("pullVertexY")->Fill ( ( pTmp.GetY()  - mcY  ) / pTmp.GetErrY()  );
      if ( pTmp.GetErrZ()  > 0 ) GetHisto("pullVertexZ")->Fill ( ( pTmp.GetZ()  - mcZ  ) / pTmp.GetErrZ()  );

      break;
    }
  }

  //fill histograms for found short-lived particles
  for(unsigned int iP=0; iP<fTopoReconstructor->GetParticles().size(); iP++)
  {

    int iParticle = fParteff.GetParticleIndex(fTopoReconstructor->GetParticles()[iP].GetPDG());
    if(iParticle < 0) continue;

    Double_t M, ErrM;
    Double_t dL, ErrdL; // decay length
    Double_t cT, ErrcT; // c*tau
    Double_t P, ErrP;
    Double_t Pt;
    Double_t Rapidity;
    Double_t Theta;
    Double_t Phi;
    Double_t Z;
//    CbmKFParticle TempPart = fTopoReconstructor->GetParticles()[iP];
    KFParticle TempPart = fTopoReconstructor->GetParticles()[iP];
    TempPart.GetMass(M,ErrM);
    TempPart.GetMomentum(P,ErrP);
    Pt = TempPart.GetPt();
    Rapidity = TempPart.GetRapidity();
    TempPart.GetDecayLength(dL,ErrdL);
    TempPart.GetLifeTime(cT,ErrcT);
    Double_t chi2 = TempPart.GetChi2();
    Int_t ndf = TempPart.GetNDF();
    Double_t prob = TMath::Prob(chi2, ndf);//(TDHelper<float>::Chi2IProbability( ndf, chi2 ));
    Theta = TempPart.GetTheta();
    Phi = TempPart.GetPhi();
    Z = TempPart.GetZ();

    KFParticleSIMD tempSIMDPart(TempPart);
    fvec l,dl;
    KFParticleSIMD pv(fTopoReconstructor->GetPrimVertex());
//     KFParticleSIMD pv;
//     for(int iC=0; iC<6; iC++)
//       pv.CovarianceMatrix()[iC] = 0;

    tempSIMDPart.GetDistanceToVertexLine(pv, l, dl);
    dL = sqrt(TempPart.X()*TempPart.X() + TempPart.Y()*TempPart.Y() + TempPart.Z()*TempPart.Z() );

    //for all particle-candidates
    hPartParam[iParticle][ 0]->Fill(M);
    hPartParam[iParticle][ 1]->Fill(P);
    hPartParam[iParticle][ 2]->Fill(Pt);
    hPartParam[iParticle][ 3]->Fill(Rapidity);
    hPartParam[iParticle][ 4]->Fill(dL);
    hPartParam[iParticle][ 5]->Fill(cT);
    hPartParam[iParticle][ 6]->Fill(chi2/ndf);
    hPartParam[iParticle][ 7]->Fill(prob);
    hPartParam[iParticle][ 8]->Fill(Theta);
    hPartParam[iParticle][ 9]->Fill(Phi);
    hPartParam[iParticle][10]->Fill(Z);
    hPartParam[iParticle][11]->Fill(l[0]/dl[0]);


    hPartParam2D[iParticle][0]->Fill(Rapidity,Pt,1);

    if(!RtoMCParticleId[iP].IsMatchedWithPdg()) //background
    {
      if(!RtoMCParticleId[iP].IsMatched())
      {
        // for ghost particles - combinatorial background
        hPartParamGhost[iParticle][ 0]->Fill(M);
        hPartParamGhost[iParticle][ 1]->Fill(P);
        hPartParamGhost[iParticle][ 2]->Fill(Pt);
        hPartParamGhost[iParticle][ 3]->Fill(Rapidity);
        hPartParamGhost[iParticle][ 4]->Fill(dL);
        hPartParamGhost[iParticle][ 5]->Fill(cT);
        hPartParamGhost[iParticle][ 6]->Fill(chi2/ndf);
        hPartParamGhost[iParticle][ 7]->Fill(prob);
        hPartParamGhost[iParticle][ 8]->Fill(Theta);
        hPartParamGhost[iParticle][ 9]->Fill(Phi);
        hPartParamGhost[iParticle][10]->Fill(Z);
        hPartParamGhost[iParticle][11]->Fill(l[0]/dl[0]);

        hPartParam2DGhost[iParticle][0]->Fill(Rapidity,Pt,1);
      }
      else
      {
        // for phisical background

        hPartParamBG[iParticle][ 0]->Fill(M);
        hPartParamBG[iParticle][ 1]->Fill(P);
        hPartParamBG[iParticle][ 2]->Fill(Pt);
        hPartParamBG[iParticle][ 3]->Fill(Rapidity);
        hPartParamBG[iParticle][ 4]->Fill(dL);
        hPartParamBG[iParticle][ 5]->Fill(cT);
        hPartParamBG[iParticle][ 6]->Fill(chi2/ndf);
        hPartParamBG[iParticle][ 7]->Fill(prob);
        hPartParamBG[iParticle][ 8]->Fill(Theta);
        hPartParamBG[iParticle][ 9]->Fill(Phi);
        hPartParamBG[iParticle][10]->Fill(Z);
        hPartParamBG[iParticle][11]->Fill(l[0]/dl[0]);

        hPartParam2DBG[iParticle][0]->Fill(Rapidity,Pt,1);
      }
      continue;
    }
    //for signal particles
    hPartParamSignal[iParticle][ 0]->Fill(M);
    hPartParamSignal[iParticle][ 1]->Fill(P);
    hPartParamSignal[iParticle][ 2]->Fill(Pt);
    hPartParamSignal[iParticle][ 3]->Fill(Rapidity);
    hPartParamSignal[iParticle][ 4]->Fill(dL);
    hPartParamSignal[iParticle][ 5]->Fill(cT);
    hPartParamSignal[iParticle][ 6]->Fill(chi2/ndf);
    hPartParamSignal[iParticle][ 7]->Fill(prob);
    hPartParamSignal[iParticle][ 8]->Fill(Theta);
    hPartParamSignal[iParticle][ 9]->Fill(Phi);
    hPartParamSignal[iParticle][10]->Fill(Z);
    hPartParamSignal[iParticle][11]->Fill(l[0]/dl[0]);

    hPartParam2DSignal[iParticle][0]->Fill(Rapidity,Pt,1);

    int iMCPart = RtoMCParticleId[iP].GetBestMatchWithPdg();
    KFMCParticle &mcPart = vMCParticles[iMCPart];
    // Fit quality of the mother particle
    {
      int iMCTrack = mcPart.GetMCTrackID();
      PndFTSCAMCTrack &mcTrack = (*fMCTracks)[iMCTrack];
      int mcDaughterId = mcPart.GetDaughterIds()[0];
      PndFTSCAMCTrack &mcDaughter = (*fMCTracks)[mcDaughterId];

      TParticlePDG* particlePDG = TDatabasePDG::Instance()->GetParticle(mcTrack.PDG());

      const float mcX =  mcDaughter.X();
      const float mcY =  mcDaughter.Y();
      const float mcZ =  mcDaughter.Z();
      const float mcPx = mcTrack.Par(3)*mcTrack.P();
      const float mcPy = mcTrack.Par(4)*mcTrack.P();
      const float mcPz = mcTrack.Par(5)*mcTrack.P();

      Double_t decayVtx[3] = { mcX, mcY, mcZ };
      Double_t recParam[8] = { 0 };
      Double_t errParam[8] = { 0 };

      for(int iPar=0; iPar<3; iPar++)
      {
        recParam[iPar] = TempPart.GetParameter(iPar);
        Double_t error = TempPart.GetCovariance(iPar,iPar);
        if(error < 0.) { error = 1.e20;}
        errParam[iPar] = TMath::Sqrt(error);
      }
      TempPart.TransportToPoint(decayVtx);
      for(int iPar=3; iPar<7; iPar++)
      {
        recParam[iPar] = TempPart.GetParameter(iPar);
        Double_t error = TempPart.GetCovariance(iPar,iPar);
        if(error < 0.) { error = 1.e20;}
        errParam[iPar] = TMath::Sqrt(error);
      }

      Double_t massMC = (particlePDG) ? particlePDG->Mass() :0.13957;

      Double_t Emc = sqrt(mcTrack.P()*mcTrack.P() + massMC*massMC);
      Double_t res[8] = {0}, 
               pull[8] = {0}, 
               mcParam[8] = { decayVtx[0], decayVtx[1], decayVtx[2],
                              mcPx, mcPy, mcPz, Emc, massMC };
      for(int iPar=0; iPar < 7; iPar++ )
      {
        res[iPar]  = recParam[iPar] - mcParam[iPar];
        if(fabs(errParam[iPar]) > 1.e-20) pull[iPar] = res[iPar]/errParam[iPar];
      }

      res[7] = M - mcParam[7];
      if(fabs(ErrM) > 1.e-20) pull[7] = res[7]/ErrM;

      for(int iPar=0; iPar < 8; iPar++ )
      {
        hFitQA[iParticle][iPar]->Fill(res[iPar]);
        hFitQA[iParticle][iPar+8]->Fill(pull[iPar]);
      }

// #ifdef DRAW
//       if(mcTrack.PDG()==3122)
//       {
//         PndFTSCADisplay::Instance().ClearView();
//         PndFTSCADisplay::Instance().SetTPCView();
//         PndFTSCADisplay::Instance().DrawTPC();
// 
//         KFParticle rPart = fTopoReconstructor->GetParticles()[iP];
//         const unsigned int NRDaughters = rPart.NDaughters();
// 
//         float param[8] = {mcX, mcY, mcZ, mcPx, mcPy, mcPz, 0, 0};
// 
//         for (unsigned int iD=0 ; iD < NRDaughters; iD++ ) {
// 
//           KFParticle dPart = fTopoReconstructor->GetParticles()[rPart.DaughterIds()[iD]];
// 
//           int iDMC = RtoMCParticleId[dPart.DaughterIds()[0]].GetBestMatch();
//           PndFTSCADisplay::Instance().SpecDrawMCTrackPointsGlobal( (*PndFTSCAPerformance::Instance().GetSubPerformance("Global Performance")->fMCTracks)[iDMC], PndFTSCAPerformance::Instance().GetSubPerformance("Global Performance")->fLocalMCPoints, kRed, 0.3 );
// 
//           float vtx[3] = {rPart.X(), rPart.Y(), rPart.Z()};
//           double vertex[3] = {rPart.X(), rPart.Y(), rPart.Z()};
//           for(int iP=0; iP<8; iP++)
//             param[iP] = dPart.Parameters()[iP];
//           double b[3];
//           dPart.GetFieldValue(vertex, b);
//           PndFTSCADisplay::Instance().DrawParticleGlobal( param, dPart.Q(), 0, dPart.GetDStoPoint(vertex), b[2], kBlue, 2 );
//         }
// 
//         param[0] = mcX;
//         param[1] = mcY;
//         param[2] = mcZ;
//         param[3] = mcPx;
//         param[4] = mcPy;
//         param[5] = mcPz;
// 
//         PndFTSCADisplay::Instance().DrawGBPoint( param, kBlack );
//         PndFTSCADisplay::Instance().DrawGBPoint( rPart.X(), rPart.Y(), rPart.Z(), kGreen );
// 
//         PndFTSCADisplay::Instance().Ask();
//       }
// #endif
//      Double_t mcT = mcDaughter.GetStartT() - mcTrack.GetStartT();
      
    }
    // Fit quality of daughters
    for(int iD=0; iD<mcPart.NDaughters(); ++iD)
    {
      int mcDaughterId = mcPart.GetDaughterIds()[iD];
//      if(!MCtoRParticleId[mcDaughterId].IsMatchedWithPdg()) continue;
      if(!MCtoRParticleId[mcDaughterId].IsMatched()) continue;
      PndFTSCAMCTrack &mcTrack = (*fMCTracks)[mcDaughterId];
//      int recDaughterId = MCtoRParticleId[mcDaughterId].GetBestMatchWithPdg();
      int recDaughterId = MCtoRParticleId[mcDaughterId].GetBestMatch();
//      CbmKFParticle Daughter = fPF->GetParticles()[recDaughterId];
      KFParticle Daughter = fTopoReconstructor->GetParticles()[recDaughterId];
      Daughter.GetMass(M,ErrM);

      TParticlePDG* particlePDG = TDatabasePDG::Instance()->GetParticle(mcTrack.PDG());

      const float mcX =  mcTrack.X();
      const float mcY =  mcTrack.Y();
      const float mcZ =  mcTrack.Z();
      const float mcPx = mcTrack.Px();
      const float mcPy = mcTrack.Py();
      const float mcPz = mcTrack.Pz();

      Double_t decayVtx[3] = {mcX, mcY, mcZ};
      Daughter.TransportToPoint(decayVtx);

      Double_t massMC = (particlePDG) ? particlePDG->Mass() :0.13957;

      Double_t Emc = sqrt(mcTrack.P()*mcTrack.P() + massMC*massMC);
      Double_t res[8] = {0}, 
               pull[8] = {0}, 
               mcParam[8] = { mcX, mcY, mcZ,
                              mcPx, mcPy, mcPz, Emc, massMC };
      for(int iPar=0; iPar < 7; iPar++ )
      {
        Double_t error = Daughter.GetCovariance(iPar,iPar);
        if(error < 0.) { error = 1.e20;}
        error = TMath::Sqrt(error);
        res[iPar]  = Daughter.GetParameter(iPar) - mcParam[iPar];
        if(fabs(error) > 1.e-20) pull[iPar] = res[iPar]/error;
      }
      res[7] = M - mcParam[7];
      if(fabs(ErrM) > 1.e-20) pull[7] = res[7]/ErrM;

      for(int iPar=0; iPar < 8; iPar++ )
      {
        hFitDaughtersQA[iParticle][iPar]->Fill(res[iPar]);
        hFitDaughtersQA[iParticle][iPar+8]->Fill(pull[iPar]);
      }
    }
  }

  //fill histograms with ChiPrim for every particle
  for(unsigned int iP=0; iP<fTopoReconstructor->GetParticles().size(); iP++)
  {
    const KFParticle &rPart = fTopoReconstructor->GetParticles()[iP];
    const unsigned int NRDaughters = rPart.NDaughters();
    if (NRDaughters > 1) break;
    if( RtoMCParticleId[iP].GetBestMatch()<0 ) continue;

    KFMCParticle &mPart = vMCParticles[ RtoMCParticleId[iP].GetBestMatch() ];

    if(mPart.GetMotherId() < 0)
    {
      hTrackParameters[KFPartEfficiencies::nParticles]->Fill(fTopoReconstructor->GetChiPrim()[iP] );
      continue;
    }

    KFMCParticle &mMotherPart = vMCParticles[mPart.GetMotherId()];
    int iParticle = fParteff.GetParticleIndex(mMotherPart.GetPDG());
    float chiPrim = fTopoReconstructor->GetChiPrim()[iP];
    if(iParticle > -1 && iParticle<KFPartEfficiencies::nParticles)
      hTrackParameters[iParticle]->Fill(chiPrim );
  }

  //fill histograms of the primary vertex quality
  {
    //Find MC parameters of the primary vertex 
    float mcPVx[3]={0.f};
    for(int iMC=0; iMC<nMCTracks; ++iMC)
    {
      PndFTSCAMCTrack &mcTrack = (*fMCTracks)[iMC];
      if(mcTrack.MotherId() < 0)
      {
        mcPVx[0]=mcTrack.X();
        mcPVx[1]=mcTrack.Y();
        mcPVx[2]=mcTrack.Z();
        break;
      }
    }

    KFParticle & vtx = fTopoReconstructor->GetPrimVertex();
    double dRPVr[3] = {vtx.X()-mcPVx[0],
                      vtx.Y()-mcPVx[1],
                      vtx.Z()-mcPVx[2]};
    double dRPVp[3] = {dRPVr[0]/sqrt(vtx.CovarianceMatrix()[0]),
                      dRPVr[1]/sqrt(vtx.CovarianceMatrix()[2]),
                      dRPVr[2]/sqrt(vtx.CovarianceMatrix()[5])};
    for(unsigned int iHPV=0; iHPV<3; ++iHPV)
      hPVFitQa[iHPV]->Fill(dRPVr[iHPV]);
    for(unsigned int iHPV=3; iHPV<6; ++iHPV)
      hPVFitQa[iHPV]->Fill(dRPVp[iHPV-3]);
  }
} // void PndFTSTopoPerformance::FillHistos()

#endif //DO_TPCCATRACKER_EFF_PERFORMANCE
