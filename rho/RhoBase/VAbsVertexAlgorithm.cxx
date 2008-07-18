//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsVertexAlgorithm							//
//                                                                      //
// Abstract interface of the vertexing algorithms			//
//                                                                      //
// Author: Marcel Kunze, RUB, Mar. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TLorentzVector.h"
#include "RhoBase/TFitParams.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/VAbsVertexAlgorithm.h"

ClassImp(VAbsVertexAlgorithm)

#include <iostream>
using namespace std;

VAbsVertexAlgorithm::VAbsVertexAlgorithm()
: fMode(Standard), fFlyResonances(kFALSE)
{
}

VAbsVertexAlgorithm::~VAbsVertexAlgorithm()
{
}

Double_t 
VAbsVertexAlgorithm::Chi2Contribution(const TCandidate& bc) const{
    return -1;
}

TLorentzVector   
VAbsVertexAlgorithm::P4Daughters(const TCandidate& bc)
{
    return bc.GetFitParams().P4();
}


TMatrixD&
VAbsVertexAlgorithm::P4CovDaughters(const TCandidate& bc)
{
    return bc.GetFitParams().P4Cov();
}

TVector3     
VAbsVertexAlgorithm::PosDaughters(const TCandidate& bc)
{
   return bc.GetFitParams().Pos();
}


TMatrixD&
VAbsVertexAlgorithm::PosCovDaughters(const TCandidate& bc)
{
    return bc.GetFitParams().PosCov();
}


TMatrixD&
VAbsVertexAlgorithm::PosP4CovDaughters(const TCandidate& bc)
{
    return bc.GetFitParams().XPCov();
}


TMatrixD&
VAbsVertexAlgorithm::Cov7Daughters(const TCandidate& bc)
{
    return bc.GetFitParams().Cov7();
}
