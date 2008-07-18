//////////////////////////////////////////////////////////////////////////
//                                                                      //
// VAbsPidInfo								//
//                                                                      //
// Particle identification info class					//
// Abstract definition of the PidInfo database				//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 1999					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/VAbsPidInfo.h"

ClassImp(VAbsPidInfo)

TBuffer &operator>>(TBuffer &buf, VAbsPidInfo *&obj)
{
   obj = (VAbsPidInfo *) buf.ReadObject(VAbsPidInfo::Class());
   return buf;
}

using namespace std;

VAbsPidInfo::VAbsPidInfo()
{}
