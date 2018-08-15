#ifndef PNDRICHRESOLUTION_H
#define PNDRICHRESOLUTION_H

#include "PndPidCandidate.h"
#include "TVector3.h"
#include "PndRichCalDb.h"

class  PndRichResolution //: public FairGeoSet
{

 private:

    PndRichCalDb* caldb;

 protected:
  public:
    PndRichResolution();
    ~PndRichResolution();

    Double_t Sigma(dbpoint pnt);
    Double_t Shift(dbpoint pnt);
    Double_t Efficiency(dbpoint pnt);
    
};

#endif
