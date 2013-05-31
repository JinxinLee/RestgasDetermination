#include "PndTrkCleanup.h"
#include "PndTrkCTGeometryCalculations.h"
#include "PndTrkVectors.h"
#include <iostream>
#include <cmath>


// Root includes
#include "TROOT.h"

#define MAX_NOT_CONNECTED  1

using namespace std;



//----------begin of function PndTrkCleanup::BadTrack_ParStt

bool PndTrkCleanup::BadTrack_ParStt(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t STRAWRADIUS,
	Short_t Charge,
	Double_t Xcross[2],  // Xcross[0]=point of entrance;
				//  Xcross[1]=point of exit.
	Double_t Ycross[2],
	Short_t nHits,
	Short_t* ListHits,
	Double_t info[][7],
	int istampa,
	Double_t cut,
	Short_t maxnum,
	Short_t islack// uncertainty allowed as far as
		// the n. of hits that should be present in a given section of the Stt track.
	)
{
	Short_t	ibad,
			ihit,
			ninside;

	Double_t	cut2,
			length,
			Xprevious,
			Yprevious,
			S,
			Distance[nHits+1];
	const Double_t PI = 3.141592654;

	// class with all the geometry calculations :
	PndTrkCTGeometryCalculations GeometryCalculator;

	cut2=cut*cut;
	ibad=0;

	Xprevious=Xcross[0];
	Yprevious=Ycross[0];

	length= GeometryCalculator.CalculateArcLength(Oxx,
				Oyy,
				Rr,
				Charge,
				Xcross,
				Ycross
				);
if(istampa>1) {cout<<"in BadTrack_ParStt : Xingresso "<<Xcross[0]<<", Yingresso "<<Ycross[0]
	<<", Xuscita "<<Xcross[1]<<", Yuscita "<<Ycross[1]<<", Lungh. arco "
	<<length<<", islack "<<islack<<endl;
	}


	ninside=0;
	for(ihit=0; ihit<nHits;ihit++){
		S = atan2(info[ListHits[ihit]][1]-Oyy,info[ListHits[ihit]][0]-Oxx);
		if(S<0.) S += 2.*PI;
		if(S<0.) S = 0.;
		if( ! GeometryCalculator.IsInsideArc(
			Oxx,Oyy,Charge,
			Xcross,
			Ycross,
			S
		) ) {
			continue;
			}
		ninside++;
		Distance[ihit]= 
			(info[ListHits[ihit]][0]-Xprevious)*
			(info[ListHits[ihit]][0]-Xprevious)+
			(info[ListHits[ihit]][1]-Yprevious)*
			(info[ListHits[ihit]][1]-Yprevious);
if(istampa>1) {cout<<"in BadTrack_ParStt :hit || n. "<<ListHits[ihit]<<", X "<<info[ListHits[ihit]][0]
<<", Y "<<info[ListHits[ihit]][1]<<"\n\tX prima "<<Xprevious
<<", Y prima "<<Yprevious<<", Distanza "<<sqrt(Distance[ihit])<<", cut = "
<<cut<<endl;
}
		Xprevious=info[ListHits[ihit]][0];
		Yprevious=info[ListHits[ihit]][1];


		if(Distance[ihit]>cut2){
			if(Distance[ihit]>16.*cut2){
				return true;
			}
			ibad++;
		}

	}	// end of   for(ihit=0 ;ihit<nHits;ihit++)


	// cut on the minimum (conservative) n. hits that must have fired

	int nume;
	nume = 0.5*length/STRAWRADIUS -islack;
	if( ninside < nume ){
		if(istampa>1){
			int icz = 0.5*length/STRAWRADIUS;
			cout<<"in BadTrack_ParStt, n. Hits inside = "<<ninside
			<<" is < n. hits that should be inside at least = "
			<<icz<<"-islack ("<<islack<<"), track rejected!\n";
		}
		return true;
	}

	// compute the distance of last hit to point at which track leaves this detector volume.
	// In case nHits = 0 don't do this check (whether or not the track is genuine has
	// been decided already in the previous  for(ihit=0 ;ihit<nHits;ihit++) loop).

	if(nHits>0){

	// here S is already the fi of the last point.
	  if( GeometryCalculator.IsInsideArc(Oxx,Oyy,Charge,
			Xcross,
			Ycross,
			S ))
	  {

		Distance[nHits] =
			(info[ListHits[nHits-1]][0]-Xcross[1])*
			(info[ListHits[nHits-1]][0]-Xcross[1])+
			(info[ListHits[nHits-1]][1]-Ycross[1])*
			(info[ListHits[nHits-1]][1]-Ycross[1]);
						;
		if(istampa>1)cout<<"in BadTrack_ParStt, Stt || hit n. (original notation) "
			<<ListHits[nHits-1]<<", Distance to boundary = "
			<<sqrt(Distance[nHits])<<", 4*cut "<<4.*cut<<endl;

		if( Distance[nHits]>cut2 ){
			if( Distance[nHits]>16.*cut2)return true;
			ibad++;
		}  // end of  if( Distance[nHits]>cut2 )
	  }  // end of if( IsInsideArc

if(istampa>1)cout<<"in BadTrack_ParStt, ibad "<<ibad<<", max bad allowed = "<< maxnum<<endl;

	if( ibad > maxnum) return true;
	return false;

	} // end of   if(nHits>0)



}


//----------end of function PndTrkCleanup::BadTrack_ParStt








//----------begin of function PndTrkCleanup::SeparateInnerOuterParallel

void PndTrkCleanup::SeparateInnerOuterParallel(

	// input
	Short_t nHits,
	Short_t *ListHits,
	Double_t info[][7],
	Double_t R_STT_INNER_PAR_MAX,

	// output
	Short_t *nInnerHits,
	Short_t *ListInnerHits,
	Short_t *nOuterHits,
	Short_t *ListOuterHits,

	Short_t *nInnerHitsLeft,
	Short_t *ListInnerHitsLeft,
	Short_t *nInnerHitsRight,
	Short_t *ListInnerHitsRight,

	Short_t *nOuterHitsLeft,
	Short_t *ListOuterHitsLeft,
	Short_t *nOuterHitsRight,
	Short_t *ListOuterHitsRight
	)
{

	Short_t ihit;

	Double_t r;

//   separation of inner Parallel Stt hits from outer Parallel Stt hits.

	*nInnerHits=0;
	*nInnerHitsLeft=0;
	*nInnerHitsRight=0;
	*nOuterHits=0;
	*nOuterHitsLeft=0;
	*nOuterHitsRight=0;
	for(ihit=0  ;ihit<nHits;ihit++){
		r = sqrt( info[ListHits[ihit]][0]*info[ListHits[ihit]][0] +
			 info[ListHits[ihit]][1]*info[ListHits[ihit]][1]);
		// the value 2.*RStrawDetectorParMax/sqrt(3.) is because RStrawDetectorParMax
		// is an Apotema !
		if(r>R_STT_INNER_PAR_MAX ){	// outer Parallel hit.
			ListOuterHits[ *nOuterHits ] = ListHits[ihit];
			(*nOuterHits)++;
			if(info[ListHits[ihit]][0]<0.){
				ListOuterHitsLeft[ *nOuterHitsLeft ] = ListHits[ihit];
				(*nOuterHitsLeft)++;
			} else {
				ListOuterHitsRight[ *nOuterHitsRight ] = ListHits[ihit];
				(*nOuterHitsRight)++;
			}
		}else{
			ListInnerHits[ *nInnerHits ] = ListHits[ihit];
			(*nInnerHits)++;
			if(info[ListHits[ihit]][0]<0.){
				ListInnerHitsLeft[ *nInnerHitsLeft ] = ListHits[ihit];
				(*nInnerHitsLeft)++;
			} else {
				ListInnerHitsRight[ *nInnerHitsRight ] = ListHits[ihit];
				(*nInnerHitsRight)++;
			}
		}
	}

}


//----------end of function PndTrkCleanup::SeparateInnerOuterParallel




