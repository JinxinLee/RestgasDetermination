//-------------------------------------------------------------------
// $Id: LinearInterpolPolicy.hh,v 1.3 2005/09/02 15:11:06 sneubert Exp $
//
// Description:
//      Policy-implementation.
//      Provides a method void Interpolate(...) to distribute primary clusters
//      in the TPC between two geant hits.      
//
//      Here a simple linear interpolation is implemented. 
//
//
// Author List:
//    Sebastian Neubert                     TUM      original author
//
//-------------------------------------------------------------------

#ifndef LINEARINTERPOLPOLICY_HH
#define LINEARINTERPOLOLICY_HH


#include "TClonesArray.h"

class PndTpcPoint;
class PndTpcPrimaryCluster;

struct LinearInterpolPolicy {
  void Interpolate(const PndTpcPoint* hit1, 
		   const PndTpcPoint* hit2, 
		   TClonesArray* CLusterList, 
		   unsigned int from, 
		   unsigned int n);
};

#endif

