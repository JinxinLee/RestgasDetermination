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
#include "TString.h"


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
#include "PndFsmIdealPid.h"

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
  TString tname(name);

  if (tname.BeginsWith("EmcBarrel")){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmEmcBarrel(par));
    aDet->setName(name);
    return aDet;
  } else
  if (tname=="EmcFwCap"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmEmcFwCap(par));
    aDet->setName(name);
    return aDet;
  } else
  if (tname=="EmcBwCap"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmEmcBwCap(par));
    aDet->setName(name);
    return aDet;
  } else
  if (tname=="EmcFS"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmEmcFS(par));
    aDet->setName(name);
    return aDet;
  } else
  if (tname=="Stt"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmStt(par));
    aDet->setName(name);
    return aDet;
  } else
  if (tname=="MdcFS"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmMdcFS(par));
    aDet->setName(name);
    return aDet;
  } else
  if (tname=="MdcTS"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmMdcTS(par));
    aDet->setName(name);
    return aDet;
  } else
  if (tname.BeginsWith("EffTracker")){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmEffTracker(par));
    aDet->setName(name);
    return aDet;
  } else
  if (tname=="DrcBarrel"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmDrcBarrel(par));
    aDet->setName(name);
    aDet->setStorePid();
    return aDet;
  } else
  if (tname=="DrcDisc"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmDrcDisc(par));
    aDet->setName(name);
    aDet->setStorePid();
    return aDet;
  } else
  if (tname=="Mvd"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmMvd(par));
    aDet->setName(name);
    aDet->setStorePid();
    return aDet;
  } else
  if (tname=="Mvd2"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmMvd2(par));
    aDet->setName(name);
    aDet->setStorePid();
    return aDet;
  } else
  if (tname=="Tof"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmTof(par));
    aDet->setName(name);
    aDet->setStorePid();
    return aDet;
  } else
  if (tname=="Rich"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmRich(par));
    aDet->setName(name);
    aDet->setStorePid();
    return aDet;
  } else
  // --- scrutiny process options
  if (tname.BeginsWith("ScSttAlone") || tname.BeginsWith("ScSttMvd") || tname.BeginsWith("ScSttMvdGem") ||
      tname.BeginsWith("ScSttGem")   || tname.BeginsWith("ScMvdGem") || tname.BeginsWith("ScMvdGemFts")    ||
      tname.BeginsWith("ScMvdFts")   || tname.BeginsWith("ScGemFts") ||  tname=="ScFts" ) {
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmSimpleTracker(par));
    aDet->setName(name);
    return aDet;
  } else
  if (tname=="MvdPid") {
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmMvdPid(par) );
    aDet->setStorePid();
    return aDet;
  } else
  if (tname.BeginsWith("ScEmcPidBarrel") || tname=="ScEmcPidFwCap" || tname=="ScEmcPidBwCap" || tname=="ScEmcPidFS"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmEmcPid(par) );
	aDet->setName(name);
    aDet->setStorePid();
	return aDet;
  } else
  if (tname=="SttPid"){
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmSttPid(par) );
    aDet->setStorePid();
    return aDet;
  }
  if (tname=="ScVtxMvd" || tname=="ScVtxNoMvd") {
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmSimpleVtx(par));
    aDet->setName(name);
    return aDet;
  } else
  if (tname=="ScMdtPidBarrel" || tname=="ScMdtPidForward") {
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmMdtPid(par));
    aDet->setName(name);
    aDet->setStorePid();
    return aDet;
  }
  // general Options
  else
  if (tname.BeginsWith("SimpleTracker")) {
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmSimpleTracker(par));
    aDet->setName(name);
    return aDet;
  } else
  if (tname.BeginsWith("SimpleVtx")) {
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmSimpleVtx(par));
    aDet->setName(name);
    return aDet;
  } else
  if (tname=="IdealPid") {
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmIdealPid(par));
    aDet->setName(name);
    aDet->setStorePid();
    return aDet;
  }
  if (tname=="CmpDet") {
    PndFsmAbsDet* aDet = (PndFsmAbsDet*)(new PndFsmCmpDet(par));
    aDet->setName(name);
    return aDet;
  } else
  if (tname=="CombiDet") {
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

