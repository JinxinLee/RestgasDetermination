//--------------------------------------------------------------------------
// File and Version Information:
//
// $Id: Tokenize.cc,v 1.1.1.1 2005/03/29 16:49:16 steinke Exp $
//
// Description:
//      The class is a replacement for Rogue Wave's RWCTokenizer.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//       A. De Silva
//
// Copyright Information:
//
// Bertram Kopf (RUB) migrated to PandaRoot
//------------------------------------------------------------------------

#include "Tokenize.h"

#include <string>

panda::PndString::Tokenize::Tokenize(const std::string& str) 
  : _nextPos(0)
  , _strPtr(new std::string(str))
{
}

panda::PndString::Tokenize::~Tokenize() 
{
  delete _strPtr;
}

std::string 
panda::PndString::Tokenize::getStr(const std::string& token) {
  int wordBeg, wordEnd;
  std::string result("\0");
  wordBeg = _strPtr->find_first_not_of(token, _nextPos);
  if (wordBeg != std::string::npos) {
    wordEnd = _strPtr->find_first_of(token, wordBeg);
    wordEnd = (wordEnd != std::string::npos) ? wordEnd : _strPtr->size();
    result.assign(*_strPtr, wordBeg, wordEnd-wordBeg);
    _nextPos = wordEnd;
  }

  return result;
}

  


