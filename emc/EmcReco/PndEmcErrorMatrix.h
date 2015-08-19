//--------------------------------------------------------------------------
// Description:
//	Class PndEmcErrorMatrix
//      Calculate Error Matrix for the given EmcCluster
//      with parametrization defined by the given parameter PndEmcErrorMatrixPar
//
//------------------------------------------------------------------------

//#pragma once
#ifndef PNDEMCERRORMATRIX_H
#define PNDEMCERRORMATRIX_H

#include "TMatrixD.h"
#include "PndEmcCluster.h"
#include "TString.h"
#include <cstdlib>

class PndEmcErrorMatrixPar;
class PndEmcErrorMatrixParObject;

class PndEmcErrorMatrix 
{
	public:
 
	// Constructors
	PndEmcErrorMatrix();
	// Destructor
	~PndEmcErrorMatrix();
	
	void Init(PndEmcErrorMatrixParObject *par);
	void InitFromFile(Int_t geomVersion);
	PndEmcErrorMatrixParObject* GetParObject();
	
	// Error Matrix for Cluster
	// 4x4 matrix in E, theta, phi, R
	TMatrixD GetErrorMatrix(const PndEmcCluster &cluster) const;
		
	// 4x4 matrix in (px, py, pz, E)
	TMatrixD Get4MomentumErrorMatrix(const PndEmcCluster &cluster) const;

	// 7x7 matrix in (x, y, z, px, py, pz, E)
	TMatrixD GetErrorP7(const PndEmcCluster &cluster) const;
	 
	private:
		PndEmcErrorMatrixParObject *fErrorMatrixParObject;

        PndEmcErrorMatrix(const  PndEmcErrorMatrix& L);
        PndEmcErrorMatrix& operator= (const  PndEmcErrorMatrix&) {return *this;};
};

// helper function to convert error matrix
// copied from BbrGeom/BbrError.cc
TMatrixD similarityWith(const TMatrixD& mat, const TMatrixD& m1);

#endif
