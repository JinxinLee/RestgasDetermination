  //-*- Mode: C++ -*-
  // *****************************************************************************
  //                                                                             *
  // @Autors: I.Kulakov; M.Zyzak; I.Kisel                                        *
  // @e-mail: I.Kulakov@gsi.de; M.Zyzak@gsi.de; I.Kisel@compeng.uni-frankfurt.de *
  //                                                                             *
  // *****************************************************************************

#ifndef PNDCAFUNCTIONALITY_H
#define PNDCAFUNCTIONALITY_H

#include "PndCAVector.h"
#include "PndCATrackParamVector.h"
#include "PndCAParam.h"
#include "PndCAMath.h"
#include "PndCAStation.h"

class PndCAHits;
#include <vector>
using std::vector;

// Functions and parameters of CA algorithm to be called from any place
namespace PndCAFunctionality
{
  float_m FitIteration(const PndCAParam& caParam, const PndCAHits& hits, PndCATrackParamVector& param, const vector<PndCATESV>& iHits, const PndCATarget& target, bool dir, bool useParam, const float_m& active = float_m(true) );

  float_m Fit(const PndCAParam& caParam, const PndCAHits& hits, PndCATrackParamVector& param, const vector<PndCATESV>& iHits, const PndCATarget& target,  bool dir, const float_m& active = float_m(true) );
  
}

#endif
