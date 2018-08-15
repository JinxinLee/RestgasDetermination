// **************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: Sergey Gorbunov <sergey.gorbunov@kip.uni-heidelberg.de> *
//                  Ivan Kisel <kisel@kip.uni-heidelberg.de>                *
//                  for The ALICE HLT Project.                              *
//                                                                          *
// Developed by:   Igor Kulakov <I.Kulakov@gsi.de>                          *
//                 Maksym Zyzak <M.Zyzak@gsi.de>                            *
//                                                                          *
// Permission to use, copy, modify and distribute this software and its     *
// documentation strictly for non-commercial purposes is hereby granted     *
// without fee, provided that the above copyright notice appears in all     *
// copies and that both the copyright notice and this permission notice     *
// appear in the supporting documentation. The authors make no claims       *
// about the suitability of this software for any purpose. It is            *
// provided "as is" without express or implied warranty.                    *
//                                                                          *
//***************************************************************************

#include "PndFTSCAMCVertex.h"

PndFTSCAMCVertex::PndFTSCAMCVertex()
{
  for( int i = 0; i < 3; i++) fPar[i] = 0;
}


ostream& operator<<(ostream& out, const PndFTSCAMCVertex &a)
{
  for (int i = 0; i < 3; i++) out << a.fPar[i] << std::endl;
  return out;
}


istream& operator>>(istream& in, PndFTSCAMCVertex &a)
{
  for (int i = 0; i < 3; i++) in >> a.fPar[i];
  return in;
}

