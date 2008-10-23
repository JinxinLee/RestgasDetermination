#include "PndStringVector.h"
#include <iostream>


PndStringVector::PndStringVector (std::string AInput, std::string ADelimiter)
{
  SetInput (AInput);
  SetDelimiter (ADelimiter);
}


std::string PndStringVector::GetString (void)
{
  std::string::size_type pos;
  std::string aString;
  pos = fInput.find_first_of(fDelimiter.c_str(), fStartPos);
  if (pos-fStartPos == 0) {                //first value at fStartPos is a fDelimiter
    fStartPos += 1;
    return "";
  }

  else if (pos != std::string::npos){           // a delimiter was found after fStartPos
    aString = fInput.substr(fStartPos, pos-fStartPos);
    fStartPos = pos + 1;
    return aString;
  }

  else {                          //no delimiter was found after fStartPos
    aString = fInput.substr(fStartPos, pos-fStartPos);
    fStartPos = pos;
    return aString;
  }
}


std::vector<std::string> PndStringVector::GetStringVector(void)
{
  fStartPos = 0;
  std::string value;

  ResetVector();
  while (fStartPos != std::string::npos){
    value = GetString();
    if (value.length() > 0)
      fStrings.push_back(value);
  }
  return fStrings;
}

void PndStringVector::Print()
{
  std::cout << "PndStringVector for: " << fInput << std::endl;
  for (Int_t i = 0; i < fStrings.size(); i++)
    std::cout << i << ": " << fStrings[i] << std::endl;
}
