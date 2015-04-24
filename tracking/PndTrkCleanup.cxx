#include "PndTrkCleanup.h"
#include "PndTrkVectors.h"
#include "PndTrkConstants.h"


#include <iostream>
#include <math.h>


// Root includes
#include "TROOT.h"


using namespace std;


//----------begin of function PndTrkCleanup::BadTrack_ParStt

bool PndTrkCleanup::BadTrack_ParStt(
	Double_t Oxx,
	Double_t Oyy,
	Double_t Rr,
	Double_t strawradius,
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
	nume = 0.5*length/strawradius -islack;
	if( ninside < nume ){
		if(istampa>1){
			int icz = 0.5*length/strawradius;
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



	return true;
}


//----------end of function PndTrkCleanup::BadTrack_ParStt


//----------begin of function PndTrkCleanup::GoodTrack

bool PndTrkCleanup::GoodTrack(
		Double_t info[][7],		// input
		bool farthest_hit_is_boundary,	// input
		Double_t Ox,			// input; center of the current track;
		Double_t Oy,			// input; center of the current track;
		Double_t R,			// input; Radius of the current track;
		Short_t Charge,			// input; Charge of the current track;
		Short_t nHits,			// input
		Short_t* ListHits,		// input
		Short_t *StrawCode, // first straw boundary code (a straw can belong to 2 boundaries);
		Short_t *StrawCode2,
		Short_t *TubeID,		// input
		Short_t *nParContiguous,	// input
		Short_t ListParContiguous[][6],	// input
		Double_t *xTube,		// input
		Double_t *yTube,		// input
		Double_t *zTube,		// input
		Double_t *xxyyTube,		// input

		Short_t & holes			// input and output
			)
{

   // Convention for the Sector number used in GoodTrack :
   // Sector = 1 --> Axial Outer Right
   // Sector = 2 --> Axial Inner Right
   // Sector = 3 --> Axial Inner Left
   // Sector = 4 --> Axial Outer Left


   // holes == # of "holes" in the track;
   // no_holes == flag requiring total continuity of track (no holes) ;
   //farthest_hit_is_boundary --> if this flag is true the hit of the track furthest from the origin is
			// at the boundary of the sector;

   // nHits == # hits in this track under scrutiny;	
   // info == some information on the hits;
   // ListHits == list hits in this track under scrutiny;	
   // TubeID == hit number;	


   bool		boundary,
   		yes;

   Short_t
		i,
		index,
		index_last,
		inner,
		j,
		nIntersections,
		outer,
		tube_current,
		tube_next;


   Double_t
		C2,
		dist,
		dist2,
		fi,
		Start[3],
		Xend[2],
		Yend[2];

   PndTrkCTGeometryCalculations GeometryCalculator;

// the first hit is the farthest from (0,0,0);
// StrawCode convention (in the following left or right is looking to the beam from downstream) :
//   -1 = not a boundary straw;
//   10= inner axial boundary left;
//   20= inner axial boundary right;
//   12= outer VERTICAL (BUT NOT OUTERMOST) axial boundary left;
//   22= outer VERTICAL (BUT NOT OUTERMOST)  axial boundary right;
//   13= outermost axial boundary left;
//   23= outermost axial boundary right;

// the flags are 2 (StrawCode and StrawCode2) since a straw can belong to 2 boundaries;




   // first check if the first hit is required to be at the boundary; if so verify the condition;
   // only one hole is allowed;
   if( farthest_hit_is_boundary ){
   	index = TubeID[ ListHits[nHits-1] ]; //  number corresponding to the Straw of the last hit in the list;
	if( StrawCode[ index -1 ] == -1 && StrawCode2[ index -1 ] ==-1   ) {
	 // not at any boundary; before returning false check if one hole is still allowed and if so
	 // check if any of the neighbours of the first hit, is at boundary : if so increment the
	 //  number of holes and go on;
	 	if( holes >= MAX_NOT_CONNECTED) return false;  // because we know already that holes becomes >= MAX_NOT_CONNECTED+1;
		yes=false;
		//  loop over the Straws contiguous to the current under scrutiny;
		for(i=0;i<nParContiguous[index-1];i++){
	 		if( StrawCode[ ListParContiguous[index-1][i] -1 ] > -1 ||
			     StrawCode2[ ListParContiguous[index-1][i] -1 ] > -1)
			{
				holes++;
				yes=true;
				break;
			}    
	 	}  // end of for(i=0;i<nHits-1;i++)
		if(!yes)  return false;	// none of the neighbouring Straws is at the boundary;


	}	// end of  if( StrawCode[ index -1 ] == -1 && StrawCode2[ index -1 ] ==-1   )

   }	// end of  if( farthest_hit_is_boundary )

   //---------------------------------------------------
   // now check if hits are contiguous going from the outermost to the innermost;
   for(i=0;i<nHits-1;i++){
	outer = nHits-i-1;
	inner = nHits-i-2;
//   	tube_current = TubeID[ ListHits[outer] ];
//	tube_next = TubeID[ ListHits[inner] ];

	// distance squared between centers of the two straws;
	dist2 = (info[ListHits[outer]][0]-info[ListHits[inner]][0])*(info[ListHits[outer]][0]-info[ListHits[inner]][0]);
	dist2 += (info[ListHits[outer]][1]-info[ListHits[inner]][1])*(info[ListHits[outer]][1]-info[ListHits[inner]][1]);
	if( dist2 < DIAMETERSTRAWTUBE2 * 1.1) continue; //tubes are contiguous; the factor 1.1 just to be sure
							// against rounding errors ;

	// ----------------------------------------------------------------------------------------------------------
	//  case when tube_current and tube_next are not contiguous;

	// case in which there are only 1 entrance point and 1 exit point for the track (the most common one);
	// in this case all hits of the track are internal in this sector --> calculate the contiguity level
	// of the two hits under scrutiny;

		// next-to-contiguos hits;		
		if( dist2 < 16.*STRAWRADIUS*STRAWRADIUS * 1.1) {
			// increase   holes  by 1 and then check if  holes>MAX_NOT_CONNECTED discard the track;
			holes++;
			if(holes > MAX_NOT_CONNECTED) return false;
			continue;	// case accepted;
		} else {
			// distance between tube_current and tube_next >= 2 straws, discard the track;
			return false;
		}


   }  // end of for(i=0;i<nHits;i++


//-----------------------------------------------------------------------------------------------

   // check on the innermost hit (namely : ListHits[0]) ; it must be a boundary hit OR a next-to-boundary hit;

   index_last = TubeID[ListHits[0]] -1  ;
   // it is at the boundary, therefore track is accepted;
   if(!(StrawCode[index_last] == -1 && StrawCode2[index_last] == -1)){
	return true;
   }

   if(holes == MAX_NOT_CONNECTED){
	// in this case the track must be rejected because the number of holes is > MAX_NOT_CONNECTED;
	return false;

   } else {
	// check if any of the neighbor straws, BELONGING TO THE TRACK AND IN THE RIGHT
	// ORDER (according to the Charge), is a boundary track;

	boundary = false;
	Xend[0] = Yend[0] = 0. ;	// the origin;
	Xend[1] = xTube[index_last];
	Yend[1] = yTube[index_last];
	for(j=0;j<nParContiguous[ index_last ];j++){
		tube_next = ListParContiguous[index_last][j]  ;

		// calculate the distance between the center of the trajectory and the center of the straw;
		// C2 has been calculated abova --> C2 = Ox*Ox + Oy*Oy ;
		dist2 = xxyyTube[tube_next-1] -2.*(xTube[tube_next-1]*Ox + yTube[tube_next-1]*Oy) +C2;
		dist = fabs(sqrt(dist2)-R);
		if( dist > STRAWRADIUS * 1.5) continue; //tubes doesn't lie on trajectory; the factor 1.5 just to be sure

		// now check that the tube_next lies between (0,0) and the hit = ListHits[0] when running on the
		// trajectory according to the charge (namely : +ve --> clockwise, -ve --> anticlockwise);
		// the coordinates of the ends of the arc are given in Xend[2] and Yend[2];
		// fi is the angle (between 0. and 2 PI ) of the point under srutiny (==tube_next center);

		fi = atan2( yTube[tube_next-1]-Oy, xTube[tube_next-1]-Ox);
		if(fi<0.) fi += 2.*PI;
		if(fi<0.) fi = 0.;

		if( GeometryCalculator.IsInsideArc(Ox,Oy,Charge,Xend,Yend,fi)){
			// check if this is at boundary;
			if( !(StrawCode[tube_next-1] == -1 && StrawCode2[tube_next-1] == -1) ) {
				boundary = true;
				break;
			}
		}	// end of  if( GeometryCalculator.IsInsideArc(Ox,Oy,Charge,Xend,Yend,f))

	}	// end of  for(j=0;j<nParContiguous[ index_last ];j++)

	if(boundary){
		holes++;
		return true;
	} else {
		return false;
	}

   }	// end of   if(holes == MAX_NOT_CONNECTED)


}

//----------end of function PndTrkCleanup::GoodTrack

//----------begin of function PndTrkCleanup::Is_Contained_in_Mvd_Vertical_Strip

 Short_t PndTrkCleanup::Is_Contained_in_Mvd_Vertical_Strip(
				Short_t iLayer,	// index of the Mvd Disk Layer under scrutiny here;
 				Short_t nXlow, //  index of the strip containing Xlow;
				Short_t nXup, //  index of the strip containing Xlow;
				Double_t Ylow,
				Double_t Yup
					)
{

	Short_t
		j,
		strip_index,
		sum_intersections_types;

	Double_t
		tmpYlow,
		tmpYup;

	// nXlow == 0 --> X position >=0; the first 'negative X' strip has index -1;



	sum_intersections_types = 0;


	if( Ylow < 0.  &&  Yup > 0.){
		for(j=nXlow;j<= nXup;j++){ // loop over the active regions of the i-th Mvd Disk;
			if(j<0) strip_index = -j -1; else strip_index=j; // this fixes the case with negative index;

			if(MVD_DISK_LAYER_YMIN[iLayer][j] == 0. ){ // case in which the sensor is continuous in Y;
					if( -Ylow <= MVD_DISK_LAYER_YMAX[iLayer][j] && Yup <= MVD_DISK_LAYER_YMAX[iLayer][j])
					{
						sum_intersections_types++;  // hit contained in this sensor;
					}  // if the if condition is not satisfied the hit is only partially contained
					   // within errors and therefore sum_intersections_types is not incremented nor
					   // decremented;
			} else {	// case in which there is a gap in Y in the sensor;

					if( -Ylow< MVD_DISK_LAYER_YMIN[iLayer][j] && Yup < MVD_DISK_LAYER_YMIN[iLayer][j])
					{
						sum_intersections_types-- ;  // case in which the hit is NOT contained
							// within errors in the sensor because there is the Y gap in the
							// sensor; 
					};
					// in all other cases the hit is PARTIALLY contained within errors and therefore
					// sum_intersections_types is not incremented nor decremented;

			} // end of if(MVD_DISK_LAYER_YMIN[iLayer][j]
		}	//  end of for(j=nXlow;j<= nXup;j++)
		// final result :
		if( sum_intersections_types == 1 + nXup - nXlow) { // hit was in all strips;
				return 1;
		} else if ( sum_intersections_types == -(1 + nXup - nXlow) ) {  // hit was out in all strips;
				return -1;
		} else {  // sometimes was in, sometimes was out;
				return 0;
		}


	} else if( Ylow >=0.){  // continuation of if( Ylow < 0.  &&  Yup > 0.)
			tmpYlow = Ylow;
			tmpYup = Yup;
	} else {  // here it is the case when Yup <=0.;
			tmpYup = -Ylow;
			tmpYlow = -Yup;
	}

	for(j=nXlow;j<= nXup;j++){ // loop over the active regions of the i-th Mvd Disk;
			if(j<0) strip_index = -j -1; else strip_index=j; // this fixes the case with negative index;
			if ( tmpYlow > MVD_DISK_LAYER_YMAX[iLayer][strip_index] ||
			     tmpYup < MVD_DISK_LAYER_YMIN[iLayer][strip_index]){
				sum_intersections_types--; //  +1 --> track completely in the sensors;
							   //  + 0 --> uncertain;  -1 --> out of the sensor;
			} else if ( tmpYlow < MVD_DISK_LAYER_YMIN[iLayer][strip_index]){
				;
			} else {	// here is the case YMIN<= Ylow <= YMAX;
				if(tmpYup <= MVD_DISK_LAYER_YMAX[iLayer][strip_index]){
				sum_intersections_types++; //  +1 --> track completely in the sensors;
							   //  + 0 --> uncertain;  -1 --> out of the sensor;
				}
				// in the remaining case the hit is uncertain that it hit the sensor;
				// therefore sum_intersections_types remains unchanged;
			}
	}	//  end of for(j=nXlow;j<= nXup;j++)


	if( sum_intersections_types == 1 + nXup - nXlow) { // hit was in in all strips;
			return 1;
	} else if ( sum_intersections_types == -(1 + nXup - nXlow) ) {  // hit was out in all strips;
			return -1;
	} else {  // sometimes was in, sometimes was out;
			return  0;
	}


}
//----------end of function PndTrkCleanup::Is_Contained_in_Mvd_Vertical_Strip




//----------begin of function PndTrkCleanup::MvdCleanup

 bool PndTrkCleanup::MvdCleanup(
	Double_t Ox,
	Double_t Oy,
	Double_t R,
	Double_t fi0,
	Double_t kappa,
	Double_t charge,
	Double_t* XMvdPixel,  // list of the X positions of ALL Mvd hits of the event;
	Double_t* XMvdStrip,  // list of the X positions of ALL Mvd hits of the event;
	Double_t* YMvdPixel,  // list of the Y positions of ALL Mvd hits of the event;
	Double_t* YMvdStrip,  // list of the Y positions of ALL Mvd hits of the event;
	Double_t* ZMvdPixel,  // list of the Z positions of ALL Mvd hits of the event;
	Double_t* ZMvdStrip,  // list of the Z positions of ALL Mvd hits of the event;
	Short_t nPixelHitsinTrack,  // number of Mvd Pixel hits in this track;
	Short_t * ListMvdPixelHitsinTrack,	
	Short_t nStripHitsinTrack,  // number of Mvd Strip hits in this track;
	Short_t * ListMvdStripHitsinTrack,
	Double_t extra_distance,
	Double_t extra_distance_Z,
	PndTrkCTGeometryCalculations* GeomCalculator
	)
{

/*
	Double_t Ox  -->  X of center of the Helix of the particle trajectory;
	Double_t Oy  -->  Y of center of the Helix of the particle trajectory;
	Double_t R,  -->  radius of the Helix of the particle trajectory;
	Double_t fi0 -->  FI0 of the Helix of the particle trajectory;
	Double_t kappa  -->  KAPPA of the Helix of the particle trajectory;
	Double_t charge -->  charge of the particle;
	Double_t semiverticalgap -->  half dimension of the gap between the two STT sectors;
	Short_t nMvdHits -->  number of Mvd hits in this track;
	Double_t extra_distance --> in cm; extra distance allowed during the process to decide whether there
				should be an hit in a Mvd sensitive layer;
	PndTrkCTGeometryCalculations* GeomCalculator  -->  class that makes the geometrical calculations;
*/

	bool
		GoOn;

	Short_t
		i,
		type_of_intersection_in_disk[MVD_DISK_LAYERS],	// = +1 --> track completely in the sensors;
							//  = 0 --> uncertain; = -1 --> out of the sensor;
		j,
		nFaults,
		n_intersections_barrel[MVD_BARREL_LAYERS],
		nXlow,
		nXup,
		strip_index,
		sum_intersections_types,
		yes_intersect
		;
	Double_t
		absYlow,
		absYup,
		FiOrderedList[2],
		phase,
		r2,
		tmpYlow,
		tmpYup,
		Xcross[2],
		Ycross[2],
		X_barrel[MVD_BARREL_LAYERS][2],
		Xlow,
		Xup,
		Y_barrel[MVD_BARREL_LAYERS][2],
		Ylow,
		Yup,
		Z_barrel[MVD_BARREL_LAYERS][2],
		X_disk,
		Y_disk,
		Z_disk,
		z;

	PndTrkCTGeometryCalculations  GeometryCalculator;


//------------------------------------------------------- MVD BARREL ----------------------------------------------------------

	// loop over the number of barrel Mvd layers;
	// calculate if trajectory intersects the Mvd barrel layers;
	for(i=0;i<MVD_BARREL_LAYERS;i++){
		n_intersections_barrel[i] = 0; // number of intersections of the present Mvd Barrel layer; in general
					// they are 2, the first corresponding to the minimum Radius, the second
					// corresponding to the maximum radius;
	
		// yes_intersect = 0 --> 2 intersections
		// otherwise yes_intersect = -1;
		// I use the method FindIntersectionsOuterCircle that calculates the intersection between
		// two circles;
		yes_intersect = GeometryCalculator.FindIntersectionsOuterCircle(
				Ox,
				Oy,
				R,
				MVD_BARREL_AVERAGE_RADIUS[i],// AVERAGE radius of the i-th Mvd barrel layer;
				Xcross,
				Ycross
				);
		if(yes_intersect==0 ) {   // there are 2 intersections of the trajectory with this barrel;
			// calculate the entrance point based on the rotation direction of the particle
			// (positive --> clockwise);
			GeometryCalculator.ChooseEntranceExit3(
				Ox,
				Oy,
				charge,
				fi0,
				2,
				Xcross, // input and output;
				Ycross, // input and output;
				FiOrderedList // output;
			);

			// calculate the Z coordinate of the intersection; since kappa is necessarily
			//  > 1.e-10 by construction, then Z is always well defined;
			z = (FiOrderedList[0]-fi0)/kappa;

			// condition for having Mvd hits in the Mvd Barrel layers certainly, namely taking
			// into account also possible errors in the Z caused by the uncertainty of the
			// trajectory; such an error is called extra_distance_Z (in cm);

			// condition by which the trajectory surely had to cross the barrel layer;
			if( z<= MVD_BARREL_ZLIMITS[1][i]-extra_distance_Z && 
			    z>= MVD_BARREL_ZLIMITS[0][i]+extra_distance_Z &&
			   (!(z>MVD_BARREL_NOZONE_Z[0]-extra_distance_Z && // conservative!
			     z<MVD_BARREL_NOZONE_Z[1]+extra_distance_Z&& // out of the target pipe; conservative!
			     Xcross[0] > MVD_BARREL_NOZONE_X[0]-extra_distance &&   // out of the target pipe;
			     Xcross[0] < MVD_BARREL_NOZONE_X[1]+extra_distance))
			 ){	// case in which there should be Mvd hits;
			 	X_barrel[i][n_intersections_barrel[i]] = Xcross[0];
				Y_barrel[i][n_intersections_barrel[i]] = Ycross[0];
				Z_barrel[i][n_intersections_barrel[i]] = z;
		     		n_intersections_barrel[i]++;
			}
		}  // end of if(yes_intersect>0 )


	};  // end of  for(i=0;i<MVD_BARREL_LAYERS;i++)



//------------  check if there are the hits in the barrel in the layer predicted by the previous extrapolation of the track;
	nFaults = 0;
	for(i=0;i<MVD_BARREL_LAYERS;i++){
		if (n_intersections_barrel[i]>0){
			// loop over all Mvd hits of the track;
			nFaults++;
			GoOn=true;
			for(j=0;j< nPixelHitsinTrack; j++){
				r2 = XMvdPixel[ ListMvdPixelHitsinTrack[j] ]*XMvdPixel[ ListMvdPixelHitsinTrack[j] ]+
				     YMvdPixel[ ListMvdPixelHitsinTrack[j] ]*YMvdPixel[ ListMvdPixelHitsinTrack[j] ];
				if( fabs( r2 - MVD_BARREL_RADIASQMean[i]) <= MVD_BARREL_RADIASQDifference[i]){
					nFaults--;
					GoOn = false;
					break;
				}
			} // end of for(j=0;j< nPixelHitsinTrack; j++)
			if(GoOn){
			for(j=0;j< nStripHitsinTrack; j++){
				r2 = XMvdStrip[ ListMvdStripHitsinTrack[j] ]*XMvdStrip[ ListMvdStripHitsinTrack[j] ]+
				     YMvdStrip[ ListMvdStripHitsinTrack[j] ]*YMvdStrip[ ListMvdStripHitsinTrack[j] ];
				if( fabs( r2 - MVD_BARREL_RADIASQMean[i]) <= MVD_BARREL_RADIASQDifference[i]){
					nFaults--;
					break;
				}
			} // end of for(j=0;j< nStripHitsinTrack; j++)
			}	// end of   if(GoOn)
		}  // end of     if (n_intersections_barrel[i]>0)
	};  // end of  for(i=0;i<MVD_BARREL_LAYERS;i++)
	
	if(nFaults>1) return false;

//------------------------------------------------------- MVD DISKS ----------------------------------------------------------


	// loop over the number of Disks Mvd;
	// calculate if trajectory intersects the Mvd Disk layers;
	for(i=0;i<MVD_DISK_LAYERS;i++){
		// calculate the intersections on the Mvd Disks;
		phase = fi0 + kappa *  MVD_DISK_Z[i];	
		X_disk = Ox + R* cos(phase) ;
		Xup = X_disk + extra_distance;
		Xlow = X_disk - extra_distance;
		Y_disk = Oy + R* sin(phase) ;
		Ylow = Y_disk - extra_distance;
		Yup = Y_disk + extra_distance;

		// now calculate if the intersection falls in the sensor active region of the Mvd Disk;
		nXlow = Xlow / MVD_DISK_LAYER_deltaX[i];
		if( nXlow < 0 ) nXlow --;
		nXup  = Xup  / MVD_DISK_LAYER_deltaX[i];
		if( nXup < 0 ) nXup --;  // this is because the first strip at negative X cannot have
					// index 0 but it must start from -1;

		if(nXlow< - MVD_DISK_PIECES[i])	// Xlow is out of the sensors;
		{
			if(nXup <= - MVD_DISK_PIECES[i]) {
				type_of_intersection_in_disk[i]=-1;	// out of the sensors;
				continue;
			} else {
				// if Y is out of the sensor --> type_of_intersection_in_disk[i]= -1;
				// if Y is in the sensor --> type_of_intersection_in_disk[i]= 0;

				type_of_intersection_in_disk[i] = Is_Contained_in_Mvd_Vertical_Strip(
					i,	// index of the Mvd Disk Layer under scrutiny here;
 					- MVD_DISK_PIECES[i], //  minimum possible index;
					nXup, //  index of the strip containing Xup;
					Ylow, // Ylow (abs of it if it is the case);
					Yup // Yup (abs of it if it is the case);
				);
				if(type_of_intersection_in_disk[i]== 1) type_of_intersection_in_disk[i]= 0; // because
					// Xlow was outside already of the boundary;
				continue;
			}
		} else if ( nXlow >= MVD_DISK_PIECES[i] )	// Xlow is out of the sensors;
		{
			type_of_intersection_in_disk[i]=-1;	// out of the sensors
			continue;
		}

		if( nXup >= MVD_DISK_PIECES[i] ) // at this point here Xlow is inside the sensors but Xup is not;
		{
			// check if Y is in the sensor --> type_of_intersection_in_disk[i]= 0, OR
			// Y is out of the sensor --> type_of_intersection_in_disk[i]= -1;

			type_of_intersection_in_disk[i] = Is_Contained_in_Mvd_Vertical_Strip(
				i,	// index of the Mvd Disk Layer under scrutiny here;
 				nXlow, //  index of the strip containing Xlow;
				MVD_DISK_PIECES[i]-1, //  maximum possible index;
				Ylow, // Ylow (abs of it if it is the case);
				Yup // Yup (abs of it if it is the case);
										);
			if(type_of_intersection_in_disk[i]== 1) type_of_intersection_in_disk[i]= 0; // because
					// nXup was outside already of the boundary;
			continue;
		} // end of   if( nXup >= MVD_DISK_PIECES[i] )

		// here both Xlow and Xup are IN the sensors;
		type_of_intersection_in_disk[i] = Is_Contained_in_Mvd_Vertical_Strip(
				i,	// index of the Mvd Disk Layer under scrutiny here;
 				nXlow, //  index of the strip containing Xlow;
				nXup, //  index of the strip containing Xup;
				Ylow, // Ylow (abs of it if it is the case);
				Yup // Yup (abs of it if it is the case);
										);
	};  // end of  for(i=0;i<MVD_DISKS_LAYERS;i++)



//------------  check if there are the hits in the barrel in the layer predicted by the previous extrapolation of the track;
	nFaults = 0;
	for(i=0;i<MVD_DISK_LAYERS;i++){
		if (type_of_intersection_in_disk[i]==1){
			// loop over all Mvd hits of the track;
			for(j=0;j< nPixelHitsinTrack; j++){
				if( fabs( ZMvdPixel[ ListMvdPixelHitsinTrack[j] ]-MVD_DISK_Z[i]) > 1.)
				{
					nFaults++;
				}
			} // end of for(j=0;j< nPixelHitsinTrack; j++)

			for(j=0;j< nStripHitsinTrack; j++){
				if( fabs( ZMvdStrip[ ListMvdStripHitsinTrack[j] ]-MVD_DISK_Z[i]) > 1.) nFaults++;
			} // end of for(j=0;j< nStripHitsinTrack; j++)

		}

	};  // end of  for(i=0;i<MVD_DISKS_LAYERS;i++)
	
	if(nFaults>1) return false;


	return true;

}
//----------end of function PndTrkCleanup::MvdCleanup





//----------begin of function PndTrkCleanup::MvdCleanup_prova

 bool PndTrkCleanup::MvdCleanup_prova(
	Double_t Ox,
	Double_t Oy,
	Double_t R,
	Double_t fi0,
	Double_t kappa,
	Double_t charge,
	Double_t semiverticalgap,
	Short_t nMvdHits,
	PndTrkCTGeometryCalculations* GeomCalculator
	)
{

	if( !GeomCalculator->IsInTargetPipe(
			Ox,
			Oy,
			R,
			fi0,
			kappa,
			charge,
			semiverticalgap
					) && nMvdHits==0 ) return false;
	return true;

}
//----------end of function PndTrkCleanup::MvdCleanup_prova








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


//----------begin of function PndTrkCleanup::ParalCleanup

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
	Double_t strawradius
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


	islack=1;  // uncertainty allowed in the # of straws that should be hit in a given part
			// of the Stt detector.


//------------------------
//  elimination of hits outside the physical FI range (FiLimitAdmissible) due to finite length of
//  Straws.


if(istampa>1) {
	cout<<"SttParalCleanup, evento n. "<<IVOLTE<<", n. || in ingresso "<<
	nHits<<", prima di essere purgati."<<endl; }

	epsilonTheta = strawradius/Rr;  // some extra slac for being conservative.
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
			< 9.*strawradius*strawradius ) flagInnerSttR=-1;

	if( flagInnerSttL == 0 && (XcrossL[0]-XcrossL[1])*(XcrossL[0]-XcrossL[1])+
			(YcrossL[0]-YcrossL[1])*(YcrossL[0]-YcrossL[1])
			< 9.*strawradius*strawradius ) flagInnerSttR=-1;

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
			strawradius,
			Charge,
			Xcross,  // Xcross[0]=point of entrance; Xcross[1]=point of exit.
			Ycross,
			nInnerHits,
			ListInnerHits,
			info,
			istampa,
			2.*2*strawradius,	//  cut of proximity between hits.
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
			< 9.*strawradius*strawradius ) flagInnerSttR=-1;

	if( flagInnerSttL == 0 && (XcrossL[0]-XcrossL[1])*(XcrossL[0]-XcrossL[1])+
			(YcrossL[0]-YcrossL[1])*(YcrossL[0]-YcrossL[1])
			< 9.*strawradius*strawradius ) flagInnerSttR=-1;


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
			strawradius,
			Charge,
			Xcross,  // Xcross[0]=point of entrance; Xcross[1]=point of exit.
			Ycross,
			nOuterHits,
			ListOuterHits,
			info,
			istampa,
			2.*2.*strawradius,	//  cut of proximity between hits.
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
	Double_t strawradius
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

	//  class with all the geometry calculations :
	PndTrkCTGeometryCalculations GeometryCalculator;



	cut2=cut*cut;
	islack=1;// uncertainty allowed as far as
		// the n. of hits that should be present in a given section of the Stt track.


