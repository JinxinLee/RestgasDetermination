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

// Description:
//      Abstract interface to a BField
//      User has to inherit his adaptor from this class


#ifndef ABSBFIELDIFC_H
#define ABSBFIELDIFC_H

#include "TVectorT.h"

class AbsBFieldIfc {
public:

  // Constructors/Destructors ---------
  AbsBFieldIfc(){;}
  virtual ~AbsBFieldIfc(){;}

  // Accessors -----------------------
  virtual TVectorT<double> get(const TVectorT<double>& pos) const =0;
 
};

#endif
