// $Id: PndCAPerformance.cxx,v 1.13 2010/09/01 10:38:27 ikulakov Exp $
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

#include "PndCAPerformance.h"

#include "PndCACounters.h"
#include "PndCAPerformanceBase.h"
#include "PndCAGlobalPerformance.h"
#include "PndCAParam.h"

#include "PndCAGBHit.h"
#include "PndCAMCTrack.h"
#ifndef HLTCA_STANDALONE
#include "PndCAMCPoint.h"
#endif
#include "PndCAGBTrack.h"
#include "PndCAGBTracker.h"

//#include "PndCADisplay.h"

#include "PndCAParameters.h"

#include "TRandom3.h" //dbg
#include "TMath.h"
#include "TROOT.h"
#include "Riostream.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TStyle.h"

PndCAPerformance::PndCAPerformance()
{
  static bool first_call = true;

  if (first_call){
    typedef TSubPerformance TSP;
    
      /// Just define here all sub-performances
      /// TSP(new __ClassName__               , __Name__      ),
    const int NSPerfo = 1;
    const TSP perfos[NSPerfo] = {
      TSP(new PndCAGlobalPerformance, "Global Performance"),
    };

    subPerformances.resize(NSPerfo);
    for (int iP = 0; iP < NSPerfo; iP++){
      subPerformances[iP] = perfos[iP];
    }
  }
  first_call = false;
  //* constructor
}

PndCAPerformance::~PndCAPerformance()
{
  //* destructor
  fHitLabels.clear();
  fMCTracks.clear();
  for (unsigned int iPerf = 0; iPerf < subPerformances.size(); iPerf++){ // TODO: why we can't do this in subPerformances.~destructor() ??
    if (subPerformances[iPerf].perf) delete subPerformances[iPerf].perf;
  }
}

PndCAPerformance &PndCAPerformance::Instance()
{
  // reference to static object
  static PndCAPerformance gPndCAPerformance;
  return gPndCAPerformance;
}

bool PndCAPerformance::SetNewEvent(PndCAGBTracker* const tracker, string mcTracksFile, string mcPointsFile)
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
} // void PndCAPerformance::SetNewEvent

void PndCAPerformance::InitSubPerformances()
{
    // Init subperformances
  static bool first_call = true;

  for (unsigned int iPerf = 0; iPerf < subPerformances.size(); iPerf++){
    subPerformances[iPerf]->SetNewEvent(fTracker, &fHitLabels, &fMCTracks, &fLocalMCPoints);
  }

  if (first_call) CreateHistos();
  
  first_call = false;
} // void PndCAPerformance::InitSubPerformances

void PndCAPerformance::CreateHistos()
{
  for (unsigned int iPerf = 0; iPerf < subPerformances.size(); iPerf++){
    if(!(subPerformances[iPerf]->IsHistoCreated())) 
      subPerformances[iPerf]->CreateHistos(subPerformances[iPerf].name, fOutputFile);
  }
}

bool PndCAPerformance::CreateHistos(string name)
{
  unsigned i = 0;
  for( ; i < (subPerformances.size()) && (subPerformances[i].name != name); i++);
  if(!(subPerformances[i]->IsHistoCreated()) && i != subPerformances.size()) 
    subPerformances[i]->CreateHistos(subPerformances[i].name, fOutputFile);
  if ( i == subPerformances.size() ) return 0;
  return 1;
}


void PndCAPerformance::WriteDir2Current( TObject *obj )
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


void PndCAPerformance::WriteHistos()
{
  if(fOutputFile) WriteDir2Current(fOutputFile);
}