//----------begin of function PndTrkCleanup::SeparateInnerOuterRightLeftAxialStt

void PndTrkCleanup::SeparateInnerOuterRightLeftAxialStt(

	// input
	Double_t info[][7],
	Short_t *ListHits,
	Short_t nHits,
	Double_t R_STT_INNER_PAR_MAX,

	// output

	Short_t *ListInnerHitsLeft,
	Short_t *ListInnerHitsRight,
	Short_t *ListOuterHitsLeft,
	Short_t *ListOuterHitsRight,
	Short_t *nInnerHitsLeft,
	Short_t *nInnerHitsRight,
	Short_t *nOuterHitsLeft,
	Short_t *nOuterHitsRight
	)
{

	Short_t ihit;

	Double_t r;

//   separation of inner Parallel Stt hits from outer Parallel Stt hits.

	*nInnerHitsLeft=0;
	*nInnerHitsRight=0;
	*nOuterHitsLeft=0;
	*nOuterHitsRight=0;
	for(ihit=0  ;ihit<nHits;ihit++){
		r = sqrt( info[ListHits[ihit]][0]*info[ListHits[ihit]][0] +
			 info[ListHits[ihit]][1]*info[ListHits[ihit]][1]);
		// the value 2.*RStrawDetectorParMax/sqrt(3.) is because RStrawDetectorParMax
		// is an Apotema !
		if(r>R_STT_INNER_PAR_MAX ){	// outer Parallel hit.
			if(info[ListHits[ihit]][0]<0.){
				ListOuterHitsLeft[ *nOuterHitsLeft ] = ListHits[ihit];
				(*nOuterHitsLeft)++;
			} else {
				ListOuterHitsRight[ *nOuterHitsRight ] = ListHits[ihit];
				(*nOuterHitsRight)++;
			}
		}else{
			if(info[ListHits[ihit]][0]<0.){
				ListInnerHitsLeft[ *nInnerHitsLeft ] = ListHits[ihit];
				(*nInnerHitsLeft)++;
			} else {
				ListInnerHitsRight[ *nInnerHitsRight ] = ListHits[ihit];
				(*nInnerHitsRight)++;
			}
		} // end of if(r>R_STT_INNER_PAR_MAX )
	}  // end of for(ihit=0  ;ihit<nHits;ihit++)


	return;
}


//----------end of function PndTrkCleanup::SeparateInnerOuterRightLeftAxialStt



