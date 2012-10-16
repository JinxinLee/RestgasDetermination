#include "PndTrkLegendreFits.h"
#include "PndTrkTracking.h"
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
PndTrkLegendreFits::PndTrkLegendreFits()
{
 fNThetaDiv = 360,  // this is one of the dimension of the accumulation Matrix;
 fNRDiv = 100;   // this is the other dimension;
 fRMin=0.;
 fThetaMax=2.*PI,
 fThetaMin=0.;
 fDeltaT = (fThetaMax-fThetaMin)/fNThetaDiv;

 fIcounter =0;
};

//----------begin of function PndTrkLegendreFits::FindMaximumInMatrix

int PndTrkLegendreFits::FindMaximumInMatrix(
		int nRDiv,		// input, n. division in the R axis;
		int nThetaD,	// input  n. division in the Theta axis;
		UShort_t *Mat,	// input Matrix of dimensions [fNRDiv][fNThetaDiv] of
				// which the Maximum has to be searched;

		int *iRMax,		// output, bin in R of the location of the Maximum;
		int *iTMax		// output, bin in Theta of the location of the Maximum;
						)
{

 int i,j;

 // initializations;
 int max = -99999;

 for(i=0;i<nRDiv;i++){
   for(j=0;j<nThetaD;j++){
	if(Mat[i*nThetaD+j]>max){
		max = Mat[i*nThetaD+j];
		*iRMax = i;
		*iTMax = j;
	}  // end of if(Max[i*nThetaD+j]>max)
   }
 }  // end of for(i=0;i<nRDiv;i++)

 return  max;

}
//----------end of function PndTrkLegendreFits::FindMaximumInMatrix





//----------begin of function PndTrkLegendreFits::FitHelixCylinder

Short_t PndTrkLegendreFits::FitHelixCylinder(
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
	Double_t *pAlfa,
	Double_t *pBeta,
	Double_t *pGamma,
	bool *Type,
	int istampa,
	int IVOLTE
	)
{

 int result;

 Double_t
	cosT,
	sinT,
	R,	// output parameter of the straight line; Xcos(Theta)+Y*sin(Theta)=R;
	Theta;  // output parameter of the straight line; Xcos(Theta)+Y*sin(Theta)=R;

fIcounter=IVOLTE;
 result = LoadMatrix_FindMaximum(
		nHitsinTrack,			// input
		Xconformal,			// X position (in conformal or SZ or whatever);
		Yconformal,			// Y position (in conformal or SZ or whatever);
		DriftRadiusconformal,		// negative if Mvd hit or similar;
		ErrorDriftRadiusconformal,	// for the Mvd this is the Radius of the circumference
						// translated with the Conformal transformation,
						// which is, in the XY space : a) centered on the Pixel
						// or Strip; with radius = 0.01 cm --> therefore encompassing
						// completely the Pixel or Stip hit.

		&R,	// output parameter of the straight line; Xcos(Theta)+Y*sin(Theta)=R;
		&Theta  // output parameter of the straight line; Xcos(Theta)+Y*sin(Theta)=R;
				);

 if (result<0) return result;

 cosT = cos(Theta);
 sinT = sin(Theta);

 //------------------------- final summary of the fit results; load output variables;

 *pGamma = 0.;

 // R != 0 --> normal case : the trajectory is a circle in XY passing for the origin;
 if( fabs( R ) > 1.e-10) {
	*pAlfa = -cosT/R;
	*pBeta = -sinT/R;
	*Type=true;
 } else {
 // in this case the trajectory is a straight line :     Y*sin(Theta) + X*cos(Theta) = 0
 // in XY;
	*Type=false;
	return 1; // the fit did not fail anyway, so return value>0 ;
 }



// now take into account the displacement and correct
 *pGamma += (trajectory_vertex[0]*trajectory_vertex[0]+
		trajectory_vertex[1]*trajectory_vertex[1]
		-*pAlfa*trajectory_vertex[0]-*pBeta*trajectory_vertex[1]);
 *pAlfa -=  2.*trajectory_vertex[0];
 *pBeta -=  2.*trajectory_vertex[1];


// calculate  *emme and *qu using the newly calculated *pAlfa, *pBeta, *pGamma of the
// circular trajectory in XY. Assuming also that *pGamma ~ 0, that is the circumference
// goes thru the origin.

 if( abs(*pBeta)>1e-10) {
 // normal case of a straight line in UV that can be put in the    V = m*U +q  form;
	*emme = -(*pAlfa)/(*pBeta);
	*qu  = -1./(*pBeta);
	return 1;
  } else {
	*emme = -(*pAlfa)/1e-10;
	*qu  = -1./1e-10;
	return 99;
 }

}
//----------end of function PndTrkLegendreFits::FitHelixCylinder



