/*
 * PndMdtDigiRingSorter.cxx
 */

#include "PndMdtDigiRingSorter.h"
#include "FairTimeStamp.h"

ClassImp(PndMdtDigiRingSorter);


PndMdtDigiRingSorter::~PndMdtDigiRingSorter() {
  // TODO Auto-generated destructor stub
}

FairTimeStamp* PndMdtDigiRingSorter::CreateElement(FairTimeStamp* data) {
//preservation, or invoke copy construction of data, after converting data type 
  return (FairTimeStamp*) data->Clone();
}
