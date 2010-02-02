#include "PndStringVector.h"
#include <iostream>
#include <sstream>


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
  TestFirst();
  TestLast();
  while (fStartPos != std::string::npos){
    value = GetString();
    if (value.length() > 0)
      fStrings.push_back(value);
  }
  return fStrings;
}

std::string PndStringVector::Replace(std::string from, std::string to)
{
	std::string olddel = fDelimiter;
	SetDelimiter(from);
	std::stringstream result;
	std::vector<std::string> strVector = GetStringVector();
	if (GetIfFirst()) result << to;
	result << strVector[0];
	for (int i = 1; i < strVector.size(); i++){
		result << to << strVector[i];
	}
	if(GetIfLast()) result << to;
	fDelimiter = olddel;
	//std::cout << "New String: " << result.str() << std::endl;
	return result.str();
}

void PndStringVector::Print()
{
  std::cout << "PndStringVector for: " << fInput << std::endl;
  for (Int_t i = 0; i < fStrings.size(); i++)
    std::cout << i << ": " << fStrings[i] << std::endl;
}
