//--------------------------------------------------------------------------
// File and Version Information:
//
// $Id: String.hh,v 1.1.1.1 2005/03/29 16:49:16 steinke Exp $
//
// Description:
//      The functions, within this namespace, are needed by BaBar to replace
//      those lost in the rw to STL string migration.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//       A. De Silva
//       A. Ryd
//
// Copyright Information:
//
//------------------------------------------------------------------------

#ifndef PANDA_STRING_HH
#define PANDA_STRING_HH

#include <algorithm>
#include <string>
#include <cctype>

namespace panda {
  namespace String {
    
    // transformations from lower toupper case or vice-versa
    
    // needed because for_each is a non-modifying sequence algorithm
    inline void lowerCh(char& c) { c = tolower(c); }
    inline void upperCh(char& c) { c = toupper(c); }
           
    inline void transformToLower(std::string& str) {
      std::for_each(str.begin(), str.end(), lowerCh);
    }  
    inline void transformToUpper(std::string& str) {
      std::for_each(str.begin(), str.end(), upperCh);
    }

    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);
    

    // case-insensitive comparisons
    //
    //    It returns an integer less  than, equal  to,  or  greater  than
    //    zero if s is found, respectively, to be less than, to match, or
    //    be greater than  s2.
    //
    int compare_nocase(const std::string& s, const std::string& s2);

    int compare_nocase(const std::string& s, const char* s2);


    // case-insensitive search
    int find_nocase(const std::string& s, const std::string& s2);

    inline int find_nocase(const std::string& s, const char* s2) {
      return find_nocase(s,std::string(s2));
    }


    // Provides a hash function for strings
    unsigned int rwHash(const std::string& str);
    
  }
}

#endif


