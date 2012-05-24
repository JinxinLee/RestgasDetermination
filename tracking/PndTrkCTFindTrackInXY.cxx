#include "PndTrkCTFindTrackInXY.h"
#include "PndTrkCTGeometryCalculations.h"
#include "PndTrkGlpkFits.h"
#include "PndTrkCTGeometryCalculations.h"
#include "PndTrkMergeSort.h"
#include <iostream>
#include <cmath>



#define PI	3.141592654

using namespace std;


//----------begin of function PndTrkCTFindTrackInXY::AcceptHitsConformal

bool PndTrkCTFindTrackInXY::AcceptHitsConformal(
 Double_t  distance,
 Double_t  DriftConfR, //drift radius in conformal space
 Double_t  StrawConfR  // straw radius in conformal space
		)
{
  if( fabs(distance-DriftConfR) < 2.*StrawConfR ) return true;
  return false;


}

//----------end of function PndTrkCTFindTrackInXY::AcceptHitsConformal




//----------begin of function PndTrkCTFindTrackInXY::AssociateSciTilHit



UShort_t PndTrkCTFindTrackInXY::AssociateSciTilHit(
	Double_t dimensionscitil,
	Double_t *esse, // output, list of  S of the SciTil hits associated. 
	bool* InclusionListSciTil,
	UShort_t *List, // output, list of SciTil hits associated (max. 2);
	UShort_t maxscitilhitsintrack,
	UShort_t nSciTilHits,
	Double_t Oxx,
	Double_t Oyy,
	Double_t posizSciTil[][3],
	Double_t Rr
	)
{

 bool intersect;

 UShort_t
	igoodScit,
	iScitHit,
	Nint;

 Double_t
	distance,
	QQ,
	sqrtRR,
	SIGN,
	XintersectionList[2],
	YintersectionList[2];


 PndTrkCTGeometryCalculations GeomCalculator;


 igoodScit=0;
 for(iScitHit=0; iScitHit<nSciTilHits; iScitHit++){
	if(!InclusionListSciTil[iScitHit]) continue;

	intersect=GeomCalculator.IntersectionSciTil_Circle(
	  dimensionscitil,
	  posizSciTil[iScitHit][0],
	  posizSciTil[iScitHit][1],
	  Oxx,
	  Oyy,
	  Rr,
	  &Nint,  // output
	  XintersectionList,  // output
	  YintersectionList  // output
	);

	if(intersect){
		if( igoodScit == maxscitilhitsintrack) break;
		List[igoodScit] = iScitHit;

		// calculate S on the lateral face of the Helix.
		if ( Nint==1){	// the majority of the cases
			esse[igoodScit] = atan2(YintersectionList[0]-Oyy,
				XintersectionList[0]-Oxx);
		} else {  // in this case Nint=2 (it should be a very rare case).
			// do an average of the two positions.
			esse[igoodScit] = atan2( 0.5*(YintersectionList[0]+
					YintersectionList[1])-Oyy,
				0.5*(XintersectionList[0]+
					XintersectionList[1])-Oxx);
		} // end of  if ( Nint==1)
		if ( esse[igoodScit]<0.) esse[igoodScit] += 2.*PI;
		igoodScit++;

	} // end of  if(intersect && distance<olddist)
 }  // end of  for(iScitHit=0; iScitHit<nScitHits; iScitHit++)

 return igoodScit;

}

//----------end of function PndTrkSttTrackFinderReal::AssociateSciTilHit

//----------begin of function PndTrkCTFindTrackInXY::FindCharge

void PndTrkCTFindTrackInXY::FindCharge(
	Double_t oX,
	Double_t oY,
	UShort_t nParallelHits,
	Double_t *X,
	Double_t *Y,
	Short_t  * Charge
	)
{

	UShort_t ihit,
			nleft,
			nright;

	Double_t cross,
		 disq,
		 minl,
		 minr;


	// this methods works with the hypothesis that this track comes
	//  from (0,0)

	for(ihit=0, nleft=0, nright=0, minr = 9999999., minl = 9999999.; ihit<nParallelHits; ihit++){ 
	// find the Z component of the cross product between the vector from (0,0) to center of
	// circular trajectory [namely, (oX,oY) ]  and the Position vector of the center of the
	// parallel Hits [namely, (x,y)].

		cross = oX*Y[ihit] -
			oY*X[ihit];

	// if  cross >0  hits stays 'on the left' (which means clockwise to go from the origin
	// to the hit following the smaller path) otherwise it stays 'on the right'.

		if (cross>0.) {
			disq =	X[ihit]*X[ihit]+Y[ihit]*Y[ihit];
			nleft++;
		} else {
			nright++;
		}
	}	// end of   for(ihit=0, nleft=0, nright=0;....

	if( nright> nleft) {
		*Charge = -1;
	} else if ( nleft > nright) {
		*Charge = 1;
	} else {	// then choose according the closest hit ti the center
		if( minr < minl ) *Charge = -1;
		else  *Charge = 1;
	}



}

//----------end of function PndTrkCTFindTrackInXY::FindCharge


//----------begin of function PndTrkCTFindTrackInXY::FindTrackInXYProjection

