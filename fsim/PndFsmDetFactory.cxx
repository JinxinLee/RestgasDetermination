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
//      Ralf Kliemt
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
#include "PndFsmMdtPid.h"
#include "PndFsmEmcPid.h"
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
  if (name=="EmcBarrel"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmEmcBarrel(par));
    aDet->setName(name);
    aDet->setStorePid();
    return aDet;
  } else
  if (name=="EmcFwCap"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmEmcFwCap(par));
    aDet->setName(name);
    aDet->setStorePid();
    return aDet;
  } else
  if (name=="EmcBwCap"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmEmcBwCap(par));
    aDet->setName(name);
    aDet->setStorePid();
    return aDet;
  } else
  if (name=="EmcFS"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmEmcFS(par));
    aDet->setName(name);
    aDet->setStorePid();
    return aDet;
  } else
  if (name=="Stt"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmStt(par));
    aDet->setName(name);
    aDet->setStorePid();
    return aDet;
  } else
  if (name=="MdcFS"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmMdcFS(par));
    aDet->setName(name);
    return aDet;
  } else
  if (name=="MdcTS"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmMdcTS(par));
    aDet->setName(name);
    return aDet;
  } else
  if (name=="EffTracker"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmEffTracker(par));
    aDet->setName(name);
    return aDet;
  } else
  if (name=="DrcBarrel"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmDrcBarrel(par));
    aDet->setName(name);
    aDet->setStorePid();
    return aDet;
  } else
  if (name=="DrcDisc"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmDrcDisc(par));
    aDet->setName(name);
    aDet->setStorePid();
    return aDet;
  } else
  if (name=="Mvd"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmMvd(par));
    aDet->setName(name);
    aDet->setStorePid();
    return aDet;
  } else
  if (name=="Mvd2"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmMvd2(par));
    aDet->setName(name);
    aDet->setStorePid();
    return aDet;
  } else
  if (name=="Tof"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmTof(par));
    aDet->setName(name);
    aDet->setStorePid();
    return aDet;
  } else
  if (name=="Rich"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmRich(par));
    aDet->setName(name);
    aDet->setStorePid();
    return aDet;
  } else
  // --- scrutiny process options
  if (name=="ScSttAlone" || name=="ScSttMvd" || name=="ScSttMvdGem" ||
      name=="ScSttGem"   || name=="ScMvdGem" || name=="ScMvdGemFts"    ||
      name=="ScMvdFts"   || name=="ScGemFts" ||  name=="ScFts" ) {
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmSimpleTracker(par));
    aDet->setName(name);
    return aDet;
  } else
  if (name=="MvdPid") {
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmMvdPid(par) );
    aDet->setStorePid();
    return aDet;
  } else
  if (name=="ScEmcPidBarrel" || name=="ScEmcPidFwCap" || name=="ScEmcPidBwCap"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmEmcPid(par) );
	aDet->setName(name);
    aDet->setStorePid();
	return aDet;
  } else
  if (name=="SttPid"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmSttPid(par) );
    aDet->setStorePid();
    return aDet;
  }
  if (name=="ScVtxMvd" || name=="ScVtxNoMvd") {
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmSimpleVtx(par));
    aDet->setName(name);
    return aDet;
  } else
  if (name=="ScMdtPidBarrel" || name=="ScMdtPidForward") {
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmMdtPid(par));
    aDet->setName(name);
    aDet->setStorePid();
    return aDet;
  }
  // general Options
  else
  if (name=="SimpleTracker") {
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmSimpleTracker(par));
    aDet->setName(name);
    return aDet;
  } else
  if (name=="SimpleVtx") {
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmSimpleVtx(par));
    aDet->setName(name);
    return aDet;
  } else
  if (name=="CmpDet") {
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmCmpDet(par));
    aDet->setName(name);
    return aDet;
  } else
  if (name=="CombiDet") {
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmCombiDet(par));
    aDet->setName(name);
    return aDet;
  } else {
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

