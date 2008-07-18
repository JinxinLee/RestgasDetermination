//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TLorentzVectorErr		    					//
//                                                                      //
// LorentzVector w/ error						//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoMath/TLorentzVectorErr.h"
#include "RhoMath/TVectorErr.h"

ClassImp(TLorentzVectorErr)

TBuffer &operator>>(TBuffer &buf, TLorentzVectorErr *&obj)
{
   obj = (TLorentzVectorErr *) buf.ReadObject(TLorentzVectorErr::Class());
   return buf;
}

#include <iostream>
using namespace std;

TLorentzVectorErr::TLorentzVectorErr(const TVectorErr & p3, 
					 Double_t mass) :
TLorentzVector((const TVector3 &)p3, sqrt(p3.Mag2() + mass * mass))
{ 
    fCovMatrix = new TError(4);
    // The 3-vector part of the error, initialized as a 0-matrix, then copied 
    // from p3:
    static TMatrixD p4Err(4,4);
    int i, j;
    for (i = 0; i < 3; ++i){
	for (j = i; j < 3; ++j){  // start from j=i, since (i,j) = (j,i)
	    p4Err(i,j) = p3.CovMatrix()(i,j);
	}
    }
    
    // The energy part of the error:
    const Double_t energy = E();
    
    if (energy != 0){
	p4Err(4,4) = 0.0;
	for (int i = 0; i < 3; ++i){
	    for (int j = 0; j < 3; ++j){
		p4Err(4,4) += p3[i] * p3[j] *
		    p3.CovMatrix()(i, j);
	    }
	}
	p4Err(4,4) /= (energy * energy);
    }
    
    // The E-p correlated part of the error is
    // <(E - <E>) (Pi - <Pi>)> = dE/dPj Vjk dPi/dPk
    // Since dPi/dPk = delta_ik, we get dE/dPj Vji.
    for (i = 0; i < 3; ++i){
	for (j = 0; j < 3; ++j){
	    p4Err(i,3) += p3(j) / energy * p3.CovMatrix()(j,i);	
	    // No need to switch indices for a TMatrixD, so no p4Err(4,i)
	}
    }
    
    SetCovMatrix(p4Err);
}

// argumentless constructor:
TLorentzVectorErr::TLorentzVectorErr() : TLorentzVector() 
{
    fCovMatrix = new TError(4);
}

// auto casting constructor
TLorentzVectorErr::TLorentzVectorErr(const TLorentzVector &p) : 
TLorentzVector(p)	
{
    fCovMatrix = new TError(4);
}

TLorentzVectorErr::TLorentzVectorErr(const TLorentzVector &p, const TError& covMat) : 
TLorentzVector(p)
{
    fCovMatrix = new TError(covMat);
}

// copy constructor
TLorentzVectorErr::TLorentzVectorErr(const TLorentzVectorErr& v) : TLorentzVector(v)	
{
    fCovMatrix = new TError(v.CovMatrix());
}

// assignment operator:
TLorentzVectorErr& TLorentzVectorErr::operator=(const TLorentzVectorErr& v)
{
    if (this != &v) {
	TLorentzVector::operator=(v);
	fCovMatrix = new TError(v.CovMatrix());
    }
    return *this;
}


// mathematical modifiers:
TLorentzVectorErr& TLorentzVectorErr::operator - () {
    ((TLorentzVector)*this) *= -1.;
    return *this;  // fCovMatrix unaltered
}

TLorentzVectorErr& TLorentzVectorErr::operator += (const TLorentzVectorErr& v){
    TLorentzVector::operator+=(v);
    *fCovMatrix += v.CovMatrix();
    return *this;
}

TLorentzVectorErr& TLorentzVectorErr::operator -= (const TLorentzVectorErr& v){
    TLorentzVector::operator-=(v);
    *fCovMatrix += v.CovMatrix();
    return *this;
}

//-------------------------------------------------------------
Double_t 
TLorentzVectorErr::DetermineChisq(const TLorentzVector& refVector)
{
    static TVectorD temp(4);
    temp(0) = refVector.X()-this->X();
    temp(1) = refVector.Y()-this->Y();
    temp(2) = refVector.Z()-this->Z();
    temp(3) = refVector.T()-this->T();
    
    return fCovMatrix->DetermineChisq(temp);
}


//-------------------------------------------------------------
TLorentzVectorErr& 
TLorentzVectorErr::Transform(const TLorentzRotation& rot) {
    TLorentzVector::Transform(rot);
    *fCovMatrix = fCovMatrix->Similarity(rot);
    return *this;
}

//-------------------------------------------------------------
TLorentzVectorErr& 
TLorentzVectorErr::Transform(const TRotation& rot){
    TLorentzVector::Transform(rot);
    static TMatrixD tempRot(4,4);
    
    // Fill a 4x4 matrix from the 3x3 TRotation. Note that they use different
    // indexing schemes (!?@#$&^*&#$@#):
    
    int row;
    int col;
    for (row = 0; row < 3; ++row){   // 3 is the size of TRotation
	for (col = 0; col < 3; ++col){ // (which provides no enum)
	    tempRot(row, col) = rot(row, col);
	}
    }
    
    // fill the 4th row:
    tempRot(3,3) = 1.0;
    for (col = 0; col < 3; ++col){
	tempRot(3, col) = 0.0;
    }
    
    // fill the 4th column:
    for (row = 0; row < 3; ++row){
	tempRot(row, 3) = 0.0;
    }
    
    *fCovMatrix = fCovMatrix->Similarity(tempRot);
    return *this;
}



//-------------------------------------------------------------
TLorentzVectorErr 
operator + (const TLorentzVectorErr& v, const TLorentzVectorErr& w){
    TLorentzVectorErr ve(TLorentzVector(v.X()+w.X(),v.Y()+w.Y(),v.Z()+w.Z(),
	v.T()+w.T()),
	(v.CovMatrix()+w.CovMatrix()));
    return ve;
}


//-------------------------------------------------------------
TLorentzVectorErr 
operator - (const TLorentzVectorErr& v, const TLorentzVectorErr& w){
    TLorentzVectorErr ve(TLorentzVector(v.X()-w.X(),v.Y()-w.Y(),v.Z()-w.Z(),
	v.T()-w.T()), 
	(v.CovMatrix()+w.CovMatrix()));
    return ve;
}

void TLorentzVectorErr::PrintOn(std::ostream &out) const
{
    out << X() << "\t" << Y() << "\t" << Z() << "\t" << T();
}

std::ostream& operator<<(std::ostream & stream, const TLorentzVectorErr & verr) { verr.PrintOn(stream); return stream; }