bool PndTrkCTFindTrackInXY::FindTrackInXYProjection(
	struct FindTrackInXYProjection_InputData* in
	)
{

 // load the struct members in local variables;

 Double_t (*info)[7]=in->info;
 Double_t (*infoparalConformal)[5] = in->infoparalConformal  ;

 // make a POINTER to an ARRAY[][3] of
 // Double_t and assign value present in the calling sequence of this method;

 Double_t (*posizSciTil)[3]=(Double_t (*)[3])in->posizSciT;

//---------------

 UShort_t
	i,
	j,
	Naux,
	Nbaux,
	NN,
	Nouter,
	auxListHitsinTrack[in->maxstthits],
	OutputListHitsinTrack[in->maxstthits],
	OutputList2HitsinTrack[in->maxstthits],
	ListHitsinTrackinWhichToSearch[in->maxstthits];

 Short_t
	flagStt,
	status;


 Double_t
	aaa,
	m,
	q,
	rotationangle,
	rotationcos,
	rotationsin;



 PndTrkCTGeometryCalculations GeomCalculator;




//----------------
 *(in->nHitsinTrack) = FindTrackPatterninBoxConformal(
	in->FiConformalIndex,
	in->HitsinBoxConf,
	in->iHit, // seed hit; if it is negative it is a SciTil hit.
	in->InclusionListStt,
	info,
	in->ListHitsinTrack,
	in->ListSttParHits,
	in->maxstthitsintrack,
	in->maxstthits,
	in->minimumhitspertrack,
	in->nBoxConf,
	in->nfidivconformal,
	in->nFicell,
	2,   //  distance in Fi cells allowed
	in->nsttparhit,
	in->nRcell, // R cell of the seed hit;
	// can be negative beacuse of SciTil hits;
	1,   //  distance in R cells allowed
	in->RConformalIndex,
	in->nrdivconformal
			);



 if( *(in->nHitsinTrack) < in->minimumhitspertrack ||
	*(in->nHitsinTrack) > in->maxstthitsintrack) {
	return false;
 }




//-----------------------

//   find among the ListHitsinTrack  if there are at least
//   a minimum # of hits belonging to the outer part of the STT  system.
//   At this point of the code the Stt hits are already ordered from
//   the outermost to the innermost.

 for(j=0, Nouter =0; j<*(in->nHitsinTrack); j++){
  if(info[ (in->ListHitsinTrack)[j] ][0]*
	info[ (in->ListHitsinTrack)[j] ][0]+
	info[ (in->ListHitsinTrack)[j] ][1]*
	info[ (in->ListHitsinTrack)[j] ][1]
	< in->apotemamaxskewstraw*in->apotemamaxskewstraw  ) break;
  Nouter++;
 }




 if( Nouter >= in->minouterhitspertrack) {
	for(i=0; i< Nouter;i++){
	  ListHitsinTrackinWhichToSearch[i]=
		(in->ListHitsinTrack)[i];
	}

	for(i=0; i< Nouter;i++){
	 Naux =  FindTrackPatterninBoxConformalSpecial(
		in->FiConformalIndex,
		in->HitsinBoxConf,
		in->InclusionListStt,
		info,
		ListHitsinTrackinWhichToSearch[i], // seed hit.
		ListHitsinTrackinWhichToSearch,
		in->ListSttParHits,
		in->maxstthits,
		in->minimumhitspertrack,
		in->nBoxConf,
		1, // NFiCELLDISTANCE
		in->nfidivconformal,
		in->nsttparhit,
		Nouter,
		3, // NRCELLDISTANCE
		in->nrdivconformal,
		OutputListHitsinTrack,
		in->RConformalIndex
			);

	 if( Naux >= in->minouterhitspertrack && Naux > 0.7 * Nouter )  break;
		 if( Naux >= in->minouterhitspertrack) {

//   further collection of hits in the inner region but this time strictly connected
//   to the outer ones

//  first the list of non outer hits
	   for(j=Nouter;j<*(in->nHitsinTrack); j++){
		ListHitsinTrackinWhichToSearch[j-Nouter] =
			(in->ListHitsinTrack)[j];
	   }

	   Nbaux =  FindTrackStrictCollection(
		in->FiConformalIndex,
		in->InclusionListStt,
		ListHitsinTrackinWhichToSearch[i],   //  seed hit.
		ListHitsinTrackinWhichToSearch,
		in->maxstthits,
		1,    // NFiCELLDISTANCE
		in->nfidivconformal,
	//  n. of hits to search in ListHitsinTrackinWhichToSearch
		*(in->nHitsinTrack)-Nouter,
		OutputList2HitsinTrack
					);
//   add the new hits found to the list

	   *(in->nHitsinTrack)=Naux+Nbaux;
	   if( *(in->nHitsinTrack) >= in->minimumhitspertrack &&
		*(in->nHitsinTrack) <=in->maxstthitsintrack) {
		for(j=0;j<Naux;j++){
			(in->ListHitsinTrack)[j] =
				OutputListHitsinTrack[j];
		}
		for(j=0;j<Nbaux;j++){
			(in->ListHitsinTrack)[Naux+j] =
				OutputList2HitsinTrack[j];
		}
		break;

	   }// end of  if( *nHitsinTrack >= ....
	 }  // end of  if( Naux >= minouterhitspertrack)
	}   // end of for(i=0; i< Nouter;i++)
 }    // end of if( Nouter >= minouterhitspertrack)

 if( *(in->nHitsinTrack) < in->minimumhitspertrack ||
  *(in->nHitsinTrack)>in->maxstthitsintrack) {
	return false;
 }

//---------------------------

//  finding the rotation angle for best utilization of the MILP procedure

 for(j=0, rotationcos=0., rotationsin=0.; j<*(in->nHitsinTrack); j++){
	rotationcos += cos((0.5+
	(in->FiConformalIndex)[ (in->ListHitsinTrack)[j] ])
	*2.*PI/in->nfidivconformal) ;

	rotationsin += sin((0.5+
	in->FiConformalIndex[ (in->ListHitsinTrack)[j] ])
	*2.*PI/in->nfidivconformal) ;
 }
 rotationcos /=*(in->nHitsinTrack);
 rotationsin /=*(in->nHitsinTrack);
 rotationangle = atan2(rotationsin, rotationcos);
//  fitting with superfast MILP code
 bool Type;

 int	nFitPoints,
	offset;
 Double_t
	Xconformal[1+(*(in->nHitsinTrack))],
	Yconformal[1+(*(in->nHitsinTrack))],
	DriftRadiusconformal[1+(*(in->nHitsinTrack))],
	ErrorDriftRadiusconformal[1+(*(in->nHitsinTrack))];


 if(in->iHit<0){	// case with a hit in the SciTil
  aaa = in->posizSciTilx*in->posizSciTilx+in->posizSciTily*in->posizSciTily;
  Xconformal[0] =in->posizSciTilx/aaa;
  Yconformal[0] =in->posizSciTily/aaa;
  ErrorDriftRadiusconformal[0] = in->dimensionscitil/aaa;
  DriftRadiusconformal[0]=-1.;  // treat it like it is a Mvd hit.
 // +1 comes from one SciTil hit.
   offset=1;
   nFitPoints = *(in->nHitsinTrack) + 1;

 } else {	// no SciTil hit.
   offset=0;
   nFitPoints = *(in->nHitsinTrack);
 }  // end of  if(in->iHit<0)

  for(j=0; j<(*(in->nHitsinTrack)); j++){
    Xconformal[j+offset] =infoparalConformal[(in->ListHitsinTrack)[j]][0];
    Yconformal[j+offset] =infoparalConformal[(in->ListHitsinTrack)[j]][1];
    ErrorDriftRadiusconformal[j+offset]=
		infoparalConformal[(in->ListHitsinTrack)[j]][2];
    DriftRadiusconformal[j+offset]=
		infoparalConformal[(in->ListHitsinTrack)[j]][2];
  }


 PndTrkGlpkFits fit;
 status = fit.FitHelixCylinder(
		nFitPoints, // +1 comes from one SciTil hit.
		Xconformal,
		Yconformal,
		DriftRadiusconformal,
		ErrorDriftRadiusconformal,
		rotationangle,  //  rotationangle, da mettere
		in->trajectory_vertex,	//  vertex in (X,Y) of this trajectory
		in->maxhitsinfit,  //  maximum n. of hits allowed in fast fit
		&m,
		&q,
		in->ALFA,
		in->BETA,
		in->GAMMA,
		in->TypeConf,
		0, // istampa
		0 //  IVOLTE
			);


cout<<"cazzo,crasha in FindTrackInXYProjection [dopo]?\n";

 if(status < 0  ) return false;

//  this trasformation is valid even if the equation is a straight line from the fit

 *(in->Oxx)= -0.5*(*(in->ALFA));
 *(in->Oyy)= -0.5*(*(in->BETA));
 *(in->Rr)= (*(in->Oxx)) * (*(in->Oxx)) + (*(in->Oyy)) * (*(in->Oyy))-(*(in->GAMMA));

	// some obvious preliminary cuts
 if( *(in->Rr) < 0. )  return false;
 *(in->Rr)= sqrt( *(in->Rr) );
 aaa = sqrt( (*(in->Oxx)) * (*(in->Oxx)) + (*(in->Oyy)) * (*(in->Oyy))  );

 // the following is because the circumference is supposed to come from (0,0);
 //   here the factor 0.9 is used in order to be conservative.
 if(aaa< 0.9*in->rstrawdetectormin/2.) return false;

//   here the factor 0.9 is used in order to be conservative.
 if ( *(in->Rr) + aaa < in->rstrawdetectormin *0.9 ) return false;

//---------------------------

//  check again if the SciTil hit (if present) is compatible with this circle trajectory
//  in  XY by finding if it has intersection (in the XY plane) with Helix circle

//  equation of the SciTil segment :  y0 * y + x0 * x - x0**2 - y0**2 = 0
//  where  (x0,y0) = position of center of the SciTil.

//  delimiting points of the SciTil segment :  define L = length of the SciTil, 
//  and RR = sqrt(x0**2+y0**2), SIGN = the sign of (-x0*y0) or SIGN=1 when y0=0,
//  SIGN=irrelevant when x0=0;   then :
//  P1 =  [ x0- abs{(L/2)*y0/RR}; y0-SIGN*abs{(L/2)*x0/RR} ],
//  P2 =  [ x0+abs{(L/2)*y0/RR}; y0+SIGN*abs{(L/2)*x0/RR} ].

 bool intersect;
 UShort_t
	Nint;
//	nSciT;
 Double_t distance,
	QQ,
	sqrtRR,
	SIGN,
	XintersectionList[2],
	YintersectionList[2];


 // whether or not the seed hit was a Stt hit try if any SciTil hits are
 // associated to this track cand.

 if(in->YesSciTil){
	//   *(in->nSciTilHitsinTrack)    is the n. of SciTil hit associated to this track.
	*(in->nSciTilHitsinTrack)= AssociateSciTilHit(
	in->dimensionscitil,
	in->S_SciTilHitsinTrack,// output; S on the lateral face of the Helix
	 // of the SciTil hit (if present).
	in->InclusionListSciTil,
	in->ListSciTilHitsinTrack,
	in->maxscitilhitsintrack,
	in->nSciTilHits,
	*(in->Oxx),
	*(in->Oyy),
	posizSciTil,
	*(in->Rr)
				);



	if( *(in->nSciTilHitsinTrack) >0 ){
		for(j=0;j<*(in->nSciTilHitsinTrack);j++){
			(in->InclusionListSciTil)[(in->ListSciTilHitsinTrack)[j]]
				=false;
		}
	}

 }else{
	*(in->nSciTilHitsinTrack)=0;
 } // end of if(YesSciTil)


//---------------------  better association of the hits in the track candidate
// treat differently the case in which the track has radius < rstrawdetectormax/2
// and the other case.

 if( *(in->Rr) < in->rstrawdetectormax/2){
	GeomCalculator.FindingParallelTrackAngularRange(
		*(in->Oxx),
		*(in->Oyy),
		*(in->Rr),
		1,  // this is supposed to be the charge, irrelevant here if it is +1 or -1.
		in->Fi_low_limit,
		in->Fi_up_limit,
		&flagStt,
		in->rstrawdetectormin,
		in->rstrawdetectormax
		);

	NN =  TrkAssociatedParallelHitsToHelix5(
		auxListHitsinTrack,              //  this is the output
		in->InclusionListStt,
		*(in->Fi_low_limit),
		*(in->Fi_up_limit),
		info,
		in->ListSttParHits,
		in->nsttparhit,
		*(in->Oxx),
		*(in->Oyy),
		*(in->Rr),
		in->strawradius
				);


 }  else {

	NN =  TrkAssociatedParallelHitsToHelixQuater(
		auxListHitsinTrack,	//  this is the output
		in->deltanr,
		in->FiConformalIndex,
		in->HitsinBoxConf,
		in->InclusionListStt,
		info,
		infoparalConformal,
		in->ListHitsinTrack,
		m,
		in->maxstthits,
		in->nBoxConf,
		in->nfidivconformal,
		*(in->nHitsinTrack),
		in->nsttparhit,
		in->nrdivconformal,
		*(in->Oxx),
		*(in->Oyy),
		q,
		in->radiaConf,
		in->RConformalIndex,
		*(in->Rr),
		in->rstrawdetectormin,
		status,
		in->strawradius
			);

 } // end of  if( Rr < rstrawdetectormax/2)

 if( NN < in->minimumhitspertrack || NN>in->maxstthitsintrack) return false;

 *(in->nHitsinTrack)=NN;

 for(i=0; i< *(in->nHitsinTrack);i++){
	(in->ListHitsinTrack)[i]=auxListHitsinTrack[i];
 }



//----------------------------- Finding the Charge
	// The charge is calculated first by dividing the hits in two categories by using the Perpendicular
	// to the tangent to the trajectory in (0,0). Then simply the majority of the hits decides the
	// sign of the  charge. See Gianluigi's Logbook page 290.

	Double_t
		X[*(in->nHitsinTrack)],
		Y[*(in->nHitsinTrack)];

	for(i=0;i<*(in->nHitsinTrack);i++){
		X[i]=info[(in->ListHitsinTrack)[i]][0];
		Y[i]=info[(in->ListHitsinTrack)[i]][1];
	}
	FindCharge(
		*(in->Oxx),
		*(in->Oyy),
		*(in->nHitsinTrack),
		X,
		Y,
		in->Charge
		);


//----------------------------- Ordering.
	// The ordering reflects the angle Fi in the Helix reference frame because
	// the hits are ordered by going around the trajectory cclockwise for
	// positive tracks or counterclockwise for negative particles; in other words
	//  it is not used simply the distance of the hit from (0,0) as ordering parameter
	//  but the track length of the circle.
	// this method finds also Fi_initial_helix_referenceframe and Fi_final_helix_referenceframe.
	// The former is simply the fi angle of the point (0,0) with respect to the center
	// of this Helix. Important : this angle has to be > 0 always and it is between 0. and
	// 2 PI here (later,  FixDiscontinuitiesFiangleinSZplane may change it adding +2PI or
	// -2PI if necessary).
	// Fi_final_helix_referenceframe is made such that  it is < Fi_initial_helix_referenceframe
	// when the track is  positive, and it is > Fi_initial_helix_referenceframe for negative
	// tracks.

 OrderingParallel(
		*(in->Charge),
		in->Fi_initial_helix_referenceframe,//output
		in->Fi_final_helix_referenceframe,// output
		info,
		in->ListHitsinTrack,
		*(in->nHitsinTrack),
		*(in->Oxx),
		*(in->Oyy),
		in->U,
		in->V
			);

//   finding the FI angular range (in the laboratory frame) spanned by this parallel track

 GeomCalculator.FindingParallelTrackAngularRange(
		*(in->Oxx),
		*(in->Oyy),
		*(in->Rr),
		*(in->Charge),
		in->Fi_low_limit,
		in->Fi_up_limit,
		&flagStt,
		in->rstrawdetectormin,
		in->rstrawdetectormax
					);


	if( flagStt == -2 ) return false;	// track outside cylinder rstrawdetectormax.
	if( flagStt == -1 ) return false;	// track inside cylinder rstrawdetectormin.
	if( flagStt ==  1 ) return false;	// track comprised in cylinder : discard,
					// because at this stage only tracks from
					// vertex are searched.





	return true;
};

