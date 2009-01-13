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
  pos = Input.find_first_of(Delimiter.c_str(), StartPos);
  if (pos-StartPos == 0) {                //first value at StartPos is a Delimiter
    StartPos += 1;
    return "";
  }

  else if (pos != std::string::npos){           // a delimiter was found after StartPos
    aString = Input.substr(StartPos, pos-StartPos);
    StartPos = pos + 1;
    return aString;
  }

  else {                          //no delimiter was found after StartPos
    aString = Input.substr(StartPos, pos-StartPos);
    StartPos = pos;
    return aString;
  }
}


std::vector<std::string> PndStringVector::GetStringVector(void)
{
  StartPos = 0;
  std::string value;

  ResetVector();
  while (StartPos != std::string::npos){
    value = GetString();
    if (value.length() > 0)
      Strings.push_back(value);
  }
  return Strings;
}

void PndStringVector::Print()
{
  std::cout << "PndStringVector for: " << Input << std::endl;
  for (Int_t i = 0; i < Strings.size(); i++)
    std::cout << i << ": " << Strings[i] << std::endl;
}
