#ifndef CUTS_C
#define CUTS_C

#include "../src/TCcluster.h"
#include "../src/TCtrack.h"
#include <algorithm>
#include <vector>
#include <set>


bool IEEE(TCtrack t){

  if (t.nClFit()<5){
    return false;
  }
  if (fabs(t.getThX())<1){
    return false;
  }
  if (fabs(t.getThY())<1){
    return false;
  }
  if (fabs(t.getTh())<1){
    return false;
  }
  if (fabs(t.getTh())>30){
    return false;
  }

  return true;
}


#endif
