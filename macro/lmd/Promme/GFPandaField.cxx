/* Copyright 2008-2010, Technische Universitaet Muenchen,
   Authors: Christian Hoeppner & Sebastian Neubert

   This file is part of GENFIT.

   GENFIT is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   GENFIT is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with GENFIT.  If not, see <http://www.gnu.org/licenses/>.
*/
#include"GFPandaField.h"
#include "FairRunAna.h"
#include "FairField.h"

TVector3 GFPandaField::get(const TVector3& pos) const{
  double x[3] = {pos.X(), pos.Y(), pos.Z()};
  double B[3] = {0,0,0};
  FairRunAna::Instance()->GetField()->Field(x,B);
  return TVector3(B[0], B[1], B[2]);
}
