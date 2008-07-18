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
//    std::vector<std::string> Output
//    TPndStringVector Input("Column:Row Type: Test ", ": ");
//    Output = Input.GetStringVector();
//
//    Output[0] = "Column"
//    Output[1] = "Row"
//    Output[2] = "Type"
//    Output[3] = "Test"
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
       void SetInput (std::string AInput)       {Input = AInput;};
       void SetDelimiter (std::string ADelimiter) {Delimiter = ADelimiter;};
       void ResetVector(){Strings.clear();};
       std::vector<std::string> GetStringVector (void);
       void Print();

  private :
       std::string::size_type StartPos;
       std::vector<std::string> Strings;
       std::string Input;
       std::string Delimiter;
       std::string Output;
       std::string GetString (void);
};

#endif
