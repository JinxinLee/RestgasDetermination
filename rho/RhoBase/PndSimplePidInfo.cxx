//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndSimplePidInfo								//
//                                                                      //
// Particle identification info class					//
// Abstract definition of the PidInfo database				//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 1999					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoBase/PndSimplePidInfo.h"

ClassImp(PndSimplePidInfo)

TBuffer &operator>>(TBuffer &buf, PndSimplePidInfo *&obj)
{
   obj = (PndSimplePidInfo *) buf.ReadObject(PndSimplePidInfo::Class());
   return buf;
}

using namespace std;

PndSimplePidInfo::PndSimplePidInfo()
{
  int i=0;
  for (i=0;i<5;i++){
    fSignificance[i]=0;
    fLikelihood[i]=0;
  }
}

void PndSimplePidInfo::PrintOn( std::ostream& o )
{
  int i=0;
  
  for (i=0;i<5;i++){
    o<<"(Hypo="<<i<<"; LH="<<fLikelihood[i]<<")  ";
  } 
  o<<std::endl;
}


void PndSimplePidInfo::SetLikelihood(Int_t hypo, Float_t llh)
{
  if (hypo>=0 && hypo<5) fLikelihood[hypo]=llh;
}
