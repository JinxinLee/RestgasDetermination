// $Id: PndFTSCAPerformance.cxx,v 1.13 2010/09/01 10:38:27 ikulakov Exp $
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

#include "PndFTSCAPerformance.h"

#include "PndFTSCounters.h"
#include "PndFTSPerformanceBase.h"
#include "PndFTSCAGlobalPerformance.h"
//#include "PndFTSTopoPerformance.h"
#include "PndFTSCAParam.h"

#include "PndFTSCAGBHit.h"
#include "PndFTSCAMCTrack.h"
#ifndef HLTCA_STANDALONE
#include "PndFTSCAMCPoint.h"
#endif
#include "PndFTSCAGBTrack.h"
#include "PndFTSCAGBTracker.h"

#include "PndFTSCADisplay.h"

#include "PndFTSCAParameters.h"

#include "TRandom3.h" //dbg
#include "TMath.h"
#include "TROOT.h"
#include "Riostream.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TStyle.h"

PndFTSCAPerformance::PndFTSCAPerformance()
{
  static bool first_call = true;

  if (first_call){
    typedef TSubPerformance TSP;
    
      /// Just define here all sub-performances
      /// TSP(new __ClassName__               , __Name__      ),
#if !defined(KFPARTICLE)
    const int NSPerfo = 1;
#else
#ifndef STAR_STANDALONE
    const int NSPerfo = 1;
#else // HLTCA_STANDALONE
    const int NSPerfo = 1;
#endif // HLTCA_STANDALONE
#endif
    const TSP perfos[NSPerfo] = {
      TSP(new PndFTSCAGlobalPerformance, "Global Performance")
#ifdef KFPARTICLE
      //,TSP(new PndFTSTopoPerformance, "Topo Performance")
#endif
    };

    subPerformances.resize(NSPerfo);
    for (int iP = 0; iP < NSPerfo; iP++){
      subPerformances[iP] = perfos[iP];
    }
  }
  first_call = false;
  //* constructor
}

PndFTSCAPerformance::~PndFTSCAPerformance()
{
  //* destructor
  fHitLabels.Resize( 0 );
  fMCTracks.Resize( 0 );
  for (unsigned int iPerf = 0; iPerf < subPerformances.size(); iPerf++){ // TODO: why we can't do this in subPerformances.~destructor() ??
    if (subPerformances[iPerf].perf) delete subPerformances[iPerf].perf;
  }
}

PndFTSCAPerformance &PndFTSCAPerformance::Instance()
{
  // reference to static object
  static PndFTSCAPerformance gPndFTSCAPerformance;
  return gPndFTSCAPerformance;
}

bool PndFTSCAPerformance::SetNewEvent(PndFTSCAGBTracker* const tracker, string mcTracksFile, string mcPointsFile)
{
  fTracker = tracker;
    // Read MC info from file
  FILE *file = std::fopen( mcTracksFile.data(), "rb" );
  if ( !file ) {
    return false;
  }
  ReadMCEvent( file );
  fclose( file );
  
  file = std::fopen( mcPointsFile.data(), "rb" );
  if ( !file ) {
    return false;
  }
  ReadLocalMCPoints( file );
  fclose( file );

  return true;
} // void PndFTSCAPerformance::SetNewEvent

void PndFTSCAPerformance::InitSubPerformances()
{
    // Init subperformances
  static bool first_call = true;
  for (unsigned int iPerf = 0; iPerf < subPerformances.size(); iPerf++){
    subPerformances[iPerf]->SetNewEvent(fTracker, &fHitLabels, &fMCTracks, &fLocalMCPoints);
  }
#ifdef KFPARTICLE
  /*if ( GetSubPerformance("Topo Performance") )
    dynamic_cast<PndFTSTopoPerformance*>(GetSubPerformance("Topo Performance"))->SetNewEvent2(fTopoReconstructor);*/
#endif
  
//   if (first_call) CreateHistos();
  
  first_call = false;
} // void PndFTSCAPerformance::InitSubPerformances

void PndFTSCAPerformance::CreateHistos()
{
  for (unsigned int iPerf = 0; iPerf < subPerformances.size(); iPerf++){
    if(!(subPerformances[iPerf]->IsHistoCreated())) 
      subPerformances[iPerf]->CreateHistos(subPerformances[iPerf].name, fOutputFile);
  }
}

bool PndFTSCAPerformance::CreateHistos(string name)
{
  unsigned i = 0;
  for( ; i < (subPerformances.size()) && (subPerformances[i].name != name); i++);
  if(!(subPerformances[i]->IsHistoCreated()) && i != subPerformances.size()) 
    subPerformances[i]->CreateHistos(subPerformances[i].name, fOutputFile);
  if ( i == subPerformances.size() ) return 0;
  return 1;
}


void PndFTSCAPerformance::WriteDir2Current( TObject *obj )
{
  //* recursive function to write down all created in the file histos
  if ( !obj->IsFolder() ) obj->Write();
  else {
    TDirectory *cur = gDirectory;
    ((TDirectory*)obj)->cd();
    TList *listSub = ( ( TDirectory* )obj )->GetList();
    TIter it( listSub );
    while ( TObject *obj1 = it() ) WriteDir2Current( obj1 );
    cur->cd();
  }
}


void PndFTSCAPerformance::WriteHistos()
{
  if(fOutputFile) WriteDir2Current(fOutputFile);
}