//----------begin of function PndTrkCleanup::SttParalCleanup

 bool PndTrkCleanup::SttParalCleanup(
	Double_t ApotemaInnerParMax,
	Double_t ApotemaMinOuterPar,
	Short_t  Charge,
	Double_t FI0,
	Double_t FiLimitAdmissible,
	Double_t GAP,
	Double_t info[][7],
	int	istampa,
	int	IVOLTE,
	Short_t *Listofhits,
	Short_t nHits,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t RStrawDetMax, // radius of circle encompassing ALL
				// the straw detector;
	Double_t RStrawDetMin,
	Double_t Start[3],
	Double_t STRAWRADIUS
	)
{


// this method does 3 things :
//
//	1)  finds the entrance and exit points in the STT parallel volumes of the current track;
//	2)  eliminates from the track hit list possible spurious hits that are not encompassed
//		by the entrance and exit point;
//	3)  eliminates the tracks if the hit sequence is not continuous enough.


	bool
		flaggo;

	Short_t flagInnerSttR,
		flagOuterSttR,
		flagInnerSttL,
		flagOuterSttL,
		flagOutStt;

	Short_t	enne,
			i,
			ihit,
			ipurged,
			islack,
			nintersections,
			nnn,
			nInnerHits,
			nInnerHitsLeft,
			nInnerHitsRight,
			nOuter,
			nOuterHits,
			nOuterHitsLeft,
			nOuterHitsRight,
			nIntersections[2],
			ListHits[nHits],
			ListInnerHits[nHits],
			ListInnerHitsLeft[nHits],
			ListInnerHitsRight[nHits],
			ListOuterHits[nHits],
			ListOuterHitsLeft[nHits],
			ListOuterHitsRight[nHits];

	Double_t	epsilonTheta,
			fi,
			r,
			aux[2],
			Xcross[2],
			Ycross[2],
			XcrossL[2],
			YcrossL[2],
			XcrossR[2],
			YcrossR[2],
			XcrossOut[2],
			YcrossOut[2],
			XintersectionList[7], // there is also the last boundary FiLimitAdmissible
			YintersectionList[7]; // take into account and the two possible
					      // intersections with the external circle.

	const Double_t PI = 3.141592654;

	islack=1;  // uncertainty allowed in the # of straws that should be hit in a given part
			// of the Stt detector.


//------------------------
//  elimination of hits outside the physical FI range (FiLimitAdmissible) due to finite length of
//  Straws.


if(istampa>1) {
	cout<<"SttParalCleanup, evento n. "<<IVOLTE<<", n. || in ingresso "<<
	nHits<<", prima di essere purgati."<<endl; }

	epsilonTheta = STRAWRADIUS/Rr;  // some extra slac for being conservative.
	for(i=0, ipurged=0; i< nHits; i++){

		fi = atan2( info[Listofhits[i]][1]-Oyy,info[Listofhits[i]][0]-Oxx);
		if (fi<0.) fi+=2.*PI;

	  if(Charge <0) {
		if( fi > FI0){
			if( fi>FiLimitAdmissible+epsilonTheta) continue;
		} else {
			fi += 2.*PI;
			if( fi >FiLimitAdmissible+epsilonTheta ) continue; 
		}  // end of  if( fi > FI0)
	  } else {  // continuation of  if(Charge <0)
		if( fi > FI0){
			fi -= 2.*PI;
		}  // end of  if( fi > FI0)
		if (fi < FiLimitAdmissible-epsilonTheta) continue;
	  } // end of if(Charge <0)

	  ListHits[ipurged]=Listofhits[i];
	  ipurged++;
	}  // end of    for(i=0, ipurged=0; i< nHits; i++)

	nHits = ipurged;
if(istampa>1) {
	cout<<"SttParalCleanup, evento n. "<<IVOLTE<<", n. || hits "<<
	nHits<<", dopo di essere purgati."<<endl; }

	if(nHits==0){
		nInnerHits=0;
		nInnerHitsRight=0;
		nInnerHitsLeft=0;
		nOuterHits=0;
		nOuterHitsRight=0;
		nOuterHitsLeft=0;
		// don't discard track yet, see if it should have parallel hits.
	} else {

//------------------
//   separation of inner Parallel Stt hits from outer Parallel Stt hits.

	SeparateInnerOuterParallel(

				// input
				nHits,
				ListHits,
				info,
				2.*ApotemaInnerParMax/sqrt(3.),

				// output
				&nInnerHits,
				ListInnerHits,
				&nOuterHits,
				ListOuterHits,

				&nInnerHitsLeft,
				ListInnerHitsLeft,
				&nInnerHitsRight,
				ListInnerHitsRight,

				&nOuterHitsLeft,
				ListOuterHitsLeft,
				&nOuterHitsRight,
				ListOuterHitsRight
				);

if(istampa>1) {
	cout<<"SttParalCleanup, evento n. "<<IVOLTE
	<<", dopo separate INNER/OUTER, n. || inner hits left "<<
	nInnerHitsLeft<<", right : "<< nInnerHitsRight <<endl; }
//------------------------------------------
	// find the entrance and exit of the track in the Inner Left Parallel Straw region.
	// This region is bounded by two Hexagons, and it has the target gap in the middle.

	// flag meaning :
	// -1 -->  track outside outer perimeter;
	// 0 -->  at least 1 intersection with polygon, therefore a possible entry and an exit;
	// 1 -->  track contained completely between the two polygons;

	} // end of if(nHits==0)

//	first of all, find possible intersection points with outer circle encompassing
//	the Stt system.

	//  class with all the geometry calculations:
	PndTrkCTGeometryCalculations  GeometryCalculator;

	flagOutStt = GeometryCalculator.FindIntersectionsOuterCircle(
				Oxx,
				Oyy,
				Rr,
				RStrawDetMax,
				XcrossOut,
				YcrossOut
				);


//	intersection with Inner Section.

	flagInnerSttL= GeometryCalculator.FindTrackEntranceExitbiHexagonLeft(
				GAP,
				Oxx,
				Oyy,
				Rr,
				Charge,
				Start,
				RStrawDetMin,
				ApotemaInnerParMax,
				XcrossL,
				YcrossL
				);
	// find the entrance and exit of the track in the Inner Right Parallel Straw region.
	// This region is bounded by two Hexagons, and it has the target gap in the middle.

	flagInnerSttR= GeometryCalculator.FindTrackEntranceExitbiHexagonRight(
				GAP,
				Oxx,
				Oyy,
				Rr,
				Charge,
				Start,
				RStrawDetMin,
				ApotemaInnerParMax,
				XcrossR,
				YcrossR
				);



if(istampa>1) {
	cout<<"SttParalCleanup, evento n. "<<IVOLTE<<", flagInnerSttR (-1,0,1) = "<<flagInnerSttR
	<<", flagInnerSttL "<<flagInnerSttL<<", flagOutStt "<<flagOutStt<<
	", FI0  "<<FI0<<",  FiLimitAdmissible "<<FiLimitAdmissible<<endl; }

//-----------------

//	working in the hypothesis that his is a track coming from Vertex at (0,0).

	// case when track is contained either in Left of Right Inner Stt Parallel sections.
	if( flagInnerSttL == 1 || flagInnerSttR == 1 ){
		return false;
	}

	// if a track enters only marginally in the skew volumes, define the track
	// as non-entering and the corresponding flag to -1.

	if( flagInnerSttR == 0 && (XcrossR[0]-XcrossR[1])*(XcrossR[0]-XcrossR[1])+
			(YcrossR[0]-YcrossR[1])*(YcrossR[0]-YcrossR[1])
			< 9.*STRAWRADIUS*STRAWRADIUS ) flagInnerSttR=-1;

	if( flagInnerSttL == 0 && (XcrossL[0]-XcrossL[1])*(XcrossL[0]-XcrossL[1])+
			(YcrossL[0]-YcrossL[1])*(YcrossL[0]-YcrossL[1])
			< 9.*STRAWRADIUS*STRAWRADIUS ) flagInnerSttR=-1;

	// case when track is outside both Inner Stt Parallel sections.
	if( flagInnerSttL == -1 && flagInnerSttR == -1 ){
	} else {



	// case when the track crosses both InnerLeft and InnerRight.
	// Decide what was crossed first and ignore the other part.
	// This may be changed in the future.
	if( flagInnerSttL == 0 && flagInnerSttR == 0 ) {
		XintersectionList[0]=XcrossL[0];
		YintersectionList[0]=YcrossL[0];
		XintersectionList[1]=XcrossL[1];
		YintersectionList[1]=YcrossL[1];
		XintersectionList[2]=XcrossR[0];
		YintersectionList[2]=YcrossR[0];
		XintersectionList[3]=XcrossR[1];
		YintersectionList[3]=YcrossR[1];
		nintersections=4;
		GeometryCalculator.ChooseEntranceExitbis(
			Oxx,
			Oyy,
			Charge,
			FI0,
			nintersections,// n. intersection in input.
			XintersectionList,
			YintersectionList,
			Xcross,	// output
			Ycross	// output
				);
		// now decide which sector was crossed first.
		if( (fabs(XcrossL[0]-Xcross[0])<1.e-5&&fabs(YcrossL[0]-Ycross[0])<1.e-5)
					||
		    (fabs(XcrossL[1]-Xcross[0])<1.e-5&&fabs(YcrossL[1]-Ycross[0])<1.e-5)
		   ) {  // the Left part was entered first.
		   flagInnerSttR=-1;
		} else {  // the Right part was entered first.
		   flagInnerSttL=-1;
		} // end of  if( (fabs(XcrossL[0]-Xcross.....
	}  // end of if( (flagInnerSttL == 0 && flagInnerSttR = 0 )



//---------  the other 2 possible cases.

	if( flagInnerSttL == 0){
	   nnn=nInnerHitsLeft;
	   for(i=0;i<2;i++){
		XintersectionList[i]=XcrossL[i];
		YintersectionList[i]=YcrossL[i];
	   }
	} else {  // continuation of if( (flagInnerSttL == 0), case in which
		  // flagInnerSttR == 0.
	   nnn=nInnerHitsRight;
	   for(i=0;i<2;i++){
		XintersectionList[i]=XcrossR[i];
		YintersectionList[i]=YcrossR[i];
	   }
	}  // end of   if( (flagInnerSttL == 0)



	nintersections=2;
	if(fabs(FiLimitAdmissible-FI0) < 2.*PI){  // in this case the point
			// corresponding to FiLimitAdmissible can play a role in the
			// determination of the limiting points of the hits.
			aux[0]=Oxx+Rr*cos(FiLimitAdmissible);
			aux[1]=Oyy+Rr*sin(FiLimitAdmissible);
			XintersectionList[2]=aux[0];
			YintersectionList[2]=aux[1];
			nintersections++;
	}  // end of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)

	if( flagOutStt ==0){// 2 intersections with outer Stt circle.
				XintersectionList[nintersections]=XcrossOut[0];
				XintersectionList[nintersections+1]=XcrossOut[1];
				YintersectionList[nintersections]=YcrossOut[0];
				YintersectionList[nintersections+1]=YcrossOut[1];
				nintersections +=2;
	}


	GeometryCalculator.ChooseEntranceExitbis(
				Oxx,
				Oyy,
				Charge,
				FI0,
				nintersections,// n. intersection in input.
				XintersectionList,
				YintersectionList,
				Xcross,	// output
				Ycross	// output
				);

	if(fabs(FiLimitAdmissible-FI0) < 2.*PI){
			// case when this track exit in Z before having the possibility
			// of hitting the Stt parallel inner section.
		if( fabs(aux[0]-Xcross[0])<1.e-5&& fabs(aux[1]-Ycross[0])<1.e-5 ){
			return true;
		}
		flaggo=true;
		if( flagOutStt ==0 // 2 intersections with outer Stt circle.
		   // case when this track exits the Stt outer circle without
		   // hitting the Stt parallel inner section (for instance the track
		   &&  ((fabs(XcrossOut[0]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[0]-Ycross[0])<1.e-5 )
					||
				(fabs(XcrossOut[1]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[1]-Ycross[0])<1.e-5 )
				)
				){
			flaggo=false ;
		} else { // continuation of  if( flagOutStt ==0)

			// most usual case when track crossed the Inner parallel Stt.
			if (nnn == 0) return false;

			// if the exit point is actually given by FiLimitAdmissible, then allow
			// an extra uncertainty in the # Stt hit that must be present;
			// this is done because FiLimitAdmissible is not a very precise number.
			if( fabs(aux[0]-Xcross[1])<1.e-5&& fabs(aux[1]-Ycross[1])<1.e-5 ){
				islack = 3;
			}
		} //  end of  if( flagOutStt ==0)

	} else { // continuation of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)
		if( flagOutStt ==0){// 2 intersections with outer Stt circle.
			   // case when this track exits the Stt outer circle without
			   // hitting the Stt parallel inner section (for instance the track
			   flaggo=true;
			   if( (fabs(XcrossOut[0]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[0]-Ycross[0])<1.e-5 )
					||
				(fabs(XcrossOut[1]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[1]-Ycross[0])<1.e-5 )
				){
				flaggo=false;
			   }
		} else { // continuation of  if( flagOutStt ==0)

			// most usual case when track crossed the Inner parallel Stt.
			if (nnn == 0) return false;
		}  // end of  if( flagOutStt ==0)
	}  // end of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)



	if(flaggo){

//-------------  cleanup of the spurious tracks first using the inner parallel straws.

if(istampa>1) {
	cout<<"SttParalCleanup, evento n. "<<IVOLTE<<", prima di BadTrack_ParStt; Xin Inner "<<
	Xcross[0]<<", Yin Inner "<<Ycross[0]<<
	",  Xout Inner "<<Xcross[1]<<", Yout Inner "<<Ycross[1]<<endl; }


	// at this point the n. of inner hits cannot be 0 for a true track.
	if ( BadTrack_ParStt(
			Oxx,
			Oyy,
			Rr,
			STRAWRADIUS,
			Charge,
			Xcross,  // Xcross[0]=point of entrance; Xcross[1]=point of exit.
			Ycross,
			nInnerHits,
			ListInnerHits,
			info,
			istampa,
			2.*2*STRAWRADIUS,	//  cut of proximity between hits.
			1,	// maximum allowed # consecutive hits with distance > cut.
			islack // uncertainty allowed as far as the n. of hits that should be present.
					)
	   ){

		return false;
	}

if(istampa>1) cout<<"uscito da BadTrack_ParStt.\n";

//-----------------------------------------------------
	}  // end of if(flaggo)
	} // end of if( flagInnerSttL == -1 && flagInnerSttR == -1 )
//outer: ;

	islack=1;   // reset the extra uncertainty in the # Stt.




//------------ Outer Parallel Stt hits section.
	// find the entrance and exit of the track in the Outer Parallel Straw region, Left side.
	// This region is bounded by a Hexagon (inner), a Circle (outer) and it has
	// the target gap in the middle.

	//  Returns -1 if there are 0 or 1 intersections, 0 if there are at least 2 intersections.
		flagOuterSttL= GeometryCalculator.FindTrackEntranceExitHexagonCircleLeft(
				Oxx,
				Oyy,
				Rr,
				Charge,
				Start,
				ApotemaMinOuterPar,
				RStrawDetMax,
				GAP,
				XcrossL,
				YcrossL
				);



//------------
	// find the entrance and exit of the track in the Outer Parallel Straw region, Right side.
	// This region is bounded by a Hexagon (inner), a Circle (outer) and it has
	// the target gap in the middle.
		flagOuterSttR= GeometryCalculator.FindTrackEntranceExitHexagonCircleRight(
				Oxx,
				Oyy,
				Rr,
				Charge,
				Start,
				ApotemaMinOuterPar,
				RStrawDetMax,
				GAP,
				XcrossR,
				YcrossR
				);


if(istampa>1) {
	cout<<"SttParalCleanup, evento n. "<<IVOLTE<<", flagOuterSttR (-1,0,1) = "<<flagOuterSttR
	<<", flagOuterSttL "<<flagOuterSttL<<endl; }


//--------------------------------

	// case when track is contained either in Left of Right Outer Stt Parallel sections.
	if( flagOuterSttL == 1 || flagOuterSttR == 1 ){
		return false;
	}


	// if a track enters only marginally in the skew volumes, define the track
	// as non-entering and the corresponding flag to -1.

	if( flagInnerSttR == 0 && (XcrossR[0]-XcrossR[1])*(XcrossR[0]-XcrossR[1])+
			(YcrossR[0]-YcrossR[1])*(YcrossR[0]-YcrossR[1])
			< 9.*STRAWRADIUS*STRAWRADIUS ) flagInnerSttR=-1;

	if( flagInnerSttL == 0 && (XcrossL[0]-XcrossL[1])*(XcrossL[0]-XcrossL[1])+
			(YcrossL[0]-YcrossL[1])*(YcrossL[0]-YcrossL[1])
			< 9.*STRAWRADIUS*STRAWRADIUS ) flagInnerSttR=-1;


	// case when track is outside both Outer Stt Parallel sections.
	if( flagOuterSttL == -1 && flagOuterSttR == -1 ){
		return true ;
	}


	// case when the track crosses both OuterLeft and OuterRight.
	// Decide what was crossed first and ignore the other part.
	// This may be changed in the future.
	if( flagOuterSttL == 0 && flagOuterSttR == 0 ) {
		XintersectionList[0]=XcrossL[0];
		YintersectionList[0]=YcrossL[0];
		XintersectionList[1]=XcrossL[1];
		YintersectionList[1]=YcrossL[1];
		XintersectionList[2]=XcrossR[0];
		YintersectionList[2]=YcrossR[0];
		XintersectionList[3]=XcrossR[1];
		YintersectionList[3]=YcrossR[1];
		nintersections=4;
		GeometryCalculator.ChooseEntranceExitbis(
			Oxx,
			Oyy,
			Charge,
			FI0,
			nintersections,// n. intersection in input.
			XintersectionList,
			YintersectionList,
			Xcross,	// output
			Ycross	// output
				);
		// now decide which sector was crossed first.
		if( (fabs(XcrossL[0]-Xcross[0])<1.e-5&&fabs(YcrossL[0]-Ycross[0])<1.e-5)
					||
		    (fabs(XcrossL[1]-Xcross[0])<1.e-5&&fabs(YcrossL[1]-Ycross[0])<1.e-5)
		   ) {  // the Left part was entered first.
		   flagOuterSttR=-1;
		} else {  // the Right part was entered first.
		   flagOuterSttL=-1;
		} // end of  if( (fabs(XcrossL[0]-Xcross.....
	}  // end of if( (flagInnerSttL == 0 && flagInnerSttR = 0 )

//-------------stampe.
if(istampa>1) {
cout<<"in SttParalCleanup Outer, caso traccia entra in L and R outer. Dopo scelta, flagOuterSttR "
	<<flagOuterSttR<<", flagOuterSttL "<<flagOuterSttL<<endl;
}
//-------------fine stampe.

	if( flagOuterSttL == 0){
	   nnn=nOuterHitsLeft;
	   for(i=0;i<2;i++){
		XintersectionList[i]=XcrossL[i];
		YintersectionList[i]=YcrossL[i];
	   }
	} else {  // continuation of if( (flagOuterSttL == 0), case in which
		  // flagOuterSttR == 0.
	   nnn=nOuterHitsRight;
	   for(i=0;i<2;i++){
		XintersectionList[i]=XcrossR[i];
		YintersectionList[i]=YcrossR[i];
	   }
	}  // end of   if( (flagOuterSttL == 0)


//-------------stampe.
if(istampa>1) {
cout<<"in SttParalCleanup Outer, nhit considerati "<< nnn <<endl;
}
//-------------fine stampe.


	nintersections=2;
	if(fabs(FiLimitAdmissible-FI0) < 2.*PI){  // in this case the point
			// corresponding to FiLimitAdmissible can play a role in the
			// determination of the limiting points of the hits.
			XintersectionList[2]=aux[0];
			YintersectionList[2]=aux[1];
			nintersections++;
//-------------stampe.
if(istampa>1) {
cout<<"in SttParalCleanup Outer, caso in cui  FiLimitAdmissible = "<< FiLimitAdmissible
<<"  conta!" <<endl;
}
//-------------fine stampe.
	}  // end of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)

	if( flagOutStt ==0){// 2 intersections with outer Stt circle.
				XintersectionList[nintersections]=XcrossOut[0];
				XintersectionList[nintersections+1]=XcrossOut[1];
				YintersectionList[nintersections]=YcrossOut[0];
				YintersectionList[nintersections+1]=YcrossOut[1];
				nintersections +=2;
	}
//-------------stampe.
if(istampa>1) {
cout<<"in SttParalCleanup Outer, prima di  ChooseEntranceExitbis, nintersections "
<< nintersections<<" e loro lista :"<<endl;
	for(int ic=0;ic<nintersections;ic++){
		cout<<"\tX["<<ic<<"] = "<<XintersectionList[ic]
		<<", Y["<<ic<<"] = "<<YintersectionList[ic]<<endl;
	}
}
//-------------fine stampe.

	GeometryCalculator.ChooseEntranceExitbis(
				Oxx,
				Oyy,
				Charge,
				FI0,
				nintersections,// n. intersection in input.
				XintersectionList,
				YintersectionList,
				Xcross,	// output
				Ycross	// output
				);
//-------------stampe.
if(istampa>1) {
cout<<"in SttParalCleanup Outer, dopo di  ChooseEntranceExitbis, Xin"
<< Xcross[0]<<", Yin "<< Ycross[0]<<", Xout " << Xcross[1]<<", Yout "<< Ycross[1]
<<endl;
}
//-------------fine stampe.


	if(fabs(FiLimitAdmissible-FI0) < 2.*PI){
			// case when this track exit in Z before having the possibility
			// of hitting the Stt parallel inner section.

			// case when this track exit in Z before having the possibility
			// of hitting the Stt parallel inner section.
		if( fabs(aux[0]-Xcross[0])<1.e-5&& fabs(aux[1]-Ycross[0])<1.e-5 ){
			return true;
		}

		if( flagOutStt ==0){// 2 intersections with outer Stt circle.
		   // case when this track exits the Stt outer circle without
		   // hitting the Stt parallel Outer section (for instance the track
		   if( (fabs(XcrossOut[0]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[0]-Ycross[0])<1.e-5 )
					||
				(fabs(XcrossOut[1]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[1]-Ycross[0])<1.e-5 )
				){
				//nOuterHits=0; // eliminate all the hits from hit list.
				//nOuterHitsRight=0;
				//nOuterHitsLeft=0;
				return true ;
		   }
		} // end of  if( flagOutStt ==0)


		// most usual case when track crossed the Outer parallel Stt.
		if (nnn == 0) return false;

		// if the exit point is actually given by FiLimitAdmissible, then allow
		// an extra uncertainty in the # Stt hit that must be present;
		// this is done because FiLimitAdmissible is not a very precise number.
		if( fabs(aux[0]-Xcross[1])<1.e-5&& fabs(aux[1]-Ycross[1])<1.e-5 ){
			islack = 3;
		}

	} else { // continuation of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)

		if( flagOutStt ==0){// 2 intersections with outer Stt circle.
			   // case when this track exits the Stt outer circle without
			   // hitting the Stt parallel Outer section (for instance the track
			   if( (fabs(XcrossOut[0]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[0]-Ycross[0])<1.e-5 )
					||
				(fabs(XcrossOut[1]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[1]-Ycross[0])<1.e-5 )
				){
				//nOuterHits=0; // eliminate all the hits from hit list.
				//nOuterHitsRight=0;
				//nOuterHitsLeft=0;
				return true ;
			   }
		} // end of  if( flagOutStt ==0)


		// most usual case when track crossed the Outer parallel Stt.
		if (nnn == 0) return false;

	}  // end of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)




//-------------------- stampe
if(istampa>=2){
cout<<"SttParalCleanup, OUTER, caso R || L true, IVOLTE = "<<IVOLTE<<"\n\t Xcross[0] "
<< Xcross[0]<<", Ycross[0] " <<Ycross[0]<<"\n\t Xcross[1] "
<< Xcross[1]<<", Ycross[1] " <<Ycross[1]<<" e charge = "<<Charge<<", FiLimitAdmissible "
<<FiLimitAdmissible<<" (X="<<Oxx+Rr*cos(FiLimitAdmissible)
  <<", Y="<< Oyy+Rr*sin(FiLimitAdmissible)<<")." <<endl;
}
//-------------------fine stampe



//  cleanup of the spurious tracks now using the outer parallel straws.


	// at this point nOuterHits cannot be 0 for a real track.
	if ( BadTrack_ParStt(
			Oxx,
			Oyy,
			Rr,
			STRAWRADIUS,
			Charge,
			Xcross,  // Xcross[0]=point of entrance; Xcross[1]=point of exit.
			Ycross,
			nOuterHits,
			ListOuterHits,
			info,
			istampa,
			2.*2.*STRAWRADIUS,	//  cut of proximity between hits.
			1,	// maximum allowed # consecutive hits with distance > cut.
			islack // uncertainty allowed as far as the n. of hits that should be present.
					)
	   ) return false;

//      }  // end of  if(nOuterHits==0)

//----------------------------------------------------------------------------

// finito: ;

	// if the code comes here it means that the track is acceptable.

//	nHits = nOuterHits+nInnerHits;

	return true;

};



