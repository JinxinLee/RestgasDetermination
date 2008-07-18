//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TPointErr		    						//
//                                                                      //
// Point w/ error class							//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <float.h>
#include "RhoMath/TPointErr.h"

ClassImp(TPointErr)

TBuffer &operator>>(TBuffer &buf, TPointErr *&obj)
{
   obj = (TPointErr *) buf.ReadObject(TPointErr::Class());
   return buf;
}

TPointErr operator + (const TPointErr& v, const TVectorErr& w){
    TPointErr pe(TVector3(v.X()+w.X(),v.Y()+w.Y(),v.Z()+w.Z()),
	(v.CovMatrix()+w.CovMatrix()));
    return pe;
}

TPointErr operator + (const TVectorErr& w, const TPointErr& v){
    TPointErr pe(TVector3(v.X()+w.X(),v.Y()+w.Y(),v.Z()+w.Z()), 
	(v.CovMatrix()+w.CovMatrix()));
    return pe;
}

TPointErr operator - (const TPointErr& v, const TVectorErr& w){
    TPointErr pe(TVector3(v.X()-w.X(),v.Y()-w.Y(),v.Z()-w.Z()), 
	(v.CovMatrix()+w.CovMatrix()));
    return pe;
}

TVectorErr operator - (const TPointErr& v, const TPointErr& w){
    TVectorErr ve(TVector3(v.X()-w.X(),v.Y()-w.Y(),v.Z()-w.Z()),
	(v.CovMatrix()+w.CovMatrix()));
    return ve;
}


