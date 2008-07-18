#include "PndTpcDigiAmplitude.h"
#include "PndTpcDigi.h"

bool 
PndTpcDigiAmplitude::operator()(PndTpcDigi* s1, PndTpcDigi* s2)
{
  return s1->amp()<s2->amp();
}