//----------end of function PndTrkCleanup::SttParalCleanup

//----------begin of function PndTrkCleanup::SttSkewCleanup
bool PndTrkCleanup::SttSkewCleanup(
	Double_t ApotemaMaxSkew,
	Double_t ApotemaMinSkew,
	Short_t  Charge,
	Double_t cut, // cut distance (in cm).
	Double_t FI0,
	Double_t FiLimitAdmissible,
	Double_t GAP,
	Double_t info[][7],
	int istampa,
	int IVOLTE,
	Short_t *Listofhits,
	Short_t maxnum, // max number allowed of failures to pass the cut.
	int MAXSTTHITS,
	Short_t nHits,
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,	
	Double_t RStrawDetMax,
	Double_t *S,
	Double_t Start[3],
	Double_t STRAWRADIUS
	)

{

	bool ConsiderLastHit;

	Short_t flagSttL,
		flagSttR,
		flagOutStt;

	Short_t	i,
			ipurged,
			ibad,
			islack,
			nHitsLeft,
			nHitsRight,
			nintersections,
			ninside,
			nnn,
			nIntersections[2],
			ListHits[nHits],
			ListHitsRight[nHits],
			ListHitsLeft[nHits];

	Double_t	cut2,
			epsilonTheta,
			fi,
			FiStart,
			length,
			r,
			Sprevious,
			aux[2],
			Distance[MAXSTTHITS+1],
			Xcross[2],
			Ycross[2],
			XcrossL[2],
			YcrossL[2],
			XcrossR[2],
			YcrossR[2],
			XcrossOut[2],
			YcrossOut[2],
			XintersectionList[5], // second index =0 --> inner Hexagon, =1 --> outer.
			YintersectionList[5]; // first index : all the possible intersections
						  // (up to 12 intersections).
	const Double_t PI = 3.141592654;

	//  class with all the geometry calculations :
	PndTrkCTGeometryCalculations GeometryCalculator;



	cut2=cut*cut;
	islack=1;// uncertainty allowed as far as
		// the n. of hits that should be present in a given section of the Stt track.


//------------------------
//  elimination of hits outside the physical FI range (FiLimitAdmissible) due to finite length of
//  Straws.

	epsilonTheta = STRAWRADIUS/Rr;  // some extra slac for being conservative.

if(istampa>1)
cout<<"\n\nevt "<<IVOLTE<<", FI0 "<<FI0<<", Filimit "
<< FiLimitAdmissible+epsilonTheta <<", Ox "<<Oxx<<", Oy "<<Oyy<<", R "<<Rr<<endl;

	for(i=0, ipurged=0; i< nHits; i++){
	  fi = S[i];

if(istampa>1)cout<<"\thit // n. "<<Listofhits[i]<<", fi "<<fi<<endl;

	  if(Charge <0) {
		if(fi > FI0){
			if( fi>FiLimitAdmissible+epsilonTheta) continue;
		} else {
			fi += 2.*PI;
			if( fi > FiLimitAdmissible+epsilonTheta ) continue; 
		}  // end of  if( fi > FI0)
	  } else {  // continuation of  if(Charge <0)
		if( fi > FI0){
			fi -= 2.*PI;
		}  // end of  if( fi > FI0)
		if (fi < FiLimitAdmissible-epsilonTheta) continue;
	  } // end of if(Charge <0)
if(istampa>1)cout<<"in SttSkewCleanup : hit preso!"<<endl;

	  ListHits[ipurged]=Listofhits[i];
	  S[ipurged]=S[i];
	  ipurged++;
	}  // end of    for(i=0, ipurged=0; i< nHits; i++)

if(istampa>1)cout<<"in SttSkewCleanup : hit skew prima di purga = "
<<nHits<<", dopo purga "<<ipurged<<endl;

	nHits = ipurged;
	if(nHits==0){ // don't discard track yet, see if it should have
				// skew hits or not.
		nHitsRight=nHitsLeft=0;
//		goto jampa;
	} else {



	// separation of Right and Left Skew hits.

	nHitsRight=nHitsLeft=0;
	for(i=0;i< nHits; i++){
		if(info[ListHits[i]][0]<0.){
			ListHitsLeft[nHitsLeft]=ListHits[i];
			nHitsLeft++;
		}else{
			ListHitsRight[nHitsRight]=ListHits[i];
			nHitsRight++;
		}
	}

if(istampa>1)cout<<"in SttSkewCleanup : n. hit skew Left = "
<<nHitsLeft<<", right "<< nHitsRight  <<endl;

	} // end of if(nHits==0)
//jampa: ;
//	first of all, find possible intersection points with outer circle encompassing
//	the Stt system.

	flagOutStt = GeometryCalculator.FindIntersectionsOuterCircle(
				Oxx,
				Oyy,
				Rr,
				RStrawDetMax,
				XcrossOut,
				YcrossOut
				);

//------------------------------------------
	// find the entrance and exit of the track in the Skew Straw region.
	// This region is bounded by two Hexagons, and it has the target gap
	// in the middle. So Left is the left looking from downstream.

	flagSttL=GeometryCalculator.FindTrackEntranceExitbiHexagonLeft(
				GAP,
				Oxx,
				Oyy,
				Rr,
				Charge,
				Start,
		ApotemaMinSkew,
		ApotemaMaxSkew, // Apotema is the distance of a Hexagonal side from (0,0)
				XcrossL,
				YcrossL
				);


	flagSttR= GeometryCalculator.FindTrackEntranceExitbiHexagonRight(
				GAP,
				Oxx,
				Oyy,
				Rr,
				Charge,
				Start,
		ApotemaMinSkew,
		ApotemaMaxSkew, // Apotema is the distance of a Hexagonal side from (0,0)
				XcrossR,
				YcrossR
				);

	// find the entrance and exit of the track in the Skew Straw region.
	// This region is bounded by two Hexagons, and it has the target gap in the middle.

if(istampa>1)cout<<"in SttSkewCleanup : flagLeft (-1,0,1) = "<<flagSttL
<<", right "<< flagSttR  <<endl;
	if (flagSttR == 1 || flagSttL == 1 ) { // the trajectory is contained completely
					// in  the Right or Left Skew section, reject!
		return false ;
	}

	// if a track enters only marginally in the skew volumes, define the track
	// as non-entering and the corresponding flag to -1.

	if( flagSttR == 0 && (XcrossR[0]-XcrossR[1])*(XcrossR[0]-XcrossR[1])+
			(YcrossR[0]-YcrossR[1])*(YcrossR[0]-YcrossR[1])
			< 16.*STRAWRADIUS*STRAWRADIUS ){
		flagSttR=-1;
if(istampa>1)cout<<"in SttSkewCleanup : distanza entrata-uscita<4*STRAWRADIUS,flagSttR set at -1!\n";
	}

	if( flagSttL == 0 && (XcrossL[0]-XcrossL[1])*(XcrossL[0]-XcrossL[1])+
			(YcrossL[0]-YcrossL[1])*(YcrossL[0]-YcrossL[1])
			< 16.*STRAWRADIUS*STRAWRADIUS ){
		flagSttR=-1;
if(istampa>1)cout<<"in SttSkewCleanup : distanza entrata-uscita<4*STRAWRADIUS,flagSttL set at -1!\n";
	}

	if (flagSttR != 0 && flagSttL != 0 ) {
		//nHits=0;
		if(istampa>1)cout<<"in SttSkewCleanup : flagSttR = "<<flagSttR
		<<", e  flagSttL = "<<flagSttL<<", exit con true!\n";
		return true; // don't discard track because it may have Mvd hits anyway
				// and/or they can have Inner Parallel hits.
	}


	// case when the track crosses both SkewLeft and SkewRight.
	// Decide what was crossed first and ignore the other part.
	// This may be changed in the future.



	if( flagSttL == 0 && flagSttR == 0 ) { // crosses both right and left sections.
		XintersectionList[0]=XcrossL[0];
		YintersectionList[0]=YcrossL[0];
		XintersectionList[1]=XcrossL[1];
		YintersectionList[1]=YcrossL[1];
		XintersectionList[2]=XcrossR[0];
		YintersectionList[2]=YcrossR[0];
		XintersectionList[3]=XcrossR[1];
		YintersectionList[3]=YcrossR[1];
		nintersections=4;
		GeometryCalculator.ChooseEntranceExitbis(
			Oxx,
			Oyy,
			Charge,
			FI0,
			nintersections,// n. intersection in input.
			XintersectionList,
			YintersectionList,
			Xcross,	// output
			Ycross	// output
				);
		// now decide which sector was crossed first.
		if( (fabs(XcrossL[0]-Xcross[0])<1.e-5&&fabs(YcrossL[0]-Ycross[0])<1.e-5)
					||
		    (fabs(XcrossL[1]-Xcross[0])<1.e-5&&fabs(YcrossL[1]-Ycross[0])<1.e-5)
		   ) {  // the Left part was entered first.
		   flagSttR=-1;
		} else {  // the Right part was entered first.
		   flagSttL=-1;
		} // end of  if( (fabs(XcrossL[0]-Xcross.....
	}  // end of if( (flagInnerSttL == 0 && flagInnerSttR = 0 )



//---------  the other 2 possible cases.



	if( flagSttL == 0){
	   nnn=nHitsLeft;
	   for(i=0;i<2;i++){
		XintersectionList[i]=XcrossL[i];
		YintersectionList[i]=YcrossL[i];
	   }
	} else {  // continuation of if( (flagSttL == 0), case in which
		  // flagSttR == 0.
	   nnn=nHitsRight;
	   for(i=0;i<2;i++){
		XintersectionList[i]=XcrossR[i];
		YintersectionList[i]=YcrossR[i];
	   }
	}  // end of   if( (flagInnerSttL == 0)

	nintersections=2;
	if(fabs(FiLimitAdmissible-FI0) < 2.*PI){  // in this case the point
			// corresponding to FiLimitAdmissible can play a role in the
			// determination of the limiting points of the hits.
			aux[0]=Oxx+Rr*cos(FiLimitAdmissible);
			aux[1]=Oyy+Rr*sin(FiLimitAdmissible);
			XintersectionList[2]=aux[0];
			YintersectionList[2]=aux[1];
			nintersections++;
	}  // end of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)


	if( flagOutStt ==0){// 2 intersections with outer Stt circle.
				XintersectionList[nintersections]=XcrossOut[0];
				XintersectionList[nintersections+1]=XcrossOut[1];
				YintersectionList[nintersections]=YcrossOut[0];
				YintersectionList[nintersections+1]=YcrossOut[1];
				nintersections +=2;
	}

	GeometryCalculator.ChooseEntranceExitbis(
				Oxx,
				Oyy,
				Charge,
				FI0,
				nintersections,// n. intersection in input.
				XintersectionList,
				YintersectionList,
				Xcross,	// output
				Ycross	// output
				);

	if(fabs(FiLimitAdmissible-FI0) < 2.*PI){
			// case when this track exit in Z before having the possibility
			// of hitting the Stt parallel inner section.
		if( fabs(aux[0]-Xcross[0])<1.e-5&& fabs(aux[1]-Ycross[0])<1.e-5 ){
			return true;
		}
		if( flagOutStt ==0){// 2 intersections with outer Stt circle.
		   // case when this track exits the Stt outer circle without
		   // hitting the Stt parallel inner section (for instance the track
		   if( (fabs(XcrossOut[0]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[0]-Ycross[0])<1.e-5 )
					||
				(fabs(XcrossOut[1]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[1]-Ycross[0])<1.e-5 )
				){
				return true ;
		   }
		} // end of  if( flagOutStt ==0)

		// most usual case when track crossed the Inner parallel Stt.
		if (nnn == 0) return false;

		// if the exit point is actually given by FiLimitAdmissible, then allow
		// an extra uncertainty in the # Stt hit that must be present;
		// this is done because FiLimitAdmissible is not a very precise number.
		if( fabs(aux[0]-Xcross[1])<1.e-5&& fabs(aux[1]-Ycross[1])<1.e-5 ){
			islack = 3;
		}


	} else { // continuation of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)
		if( flagOutStt ==0){// 2 intersections with outer Stt circle.
			   // case when this track exits the Stt outer circle without
			   // hitting the Stt parallel inner section (for instance the track
			   if( (fabs(XcrossOut[0]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[0]-Ycross[0])<1.e-5 )
					||
				(fabs(XcrossOut[1]-Xcross[0])<1.e-5
				&&fabs(YcrossOut[1]-Ycross[0])<1.e-5 )
				){
				return true ;
			   }
		} // end of  if( flagOutStt ==0)

		// most usual case when track crossed the Skew Stt.
		if (nnn == 0) return false;

	}  // end of  if(fabs(FiLimitAdmissible-FI0) < 2.*PI)



	length=	GeometryCalculator.CalculateArcLength(Oxx,
				Oyy,
				Rr,
				Charge,
				Xcross,
				Ycross
				);
//-------------------- stampe
if(istampa>=2){
cout<<"in SttSkewCleanup,  IVOLTE = "<<IVOLTE<<"\n\t Xcross[0] "
<< Xcross[0]<<", Ycross[0] " <<Ycross[0]<<"\n\t Xcross[1] "
<< Xcross[1]<<", Ycross[1] " <<Ycross[1]<<", R = "<<Rr<<", Lungh. arco "<<length<<endl;
}
//-------------------fine stampe

//-------------------------------------------------------------------------


	Sprevious = atan2(Ycross[0]-Oyy,Xcross[0]-Oxx);
	ibad=0;
	ninside=0;

	for (i=0; i<nHits;i++){
		if( ! GeometryCalculator.IsInsideArc(
			Oxx,Oyy,Charge,
			Xcross,
			Ycross,
			S[i])
			){
			continue;
if(istampa>1)cout<<"in SttSkewCleanup :hit n. "<< ListHits[i]
	<<" is NOT inside the arc between entrance and exit; hit excluded!\n";
		}

		ninside++;

		Distance[i] = 2.*Rr*Rr*(1.-cos(S[i]-Sprevious)); // this is the usual
					//  distance**2 formula: (x1-x2)**2+(y1-y2)**2;
					// it is already 'protected' against S[i] jumps
					// around 2PI/0.
		Sprevious = S[i];
		if(Distance[i]<0.) Distance[i]=0.;   // rounding errors protection.
if(istampa>=2)cout<<"in SttSkewCleanup, Hit n. "<< ListHits[i]<<" has Distance "
<<sqrt(Distance[i]) <<endl;
		if(Distance[i]>cut2){
			if(Distance[i]>16.*cut2){
if(istampa>=2)cout<<"in SttSkewCleanup, Hit n. "<< ListHits[i]<<" has Distance "
<<sqrt(Distance[i]) <<" which is >4.*cut [="<<cut<<"], discard the track!"<<endl;
				return false;
			}
if(istampa>=2)cout<<"in SttSkewCleanup, Hit n. "<< ListHits[i]<<" has Distance "
<<sqrt(Distance[i]) <<" which is > cut [="<<cut<<"]."<<endl;
			ibad++;
		}
	}	// end of do (i=1; i<nHits;i++)



	// cut on the minimum (conservative) n. hits that must have fired
	if( ninside < ((int) 0.5*length/STRAWRADIUS )-islack ){
		if(istampa>1){
			cout<<"in SttSkewCleanup, n. Hits inside = "<<ninside
			<<" is < n. hits that should be inside at least = "
			<<((int) 0.5*length/STRAWRADIUS)<<"-islack ("<<
			islack<<"), track rejected!\n";
			return false;
		}
	}


	// compute the distance of last hit to point at which track leaves this detector volume
	// or the last physical possible Fi (given the length of the  straw).

	if(GeometryCalculator.IsInsideArc(Oxx,Oyy,Charge,
			Xcross,
			Ycross,
			S[nHits-1])
			) {
		Distance[nHits] =
		(Oxx+Rr*cos(S[nHits-1])-Xcross[1])*(Oxx+Rr*cos(S[nHits-1])-Xcross[1]) +
		(Oyy+Rr*sin(S[nHits-1])-Ycross[1])*(Oyy+Rr*sin(S[nHits-1])-Ycross[1]);

if(istampa>=2)cout<<"in SttSkewCleanup, last Hit n. "<< ListHits[nHits-1]<<" has Distance from boundary "
<<sqrt(Distance[nHits]) <<endl;
	   if( Distance[nHits]>cut2 ){
		if( Distance[nHits]>16.*cut2){
if(istampa>=2)cout<<"in SttSkewCleanup, last Hit n. "<< ListHits[nHits-1]<<" has Distance from boundary "
<<sqrt(Distance[nHits]) <<" which is >4 .*cut [="<<cut<<"], discard the track!"<<endl;
			return false;
		}
if(istampa>=2)cout<<"in SttSkewCleanup, last Hit n. "<< ListHits[nHits-1]<<" has Distance from boundary "
<<sqrt(Distance[nHits]) <<" and it is > cut [="<<cut<<"]."<<endl;
		ibad++;
	   }
	}  // end of if(IsInsideArc(



	if( ibad > maxnum){
if(istampa>=2)cout<<"in SttSkewCleanup, reject this track because ibad = "<< ibad
<<" and it is >  maxnum [="<<maxnum<<"].\n";
		 return false;
	}

	return true;

};


