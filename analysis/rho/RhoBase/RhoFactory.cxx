//////////////////////////////////////////////////////////////////////////
//                                                                      //
// RhoFactory                                                           //
//                                                                      //
// Object factory for RhoCandidate and Vertices (RhoVector3Err)         //
//                                                                      //
// Author List:                                                         //
// Marcel Kunze,  RUB, Feb. 01                                          //
// Copyright (C) 1999-2001, Ruhr-University Bochum.                     //
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include "RhoBase/RhoFactory.h"
#include "RhoBase/RhoCandidate.h"

ClassImp ( RhoFactory )

using namespace std;

RhoFactory* RhoFactory::fgInstance = 0;
TClonesArray* RhoFactory::fgCandBuffer = 0;
Int_t RhoFactory::fgCandPointer=0;
Int_t RhoFactory::fgCandWatermark=0;

void RhoFactory::Reset()
{
  if (fgCandBuffer != 0) fgCandBuffer->Delete();
  fgCandPointer = 0;
}

RhoFactory* RhoFactory::Instance()
{
  if ( !fgInstance ) { fgInstance= new RhoFactory(); }
  return fgInstance;
}

RhoFactory::~RhoFactory()
{
  if(fgCandBuffer){
    fgCandBuffer->Delete();
    delete fgCandBuffer;
  }
  delete fgInstance;
  fgInstance = 0;
}

//---------------------------------------

RhoCandidate* RhoFactory::NewCandidate()
{
  RhoCandidate c;
  return NewCandidate ( c );
}

RhoCandidate* RhoFactory::NewCandidate ( const RhoCandidate& c )
{
  return NewCandidate(&c);
//  if ( fgCandBuffer==0 ) { fgCandBuffer = new TClonesArray ( "RhoCandidate" ); }
//  int current = fgCandPointer++;
//  if ( current>fgCandWatermark ) { fgCandWatermark = current; }
//  if ( current<fgCandWatermark ) {
//    RhoCandidate* b = GetCandidate ( current );
//    b->RemoveAssociations();
//  }
//  new ( ( *fgCandBuffer ) [current] ) RhoCandidate ( c );
//  return GetCandidate ( current );
}

RhoCandidate* RhoFactory::NewCandidate ( const RhoCandidate* c )
{
  if ( fgCandBuffer==0 ) { fgCandBuffer = new TClonesArray ( "RhoCandidate" ); }
  int current = fgCandPointer++;
  if ( current>fgCandWatermark ) { fgCandWatermark = current; }
  if ( current<fgCandWatermark ) {
    RhoCandidate* b = GetCandidate ( current );
    b->RemoveAssociations();
  }
  new ( ( *fgCandBuffer ) [current] ) RhoCandidate ( *c );
  return GetCandidate ( current );
}



// RhoCandidate* RhoFactory::NewCandidate ( TLorentzVector p4,
//     RhoError& p4Err,
//     RhoCandListIterator& iterDau,
//     RhoVector3Err& theVertex,
//     const TParticlePDG* hypo )
// {
//   if ( fgCandBuffer==0 ) { fgCandBuffer = new TClonesArray ( "RhoCandidate" ); }
//   int current = fgCandPointer++;
//   if ( current>fgCandWatermark ) { fgCandWatermark = current; }
//   if ( current<fgCandWatermark ) {
//     RhoCandidate* c = GetCandidate ( current );
//     c->RemoveAssociations();
//   }
//   new ( ( *fgCandBuffer ) [current] ) RhoCandidate ( p4,p4Err,iterDau,theVertex,hypo );
//   return GetCandidate ( current );
// }

RhoCandidate* RhoFactory::GetCandidate ( Int_t i )
{
  if ( fgCandBuffer==0 || i>fgCandWatermark ) { return 0; }
  return ( RhoCandidate* ) ( *fgCandBuffer ) [i];
}

Int_t RhoFactory::GetCandidateWatermark()
{
  return fgCandWatermark;
}

Int_t RhoFactory::GetCandPointer()
{
  return fgCandPointer;
}

