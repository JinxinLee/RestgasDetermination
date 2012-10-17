#include "PndTrkChi2Fits.h"
// #include "PndTrkTracking.h"
#include <cmath>
#include <iostream>
// Root includes
#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

using namespace std;

#define PI	3.141592654

 /** Default constructor **/
PndTrkChi2Fits::PndTrkChi2Fits(){};

//----------begin of function PndTrkChi2Fits::FitHelixCylinder

Short_t PndTrkChi2Fits::FitHelixCylinder(
	Short_t nHitsinTrack,
	Double_t *Xconformal,
	Double_t *Yconformal,
	Double_t *DriftRadiusconformal,
	Double_t *ErrorDriftRadiusconformal,
	Double_t rotationangle,
	Double_t trajectory_vertex[2],
	Short_t NMAX,
	Double_t *emme,
	Double_t *qu,
	Double_t *pAlfa, // this is input and output;
	Double_t *pBeta, // this is input and output;
	Double_t *pGamma, // this is input and output;
	bool *Type,
	int istampa,
	int IVOLTE
	)
{

 int
	i;

 Double_t
	alfetta,
	cose = cos(rotationangle),
	dete,
	mm,
	sigma2,
	sine = sin(rotationangle),
	Su,
	Suu,
	Suv,
	Sv,
	S1,
	ui,
	u1,
	u2,
	vi,
	v1,
	v2;

 // the parameters of the track trajectory in conformal space is a straight line;
 // here it is parametrized as :  (*pBeta) * V + (*pAlfa)*U +1 =0
 // where U, V are the conformal coordinates :  U = X/( X**2 + Y**2 );
 // In this method *pAlfa, *pBeta are inputs from the fit to the candidate track
 // performed earlier; also, assume *pGamma = 0.

 // perform the required translation of coordinates in new center trajectory_vertex[2]
 // in XY space !!
 // The arrays Xconformal and Yconformal are SUPPOSED TO BE CALCULATED ALREADY TAKING
 // INTO ACCOUNT THE TRANSLATION, therefore no need to modify them here for that. However
 // they still need to be rotated.

 // modify the input values for the translation :
 *pAlfa += 2.*trajectory_vertex[0];
 *pBeta += 2.*trajectory_vertex[1];

 // now take into account the rotation by rotationangle;

 alfetta = *pAlfa;
 *pAlfa = *pAlfa*cose + *pBeta*sine;
 *pBeta = -alfetta*sine + *pBeta*cose;

 Double_t
	Xp[nHitsinTrack],
	Yp[nHitsinTrack];

 // rotate the points;
 for(i=0;i<nHitsinTrack; i++){
	Xp[i] =  Xconformal[ i ] *cose + Yconformal[ i ]*sine;
	Yp[i] = -Xconformal[ i ] *sine + Yconformal[ i ]*cose;
 }
//--------------------

 // starts fitting procedure by finding the POCA first and then doing the Chi**2 minimization;

 mm = sqrt( (*pAlfa)*(*pAlfa) + (*pBeta)*(*pBeta) );

 // find the POCA of the Stt hit or Mvd hit to the original trajectory in conformal space;

 for(i=0; i<nHitsinTrack; i++){

	if( DriftRadiusconformal[i]<0){  // this is a Mvd hit;
		ui = Xp[i];  // U
		vi = Yp[i];  // V
	}else {  // this is a Stt axial;
		u1 = Xp[i] - DriftRadiusconformal[i] * (*pAlfa)/mm;
		v1 = Yp[i] - DriftRadiusconformal[i] * (*pBeta)/mm;
		u2 = Xp[i] + DriftRadiusconformal[i] * (*pAlfa)/mm;
		v2 = Yp[i] + DriftRadiusconformal[i] * (*pBeta)/mm;
		// poca is the point closest to the straight line;
		fabs((*pBeta)*v1+(*pAlfa)*u1+1) < fabs((*pBeta)*v2+(*pAlfa)*u2+1) ?
			ui = u1, vi = v1 :
			ui = u2, vi = v2 ;
	}

	// now the minimization with the chi2 formula;
	sigma2 = ErrorDriftRadiusconformal[i]*ErrorDriftRadiusconformal[i];
	Su += ui/sigma2;
	Sv += vi/sigma2;
	
	Suv += ui * vi/sigma2;
	Suu += ui* ui/sigma2;
	
	S1 += 1./sigma2;

 }  // end of for(i=0; i<nHitsinTrack; i++)


	dete = Suu * S1 - Su * Su;
	if(fabs(dete) < 1e-10) return -5;   // fit fails;

	*emme = (Suv * S1 - Su * Sv)/dete;
	*qu =  (Suu * Sv - Su * Suv)/dete;
	// protect the extreme case of q=0 (in principle not possible, it would
	// correspond to a straight line trajectory in the XY plane;
	if( fabs(*qu) < 1.e-9 ) {
		if ((*qu) > 0. ) *qu = 1.e-9 ; else *qu = -1.e-9;
	}

 // calculate the output coefficients of the circumference in XY plane;

	*pAlfa = (*emme)/(*qu);
	*pBeta = -1./(*qu);

//  now take into account the rotation and correct back; the affected quantities are ALFA and BETA,
//  (*emme and *qu also but later);
 alfetta = *pAlfa;
 *pAlfa = *pAlfa*cose - *pBeta*sine;
 *pBeta = alfetta*sine + *pBeta*cose;

// calculate the coefficients taking into account the translation;
// now take into account the displacement and calculate Gamma;

 *pGamma = (trajectory_vertex[0]*trajectory_vertex[0]+   // *pGamma is assumed to be 0 at the beginning;
		trajectory_vertex[1]*trajectory_vertex[1]
		-*pAlfa*trajectory_vertex[0]-*pBeta*trajectory_vertex[1]);

 // calculate the coefficients taking into account the translation;
 *pAlfa -=  2.*trajectory_vertex[0];
 *pBeta -=  2.*trajectory_vertex[1];


// calculate  *emme and *qu using the newly calculated *pAlfa, *pBeta, *pGamma of the
// circular trajectory in XY. Assuming also that *pGamma ~ 0, that is, the circumference
// goes thru the origin.

 if( abs(*pBeta)>1e-9) {
 // normal case of a straight line in UV that can be put in the    V = m*U +q  form;
	*emme = -(*pAlfa)/(*pBeta);
	*qu  = -1./(*pBeta);
	return 1;
  } else {
	*emme = -(*pAlfa)/1e-9;
	*qu  = -1./1e-9;
	return 99;
 }

}
//----------end of function PndTrkChi2Fits::FitHelixCylinder




ClassImp(PndTrkChi2Fits)

