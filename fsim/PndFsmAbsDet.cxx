//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmAbsDet.cc,v 1.2 2006/07/13 10:43:45 klausg Exp $
//
// Description:
//      Class FsmAbsDet
//
//  Response of FsmDet for a FsmTrack
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
#include "PndFsmRandom.h"
#include "PndFsmAbsDet.h"
#include "StrTok.h"

//-------------
// C Headers --
//-------------

//---------------
// C++ Headers --
//---------------
#include <iostream>
#include <algorithm>
using std::cout;
using std::endl;
using std::ostream;
using std::string;

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
//#include "ErrLogger/ErrLog.hh"
//#include "boost/boost/tokenizer.hpp"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

//----------------
// Constructors --
//----------------

PndFsmAbsDet::PndFsmAbsDet()
{
  initParameters();

  ArgList par;
  //set default parameter values and parses a parameter list
  parseParameterList(par);
  _rand=PndFsmRandom::Instance();
  _fdbPDG = TDatabasePDG::Instance();
  _doesPid=kFALSE;
}

PndFsmAbsDet::PndFsmAbsDet(ArgList &par)
{
  initParameters();
  //set default parameter values and parses a parameter list
  //i.e. std::list<std::string> of the form
  //"a=1" "b=2" "c=3"
  parseParameterList(par);
  _rand=new TRandom3();
  _fdbPDG = TDatabasePDG::Instance();
  _doesPid=kFALSE;
}

//--------------
// Destructor --
//--------------

PndFsmAbsDet::~PndFsmAbsDet()
{
}

//--------------
// Operations --
//--------------

bool
PndFsmAbsDet::setParameter(std::string &name, double value)
{
  return false;
}

bool
PndFsmAbsDet::setParameter(std::string &name, std::string &value)
{
  return false;
}

void
PndFsmAbsDet::initParameters()
{
  _detName = "default";
}


void
PndFsmAbsDet::print(std::ostream &o)
{
  o<<"Detector:"<<detName()<<endl;
}


void
PndFsmAbsDet::parseParameterList(ArgList &par)
{
  // ArgList is a std::list<std::string> of the form ("a=1","b=2","c=3","d=4")

  // Default values for the parameters

  CStrTok tokenizer;
  char csrc[200];

  if (par.size() != 0) {

    //cout <<"Parameters for detector <"<<_detName<<">"<<endl;
    for(ArgList::const_iterator argIt=par.begin(); argIt!=par.end();argIt++) {

      const char *src=argIt->data();
      strcpy(csrc,src);

      char* token = tokenizer.GetFirst(csrc,"=");
      string name(token);
      //cout <<"variable: -" << name << "-   ";
      token = tokenizer.GetNext("=");

      double value = atof(token);
      string strvalue(token);

      //cout <<"value: -"<<value<<"-   "<<endl;

      if (value!=0 || strvalue=="0" || strvalue=="0.0")
      {
        if (!setParameter(name,value))
	  cout  <<" -W- (PndFsmAbsDet::parseParameterList) Unknown Parameter: <"<<name<< endl;
      }
      else
      {
        if (!setParameter(name,strvalue))
	  cout  <<" -W- (PndFsmAbsDet::parseParameterList) Unknown Parameter: <"<<name<< endl;
      }

    }
  }

  /*
  if (par.size() != 0) {

    //cout <<"Parameters for detector <"<<_detName<<">"<<endl;
    for(ArgList::const_iterator argIt=par.begin(); argIt!=par.end();argIt++) {
      typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
      boost::char_separator<char> sep("=");
      tokenizer tokens(*argIt, sep);
      tokenizer::iterator tok_iter = tokens.begin();
      string name(*tok_iter);
      ++tok_iter;
      double value=atof((*tok_iter).c_str());

      //cout  <<"<"<<name<<"> = "<<value<<endl;

      string name("efficiency");
      double value=0.99;
      if (!setParameter(name,value))
	  cout  <<" -W- Unknown Parameter: <"<<name<<"> in PndFsmAbsDet::parseParameterList"  << endl;
    }
  }
  */
  //print(std::cout);

}