//----------end of function PndTrkCTFindTrackInXY::FindTrackInXYProjection



//----------begin of function PndTrkCTFindTrackInXY::FindTrackPatterninBoxConformal

Short_t PndTrkCTFindTrackInXY::FindTrackPatterninBoxConformal(
	UShort_t *FiConformalIndex,
	void* HitsinBoxConf,
	Short_t ihit, // seed hit;
	bool *InclusionListStt,
	Double_t info[][7],
	UShort_t* ListHitsinTrack,
	UShort_t* ListSttParHits,
	UShort_t maxstthitsintrack,
	UShort_t maxstthits,
	UShort_t minimumhitspertrack,
	void* nBoxConf,
	UShort_t nfidivconformal,
	UShort_t nFicell, // Fi cell of the seed hit;
	UShort_t NFiCELLDISTANCE,
	UShort_t Nparal,
	Short_t nRcell, // R cell of the seed hit;
	// can be negative beacuse of SciTil hits;
	UShort_t NRCELLDISTANCE,
	UShort_t *RConformalIndex,
	UShort_t nrdivconformal
			)
{

 bool
	status,
	TemporaryInclusionList[maxstthits];

 Short_t	i;
 UShort_t	j,
		iFi,
		iR,
		nRmin,
		nRmax,
		nRemainingHits,
		nHitsinTrack,
		auxIndex[maxstthits],
		Remaining[maxstthits];

 Short_t iFi2;

 Double_t auxRvalues[maxstthits];


 // make a POINTER to an ARRAY[nfidivconformal] of UShort_t and assign value present in the calling
 // sequence of this method;
 UShort_t (*nBoxConformal)[nfidivconformal] = (UShort_t (*)[nfidivconformal]) nBoxConf;

 // make a POINTER to an ARRAY[nrdivconformal][nfidivconformal] of UShort_t and assign value present
 // in the calling sequence of this method;
 UShort_t (*HitsinBoxConformal)[nrdivconformal][nfidivconformal]=
		(UShort_t (*)[nrdivconformal][nfidivconformal])HitsinBoxConf;



//   ihit        is the hit number in the PARALLEL number scheme


 for(i=0, nRemainingHits=0; i<Nparal; i++){

	if( i != ihit && InclusionListStt[  ListSttParHits[i]   ] ) {   //  Inclusion of the
				//  parallel hit straws already used in other tracks
				//  remember the index of InclusionListStt is in the
				//  ORIGINAL scheme of hits
		TemporaryInclusionList[ ListSttParHits[i]  ]= true;
		Remaining[nRemainingHits]= i;   //  index of the PARALLEL hit
		nRemainingHits++;
	} else {
		TemporaryInclusionList[ ListSttParHits[i]  ]= false;
	}
 }

 if( nRemainingHits < minimumhitspertrack )    return 0;


//  cells of the seed hit

 if(ihit>=0){
	nHitsinTrack=1;
	ListHitsinTrack[0]=  ihit ;
	i = 0;
 } else {
	nHitsinTrack=0;
	i = -1;
 }

 status=true;

 while( nRemainingHits > 0 &&  i < nHitsinTrack && status) {


	if (nRcell - NRCELLDISTANCE < 0 ) {
		nRmin = 0;
	}  else {
		nRmin = nRcell - NRCELLDISTANCE;
	}
	if (nRcell + NRCELLDISTANCE >= nrdivconformal ) {
		nRmax = nrdivconformal-1;
	}  else {
		nRmax = nRcell + NRCELLDISTANCE;
	}
    for( iR= nRmin ; iR<= nRmax && status ; iR++){
      for(iFi2=nFicell-NFiCELLDISTANCE;iFi2<=nFicell+NFiCELLDISTANCE && status;iFi2++){
		if ( iFi2 < 0 )  {
			iFi = nfidivconformal + iFi2;
		} else if ( iFi2 >= nfidivconformal) {
			iFi = iFi2  - nfidivconformal;
		}  else {
			iFi = iFi2;
		}
	 for (j = 0; j< nBoxConformal[iR][iFi]; j++){
	    if( InclusionListStt[ HitsinBoxConformal[j][iR][iFi] ]
					&&
		   TemporaryInclusionList[HitsinBoxConformal[j][iR][iFi]]){
			// hit number in the PARALLEL straws scheme
			ListHitsinTrack[nHitsinTrack]=HitsinBoxConformal[j][iR][iFi] ;
			nHitsinTrack++;
		if( nHitsinTrack >= maxstthitsintrack){
		 // finish the search
		 status=false; // finish all outer loops as well.
		 break ;
		}
		TemporaryInclusionList[HitsinBoxConformal[j][iR][iFi]]= false;
		nRemainingHits--;
	    } // end of if( InclusionListStt[  ListSttParHits[...]]
         }// end of  for (j = 0; j< nBoxConformal[iR][iFi]; j++)
      }  // end of  for( iFi2 = nFicell - NFiCELLDISTANCE ;
    }	// end of  for( iR= nRmin ; iR<= nRmax ; iR++)
//----------------
    i++;
    if(i<maxstthitsintrack){
	nRcell = RConformalIndex[ListHitsinTrack[i]];
	nFicell = FiConformalIndex[ListHitsinTrack[i]];
    }
   }    //  end      while ( nRemainingHits > 0 && i < nHitsinTrack)



    return nHitsinTrack;

}