//----------end of function PndTrkCleanup::SttSkewCleanup

//----------begin of function PndTrkCleanup::TrackCleanup


bool PndTrkCleanup::TrackCleanup(
	Double_t ApotemaMaxInnerPar,
	Double_t ApotemaMaxSkew,
	Double_t ApotemaMinOuterPar,
	Double_t ApotemaMinSkew,
	Double_t *auxS,
	Short_t  Charge,
	Double_t FI0,
	Double_t GAP,
	Double_t info[][7],
	int	istampa,
	int	IVOLTE,
	Double_t KAPPA,
	Short_t *ListHitsPar,
	Short_t *ListHitsSkew,
	int	MAXSTTHITS,
	Short_t nHitsPar,  // n. hits parall Stt
	Short_t nHitsSkew,  // n. hits parall Stt
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t RStrawDetMax,
	Double_t RStrawDetMin,
	Double_t SEMILENGTH_STRAIGHT,
	Double_t Start[3],
	Double_t STRAWRADIUS,
	Double_t ZCENTER_STRAIGHT
	)
{
// this method does 3 things :
//
//	1)  finds the entrance and exit points in the STT parallel and skew volumes of the current track;
//	2)  eliminates from the track hit list possible spurious hits that are not encompassed
//		by the entrance and exit point;
//	3)  eliminates the tracks if the hit sequence is not continuous enough.


	bool ConsiderLastHit;

	Short_t flagInnerStt,
		flagOuterStt;

	Short_t	ihit,
			nInnerHits,
			nOuterHits,
			nIntersections[2],
			ListInnerHits[MAXSTTHITS],
			ListOuterHits[MAXSTTHITS];

	Double_t	FiLimitAdmissible,
			r,
			Xcross[2],
			Ycross[2],
			XintersectionList[12][2], // second index =0 --> inner Hexagon, =1 --> outer.
			YintersectionList[12][2]; // first index : all the possible intersections
						  // (up to 12 intersections).

//------------------------
	// calculation of the Maximum FI angle possible (if it is a +ve charge) of the Minimum
	// for this track, taking into account
	// that the maximum possible Z of a hit is ZCENTER_STRAIGHT + SEMILENGTH_STRAIGHT; the minimum
	// Z of a hit is ZCENTER_STRAIGHT - SEMILENGTH_STRAIGHT.
	if(Charge<0){
		if( KAPPA>0.){
			FiLimitAdmissible = FI0 + KAPPA*(ZCENTER_STRAIGHT + SEMILENGTH_STRAIGHT) ;
		} else {
			FiLimitAdmissible = FI0 + KAPPA*(ZCENTER_STRAIGHT - SEMILENGTH_STRAIGHT) ;
		}
	} else {
		if( KAPPA>0.){
			FiLimitAdmissible = FI0 + KAPPA*(ZCENTER_STRAIGHT - SEMILENGTH_STRAIGHT) ;
		} else {
			FiLimitAdmissible = FI0 + KAPPA*(ZCENTER_STRAIGHT + SEMILENGTH_STRAIGHT) ;
		}
	}  // end of    if(Charge<0)
//-----------------------------------------------------------------------------------------------------

	// parallel cleanup.

//----------------stampe
if(istampa>=2){
cout<<" IVOLTE = "<<IVOLTE<<", prima di paral cleanup, nHitsPar "<<nHitsPar<<
", KAPPA = "<<KAPPA <<", charge "<<Charge<<", FI0 "<<FI0
<<"\n\tFiLimitAdmissible "<<
FiLimitAdmissible<<", X limit "<<Oxx+Rr*cos(FiLimitAdmissible)<<
", Y limit "<<Oyy+Rr*sin(FiLimitAdmissible)<<", Ox "<<Oxx<<", Oy "<<Oyy<<", R "<<Rr<<endl;
}
//-------------------- fine stampe

if(istampa>1) cout<<"\tentra in SttParalCleanup\n";

//	if(nHitsPar>0 && !SttParalCleanup(
	if(!SttParalCleanup(
				ApotemaMaxInnerPar,
				ApotemaMinOuterPar,
				Charge,
				FI0,
				FiLimitAdmissible,
				GAP,
				info,
				istampa,
				IVOLTE,
				ListHitsPar, // input only for now. 
				nHitsPar, // it doesn't get modify for now.
				Oxx,
				Oyy,
				Rr,
				RStrawDetMax,
				RStrawDetMin,
				Start,
				STRAWRADIUS
				)
	){
if(istampa>1) cout<<"uscito da SttParalCleanup : false\n";
		return false;
	}
if(istampa>1) cout<<"uscito da : SttParalCleanup true\n";




//----------------------------------------------------------------------------

	// skew cleanup.
if(istampa>1) cout<<"\tentra in SttSkewCleanup\n";
	if ( ! (SttSkewCleanup(
			ApotemaMaxSkew,
			ApotemaMinSkew,
			Charge,
			3., // cut distance
			FI0,
			FiLimitAdmissible,
			GAP,
			info,
			istampa,
			IVOLTE,
			ListHitsSkew, // it doesn't get modify for now.
			1, // max number of failures allowed.
			MAXSTTHITS,
			nHitsSkew, // it doesn't get modify for now.
			Oxx,
			Oyy,
			Rr,
			RStrawDetMax,
			auxS,
			Start,  // starting point of trajectory.
			STRAWRADIUS
			) ) ) {

if(istampa>1) cout<<"uscito da SttSkewCleanup false\n";
		  return false;
				}

if(istampa>1) cout<<"uscito da SttSkewCleanup true\n";


	return true;

};



