//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmDetFactory.cc,v 1.12 2007/05/24 08:07:40 klausg Exp $
//
// Description:
//      Class FsmDetFactory
//
//  Factory for FsmDetectors
//
//  This software was developed for the PANDA collaboration.  If you
//  use all or part of it, please give an appropriate acknowledgement.
//
// Author List:
//      Klaus Goetzen                    Original Author
//
// Copyright Information:
//      Copyright (C) 2006              GSI
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "PndFsmDetFactory.h"

//-------------
// C Headers --
//-------------

//---------------
// C++ Headers --
//---------------
#include <iostream>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
//#include "ErrLogger/ErrLog.hh"
#include "StrTok.h"


#include "PndFsmEmcBarrel.h"
#include "PndFsmEmcFwCap.h"
#include "PndFsmEmcBwCap.h"
#include "PndFsmEmcFS.h"
#include "PndFsmStt.h"
#include "PndFsmTpc.h"
#include "PndFsmMdcFS.h"
#include "PndFsmMdcTS.h"
#include "PndFsmEffTracker.h"
#include "PndFsmDrcBarrel.h"
#include "PndFsmDrcDisc.h"
#include "PndFsmMvd.h"
#include "PndFsmMvd2.h"
#include "PndFsmTof.h"
#include "PndFsmRich.h"
#include "PndFsmSimpleTracker.h"
#include "PndFsmCmpDet.h"
#include "PndFsmCombiDet.h"
#include "PndFsmMvdPid.h"
#include "PndFsmSttPid.h"
#include "PndFsmSimpleVtx.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

//----------------
// Constructors --
//----------------

PndFsmDetFactory::PndFsmDetFactory()
{
}

//--------------
// Destructor --
//--------------

PndFsmDetFactory::~PndFsmDetFactory()
{
}

//--------------
// Operations --
//--------------

PndFsmAbsDet*
PndFsmDetFactory::create(std::string &name,ArgList &par)
{
  if (name=="EmcBarrel")
    return (PndFsmAbsDet*)( new PndFsmEmcBarrel(par) ) ;
  else
  if (name=="EmcFwCap")
    return (PndFsmAbsDet*)( new PndFsmEmcFwCap(par) ) ;
  else
  if (name=="EmcBwCap")
    return (PndFsmAbsDet*)( new PndFsmEmcBwCap(par) ) ;
  else
  if (name=="EmcFS")
    return (PndFsmAbsDet*)( new PndFsmEmcFS(par) ) ;
  else
  if (name=="Tpc")
    return (PndFsmAbsDet*)( new PndFsmTpc(par) ) ;
  else
  if (name=="Stt")
    return (PndFsmAbsDet*)( new PndFsmStt(par) ) ;
  else
  if (name=="MdcFS")
    return (PndFsmAbsDet*)( new PndFsmMdcFS(par) ) ;
  else
  if (name=="MdcTS")
    return (PndFsmAbsDet*)( new PndFsmMdcTS(par) ) ;
  else
  if (name=="EffTracker")
    return (PndFsmAbsDet*)( new PndFsmEffTracker(par) ) ;
  else
  if (name=="DrcBarrel")
    return (PndFsmAbsDet*)( new PndFsmDrcBarrel(par) ) ;
  else
  if (name=="DrcDisc")
    return (PndFsmAbsDet*)( new PndFsmDrcDisc(par) ) ;
  else
  if (name=="Mvd")
    return (PndFsmAbsDet*)( new PndFsmMvd(par) ) ;
  else
  if (name=="Mvd2")
    return (PndFsmAbsDet*)( new PndFsmMvd2(par) ) ;
  else
  if (name=="Tof")
    return (PndFsmAbsDet*)( new PndFsmTof(par) ) ;
  else
  if (name=="Rich")
    return (PndFsmAbsDet*)( new PndFsmRich(par) ) ;
  else
  // --- scrutiny process options
  if (name=="ScSttAlone" || name=="ScSttMvd" || name=="ScSttMvdGem" ||
      name=="ScSttGem"   || name=="ScMvdGem" || name=="ScMvdGemFts"    ||
      name=="ScMvdFts"   || name=="ScGemFts" ||  name=="ScFts" ) {
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmSimpleTracker(par));
    aDet->setName(name);
    return aDet;
  }
  else
  if (name=="MvdPid")
    return (PndFsmAbsDet*)( new PndFsmMvdPid(par) );
  else
  if (name=="SttPid")
    return (PndFsmAbsDet*)( new PndFsmSttPid(par) );
  if (name=="ScVtxMvd"   || name=="ScVtxNoMvd") {
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmSimpleVtx(par));
    aDet->setName(name);
    return aDet;
  }
  // general Options
  else
  if (name=="SimpleTracker")
    return (PndFsmAbsDet*)( new PndFsmSimpleTracker(par) ) ;
  else
  if (name=="SimpleVtx")
    return (PndFsmAbsDet*)( new PndFsmSimpleVtx(par) ) ;
  else
  if (name=="CmpDet")
    return (PndFsmAbsDet*)( new PndFsmCmpDet(par) ) ;
  else
  if (name=="CombiDet")
    return (PndFsmAbsDet*)( new PndFsmCombiDet(par) ) ;
  else {
    std::cout  <<" -W- (PndFsmDetFactory::create) Unknown detector: <"<<name<<">"<< std::endl;
    return 0;
  }
}

PndFsmAbsDet*
PndFsmDetFactory::create(std::string &name, std::string par)
{
  // par is a string of the form "a=1 b=2 c=3 d=4"
  // this function splits the single assignments in the string to an ArgList
  // i.e. a std::list<std::string> of the form ("a=1","b=2","c=3","d=4")

  ArgList parList;

  CStrTok tokenizer;
  char csrc[200];

  const char *src=par.data();
  strcpy(csrc,src);

  char* token = tokenizer.GetFirst(csrc, " \t");

  while(token)
  {
    parList.push_back(token);
    token=tokenizer.GetNext(" \t");
  }

  /*
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep_blank(" ");
  tokenizer tokens_bl(par, sep_blank);

  for (tokenizer::iterator tok_iter_bl = tokens_bl.begin();tok_iter_bl!=tokens_bl.end();tok_iter_bl++)
    parList.push_back(*tok_iter_bl);
  */

  return (PndFsmAbsDet*)create(name, parList);
}