//----------end of function PndTrkCTFindTrackInXY::FindTrackPatterninBoxConformal



//----------begin of function PndTrkCTFindTrackInXY::FindTrackPatterninBoxConformalSpecial

//  The difference with FindTrackPatterninBoxConformal is that the search is performed
//  in the selected list    ListHitsinTrackinWhichToSearch
//  instead of all the parallel list.

Short_t PndTrkCTFindTrackInXY::FindTrackPatterninBoxConformalSpecial(
	UShort_t *FiConformalIndex,
	void* HitsinBoxConf,
	bool *InclusionListStt,
	Double_t info[][7],
	UShort_t iSeed,
	UShort_t *ListHitsinTrackinWhichToSearch,
	UShort_t* ListSttParHits,
	UShort_t maxstthits,
	UShort_t minimumhitspertrack,
	void* nBoxConf,
	UShort_t NFiCELLDISTANCE,
	UShort_t nfidivconformal,
	UShort_t Nparal,
	UShort_t NparallelToSearch,
	UShort_t NRCELLDISTANCE,
	UShort_t nrdivconformal,
	UShort_t *OutputListHitsinTrack,
	UShort_t *RConformalIndex
					)
{


 bool TemporaryInclusionList[maxstthits];

 UShort_t	i,
		i2,
		j,
		iFi,
		iR,
		nFicell,
		nHitsinTrack,
		nRcell,
		nRemainingHits,
		nRmax,
		nRmin,
		auxIndex[maxstthits],
		Remaining[maxstthits];

 Short_t iFi2;

 Double_t auxRvalues[maxstthits];


 // make a POINTER to an ARRAY[nfidivconformal] of UShort_t and assign value present in the calling
 // sequence of this method;
 UShort_t (*nBoxConformal)[nfidivconformal] = (UShort_t (*)[nfidivconformal]) nBoxConf;

 // make a POINTER to an ARRAY[nrdivconformal][nfidivconformal] of UShort_t and assign value present
 // in the calling sequence of this method;
 UShort_t (*HitsinBoxConformal)[nrdivconformal][nfidivconformal]=
		(UShort_t (*)[nrdivconformal][nfidivconformal])HitsinBoxConf;


//   iSeed        is the hit number in the ORIGINAL number scheme

//--------    the following initialization is essential for the algorithm to work
 for(i=0; i<Nparal; i++){
	TemporaryInclusionList[ ListSttParHits[i]  ]= false;
 }
//-------------

 for(i2=0, nRemainingHits=0; i2<NparallelToSearch; i2++){
	i=ListHitsinTrackinWhichToSearch[i2];
//  Inclusion of the parallel hit straws already used in other tracks;
//  remember the index of InclusionListStt is in the ORIGINAL scheme of hits.
	if( i != iSeed && InclusionListStt[ListSttParHits[i]] ) {
		TemporaryInclusionList[ListSttParHits[i]]= true;
		Remaining[nRemainingHits]= i;   //  index of the PARALLEL hit
		nRemainingHits++;
	}
 }


 if( nRemainingHits < minimumhitspertrack )    return 0;

//  cells of the seed hit

 nHitsinTrack=1;
 OutputListHitsinTrack[0]=  iSeed ;
 i = 0;
 while( nRemainingHits > 0 &&  i < nHitsinTrack) {

	nRcell = RConformalIndex[OutputListHitsinTrack[i]];
		nFicell = FiConformalIndex[OutputListHitsinTrack[i]];

//---------------

	if (nRcell - NRCELLDISTANCE < 0 ) {
		nRmin = 0;
	}  else {
		nRmin = nRcell - NRCELLDISTANCE;
	}
	if (nRcell + NRCELLDISTANCE >= nrdivconformal ) {
		nRmax = nrdivconformal-1;
	}  else {
		nRmax = nRcell + NRCELLDISTANCE;
	}

	for( iR= nRmin ; iR<= nRmax ; iR++){
	  for( iFi2=nFicell-NFiCELLDISTANCE ;
		iFi2<=nFicell+NFiCELLDISTANCE;iFi2++){
		if ( iFi2 < 0 )  {
			iFi = nfidivconformal + iFi2;
		} else if ( iFi2 >= nfidivconformal) {
			iFi = iFi2  - nfidivconformal;
		} else {
			iFi = iFi2;
		}
	     for (j = 0; j< nBoxConformal[iR][iFi]; j++){
		if(InclusionListStt[HitsinBoxConformal[j][iR][iFi]]
				&&
			TemporaryInclusionList[HitsinBoxConformal[j][iR][iFi]]) {
//  hit number in the ORIGINAL straws scheme
		  OutputListHitsinTrack[nHitsinTrack]=HitsinBoxConformal[j][iR][iFi] ;
		  nHitsinTrack++;
		  TemporaryInclusionList[HitsinBoxConformal[j][iR][iFi]]
			= false;
		  nRemainingHits--;
		}  // end of    if(InclusionListStt[ .. ]
	     } // end of for (j = 0; j< nBoxConformal[iR][iFi]; j++)
	  }  // end of    for( iFi2=nFice
	}  // end of  for( iR= nRmin
//----------------
	i++;

 }    //  end while ( nRemainingHits > 0 && i < nHitsinTrack)


 return nHitsinTrack;

}


//----------end of function PndTrkCTFindTrackInXY::FindTrackPatterninBoxConformalSpecial


//----------begin of function PndTrkCTFindTrackInXY::FindTrackStrictCollection

Short_t PndTrkCTFindTrackInXY::FindTrackStrictCollection(
	UShort_t *FiConformalIndex,
	bool *InclusionListStt,
	//  seed track (original notation) as far as the Fi angle is concerned
	UShort_t iSeed,
	UShort_t *ListHitsinTrackinWhichToSearch,
	UShort_t maxstthits,
	UShort_t NFiCELLDISTANCE,
	UShort_t nfidivconformal,
	//  n. of hits to search in ListHitsinTrackinWhichToSearch
	UShort_t NParallelToSearch,
	UShort_t  *OutputListHitsinTrack
	)
{

 UShort_t
	i,
	iFi,
	iFiseed,
	iR,
	j,
	nHitsinTrack;


 Double_t auxRvalues[maxstthits];

//   iSeed        is the hit number in the ORIGINAL number scheme

 iFiseed = FiConformalIndex[iSeed];

 nHitsinTrack=0;
 for(i=0; i<NParallelToSearch; i++){
	//  Inclusion of the parallel hit straws already used in other tracks
     if( InclusionListStt[ListHitsinTrackinWhichToSearch[i]]) {
	//  the index of InclusionListStt is in the ORIGINAL scheme of hits

	iFi = FiConformalIndex[ListHitsinTrackinWhichToSearch[i]];
	if( iFi == iFiseed ) {
	   OutputListHitsinTrack[nHitsinTrack]=ListHitsinTrackinWhichToSearch[i];
	   nHitsinTrack++;
	} else if ( iFi < iFiseed ) {
	   if(iFiseed - iFi <= NFiCELLDISTANCE ){
		OutputListHitsinTrack[nHitsinTrack]=ListHitsinTrackinWhichToSearch[i];
		nHitsinTrack++;
	   } else {
		if(iFi+nfidivconformal-iFiseed<=NFiCELLDISTANCE) {
		  OutputListHitsinTrack[nHitsinTrack]=ListHitsinTrackinWhichToSearch[i];
		  nHitsinTrack++;
		}
	   } // end of  if(iFiseed - iFi <= NFiCELLDISTANCE)
	}else{   //  continuation of  if( iFi == iFiseed )
	   if( -iFiseed + iFi <= NFiCELLDISTANCE ) {
		OutputListHitsinTrack[nHitsinTrack]=ListHitsinTrackinWhichToSearch[i];
		nHitsinTrack++;
	   }else{
		if( -iFi + nfidivconformal + iFiseed<= NFiCELLDISTANCE ) {
		  OutputListHitsinTrack[nHitsinTrack]=ListHitsinTrackinWhichToSearch[i];
		  nHitsinTrack++;
		}
	   }  // end of  if( -iFiseed + iFi <= NFiCELLDISTANCE )

	}  //  end of   if( iFi == iFiseed )

        }    //  end of    if( InclusionListStt[ListHitsinTrackinWhichToSearch[i]])

     }   //  end of        for(i=0; i<NparallelToSearch; i++)



 return nHitsinTrack;

}