//----------end of function PndTrkCleanup::TrackCleanup

//----------begin of function PndTrkCleanup::XYCleanup
bool PndTrkCleanup::XYCleanup(
	Double_t info[][7],
	Short_t (*ListParContiguous)[6],
	Short_t *nParContiguous,
	Short_t *StrawCode,
	Short_t *StrawCode2,
	Short_t *TubeID,

	Short_t *ListHits,
	Short_t nHits,
	Double_t R_STT_INNER_PAR_MAX
				)
{

   bool	connected;

   Short_t
	i,
	j,
	not_connected,
	tListInnerHitsLeft[nHits],
	tListInnerHitsRight[nHits],
	tListOuterHitsLeft[nHits],
	tListOuterHitsRight[nHits],
	tube,
	tube_next,
	nInnerHitsLeft,
	nInnerHitsRight,
	nOuterHitsLeft,
	nOuterHitsRight;

   Vec <Short_t>
	ListInnerHitsLeft(tListInnerHitsLeft,nHits,"ListInnerHitsLeft"),
	ListInnerHitsRight(tListInnerHitsRight,nHits,"ListInnerHitsRight"),
	ListOuterHitsLeft(tListOuterHitsLeft,nHits,"ListOuterHitsLeft"),
	ListOuterHitsRight(tListOuterHitsRight,nHits,"ListOuterHitsRight");

   //  separate the inner axial Stt hits from outer axial Stt hits,
   //  right (looking into the beam) from left;

   SeparateInnerOuterRightLeftAxialStt(

	// input
	info,
	ListHits,
	nHits,
	R_STT_INNER_PAR_MAX,

	// output

	tListInnerHitsLeft,
	tListInnerHitsRight,
	tListOuterHitsLeft,
	tListOuterHitsRight,
	&nInnerHitsLeft,
	&nInnerHitsRight,
	&nOuterHitsLeft,
	&nOuterHitsRight
   );

 // check continuity between first hit in inner axial hit list and the last inner axial;
 // left and right;

   not_connected = 0;

 // Left Inner;
   for(i=0;i<nInnerHitsLeft-1;i++){
   	tube = TubeID[ ListInnerHitsLeft[i] ];
	tube_next = TubeID[ ListInnerHitsLeft[i+1] ];
	for(j=0;j<nParContiguous[ tube-1 ];j++){
		if( tube_next == ListParContiguous[ tube-1 ][j] ){
			connected = true;
			break;
		}
	}
	if(!connected) {
		not_connected ++;
		if( not_connected > MAX_NOT_CONNECTED) return false;
	} // end of  if(!connected)
   }  // end of for(i=0;i<nInnerHitsLeft;i++


 // Left Outer;
   for(i=0;i<nOuterHitsLeft-1;i++){
   	tube = TubeID[ ListOuterHitsLeft[i] ];
	tube_next = TubeID[ ListOuterHitsLeft[i+1] ];
	for(j=0;j<nParContiguous[ tube-1 ];j++){
		if( tube_next == ListParContiguous[ tube-1 ][j] ){
			connected = true;
			break;
		}
	}
	if(!connected) {
		not_connected ++;
		if( not_connected > MAX_NOT_CONNECTED) return false;
	} // end of  if(!connected)
   }  // end of for(i=0;i<nOuterHitsLeft;i++




 // Right Inner;
   for(i=0;i<nInnerHitsRight-1;i++){
   	tube = TubeID[ ListInnerHitsRight[i] ];
	tube_next = TubeID[ ListInnerHitsRight[i+1] ];
	for(j=0;j<nParContiguous[ tube-1 ];j++){
		if( tube_next == ListParContiguous[ tube-1 ][j] ){
			connected = true;
			break;
		}
	}
	if(!connected) {
		not_connected ++;
		if( not_connected > MAX_NOT_CONNECTED) return false;
	} // end of  if(!connected)
   }  // end of for(i=0;i<nInnerHitsRight;i++


 // Right Outer;
   for(i=0;i<nOuterHitsRight-1;i++){
   	tube = TubeID[ ListOuterHitsRight[i] ];
	tube_next = TubeID[ ListOuterHitsRight[i+1] ];
	for(j=0;j<nParContiguous[ tube-1 ];j++){
		if( tube_next == ListParContiguous[ tube - 1 ][j] ){
			connected = true;
			break;
		}
	}
	if(!connected) {
		not_connected ++;
		if( not_connected > MAX_NOT_CONNECTED) return false;
	} // end of  if(!connected)
   }  // end of for(i=0;i<nOuterHitsRight;i++



	return true;
}
//----------end of function PndTrkCleanup::XYCleanup


ClassImp(PndTrkCleanup);
