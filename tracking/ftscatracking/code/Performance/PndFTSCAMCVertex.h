//-*- Mode: C++ -*-
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef PNDFTSCAMCVERTEX_H
#define PNDFTSCAMCVERTEX_H

#include "PndFTSCADef.h"
#include "PndFTSCAMCVertex.h"

#include <iostream>
using std::ostream;
using std::istream;


/**
 * @class PndFTSCAMCVertex
 * store MC track information for Performance
 */
class PndFTSCAMCVertex
{
 public:
  PndFTSCAMCVertex();

  float Par( int i )  const { return fPar[i]; }

  float X()           const { return fPar[0]; }
  float Y()           const { return fPar[1]; }
  float Z()           const { return fPar[2]; }

  void SetPar( int i, float v )   { fPar[i] = v; }
  
  void SetX( float v )            { fPar[0] = v; }
  void SetY( float v )            { fPar[1] = v; }
  void SetZ( float v )            { fPar[2] = v; }

  friend ostream& operator<<(ostream& out, const PndFTSCAMCVertex &a);
  friend istream& operator>>(istream& in, PndFTSCAMCVertex &a);

 protected:

  float fPar[3];         //* x,y,z
};

#endif
