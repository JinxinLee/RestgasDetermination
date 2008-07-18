//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TDummyFitter			  					//
//                                                                      //
// Author: Marcel Kunze, RUB, Feb. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoTools/TDummyFitter.h"

ClassImp(TDummyFitter)

TBuffer &operator>>(TBuffer &buf, TDummyFitter *&obj)
{
   obj = (TDummyFitter *) buf.ReadObject(TDummyFitter::Class());
   return buf;
}