//----------end of function PndTrkCTFindTrackInXY::FindTrackStrictCollection

//----------begin of function PndTrkCTFindTrackInXY::OrderingParallel

void PndTrkCTFindTrackInXY::OrderingParallel(
	Short_t  Charge,
	Double_t *Fi_final_helix_referenceframe,
	Double_t *Fi_initial_helix_referenceframe,
	Double_t info[][7],
	UShort_t *ListParallelHits,
	UShort_t nParallelHits,
	Double_t oX,
	Double_t oY,
	Double_t *U,
	Double_t *V
	)
{

 UShort_t
	i,
	j,
	tmp[nParallelHits];

 Int_t	tempo[nParallelHits];

      Double_t	aaa,
		b1,
		firstR2,
		lastR2,
		aux[nParallelHits];

 PndTrkMergeSort MergeSort;



//  here there is the ordering of the hits, under the assumption that the circumference
//  in XY goes through (0,0).
//  Moreover, the code before is supposed to have selected trajectories in XY with (Ox,Oy)
//  farther from (0,0) by > 0.9 * RminStrawDetector/2 and consequently Ox and Oy are not both 0.
//  The scheme for the ordering of the hit is as follows :
//  1)  order hits by increasing U or V of the conformal mapping; see Gianluigi's Logbook page 283;
//  2)  find the charge of the track by checking if it is closest to the center in XY
//	the first or the last of the ordered hits.
//  3)  in case, invert the ordering of U, V and ListParallelHits such that the first hits in the
//	list are alway those closer to the (0,0).


//   ordering of the hits

	aaa = atan2( oY, oX);  // atan2 defined between -PI and PI.

	// the following statement is necessary since for unknown reason the root interpreter
	// gives a weird error when using PI directly in the if statement below!!!!!!! I lost
	// 2 hours trying to figure this out!
	b1 = PI/4.;

	if((b1<aaa && aaa<3.*b1) || (aaa>-3.*b1&&aaa<-b1)){//use U as ordering variable;
							//[case 1 or 3 Gianluigi's Logbook page 285].
		for (j = 0; j< nParallelHits; j++){
			U[j]=info[ ListParallelHits[j] ][0]/(
			info[ ListParallelHits[j] ][0]*
			info[ ListParallelHits[j] ][0]+
			info[ ListParallelHits[j] ][1]*
			info[ ListParallelHits[j] ][1]);
		}
		for (j = 0; j< nParallelHits; j++){
			tempo[j]= (Int_t) ListParallelHits[j];
		}
		MergeSort.Merge_Sort( nParallelHits, U, tempo);
		for (j = 0; j< nParallelHits; j++){
			ListParallelHits[j] = (UShort_t) tempo[j];
		}

		if((aaa>b1&&aaa<3.*b1)){  //  case #1;
			if( Charge == -1){
				// inverting the order of the hits.
				for(i=0;i<nParallelHits;i++){
					tmp[i]=ListParallelHits[nParallelHits-1-i];
					aux[i] = U[nParallelHits-1-i];
				}
				for(i=0;i<nParallelHits;i++){
					ListParallelHits[i]=tmp[i];
					U[i] = aux[i];
				}
			}
			for (j = 0; j< nParallelHits; j++){
				V[j]=info[ ListParallelHits[j] ][1]/(
				info[ ListParallelHits[j] ][0]*
				info[ ListParallelHits[j] ][0]+
				info[ ListParallelHits[j] ][1]*
				info[ ListParallelHits[j] ][1]);
			}
		} else{  //  case # 3.
			if(Charge == 1){
				// inverting the order of the hits.
				for(i=0;i<nParallelHits;i++){
					tmp[i]=ListParallelHits[nParallelHits-1-i];
					aux[i] = U[nParallelHits-1-i];
				}
				for(i=0;i<nParallelHits;i++){
					ListParallelHits[i]=tmp[i];
					U[i] = aux[i];
				}
			}// end of  if( Charge ==1)
			for (j = 0; j< nParallelHits; j++){
				V[j]=info[ ListParallelHits[j] ][1]/(
				info[ ListParallelHits[j] ][0]*
				info[ ListParallelHits[j] ][0]+
				info[ ListParallelHits[j] ][1]*
				info[ ListParallelHits[j] ][1]);
			}
		}// end of  if((aaa>b1&&aaa<3.*b1))

	} else { // use V as ordering variable [case 2 or 4 Gianluigi's Logbook page 285].
		for (j = 0; j< nParallelHits; j++){
			V[j]=info[ ListParallelHits[j] ][1]/(
			info[ ListParallelHits[j] ][0]*
			info[ ListParallelHits[j] ][0]+
			info[ ListParallelHits[j] ][1]*
			info[ ListParallelHits[j] ][1]);
		}
		for (j = 0; j< nParallelHits; j++){
			tempo[j]= (Int_t) ListParallelHits[j];
		}
		MergeSort.Merge_Sort( nParallelHits, V, tempo);
		for (j = 0; j< nParallelHits; j++){
			ListParallelHits[j] = (UShort_t) tempo[j];
		}

		if((aaa<=-3.*b1 || aaa>=3.*b1)){  //  case #2;
			if( Charge == -1){
				// inverting the order of the hits.
				for(i=0;i<nParallelHits;i++){
					tmp[i]=ListParallelHits[nParallelHits-1-i];
					aux[i] = V[nParallelHits-1-i];
				}
				for(i=0;i<nParallelHits;i++){
					ListParallelHits[i]=tmp[i];
					V[i] = aux[i];
				}
			}
			for (j = 0; j< nParallelHits; j++){
				U[j]=info[ ListParallelHits[j] ][0]/(
				info[ ListParallelHits[j] ][0]*
				info[ ListParallelHits[j] ][0]+
				info[ ListParallelHits[j] ][1]*
				info[ ListParallelHits[j] ][1]);
			}
		} else{  //  case # 4.
			if( Charge == 1){
				// inverting the order of the hits.
				for(i=0;i<nParallelHits;i++){
					tmp[i]=ListParallelHits[nParallelHits-1-i];
					aux[i] = V[nParallelHits-1-i];
				}
				for(i=0;i<nParallelHits;i++){
					ListParallelHits[i]=tmp[i];
					V[i] = aux[i];
				}
			}
			for (j = 0; j< nParallelHits; j++){
				U[j]=info[ ListParallelHits[j] ][0]/(
				info[ ListParallelHits[j] ][0]*
				info[ ListParallelHits[j] ][0]+
				info[ ListParallelHits[j] ][1]*
				info[ ListParallelHits[j] ][1]);
			}
		}

	} //  end of   if((aaa>b1&& ....






//  FI initial value (at 0,0  vertex) in the Helix reference frame

	*Fi_initial_helix_referenceframe = atan2(-oY,-oX) ;//  this is in order to be coherent
			//  with the calculatation of Fi, which is atan2(oY,oX). 
			//  atan2  is defined in [-PI,PI)
	if ( *Fi_initial_helix_referenceframe <0.)
			*Fi_initial_helix_referenceframe += 2.*PI;

//  FI of the last parallel hit in the Helix reference frame

	*Fi_final_helix_referenceframe = atan2(
		info[ ListParallelHits[nParallelHits-1] ][1]-oY,
		info[ ListParallelHits[nParallelHits-1] ][0]-oX
						);
	if ( *Fi_final_helix_referenceframe <0.)
			*Fi_final_helix_referenceframe += 2.*PI;

	if ( Charge > 0 ) {
		if( *Fi_final_helix_referenceframe> *Fi_initial_helix_referenceframe)
			*Fi_final_helix_referenceframe -= 2.*PI;
		if( *Fi_final_helix_referenceframe> *Fi_initial_helix_referenceframe)
			*Fi_final_helix_referenceframe = *Fi_initial_helix_referenceframe;
	} else {
		if( *Fi_final_helix_referenceframe< *Fi_initial_helix_referenceframe)
			*Fi_final_helix_referenceframe += 2.*PI;
		if( *Fi_final_helix_referenceframe< *Fi_initial_helix_referenceframe)
			*Fi_final_helix_referenceframe = *Fi_initial_helix_referenceframe;
	}




 return;


}
//----------end of function PndTrkCTFindTrackInXY::OrderingParallel