void PndFTSCAPerformance::ExecPerformance()
{
  fStatNEvents++;
#ifdef KFPARTICLE
  for (unsigned int iPerf = 0; iPerf < subPerformances.size(); iPerf++){
#else
  for (unsigned int iPerf = 0; iPerf < subPerformances.size(); iPerf++){
#endif
    if(subPerformances[iPerf].IsGlobalPerf) subPerformances[iPerf]->Exec(0);
  }
#if 1  // current event efficiencies
  for (unsigned int iPerf = 0; iPerf < subPerformances.size(); iPerf++){    
    cout << endl
        << " ---- " << subPerformances[iPerf].name << " event " << fStatNEvents << " ---- "<< endl;
    subPerformances[iPerf]->PrintEfficiency();
  }
  cout << endl << " ============================== " << endl;
#endif //0
  for (unsigned int iPerf = 0; iPerf < subPerformances.size(); iPerf++){  
    cout << endl
        << " ---- " << subPerformances[iPerf].name << " " << fStatNEvents << " events Statistic ---- " << endl;
    if(subPerformances[iPerf].IsGlobalPerf) subPerformances[iPerf]->PrintEfficiencyStatistic();
  };

}

PndFTSPerformanceBase* PndFTSCAPerformance::GetSubPerformance(string name)
{
  unsigned i = 0;
  for( ; (i < subPerformances.size()) && (subPerformances[i].name != name); i++);
  //ASSERT ( i != subPerformances.size() , " Incorrect name of subPerformance used.");
  if ( i == subPerformances.size() ) return 0;
  return subPerformances[i].perf;
} // PndFTSPerformanceBase* PndFTSCAPerformance::GetSubPerformance(string name)


  /// -------------------- Read\write MC information ---------------------
void PndFTSCAPerformance::WriteMCEvent( FILE *file ) const
{
  // write MC information to the file
  int n = fMCTracks.Size();
  std::fwrite( &n, sizeof( int ), 1, file );
  n = fHitLabels.Size();
  std::fwrite( &n, sizeof( int ), 1, file );
  std::fwrite( fMCTracks.Data(), sizeof( PndFTSCAMCTrack ), fMCTracks.Size(), file );
  std::fwrite( fHitLabels.Data(), sizeof( PndFTSCAHitLabel ), fHitLabels.Size(), file );
}

void PndFTSCAPerformance::ReadMCEvent( FILE *file )
{
  // read mc info from the file
  int read;
  int n;

  read = std::fread( &n, sizeof( int ), 1, file );
  assert( read == 1 );
  fMCTracks.Resize( n );
  read = std::fread( &n, sizeof( int ), 1, file );
  assert( read == 1 );
  fHitLabels.Resize( n );

  read = std::fread( fMCTracks.Data(), sizeof( PndFTSCAMCTrack ), fMCTracks.Size(), file );
  assert( read == fMCTracks.Size() );

  read = std::fread( fHitLabels.Data(), sizeof( PndFTSCAHitLabel ), fHitLabels.Size(), file );
  assert( read == fHitLabels.Size() );
  UNUSED_PARAM1(read);
//   for (int i = 0; i < fMCTracks.Size(); i++){
//     PndFTSCAMCTrack& mc = fMCTracks[i];
//         std::cout << mc.PDG() << " ";
//         std::cout << std::endl;
//         for (int iPar = 0; iPar < 7; iPar++)
//           std::cout << mc.Par(iPar) << " ";
//         std::cout << std::endl;
//         for (int iPar = 0; iPar < 7; iPar++)
//           std::cout << mc.TPCPar(iPar) << " ";
//         std::cout << std::endl;
// //         std::cout << mc.P() << " ";
//   };
//   for (int i = 0; i < fHitLabels.Size(); i++){
//     PndFTSCAHitLabel& l = fHitLabels[i];
//     for (int iPar = 0; iPar < 3; iPar++)
//      std::cout << l.fLab[iPar] << " ";
//     std::cout << std::endl;
//   };
}

void PndFTSCAPerformance::ReadLocalMCPoints( FILE *file )
{
  
  int read;
  int n;

  read = std::fread( &n, sizeof( int ), 1, file );
  assert( read == 1 );
  fLocalMCPoints.Resize( n );

  read = std::fread( fLocalMCPoints.Data(), sizeof( PndFTSCALocalMCPoint ), fLocalMCPoints.Size(), file );
  assert( read == fLocalMCPoints.Size() );
  UNUSED_PARAM1(read);
} // void PndFTSCAPerformance::ReadLocalMCPoints( FILE *file )


void PndFTSCAPerformance::SetMCTracks(vector<PndFTSCAMCTrack>& mcTracks)
{
  const int N = mcTracks.size();
  fMCTracks.Resize(N);
  for(int i = 0; i < N; i++){
    fMCTracks[i] = mcTracks[i];
  }
}

void PndFTSCAPerformance::SetMCPoints(vector<PndFTSCALocalMCPoint>& mcPoints)
{
  const int N = mcPoints.size();
  fLocalMCPoints.Resize(N);
  for(int i = 0; i < N; i++){
    fLocalMCPoints[i] = mcPoints[i];
  }
}

void PndFTSCAPerformance::SetHitLabels(vector<PndFTSCAHitLabel>& hitLabels)
{
  const int N = hitLabels.size();
  fHitLabels.Resize(N);
  for(int i = 0; i < N; i++){
    fHitLabels[i] = hitLabels[i];
  }					   
}

void PndFTSCAPerformance::SaveDataInFiles(string prefix) const
{
  /*
  {
    ofstream ofile((prefix+"hitLabels.data").data(),ios::out|ios::app);
    const int Size = fHitLabels.Size();
    ofile << Size << std::endl;
    for (int i = 0; i < fHitLabels.Size(); i++){
      const PndFTSCAHitLabel &l = fHitLabels[i];
      ofile << l;
    }
    ofile.close();
  }

  {
    ofstream ofile((prefix+"MCTracks.data").data(),ios::out|ios::app);
    const int Size = fMCTracks.Size();
    ofile << Size << std::endl;
    for (int i = 0; i < fMCTracks.Size(); i++){
      const PndFTSCAMCTrack &l = fMCTracks[i];
      ofile << l;
    }
    ofile.close();
  }

  {
    ofstream ofile((prefix+"MCPoints.data").data(),ios::out|ios::app);
    const int Size = fLocalMCPoints.Size();
    ofile << Size << std::endl;
    for (int i = 0; i < fLocalMCPoints.Size(); i++){
      const PndFTSCALocalMCPoint &l = fLocalMCPoints[i];
      ofile << l;
    }
    ofile.close();
  }*/
}

bool PndFTSCAPerformance::ReadData(vector <int>& labels, vector <PndFTSCALocalMCPoint>& mcpoints, vector<PndFTSCAMCTrack>& mctracks)
{

  {
    fHitLabels.Resize(labels.size());
    for (int i = 0; i < labels.size(); i++){
      PndFTSCAHitLabel l;
      l.fLab[0] = labels[i];
      l.fLab[1] = -1;
      l.fLab[2] = -1;
      fHitLabels[i] = l;
      //cout<<"labels[i] "<<labels[i]<<endl;
      //cout<<"label l["<<i<<"]: "<<fHitLabels[i].fLab[0]<<" "<<fHitLabels[i].fLab[1]<<" "<<fHitLabels[i].fLab[2]<<" ";
    }
    /*ifstream ifile((prefix+"hitLabels.data").data());
    if ( !ifile.is_open() ) return 0;
    int Size;
    ifile >> Size;
    cout<<"hitlables size "<<Size<<endl;
    fHitLabels.Resize(Size);
    for (int i = 0; i < Size; i++){
      PndFTSCAHitLabel &l = fHitLabels[i];
      ifile >> l;
    }
    ifile.close();*/
  }
    cout<<endl;
  {
    fMCTracks.Resize(mctracks.size());
    for (int i = 0; i < mctracks.size(); i++){
      fMCTracks[i] = mctracks[i];
      //cout<<"fMCTracks["<<i<<"]: "<<fMCTracks[i]; 
    }
    
    /*ifstream ifile((prefix+"MCTracks.data").data());
    if ( !ifile.is_open() ) return 0;
    int Size;
    ifile >> Size;
    cout<<"mctracks size "<<Size<<endl;
    fMCTracks.Resize(Size);
    for (int i = 0; i < Size; i++){
      PndFTSCAMCTrack &l = fMCTracks[i];
      ifile >> l;
    }
    ifile.close();*/
  }


  {
    fLocalMCPoints.Resize(mcpoints.size());
    //cout<<"mcpoints.size() "<<mcpoints.size()<<endl;
    for (int i = 0; i < mcpoints.size(); i++){
      fLocalMCPoints[i] = mcpoints[i];
      //cout<<"fLocalMCPoints["<<i<<"].IRow(): "<<fLocalMCPoints[i].IRow()<<" QP() "<<fLocalMCPoints[i].QP()<<endl; 
    }
    /*ifstream ifile((prefix+"MCPoints.data").data());
    if ( !ifile.is_open() ) return 0;
    int Size;
    ifile >> Size;
    cout<<"mcpoints size "<<Size<<endl;
    fLocalMCPoints.Resize(Size);
    for (int i = 0; i < Size; i++){
      PndFTSCALocalMCPoint &l = fLocalMCPoints[i];
      ifile >> l;
    }
    ifile.close();*/
  }
  
	// calculate needed additional info
  {
    const int NMCTracks = fMCTracks.Size();
      // NMCRows
    for (int i = 0; i < NMCTracks; ++i ){
      PndFTSCAMCTrack& t = fMCTracks[i];

      vector<int> rows;
      rows.resize( PndFTSCAParameters::MaxNStations, 0 );
      for (int j = 0, iP = t.FirstMCPointID(); j < t.NMCPoints(); iP++, j++){
        //cout<<"fLocalMCPoints[iP].IRow() "<<fLocalMCPoints[iP].IRow()<<endl;
        rows[fLocalMCPoints[iP].IRow()]++;
      }

      int nRows = 0;
      
      int nMCContRows = 0;
      int istaold = -2, ncont=0;
      for (unsigned int j = 0 ; j < rows.size(); j++) {
        if( rows[j] > 0 ) {
          nRows++;
          
          if( istaold == static_cast<int>(j)-1 ) {
            ncont++;
           }
          else {
            nMCContRows = (nMCContRows > ncont) ? nMCContRows : ncont;
            ncont = 1;
          }
          istaold = j;
        }
      }
      nMCContRows = (nMCContRows > ncont) ? nMCContRows : ncont;
      
      t.SetNMCRows( nRows );
      t.SetNMCContRows( nMCContRows );
    }
    
      // NHitRows
    
    vector<int> zero(PndFTSCAParameters::MaxNStations, 0);
    vector< vector<int> > nmchits(NMCTracks,zero);
    PndFTSResizableArray<PndFTSCAGBHit>& hits = const_cast<PndFTSCAGBTracker *>(fTracker)->fHits;
    for (int i = 0; i < hits.Size(); i++) {
      int id = hits[i].ID();
      for ( int il = 0; il < 3; il++ ) {
        int trackId = HitLabel(id).fLab[il];
        if(trackId < 0) continue;
        ASSERT(trackId < NMCTracks, "Incorrect MCPoints of MCTracks file. " <<  trackId << " < " << NMCTracks );
        assert( hits[i].IRow() < PndFTSCAParameters::MaxNStations );
        nmchits[trackId][hits[i].IRow()]++;
      }
    }
    
    for (int i = 0; i < NMCTracks; ++i ){
      PndFTSCAMCTrack& t = fMCTracks[i];
      int nRows = 0;

      int nHitContRows = 0;
      int istaold = -1, ncont=0;
      for (int j = 0; j < PndFTSCAParameters::MaxNStations; ++j ) {
        if ( nmchits[i][j] ) {
          nRows++;

          if( istaold == j-1 ) {
            ncont++;
           }
          else {
            nHitContRows = (nHitContRows > ncont) ? nHitContRows : ncont;
            ncont = 1;
          }
          istaold = j;
        }
      }
      nHitContRows = (nHitContRows > ncont) ? nHitContRows : ncont;

      t.SetNHitRows( nRows );
      t.SetNHitContRows( nHitContRows );
    }

      // Angle, HitErrors
    for ( int ih = 0; ih < hits.Size(); ih++ ) {
      PndFTSCAGBHit &hit = hits[ih];
      const PndFTSCAHitLabel &l = (fHitLabels)[hit.ID()];

      const int iMC = l.fLab[0];
      if( iMC<0 ) continue;
      PndFTSCAMCTrack &mc = (fMCTracks)[iMC];


      int MCindex = -1;
      int nFirstMC = mc.FirstMCPointID();
      int nMCPoints = mc.NMCPoints();
      PndFTSCALocalMCPoint *points = &((fLocalMCPoints).Data()[nFirstMC]);

      for(int iMCPoint=0; iMCPoint<nMCPoints; iMCPoint++)
      {
        if(points[iMCPoint].IRow() == hit.IRow())
        {
          if(fabs(hit.Y() - points[iMCPoint].Y())<2 && fabs(hit.Z() - points[iMCPoint].Z())<2) // dbg +
            MCindex = iMCPoint;
        }
      }
      if(MCindex == -1)
      {
        continue;
      }

      points[MCindex].SetAngle(hit.Angle());
      points[MCindex].SetErr2Z(hit.Err2Z());
      points[MCindex].SetErr2Y(hit.Err2Y());
    }

      // PV
    fPV[0] = fPV[1] = fPV[2] = 0;
    for (int i = 0; i < NMCTracks; ++i ){
      PndFTSCAMCTrack& t = fMCTracks[i];
      if (t.MotherId() > 0) continue;
      fPV[0] = t.X();
      fPV[1] = t.Y();
      fPV[2] = t.Z();
      break;
    }

#ifdef STAR_HFT
      // Secondary tracks. Tmp
    for (int i = 0; i < NMCTracks; ++i ){
      PndFTSCAMCTrack& t = fMCTracks[i];
      if (t.MotherId() > 0) continue;
      t.SetMotherId( 1 );
      if ( ( abs(fPV[0] - t.X()) <= 1e-7 ) && ( abs(fPV[1] - t.Y()) <= 1e-7 ) && ( abs(fPV[2] - t.Z()) <= 1e-7 ) )
        t.SetMotherId( -1 );
    }
#endif
  }
  
  return 1;
}

int PndFTSCAPerformance::GetMCPoint( const PndFTSCAGBHit& hit ) const 
{
  const PndFTSCAHitLabel &l = fHitLabels[hit.ID()];

  int iMCP = -1;
  float dxMin = 1.e6;
  for( int k = 0; k < 3 && iMCP == -1; k++ ) {
    const int iMC = l.fLab[k];
    if (iMC < 0) continue;

    const PndFTSCAMCTrack &mc = fMCTracks[iMC];

    int nFirstMC = mc.FirstMCPointID();
    int nMCPoints = mc.NMCPoints();

    // float r2Min = 1e10;
    const PndFTSCALocalMCPoint *points = &(fLocalMCPoints.Data()[nFirstMC]);
    for(int iMCPoint=0; iMCPoint<nMCPoints; iMCPoint++)
    {
      const PndFTSCALocalMCPoint &p = points[iMCPoint];
#ifdef DRIFT_TUBES
      if(fabs(p.Z() - hit.Z()) > 0.3) continue;
#else
      if( fabs(xPLoc - xHLoc) > 2.f || fabs(p.Angle() - hit.Angle()) > 1e-6 ) continue;
#endif
      float sinA = fTracker->GetParameters().Station(hit.IRow()).f.sin;
      float cosA = fTracker->GetParameters().Station(hit.IRow()).f.cos;
      float correction = cosA*(p.Px()/p.Pz()) + sinA*(p.Py()/p.Pz());
      float dx = cosA*hit.X() + sinA*hit.Y() + hit.R()*sqrt(1.+correction*correction) - cosA*p.X()- sinA*p.Y(); // dx = hit.X() - hit.R() - p.X();
      if(hit.IsLeft())
        dx =  cosA*hit.X() + sinA*hit.Y() - hit.R()*sqrt(1.+correction*correction) - cosA*p.X()- sinA*p.Y(); // dx = hit.X() + hit.R() - p.X();

      if(fabs(dx)<dxMin)
      {
        if( fabs(dx) < 10*sqrt(hit.Err2R())*sqrt(1.+correction*correction) ) //if( fabs(dx) < 5*sqrt(hit.Err2X0()) ) // suppose we have only one MCPoint per track per station
	{
          iMCP = iMCPoint + nFirstMC;
	  dxMin = fabs(dx);
	}
      }
    }
  }

  
  return iMCP;
}

#include "PndFTSCADisplay.h"
  // Use smeared MCposition instead of hits
void PndFTSCAPerformance::ShiftHitsToMC( float errorX1, float errorX2 ){
  PndFTSResizableArray<PndFTSCAGBHit>& hits = const_cast<PndFTSCAGBTracker *>(fTracker)->fHits;

  std::sort( hits.Data(), hits.Data() + const_cast<PndFTSCAGBTracker *>(fTracker)->NHits(), PndFTSCAGBHit::Compare ); // has an influence on the procedure. CHECKME why?
  
  static TRandom3 rand;
  {
    int nHits = fTracker->NHits();

    for ( int ih = 0; ih < nHits; ih++ ) {
      PndFTSCAGBHit &hit = hits[ih];

      const int iMCP = GetMCPoint(hit);
      if (iMCP == -1) continue;
      const PndFTSCALocalMCPoint& point = fLocalMCPoints.Data()[iMCP];

// #ifdef DRAW
//       PndFTSCADisplay::Instance().SetTPCView();
//       PndFTSCADisplay::Instance().DrawTPC();
//       for(int iMCPoint=0; iMCPoint<nMCPoints; iMCPoint++)
//       {
//         PndFTSCADisplay::Instance().DrawGBPoint((float)points[iMCPoint].X(),
//                                                    (float)points[iMCPoint].Y(),
//                                                    (float)points[iMCPoint].Z(), 1, (Size_t)1);
//       }
//       PndFTSCADisplay::Instance().DrawGBPoint((float)hit.X(), 
//                                                  (float)hit.Y(),
//                                                  (float)hit.Z(),
//                                                  -1, (Size_t)0.5);
//       PndFTSCADisplay::Instance().DrawGBPoint((float)point.X(), 
//                                                  (float)point.Y(),
//                                                  (float)point.Z(),
//                                                   2, (Size_t)0.5);
//       std::cout << hit.IRow() << "    " << hit.X() << " " << hit.Y() << " " << hit.Z() << std::endl;
//       std::cout << point.IRow() << "    " << point.X() << " " << point.Y() << " " << point.Z() << std::endl;
// 
//       PndFTSCADisplay::Instance().Ask();
// #endif

      if ( errorX2 <= 0 ) errorX2 = errorX1;
      
      const float err2X1 = errorX1*errorX1;
      const float err2X2 = errorX2*errorX2;

      if ( errorX1 <= 0 )  ;
      else                 hit.SetErr2Y(err2X1);
      if ( errorX2 <= 0 )  ;
      else                 hit.SetErr2Z(err2X2);
      if ( errorX1 <= 0 || errorX2 <= 0 ) ;
      else                 hit.SetErrYZ(0);

      const float_v Err2Y = hit.Err2Y(), Err2Z = hit.Err2Z();
      
      double mcX =  point.X();
      double mcY =  point.Y();
      double mcZ =  point.Z();
      double angle = hit.Angle();
      
      double xl,yl,zl;
      PndFTSCAParameters::GlobalToCALocal( mcX, mcY, mcZ, angle, xl, yl, zl );
      yl += sqrt(Err2Y[0])*rand.Gaus();
      zl += sqrt(Err2Z[0])*rand.Gaus();
      double xg,yg,zg;
      PndFTSCAParameters::CALocalToGlobal( xl, yl, zl, angle, xg, yg, zg );
      hit.SetX( xg );
      hit.SetY( yg );
      hit.SetZ( zg );
    }
  }
}

  // Create new hits spreading MCPositions
void PndFTSCAPerformance::ResimulateHits( float errorX1, float errorX2 ){
  PndFTSResizableArray<PndFTSCAGBHit>& hits = const_cast<PndFTSCAGBTracker *>(fTracker)->fHits;

  const int NHits = fLocalMCPoints.Size();
  const_cast<PndFTSCAGBTracker *>(fTracker)->fNHits = NHits;
  // hits.Clear();
  hits.Resize(NHits);
  fHitLabels.Resize(NHits);
  PndFTSResizableArray<FTSCAStrip> &fStrips = const_cast<PndFTSCAGBTracker *>(fTracker)->fFStrips; // create a virtual strip for each hit (currently strips position is not used for FTS, so don't have to fill them)
  PndFTSResizableArray<FTSCAStrip> &bStrips = const_cast<PndFTSCAGBTracker *>(fTracker)->fBStrips; // create a virtual strip for each hit

  fStrips.Resize(NHits);
  bStrips.Resize(NHits);
  
  
  static TRandom3 rand;
  for ( int ih = 0; ih < NHits; ih++ ) {
    PndFTSCALocalMCPoint &mcP = fLocalMCPoints[ih];

    PndFTSCAGBHit &hit = hits[ih];
    hit.SetID(ih);
    PndFTSCAHitLabel &l = fHitLabels[ih];
    l.fLab[0] = mcP.TrackI();
    l.fLab[1] = -1;
    l.fLab[2] = -1;

      // get errors
    if ( errorX2 <= 0 ) errorX2 = errorX1;
    
    const float err2X1 = errorX1*errorX1;
    const float err2X2 = errorX2*errorX2;
#ifdef STAR_HFT
    if ( errorX1 <= 0 )  hit.SetErr2Y(mcP.Err2Y());
    else                 hit.SetErr2Y(err2X1);
    if ( errorX2 <= 0 )  hit.SetErr2Z(mcP.Err2Z());
    else                 hit.SetErr2Z(err2X2);
#else
    hit.SetErr2Y(err2X1); hit.SetErr2Z(err2X2);
#endif
    float_v Err2Y = hit.Err2Y(), Err2Z = hit.Err2Z();

    double mcX =  mcP.X();
    double mcY =  mcP.Y();
    double mcZ =  mcP.Z();

    hit.SetIRow( mcP.IRow() );
    hit.SetAngle( mcP.Angle() );
    
    double mcXTmp  =  mcX*cos( hit.Angle() ) +  mcY*sin( hit.Angle() );
    double mcYTmp  = -mcX*sin( hit.Angle() ) +  mcY*cos( hit.Angle() );
    mcYTmp -= sqrt(Err2Y[0])*rand.Gaus();
    mcZ -= sqrt(Err2Z[0])*rand.Gaus();
    mcX = mcXTmp*cos( hit.Angle() ) -  mcYTmp*sin( hit.Angle() );
    mcY = mcXTmp*sin( hit.Angle() ) +  mcYTmp*cos( hit.Angle() );
    
    hit.SetX( mcX );
    hit.SetY( mcY );
    hit.SetZ( mcZ );

    hit.SetFStripP( &fStrips[ih] );
    hit.SetBStripP( &bStrips[ih] );
  }
}

  // Match hits with closest MCPoint
void PndFTSCAPerformance::RematchHits(){
  PndFTSResizableArray<PndFTSCAGBHit>& hits = const_cast<PndFTSCAGBTracker *>(fTracker)->fHits;

  const int NHits = fTracker->NHits();

  for ( int ih = 0; ih < NHits; ih++ ) {
    PndFTSCAGBHit &hit = hits[ih];
    PndFTSCAHitLabel &l = fHitLabels[ih];

    int MCindex = -1;
    float R2 = 10e30;
    for(int iMCPoint=0; iMCPoint < fLocalMCPoints.Size(); iMCPoint++)
    {
      if(fLocalMCPoints[iMCPoint].IRow()   != hit.IRow()) continue;

      const float dY = hit.Y() - fLocalMCPoints[iMCPoint].Y();
      const float dZ = hit.Z() - fLocalMCPoints[iMCPoint].Z();
      const float r2 = dY*dY + dZ*dZ;
      
      if( r2 < R2 ) {
        MCindex = iMCPoint;
        R2 = r2;
      }
    }
    if(MCindex == -1)
    {
      l.fLab[0] = -1;
      l.fLab[1] = -1;
      l.fLab[2] = -1;
      continue;
    }

    
    l.fLab[0] = fLocalMCPoints[MCindex].TrackI();
    l.fLab[1] = -1;
    l.fLab[2] = -1;
  }
}

#ifdef DRIFT_TUBES
struct Vector3 {
  Vector3( const float& x, const float& y, const float& z):fx(x),fy(y),fz(z){};
  float X() const { return fx; }
  float Y() const { return fy; }
  float Z() const { return fz; }
 private:
  float fx,fy,fz;
};

bool operator<(const Vector3& a, const Vector3& b) {
  if (a.X() != b.X())
    return a.X() < b.X();
  else
    if (a.Y() != b.Y())
      return a.Y() < b.Y();
    else
      return a.Z() < b.Z();
}

 // Rid of hits with same wires positions
void PndFTSCAPerformance::CombineHits(){
  PndFTSResizableArray<PndFTSCAGBHit>& hits = const_cast<PndFTSCAGBTracker *>(fTracker)->fHits;
  const int NHits = hits.Size();
  map<Vector3,float> minR[PndFTSCAParameters::MaxNStations];
  map<Vector3,bool> isPileuped[PndFTSCAParameters::MaxNStations];
  for (int i = 0; i < NHits; i++){
    PndFTSCAGBHit &l = hits[i];
    const int iS = l.IRow();
    
    Vector3 v(l.X(), l.Y(), l.Z());
    if ( minR[iS].count(v) > 0 ) {
      minR[iS][v] = min(l.R(), minR[iS][v]);
      isPileuped[iS][v] = true;
      //assert(iS >= PndFTSCAParameters::MaxNStations/*PndFTSCAParameters::NMVDStations*/); // MVD
    }
    else {
      minR[iS][v] = l.R();
      isPileuped[iS][v] = false;
    }
  }

  PndFTSResizableArray<PndFTSCAGBHit> hits2;
  int nHits2 = 0;
  for( int iS = 0; iS < PndFTSCAParameters::MaxNStations; iS++ ) {
    nHits2 += minR[iS].size();
  }
  hits2.Resize(nHits2);
  for (int i = 0, j = 0; i < NHits; i++){
    PndFTSCAGBHit &l = hits[i];
    Vector3 v(l.X(), l.Y(), l.Z());
    const int iS = l.IRow();

    if ( minR[iS][v] == l.R() ) {
      l.SetIsPileuped(isPileuped[iS][v]);
      hits2[j] = l;
      j++;
    }
  }

#ifndef HLTCA_STANDALONE
    // merge labels
  for (int j = 0; j < nHits2; j++){
    PndFTSCAGBHit &l2 = hits2[j];
    const int id2 = l2.ID();
    Vector3 v2(l2.X(), l2.Y(), l2.Z());
    for (int i = 0; i < NHits; i++){
      PndFTSCAGBHit &l = hits[i];
      const int id = l.ID();
      if ( id2 == id ) continue;
      Vector3 v(l.X(), l.Y(), l.Z());
      if ( l.X() != l2.X() || l.Y() != l2.Y() || l.Z() != l2.Z() ) continue;

      int k = 0;
      bool flag = false;
      for( ; k < 3 && fHitLabels[id2].fLab[k] != -1; k++ )
        if ( fHitLabels[id2].fLab[k] == fHitLabels[id].fLab[0] ) flag = true;
      if (flag) continue;
      if (k >= 3) continue;
      fHitLabels[id2].fLab[k] = fHitLabels[id].fLab[0]; // suppose we have only 1 label from the begining
    }
  }
#endif
  
  const_cast<PndFTSCAGBTracker *>(fTracker)->SetNHits(nHits2);
  for (int i = 0; i < nHits2; i++){
    hits[i] = hits2[i];
  }

  // do not need to correct labels, since they are matched with hits by hit::fID
}

 // Rid of hits with same wires positions
void PndFTSCAPerformance::DivideHitsOnLR(){
  PndFTSResizableArray<PndFTSCAGBHit>& hits = const_cast<PndFTSCAGBTracker *>(fTracker)->fHits;
  const int NHits = hits.Size();

  int nMvdHits = 0;

//   for (int i = 0; i < NHits; i++){
//     PndFTSCAGBHit &hIn = hits[i];
//     if(hIn.IRow() < PndFTSCAParameters::MaxNStations /*PndFTSCAParameters::NMVDStations*/)
//       nMvdHits++;
//   }

  PndFTSResizableArray<PndFTSCAGBHit> hits2;
  const int NHits2 = nMvdHits + 2*(NHits-nMvdHits);
  hits2.Resize(NHits2);

  for(int i=0; i<nMvdHits; i++)
  {
    hits2[i] = hits[i];
  }

  for (int i = nMvdHits; i < NHits; i++){
    PndFTSCAGBHit &hIn = hits[i];
    PndFTSCAGBHit &hOut1 = hits2[2*i-nMvdHits];
    PndFTSCAGBHit &hOut2 = hits2[2*i-nMvdHits+1];
    
    const float k = 1.5; // diff between real size and sigma
    const float r = hIn.R();

    // const float x = hIn.X();
    // const float y = hIn.Y();
    


     
    const float errT = 0.02/k; // tangential error 2 mm
//    cout << 0.02/k << " " << sqrt(hIn.Err2R()) << endl;
    const float errN = sqrt(hIn.Err2R()); // normal error
    // const float R = 0.5/k; // TODO
    // const float e0 = errT/R;
    // const float e1 = errN/R;

    hOut1 = hIn;
    {
      const double beta = hIn.Beta(); // strip angle

      const double C11 = errN*errN;
      const double C22 = hIn.Err2X2();
      const double s = sin(beta);
      const double c = cos(beta);
      hOut1.SetErr2X0( errT*errT );
      hOut1.SetErrX12( -s*c*(C11-C22) );
      hOut1.SetErr2X1( c*c*C11+s*s*C22 );
      hOut1.SetErr2X2( s*s*C11+c*c*C22 );
    }

#ifdef PANDA_STT
    const float x = hIn.X();
    const float y = hIn.Y();
    const float a = hIn.Angle();
    
    hOut1.SetX( x + r*cos(a) ); // neglect 3deg angle
    hOut1.SetY( y - r*sin(a) );
#else //PANDA_FTS
    const float x = hIn.X();
    
    hOut1.SetX( x ); // neglect 3deg angle
#endif
    hOut1.SetIsLeft( false );
    hOut2 = hOut1;

#ifdef PANDA_STT
    hOut2.SetX( x - r*cos(a) );
    hOut2.SetY( y + r*sin(a) );
#else
    hOut1.SetX( x );
#endif
    hOut2.SetIsLeft( true );
    
    hOut1.SetID(hIn.ID()*2-nMvdHits);
    hOut2.SetID(hIn.ID()*2-nMvdHits+1);
  }

  const_cast<PndFTSCAGBTracker *>(fTracker)->SetNHits(NHits2);
  for (int i = 0; i < NHits2; i++){
    hits[i] = hits2[i];
  }
  
    // create additional hitLables
  PndFTSResizableArray<PndFTSCAHitLabel> labs2;
  const int NLabs = fHitLabels.Size();
  labs2.Resize(nMvdHits + 2*(NLabs-nMvdHits));
  for (int i = 0; i < nMvdHits; i++){
    labs2[i] = fHitLabels[i];
  }
  for (int i = nMvdHits; i < NLabs; i++){
    labs2[2*i-nMvdHits] = fHitLabels[i];
    labs2[2*i-nMvdHits+1] = fHitLabels[i];
  }
  fHitLabels.Resize(nMvdHits + 2*(NLabs-nMvdHits));
  for (int i = 0; i < nMvdHits + 2*(NLabs-nMvdHits); i++){
    fHitLabels[i] = labs2[i];
  }

  // clean labels (half of created hits are "fake")
  for (int iHit = nMvdHits; iHit < hits.Size(); iHit+=2)
  {
    const PndFTSCAGBHit &hitR = hits[iHit];
    const PndFTSCAGBHit &hitL = hits[iHit+1];
    
    int iMCP = -1;
    float dxMin = 1.e6;
    for( int k = 0; k < 3 && iMCP == -1; k++ ) 
    {
      const int iMCTrack = fHitLabels[iHit].fLab[k];
      if (iMCTrack < 0) continue;

      const PndFTSCAMCTrack &mcTrack = fMCTracks[iMCTrack];

      int nFirstMC = mcTrack.FirstMCPointID();
      int nMCPoints = mcTrack.NMCPoints();

      // float r2Min = 1e10;
      const PndFTSCALocalMCPoint *points = &(fLocalMCPoints.Data()[nFirstMC]);
      for(int iMCPoint=0; iMCPoint<nMCPoints; iMCPoint++)
      {
	const PndFTSCALocalMCPoint &p = points[iMCPoint];

	if(fabs(p.Z() - hitR.Z()) > 0.5) continue;

	float sinA = fTracker->GetParameters().Station(hitR.IRow()).f.sin;
	float cosA = fTracker->GetParameters().Station(hitR.IRow()).f.cos;
	float correction = cosA*(p.Px()/p.Pz()) + sinA*(p.Py()/p.Pz());
	float dxR = cosA*hitR.X() + sinA*hitR.Y() + hitR.R()*sqrt(1.+correction*correction) - cosA*p.X()- sinA*p.Y();
	float dxL = cosA*hitL.X() + sinA*hitL.Y() - hitL.R()*sqrt(1.+correction*correction) - cosA*p.X()- sinA*p.Y();

	float dx = dxR;
	if(fabs(dxL)<fabs(dxR))
	  dx = dxL;
	
	if(fabs(dx)<dxMin)
	{
// 	  if( fabs(dx) < 10*sqrt(hitR.Err2R())*sqrt(1.+correction*correction) )
	  {
	    iMCP = iMCPoint + nFirstMC;
	    dxMin = fabs(dx);
	  }
	}
      }
    }
  
    if (iMCP < 0) 
    {
      for(int iLabel=0; iLabel<3; iLabel++)
      {
        fHitLabels[iHit].fLab[iLabel] = -1;
        fHitLabels[iHit+1].fLab[iLabel] = -1;
      }
      continue;
    }
    
    const PndFTSCALocalMCPoint &p = fLocalMCPoints[iMCP];
    
    float sinA = fTracker->GetParameters().Station(hitR.IRow()).f.sin;
    float cosA = fTracker->GetParameters().Station(hitR.IRow()).f.cos;
    float correction = cosA*(p.Px()/p.Pz()) + sinA*(p.Py()/p.Pz());
    float dxR = cosA*hitR.X() + sinA*hitR.Y() + hitR.R()*sqrt(1.+correction*correction) - cosA*p.X()- sinA*p.Y(); // dx = hit.X() - hit.R() - p.X();
    float dxL = cosA*hitL.X() + sinA*hitL.Y() - hitL.R()*sqrt(1.+correction*correction) - cosA*p.X()- sinA*p.Y(); // dx = hit.X() + hit.R() - p.X();
    
    if(fabs(dxR) < fabs(dxL))
      for(int iLabel=0; iLabel<3; iLabel++)
        fHitLabels[iHit+1].fLab[iLabel] = -1;
    else
      for(int iLabel=0; iLabel<3; iLabel++)
        fHitLabels[iHit].fLab[iLabel] = -1;  
  }  

#if 0
  const int NMCPoints = GetMCPoints()->Size();
  vector<vector<int> > hitsOnPoint(NMCPoints);
  for(int iH = nMvdHits; iH < hits.Size(); iH++) {
    const PndFTSCAGBHit &h = hits[iH];
    int id = GetMCPoint(h);
    if(id < 0) {
        // delete match with MCTracks
      for( int k = 0; k < 3; k++ ) {
        fHitLabels[h.ID()].fLab[k] = -1;
      }
      continue;
    }
    hitsOnPoint[id].push_back(iH);
  }
  
std::cout << "fHitLabels " << fHitLabels.Size() << std::endl;
  int nNegative = 0;
  for(int iHL=0; iHL<fHitLabels.Size(); iHL++)
  {
    std::cout << "iHL " << iHL << " " << fHitLabels[iHL].fLab[0] << " " <<fHitLabels[iHL].fLab[1] << " " << fHitLabels[iHL].fLab[2] << " " <<  std::endl;
    if(fHitLabels[iHL].fLab[0] < 0 && fHitLabels[iHL].fLab[1] < 0 && fHitLabels[iHL].fLab[2] < 0)
      nNegative++;
  }  
std::cout << "nNegative " << nNegative << std::endl;
  
    // clean double hits. Comment this out to calculate efficiency without tacking into account left-right ambiguity
// #ifdef INCLUDE_LR_EFF
  for(int iT=0; iT<NMCPoints; iT++) {
    const vector<int> &hop = hitsOnPoint[iT];
    if (hop.size() == 0) continue;
    float r2Min = 1e10;
    int iBestH = -1;
    for( unsigned int iH = 0; iH < hop.size(); iH++ ) {
      const PndFTSCAGBHit &h = hits[hop[iH]];
      const int iMCP = GetMCPoint(h);
      if (iMCP < 0) continue;
      const PndFTSCALocalMCPoint &p = fLocalMCPoints[iMCP];
      
      float sinA = fTracker->GetParameters().Station(h.IRow()).f.sin;
      float cosA = fTracker->GetParameters().Station(h.IRow()).f.cos;
      float correction = cosA*(p.Px()/p.Pz()) + sinA*(p.Py()/p.Pz());
      float dx = cosA*h.X() + sinA*h.Y() + h.R()*sqrt(1.+correction*correction) - cosA*p.X()- sinA*p.Y(); // dx = hit.X() - hit.R() - p.X();
      if(h.IsLeft())
        dx =  cosA*h.X() + sinA*h.Y() - h.R()*sqrt(1.+correction*correction) - cosA*p.X()- sinA*p.Y(); // dx = hit.X() + hit.R() - p.X();
      
      if (fabs(dx) < r2Min) {
        r2Min = fabs(dx);
        iBestH = iH;
      }
    }

// #ifndef HLTCA_STANDALONE
std::cout << "Ololo!!  " << hop.size() << std::endl;
    for( unsigned int iH = 0; iH < hop.size(); iH++ ) {
      if (static_cast<int>(iH) == iBestH) continue;
      const PndFTSCAGBHit &h = hits[hop[iH]];
//       if (h.R()*h.R() < 9.f*h.Err2R()) continue; // can't really distinguish left and right side of this tubes

      const int id = GetMCPoint(h);
      if(id < 0) continue;
      const int iMCT = fLocalMCPoints[id].TrackI();
        // delete match with MCTrack
      for( int k = 0; k < 3; k++ ) {
        if ( fHitLabels[h.ID()].fLab[k] == iMCT ) {
          fHitLabels[h.ID()].fLab[k] = -1;
        }
      }
    }
// #endif
  }
// #endif // 1

std::cout << "fHitLabels " << fHitLabels.Size() << std::endl;
  int nNegative2 = 0;
  for(int iHL=0; iHL<fHitLabels.Size(); iHL++)
  {
    std::cout << "iHL " << iHL << " " << fHitLabels[iHL].fLab[0] << " " <<fHitLabels[iHL].fLab[1] << " " << fHitLabels[iHL].fLab[2] << " " <<  std::endl;
    if(fHitLabels[iHL].fLab[0] < 0 && fHitLabels[iHL].fLab[1] < 0 && fHitLabels[iHL].fLab[2] < 0)
      nNegative2++;
  }  
std::cout << "nNegative " << nNegative2 << std::endl;
#endif
}
#endif // PANDA_STT



#endif //DO_TPCCATRACKER_EFF_PERFORMANCE