//----------begin of function PndTrkLegendreFits::FitSZspace

Short_t PndTrkLegendreFits::FitSZspace(
	Short_t nSkewHitsinTrack,
	Double_t *S,
	Double_t *Z, //
	Double_t *DriftRadiusProjected,
	Double_t *ErrorDriftRadiusProjected, //
	Double_t FInot,
	Short_t NMAX,
	Double_t *emme,
	int IVOLTE
	)
{

 const int
	MAXCONTENT= 30000,
	ThetaDiv = 180;

 int
	i,
	CellMax,
	ncell,
	result;

 UShort_t
	Matrix[ThetaDiv];

 Double_t
	delta,
	max,
	theta1,
	theta2,
	ThetaMax;

 // this fit in SZ is a fit with only 1 variable :  S = kappa * Z + fi0   with kappa
 // the variable, and  fi0 = FInot  is fixed (obtained by the previous XY fit);
 // therefore here it is calculated an 'accumulation' plot in only 1 dimension.
 // The variable chosen is  Theta --> tan(Theta) = kappa; in this way Theta is a
 // variable bound between 0 and PI;   the equation is then
 // S = tan(Theta) * Z + fi0.
 // For each Stt Skew hit I calculate 2 entries (only 2  are the possible tangents to
 // drift elipsoid, having a fixed fi0) in the accumulation plot and onl;y 1 entry for
 // the Mvd and SciTil hits.


 // reset the Matrix;
 size_t len;
 len = sizeof(Matrix);
 memset (Matrix,0,len);

 // filling the matrix;

 for(i=0;i<nSkewHitsinTrack;i++){
    if(DriftRadiusProjected[i]<0.){
	// Stt hit : 2 possible tangent;

	// calculation of the first possible tangent;
	if( abs(Z[i]+DriftRadiusProjected[i])>1.e-10){
		theta1 = atan( (S[i]-FInot)/(Z[i]+DriftRadiusProjected[i]) );
	} else {
		theta1 = PI/2.;
	}
	// filling the Matrix; keep in mind that   atan  goes from -PI/2 to PI/2
	// and so theta1, theta2 do.
	ncell = (int) ( (theta1 +PI/2.)/ThetaDiv );
	if(ncell <0) { ncell=0; } else if (ncell>=ThetaDiv) {ncell = ThetaDiv-1;}
	Matrix[ncell]++;

	// calculation of the second possible tangent;
	if( abs(Z[i]-DriftRadiusProjected[i])>1.e-10){
		theta2 = atan( (S[i]-FInot)/(Z[i]-DriftRadiusProjected[i]) );
	} else {
		theta2 = PI/2.;
	}
	// filling the Matrix; keep in mind that   atan  goes from -PI/2 to PI/2
	// and so theta1, theta2 do.
	ncell = (int) ( (theta2 + PI/2.)/ThetaDiv );
	if(ncell <0) { ncell=0; } else if (ncell>=ThetaDiv) {ncell = ThetaDiv-1;}
	Matrix[ncell]++;

    } else {
	// Mvd or SciTil hit : only 1 tangent is possible;

	// calculation of the only possible tangent;
	if( abs(Z[i])>1.e-10){
		theta1 = atan( (S[i]-FInot)/Z[i] );
	} else {
		theta1 = PI/2.;
	}
	// filling the Matrix; keep in mind that   atan  goes from -PI/2 to PI/2
	// and so theta1, theta2 do.
	ncell = (int) ( (theta1 +PI/2.)/ThetaDiv );
	if(ncell <0) { ncell=0; } else if (ncell>=ThetaDiv) {ncell = ThetaDiv-1;}
	Matrix[ncell]++;
    }
 }   // end of  for(i=0;i<nSkewHitsinTrack;i++)


 // find the cell of the maximum in the Matrix;
 max = Matrix[0];
 CellMax = 0;
 for(i=1;i<ThetaDiv;i++){
	if(max<Matrix[i]) { max = Matrix[i]; CellMax = i; }
 }	// end of  for(i=0;i<ThetaDiv;i++)

 //------------------------- final summary of the fit results; load output variables;

 delta = PI/ThetaDiv;
 ThetaMax = -PI/2. + (CellMax+0.5) * delta;

 if (abs(PI/2. - ThetaMax) < 1.e-5){
	*emme = 9999999.;
	return 1;
 } else {
	*emme = tan(ThetaMax);
	return 1;
 }

}

//----------end of function PndTrkLegendreFits::FitSZspace

