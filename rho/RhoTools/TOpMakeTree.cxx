//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TOpMakeTree.cxx,v 1.3 2002-02-01 23:00:17 marcel Exp $
//
// Description:
//	Class TOperator
//
//	Implementation, adds 4vectors at origin
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Bob Jacobsen     		Original Author
//
// Copyright Information:
//	Copyright (C) 1996		Lawrence Berkeley Laboratory
//
// ROOT Version by Marcel Kunze, RUB
//------------------------------------------------------------------------

#include "RhoTools/TOpMakeTree.h"
#include <assert.h>
#include "RhoBase/TFactory.h"
#include "RhoBase/TCandidate.h"
#include "RhoTools/TAdd4Vertexer.h"
#include "RhoTools/TOpAdd4.h"
#include "RhoBase/TCandListIterator.h"

ClassImp(TOpMakeTree)

TBuffer &operator>>(TBuffer &buf, TOpMakeTree *&obj)
{
   obj = (TOpMakeTree *) buf.ReadObject(TOpMakeTree::Class());
   return buf;
}

#include <iostream>
using namespace std;

TOpMakeTree::TOpMakeTree(TAdd4Vertexer& v )
  : _theVertexer( (TAdd4Vertexer *) v.Clone() )
{
}

TOpMakeTree::TOpMakeTree(  )
{
    _theVertexer = new TAdd4Vertexer();
}

//--------------
// Destructor --
//--------------

TOpMakeTree::~TOpMakeTree( )
{
  delete _theVertexer;
}

//--------------
// Operations --
//--------------

void 
TOpMakeTree::PrintOn(std::ostream& o) const 
{
  o << "TOpMakeTree class ";
}

std::ostream&  
operator << (std::ostream& o, const TOpMakeTree& a) 
{ a.PrintOn(o); return o; }


//
//
TCandidate*
TOpMakeTree::CreateFromList( TCandListIterator& iterDau )
{

  //
  // loop on the candidates of the list and create the list 
  // of outgoing candidates
  //
  TCandList outGoing;
  TCandidate* dau=0;
  while( (dau = iterDau.Next()) ) 
    AddToVertexingList( *dau, outGoing );
  
  // create the vertex, using the vertexer
  TCandListIterator iterOutGoing( outGoing );
  VAbsVertex* theVertex =  _theVertexer->Compute( iterOutGoing );

  // protection against null returned values of theVertex
  if( theVertex==0 )
    {
      cerr << "fancy vertexer failed : use the TOpAdd4Vertexer instead "
		      << endl;
      iterOutGoing.Rewind();
      TOpAdd4 dummy;
      return dummy.CreateFromList( iterDau );
    }
  assert( theVertex!=0 );

  // create the Base
  TError *err = (TError *) &_theVertexer->P4Err();
  TCandidate* theCandBase = //new TCandidate(_theVertexer->P4(),*err,iterDau, *theVertex );
      TFactory::Instance()->NewCandidate(_theVertexer->P4(),*err,iterDau, *theVertex );

  // create the candidate
  //TCandidate* inComing = new TCandidate( theCandBase );

  // return the candidate
  return theCandBase;

}



