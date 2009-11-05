//--------------------------------------------------------------------------
// File and Version Information:
//
// $Id: Tokenize.hh,v 1.1.1.1 2005/03/29 16:49:16 steinke Exp $
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

#ifndef PANDA_TOKENIZE_HH
#define PANDA_TOKENIZE_HH

#include <string>

namespace panda {
  namespace PndString {

    class Tokenize {
      
    public:
      
      Tokenize(const std::string& str);
      ~Tokenize();
      
      std::string operator() () {
	return getStr(std::string(" \t\n\0"));
      }
      
      std::string operator() (const char* s) {
	std::string token(s);  
	token += "\0";
	return getStr(token);
      }
      
      std::string operator() (const char* s, size_t num) {
	return getStr(std::string(s, num));
      }
      
    private:
      
      std::string getStr(const std::string& token);
      
      size_t             _nextPos;    
      const std::string* _strPtr;
      
    };

  }
}

#endif


