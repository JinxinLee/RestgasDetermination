#include "PndFsmRandom.h"
#include "TRandom3.h"

TRandom3* PndFsmRandom::Instance() {
  if (!frand)
    frand=new TRandom3();
  return frand;
}

TRandom3* PndFsmRandom::frand=0;
