/*  Copyright © 2008, Christian Hoeppner & Sebastian Neubert
    All rights reserved.

    This file is part of Genfit.

    Genfit is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Genfit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser Public License for more details.

    You should have received a copy of the GNU Lesser Public License
    along with Genfit.  If not, see <http://www.gnu.org/licenses/>. */

// ----------------------------------------------------------
// Please see WirepointHitPolicy.h  before using this class.
// ----------------------------------------------------------

#include "WirepointHitPolicy.h"

#include "assert.h"
#include <cmath>

#include "TMath.h"
#include "TVector3.h"

#include "AbsRecoHit.h"

TMatrixT<double> 
WirepointHitPolicy::hitCoord(AbsRecoHit* hit,const DetPlane& plane)
{
  TMatrixT<double> returnMat(2,1);

  checkPlane(hit, plane);

  // raw x1, y1, z1, x2, y2, z2, rdrift, zreco
  TMatrixT<double> rC = hit->getRawHitCoord();

  returnMat[0][0] = rC[6][0];
  returnMat[1][0] = rC[7][0];
  return returnMat;
}

TMatrixT<double> 
WirepointHitPolicy::hitCov(AbsRecoHit* hit,const DetPlane& plane)
{
  checkPlane(hit, plane);

  TMatrixT<double> returnCov(2,2);
  TMatrixT<double> rawCov = hit->getRawHitCov();

  returnCov[0][0] = rawCov[6][6];
  returnCov[1][0] = rawCov[7][6];
  returnCov[0][1] = rawCov[6][7];
  returnCov[1][1] = rawCov[7][7];

  return  returnCov;
}



void WirepointHitPolicy::checkPlane(AbsRecoHit* hit,const DetPlane& plane)
{
  // raw x1, y1, z1, x2, y2, z2, rdrift, zreco
  TMatrixT<double> rC = hit->getRawHitCoord();

  assert(rC.GetNrows()==8);
  
  TVector3 wire1(rC[0][0], rC[1][0], rC[2][0]);
  TVector3 wire2(rC[3][0], rC[4][0], rC[5][0]);
  TVector3 wiredirection = wire1 - wire2;
  
  TVector3 vaxis = plane.getV();
  wiredirection.SetMag(1.);
  vaxis.SetMag(1.);

  if(fabs(TMath::Abs(wiredirection.Dot(vaxis)) - 1) > 1e-3)
    {
     
      std::cout << "WirepointHitPolicy: plane not valid!!" << std::endl;
    }
}
