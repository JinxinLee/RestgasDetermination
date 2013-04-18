#include "PndTrkCTFindTrackInXY2.h"
#include "PndTrkCTGeometryCalculations.h"
#include "PndTrkGlpkFits.h"
#include "PndTrkLegendreFits.h"
#include "PndTrkCTGeometryCalculations.h"
#include "PndTrkMergeSort.h"
#include "PndTrkVectors.h"
#include <iostream>
#include <cmath>



#define PI	3.141592654

using namespace std;


//----------begin of function PndTrkCTFindTrackInXY2::AcceptHitsConformal

bool PndTrkCTFindTrackInXY2::AcceptHitsConformal(
 Double_t  distance,
 Double_t  DriftConfR, //drift radius in conformal space
 Double_t  StrawConfR  // straw radius in conformal space
		)
{
  if( fabs(distance-DriftConfR) < 2.*StrawConfR ) return true;
  return false;


}

//----------end of function PndTrkCTFindTrackInXY2::AcceptHitsConformal




//----------begin of function PndTrkCTFindTrackInXY2::AssociateSciTilHit



Short_t PndTrkCTFindTrackInXY2::AssociateSciTilHit(
	Double_t dimensionscitil,
	Double_t *esse, // output, list of  S of the SciTil hits associated. 
	bool* InclusionListSciTil,
	Short_t *List, // output, list of SciTil hits associated (max. 2);
	Short_t maxscitilhitsintrack,
	Short_t nSciTilHits,
	Double_t Oxx,
	Double_t Oyy,
	Double_t posizSciTil[][3],
	Double_t Rr
	)
{

 bool intersect;

 Short_t
	igoodScit,
	iScitHit,
	Nint;

 Double_t
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

//----------begin of function PndTrkCTFindTrackInXY2::FindCharge

void PndTrkCTFindTrackInXY2::FindCharge(
	Double_t oX,
	Double_t oY,
	Short_t nParallelHits,
	Double_t *X,
	Double_t *Y,
	Short_t  * Charge
	)
{

	Short_t ihit,
			nleft,
			nright;

	Double_t cross,
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

//----------end of function PndTrkCTFindTrackInXY2::FindCharge


//----------begin of function PndTrkCTFindTrackInXY2::FindTrackInXYProjection

bool PndTrkCTFindTrackInXY2::FindTrackInXYProjection(
	struct FindTrackInXYProjection2_InputOutputData* InOut
	)
{

 // load the struct members in local variables;

 Double_t (*info)[7]=InOut->info;
 Double_t (*infoparalConformal)[5] = InOut->infoparalConformal  ;

 // make a POINTER to an ARRAY[][3] of
 // Double_t and assign value present in the calling sequence of this method;

 Double_t (*posizSciTil)[3]=(Double_t (*)[3])InOut->posizSciT;

//---------------

 Short_t
	i,
	j,
	Naux,
	Nbaux,
	NN,
	Nouter,
	auxListHitsinTrack[InOut->maxstthits],
	OutputListHitsinTrack[InOut->maxstthits],
	OutputList2HitsinTrack[InOut->maxstthits],
	ListHitsinTrackinWhichToSearch[InOut->maxstthits];

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

 // find the cluster starting from the seed Stt axial hit; the cluster will not have more than maxstthitsintrack hits;

 *(InOut->nHitsinTrack) = FindTrackPattern( InOut );



 // start the selection of the cluster;
 // requirement of minimum number of hits (the mrequirement of maximum number of hits is already been
 // fulfilled in FindTrackPattern);
 if( *(InOut->nHitsinTrack) < InOut->minimumhitspertrack ) return false;
//------------------------------

 // now check if the cluster is a good one;


// StrawCode convention (in the following left or right is looking to the beam from downstream) :
//   -1 = not a boundary straw;
//   10= inner axial boundary left;
//   20= inner axial boundary right;
//   12= outer VERTICAL (BUT NOT OUTERMOST) axial boundary left;
//   22= outer VERTICAL (BUT NOT OUTERMOST)  axial boundary right;
//   13= outermost axial boundary left;
//   23= outermost axial boundary;  right;
//   14= outer axial boundary, (innermost), left;
//   24= outer axial boundary (innermost), right;
//   15= outer axial boundary, special zone, left;
//   25= outer axial boundary, special zone, right;


 Short_t StrawCode = InOut->StrawCode[ InOut->TubeID[ InOut->iHit ] -1 ] ; // -1 is necessary because
		//  the Stt Straws go from 1 to 4542 while the corresponding TubeID is from 0
		//  to 4541;
 Short_t StrawCode2 = InOut->StrawCode2[ InOut->TubeID[ InOut->iHit ] -1 ] ; // -1 is necessary because
		//  the Stt Straws go from 1 to 4542 while the corresponding TubeID is from 0
		//  to 4541;


 if( StrawCode == 13 || StrawCode == 23 || StrawCode2 == 13 || StrawCode2 == 23 ){

 //  case in which the seed hit is at the Outer Boundary of the Stt system;
 // use the contiguity of the Stt outer axial hits and require that the track hits the other
 // boundary of the outer axial section;


 } else if (
 //  case in which the seed hit is at the Inner and Vertical Boundary of the Stt system and
 // in the Zone number 5;
 	StrawCode == 10 ||StrawCode == 20 || StrawCode == 15 ||StrawCode == 25
 	|| StrawCode == 12 ||StrawCode == 22
		||
 	StrawCode2 == 10 ||StrawCode2 == 20
		 ) {





 } else {
	cout<<"from FindTrackPattern : this Stt hit is not at the boundary; this cannot be!\n";
 }

//-----------------------------------



//-----------------------

//   find among the ListHitsinTrack  if there are at least
//   a minimum # of hits belonging to the outer part of the STT  system.
//   At this point of the code the Stt hits are already ordered from
//   the outermost to the innermost.

 for(j=0, Nouter =0; j<*(InOut->nHitsinTrack); j++){
  if(info[ (InOut->ListHitsinTrack)[j] ][0]*
	info[ (InOut->ListHitsinTrack)[j] ][0]+
	info[ (InOut->ListHitsinTrack)[j] ][1]*
	info[ (InOut->ListHitsinTrack)[j] ][1]
	< InOut->apotemamaxskewstraw*InOut->apotemamaxskewstraw  ) break;
  Nouter++;
 }

 if( Nouter >= InOut->minouterhitspertrack) {
	for(i=0; i< Nouter;i++){
	  ListHitsinTrackinWhichToSearch[i]=
		(InOut->ListHitsinTrack)[i];
	}

	for(i=0; i< Nouter;i++){
	 Naux =  FindTrackPatterninBoxConformalSpecial(
		InOut->FiConformalIndex,
		InOut->HitsinBoxConf,
		InOut->InclusionListStt,
		info,
		ListHitsinTrackinWhichToSearch[i], // seed hit.
		ListHitsinTrackinWhichToSearch,
		InOut->ListSttParHits,
		InOut->maxstthits,
		InOut->minimumhitspertrack,
		InOut->nBoxConf,
		1, // NFiCELLDISTANCE
		InOut->nfidivconformal,
		InOut->nsttparhit,
		Nouter,
		3, // NRCELLDISTANCE
		InOut->nrdivconformal,
		OutputListHitsinTrack,
		InOut->RConformalIndex
			);

	 if( Naux >= InOut->minouterhitspertrack && Naux > 0.7 * Nouter )  break;
		 if( Naux >= InOut->minouterhitspertrack) {

//   further collection of hits in the inner region but this time strictly connected
//   to the outer ones

//  first the list of non outer hits
	   for(j=Nouter;j<*(InOut->nHitsinTrack); j++){
		ListHitsinTrackinWhichToSearch[j-Nouter] =
			(InOut->ListHitsinTrack)[j];
	   }

	   Nbaux =  FindTrackStrictCollection(
		InOut->FiConformalIndex,
		InOut->InclusionListStt,
		ListHitsinTrackinWhichToSearch[i],   //  seed hit, in ORIGINAL numbering.
		ListHitsinTrackinWhichToSearch,   //  in ORIGINAL numbering.
		InOut->maxstthits,
		1,    // NFiCELLDISTANCE
		InOut->nfidivconformal,
	//  n. of hits to search in ListHitsinTrackinWhichToSearch
		*(InOut->nHitsinTrack)-Nouter,
		OutputList2HitsinTrack  // in ORIGINAl hit numbering.
					);
//   add the new hits found to the list

	   *(InOut->nHitsinTrack)=Naux+Nbaux;
	   if( *(InOut->nHitsinTrack) >= InOut->minimumhitspertrack &&
		*(InOut->nHitsinTrack) <=InOut->maxstthitsintrack) {
		for(j=0;j<Naux;j++){
			(InOut->ListHitsinTrack)[j] =
				OutputListHitsinTrack[j];
		}
		for(j=0;j<Nbaux;j++){
			(InOut->ListHitsinTrack)[Naux+j] =
				OutputList2HitsinTrack[j];
		}
		break;

	   }// end of  if( *nHitsinTrack >= ....
	 }  // end of  if( Naux >= minouterhitspertrack)
	}   // end of for(i=0; i< Nouter;i++)
 }    // end of if( Nouter >= minouterhitspertrack)

 if( *(InOut->nHitsinTrack) < InOut->minimumhitspertrack ||
  *(InOut->nHitsinTrack)>InOut->maxstthitsintrack) {
	return false;
 }

//---------------------------

//  finding the rotation angle for best utilization of the MILP procedure

 for(j=0, rotationcos=0., rotationsin=0.; j<*(InOut->nHitsinTrack); j++){
	rotationcos += cos((0.5+
	(InOut->FiConformalIndex)[ (InOut->ListHitsinTrack)[j] ])
	*2.*PI/InOut->nfidivconformal) ;

	rotationsin += sin((0.5+
	InOut->FiConformalIndex[ (InOut->ListHitsinTrack)[j] ])
	*2.*PI/InOut->nfidivconformal) ;
 }
 rotationcos /=*(InOut->nHitsinTrack);
 rotationsin /=*(InOut->nHitsinTrack);
 rotationangle = atan2(rotationsin, rotationcos);
//  fitting with superfast MILP code
 bool Type;

 int	nFitPoints,
	offset;
 Double_t
	Xconformal[1+(*(InOut->nHitsinTrack))],
	Yconformal[1+(*(InOut->nHitsinTrack))],
	DriftRadiusconformal[1+(*(InOut->nHitsinTrack))],
	ErrorDriftRadiusconformal[1+(*(InOut->nHitsinTrack))];


 if(InOut->iHit<0){	// case with a hit in the SciTil
  aaa = InOut->posizSciTilx*InOut->posizSciTilx+InOut->posizSciTily*InOut->posizSciTily;
  Xconformal[0] =InOut->posizSciTilx/aaa;
  Yconformal[0] =InOut->posizSciTily/aaa;
  ErrorDriftRadiusconformal[0] = InOut->dimensionscitil/aaa;
  DriftRadiusconformal[0]=-1.;  // treat it like it is a Mvd hit.
 // +1 comes from one SciTil hit.
   offset=1;
   nFitPoints = *(InOut->nHitsinTrack) + 1;

 } else {	// no SciTil hit.
   offset=0;
   nFitPoints = *(InOut->nHitsinTrack);
 }  // end of  if(InOut->iHit<0)



  for(j=0; j<(*(InOut->nHitsinTrack)); j++){
    Xconformal[j+offset] =infoparalConformal[(InOut->ListHitsinTrack)[j]][0];
    Yconformal[j+offset] =infoparalConformal[(InOut->ListHitsinTrack)[j]][1];
    ErrorDriftRadiusconformal[j+offset]=
		infoparalConformal[(InOut->ListHitsinTrack)[j]][2];
    DriftRadiusconformal[j+offset]=
		infoparalConformal[(InOut->ListHitsinTrack)[j]][2];
  }


// PndTrkGlpkFits fit;

 PndTrkLegendreFits fit;

 status = fit.FitHelixCylinder(
		nFitPoints, // +1 comes from one SciTil hit.
		Xconformal,
		Yconformal,
		DriftRadiusconformal,
		ErrorDriftRadiusconformal,
		rotationangle,  //  rotationangle, da mettere
		InOut->trajectory_vertex,	//  vertex in (X,Y) of this trajectory
		InOut->maxhitsinfit,  //  maximum n. of hits allowed in fast fit
		&m,
		&q,
		InOut->ALFA,
		InOut->BETA,
		InOut->GAMMA,
		InOut->TypeConf,
		0, // istampa
		InOut->icounter //  IVOLTE
			);


 if(status < 0  ) return false;

//  this trasformation is valid even if the equation is a straight line from the fit

 *(InOut->Oxx)= -0.5*(*(InOut->ALFA));
 *(InOut->Oyy)= -0.5*(*(InOut->BETA));
 *(InOut->Rr)= (*(InOut->Oxx)) * (*(InOut->Oxx)) + (*(InOut->Oyy)) * (*(InOut->Oyy))-(*(InOut->GAMMA));

	// some obvious preliminary cuts
 if( *(InOut->Rr) < 0. )  return false;
 *(InOut->Rr)= sqrt( *(InOut->Rr) );
 aaa = sqrt( (*(InOut->Oxx)) * (*(InOut->Oxx)) + (*(InOut->Oyy)) * (*(InOut->Oyy))  );

 // the following is because the circumference is supposed to come from (0,0);
 //   here the factor 0.9 is used in order to be conservative.
 if(aaa< 0.9*InOut->rstrawdetectormin/2.) return false;

//   here the factor 0.9 is used in order to be conservative.
 if ( *(InOut->Rr) + aaa < InOut->rstrawdetectormin *0.9 ) return false;

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
 Short_t
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


 if(InOut->YesSciTil){
	//   *(InOut->nSciTilHitsinTrack)    is the n. of SciTil hit associated to this track.
	*(InOut->nSciTilHitsinTrack)= AssociateSciTilHit(
	InOut->dimensionscitil,
	InOut->S_SciTilHitsinTrack,// output; S on the lateral face of the Helix
	 // of the SciTil hit (if present).
	InOut->InclusionListSciTil,
	InOut->ListSciTilHitsinTrack,
	InOut->maxscitilhitsintrack,
	InOut->nSciTilHits,
	*(InOut->Oxx),
	*(InOut->Oyy),
	posizSciTil,
	*(InOut->Rr)
				);

 // even though it should be impossible in principle, EXCLUDE the possibility of having more
 // than TWO SciTil hits belonging to a track;

	if( *(InOut->nSciTilHitsinTrack) >0 ){
	  // even though it should be impossible in principle, EXCLUDE
	  // the possibility of having more
	  // than TWO SciTil hits belonging to a track;
	  if( *(InOut->nSciTilHitsinTrack) > 2 ) *(InOut->nSciTilHitsinTrack)=2;

	  for(j=0;j<*(InOut->nSciTilHitsinTrack);j++){
		(InOut->InclusionListSciTil)[(InOut->ListSciTilHitsinTrack)[j]]
			=false;
	  }
	}

 }else{
	*(InOut->nSciTilHitsinTrack)=0;
 } // end of if(YesSciTil)


//---------------------  better association of the hits in the track candidate
// treat differently the case in which the track has radius < rstrawdetectormax/2
// and the other case.

 if( *(InOut->Rr) < InOut->rstrawdetectormax/2){
	GeomCalculator.FindingParallelTrackAngularRange(
		*(InOut->Oxx),
		*(InOut->Oyy),
		*(InOut->Rr),
		1,  // this is supposed to be the charge, irrelevant here if it is +1 or -1.
		InOut->Fi_low_limit,
		InOut->Fi_up_limit,
		&flagStt,
		InOut->rstrawdetectormin,
		InOut->rstrawdetectormax
		);

	NN =  TrkAssociatedParallelHitsToHelix5(
		auxListHitsinTrack,              //  this is the output
		InOut->InclusionListStt,
		*(InOut->Fi_low_limit),
		*(InOut->Fi_up_limit),
		info,
		InOut->ListSttParHits,
		InOut->nsttparhit,
		*(InOut->Oxx),
		*(InOut->Oyy),
		*(InOut->Rr),
		InOut->strawradius
				);


 }  else {




	NN =  TrkAssociatedParallelHitsToHelixQuater(
		auxListHitsinTrack,	//  this is the output
		InOut->deltanr,
		InOut->FiConformalIndex,
		InOut->HitsinBoxConf,
		InOut->InclusionListStt,
		info,
		infoparalConformal,
		InOut->ListHitsinTrack,
		m,
		InOut->maxstthits,
		InOut->nBoxConf,
		InOut->nfidivconformal,
		*(InOut->nHitsinTrack),
		InOut->nsttparhit,
		InOut->nrdivconformal,
		*(InOut->Oxx),
		*(InOut->Oyy),
		q,
		InOut->radiaConf,
		InOut->RConformalIndex,
		*(InOut->Rr),
		InOut->rstrawdetectormin,
		status,
		InOut->strawradius
			);

 } // end of  if( Rr < rstrawdetectormax/2)

 if( NN < InOut->minimumhitspertrack || NN>InOut->maxstthitsintrack) return false;

 *(InOut->nHitsinTrack)=NN;

 for(i=0; i< *(InOut->nHitsinTrack);i++){
	(InOut->ListHitsinTrack)[i]=auxListHitsinTrack[i];
 }



//----------------------------- Finding the Charge
	// The charge is calculated first by dividing the hits in two categories by using the Perpendicular
	// to the tangent to the trajectory in (0,0). Then simply the majority of the hits decides the
	// sign of the  charge. See Gianluigi's Logbook page 290.

	Double_t
		X[*(InOut->nHitsinTrack)],
		Y[*(InOut->nHitsinTrack)];

	for(i=0;i<*(InOut->nHitsinTrack);i++){
		X[i]=info[(InOut->ListHitsinTrack)[i]][0];
		Y[i]=info[(InOut->ListHitsinTrack)[i]][1];
	}
	FindCharge(
		*(InOut->Oxx),
		*(InOut->Oyy),
		*(InOut->nHitsinTrack),
		X,
		Y,
		InOut->Charge
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
		*(InOut->Charge),
		InOut->Fi_initial_helix_referenceframe,//output
		InOut->Fi_final_helix_referenceframe,// output
		info,
		InOut->ListHitsinTrack,
		*(InOut->nHitsinTrack),
		*(InOut->Oxx),
		*(InOut->Oyy),
		InOut->U,
		InOut->V
			);

//   finding the FI angular range (in the laboratory frame) spanned by this parallel track

 GeomCalculator.FindingParallelTrackAngularRange(
		*(InOut->Oxx),
		*(InOut->Oyy),
		*(InOut->Rr),
		*(InOut->Charge),
		InOut->Fi_low_limit,
		InOut->Fi_up_limit,
		&flagStt,
		InOut->rstrawdetectormin,
		InOut->rstrawdetectormax
					);


	if( flagStt == -2 ) return false;	// track outside cylinder rstrawdetectormax.
	if( flagStt == -1 ) return false;	// track inside cylinder rstrawdetectormin.
	if( flagStt ==  1 ) return false;	// track comprised in cylinder : discard,
					// because at this stage only tracks from
					// vertex are searched.




	return true;
};

//----------end of function PndTrkCTFindTrackInXY2::FindTrackInXYProjection



//----------begin of function PndTrkCTFindTrackInXY2::FindTrackPattern

Short_t PndTrkCTFindTrackInXY2::FindTrackPattern(
	struct FindTrackInXYProjection2_InputOutputData* InOut
			)
{



 Short_t
		ContiguousTube,
		CurrentTube,
		i,
		tCurrentListofTubes[ InOut->maxstthitsintrack ];  // current list of tubes forming the cluster;

 Vec <Short_t>
		CurrentListofTubes(tCurrentListofTubes, InOut->maxstthitsintrack, "CurrentListofTubes");



// the following is a necessary initialization every time a new cluster is searched starting from a given seed hit;

 bool tAlreadyConsidered[ InOut->number_straws ] ;   // for each Stt Straw;
 memset ( tAlreadyConsidered,false, sizeof( tAlreadyConsidered ));
 Vec<bool> AlreadyConsidered(tAlreadyConsidered,InOut->number_straws,"AlreadyConsidered");


 // algorithm of clusterization by proximity only;

 Short_t nHitsinTrack = 1;
 Short_t nHitsAnalyzed = 0;
 InOut->ListHitsinTrack[0] = InOut->iHit;
 CurrentListofTubes[0] = InOut->TubeID[ InOut->iHit ];
 bool	go = true;


 // nParContiguous[i] is the number of contiguous straws to the i-th straw (therefore the dimension
 //	is nParContiguous[4542+1] since nParContiguous[4542] = contiguous of straw n. 4542;
 // ListParContiguous is the List in TubeID contiguous to a given Tube number;
 // it MUST be used as follows : ListParContiguous[ j ][i] = the TubeID of the (i+1)th contiguous straw
 //	to the j-th straw (tube number); therefore the dimension is ListParContiguous[4542+1][6];
 while(nHitsAnalyzed < nHitsinTrack && go){	// main loop;
	CurrentTube = CurrentListofTubes[nHitsAnalyzed];
	AlreadyConsidered[ CurrentTube-1 ] = true;
	nHitsAnalyzed++;

	for(i=0;i< InOut->nParContiguous[ CurrentTube-1 ] ; i++ ) {
		// SttStrawOn is -1 if the Tube was not hit, it is the Stt Hit number in opposite case;
		ContiguousTube = InOut->ListParContiguous[ CurrentTube-1 ][i];

		if( AlreadyConsidered[ ContiguousTube-1 ])continue;

		if ( InOut->SttStrawOn[ ContiguousTube-1 ] > -1 ){
		if(! InOut->InclusionListStt[ InOut->SttStrawOn[ ContiguousTube-1] ] ) continue;

			if( nHitsinTrack == InOut->maxstthitsintrack ){
				go = false;
				break;
			}  // end of  if( nHitsinTrack

			//  CurrentListofHits is the List of Stt HIT numbers belonging to the present cluster;
			InOut->ListHitsinTrack[nHitsinTrack] = InOut->SttStrawOn[ ContiguousTube -1 ] ;
			CurrentListofTubes[nHitsinTrack] = ContiguousTube ;
			AlreadyConsidered[ ContiguousTube-1 ] = true;
			nHitsinTrack ++ ;
		}
	}   // end of for(i=0;i< InOut->n ...
 }   // end of while(nRemaining > 0)

 return nHitsinTrack;

}


//----------end of function PndTrkCTFindTrackInXY2::FindTrackPattern



//----------begin of function PndTrkCTFindTrackInXY2::FindTrackPatterninBoxConformalSpecial

//  The difference with FindTrackPatterninBoxConformal is that the search is performed
//  in the selected list    ListHitsinTrackinWhichToSearch
//  instead of all the parallel list.

Short_t PndTrkCTFindTrackInXY2::FindTrackPatterninBoxConformalSpecial(
	Short_t *FiConformalIndex,
	Short_t* HitsinBoxConformal,
	bool *InclusionListStt,
	Double_t info[][7],
	Short_t iSeed, // in ORIGINAL hit numbering.
	Short_t *ListHitsinTrackinWhichToSearch, // in original hit numbering.
	Short_t* ListSttParHits,
	Short_t maxstthits,
	Short_t minimumhitspertrack,
	Short_t* nBoxConformal,
	Short_t NFiCELLDISTANCE,
	Short_t nfid,  // divisions in Fi in the conformal;
	Short_t Nparal,
	Short_t NparallelToSearch,
	Short_t NRCELLDISTANCE,
	Short_t nrd,  // divisions in R in the conformal;
	Short_t *OutputListHitsinTrack, // in original hit numbering.
	Short_t *RConformalIndex
					)
{


 bool TemporaryInclusionList[maxstthits];

 Short_t
	bi_index,
	i,
	i2,
	j,
	iFi,
	iFi2,
	iR,
	nFicell,
	nHitsinTrack,
	nRcell,
	nRemainingHits,
	nRmax,
	nRmin,
	tri_index,
	auxIndex[maxstthits],
	Remaining[maxstthits];


 Double_t auxRvalues[maxstthits];




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
	if (nRcell + NRCELLDISTANCE >= nrd ) {
		nRmax = nrd-1;
	}  else {
		nRmax = nRcell + NRCELLDISTANCE;
	}

	for( iR= nRmin ; iR<= nRmax ; iR++){
	  for( iFi2=nFicell-NFiCELLDISTANCE ;
		iFi2<=nFicell+NFiCELLDISTANCE;iFi2++){
		if ( iFi2 < 0 )  {
			iFi = nfid + iFi2;
		} else if ( iFi2 >= nfid) {
			iFi = iFi2  - nfid;
		} else {
			iFi = iFi2;
		}
	     bi_index = iR*nfid+iFi;
	     for (j = 0; j< nBoxConformal[bi_index]; j++){
		tri_index = j*nrd*nfid+bi_index;

		if(InclusionListStt[HitsinBoxConformal[tri_index]]
				&&
			TemporaryInclusionList[ HitsinBoxConformal[tri_index]]
			) {
//  hit number in the ORIGINAL straws scheme
		  OutputListHitsinTrack[nHitsinTrack]=HitsinBoxConformal[tri_index] ;
		  nHitsinTrack++;
		  TemporaryInclusionList[HitsinBoxConformal[tri_index]]
			= false;
		  nRemainingHits--;
		}  // end of    if(InclusionListStt[ .. ]
	     } // end of for (j = 0; j< *(nBoxConfor ....
	  }  // end of    for( iFi2=nFice
	}  // end of  for( iR= nRmin
//----------------
	i++;

 }    //  end while ( nRemainingHits > 0 && i < nHitsinTrack)


 return nHitsinTrack;

}


//----------end of function PndTrkCTFindTrackInXY2::FindTrackPatterninBoxConformalSpecial


//----------begin of function PndTrkCTFindTrackInXY2::FindTrackStrictCollection

Short_t PndTrkCTFindTrackInXY2::FindTrackStrictCollection(
	Short_t *FiConformalIndex,
	bool *InclusionListStt,
	//  seed track (original notation) as far as the Fi angle is concerned
	Short_t iSeed,  // in ORIGINAl hit numbering.
	Short_t *ListHitsinTrackinWhichToSearch,  // in ORIGINAl hit numbering.
	Short_t maxstthits,
	Short_t NFiCELLDISTANCE,
	Short_t nfid,
	//  n. of hits to search in ListHitsinTrackinWhichToSearch
	Short_t NParallelToSearch,
	Short_t  *OutputListHitsinTrack  // in ORIGINAl hit numbering.
	)
{

 Short_t
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
		if(iFi+nfid-iFiseed<=NFiCELLDISTANCE) {
		  OutputListHitsinTrack[nHitsinTrack]=ListHitsinTrackinWhichToSearch[i];
		  nHitsinTrack++;
		}
	   } // end of  if(iFiseed - iFi <= NFiCELLDISTANCE)
	}else{   //  continuation of  if( iFi == iFiseed )
	   if( -iFiseed + iFi <= NFiCELLDISTANCE ) {
		OutputListHitsinTrack[nHitsinTrack]=ListHitsinTrackinWhichToSearch[i];
		nHitsinTrack++;
	   }else{
		if( -iFi + nfid + iFiseed<= NFiCELLDISTANCE ) {
		  OutputListHitsinTrack[nHitsinTrack]=ListHitsinTrackinWhichToSearch[i];
		  nHitsinTrack++;
		}
	   }  // end of  if( -iFiseed + iFi <= NFiCELLDISTANCE )

	}  //  end of   if( iFi == iFiseed )

        }    //  end of    if( InclusionListStt[ListHitsinTrackinWhichToSearch[i]])

     }   //  end of        for(i=0; i<NparallelToSearch; i++)



 return nHitsinTrack;

}