//----------end of function PndTrkLegendreFits::LoadMatrix_FindMaximum
int PndTrkLegendreFits::LoadMatrix_FindMaximum(
	Short_t nHitsinTrack,			// input
	Double_t *X,			// X position (in conformal or SZ or whatever);
	Double_t *Y,			// Y position (in conformal or SZ or whatever);
	Double_t *DriftRadius,		// negative if Mvd hit or similar;
	Double_t *ErrorDriftRadiusconformal,	// for the Mvd this is the Radius of the circumference
						// translated with the Conformal transformation,
						// which is, in the XY space : a) centered on the Pixel
						// or Strip; with radius = 0.01 cm --> therefore encompassing
						// completely the Pixel or Stip hit.


	Double_t *Rout,	// output parameter of the straight line; Xcos(Theta)+Y*sin(Theta)=R;
	Double_t *Thetaout  // output parameter of the straight line; Xcos(Theta)+Y*sin(Theta)=R;
					)
{
 const int MAXCONTENT= 30000;

 int	IndexR,
	IndexT,
	i,
	iRMax,
	iTMax,
	j,
	maxval;

 UShort_t
	Matrix[fNRDiv][fNThetaDiv];

 Double_t
	DeltaR,
	Drift[nHitsinTrack],
	HistoRmax,
	R,
	Theta;


 //  initialization of the Matrix that will be loaded with points;

 size_t len;
 len = sizeof(Matrix);
 memset (Matrix,0,len);
 // the following HistoRmax corresponds to 1/R**2 in XY plane with R=40 (approximately
 // the radius of a outer axial Stt hit).
 HistoRmax = 0.000625;


 // find the maximum R of the 'histogram';
 for (i=0; i<nHitsinTrack; i++){
	R = sqrt(X[i]*X[i]+Y[i]*Y[i]);

	if( DriftRadius[i]<0. ) {  // this is a Mvd point; the Pixel/Strip
		// in the Conformal plane is approximately contained in
		// a circle centered in X[i], Y[i] of radius ErrorDriftRadiusconformal[i];
		if(HistoRmax < R){ HistoRmax=R+ErrorDriftRadiusconformal[i]; }
		Drift[i] = 0;
	} else {	// this is a Stt axial hit;
		// take into consideration also the drift radius;
		R += DriftRadius[i];
		if(HistoRmax < R){ HistoRmax=R; }
		Drift[i] = DriftRadius[i];
	}
 }  // end of for (i=0; nHitsinTrack; i++)
 DeltaR = (HistoRmax-fRMin)/fNRDiv;

 // fill the Matrix;

//  char titolo[100];
//  sprintf(titolo,"Legendre%d",fIcounter);
//  TH2F * hMatrixPlot = new TH2F(titolo, "",fNRDiv, 0. , HistoRmax
//  		, fNThetaDiv, fThetaMin, fThetaMax);



 for (i=0; i<nHitsinTrack; i++){
	// for now the number of Theta generated are one per Theta bin;
	for(j=0;j<fNThetaDiv;j++) {
		// generate the Theta in the middle of the bin;
		Theta = fThetaMin + (j+0.5)*fDeltaT;
		R = X[i]*cos(Theta) + Y[i]*sin(Theta)+Drift[i];
		IndexR = (int) ((fabs(R)-fRMin)/DeltaR);
		if(IndexR>=fNRDiv) IndexR=fNRDiv-1;
		// the following is an essential calculation for Theta,
		// because it changes by 180 degrees when
		//  X[i]*cos(Theta) + Y[i]*sin(Theta)+DriftRadius[i]<0;
		if( R <0.) {
			Theta += PI;
			if(Theta>2.*PI){ Theta -= 2.*PI; }
			IndexT = (int)((Theta-fThetaMin)/fDeltaT) ;
			if(IndexT>=fNThetaDiv) IndexT=fNThetaDiv-1;
		} else {
			IndexT = j;
		}
		if( Matrix[IndexR][IndexT] < MAXCONTENT ){
			Matrix[IndexR][IndexT]++;
		}
		if(Theta == fThetaMax) Theta -= 1e-10;
//		hMatrixPlot->Fill( fabs(R),Theta);
	}
 }  // end of for (i=0; nHitsinTrack; i++)


// hMatrixPlot->Write();
// delete hMatrixPlot;

 //  find maximum in the Matrix;
 maxval = FindMaximumInMatrix(
		fNRDiv,		// input
		fNThetaDiv,	// input
		&Matrix[0][0],	// input

		&iRMax,		// output
		&iTMax		// output
		);

 //  fit failed
 if(maxval < 0) return -5;
 // the found parameters;
 *Rout = (iRMax+0.5) *DeltaR + fRMin;
 *Thetaout = (iTMax+0.5) *fDeltaT + fThetaMin;


 return 1;
}
 //----------end of function PndTrkLegendreFits::LoadMatrix_FindMaximum



ClassImp(PndTrkLegendreFits)

