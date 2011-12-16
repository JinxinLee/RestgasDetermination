//
//  PndPidSelector.cxx
//  PANDAROOT
//
//  Created by Ralf Kliemt on 12/16/11.
//

#include <math.h>
#include "TDatabasePDG.h"

#include "PndPidSelector.h"
#include "RhoBase/TRho.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/VAbsMicroCandidate.h"


ClassImp(PndPidSelector)

TBuffer &operator>>(TBuffer &buf,PndPidSelector  *&obj)
{
  obj = (PndPidSelector *) buf.ReadObject(PndPidSelector::Class());
  return buf;
}

PndPidSelector::PndPidSelector(const char *name, const char *type) : 
VAbsPidSelector(name,type) 
{}

Bool_t PndPidSelector::Accept(TCandidate& b) 
{ 
  if (&b == 0) return kFALSE;
  
  SetTypeAndMass(b);
  
  double Le  = b.GetPidInfo(0);
  double Lmu = b.GetPidInfo(1);
  double Lpi = b.GetPidInfo(2);
  double Lk  = b.GetPidInfo(3);
  double Lp  = b.GetPidInfo(4);
  
  if (fCriterion == loose) {
    if (Lp<0.2) return kFALSE;
  }
  else if (fCriterion == veryLoose || fCriterion == variable || fCriterion == all  ) {
    return kTRUE;
  }
  else if (fCriterion == best) {
    if (Lp<Le || Lp<Lmu || Lp<Lpi || Lp<Lk) return kFALSE;
  }
  else if (fCriterion == tight) {
    if (Lp<0.5) return kFALSE; 
  } 
  else {
    if (Lp<0.9) return kFALSE;
  }
  
  return kTRUE;
}

Bool_t PndPidSelector::Accept(VAbsMicroCandidate& b) 
{ 
  return kFALSE;
}