//------------------------
//  elimination of hits outside the physical FI range (FiLimitAdmissible) due to finite length of
//  Straws.

	epsilonTheta = strawradius/Rr;  // some extra slac for being conservative.

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
			< 16.*strawradius*strawradius ){
		flagSttR=-1;
if(istampa>1)cout<<"in SttSkewCleanup : distanza entrata-uscita<4*strawradius,flagSttR set at -1!\n";
	}

	if( flagSttL == 0 && (XcrossL[0]-XcrossL[1])*(XcrossL[0]-XcrossL[1])+
			(YcrossL[0]-YcrossL[1])*(YcrossL[0]-YcrossL[1])
			< 16.*strawradius*strawradius ){
		flagSttR=-1;
if(istampa>1)cout<<"in SttSkewCleanup : distanza entrata-uscita<4*strawradius,flagSttL set at -1!\n";
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
	if( ninside < ((int) 0.5*length/strawradius )-islack ){
		if(istampa>1){
			cout<<"in SttSkewCleanup, n. Hits inside = "<<ninside
			<<" is < n. hits that should be inside at least = "
			<<((int) 0.5*length/strawradius)<<"-islack ("<<
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
//	Double_t SEMILENGTH_STRAIGHT,
	Double_t Start[3],
	Double_t strawradius
//	Double_t ZCENTER_STRAIGHT
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
				strawradius
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
			strawradius
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
	// general infos about the axial Straws;
	int istampa,
	Double_t info[][7],
	Short_t (*ListParContiguous)[6],
	Short_t *nParContiguous,
	Short_t *StrawCode,
	Short_t *StrawCode2,
	Short_t *TubeID,
	Double_t *xTube,
	Double_t *yTube,
	Double_t *zTube,
	Double_t *xxyyTube,
	// the following are the info of the track under scrutiny;
	Double_t Ox,
	Double_t Oy,
	Double_t R,
	Short_t Charge,
	Short_t *ListHits,
	Short_t nHits,
	Double_t R_STT_INNER_PAR_MAX,
	Short_t nScitilHitsInTrack,	// input, # of SciTil hits in the current track;
	Short_t* ListSciTilHitsinTrack,	// input, list of SciTil hits in the current track;
	Double_t posizSciTil[][3]	// input, info on all the SciTil position;
				)
{

   bool	connected,
	farthest_hit_is_boundary,
	good;

   Short_t
	auxListHits[MAXSTTHITSINTRACK],
	holes,
	i,
	j,
	k,
	nArcs_populated,

	tListInnerHitsLeft[nHits],
	tListInnerHitsRight[nHits],
	tListOuterHitsLeft[nHits],
	tListOuterHitsRight[nHits],
	tube_adjacent,
	tube_current,
	tube_next,
	tube_near,
	ListHitsInArc[MAXSTTHITSINTRACK][56],	// ordered list of hits in each Arc (from first to last
						// according to the charge of the particle;if the maximum
						//  # of Intersected Sector is 56, than the maximum # of Arcs is 28;

	nHitsInArc[56],	// number of hits in each Arc; the maximum # of Arcs is 56;
	nInnerHitsLeft,
	nInnerHitsRight,
	nOuterHitsLeft,
	nOuterHitsRight;
//	OrderedSectorList[56];	// ordered list of Sectors crossed (from first to last); each
				// Sector number is the Sector where the Arc lies;

   Double_t	dist2,
   		FiOrderedList[2],
   		FiStart,
		Xcross[2],
		Ycross[2];

   Vec <Short_t>
	ListInnerHitsLeft(tListInnerHitsLeft,nHits,"ListInnerHitsLeft"),
	ListInnerHitsRight(tListInnerHitsRight,nHits,"ListInnerHitsRight"),
	ListOuterHitsLeft(tListOuterHitsLeft,nHits,"ListOuterHitsLeft"),
	ListOuterHitsRight(tListOuterHitsRight,nHits,"ListOuterHitsRight");


   //  class with all the geometry calculations:
   PndTrkCTGeometryCalculations  GeometryCalculator;



 //------------------------------------------------------------------------------------------------------------
  // first of all, eliminate spurious with SciTil's since it is faster;

 // now check if there is an intersection with the SciTil;

 // calculate the intersection points in XY of the track trajectory with a circle tangent to the SciTil in
 // the middle of each SciTil tile; these can be 0 or 2;
 // if there are no intersections don't do anything; if there are 2 intersections check that there is a SciTil
 // hit in the proper place;
 //  FindIntersectionsOuterCircle returns -1 or 0;
 // if FindIntersectionsOuterCircle returns -1 there are no intersections, if it returns 0 there are 2;
   if( GeometryCalculator.FindIntersectionsOuterCircle(
		Ox,
		Oy,
		R,
		RADIUSSCITIL,
		Xcross,
		Ycross
		) >= 0 ){

	// there are 2 intersections;
	//  choose the first entrance point according to the charge of the particle;
	// ChooseEntranceExit3 works under the hypothesis that there are at least 2 intersections.

	FiStart = atan2(-Oy,-Ox);
	if( FiStart < 0.) FiStart  += TWO_PI;
	if( FiStart < 0.) FiStart = 0.;

	GeometryCalculator.ChooseEntranceExit3(
		Ox,
		Oy,
		Charge,
		FiStart,
		2,	// # of Intersections between track and Circle;
		Xcross,	// input and output; these are the intersections;
		Ycross,	// input and output; these are the intersections;
		FiOrderedList	// output  			
   					);

	// the intersection point must be close enough to at least 1 SciTil hit;

	good = false;
	for(i=0;i<nScitilHitsInTrack;i++){
		dist2 = (posizSciTil[ ListSciTilHitsinTrack[i] ][0] - Xcross[0])*
			(posizSciTil[ ListSciTilHitsinTrack[i] ][0] - Xcross[0])
						+
			(posizSciTil[ ListSciTilHitsinTrack[i] ][1] - Ycross[0])*
			(posizSciTil[ ListSciTilHitsinTrack[i] ][1] - Ycross[0]);

		if(dist2 < 2.25*DIMENSIONSCITIL*DIMENSIONSCITIL) { good = true; break;}// oversizing; in principle
					// dist2 should be < DIMENSIONSCITIL*DIMENSIONSCITIL/4 ;

  	} // end of for(i=0;i<nScitilHitsInTrack;i++)
	// failed to find a hit SciTil close to trajectory;
	if(!good) return false;
  }	// end of  if( GeometryCalculator.FindIntersectionsOuterCircle(

 // ----------------------------------------------------------------------------- end of check with SciTil's;



// now use the STT hits for cleaning;

 // Convention for the Sector number used in GoodTrack :
 // Sector = 1 --> Axial Outer Right
 // Sector = 2 --> Axial Inner Right
 // Sector = 3 --> Axial Inner Left
 // Sector = 4 --> Axial Outer Left


 // --------------------------------------------------------------------------------------------------

 // the intersections of the current track with the boundaries of each Axial Sector (Inner Left, Outer Left,
 // Inner Right, Outer Right) determine the number of Arcs in which the trajectory is subdivided;
 // for each Arc the number of ordered (according to the charge of the particle) axial STT hits are
 // found and listed;
 // nArcs_populated = # of Arcs (maximum possible  28 in the Left side + 28 in the Right Side of STT axial detector
 //  --->  56 maximum) populated by axial STT hits;
 // OrderedSectorList = ordered list, from last to first, of the Sectors corresponding to each Arc ;
 // nHitsInArc[56] = # of hits of the track belonging to each Arc (in order corresponding to the Sector order);
 // ListHitsInArc[MAXSTTHITSINTRACK][56] = list of hits in each Arc; this list is ordered clockwise or anticlockwise
 //						according to the charge;

   GeometryCalculator.ListAxialSectorsCrossedbyTrack_and_Hits(
					Ox,		// input;
					Oy,		// input;
					R,		// input;
					Charge,		// input;
					nHits,		// input;
					ListHits,	// input;
					info,		// input;
		nArcs_populated,	// output; # Arcs of trajectory populated by at least 1 axial hit; this is <= 28;
//		OrderedSectorList,	// output; ordered list of Sectors crossed (from first to last); each
//					// Sector number correspond to the Sector where the Arc lies;
		nHitsInArc,	// output; number of hits in each Sector; if the maximun # of Intersected Sector is 56,
					//  than the maximum # of Arcs is 28;

		ListHitsInArc // output; ordered list of hits in each Arc (from first to last
					// according to the charge of the particle;if the maximum
					//  # of Intersected Sector is 56, than the maximum # of Arcs is 28;
						);


//-------------------------------------------------
if(istampa>0){ cout<<"from XYCleanup,after ListAxialSectorsCrossedbyTrack_and_Hits :"<<endl<<
	"\tnArcs_populated "<<nArcs_populated<<", ordered list of Sectors crossed :"<<endl;
	for(int kg=0;kg<nArcs_populated;kg++){cout<<"\tSector  populated with "
		<<nHitsInArc[kg]<<"  hits; loro lista :"<<endl;
		for(int nn=0;nn<nHitsInArc[kg];nn++){cout<<"\t\taxial hit n. "<<ListHitsInArc[nn][kg]<<
		", R**2 "<< info[ListHitsInArc[nn][kg]][0]*info[ListHitsInArc[nn][kg]][0]+
		   info[ListHitsInArc[nn][kg]][1]*info[ListHitsInArc[nn][kg]][1] << endl;}
	};
 };
//-------------------------------------------------

// ------------------- starts analysis of the track; loop over the number of Arcs in which the track has been
//   subdivided and allow a maximum total number of holes of 1 per track;



   // Sector number convention:
   // 1 --> Right Axial Outer;
   // 2 --> Right Axial Inner;
   // 3 --> Left Axial Inner;
   // 4 --> Left Axial Outer;


   // # of "holes" in the track;
   holes = 0;

//   no_holes = false;

   // if the following flag is true the hit of the track furthest from the origin is at the boundary of
   // this sector;
   farthest_hit_is_boundary = false;

   // loop from the last Arc (the farthest according to the charge of the track) to the first one;
   for(i=nArcs_populated-1;i>=0; i--){
   	for(j=0;j<nHitsInArc[i];j++){auxListHits[j]=ListHitsInArc[j][i];}

 	if(  ! GoodTrack(
			info,				// input
			farthest_hit_is_boundary,	// input
			Ox,				// input; center of the current track;
			Oy,				// input; center of the current track;
			R,				// input; Radius of the current track;
			Charge,				// input; charge of the current track;
			nHitsInArc[i],			// input
			auxListHits,			// input
			StrawCode,			// input
			StrawCode2,			// input
			TubeID,				// input
			nParContiguous,			// input
			ListParContiguous,		// input
			xTube,				// input
			yTube,				// input
			zTube,				// input
			xxyyTube,			// input
			holes				// input and output
		)
	) { return false;}

	// now the first farthest hit in the next arc must be boundary;
	farthest_hit_is_boundary=true;
   }	// end of  for(i=nArcs_populated;i>0; i--)

 // ----------------------------------------------------------------------- 



   return true;
}
//----------end of function PndTrkCleanup::XYCleanup


ClassImp(PndTrkCleanup);