//----------end of function PndTrkCTFindTrackInXY2::FindTrackStrictCollection

//----------begin of function PndTrkCTFindTrackInXY2::OrderingParallel

void PndTrkCTFindTrackInXY2::OrderingParallel(
	Short_t  Charge,
	Double_t *Fi_initial_helix_referenceframe,
	Double_t *Fi_final_helix_referenceframe,
	Double_t info[][7],
	Short_t *ListParallelHits,
	Short_t nParallelHits,
	Double_t oX,
	Double_t oY,
	Double_t *U,
	Double_t *V
	)
{

 Short_t
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
			ListParallelHits[j] = (Short_t) tempo[j];
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
			ListParallelHits[j] = (Short_t) tempo[j];
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
//----------end of function PndTrkCTFindTrackInXY2::OrderingParallel



//----------begin of function PndTrkTrackFinderReal::TrkAssociatedParallelHitsToHelixQuater

Short_t PndTrkCTFindTrackInXY2::TrkAssociatedParallelHitsToHelixQuater(
	Short_t *auxListHitsinTrack,
	Double_t deltanr,
	Short_t *FiConformalIndex,
	Short_t* HitsinBoxConformal,
	bool* InclusionListStt,
	Double_t info[][7],
	Double_t infoparalConformal[][5],
	Short_t *ListHitsinTrack,
	Double_t m,
	Short_t maxstthits,
	Short_t* nBoxConformal,
	Short_t nfid, // number of division of the fi range [0-360] used in BoxConformal.
	Short_t nHitsinTrack,
	Int_t NhitsParallel,
	Short_t nrd,
	Double_t Oxx,
	Double_t Oyy,
	Double_t q,
	Double_t* radiaConf,
	Short_t *RConformalIndex,
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
  Short_t i, i2, j, k, l,  l2, l3, itemp, kstart, kend,
          iFi0,FFimin, FFimax;
  Short_t Nextra=8,
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



 nAssociatedHits=0;


 size_t len = sizeof(Unselected);
 memset (Unselected,true,len);


//   find the range in Fi spanned  by the candidate track

  FFimin = 10000;
  FFimax = 0;

  for(j=0; j<nHitsinTrack; j++){
    i = ListHitsinTrack[j];
	
  }  // end of for(j=0; j<nHitsinTrack; j++)




// --------  here treat the case in which a trajectory crosses the ascissa in the
// conformal plane and consequently FFimin is close to zero while FFimax is close
// to the maximum number possible (=nfid); in other words FFimin and FFimax are
// wrong;

  if( FFimax > 3.*nfid/4. && FFimin < nfid/4.) {
     FFimin = 10000;
     FFimax =  0;
     for(j=0; j<nHitsinTrack; j++){
       i = ListHitsinTrack[j];
       Fi = FiConformalIndex[i];
       if( Fi < nfid/4. ) Fi = FiConformalIndex[i]+nfid;
       if( Fi <  FFimin ) FFimin = Fi;
       if( Fi >  FFimax ) FFimax = Fi;
     }
  }


//  finding the boundaries in the Conformal plane. The basic assumption is that the range
// in Fi is much less that 180 degrees.


  FFimin -= (Short_t) nfid/Nextra;
  FFimax +=  (Short_t) nfid/Nextra;
if( FFimax - FFimin > nfid/2 ) {
    cout<<"something fishy is going on in TrkAssociatedParallelHitsToHelixQuater!"
      <<"Range in Fi (rad) is  "<<(FFimax - FFimin)*2.*PI/nfid<<endl;
    return 0;
}


//   use the equation of a line in polar coordinates
 
  if( Status ==99) {   //  case in which   0 = x + q

    if(fabs(q) > 1.e-10 ) {
      passamax=false;
      passamin=false;
      for(itemp=FFimin; itemp<=FFimax;itemp++){
        i=itemp;
        if( i< 0 ) {
          i += nfid;
        } else if (i>=nfid){
          i -=  nfid*( i/nfid );
        }
        angle = (i+0.5)*2.*PI/nfid;
        aaa = cos(angle);
        if( fabs(cos(angle)) <1.e-10)  continue;
        r = -q/aaa;
        if(r< radiaConf[0] || r>= 1./rstrawdetectormin)  continue;
        for(j=nrd-1; j>=0;j--){
          if( r>= radiaConf[j] ){
           nR = j;
           break;
          }
        }


        for(l=(Short_t) (-deltanr) ; l<(Short_t)deltanr+1;l++){
          l2 = nR+l;
          if(  l2<0 || l2 >= nrd )  continue;
              for( k=0;k<*(nBoxConformal+l2*nrd+i);k++){
                nHit_original = (Short_t)
			infoparalConformal[*(HitsinBoxConformal+k*nrd*nfid+l2*nfid+i)][3];
		if( !InclusionListStt[ nHit_original ] ) continue;
// check if the hit position is near the circle of the Helix found by the fit
                dx = -Oxx+info[ nHit_original ][0];
                dy = -Oyy+info[ nHit_original ][1];
                distance = sqrt(dx*dx+dy*dy);
                if ( fabs(Rr - distance ) > NTIMES*STRAWRADIUS )  continue;

//-------------------
                xx=infoparalConformal[  *(HitsinBoxConformal+k*nrd*nfid+l2*nfid+i)  ][0];
                dist = fabs( xx +q );
                if(  AcceptHitsConformal(  dist,
			infoparalConformal[*(HitsinBoxConformal+k*nrd*nfid+l2*nfid+i)][2],
			infoparalConformal[*(HitsinBoxConformal+k*nrd*nfid+l2*nfid+i)][4]
				) )  {
                    auxListHitsinTrack[nAssociatedHits]= *(HitsinBoxConformal+k*nrd*nfid+l2*nfid+i);
                    nAssociatedHits++; 
                }
              }	//  end of  for( k=0;k<*(nBoxConformal....
        }   //   end of for(l=(Short_t)(-deltanr);l<(Short_t)deltanr+1;l++)


        //------- special cases
           if((nR == nrd-1 && passamin && ! passamax) ||  (nR==0 && passamax && !passamin)  ) {   //  do the last two Fi columns
            if(nR == nrd-1)  passamax=true;
            if(nR == 0)   passamin=true;

            for(l2=1;l2<3;l2++){
             i2 = i+l2;
             if(i2>=nfid) i2 -=  nfid;
            for(l=-2; l<3;l++){
             l3 = nR+l;
             if(  l3<0 || l3 >= nrd )  continue;
              for( k=0;k<*(nBoxConformal+l3*nfid+i2);k++){
                nHit_original = (Short_t)
			infoparalConformal[*(HitsinBoxConformal+k*nrd*nfid+l3*nfid+i2)][3];
		if( !InclusionListStt[ nHit_original ] ) continue;
// check if the hit position is near the circle of the Helix found by the fit
                dx = -Oxx+info[ nHit_original ][0];
                dy = -Oyy+info[ nHit_original ][1];
                distance = sqrt(dx*dx+dy*dy);
                if ( fabs(Rr - distance ) > NTIMES*STRAWRADIUS )  continue;

//-------------------
                xx=infoparalConformal[  *(HitsinBoxConformal+k*nrd*nfid+l3*nfid+i2)  ][0];
                dist = fabs( xx +q );
//                if(dist < 3.*infoparalConformal[  *(HitsinBoxConformal+k*nrd*nfid+l3*nfid+i2)  ][2]){
                if(  AcceptHitsConformal(  dist,
          		infoparalConformal[*(HitsinBoxConformal+k*nrd*nfid+l3*nfid+i2)][2],
          		infoparalConformal[*(HitsinBoxConformal+k*nrd*nfid+l3*nfid+i2)][4]
          				) )  {
                    auxListHitsinTrack[nAssociatedHits]= *(HitsinBoxConformal+k*nrd*nfid+l3*nfid+i2);
                    nAssociatedHits++; 
                }
              }	//  end of   for( k=0;k<*(nBoxConformal....
            }   //   end of for(l=-2; l<3;l++)
            }   //   end of for(l2=0;l2<2;l2++)
            return  nAssociatedHits;
           } else if ((nR == nrd-1 && ! passamin && !passamax) || (nR==0 && !passamax && !passamin)){
             if(nR == nrd-1)  passamax=true;
             if(nR == 0)   passamin=true;

            for(l2=1;l2<3;l2++){
             i2 = i-l2;
             if(i2<nfid) i2 += nfid;
            for(l=-2; l<3;l++){
             l3 = nR+l;
             if(  l3<0 || l3 >= nrd )  continue;
              for( k=0;k<*(nBoxConformal+l3*nfid+i2);k++){
                nHit_original = (Short_t)
			infoparalConformal[*(HitsinBoxConformal+k*nrd*nfid+l3*nfid+i2)][3];
		if( !InclusionListStt[ nHit_original ] ) continue;
// check if the hit position is near the circle of the Helix found by the fit
                dx = -Oxx+info[ nHit_original ][0];
                dy = -Oyy+info[ nHit_original ][1];
                distance = sqrt(dx*dx+dy*dy);
                if ( fabs(Rr - distance ) > NTIMES*STRAWRADIUS )  continue;

//-------------------
                xx=infoparalConformal[*(HitsinBoxConformal+k*nrd*nfid+l3*nfid+i2)][0];
                dist = fabs( xx +q );
//                if(dist < 3.*infoparalConformal[  *(HitsinBoxConformal+k*nrd*nfid+l3*nfid+i2) ][2]){
                if(  AcceptHitsConformal(  dist,
			infoparalConformal[*(HitsinBoxConformal+k*nrd*nfid+l3*nfid+i2)][2],
			infoparalConformal[*(HitsinBoxConformal+k*nrd*nfid+l3*nfid+i2)][4]
					) )  {
                    auxListHitsinTrack[nAssociatedHits]= *(HitsinBoxConformal+k*nrd*nfid+l3*nfid+i2);
                    nAssociatedHits++; 
                }
              }	//  end of  for( k=0;k<*(nBoxConformal.....
            }   //   end of for(l=-2; l<3;l++)
            }   //   end of for(l2=0;l2<2;l2++)
           }    //   end of if((nR == nrd-1 && passamin) ||  (nR==0 && passamax)  )



      }   //  end of     for(itemp=FFimin; itemp<=FFimax;itemp++)


    } else {  //  q=0 --> x=0



      if( FFimax > nrd/4 && FFimin < nrd/4 ) {
        iFi0 =  (Short_t)  (nrd/4 );
      } else if ( FFimax > 3*nrd/4 && FFimin < 3*nrd/4 ){
        iFi0 =  (Short_t)  (3*nrd/4 );
      }  else {
                cout <<"From TrkAssociatedParallelHitsToHelixQuater  :"
                      <<"  inconsistency, 0 associated hits to this track candidate\n";
        return 0;
     }

      for(itemp=iFi0-5; itemp<=iFi0+5;itemp++){
        i=itemp;
        if( i< 0 ) {
          i += nfid;
        } else if (i>=nfid){
          i -=  nfid*( i/nfid );
//         i -= nfid;
        }
        for(l=0; l<nrd;l++){
              for( k=0;k<*(nBoxConformal+l*nfid+i);k++){
                nHit_original = (Short_t)
			infoparalConformal[*(HitsinBoxConformal+k*nrd*nfid+l*nfid+i)][3];
		if( !InclusionListStt[ nHit_original ] ) continue;
// check if the hit position is near the circle of the Helix found by the fit
                dx = -Oxx+info[ nHit_original ][0];
                dy = -Oyy+info[ nHit_original ][1];
                distance = sqrt(dx*dx+dy*dy);
                if ( fabs(Rr - distance ) >NTIMES*STRAWRADIUS )  continue;

//-------------------
                xx=infoparalConformal[*(HitsinBoxConformal+k*nrd*nfid+l*nfid+i)][0];
                dist = fabs( xx  );

                if(  AcceptHitsConformal(  dist,
			infoparalConformal[*(HitsinBoxConformal+k*nrd*nfid+l*nfid+i)][2],
			infoparalConformal[*(HitsinBoxConformal+k*nrd*nfid+l*nfid+i)][4]
				) )  {
                    auxListHitsinTrack[nAssociatedHits]=*(HitsinBoxConformal+k*nrd*nfid+l*nfid+i);
                    nAssociatedHits++;
                }
              }  //  end of for( k=0;k<*(nBoxConformal...
        }  //  end of for(l=0; l<nrd;l++)
      }   //  end of for(itemp=iFi0-5; itemp<=iFi0+5;itemp++)




      }    //   end  of if(fabs(q) > 1.e-10 )


  }  else if( fabs(q)> 1.e-10) {   //   second part of    if( Status ==99),  in this case y = m*x +q


	Fi0 = atan(m);	// Fi0 belongs to (-PI/2, PI/2] .
	aaa = atan2(q, -m*q);
	if(Fi0<0.)  {
	 Fi0 += PI;
	 if (Fi0 <0. ) Fi0 =0.;// this is between 0. and PI.
	 if (Fi0 >PI ) Fi0 =PI;// this is between 0. and PI.
	};
	ddd= fabs(q)/sqrt(1.+m*m);


        for(itemp=FFimin; itemp<=FFimax;itemp++){
         i=itemp;
         if( i< 0 ) {
            i += nfid;
         } else if (i>=nfid){
            i -=  nfid*( i/nfid );
         }

	// erre1 is the distance from origin of point of intersection of the straight
	// line of equation  y= m*x+q  with  line of equation  y = x*tan(fi1);
	// when erre1 is < 0 it means the intersection is on the opposite side of the
	// versor defined by  [cos(fi1); sin(fi1)].
	// Here we are working in the conformal plane U,V.

         fi1 = i*2.*(PI/nfid);
         if( fabs(sin(fi1)-m*cos(fi1))>1.e-10) {
                 erre1 = q/(sin(fi1)-m*cos(fi1));
         } else {
                 erre1 = 99999999999.;
         }

         fi2 = (i+1)*2.*(PI/nfid);
         if( fabs(sin(fi2)-m*cos(fi2))>1.e-10) {
                 erre2 = q/(sin(fi2)-m*cos(fi2));
         } else {
                 erre2 = 99999999999.;
         }


         for(j=0; j<nrd; j++){

              Rin = radiaConf[j];
              if(j!=nrd-1) {
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

              for(l=itemp-2; l<=itemp+2; l++){
                if( l< 0 ) {
                  l2 = l+nfid;
                } else if (l>=nfid){
                  l2 = l- nfid*( l/nfid );
               } else {
                 l2 = l;
               }
                if( j-1<0) {
                  kstart=0;
                }  else {
                  kstart = j-1;
                }
                if ( j+1 >=  nrd ) {
                  kend = nrd;
                } else {
                  kend = j+2;
                }

                for(k=kstart;k<kend;k++){

                 for( l3=0;l3<*(nBoxConformal+k*nfid+l2);l3++){
		   if( ! Unselected[*(HitsinBoxConformal+l3*nrd*nfid+k*nfid+l2)] )  continue;
		   nHit_original = (Short_t) infoparalConformal
			[*(HitsinBoxConformal+l3*nrd*nfid+k*nfid+l2)][3];

		   if( !InclusionListStt[ nHit_original ] ) continue;
// check if the hit position is near the circle of the Helix found by the fit
                   dx = -Oxx+info[ nHit_original ][0];
                   dy = -Oyy+info[ nHit_original ][1];
                   distance = sqrt(dx*dx+dy*dy);
                  if ( fabs(Rr - distance ) > NTIMES*STRAWRADIUS )  continue;

//-------------------
                   xx=infoparalConformal[*(HitsinBoxConformal+l3*nrd*nfid+k*nfid+l2)][0];
                   yy=infoparalConformal[*(HitsinBoxConformal+l3*nrd*nfid+k*nfid+l2)][1];
                   dist = fabs( -yy+ m*xx +q )/sqrt(m*m+1.);
                   if(  AcceptHitsConformal(  dist,
                                    infoparalConformal[*(HitsinBoxConformal+l3*nrd*nfid+k*nfid+l2)][2],
                                    infoparalConformal[*(HitsinBoxConformal+l3*nrd*nfid+k*nfid+l2)][4]
                                                      ) )  {

                    auxListHitsinTrack[nAssociatedHits]=*(HitsinBoxConformal+l3*nrd*nfid+k*nfid+l2);
                    Unselected[*(HitsinBoxConformal+l3*nrd*nfid+k*nfid+l2)]= false;
                    nAssociatedHits++;
                   }

                 }   //   end of  for( l3=0;l3<*(nBoxConformal...
                }   //   end of  for(k=kstart;k<kend;k++)
              }     //   end of  for(l=itemp-1; l<itemp+2; l++)

         }   //  end of for(j=0; j<nrd; j++)

        }   //   end of    for(itemp=FFimin; itemp<=FFimax;itemp++)




  } else {  //  case in which    y= m*x ,  m can be zero    ,  third part of if( Status ==99)
      iFi0 =  (Short_t)  (atan(m)*nrd/(2.*PI) );
      for(itemp=iFi0-5; itemp<=iFi0+5;itemp++){
         i=itemp;
         if( i< 0 ) {
          i += nfid;
         } else if (i>=nfid){
          i -=  nfid*( i/nfid );
         }
        for(l=0; l<nrd;l++){
              for( k=0;k<*(nBoxConformal+l*nfid+i);k++){
                nHit_original = (Short_t)
			infoparalConformal[*(HitsinBoxConformal+k*nrd*nfid+l*nfid+i)][3];
		if( !InclusionListStt[ nHit_original ] ) continue;
// check if the hit position is near the circle of the Helix found by the fit
                dx = -Oxx+info[ nHit_original ][0];
                dy = -Oyy+info[ nHit_original ][1];
                distance = sqrt(dx*dx+dy*dy);
                if ( fabs(Rr - distance ) > NTIMES*STRAWRADIUS )  continue;

//-------------------
                xx=infoparalConformal[*(HitsinBoxConformal+k*nrd*nfid+l*nfid+i)][0];
                yy=infoparalConformal[*(HitsinBoxConformal+k*nrd*nfid+l*nfid+i)][1];
                dist = fabs( m*xx-yy  )/sqrt( m*m+1.);
                if(  AcceptHitsConformal(  dist,
			infoparalConformal[*(HitsinBoxConformal+k*nrd*nfid+l*nfid+i)][2],
			infoparalConformal[*(HitsinBoxConformal+k*nrd*nfid+l*nfid+i)][4]
				) )  {
                    auxListHitsinTrack[nAssociatedHits]=*(HitsinBoxConformal+k*nrd*nfid+l*nfid+i);
                    nAssociatedHits++;
                }
              }  //  end of for( k=0;k<*(nBoxConformal
        }
      }   //  end of for(itemp=FFimin; itemp<=FFimax;itemp++)





  }   //   end of if ( Status ==99)



 return nAssociatedHits;

}



//----------end of function PndTrkTrackFinderReal::TrkAssociatedParallelHitsToHelixQuater


//----------begin of function PndTrkCTFindTrackInXY2::TrkAssociatedParallelHitsToHelix5

Short_t PndTrkCTFindTrackInXY2::TrkAssociatedParallelHitsToHelix5(
	Short_t *auxListHitsinTrack,
	bool *InclusionListStt,
	Double_t Fi_low,
	Double_t Fi_up,
	Double_t info[][7],
	Short_t *ListSttParHits,
	Int_t NhitsParallel,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t STRAWRADIUS
	)
{

  Short_t i;

  Short_t nAssociatedHits;

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



//----------end of function PndTrkCTFindTrackInXY2::TrkAssociatedParallelHitsToHelix5

ClassImp(PndTrkCTFindTrackInXY2);