void PndCAPerformance::ExecPerformance()
{
  fStatNEvents++;

  for (unsigned int iPerf = 0; iPerf < subPerformances.size(); iPerf++){
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

PndCAPerformanceBase* PndCAPerformance::GetSubPerformance(string name)
{
  unsigned i = 0;
  for( ; (i < subPerformances.size()) && (subPerformances[i].name != name); i++);
  //ASSERT ( i != subPerformances.size() , " Incorrect name of subPerformance used.");
  if ( i == subPerformances.size() ) return 0;
  return subPerformances[i].perf;
} // PndCAPerformanceBase* PndCAPerformance::GetSubPerformance(string name)


  /// -------------------- Read\write MC information ---------------------
void PndCAPerformance::WriteMCEvent( FILE *file ) const
{
  // write MC information to the file
  int n = fMCTracks.size();
  std::fwrite( &n, sizeof( int ), 1, file );
  n = fHitLabels.size();
  std::fwrite( &n, sizeof( int ), 1, file );
  std::fwrite( &fMCTracks[0], sizeof( PndCAMCTrack ), fMCTracks.size(), file );
  std::fwrite( &fHitLabels[0], sizeof( PndCAHitLabel ), fHitLabels.size(), file );
}

void PndCAPerformance::ReadMCEvent( FILE *file )
{
  // read mc info from the file
  int read;
  int n;

  read = std::fread( &n, sizeof( int ), 1, file );
  assert( read == 1 );
  fMCTracks.resize( n );
  read = std::fread( &n, sizeof( int ), 1, file );
  assert( read == 1 );
  fHitLabels.resize( n );

  read = std::fread( &fMCTracks[0], sizeof( PndCAMCTrack ), fMCTracks.size(), file );
  assert( read == (int) fMCTracks.size() );

  read = std::fread( &fHitLabels[0], sizeof( PndCAHitLabel ), fHitLabels.size(), file );
  assert( read == (int) fHitLabels.size() );
  UNUSED_PARAM1(read);
//   for (int i = 0; i < fMCTracks.size(); i++){
//     PndCAMCTrack& mc = fMCTracks[i];
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
//   for (int i = 0; i < fHitLabels.size(); i++){
//     PndCAHitLabel& l = fHitLabels[i];
//     for (int iPar = 0; iPar < 3; iPar++)
//      std::cout << l.fLab[iPar] << " ";
//     std::cout << std::endl;
//   };
}

void PndCAPerformance::ReadLocalMCPoints( FILE *file )
{
  
  int read;
  int n;

  read = std::fread( &n, sizeof( int ), 1, file );
  assert( read == 1 );
  fLocalMCPoints.resize( n );

  read = std::fread( &fLocalMCPoints[0], sizeof( PndCALocalMCPoint ), fLocalMCPoints.size(), file );
  assert( read == (int) fLocalMCPoints.size() );
  UNUSED_PARAM1(read);
} // void PndCAPerformance::ReadLocalMCPoints( FILE *file )


void PndCAPerformance::SetMCTracks(vector<PndCAMCTrack>& mcTracks)
{
  const int N = mcTracks.size();
  fMCTracks.resize(N);
  for(int i = 0; i < N; i++){
    fMCTracks[i] = mcTracks[i];
  }
}

void PndCAPerformance::SetMCPoints(vector<PndCALocalMCPoint>& mcPoints)
{
  const int N = mcPoints.size();
  fLocalMCPoints.resize(N);
  for(int i = 0; i < N; i++){
    fLocalMCPoints[i] = mcPoints[i];
  }
}

void PndCAPerformance::SetHitLabels(vector<PndCAHitLabel>& hitLabels)
{
  const int N = hitLabels.size();
  fHitLabels.resize(N);
  for(int i = 0; i < N; i++){
    fHitLabels[i] = hitLabels[i];
  }					   
}

void PndCAPerformance::SaveDataInFiles(string prefix) const
{

  {
    ofstream ofile((prefix+"hitLabels.data").data(),ios::out|ios::app);
    const int Size = fHitLabels.size();
    ofile << Size << std::endl;
    for (unsigned int i = 0; i < fHitLabels.size(); i++){
      const PndCAHitLabel &l = fHitLabels[i];
      ofile << l;
    }
    ofile.close();
  }

  {
    ofstream ofile((prefix+"MCTracks.data").data(),ios::out|ios::app);
    const int Size = fMCTracks.size();
    ofile << Size << std::endl;
    for (unsigned int i = 0; i < fMCTracks.size(); i++){
      const PndCAMCTrack &l = fMCTracks[i];
      ofile << l;
    }
    ofile.close();
  }

  {
    ofstream ofile((prefix+"MCPoints.data").data(),ios::out|ios::app);
    const int Size = fLocalMCPoints.size();
    ofile << Size << std::endl;
    for (unsigned int i = 0; i < fLocalMCPoints.size(); i++){
      const PndCALocalMCPoint &l = fLocalMCPoints[i];
      ofile << l;
    }
    ofile.close();
  }
}

bool PndCAPerformance::ReadDataFromFiles(string prefix)
{

  {
    ifstream ifile((prefix+"hitLabels.data").data());
    if ( !ifile.is_open() ) return 0;
    int Size;
    ifile >> Size;
    fHitLabels.resize(Size);
    for (int i = 0; i < Size; i++){
      PndCAHitLabel &l = fHitLabels[i];
      ifile >> l;
    }
    ifile.close();
  }

 
  {
    ifstream ifile((prefix+"MCTracks.data").data());
    if ( !ifile.is_open() ) return 0;
    int Size;
    ifile >> Size;
    fMCTracks.resize(Size);
    for (int i = 0; i < Size; i++){
      PndCAMCTrack &l = fMCTracks[i];
      ifile >> l;
    }
    ifile.close();
  }


  {
    ifstream ifile((prefix+"MCPoints.data").data());
    if ( !ifile.is_open() ) return 0;
    int Size;
    ifile >> Size;
    fLocalMCPoints.resize(Size);
    for (int i = 0; i < Size; i++){
      PndCALocalMCPoint &l = fLocalMCPoints[i];
      ifile >> l;
      if( l.IRow() >= PndCAParameters::MaxNStations ){
	cout<<"wrong mc point station number: "<<(int) l.IRow()<<" out of "<<PndCAParameters::MaxNStations<<endl;
	l.SetIRow( PndCAParameters::MaxNStations-1);
      }
    }
    ifile.close();
  }


  // calculate needed additional info
  {
    const int NMCTracks = fMCTracks.size();
    // NMCRows
    for (int i = 0; i < NMCTracks; ++i ){
      PndCAMCTrack& t = fMCTracks[i];
      vector<int> rows;
      rows.resize( PndCAParameters::MaxNStations, 0 );
      for (int j = 0, iP = t.FirstMCPointID(); j < t.NMCPoints(); iP++, j++){
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
    
    vector<int> zero(PndCAParameters::MaxNStations, 0);
    vector< vector<int> > nmchits(NMCTracks,zero);
    vector<PndCAGBHit>& hits = const_cast<PndCAGBTracker *>(fTracker)->fHits;
    
    for ( int i = 0; i < fTracker->NHits(); i++) {
      int id = hits[i].ID();
      for ( int il = 0; il < 3; il++ ) {
        int trackId = HitLabel(id).fLab[il];
        if(trackId < 0) continue;
        ASSERT(trackId < NMCTracks, "Incorrect MCPoints of MCTracks file. " <<  trackId << " < " << NMCTracks );
	if(  hits[i].IRow() >= PndCAParameters::MaxNStations ){
	  cout<<"wrong hit station number: "<<(int) hits[i].IRow()<<" out of "<<PndCAParameters::MaxNStations<<endl;
	}
        assert( hits[i].IRow() < PndCAParameters::MaxNStations );
        nmchits[trackId][hits[i].IRow()]++;
      }
    }

    for (int i = 0; i < NMCTracks; ++i ){
      PndCAMCTrack& t = fMCTracks[i];
      int nRows = 0;

      int nHitContRows = 0;
      int istaold = -1, ncont=0;
      for (int j = 0; j < PndCAParameters::MaxNStations; ++j ) {
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

    // PV
    fPV[0] = fPV[1] = fPV[2] = 0;
    for (int i = 0; i < NMCTracks; ++i ){
      PndCAMCTrack& t = fMCTracks[i];
      if (t.MotherId() > 0) continue;
      fPV[0] = t.X();
      fPV[1] = t.Y();
      fPV[2] = t.Z();
      break;
    }

  }
  
  return 1;
}


//#include "PndCADisplay.h"


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
void PndCAPerformance::CombineHits(){

  vector<PndCAGBHit>& hits = const_cast<PndCAGBTracker *>(fTracker)->fHits;
  //unsigned int nHits = fTracker->NHits();
  int nLabels = fHitLabels.size();

  int nHitsNew=0;
  for( unsigned int ih = 0; ih < hits.size(); ih++ ){
    PndCAGBHit &hi = hits[ih];
    int jh=0;
    for( ; jh<nHitsNew; jh++ ){
      PndCAGBHit &hj = hits[jh];
      if( hi.IRow() != hj.IRow() ) continue;
      double dx = hi.GlobalX() - hj.GlobalX();
      double dy = hi.GlobalY() - hj.GlobalY();
      double dz = hi.Z() - hj.Z();
      if( dx*dx+dy*dy+dz*dz >  1.e-8 ) continue;
      break;
    }
    PndCAGBHit &hj = hits[jh];
    if( jh==nHitsNew ){ // store hit
      hj = hi;
      nHitsNew++;
      continue;
    }
    // merge
    if(hi.IRow() < PndCAParameters::NMVDStations){
      cout<<"Pile up in MVD, something is wrong"<<endl;
      exit(0);
    }
    bool mergeLabels = ( hj.ID() < nLabels && hi.ID()<nLabels );
    PndCAHitLabel *lto = 0, *lfrom = 0;
    
    if( mergeLabels ){ 
      lto = &fHitLabels[hj.ID()];
      lfrom = &fHitLabels[hi.ID()];
    }
    
    if( hi.R() < hj.R() ){ // merge hits
      if( mergeLabels ){
	lto   = &fHitLabels[hi.ID()];
	lfrom = &fHitLabels[hj.ID()];
      }
      hj = hi;
    }
    if( mergeLabels ){ // merge labels
      int k = 0;      
      for( ; k < 3 && lto->fLab[k] >=0; k++ );
      if ( k>=3 ) continue; // no space to store new label
      for( int a=0; a<3; a++ ){
	int lab = lfrom->fLab[a];
	if( lab<0 ) continue;
	bool store = true;
	for( int b=0; b<k; b++ ) if( lto->fLab[b]==lab ) store=false;
	if( store ) lto->fLab[k] = lab; 
      }
    }
  }  

  const_cast<PndCAGBTracker *>(fTracker)->fNHits = nHitsNew;
  hits.resize(nHitsNew);
}



#endif //DO_TPCCATRACKER_EFF_PERFORMANCE
