// ******************************************************
// DecayTreeFitter Package
// We thank the original author Wouter Hulsbergen 
// (BaBar, LHCb) for providing the sources. 
// http://arxiv.org/abs/physics/0503191v1 (2005)
// Adaptation & Development for PANDA: Ralf Kliemt (2015)
// ******************************************************
//
//  SortTool.h
//  TreeFitter
//
//  Created by Ralf Kliemt on 26/01/15.
//  Copyright (c) 2015 Ralf Kliemt. All rights reserved.
//

#ifndef TREEFITTER_SORTTOOL_H
#define TREEFITTER_SORTTOOL_H 1

#include "ParticleBase.h"
#include "RecoTrack.h"

namespace DecayTreeFitter
{
  inline bool sortByType(const ParticleBase* lhs, const ParticleBase* rhs)
  {
    int lhstype = lhs->type() ;
    int rhstype = rhs->type() ;
    bool rc = false ;
    if( lhstype == rhstype  && lhstype == ParticleBase::kRecoTrack )
      rc =  lhs->particle()->Pt() > rhs->particle()->Pt() ;
    else if( lhs->particle()->NDaughters()>0 && rhs->particle()->NDaughters()>0 )
      rc = lhs->nFinalChargedCandidates() > rhs->nFinalChargedCandidates() ;
    else
      rc = lhstype < rhstype ;
    return rc ;
  }
  inline bool compTrkTransverseMomentum(const RecoTrack* lhs, const RecoTrack* rhs)
  {
    return lhs->particle()->Pt() > rhs->particle()->Pt() ;
  }
} // 
#endif
