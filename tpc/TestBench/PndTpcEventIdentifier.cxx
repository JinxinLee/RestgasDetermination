
#include "PndTpcEventIdentifier.h"

ClassImp(PndTpcEventIdentifier)


PndTpcEventIdentifier::PndTpcEventIdentifier()
: fSpill(0),
  fEventInSpill(0)
{;}


PndTpcEventIdentifier::PndTpcEventIdentifier(unsigned int ev,
					     unsigned int sp) 
: fSpill(sp),
  fEventInSpill(ev)
{;}


//meh.
  
