//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TDoubleErr		    						//
//                                                                      //
// Double w/ error							//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoMath/TDoubleErr.h"

#include <math.h>

ClassImp(TDoubleErr)

TBuffer &operator>>(TBuffer &buf, TDoubleErr *&obj)
{
   obj = (TDoubleErr *) buf.ReadObject(TDoubleErr::Class());
   return buf;
}

#include <iostream>
using namespace std;

static const char rscid[] = "$Id: TDoubleErr.cxx,v 1.3 2002-02-01 22:34:57 marcel Exp $";

TDoubleErr::TDoubleErr(const TDoubleErr & original) {
    *this = original;
}

TDoubleErr & TDoubleErr::operator = (const TDoubleErr & original) {
    fValue = original.fValue;
    fCovariance = original.fCovariance;
    return *this;
}

Double_t 
TDoubleErr::DetermineChisq(Double_t ref) const {
    if (0 >= fCovariance){
	return -1.;
    }
    else{
	Double_t diff = (ref - fValue);
	return (diff * diff) / fCovariance;
    }
}

TDoubleErr TDoubleErr::operator-() {
    return TDoubleErr(-fValue, fCovariance);
}

TDoubleErr TDoubleErr::operator - (const TDoubleErr & bde) {
    return TDoubleErr(fValue - bde.fValue, fCovariance - bde.fCovariance);
}

TDoubleErr TDoubleErr::operator + (const TDoubleErr & bde) {
    return TDoubleErr(fValue + bde.fValue, fCovariance + bde.fCovariance);
}

TDoubleErr TDoubleErr::operator * (const TDoubleErr & bde) {
    return TDoubleErr(fValue * bde.fValue, 
		      fCovariance * bde.fValue * bde.fValue +
		      bde.fCovariance * fValue * fValue);
}

TDoubleErr TDoubleErr::operator / (const TDoubleErr & bde) {
    register Double_t bde2 = bde.fValue * bde.fValue;
    
    return TDoubleErr(fValue / bde.fValue,
		      fCovariance / bde2 +
		      bde.fCovariance * fValue * fValue / (bde2 * bde2));
}		      

TDoubleErr & TDoubleErr::operator += (const TDoubleErr & bde) {
    *this = *this + bde;
    return *this;
}

TDoubleErr & TDoubleErr::operator -= (const TDoubleErr & bde) {
    *this = *this - bde;
    return *this;
}

TDoubleErr & TDoubleErr::operator *= (const TDoubleErr & bde) {
    *this = *this * bde;
    return *this;
}

TDoubleErr & TDoubleErr::operator /= (const TDoubleErr & bde) {
    *this = *this / bde;
    return *this;
}

std::ostream & operator<<(std::ostream & stream, const TDoubleErr & bde) {
    stream << "value: " << bde.Value() 
	<< " covariance: " << bde.Covariance();
    
    return stream;
}

