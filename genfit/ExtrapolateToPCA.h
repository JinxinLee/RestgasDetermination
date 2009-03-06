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

//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Function to extrapolate two tracks to common vertex
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
// Original Authors: Christian Hoeppner & Sebastian Neubert (TUM)
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef EXTRAPOLATETOPCA_HH
#define EXTRAPOLATETOPCA_HH

class Track;

void ExtrapolateToPCA(Track* trk1, Track* trk2);  


#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
