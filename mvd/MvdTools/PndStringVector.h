// -----------------------------------------------------------------
//
//          TPndStringVector
//
//          Version 1.0
//            by
//          Tobias Stockmanns
//  Seperates an input string into substrings and stores them in a
//  string vector. The seperation criteria is a string of characters.
//  If no delimiters are given the default delimiter " " is used.
//
//  Example:
//    std::vector<std::string> fOutput
//    TPndStringVector fInput("Column:Row Type: Test ", ": ");
//    fOutput = fInput.GetStringVector();
//
//    fOutput[0] = "Column"
//    fOutput[1] = "Row"
//    fOutput[2] = "Type"
//    fOutput[3] = "Test"
//
#ifndef PNDSTRINGVECTOR_H
#define PNDSTRINGVECTOR_H

#include <string>
#include <vector>
#include "Rtypes.h"


class PndStringVector
{
  public :
       PndStringVector(){};
       ~PndStringVector(){};
       PndStringVector(std::string AInput, std::string ADelimiter = " ");
       void SetInput (std::string AInput)       {fInput = AInput;};
       void SetDelimiter (std::string ADelimiter) {fDelimiter = ADelimiter;};
       void ResetVector(){fStrings.clear();};
       std::vector<std::string> GetStringVector (void);
       void Print();

  private :
       std::string::size_type fStartPos;
       std::vector<std::string> fStrings;
       std::string fInput;
       std::string fDelimiter;
       std::string fOutput;
       std::string GetString (void);
};

#endif
