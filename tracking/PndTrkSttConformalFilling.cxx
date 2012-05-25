#include "PndTrkSttConformalFilling.h"
#include <cmath>
#include <iostream>
// Root includes
#include "TROOT.h"

using namespace std;

#define PI		3.141592654

//----------begin of function PndTrkSttConformalFilling::BoxConformalFilling

void PndTrkSttConformalFilling::BoxConformalFilling(
	UShort_t *FiConformalIndex,
	void* HitsinBoxConf,
	bool *InclusionListStt,
	Double_t infoparalConformal[][5],
	UShort_t *ListSttParHits,
	UShort_t MAXHITSINCELL,
	void* nBoxConf,
	UShort_t NFIDIVCONFORMAL,
	UInt_t Nparal,
	UShort_t NRDIVCONFORMAL,
	Double_t *radiaConf,
	UShort_t *RConformalIndex
						)
{

 Short_t
	i,
	iFi,
	iR,
	j;

 Double_t Fi;

 // make a POINTER to an ARRAY[NFIDIVCONFORMAL] of UShort_t and assign value present in the calling
 // sequence of this method;
 UShort_t (*nBoxConformal)[NFIDIVCONFORMAL] = (UShort_t (*)[NFIDIVCONFORMAL]) nBoxConf;

 // make a POINTER to an ARRAY[NRDIVCONFORMAL][NFIDIVCONFORMAL] of UShort_t and assign value present
 // in the calling sequence of this method;
 UShort_t (*HitsinBoxConformal)[NRDIVCONFORMAL][NFIDIVCONFORMAL]=
		(UShort_t (*)[NRDIVCONFORMAL][NFIDIVCONFORMAL])HitsinBoxConf;

 for(i = 0; i< NRDIVCONFORMAL ; i++){
	for(j = 0; j< NFIDIVCONFORMAL ; j++){
		nBoxConformal[i][j]= 0;
	}
 }

 for(i = 0; i< Nparal ; i++){
	if( ! InclusionListStt[ ListSttParHits[i] ] ) continue;
	Fi = atan2(infoparalConformal[ListSttParHits[i]][1],
		infoparalConformal[ListSttParHits[i]][0]) ;
	if ( Fi < 0. ) Fi += 2.*PI;
	iFi =  (Short_t) (0.5*NFIDIVCONFORMAL*Fi/PI);
	if(iFi > NFIDIVCONFORMAL ) {
		iFi = NFIDIVCONFORMAL;
	} else if (iFi<0) {
		iFi = 0;
	}


	Double_t RRR = sqrt(infoparalConformal[ListSttParHits[i]][0]*
			infoparalConformal[ListSttParHits[i]][0]+
			infoparalConformal[ListSttParHits[i]][1]*
			infoparalConformal[ListSttParHits[i]][1]);

	for(j=NRDIVCONFORMAL-1, iR=0; j>0; j--){ 
		if( RRR> radiaConf[j] ){
			iR = j;
			break;
		}
	}
	if( nBoxConformal[iR][iFi] >= MAXHITSINCELL ){
		cout<<"Warning from PndTrkSttConformalFilling::BoxConformalFilling\t:"
		<<"\n\tcontent in nBoxConformal["<<iR<<"]["<<
		iFi<<"] has reached the Max allowed value = "
		<<MAXHITSINCELL<<endl;
		 continue;
	}
	HitsinBoxConformal[ nBoxConformal[iR][iFi] ][iR][iFi]
		=(UShort_t) ListSttParHits[i];
	nBoxConformal[iR][iFi]++;
	RConformalIndex[ ListSttParHits[i] ]  =  iR;
	FiConformalIndex[ ListSttParHits[i] ]  =  iFi;
 }  // end of for(i = 0; i< Nparal ; i++)



 return;

}
//----------end of function PndTrkSttConformalFilling::BoxConformalFilling





//----------begin of function PndTrkSttConformalFilling::FromXYtoConformal

void PndTrkSttConformalFilling::FromXYtoConformal(
	Double_t trajectory_vertex[2],
	Double_t info[][7],
	UShort_t *ListSttParHits,
	UInt_t Nparal,
	Double_t infoparalConformal[][5],
	Double_t STRAWRADIUS
		)
{


//   do the transformation in the conformal space :  u= x/(x**2+y**2), v= y/(x**2+y**2) for each hit from parallel
//   straws;  also the equidrift radius changes.

//

  UShort_t
	i;

  Double_t
	gamma,
	r,
	x,
	y;

  for(i=0; i<Nparal; i++){
	x = info[ListSttParHits[i]][0]-trajectory_vertex[0];
	y = info[ListSttParHits[i]][1]-trajectory_vertex[1];
	r = info[ListSttParHits[i]][3];
	gamma = x*x + y*y - r*r;
	infoparalConformal[ListSttParHits[i]][0] = x / gamma;
	infoparalConformal[ListSttParHits[i]][1] = y / gamma;
	infoparalConformal[ListSttParHits[i]][2] = r/fabs(gamma);
	//  n. of the Hit (in the original order)
	infoparalConformal[ListSttParHits[i]][3] = ListSttParHits[i] ;
	infoparalConformal[ListSttParHits[i]][4] = STRAWRADIUS/fabs(gamma);

  }


  return;
}

//----------end of function PndTrkSttConformalFilling::FromXYtoConformal




ClassImp(PndTrkSttConformalFilling)