//----------begin of function PndTrkTrackFinderReal::TrkAssociatedParallelHitsToHelixQuater

UShort_t PndTrkCTFindTrackInXY::TrkAssociatedParallelHitsToHelixQuater(
	UShort_t *auxListHitsinTrack,
	Double_t deltanr,
	UShort_t *FiConformalIndex,
	void* HitsinBoxConf,
	bool* InclusionListStt,
	Double_t info[][7],
	Double_t infoparalConformal[][5],
	UShort_t *ListHitsinTrack,
	Double_t m,
	UShort_t maxstthits,
	void* nBoxConf,
	UShort_t nfidivconformal,
	UShort_t nHitsinTrack,
	UInt_t NhitsParallel,
	UShort_t nrdivconformal,
	Double_t Oxx,
	Double_t Oyy,
	Double_t q,
	Double_t* radiaConf,
	UShort_t *RConformalIndex,
	Double_t Rr,
	Double_t rstrawdetectormin,
	Short_t Status,
	Double_t STRAWRADIUS
		)
{


 bool
	passamin,
	passamax,
	Unselected[maxstthits];  // this has to be at least nSttHits large.
cout<<"cazzocontrollo1 , maxstthits  "<<maxstthits<<endl;
  Short_t i, i2, j, k, l,  l2, l3, itemp, kstart, kend,
          iFi0,FFimin, FFimax;
  UShort_t Nextra=8,
           nFi,
           Fi,
           nR,
           nAssociatedHits,
           nHit_original;
  Double_t maxFi,
           minFi,
           dist,
           xx,
           yy,
           aaa,
           angle,
           r,
           erre1,
           erre2,
           Rin,
           Rout,
           Fi0,
           ddd,
           fi1,
           fi2,
           dx,
           dy,
           distance,
           NTIMES=1.5;   //   number of Straw radia allowed in association





 // make a POINTER to an ARRAY[nfidivconformal] of UShort_t and assign value present in the calling
 // sequence of this method;
 UShort_t (*nBoxConformal)[nfidivconformal] = (UShort_t (*)[nfidivconformal]) nBoxConf;

 // make a POINTER to an ARRAY[nrdivconformal][nfidivconformal] of UShort_t and assign value present
 // in the calling sequence of this method;
 UShort_t (*HitsinBoxConformal)[nrdivconformal][nfidivconformal]=
		(UShort_t (*)[nrdivconformal][nfidivconformal])HitsinBoxConf;
/*
for(int jcz=0;jcz<nfidivconformal;jcz++){
	for(int icz=0;icz<nrdivconformal;icz++){
	cout<<"cazzocontrollo, nBoxConformal["<<icz<<"]["<<jcz<<"] "<<
		nBoxConformal[icz][jcz]<<endl;

		if( nBoxConformal[icz][jcz]>0){
		 for(int kcz=0;kcz<nBoxConformal[icz][jcz];kcz++){
			cout<<"\tHitsinBoxConformal["<<
			kcz<<"]["<<icz<<"]["<<jcz<<"]  = "
			<<HitsinBoxConformal[kcz][icz][jcz]<<endl;
		 }
		}
	}
}
*/

 nAssociatedHits=0;


 size_t len = sizeof(Unselected);
 memset (Unselected,true,len);


//   find the range in Fi spanned  by the candidate track

  FFimin = 10000;
  FFimax = 0;
cout<<"cazzocontrollo1 , nHitsinTrack  "<<nHitsinTrack<<
", nfidivconformal "<<nfidivconformal
<<", nrdivconformal "<<nrdivconformal<<endl;

  for(j=0; j<nHitsinTrack; j++){

cout<<"cazzocontrollo1 , j  "<<j<<", ListHitsinTrack[j] "<<
ListHitsinTrack[j]<<", infoparalConformal "<<
infoparalConformal[ ListHitsinTrack[j] ][3]
<<endl;

    i = (UShort_t)  infoparalConformal[ ListHitsinTrack[j] ][3];

cout<<"cazzocontrollo1 , i  "<<i<<", FiConformalIndex[i] "<<
FiConformalIndex[i]<<endl;

    if( FiConformalIndex[i] <  FFimin ) FFimin = FiConformalIndex[i];
    if( FiConformalIndex[i] >  FFimax ) FFimax = FiConformalIndex[i];
  }


  if( FFimax > 3.*nfidivconformal/4. && FFimin < nfidivconformal/4.) {
     FFimin = 10000;
     FFimax =  0;
     for(j=0; j<nHitsinTrack; j++){
       i = (UShort_t)  infoparalConformal[ ListHitsinTrack[j] ][3];
       Fi = FiConformalIndex[i];
       if( Fi < nfidivconformal/4. ) Fi = FiConformalIndex[i]+nfidivconformal;
       if( Fi <  FFimin ) FFimin = Fi;
       if( Fi >  FFimax ) FFimax = Fi;
     }
  }


//  finding the boundaries in the Conformal plane. The basic assumption is that the range
// in Fi is much less that 180 degrees.
  
  FFimin -= (Short_t) nfidivconformal/Nextra;
  FFimax +=  (Short_t) nfidivconformal/Nextra;
if( FFimax - FFimin > nfidivconformal/2 ) {
    cout<<"something fishy is going on in TrkAssociatedParallelHitsToHelixQuater!"
      <<"Range in Fi (rad) is  "<<(FFimax - FFimin)*2.*PI/nfidivconformal<<endl;
    return 0;
}

cout<<"cazzocontrollo1 , FFimin  "<<FFimin<<", FFimax "<<FFimax<<
", Status "<<Status<<", q "<<q<<endl;


//   use the equation of a line in polar coordinates
 
  if( Status ==99) {   //  case in which   0 = x + q

    if(fabs(q) > 1.e-10 ) {
      passamax=false;
      passamin=false;
      for(itemp=FFimin; itemp<=FFimax;itemp++){
        i=itemp;
        if( i< 0 ) {
          i += nfidivconformal;
        } else if (i>=nfidivconformal){
          i -=  nfidivconformal*( i/nfidivconformal );
        }
        angle = (i+0.5)*2.*PI/nfidivconformal;
        aaa = cos(angle);
        if( fabs(cos(angle)) <1.e-10)  continue;
        r = -q/aaa;
        if(r< radiaConf[0] || r>= 1./rstrawdetectormin)  continue;
        for(j=nrdivconformal-1; j>=0;j--){
          if( r>= radiaConf[j] ){
           nR = j;
           break;
          }
        }


        for(l=(UShort_t) (-deltanr) ; l<(UShort_t)deltanr+1;l++){
          l2 = nR+l;
          if(  l2<0 || l2 >= nrdivconformal )  continue;
              for( k=0;k<nBoxConformal[l2][i];k++){
                nHit_original = (UShort_t) infoparalConformal[HitsinBoxConformal[k][l2][i]][3];
		if( !InclusionListStt[ nHit_original ] ) continue;
// check if the hit position is near the circle of the Helix found by the fit
                dx = -Oxx+info[ nHit_original ][0];
                dy = -Oyy+info[ nHit_original ][1];
                distance = sqrt(dx*dx+dy*dy);
                if ( fabs(Rr - distance ) > NTIMES*STRAWRADIUS )  continue;

//-------------------
                xx=infoparalConformal[  HitsinBoxConformal[k][l2][i]  ][0];
                dist = fabs( xx +q );
                if(  AcceptHitsConformal(  dist,
			infoparalConformal[HitsinBoxConformal[k][l2][i]][2],
			infoparalConformal[  HitsinBoxConformal[k][l2][i]  ][4]
				) )  {
                    auxListHitsinTrack[nAssociatedHits]= HitsinBoxConformal[k][l2][i];
                    nAssociatedHits++; 
                }
              }	//  end of  for( k=0;k<nBoxConformal[l2][i];k++)
        }   //   end of for(l=(UShort_t)(-deltanr);l<(UShort_t)deltanr+1;l++)


        //------- special cases
           if((nR == nrdivconformal-1 && passamin && ! passamax) ||  (nR==0 && passamax && !passamin)  ) {   //  do the last two Fi columns
            if(nR == nrdivconformal-1)  passamax=true;
            if(nR == 0)   passamin=true;

            for(l2=1;l2<3;l2++){
             i2 = i+l2;
             if(i2>=nfidivconformal) i2 -=  nfidivconformal;
            for(l=-2; l<3;l++){
             l3 = nR+l;
             if(  l3<0 || l3 >= nrdivconformal )  continue;
              for( k=0;k<nBoxConformal[l3][i2];k++){
                nHit_original = (UShort_t) infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][3];
		if( !InclusionListStt[ nHit_original ] ) continue;
// check if the hit position is near the circle of the Helix found by the fit
                dx = -Oxx+info[ nHit_original ][0];
                dy = -Oyy+info[ nHit_original ][1];
                distance = sqrt(dx*dx+dy*dy);
                if ( fabs(Rr - distance ) > NTIMES*STRAWRADIUS )  continue;

//-------------------
                xx=infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][0];
                dist = fabs( xx +q );
//                if(dist < 3.*infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][2]){
                if(  AcceptHitsConformal(  dist,
                                                 infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][2],
                                                 infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][4]
                                                      ) )  {
                    auxListHitsinTrack[nAssociatedHits]= HitsinBoxConformal[k][l3][i2];
                    nAssociatedHits++; 
                }
              }	//  end of   for( k=0;k<nBoxConformal[l3][i2];k++)
            }   //   end of for(l=-2; l<3;l++)
            }   //   end of for(l2=0;l2<2;l2++)
            return  nAssociatedHits;
           } else if ((nR == nrdivconformal-1 && ! passamin && !passamax) || (nR==0 && !passamax && !passamin)){
             if(nR == nrdivconformal-1)  passamax=true;
             if(nR == 0)   passamin=true;

            for(l2=1;l2<3;l2++){
             i2 = i-l2;
             if(i2<nfidivconformal) i2 += nfidivconformal;
            for(l=-2; l<3;l++){
             l3 = nR+l;
             if(  l3<0 || l3 >= nrdivconformal )  continue;
              for( k=0;k<nBoxConformal[l3][i2];k++){
                nHit_original = (UShort_t) infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][3];
		if( !InclusionListStt[ nHit_original ] ) continue;
// check if the hit position is near the circle of the Helix found by the fit
                dx = -Oxx+info[ nHit_original ][0];
                dy = -Oyy+info[ nHit_original ][1];
                distance = sqrt(dx*dx+dy*dy);
                if ( fabs(Rr - distance ) > NTIMES*STRAWRADIUS )  continue;

//-------------------
                xx=infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][0];
                dist = fabs( xx +q );
//                if(dist < 3.*infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][2]){
                if(  AcceptHitsConformal(  dist,
                                                 infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][2],
                                                 infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][4]
                                                      ) )  {
                    auxListHitsinTrack[nAssociatedHits]= HitsinBoxConformal[k][l3][i2];
                    nAssociatedHits++; 
                }
              }	//  end of  for( k=0;k<nBoxConformal[l3][i2];k++)
            }   //   end of for(l=-2; l<3;l++)
            }   //   end of for(l2=0;l2<2;l2++)
           }    //   end of if((nR == nrdivconformal-1 && passamin) ||  (nR==0 && passamax)  )



      }   //  end of     for(itemp=FFimin; itemp<=FFimax;itemp++)


    } else {  //  q=0 --> x=0



      if( FFimax > nrdivconformal/4 && FFimin < nrdivconformal/4 ) {
        iFi0 =  (Short_t)  (nrdivconformal/4 );
      } else if ( FFimax > 3*nrdivconformal/4 && FFimin < 3*nrdivconformal/4 ){
        iFi0 =  (Short_t)  (3*nrdivconformal/4 );
      }  else {
                cout <<"From TrkAssociatedParallelHitsToHelixQuater  :"
                      <<"  inconsistency, 0 associated hits to this track candidate\n";
        return 0;
     }

      for(itemp=iFi0-5; itemp<=iFi0+5;itemp++){
        i=itemp;
        if( i< 0 ) {
          i += nfidivconformal;
        } else if (i>=nfidivconformal){
          i -=  nfidivconformal*( i/nfidivconformal );
//         i -= nfidivconformal;
        }
        for(l=0; l<nrdivconformal;l++){
              for( k=0;k<nBoxConformal[l][i];k++){
                nHit_original = (UShort_t) infoparalConformal[  HitsinBoxConformal[k][l][i]  ][3];
		if( !InclusionListStt[ nHit_original ] ) continue;
// check if the hit position is near the circle of the Helix found by the fit
                dx = -Oxx+info[ nHit_original ][0];
                dy = -Oyy+info[ nHit_original ][1];
                distance = sqrt(dx*dx+dy*dy);
                if ( fabs(Rr - distance ) >NTIMES*STRAWRADIUS )  continue;

//-------------------
                xx=infoparalConformal[  HitsinBoxConformal[k][l][i]  ][0];
                dist = fabs( xx  );

                if(  AcceptHitsConformal(  dist,
                                                 infoparalConformal[  HitsinBoxConformal[k][l][i]  ][2],
                                                 infoparalConformal[  HitsinBoxConformal[k][l][i]  ][4]
                                                      ) )  {
                    auxListHitsinTrack[nAssociatedHits]= HitsinBoxConformal[k][l][i];
                    nAssociatedHits++;
                }
              }  //  end of for( k=0;k<nBoxConformal[l][i];k++)
        }  //  end of for(l=0; l<nrdivconformal;l++)
      }   //  end of for(itemp=iFi0-5; itemp<=iFi0+5;itemp++)




      }    //   end  of if(fabs(q) > 1.e-10 )


  }  else if( fabs(q)> 1.e-10) {   //   second part of    if( Status ==99),  in this case y = m*x +q

cout<<"cazzocontrollo1 , m  "<<m<<", q "<<q<<", -m*q "<<-m*q<<endl;

	Fi0 = atan(m);	// Fi0 belongs to (-PI/2, PI/2] .
	aaa = atan2(q, -m*q);
	if(Fi0<0.)  {
	 Fi0 += PI;
	 if (Fi0 <0. ) Fi0 =0.;// this is between 0. and PI.
	 if (Fi0 >PI ) Fi0 =PI;// this is between 0. and PI.
	};
	ddd= fabs(q)/sqrt(1.+m*m);
cout<<"cazzocontrollo1 , ddd  "<<ddd<<endl;


        for(itemp=FFimin; itemp<=FFimax;itemp++){
         i=itemp;
         if( i< 0 ) {
            i += nfidivconformal;
         } else if (i>=nfidivconformal){
            i -=  nfidivconformal*( i/nfidivconformal );
         }

	// erre1 is the distance from origin of point of intersection of the straight
	// line of equation  y= m*x+q  with  line of equation  y = x*tan(fi1);
	// when erre1 is < 0 it means the intersection is on the opposite side of the
	// versor defined by  [cos(fi1); sin(fi1)].
	// Here we are working in the conformal plane U,V.

         fi1 = i*2.*(PI/nfidivconformal);
         if( fabs(sin(fi1)-m*cos(fi1))>1.e-10) {
                 erre1 = q/(sin(fi1)-m*cos(fi1));
         } else {
                 erre1 = 99999999999.;
         }
cout<<"cazzocontrollo1 , erre1  "<<erre1<<endl;

         fi2 = (i+1)*2.*(PI/nfidivconformal);
         if( fabs(sin(fi2)-m*cos(fi2))>1.e-10) {
                 erre2 = q/(sin(fi2)-m*cos(fi2));
         } else {
                 erre2 = 99999999999.;
         }
cout<<"cazzocontrollo1 , erre2  "<<erre2<<endl;


         for(j=0; j<nrdivconformal; j++){
cout<<"cazzocontrollo2 , j  "<<j<<", radiaConf[j] "<<radiaConf[j]<<endl;

              Rin = radiaConf[j];
              if(j!=nrdivconformal-1) {
cout<<"cazzocontrollo2 , j+1  "<<j+1<<", radiaConf[j+1] "<<radiaConf[j+1]<<endl;
                 Rout =  radiaConf[j+1];
              }  else {
                 Rout = 1./rstrawdetectormin;
              }

//  note that the following algorithm works also for negative erre1  and   erre2


              if(erre1<-1.e-10 ){
                if(erre2< 0. || erre2 > Rout ){
                     continue;
                }
              } else if(fabs(erre1) < 1.e-10){
                if( Fi0 > fi2 || Fi0 < fi1){
		  continue;
		}
              } else if ( erre1<Rin) {
                if( erre2< Rin &&  erre2> 0. ) {
		 continue;
		}
              }   else if (erre1> Rout  &&  erre2 > Rout &&
			!( fi1<= aaa && aaa<=fi2 && ddd<=Rout )
                ) {
                   continue;
             }

cout<<"cazzocontrollo2 , itemp-2  "<<itemp-2<<", itemp+2 "<<itemp+2<<endl;
              for(l=itemp-2; l<=itemp+2; l++){
                if( l< 0 ) {
                  l2 = l+nfidivconformal;
                } else if (l>=nfidivconformal){
                  l2 = l- nfidivconformal*( l/nfidivconformal );
               } else {
                 l2 = l;
               }
                if( j-1<0) {
                  kstart=0;
                }  else {
                  kstart = j-1;
                }
                if ( j+1 >=  nrdivconformal ) {
                  kend = nrdivconformal;
                } else {
                  kend = j+2;
                }

cout<<"cazzocontrollo2 , kstart  "<<kstart <<", kend "<<kend<<endl;
                for(k=kstart;k<kend;k++){
cout<<"cazzocontrollo3, k "<<k<<", l2 "<<l2<<endl;
cout<<", nBoxConformal[k][l2] "<<nBoxConformal[k][l2]<<endl;

                 for( l3=0;l3<nBoxConformal[k][l2];l3++){
cout<<"cazzodentro L3 "<<l3<<", k "<<k<<", L2 "<<l2<<endl;
cout<<"cazzodentro2, HitsinBoxConformal[l3][k][l2] "
<<HitsinBoxConformal[l3][k][l2]<<endl;

cout<<"cazzodentro2, Unselected[HitsinBoxConformal] "<<
Unselected[HitsinBoxConformal[l3][k][l2] ]<<endl;
		   if( ! Unselected[HitsinBoxConformal[l3][k][l2] ] )  continue;
cout<<"cazzodentro3, infoparalConformal "<<
infoparalConformal[  HitsinBoxConformal[l3][k][l2]  ][3]<<endl;
		   nHit_original = (UShort_t) infoparalConformal
			[  HitsinBoxConformal[l3][k][l2]  ][3];
cout<<"cazzodentro3.1,InclusionListStt["<<nHit_original<<"] = "<<
	InclusionListStt[ nHit_original ]<<endl;
		   if( !InclusionListStt[ nHit_original ] ) continue;
// check if the hit position is near the circle of the Helix found by the fit
                   dx = -Oxx+info[ nHit_original ][0];
                   dy = -Oyy+info[ nHit_original ][1];
                   distance = sqrt(dx*dx+dy*dy);
cout<<"cazzodentro4,dx "<<dx<<", dy "<<dy<<", Rr "<<Rr<<", distance "<<distance<<
	", NTIMES*STRAWRADIUS "<<NTIMES*STRAWRADIUS<< endl;
                   if ( fabs(Rr - distance ) > NTIMES*STRAWRADIUS )  continue;

//-------------------
cout<<"cazzodentro5, infoparalConformal[0] "<<
 infoparalConformal[  HitsinBoxConformal[l3][k][l2]  ][0]<<
 ", infoparalConformal[1] "<<
  infoparalConformal[  HitsinBoxConformal[l3][k][l2]  ][1]<<
 ", infoparalConformal[2] "<<
  infoparalConformal[  HitsinBoxConformal[l3][k][l2]  ][2]<<
 ", infoparalConformal[4] "<<
  infoparalConformal[  HitsinBoxConformal[l3][k][l2]  ][4]<<
  endl;
                   xx=infoparalConformal[  HitsinBoxConformal[l3][k][l2]  ][0];
                   yy=infoparalConformal[  HitsinBoxConformal[l3][k][l2]  ][1];
                   dist = fabs( -yy+ m*xx +q )/sqrt(m*m+1.);
                   if(  AcceptHitsConformal(  dist,
                                    infoparalConformal[  HitsinBoxConformal[l3][k][l2]  ][2],
                                    infoparalConformal[  HitsinBoxConformal[l3][k][l2]  ][4]
                                                      ) )  {

                    auxListHitsinTrack[nAssociatedHits]= HitsinBoxConformal[l3][k][l2];
                    Unselected[HitsinBoxConformal[l3][k][l2]]= false;
                    nAssociatedHits++;
                   }

                 }   //   end of  for( l3=0;l3<nBoxConformal[k][l2];l3++)
                }   //   end of  for(k=kstart;k<kend;k++)
              }     //   end of  for(l=itemp-1; l<itemp+2; l++)

         }   //  end of for(j=0; j<nrdivconformal; j++)

        }   //   end of    for(itemp=FFimin; itemp<=FFimax;itemp++)




  } else {  //  case in which    y= m*x ,  m can be zero    ,  third part of if( Status ==99)
cout<<"cazzo, passamaidiqui?\n";
      iFi0 =  (Short_t)  (atan(m)*nrdivconformal/(2.*PI) );
      for(itemp=iFi0-5; itemp<=iFi0+5;itemp++){
         i=itemp;
         if( i< 0 ) {
          i += nfidivconformal;
         } else if (i>=nfidivconformal){
          i -=  nfidivconformal*( i/nfidivconformal );
         }
        for(l=0; l<nrdivconformal;l++){
              for( k=0;k<nBoxConformal[l][i];k++){
                nHit_original = (UShort_t) infoparalConformal[  HitsinBoxConformal[k][l][i]  ][3];
		if( !InclusionListStt[ nHit_original ] ) continue;
// check if the hit position is near the circle of the Helix found by the fit
                dx = -Oxx+info[ nHit_original ][0];
                dy = -Oyy+info[ nHit_original ][1];
                distance = sqrt(dx*dx+dy*dy);
                if ( fabs(Rr - distance ) > NTIMES*STRAWRADIUS )  continue;

//-------------------
                xx=infoparalConformal[  HitsinBoxConformal[k][l][i]  ][0];
                yy=infoparalConformal[  HitsinBoxConformal[k][l][i]  ][1];
                dist = fabs( m*xx-yy  )/sqrt( m*m+1.);
                if(  AcceptHitsConformal(  dist,
                                                 infoparalConformal[  HitsinBoxConformal[k][l][i]  ][2],
                                                 infoparalConformal[  HitsinBoxConformal[k][l][i]  ][4]
                                                      ) )  {
                    auxListHitsinTrack[nAssociatedHits]= HitsinBoxConformal[k][l][i];
                    nAssociatedHits++;
                }
              }  //  end of for( k=0;k<nBoxConformal[l][i];k++)
        }
      }   //  end of for(itemp=FFimin; itemp<=FFimax;itemp++)





  }   //   end of if ( Status ==99)



 return nAssociatedHits;

}



