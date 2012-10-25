#include "PndTrkChi2Fits.h"
#include "PndTrkVectors.h"
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
	Alfa,
	alfetta,
	Beta,
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
 Alfa = (*pAlfa) + 2.*trajectory_vertex[0];
 Beta = (*pBeta) + 2.*trajectory_vertex[1];


 // now take into account the rotation by rotationangle;

 alfetta = Alfa;
 Alfa = Alfa*cose + Beta*sine;
 Beta = -alfetta*sine + Beta*cose;


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
 // use the starting value of Alfa and Beta for finding the POCA 


 mm = sqrt( Alfa*Alfa + Beta*Beta );

 // find the POCA of the Stt hit or Mvd hit to the original trajectory in conformal space;

 Su = 0.;
 Sv = 0.;
 Suv = 0.;
 Suu = 0.;
 S1 = 0.;
 for(i=0; i<nHitsinTrack; i++){

	if( DriftRadiusconformal[i]<0){  // this is a Mvd hit;
		ui = Xp[i];  // U
		vi = Yp[i];  // V
	}else {  // this is a Stt axial;
		u1 = Xp[i] - DriftRadiusconformal[i] * Alfa/mm;
		v1 = Yp[i] - DriftRadiusconformal[i] * Beta/mm;
		u2 = Xp[i] + DriftRadiusconformal[i] * Alfa/mm;
		v2 = Yp[i] + DriftRadiusconformal[i] * Beta/mm;
		// poca is the point closest to the straight line;
		if(fabs(Beta*v1+Alfa*u1+1) < fabs(Beta*v2+Alfa*u2+1) ){
			ui = u1;
			vi = v1;
		} else {
			ui = u2;
			vi = v2;
		}
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

	Alfa = (*emme)/(*qu);
	Beta = -1./(*qu);

//  now take into account the rotation and correct back; the affected quantities are ALFA and BETA,
//  (*emme and *qu also but later);
 alfetta = Alfa;
 Alfa = Alfa*cose - Beta*sine;
 Beta = alfetta*sine + Beta*cose;

// calculate the coefficients taking into account the translation;
// now take into account the displacement and calculate Gamma;

 *pGamma = trajectory_vertex[0]*trajectory_vertex[0]+   // *pGamma is assumed to be 0 at the beginning;
		trajectory_vertex[1]*trajectory_vertex[1]
		-Alfa*trajectory_vertex[0]-Beta*trajectory_vertex[1];

 // calculate the coefficients taking into account the translation;
 Alfa -=  2.*trajectory_vertex[0];
 Beta -=  2.*trajectory_vertex[1];




// calculate  *emme and *qu using the newly calculated *pAlfa, *pBeta, *pGamma of the
// circular trajectory in XY. Assuming also that *pGamma ~ 0, that is, the circumference
// goes thru the origin.

 if( abs(Beta)>1e-9) {
 // normal case of a straight line in UV that can be put in the    V = m*U +q  form;
	*emme = -Alfa/Beta;
	*qu  = -1./Beta;
	*pAlfa = Alfa;
	*pBeta = Beta;
	return 1;
  } else {
	*emme = -Alfa/1e-9;
	*qu  = -1./1e-9;
	*pAlfa = Alfa;
	*pBeta = Beta;
	return 99;
 }

}
//----------end of function PndTrkChi2Fits::FitHelixCylinder


//----------begin of function PndTrkChi2Fits::FitSZspace

  Short_t PndTrkChi2Fits::FitSZspace(
	Short_t nHitsinTrack,
	Double_t *S,
	Double_t *Z,
	Double_t *DriftRadius,
	Double_t *ErrorDriftRadius,
	Double_t FInot,
	Short_t NMAX,
	Double_t *emme,
	int IVOLTE
	)
{

 if(nHitsinTrack<0)
 {
	cout<<"from PndTrkChi2Fits::FitSZspace  error, n hits to fit = 0, exit(-1).";
	exit(-1);
 }


 int
	Combinations,
	i,
	icomb,
	nSttHits;

 Double_t
	e2,
	mvdGSum,
	mvdUinvSum,
	mvdZqSum,
	sttS[nHitsinTrack],  // S of only the STT hits;
			// this is a little overdimensioned, but easy to do;
	sttSigma[nHitsinTrack],  // as above; first possibility of Z;
	sttZ1[nHitsinTrack],  // as above; first possibility of Z;
	sttZ2[nHitsinTrack]  // as above; second possibility of Z;
	;



 nSttHits = 0 ;
 mvdGSum=0.;
 mvdUinvSum=0.;
 mvdZqSum=0.;

 for(i=0; i<nHitsinTrack; i++){
	if( DriftRadius[i]<0. )
	{
		// Mvd hit;
		// relevant quantities for the chi2 calculation and minimization;
		e2 = ErrorDriftRadius[i]*ErrorDriftRadius[i];
		mvdGSum += Z[i]*(S[i]-FInot)/e2;
		mvdUinvSum += (S[i]-FInot)*(S[i]-FInot)/e2;
		mvdZqSum += Z[i]*Z[i]/e2 ;


	} else {
		// Stt hit;
		sttS[nSttHits] = S[i];
		sttSigma[nSttHits] = ErrorDriftRadius[i];
		sttZ1[nSttHits] = Z[i] + DriftRadius[i];
		sttZ2[nSttHits] = Z[i] - DriftRadius[i];
		nSttHits++;
	}
 } // end of  for(i=0; i<nHitsinTrack; i++)

// limit the number of Skew hits in fit, otherwise the number of combinations
// becomes too large;
 if(nSttHits>NMAX) nSttHits=NMAX;


//---- begin the fit procedure. Here S is the INDEPENDENT-like variable and  Z the
//     dependent-like one.
// For each Stt (Skew) hit the two possibilities are taken into account; therefore the
// number f chi**2 are  2**nSttHits  ; at the end of the procedure the Stt (Skew) hit pattern
// is chosen with the least chi**2;
// use a recursive algorithm to calculate all the chi**2 and minimize them because the
// a priori is NOT know how may Stt (Skew) hits are there in the track candidate.


 Combinations = pow(2,nSttHits);

 Double_t
	GSum[Combinations],
	UinvSum[Combinations],
	ZqSum[Combinations];



 Double_t
	chi2,
	Chi2min,
	Kappa,
	TotalGSum,
	TotalUinvSum,
	TotalZqSum;


 if(nSttHits>0){
//  the following is a recursive function;

 GSumCalculation(
		sttS,	// input; the independent-like variable;
		sttZ1,	// input; the dependent-like variable;
		sttZ2,	// input; the dependent-like variable;
		sttSigma, // input; the errors on sttZ;
		FInot,
		nSttHits,
		GSum	// the output; this must be an array of 2**nSttHits elements;
	);



//  the following is a recursive function;

 UinvSumCalculation(
		sttS,	// input; the independent-like variable;
		sttSigma, // input; the errors on sttZ;
		FInot,
		nSttHits,
		UinvSum	// the output; this must be an array of 2**nSttHits elements;
	);

//  the following is a recursive function;

 ZqSumCalculation(
		sttZ1,	// input; the dependent-like variable;
		sttZ2,	// input; the dependent-like variable;
		sttSigma, // input; the errors on sttZ;
		FInot,
		nSttHits,
		ZqSum	// the output; this must be an array of 2**nSttHits elements;
	);

//  calculation of the Kappa parameter (the parametrization is : Z = (S-FInot)/Kappa );
//  and the all chi**2;

 // the first combination;

 TotalGSum = mvdGSum + GSum[0];
 TotalUinvSum = mvdUinvSum + UinvSum[0];
 TotalZqSum = mvdZqSum + ZqSum[0];
 // calculation of the fit parameter obtained by minimization of the chi**2;
 if(fabs(TotalGSum)>1e-9)
 {
	Kappa = TotalUinvSum / TotalGSum;
 }else {
	Kappa = 1e9;
 }
 // calculation of the corresponding  chi**2;
 Chi2min = TotalZqSum + TotalUinvSum/Kappa -2.*TotalGSum/Kappa;
 // for the moment this is the solution;
 *emme = Kappa;

 // all the other combinations;
 for(icomb=1; icomb<Combinations; icomb++){
	TotalGSum =  mvdGSum + GSum[icomb];
	TotalUinvSum =  mvdUinvSum + UinvSum[icomb];
	TotalZqSum = mvdZqSum + ZqSum[icomb];
	// calculation of the fit parameter obtained by minimization of the chi**2;
	if(fabs(TotalGSum) > 1.e-8){
		Kappa = TotalUinvSum / TotalGSum;
		// calculation of the corresponding  chi**2;
		chi2 = TotalZqSum + TotalUinvSum/Kappa -2.*TotalGSum/Kappa;
		// choose the solution with best chi**2;
		if( chi2 <Chi2min) *emme = Kappa;
	} else {
		if(TotalGSum>0.) Kappa = 1.e8 ; else Kappa = -1.e8 ;
	}  // end of if(fabs(TotalGSum) > 1.e-8)

 }  // end of for(icomb=1; icomb<Combinations; i++)


 } else {  // continuation of   if(nSttHits>0)
	// here nSttHits is 0;
	TotalGSum = mvdGSum;
	TotalUinvSum = mvdUinvSum;
	TotalZqSum = mvdZqSum ;

	if(fabs(TotalGSum) > 1.e-8){
		Kappa = TotalUinvSum / TotalGSum;
		// choose the solution with best chi**2;
		*emme = Kappa;
	} else {
		if(TotalGSum>0.) Kappa = 1.e8 ; else Kappa = -1.e8 ;
	}  // end of if(fabs(TotalGSum) > 1.e-8)

 }  // end of  if(nSttHits>0)



return 1;
}

//----------end of function PndTrkChi2Fits::FitSZspace







//----------begin of function PndTrkChi2Fits::GSumCalculation

 void PndTrkChi2Fits::GSumCalculation(
		Double_t *S,	// input; the independent-like variable;
		Double_t *Z1,	// input, first possibility of Z; the dependent-like variable;
		Double_t *Z2,	// input; second possibility of Z; the dependent-like variable;
		Double_t *Sigma, // input; the errors on Z;
		Double_t FInot,  // input fixed parameter;
		int nHits,
		Double_t *outSum // the output; this must be an array of 2**nSttHits elements;
	)
{

 int	i,
	Combinations,
	Combinations2;

 double	e2;


 // this method calculates the sum calleg   g   in the PDG minimization formula;
 // it is a recursive method;


 Combinations = pow(2, nHits);
 Combinations2 = pow(2, nHits-1);

 if(nHits==1)
 {
	// two possible combinations;
	outSum[0] = Z1[0]*(S[0]-FInot)/(Sigma[0]*Sigma[0]);
	outSum[1] = Z2[0]*(S[0]-FInot)/(Sigma[0]*Sigma[0]);
	return;
 }

 Double_t aux[Combinations];

 // the following is the recursion;

 GSumCalculation(
		S,	// input; the independent-like variable;
		Z1,	// input; the dependent-like variable;
		Z2,	// input; the dependent-like variable;
		Sigma, // input; the errors on sttZ;
		FInot,
		nHits-1,
		outSum	// the output; this must be an array of 2**nSttHits elements;
	);

 // for each of the   Combinations2 = 2**(nHits-1) already calculated combinations
 // of Stt Skew hits, calculate two combinations by using the last SttSkew hit
 // (its Z1, Z2, S are Z1[nHits-1], Z2[nHits-1] and so on);
 // in this way a total of    Combinations = 2**nHits  combinations exist now.

 for(i=0; i<Combinations2; i++){
	e2 = Sigma[nHits-1]*Sigma[nHits-1];
	aux[i] = outSum[i] + Z1[nHits-1]*(S[nHits-1]-FInot)/e2;
	aux[i+Combinations2] = outSum[i] + Z2[nHits-1]*(S[nHits-1]-FInot)/e2;
 }  // end of for(i=0; i<Combinations; i++)

 // reload the output array (that has now more elements);
 for(i=0; i<Combinations; i++){
	outSum[i] = aux[i];
 }


 return;
}
//----------end of function PndTrkChi2Fits::GSumCalculation






//----------begin of function PndTrkChi2Fits::UinvSumCalculation

 void PndTrkChi2Fits::UinvSumCalculation(
		Double_t *S,	// input; the independent-like variable;
		Double_t *Sigma, // input; the errors on Z;
		Double_t FInot,  // input fixed parameter;
		int nHits,
		Double_t *outSum	// the output; this must be an array of 2**nSttHits elements;
	)
{

 int	i,
	Combinations,
	Combinations2;

 double	e2;

 // this method calculates the sum calleg   g   in the PDG minimization formula;
 // it is a recursive method;


 Combinations = pow(2, nHits);
 Combinations2 = pow(2, nHits-1);

 if(nHits==1)
 {
	// two possible combinations; they have the same Uinv;
	outSum[0] = (S[0]-FInot)*(S[0]-FInot)/(Sigma[0]*Sigma[0]);
	outSum[1] = outSum[0];
	return;
 }

 Double_t aux[Combinations];

 // the following is the recursion;

 UinvSumCalculation(
		S,	// input; the independent-like variable;
		Sigma, // input; the errors on sttZ;
		FInot,
		nHits-1,
		outSum	// the output; this must be an array of 2**nSttHits elements;
	);

 // for each of the   Combinations2 = 2**(nHits-1) already calculated combinations
 // of Stt Skew hits, calculate two combinations by using the last SttSkew hit
 // (its Z1, Z2, S are Z1[nHits-1], Z2[nHits-1] and so on);
 // in this way a total of    Combinations = 2**nHits  combinations exist now.

 for(i=0; i<Combinations2; i++){
	e2 = Sigma[nHits-1]*Sigma[nHits-1];
	// two combinations but with the same U inv;
	aux[i] = outSum[i] + (S[nHits-1]-FInot)*(S[nHits-1]-FInot)/e2;
	aux[i+Combinations2] = aux[i] ;
 }  // end of for(i=0; i<Combinations; i++)

 // reload the output array (that has now more elements);
 for(i=0; i<Combinations; i++){
	outSum[i] = aux[i];
 }

 return;
}
//----------end of function PndTrkChi2Fits::UinvSumCalculation





//----------begin of function PndTrkChi2Fits::ZqSumCalculation

 void PndTrkChi2Fits::ZqSumCalculation(
		Double_t *Z1,	// input, first possibility of Z; the dependent-like variable;
		Double_t *Z2,	// input; second possibility of Z; the dependent-like variable;
		Double_t *Sigma, // input; the errors on Z;
		Double_t FInot,  // input fixed parameter;
		int nHits,
		Double_t *outSum	// the output; this must be an array of 2**nSttHits elements;
	)
{

 int	i,
	Combinations,
	Combinations2;

 double	e2;

 // this method calculates the sum calleg   g   in the PDG minimization formula;
 // it is a recursive method;


 Combinations = pow(2, nHits);
 Combinations2 = pow(2, nHits-1);

 if(nHits==1)
 {
	// two possible combinations;
	outSum[0] = Z1[0]*Z1[0]/(Sigma[0]*Sigma[0]);
	outSum[1] = Z2[0]*Z2[0]/(Sigma[0]*Sigma[0]);
	return;
 }

 Double_t aux[Combinations];

 // the following is the recursion;

 ZqSumCalculation(
		Z1,	// input; the dependent-like variable;
		Z2,	// input; the dependent-like variable;
		Sigma, // input; the errors on sttZ;
		FInot,
		nHits-1,
		outSum	// the output; this must be an array of 2**nSttHits elements;
	);

 // for each of the   Combinations2 = 2**(nHits-1) already calculated combinations
 // of Stt Skew hits, calculate two combinations by using the last SttSkew hit
 // (its Z1, Z2, S are Z1[nHits-1], Z2[nHits-1] and so on);
 // in this way a total of    Combinations = 2**nHits  combinations exist now.

 for(i=0; i<Combinations2; i++){
	e2 = Sigma[nHits-1]*Sigma[nHits-1];
	aux[i] = outSum[i] + Z1[nHits-1]*Z1[nHits-1]/e2;
	aux[i+Combinations2] = outSum[i] + Z2[nHits-1]*Z2[nHits-1]/e2;
 }  // end of for(i=0; i<Combinations; i++)

 // reload the output array (that has now more elements);
 for(i=0; i<Combinations; i++){
	outSum[i] = aux[i];
 }


 return;
}
//----------end of function PndTrkChi2Fits::ZqSumCalculation








ClassImp(PndTrkChi2Fits)

