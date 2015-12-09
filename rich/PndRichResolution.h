#ifndef PNDRICHRESOLUTION_H
#define PNDRICHRESOLUTION_H

#include "PndPidCandidate.h"

class  PndRichResolution //: public FairGeoSet
{

 private:

 protected:
  public:
    PndRichResolution();
    ~PndRichResolution();

    Double_t Sigma(PndPidCandidate* pidcand);
    Double_t Shift(PndPidCandidate* pidcand);
    
};

#endif