//----------end of function PndTrkTrackFinderReal::TrkAssociatedParallelHitsToHelixQuater


//----------begin of function PndTrkCTFindTrackInXY::TrkAssociatedParallelHitsToHelix5

UShort_t PndTrkCTFindTrackInXY::TrkAssociatedParallelHitsToHelix5(
	UShort_t *auxListHitsinTrack,
	bool *InclusionListStt,
	Double_t Fi_low,
	Double_t Fi_up,
	Double_t info[][7],
	UShort_t *ListSttParHits,
	UInt_t NhitsParallel,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t STRAWRADIUS
	)
{

  Short_t i;

  UShort_t nAssociatedHits;

  Double_t angle,
           dx,
           dy,
           distance,
           NTIMES=5.;   //   number of Straw radia allowed in association.

  nAssociatedHits=0;
//   find the Hits belonging to this Track.



  for(i=0; i<NhitsParallel;i++){
	if( !InclusionListStt[ ListSttParHits[i] ] ) continue;
// check if the hit position is near the circle of the Helix found by the fit

	dx = -Oxx+info[ListSttParHits[i]][0];
	dy = -Oyy+info[ListSttParHits[i]][1];
	angle=atan2(dy,dx);
	if(angle<0.) angle += 2.*PI;
	if(angle<0.) angle =0.;
	distance = sqrt(dx*dx+dy*dy);
	if ( fabs(Rr - distance ) > NTIMES*STRAWRADIUS )  continue;
	if(angle<Fi_low) angle += 2.*PI;
	if(angle>Fi_up) continue;
	auxListHitsinTrack[nAssociatedHits]= i;
	nAssociatedHits++; 
  } // end for(i=0; i<NhitsParallel;i++)

 return nAssociatedHits;

};



//----------end of function PndTrkCTFindTrackInXY::TrkAssociatedParallelHitsToHelix5

ClassImp(PndTrkCTFindTrackInXY);
