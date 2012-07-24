#include "PndSdsMCPoint.h"
#include "PndTrkComparisonMCtruth.h"


#include "PndMCTrack.h"
// Root includes
#include "TROOT.h"
#include "TClonesArray.h"


#include <iostream>
#include <cmath>

using namespace std;
//----------begin of function PndTrkComparisonMCtruth::AssociateFoundTrackstoMCquater

 void PndTrkComparisonMCtruth::AssociateFoundTrackstoMCquater(
	Double_t BFIELD,
	Double_t CVEL,
	TClonesArray *fMCTrackArray,
	bool *keepit,
	Double_t info[][7],
	int MAXMVDPIXELHITSINTRACK,
	int MAXMVDSTRIPHITSINTRACK,
	int MAXSTTHITSINTRACK,
	Double_t *Ox,
	Double_t *Oy,
	Double_t *R,
	Double_t *X1,
	Double_t *Y1,
	Double_t *X2,
	Double_t *Y2,
	Double_t *X3,
	Double_t *Y3,
	Short_t  nTracksFoundSoFar,
	Short_t *nHitsinTrack,
	Short_t *ListHitsinTrack,
	Short_t *nSkewHitsinTrack,
	Short_t *ListSkewHitsinTrack,

	Short_t *nPixelHitsinTrack,
	Short_t *ListPixelHitsinTrack,
	Int_t   *FromPixeltoMCTrack,
	Short_t *nStripHitsinTrack,
	Short_t *ListStripHitsinTrack,
	Int_t   *FromStriptoMCTrack,
	Short_t *daTrackFoundaTrackMC
						)
{

 bool
	firstime,
	flaggo,
	inclusionMC[nTracksFoundSoFar]
		[MAXSTTHITSINTRACK+
		MAXMVDPIXELHITSINTRACK+
		MAXMVDSTRIPHITSINTRACK],
		inclusionExp[nTracksFoundSoFar];

   Short_t	ntoMCtrack[nTracksFoundSoFar],
		toMCtrackfrequency[nTracksFoundSoFar][MAXSTTHITSINTRACK];

   Short_t  i, j, jtemp,jexp , nmid;

   Short_t  itemp, massimo,
		toMCtracklist[nTracksFoundSoFar][MAXSTTHITSINTRACK];
   int nindex;

   Int_t enne;

	Double_t dx,
		 Cx,
		 Cy,
		 Rr,
		 alfa,
		 beta,
		 gamma,
		 minimo,
		 tanlow[nTracksFoundSoFar],
		 tanmid[nTracksFoundSoFar],
		 tanup[nTracksFoundSoFar],
		 toMCtrackdistance[nTracksFoundSoFar][MAXSTTHITSINTRACK];




   for(i=0; i<nTracksFoundSoFar;i++){
      daTrackFoundaTrackMC[i]=-1;
	if(!keepit[i]) continue;
     inclusionExp[i]=true;
	for(j=0; j<nHitsinTrack[i]+nSkewHitsinTrack[i]+nPixelHitsinTrack[i]
		+nStripHitsinTrack[i];j++){
		inclusionMC[i][j]=true;
	}

//--- find the minimum, middle, end Tan(angle) of this track, for the comparison later

	tanlow[i]=0.;
	tanmid[i]=0.;
	tanup [i]=0.;
	int nn = nHitsinTrack[i]+nPixelHitsinTrack[i]+nStripHitsinTrack[i];
	if( nn > 2 || nn == 1) {
		dx = X1[i] - Ox[i];
		if(fabs(dx)> 1.e-10){
			tanlow[i] = (Y1[i] - Oy[i] )/dx;
		} else {
			tanlow[i] = 999999.;
		}

		dx = X2[i] - Ox[i];
		if(fabs(dx)> 1.e-10){
			tanmid[i] = ( Y2[i] - Oy[i] )/dx;
		} else {
			tanmid[i] = 999999.;
		}

		dx = X3[i] - Ox[i];
		if(fabs(dx)> 1.e-10){
			tanup[i] = (Y3[i] - Oy[i] )/dx;
		} else {
			tanup[i] = 999999.;
		}

	} else if (nn==2) {	// continuation  of  if( nn > 2 || nn == 1)
		dx = X1[i] - Ox[i];
		if(fabs(dx)> 1.e-10){
			tanlow[i] = (Y1[i] - Oy[i] )/dx;
		} else {
			tanlow[i] = 999999.;
		}

		dx = X2[i] - Ox[i];
		if(fabs(dx)> 1.e-10){
			tanmid[i] = ( Y2[i] - Oy[i] )/dx;
		} else {
			tanmid[i] = 999999.;
		}

		tanup[i] = tanmid[i];

	}

/*
	 else{  // continuation of   if( nn > 2), case in which there is only 1 hit in the Track cand.
		dx = X1[i] - Ox[i];
		if(fabs(dx)> 1.e-10){
			tanlow[i] = (Y1[i] - Oy[i] )/dx;
		} else {
			tanlow[i] = 999999.;
		}
		tanmid[i] = tanlow[i];
		tanup[i] = tanlow[i];
	}	// end of  if( nn > 2)
*/
//-----------------------------

   }	// end of for(i=0; i<nTracksFoundSoFar;i++)





     for(jexp=0; jexp< nTracksFoundSoFar ;jexp++){
	if(!keepit[jexp]) continue;

	firstime=true;
	ntoMCtrack[jexp]=0;


// prima  gli hits paralleli ---------------------


	for(i=0; i<nHitsinTrack[jexp]; i++){
		nindex = jexp*MAXSTTHITSINTRACK + i;
		enne = (Int_t)( info[ *(ListHitsinTrack+nindex) ][6]+0.01 );
		if(enne<0) continue;   //  hit not associated to any MC track; noise hit.

		if(firstime) {
			toMCtracklist[jexp][0]= enne;
			toMCtrackfrequency[jexp][0]=1;
			firstime = false;

			getMCInfo( BFIELD, CVEL, &Cx, &Cy,fMCTrackArray,enne, &Rr);
			if( Rr<0.) {
				toMCtrackdistance[jexp][0]=-1.;
			} else {
				alfa = -2.*Cx;
				beta = -2.*Cy;
				gamma = Cx*Cx+Cy*Cy-Rr*Rr;
				toMCtrackdistance[jexp][0]= FindDistance(Ox[jexp],Oy[jexp],
				R[jexp],tanlow[jexp],tanmid[jexp],tanup[jexp],alfa,beta,gamma);
			}
			ntoMCtrack[jexp]=1;

		} else {	// continuation of  if(firstime)

			flaggo=true;
			for(j=0; j<ntoMCtrack[jexp]; j++){
				if( enne == toMCtracklist[jexp][j] ) {
					toMCtrackfrequency[jexp][j]++;
					flaggo=false;
					break;
				}
			}
			if(flaggo){
				toMCtracklist[jexp][ ntoMCtrack[jexp] ] = enne;
				toMCtrackfrequency[jexp][ ntoMCtrack[jexp] ] = 1;
				getMCInfo( BFIELD, CVEL, &Cx, &Cy,fMCTrackArray,enne, &Rr);
				if( Rr<0.) {
					toMCtrackdistance[jexp][ntoMCtrack[jexp]]=-1.;
				} else {
					alfa = -2.*Cx;
					beta = -2.*Cy;
					gamma = Cx*Cx+Cy*Cy-Rr*Rr;
					toMCtrackdistance[jexp][ntoMCtrack[jexp]]=
						FindDistance(Ox[jexp],Oy[jexp],R[jexp],
						tanlow[jexp],tanmid[jexp],tanup[jexp],
						alfa,beta,gamma);
				}
				ntoMCtrack[jexp]++;
			}  // end of if(flaggo)
		}

	}   //  end of for(i=0; i<nHitsinTrack[jexp]; i++)



// poi i pixel -------------------------------------------

	for(i=0; i<nPixelHitsinTrack[jexp]; i++){
		nindex = jexp*MAXMVDPIXELHITSINTRACK + i;
		enne = FromPixeltoMCTrack[ *(ListPixelHitsinTrack+nindex) ] ;
		if(enne<0) continue;   //  hit not associated to any MC track; noise hit.

		if(firstime) {
			toMCtracklist[jexp][0]= enne;
			toMCtrackfrequency[jexp][0]=1;
			firstime = false;

			getMCInfo(BFIELD, CVEL, &Cx, &Cy,fMCTrackArray,enne, &Rr);
			if( Rr<0.) {
				toMCtrackdistance[jexp][0]=-1.;
			} else {
				alfa = -2.*Cx;
				beta = -2.*Cy;
				gamma = Cx*Cx+Cy*Cy-Rr*Rr;
				toMCtrackdistance[jexp][0]= FindDistance(Ox[jexp],Oy[jexp],
				R[jexp],tanlow[jexp],tanmid[jexp],tanup[jexp],alfa,beta,gamma);
			}
			ntoMCtrack[jexp]=1;

		} else {	// continuation of  if(firstime)

			flaggo=true;
			for(j=0; j<ntoMCtrack[jexp]; j++){
				if( enne == toMCtracklist[jexp][j] ) {
					toMCtrackfrequency[jexp][j]++;
					flaggo=false;
					break;
				}
			}
			if(flaggo){
			toMCtracklist[jexp][ ntoMCtrack[jexp] ] = enne;
			toMCtrackfrequency[jexp][ ntoMCtrack[jexp] ] = 1;
			getMCInfo(BFIELD, CVEL, &Cx, &Cy,fMCTrackArray,enne, &Rr);
			if( Rr<0.) {
				toMCtrackdistance[jexp][ntoMCtrack[jexp]]=-1.;
			} else {
				alfa = -2.*Cx;
				beta = -2.*Cy;
				gamma = Cx*Cx+Cy*Cy-Rr*Rr;
				toMCtrackdistance[jexp][ntoMCtrack[jexp]]= FindDistance(Ox[jexp],Oy[jexp],
				R[jexp],tanlow[jexp],tanmid[jexp],tanup[jexp],alfa,beta,gamma);
			}
			ntoMCtrack[jexp]++;
			}  // end of if(flaggo)
		}

	}   //  end of for(i=0; i<nHitsinTrack[jexp]; i++)

// infine le strip -------------------------------------------

	for(i=0; i<nStripHitsinTrack[jexp]; i++){
		nindex = jexp*MAXMVDSTRIPHITSINTRACK + i;
		enne = FromPixeltoMCTrack[ *(ListPixelHitsinTrack+nindex) ] ;
//		enne = FromStriptoMCTrack[ ListStripHitsinTrack[jexp][i] ] ;
		if(enne<0) continue;   //  hit not associated to any MC track; noise hit.

		if(firstime) {
			toMCtracklist[jexp][0]= enne;
			toMCtrackfrequency[jexp][0]=1;
			firstime = false;

			getMCInfo( BFIELD, CVEL, &Cx, &Cy,fMCTrackArray,enne, &Rr);
			if( Rr<0.) {
				toMCtrackdistance[jexp][0]=-1.;
			} else {
				alfa = -2.*Cx;
				beta = -2.*Cy;
				gamma = Cx*Cx+Cy*Cy-Rr*Rr;
				toMCtrackdistance[jexp][0]= FindDistance(Ox[jexp],Oy[jexp],
				R[jexp],tanlow[jexp],tanmid[jexp],tanup[jexp],alfa,beta,gamma);
			}
			ntoMCtrack[jexp]=1;

		} else {	// continuation of  if(firstime)

			flaggo=true;
			for(j=0; j<ntoMCtrack[jexp]; j++){
				if( enne == toMCtracklist[jexp][j] ) {
					toMCtrackfrequency[jexp][j]++;
					flaggo=false;
					break;
				}
			}
			if(flaggo){
				toMCtracklist[jexp][ ntoMCtrack[jexp] ] = enne;
				toMCtrackfrequency[jexp][ ntoMCtrack[jexp] ] = 1;
				getMCInfo( BFIELD, CVEL, &Cx, &Cy,fMCTrackArray,enne, &Rr);
				if( Rr<0.) {
					toMCtrackdistance[jexp][ntoMCtrack[jexp]]=-1.;
				} else {
					alfa = -2.*Cx;
					beta = -2.*Cy;
					gamma = Cx*Cx+Cy*Cy-Rr*Rr;
					toMCtrackdistance[jexp][ntoMCtrack[jexp]]=
						FindDistance(Ox[jexp],Oy[jexp],R[jexp],
						tanlow[jexp],tanmid[jexp],tanup[jexp],
						alfa,beta,gamma);
				}
				ntoMCtrack[jexp]++;
			} // end of if(flaggo)
		}

	}   //  end of for(i=0; i<nHitsinTrack[jexp]; i++)

     }  // end of  for(jexp=0; jexp< nTracksFoundSoFar ;jexp++)

//--------------------------------------------------------------------
     itemp=0;
     while ( itemp > -1){
	itemp=-1;
	massimo = -1;
	minimo = 999999999999.;
	for(jexp=0; jexp< nTracksFoundSoFar ;jexp++){
		if(!keepit[jexp]) continue;
		if( !inclusionExp[jexp])  continue;
		for(i=0; i< ntoMCtrack[jexp]; i++){
			if( !inclusionMC[jexp][i])  continue;


			// if( toMCtrackfrequency[jexp][i]>massimo){
			if( toMCtrackdistance[jexp][i]<-0.5) continue;
			if( toMCtrackdistance[jexp][i]<minimo){
				massimo=toMCtrackfrequency[jexp][i];
				minimo=toMCtrackdistance[jexp][i];
				itemp = toMCtracklist[jexp][i];
				jtemp = jexp;
			}
		}
	}  // end of   for(jexp=0; jexp< nTracksFoundSoFar ;jexp++)
	if( itemp>-1 ){
		daTrackFoundaTrackMC[jtemp]=itemp;
		inclusionExp[jtemp]=false;
		for(jexp=0; jexp<nTracksFoundSoFar;jexp++){
			if(!keepit[jexp]) continue;
			for(int jk=0;jk<ntoMCtrack[jexp];jk++){
				if( itemp==toMCtracklist[jexp][jk]){
					inclusionMC[jexp][jk]=false;
				}
			}
		}
	}
     }    //    end while ( itemp > -1)

  return;


}






//----------end of function PndTrkComparisonMCtruth::AssociateFoundTrackstoMCquater


//----------begin of function PndTrkComparisonMCtruth::ComparisonwithMC

int PndTrkComparisonMCtruth::ComparisonwithMC(
	PndTrkComparisonMCtruth_io_Data ioData
					)
{

 int
	i,
	j,
	nele,
	ncand,
	nMCTracks,
	nMvdMCPoint;

 Double_t	dista;

 const double PI=3.141592654;


 Double_t BFIELD = ioData.Bfield;
 Short_t *Charge = ioData.Charge;
 Double_t CVEL = ioData.Cvel;
 Short_t *daTrackFoundaTrackMC = ioData.daTrackFoundaTrackMC;
 Double_t ERRORSQPIXEL = ioData.Errorsqpixel;
 Double_t ERRORSQSTRIP = ioData.Errorsqstrip;
 Double_t *FI0 = ioData.FI0;
 TClonesArray *fMCTrackArray = ioData.fMCTrackArray;
 TClonesArray *fMvdMCPointArray = ioData.fMvdMCPointArray;
 FILE * HANDLE = ioData.HANDLE;
 FILE * HANDLE2 = ioData.HANDLE2;
 Double_t  (*info)[7] = ioData.info;
 int       istampa = ioData.istampa;
 int       IVOLTE = ioData.IVOLTE;
 Double_t *KAPPA = ioData.KAPPA;
 bool     *keepit = ioData.keepit;
 bool     *InclusionListStt = ioData.InclusionListStt;
 Short_t  *ListMvdPixelHitsinTrack =  ioData.ListMvdPixelHitsinTrack;
 Short_t  *ListMvdStripHitsinTrack =  ioData.ListMvdStripHitsinTrack;
 Short_t  *ListSttParHitsinTrack =  ioData.ListSttParHitsinTrack;
 Short_t  *ListSttSkewHitsinTrack =  ioData.ListSttSkewHitsinTrack;
 Short_t  *ListTrackCandHit = ioData.ListTrackCandHit;
 Short_t  *ListTrackCandHitType = ioData.ListTrackCandHitType;
 int       MAXMCTRACKS = ioData.MAXMCTRACKS;
 int       MAXMVDPIXELHITSINTRACK = ioData.MAXMVDPIXELHITSINTRACK;
 int       MAXMVDMCPOINTS = ioData.Maxmvdmcpoints;
 int       MAXMVDSTRIPHITSINTRACK = ioData.MAXMVDSTRIPHITSINTRACK;
 int       MAXSCITILHITSINTRACK = ioData.MAXSCITILHITSINTRACK;
 int	   MAXSTTHITS = ioData.MAXSTTHITS;
 int	   MAXSTTHITSINTRACK = ioData.MAXSTTHITSINTRACK;
 int	   MAXTRACKSPEREVENT = ioData.MAXTRACKSPEREVENT;
 Short_t  *MCMvdPixelAloneList = ioData.MCMvdPixelAloneList;
 Short_t  *MCMvdStripAloneList = ioData.MCMvdStripAloneList;
 Short_t  *MCParalAloneList = ioData.MCParalAloneList;
 Short_t  *MCSkewAloneList = ioData.MCSkewAloneList;
 Short_t  *MvdPixelCommonList = ioData.MvdPixelCommonList;
 Short_t  *MvdPixelSpuriList = ioData.MvdPixelSpuriList;
 Short_t  *MvdStripCommonList = ioData.MvdStripCommonList;
 Short_t  *MvdStripSpuriList = ioData.MvdStripSpuriList;
 Short_t  *nHitsInMCTrack = ioData.nHitsInMCTrack;
 Short_t  *nMCMvdPixelAlone = ioData.nMCMvdPixelAlone;
 Short_t  *nMCMvdStripAlone = ioData.nMCMvdStripAlone;
 Short_t  *nMCParalAlone = ioData.nMCParalAlone;
 Short_t  *nMCSkewAlone = ioData.nMCSkewAlone;
 Short_t  *nMvdPixelCommon = ioData.nMvdPixelCommon;
 Short_t  *nMvdPixelHitsinTrack = ioData.nMvdPixelHitsinTrack;
 Short_t  *nMvdStripHitsinTrack = ioData.nMvdStripHitsinTrack;
 Short_t   nMvdPixelHit = ioData.nMvdPixelHit;
 Short_t  *nMvdPixelSpuriinTrack = ioData.nMvdPixelSpuriinTrack;
 Short_t  *nMvdStripCommon = ioData.nMvdStripCommon;
 Short_t   nMvdStripHit = ioData.nMvdStripHit;
 Short_t  *nMvdStripSpuriinTrack = ioData.nMvdStripSpuriinTrack;
 Short_t  *nParalCommon = ioData.nParalCommon;
 Short_t  *nSkewCommon = ioData.nSkewCommon;
 Short_t  *nSkewHitsInMCTrack = ioData.nSkewHitsInMCTrack;
 Short_t  *nSpuriParinTrack = ioData.nSpuriParinTrack;
 Short_t  *nSpuriSkewinTrack = ioData.nSpuriSkewinTrack;
 Int_t     nSttHit = ioData.nSttHit;
 Short_t  *nSttParHitsinTrack = ioData.nSttParHitsinTrack;
 Short_t  *nSttSkewHitsinTrack = ioData.nSttSkewHitsinTrack;
 Short_t   nTotalCandidates = ioData.nTotalCandidates;
 Double_t *Ox = ioData.Ox;
 Double_t *Oy = ioData.Oy;
 Short_t  *ParalCommonList = ioData.ParalCommonList;
 Short_t  *ParSpuriList = ioData.ParSpuriList;
 Double_t *R = ioData.R;
 Double_t *refindexMvdPixel = ioData.refindexMvdPixel;
 Double_t *refindexMvdStrip = ioData.refindexMvdStrip;
 Short_t  *resultFitSZagain = ioData.resultFitSZagain;
 Short_t  *SkewCommonList = ioData.SkewCommonList;
 Short_t  *SkewSpuriList = ioData.SkewSpuriList;
 bool     *SttSZfit = ioData.SttSZfit;
 Double_t *XMvdPixel = ioData.XMvdPixel;
 Double_t *XMvdStrip = ioData.XMvdStrip;
 Double_t *YMvdPixel = ioData.YMvdPixel;
 Double_t *YMvdStrip = ioData.YMvdStrip;
 Double_t *ZMvdPixel = ioData.ZMvdPixel;
 Double_t *ZMvdStrip = ioData.ZMvdStrip;


 nele = MAXSTTHITSINTRACK+
			MAXMVDPIXELHITSINTRACK+
			MAXMVDSTRIPHITSINTRACK+
			MAXSCITILHITSINTRACK;


//----------   fetching the MC truth tracks
	nMCTracks = fMCTrackArray->GetEntriesFast(); // num. tracce/evento
	if (istampa >= 2) {
	     cout<<"da PndTracking  : evento (partendo da 0)  N. "<<
	       IVOLTE<< "\n       N. di MC truth tracks  : "<<nMCTracks<<endl;
	}

	if (nMCTracks ==0){
		cout<<"da PndTracking  :  N. di MC truth tracks = 0, return!\n"<<endl;
		return 0;
	} else if(nMCTracks> MAXMCTRACKS){
		cout<<"da PndTracking  :  N. di MC truth tracks = "<<nMCTracks
		<<" and it is > MAXMCTRACKS = "<<MAXMCTRACKS
		<<", setting it equal to MAXMCTRACKS.\n";
		nMCTracks = MAXMCTRACKS;
	}

 if(istampa>=2) stampaMCTracks(
	BFIELD,
	CVEL,
	fMCTrackArray,
	nMCTracks
	);

// ---------------------------------------------  get MC Points of  MVD

 nMvdMCPoint = fMvdMCPointArray->GetEntriesFast();
  if(nMvdMCPoint>MAXMVDMCPOINTS) {
	cout<<"from PndTracking, nMvdMCPoint = "<<nMvdMCPoint
	<<" and it is > the maximum number allowed ("<<MAXMVDMCPOINTS<<
	")"<<
	", setting nMvdMCPoint to "<<MAXMVDMCPOINTS<<endl;
	nMvdMCPoint=MAXMVDMCPOINTS;
 }
 if(istampa>=2) cout<<"N. MC Points delle Mvd = "<<nMvdMCPoint<<endl;

//--------------------  comparison of the PndTrackCand from PR of the STT to the MC truth
//    associate the tracks found with Pattern Recognition to the MC tracks


//  the following method associates the Mvd hits to corresponding MC tracks

   Int_t	FromPixeltoMCTrack[nMvdPixelHit],
		FromStriptoMCTrack[nMvdStripHit];

	MvdMatchtoMC(
		ERRORSQPIXEL,
		ERRORSQSTRIP,
		fMvdMCPointArray,
		nMvdMCPoint,
		istampa,
		IVOLTE,
		nMvdPixelHit,
		nMvdStripHit,
		refindexMvdPixel,
		refindexMvdStrip,
		XMvdPixel,
		XMvdStrip,
		YMvdPixel,
		YMvdStrip,
		ZMvdPixel,
		ZMvdStrip,
		FromPixeltoMCTrack,	// output
		FromStriptoMCTrack	// output
		);

//  this section associates the found tracks  to the
//  MC tracks, creating a bilinear correspondence between MC tracks and PR Found tracks



 if( nMCTracks >0 && nTotalCandidates > 0){

	int nmid,nn;

	Double_t X1[MAXTRACKSPEREVENT],
		 Y1[MAXTRACKSPEREVENT],
		 X2[MAXTRACKSPEREVENT],
		 Y2[MAXTRACKSPEREVENT],
		 X3[MAXTRACKSPEREVENT],
		 Y3[MAXTRACKSPEREVENT];

	for(i=0; i<nTotalCandidates;i++){
		if(!keepit[i]) continue;


		nn = nSttParHitsinTrack[i]+nSttSkewHitsinTrack[i]+
			nMvdPixelHitsinTrack[i]+nMvdStripHitsinTrack[i];
		// assume that the point on trajectory at Z=0 is
		// the point of closest approach to (0,0,0)
		X1[i] = Ox[i] + R[i]*cos( FI0[i]);
		Y1[i] = Oy[i] + R[i]*sin( FI0[i]);

		//  the third point on trajectory is given by the last hit

//		switch ( ListTrackCandHitType[i][nn-1]){
		switch ( *(ListTrackCandHitType+i*nele+nn-1)){
		case 0 :	// Pixel
			X3[i] = XMvdPixel[ *(ListTrackCandHit+i*nele+nn-1) ] ;
			Y3[i] = YMvdPixel[ *(ListTrackCandHit+i*nele+nn-1) ] ;
		break;
		case 1 :	// Strip
			X3[i] = XMvdStrip[ *(ListTrackCandHit+i*nele+nn-1) ] ;
			Y3[i] = YMvdStrip[ *(ListTrackCandHit+i*nele+nn-1) ] ;
		break;
		case 2 :	// Straw parallel
			X3[i] = info[ *(ListTrackCandHit+i*nele+nn-1) ][0] ;
			Y3[i] = info[ *(ListTrackCandHit+i*nele+nn-1) ][1] ;
		break;
		case 3 :	// Straw skew
			X3[i] = info[ *(ListTrackCandHit+i*nele+nn-1) ][0] ;
			Y3[i] = info[ *(ListTrackCandHit+i*nele+nn-1) ][1] ;
		break;
		};


		//  the 2nd point on the trajectory is given by the middle between
		//  first and third point.

		Double_t angle, middle;
		angle = atan2(Y3[i]-Oy[i], X3[i]-Ox[i]);
		if(angle <0.) angle+= 2.*PI;
		if(angle <0.) angle = 0.;

		if( Charge[i]>0 ){ //  clockwise.
			if( FI0[i] < angle ) angle -= 2.*PI;
			if( FI0[i] < angle ) angle =0.;
		} else{  // counterclockwise.
			if( FI0[i] > angle ) angle += 2.*PI;
			if( FI0[i] > angle ) angle = FI0[i];
		}
			middle = (FI0[i]+angle)/2.;

		X2[i] = Ox[i] + R[i]*cos( middle );
		Y2[i] = Oy[i] + R[i]*sin( middle );


	}	// end of  for(i=0; i<nTotalCandidates;i++)


	if(nTotalCandidates>0) AssociateFoundTrackstoMCquater(
		BFIELD,
		CVEL,
		fMCTrackArray,
		keepit,
		info,
		MAXMVDPIXELHITSINTRACK,
		MAXMVDSTRIPHITSINTRACK,
		MAXSTTHITSINTRACK,
		Ox,
		Oy,
		R,
		X1,
		Y1,
		X2,
		Y2,
		X3,
		Y3,
		nTotalCandidates,
		nSttParHitsinTrack,
		ListSttParHitsinTrack,
		nSttSkewHitsinTrack,
		ListSttSkewHitsinTrack,
		nMvdPixelHitsinTrack,
		ListMvdPixelHitsinTrack,
		FromPixeltoMCTrack,
		nMvdStripHitsinTrack,
		ListMvdStripHitsinTrack,
		FromStriptoMCTrack,
		daTrackFoundaTrackMC
		);

 }  // end   if( nMCTracks >0 && nTotalCandidates > 0)


//---------- stampe.
 if(istampa>=3){
	for(i=0;i<nTotalCandidates;i++){
		cout<<"from PndTrkComparisonMCtruth : Track candidate n. "<<i;
		if(keepit[i]){
		  cout<<"  associated to MC Track n. "<<daTrackFoundaTrackMC[i]<<endl;
		} else {
		  cout<<"  has keepit  false!\n";
		}
	}
 }
//--------- fine stampe.


//  the following method counts the matched to MC and spurious hits for all STT  Found tracks and
//  also the ghost tracks


 SttMatchedSpurious(
	daTrackFoundaTrackMC,
	InclusionListStt,
	info,
	keepit,
	MAXSTTHITS,
	MAXSTTHITSINTRACK,
	MAXTRACKSPEREVENT,
	ListSttParHitsinTrack, // from PR
	ListSttSkewHitsinTrack, // from PR
	MCParalAloneList,
	MCSkewAloneList,
	nHitsInMCTrack,
	nSttParHitsinTrack, // n. hits PARALLEL from PR
	nMCParalAlone,
	nMCSkewAlone,
	nParalCommon,
	nSkewCommon,
	nSkewHitsInMCTrack,
	nSttSkewHitsinTrack, // n. hits skew, from PR
	nSpuriParinTrack,
	nSpuriSkewinTrack,
	nSttHit,
	nTotalCandidates, // those found by PR
	ParalCommonList,
	ParSpuriList,
	SkewCommonList,
	SkewSpuriList
	);

//--------------------  end comparison of the PndTrackCand from PR of the STT to the MC truth

//	assumo che la traccia MC associata alla traccia trovata dal Pattern Recognition
//	sia quella giusta e di
//	conseguenza calcolo gli hits Mvd spuri e comuni



 MvdMatchedSpurioustoTrackCand(
	daTrackFoundaTrackMC,	// input

	FromPixeltoMCTrack,		// input
	FromStriptoMCTrack,		// input
	keepit,		// input

	ListMvdPixelHitsinTrack,	// input
	ListMvdStripHitsinTrack,	// input
	MAXMVDPIXELHITSINTRACK,
	MAXMVDSTRIPHITSINTRACK,
	nMvdPixelHit,
	nMvdStripHit,
	nMvdPixelHitsinTrack,	// input
	nMvdStripHitsinTrack,	// input
	nTotalCandidates,		// input

	nMvdPixelCommon,		// output
	MvdPixelCommonList,	// output
	nMvdPixelSpuriinTrack,	// output
	MvdPixelSpuriList,	// output
	nMCMvdPixelAlone,		// output
	MCMvdPixelAloneList,	// output

	nMvdStripCommon,		// output
	MvdStripCommonList,	// output
	nMvdStripSpuriinTrack,	// output
	MvdStripSpuriList,	// output
	nMCMvdStripAlone,		// output
	MCMvdStripAloneList	// output
			);
// ora il confronto per il meeting di  Groningen

//---------- conteggio delle tracce MC accettabili!!

int citata;
int nMCTracksaccettabili=0;
int ListaMCTracksaccettabili[nMCTracks];
PndMCTrack* pMCtr;

for (i=0;i<nMCTracks;i++){
	citata=0;
   	pMCtr = (PndMCTrack*) fMCTrackArray->At(i);
   	if ( ! pMCtr ) continue;
         Double_t aaa, carica, Rr, Dd, Fifi, Oxx, Oyy, Cx, Cy, Pxx, Pyy  ;
         Int_t icode;
         icode  = pMCtr->GetPdgCode() ;    //   PDG code of track
         Oxx = pMCtr->GetStartVertex().X();    //   X of starting point track
         Oyy = pMCtr->GetStartVertex().Y();    //   Y of starting point track
         Pxx = pMCtr->GetMomentum().X();
         Pyy = pMCtr->GetMomentum().Y();
         aaa = sqrt( Pxx*Pxx + Pyy*Pyy);
         Rr =   aaa*1000./(BFIELD*CVEL);    //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla



	if(istampa>2){
		TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
		TParticlePDG *fParticle= fdbPDG->GetParticle(icode);
		if (icode>1000000000) carica = 1.;
		else  carica = fParticle->Charge()/3. ;    //   charge of track
		if(fabs(carica)<1.e-5) continue;
		Cx = Oxx + Pyy*1000./(BFIELD*CVEL*carica);
		Cy = Oyy - Pxx*1000./(BFIELD*CVEL*carica);
		cout<<"da PndTracking, evento (cominciando da 0) n. "<<IVOLTE<<
		",  traccia MC n. "<<i<<",  R MC = "<<Rr<<", Centro X = "<<Cx
		<<", Centro Y = "<<Cy<<endl;
	}



	for(int ic=0;ic<nSttHit;ic++){
		if( ( (int) (info[ic][6]+0.1) ) == i   && info[ic][5]<2.){
			citata++;
		}
	}
	if( citata>2 && fabs(Oxx)<1. && fabs(Oyy) < 1. ) {
		ListaMCTracksaccettabili[nMCTracksaccettabili]=i;
		nMCTracksaccettabili++;
	}


}  // end of  for (i=0;i<nMCTracks;i++)


//----------- fine conteggio delle tracce MC accettabili


   fprintf(HANDLE, "\n Evento %d  NTotaleTracceMC %d ------\n",IVOLTE,
  	nMCTracksaccettabili);
	int ibene=0;
	if(nMCTracksaccettabili>0){
		for(int ii=0; ii<nTotalCandidates;ii++){
			for(i=0;i<nMCTracksaccettabili;i++){
				if( daTrackFoundaTrackMC[ii]==ListaMCTracksaccettabili[i]){
					ibene++;
				}
			}
		}
	}
	if(ibene>0) fprintf(HANDLE,"\tn. volte almeno 1 traccia MC accettabile e' ricostruita %d\n"
		,ibene);
bool flaggo;
int ii, ibuone=-1;
Double_t HoughFiii;
for (ii=0; ii<nTotalCandidates  ;ii++){
   if(!keepit[ii]) { if(istampa>1) {cout<<"\tevt. n "<<IVOLTE<<", cand. "<<ii<<" ha keepit false."<<
   	endl;}; continue;}
   ibuone++;
   fprintf(HANDLE,"----------------------------------------------------------\n");
   i=daTrackFoundaTrackMC[ii];



   if( i <0  ) {
    fprintf(HANDLE,
"   No TracciaMC associated to found track n. %d in pattern recognition, with %d Hits ||, %d skew hits, %f Radius \n "
         ,ibuone,nSttParHitsinTrack[ii], nSttSkewHitsinTrack[ii],  R[ii] );
            continue;
         }
   if( ( !SttSZfit[ii] )&&(resultFitSZagain[ii] !=1 )) {
    fprintf(HANDLE,
"       TracciaMC %d; sua FoundTrack associata (n. %d) NONsoddisfaRequisitiMinimi perche' in Z-S e' fallito il fit \n"
	,i,ibuone);
            continue;
         }
   if(fabs(KAPPA[ii])<1.e-20 ){
    fprintf(HANDLE,
"       TracciaMC %d; sua FoundTrack associata (n. %d) NONsoddisfaRequisitiMinimi perche' KAPPA troppo piccolo; KAPPA = %g\n",
          i,ibuone,KAPPA[ii]);
           continue;
   }
   	dista=sqrt( Ox[ii]*Ox[ii]+Oy[ii]*Oy[ii] );
   if(fabs(dista)<1.e-20 ){
    fprintf(HANDLE,
"       TracciaMC %d; sua FoundTrack associata (n. %d) NONsoddisfaRequisitiMinimi perche' centro Helix Cilinder trovato dista solo %g da (0,0)\n",
           i,ibuone,dista);
           continue;
   }
   pMCtr = (PndMCTrack*) fMCTrackArray->At(i);
   if ( ! pMCtr ){
		fprintf(HANDLE,
		"       MC track n. %d doesn't have pointer to MC Track TClones Array\n",
		i);
	  continue;
   }

//   controllo che la traccia associata MC sia una delle tracce MC 'ragionevoli'.

	flaggo = true;
	for(int g=0; g<nMCTracksaccettabili;g++){
		if( i==ListaMCTracksaccettabili[g])
		{
			flaggo=false;
			break;
		}
	}
	if(flaggo) continue;



    fprintf(HANDLE,
"       TracciaMC %d ParHitsMC %d ParMecc %d ParMeccSpuri %d SkewHitsMC %d  SkewMecc %d SkewMeccSpuri %d\n",
             i,
             nHitsInMCTrack[ii],
             nParalCommon[ii],
             nSpuriParinTrack[ii],
             nSkewHitsInMCTrack[ii],
             nSkewCommon[ii],
             nSpuriSkewinTrack[ii]

           )  ;
    fprintf(HANDLE,
"       e corrisponde a track found n. %d\n", ibuone );
    fprintf(HANDLE,
"       AVENDO %d hits paralleli e %d hits skew non mecciati dalla corrisponde track found\n"
       , nMCParalAlone[ii],nMCSkewAlone[ii]  );

    HoughFiii =  atan2(Oy[ii],Ox[ii]);
    if(HoughFiii<0.)  HoughFiii += 2.*PI;

         Double_t aaa, carica, Rr, Dd, Fifi, Oxx, Oyy, Cx, Cy, Pxx, Pyy  ;
         Int_t icode;
         icode  = pMCtr->GetPdgCode() ;    //   PDG code of track
         Oxx = pMCtr->GetStartVertex().X();    //   X of starting point track
         Oyy = pMCtr->GetStartVertex().Y();    //   Y of starting point track
         Pxx = pMCtr->GetMomentum().X();
         Pyy = pMCtr->GetMomentum().Y();
         aaa = sqrt( Pxx*Pxx + Pyy*Pyy);
         Rr =   aaa*1000./(BFIELD*CVEL);    //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla
         TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
         TParticlePDG *fParticle= fdbPDG->GetParticle(icode);
         if (icode>1000000000) carica = 1.;
         else  carica = fParticle->Charge()/3. ;    //   charge of track
	 if(fabs(carica)<1.e-5) fprintf(HANDLE,"       MC track n. %d e' neutra, assurdo!\n",i);
         Cx = Oxx + Pyy*1000./(BFIELD*CVEL*carica);
         Cy = Oyy - Pxx*1000./(BFIELD*CVEL*carica);
         Fifi = atan2(Cy, Cx);       // MC truth Fifi angle of circle of Helix trajectory
         if(Fifi<0.)  Fifi += 2.*PI;
         Double_t Kakka ;
         if( fabs( pMCtr->GetMomentum().Z() )< 1.e-20) Kakka = 99999999.;
         else  Kakka = -carica*0.001*BFIELD*CVEL/pMCtr->GetMomentum().Z();

    fprintf(HANDLE,
"       R_MC %g R %g Fi_MC %g Fi %g KAPPA_MC %g KAPPA %g FI0_MC %g FI0 %g\n",
     Rr,
     R[ ii ],
     Fifi,
     HoughFiii,
     Kakka,
     KAPPA[ ii ],
     fmod(Fifi+ PI, 2.*PI),  //  FI0  da MC truth
     FI0[ ii ]
           );
     
//------------------------


    fprintf(HANDLE2,"Evento n. %d Found track %d messa in PndTrackCand",IVOLTE, ii);
    fprintf(HANDLE2,
"       R_MC %g R %g Fi_MC %g Fi %g KAPPA_MC %g KAPPA %g FI0_MC %g FI0 %g\n",
     Rr,
     R[ ii ],
     Fifi,
     HoughFiii,
     Kakka,
     KAPPA[ ii ],
     fmod(Fifi+ PI, 2.*PI),
     FI0[ ii ]
           );

//----------------------------------


  }   //   end of  for (ii=0; ii<nTotalCandidates  ;ii++)


//--------------ghosts

// fa il conto delle ghost solo sugli eventi che hanno almeno 1 traccia MC accettabile.
 int NParghost=0, NParhitsghost=0,icc;
 ibuone=-1;
 if( nMCTracksaccettabili>0){

    for(icc=0; icc<nTotalCandidates;icc++){
	if(!keepit[icc]) continue;
	ibuone++;
       if( daTrackFoundaTrackMC[icc] == -1){
          NParghost++;
          NParhitsghost += nSttParHitsinTrack[icc]+nSttSkewHitsinTrack[icc];

          fprintf(HANDLE,"          tracce Trovata n. %d e' Ghost\n",ibuone);
       }
    }
    fprintf(HANDLE,
"          tracceGhostTrovate %d TotaleHitsGhost %d  ----\n",
            NParghost,
            NParhitsghost
           );

    fprintf(HANDLE,"----------------------------------------------------------\n");


}  //   end of    if( nMCTracksaccettabili>0)


 return nMCTracks;
}

//----------end of function PndTrkComparisonMCtruth::ComparisonwithMC

//------------------------- begin of function  PndTrkComparisonMCtruth::FindDistance

Double_t PndTrkComparisonMCtruth::FindDistance(
	Double_t Oxx,	//  center from wich distance is calculated
	Double_t Oyy,	//  center from wich distance is calculated
	Double_t Rr,
	Double_t tanlow,
	Double_t tanmid,
	Double_t tanup,
	Double_t alfa,	//  intersection circumference parameter
	Double_t beta,	//  intersection circumference parameter
	Double_t gamma	//  intersection circumference parameter
			)
{
	Short_t i,
		 n;

	Double_t Delta,
		 m[3],
		 q,
		 dist,
		 dist1,
		 dist2,
		 distlow,
		 distmid,
		 distup,
		 totaldist,
		 x1,
		 x2,
		 y1,
		 y2;

int nevento=1;





	m[0] = tanlow;
	m[1] = tanmid;
	m[2] = tanup;

	n=0;
	totaldist=0.;


    for(i=0;i<3;i++){

	if( tanlow<999998.) {
		q = Oyy-m[i]*Oxx;
		Delta = alfa*alfa + beta*beta*m[i]*m[i] - 4.*gamma*m[i]*m[i] - 4.*q*q + 4.*m[i]*q*alfa +
			+ 2.*alfa*beta*m[i] - 4.*beta*q - 4.*gamma;
		if( Delta < 0.){
			dist = -1.;
		} else if (Delta==0.){
			x1 = 0.5*(-alfa - 2.*m[i]*q - beta*m[i] )/(1.+m[i]*m[i]);
			y1 = m[i]*x1+q;
			dist = fabs( sqrt( (Oxx-x1)*(Oxx-x1) + (Oyy-y1)*(Oyy-y1) ) - Rr);
		} else {
			Delta = sqrt(Delta);
			x1 = 0.5*(-alfa - 2.*m[i]*q - beta*m[i] - Delta)/(1.+m[i]*m[i]);
			x2 = 0.5*(-alfa - 2.*m[i]*q - beta*m[i] + Delta)/(1.+m[i]*m[i]);
			y1 = m[i]*x1+q;
			y2 = m[i]*x2+q;
			dist1 = fabs( sqrt( (Oxx-x1)*(Oxx-x1) + (Oyy-y1)*(Oyy-y1) ) - Rr);
			dist2 = fabs( sqrt( (Oxx-x2)*(Oxx-x2) + (Oyy-y2)*(Oyy-y2) ) - Rr);
			if(dist1<dist2){
				dist = dist1;
			} else{
				dist = dist2;
			}
		}
	} else {
		Delta =  beta*beta - 4.*Oxx*Oxx - 4.*Oxx*alfa - 4.*gamma;

		if( Delta < 0.){
			dist = -1.;
		} else if (Delta==0.){
			dist = fabs( fabs(Oyy+0.5*beta) - Rr);
		} else {
			Delta = sqrt(Delta);
			y1 = -0.5*beta + Delta/2.;
			y2 = -0.5*beta - Delta/2.;
			dist1 = fabs( fabs(Oyy+0.5*beta + Delta/2.) - Rr);
			dist2 = fabs( fabs(Oyy+0.5*beta - Delta/2.) - Rr);
			if(dist1<dist2) dist = dist1;
			else  dist = dist2;
		}
	}

	if( dist>-0.5) {
		totaldist+=dist;
		n++;
	}


    }	//   end of  for(i=0;i<3;i++)


	if(n!=3)  totaldist = -1.;
	else	totaldist = totaldist / n;

	return totaldist;

}



//------------------------- end of function  PndTrkComparisonMCtruth::FindDistance


//------------------------- begin of function  PndTrkComparisonMCtruth::getMCInfo

void PndTrkComparisonMCtruth::getMCInfo(
	Double_t BFIELD,
	Double_t CVEL,
	Double_t * Cx,
	Double_t * Cy,
	TClonesArray *fMCTrackArray,
	Int_t MCTrack,
	Double_t * Rr
	)
{
		Int_t icode;
		Double_t aaa, Dd, Fifi, Oxx, Oyy, Pxx, Pyy, carica  ;
		PndMCTrack* pMC;


		if( MCTrack <0) {
			*Rr=-2.;
			return;
		}


		pMC = (PndMCTrack*) fMCTrackArray->At(MCTrack);
		if ( pMC ) {
			icode  = pMC->GetPdgCode() ;    //   PDG code of track
			Oxx = pMC->GetStartVertex().X();    //   X of starting point track
			Oyy = pMC->GetStartVertex().Y();    //   Y of starting point track
			Pxx = pMC->GetMomentum().X();
			Pyy = pMC->GetMomentum().Y();
			aaa = sqrt( Pxx*Pxx + Pyy*Pyy);
			*Rr = aaa*1000./(BFIELD*CVEL);    //   R (cm) of Helix of track
						      //   projected in XY plane; B = 2 Tesla
			TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
			TParticlePDG *fParticle= fdbPDG->GetParticle(icode);
			if (icode>1000000000) carica = 1.;
			else  carica = fParticle->Charge()/3. ;    //   charge of track
			if(fabs(carica)<1.e-5) { *Rr = -3.; return;}
			*Cx = Oxx + Pyy*1000./(BFIELD*CVEL*carica);
			*Cy = Oyy - Pxx*1000./(BFIELD*CVEL*carica);
		} else {
			*Rr = -1.;
		}



	return;
}

//------------------------- end of function  PndTrkComparisonMCtruth::getMCInfo




//------------------ begin function  PndTrkComparisonMCtruth::MvdMatchedSpurioustoTrackCand
void PndTrkComparisonMCtruth::MvdMatchedSpurioustoTrackCand(
	Short_t *daTrackFoundaTrackMC,	// input
	Int_t *FromPixeltoMCTrack,	// input
	Int_t *FromStriptoMCTrack,	// input
	bool *keepit,			// input
	Short_t *ListPixelHitsinTrack,// input
	Short_t *ListStripHitsinTrack,// input
	int MAXMVDPIXELHITSINTRACK,
	int MAXMVDSTRIPHITSINTRACK,
	Short_t nMvdPixelHit,
	Short_t nMvdStripHit,
	Short_t *nPixelHitsinTrack,	// input
	Short_t *nStripHitsinTrack,	// input
	Short_t nSttTrackCand,		// input

	Short_t *nMvdPixelCommon,		// output
	Short_t *MvdPixelCommonList,		// output
	Short_t *nMvdPixelSpuriinTrack,	// output
	Short_t *MvdPixelSpuriList,		// output
	Short_t *nMCMvdPixelAlone,		// output
	Short_t *MCMvdPixelAloneList,		// output

	Short_t *nMvdStripCommon,		// output
	Short_t *MvdStripCommonList,		// output
	Short_t *nMvdStripSpuriinTrack,	// output
	Short_t *MvdStripSpuriList,		// output
	Short_t *nMCMvdStripAlone,		// output
	Short_t *MCMvdStripAloneList		// output
				)
{


//	bool includePixel[MAXTRACKSPEREVENT][nMvdPixelHit],
//	     includeStrip[MAXTRACKSPEREVENT][nMvdStripHit];
	bool includePixel[nSttTrackCand][nMvdPixelHit],
	     includeStrip[nSttTrackCand][nMvdStripHit];

	Short_t i,j;

	int index, index2;

	for(i=0; i<nSttTrackCand;i++){
		if(!keepit[i]) continue;
		nMvdPixelCommon[i]=0;
		nMvdPixelSpuriinTrack[i]=0;
		nMCMvdPixelAlone[i]=0;
		nMvdStripCommon[i]=0;
		nMvdStripSpuriinTrack[i]=0;
		nMCMvdStripAlone[i]=0;
	}

	for(i=0; i<nSttTrackCand;i++){
		if(!keepit[i]) continue;
		for(j=0;j<nMvdPixelHit;j++){
			includePixel[i][j]=true;
		}
		for(j=0;j<nMvdStripHit;j++){
			includeStrip[i][j]=true;
		}

		for(j=0;j<nPixelHitsinTrack[i];j++){
//			index = i*MAXMVDPIXELHITSINTRACK+j;
			index = i*nMvdPixelHit+j;
			includePixel[i][*(ListPixelHitsinTrack+index)]=false;
		  if( daTrackFoundaTrackMC[i]> -1){
			if( daTrackFoundaTrackMC[i] ==
				FromPixeltoMCTrack[*(ListPixelHitsinTrack+index)]
			  ){
				index2 = i*nMvdPixelHit+nMvdPixelCommon[i];
				*(MvdPixelCommonList+index2) =
				     *(ListPixelHitsinTrack+index);
				nMvdPixelCommon[i]++;
			} else {
				index2 = i*nMvdPixelHit+nMvdPixelSpuriinTrack[i];
				*(MvdPixelSpuriList+index2) =
				     *(ListPixelHitsinTrack+index);
				nMvdPixelSpuriinTrack[i]++;
			}
		  }
		}

		for(j=0;j<nStripHitsinTrack[i];j++){
//			index = i*MAXMVDSTRIPHITSINTRACK+j;
			index = i*nMvdStripHit+j;
			includeStrip[i][*(ListStripHitsinTrack+index)]=false;
		  if( daTrackFoundaTrackMC[i]> -1){
			if( daTrackFoundaTrackMC[i] ==
				FromStriptoMCTrack[*(ListStripHitsinTrack+index)]
			  ){
			  	index2 = i*nMvdStripHit+nMvdStripCommon[i];
				*(MvdStripCommonList+index2) =
				     *(ListStripHitsinTrack+index);
				nMvdStripCommon[i]++;
			} else {
				index2 = i*nMvdStripHit+nMvdStripSpuriinTrack[i];
				*(MvdStripSpuriList+index2) =
				     *(ListStripHitsinTrack+index);
				nMvdStripSpuriinTrack[i]++;
			}
		  }
		}
	}	// end of for(i=0; i<nSttTrackCand;i++)





	for(j=0; j<nSttTrackCand;j++){
	    if(!keepit[j]) continue;
	    if( daTrackFoundaTrackMC[j]> -1){
		for(i=0;i<nMvdPixelHit;i++){
			if(!includePixel[j][i]) continue;
			if( daTrackFoundaTrackMC[j]==FromPixeltoMCTrack[i] ){
				index = j*nMvdPixelHit+nMCMvdPixelAlone[j];
				*(MCMvdPixelAloneList+index)=
					i;
				nMCMvdPixelAlone[j]++;
			}
		}
	    }
	}





	for(j=0; j<nSttTrackCand;j++){
	    if(!keepit[j]) continue;
	    if( daTrackFoundaTrackMC[j]> -1){
		for(i=0;i<nMvdStripHit;i++){
			if(!includeStrip[j][i]) continue;
			if( daTrackFoundaTrackMC[j]==FromStriptoMCTrack[i] ){
				index = j*nMvdStripHit+nMCMvdStripAlone[j];
				*(MCMvdStripAloneList+index)=i;
				nMCMvdStripAlone[j]++;
			}
		}
	    }
	}




	return;
}

//------------------ end function  PndTrkComparisonMCtruth::MvdMatchedSpurioustoTrackCand







//----------begin of function PndTrkComparisonMCtruth::MvdMatchtoMC

void PndTrkComparisonMCtruth::MvdMatchtoMC(
	Double_t ERRORSQPIXEL,
	Double_t ERRORSQSTRIP,
	TClonesArray *fMvdMCPointArray,
	Short_t nMvdMCPoint,
	int istampa,
	int IVOLTE,
	Short_t nMvdPixelHit,
	Short_t nMvdStripHit,
	Double_t *refindexMvdPixel,
	Double_t *refindexMvdStrip,
	Double_t *XMvdPixel,
	Double_t *XMvdStrip,
	Double_t *YMvdPixel,
	Double_t *YMvdStrip,
	Double_t *ZMvdPixel,
	Double_t *ZMvdStrip,
	Int_t *FromPixeltoMCTrack,	// output
	Int_t *FromStriptoMCTrack	// output
	)
{

	bool	inclusionMCPoint[nMvdMCPoint];

	Short_t	i,
			j,
			jmcpoint;
	Double_t	dist,
			distance;

	Int_t	MCPointtoMCTrackID;

	Double_t	XMvdMCPoint,
			YMvdMCPoint,
			ZMvdMCPoint;

	PndSdsMCPoint * pMvdMCPoint;


//----  initializations

	for(i=0; i<nMvdMCPoint;i++){
		inclusionMCPoint[i]=true;
	}
	for(i=0; i<nMvdPixelHit;i++){
		FromPixeltoMCTrack[i]=-1;
	}
	for(i=0; i<nMvdStripHit;i++){
		FromStriptoMCTrack[i]=-1;
	}

//----------

	for(i=0; i<nMvdPixelHit;i++){
		if (refindexMvdPixel[i]<0.) continue;
		dist=ERRORSQPIXEL;
		for(j=0;j<nMvdMCPoint;j++){
			// get the MC info.
			pMvdMCPoint = (PndSdsMCPoint*) fMvdMCPointArray->At(j);
			TVector3 position;
			pMvdMCPoint->Position(position);
			XMvdMCPoint=position.X();
			YMvdMCPoint=position.Y();
			ZMvdMCPoint=position.Z();
			MCPointtoMCTrackID= pMvdMCPoint->GetTrackID();

			if( !inclusionMCPoint[j]) continue;
			distance = (XMvdMCPoint-XMvdPixel[i])*(XMvdMCPoint-XMvdPixel[i])+
				(YMvdMCPoint-YMvdPixel[i])*(YMvdMCPoint-YMvdPixel[i])+
				(ZMvdMCPoint-ZMvdPixel[i])*(ZMvdMCPoint-ZMvdPixel[i]);
if(istampa>2)	{
	cout<<"distanza**2 di Pixel hit n. "<<i
   <<" da MC Mvd Point n. "<<j<<" = "<<distance<<endl;
		}
			if( distance<dist){
				FromPixeltoMCTrack[i]=MCPointtoMCTrackID;
				jmcpoint=j;
				dist=distance;
			}
		}	// end of for(j=0;j<nMvdMCPoint;j++)

		if( FromPixeltoMCTrack[i]>= 0){
			inclusionMCPoint[jmcpoint]=false;
		}
if(istampa>=2){
	if( FromPixeltoMCTrack[i]<0 ){
cout<<"Evento n. "<<IVOLTE<<
"; il pixel hit n. "<<i<<" non e' associato ad alcun Mvd Point (FromPixeltoMCTrack=-1).\n";
	} else {
cout<<"Evento n. "<<IVOLTE<<
  "; associato pixel hit n. "<<i<<" a  MC Mvd Point n. "<<jmcpoint
		<<" e di conseguenza alla traccia MC n. "<<FromPixeltoMCTrack[i]<<endl;
	}
}

	}	// end of for(i=0; i<nMvdPixelHit;i++)






	for(i=0; i<nMvdStripHit;i++){
		if (refindexMvdStrip[i]<0.) continue;
		dist=ERRORSQSTRIP;
		for(j=0;j<nMvdMCPoint;j++){
			// get the MC info.
			pMvdMCPoint = (PndSdsMCPoint*) fMvdMCPointArray->At(j);
			TVector3 position;
			pMvdMCPoint->Position(position);
			XMvdMCPoint=position.X();
			YMvdMCPoint=position.Y();
			ZMvdMCPoint=position.Z();
			MCPointtoMCTrackID= pMvdMCPoint->GetTrackID();


			if( !inclusionMCPoint[j]) continue;
			distance = (XMvdMCPoint-XMvdStrip[i])*(XMvdMCPoint-XMvdStrip[i])+
				(YMvdMCPoint-YMvdStrip[i])*(YMvdMCPoint-YMvdStrip[i])+
				(ZMvdMCPoint-ZMvdStrip[i])*(ZMvdMCPoint-ZMvdStrip[i]);
if(istampa>2) cout<<"distanza**2 di Strip hit n. "<<i
   <<" da MC Mvd Point n. "<<j<<" = "<<distance<<endl;
			if( distance<dist){
				FromStriptoMCTrack[i]=MCPointtoMCTrackID;
				jmcpoint=j;
				dist=distance;
			}
		}	// end of for(j=0;j<nMvdMCPoint;j++)
		if( FromStriptoMCTrack[i]>= 0){
			inclusionMCPoint[jmcpoint]=false;
		}
if(istampa>=2){
	if( FromStriptoMCTrack[i]<0 ){
cout<<"Evento n. "<<IVOLTE<<
",  lo strip hit n. "<<i<<" non e' associato ad alcun Mvd Point (FromStriptoMCTrack=-1).\n";
	} else {
cout<<"Evento n. "<<IVOLTE<<
		";  associato strip hit n. "<<i<<" a  MC Mvd Point n. "<<jmcpoint
		<<" e di conseguenza alla traccia MC n. "<<FromStriptoMCTrack[i]<<endl;
	}
}

	}	// end of for(i=0; i<nMvdStripHit;i++)

	return;
}


//----------end of function PndTrkComparisonMCtruth::MvdMatchtoMC



//----------begin of function PndTrkComparisonMCtruth::StampaMCTracks
 void PndTrkComparisonMCtruth::stampaMCTracks(
	Double_t BFIELD,
	Double_t CVEL,
	TClonesArray* fMCTrackArray,
	int nMCTracks
						)
 {
	cout<<"from PndTrkComparisonMCtruth::StampaMCTracks  n. MC Tracks "
		<<nMCTracks<<" e lista solo di quelle che vengono da (0,0,0) :\n";
		for(int ic=0;ic<nMCTracks;ic++){
			PndMCTrack* pMC = (PndMCTrack*) fMCTrackArray->At(ic);
			if ( !( fabs(pMC->GetStartVertex().X())<0.5 &&
			     fabs(pMC->GetStartVertex().Y())<0.5 &&
			     fabs(pMC->GetStartVertex().Z())<0.5 )) continue;
			double carica;
			int icode  = pMC->GetPdgCode() ;    //   PDG code of track
			double Pxx = pMC->GetMomentum().X();
			double Pyy = pMC->GetMomentum().Y();
			double aaa = sqrt( Pxx*Pxx + Pyy*Pyy);
			double Rr =   aaa*1000./(BFIELD*CVEL);// R (cm) of Helix of track projected
						//  in XY plane; B = 2 Tesla
			TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
			TParticlePDG *fParticle= fdbPDG->GetParticle(icode);
			if (icode>1000000000) carica = 1.;
			else  carica = fParticle->Charge()/3. ;    //   charge of track

			cout<<"\tTraccia n. "<<ic<<", Px "<<pMC->GetMomentum().X()
			<<", Py "<<pMC->GetMomentum().Y()
			<<", Pz "<<pMC->GetMomentum().Z()
			<<", carica = "<<carica
			<<"\n\t\tRaggio "<<Rr<<", Xvert "<<pMC->GetStartVertex().X()
			<<", Yvert "<<pMC->GetStartVertex().Y()
			<<", Zvert "<<pMC->GetStartVertex().Z()<<endl;
		}

}

//----------end of function PndTrkComparisonMCtruth::StampaMCTracks

//------------------------- begin of function  PndTrkComparisonMCtruth::SttMatchedSpurious


void PndTrkComparisonMCtruth::SttMatchedSpurious(
	Short_t *daTrackFoundaTrackMC,
	bool *InclusionListStt,
	Double_t info[][7],
	bool *keepit,
	int MAXSTTHITS,
	int MAXSTTHITSINTRACK,
	int MAXTRACKSPEREVENT,
	Short_t *ListHitsinTrack, // from PR
	Short_t *ListSkewHitsinTrack, // from PR
	Short_t *MCParalAloneList,
	Short_t *MCSkewAloneList,
	Short_t *nHitsInMCTrack,
	Short_t  nHitsinTrack[], // n. hits PARALLEL from PR
	Short_t *nMCParalAlone,
	Short_t *nMCSkewAlone,
	Short_t *nParalCommon,
	Short_t *nSkewCommon,
	Short_t *nSkewHitsInMCTrack,
	Short_t *nSkewHitsinTrack, // n. hits skew, from PR
	Short_t *nSpuriParinTrack,
	Short_t *nSpuriSkewinTrack,
	Short_t  ntotalHits,
	Short_t  nTracksFoundSoFar, // those found by PR
	Short_t *ParalCommonList,
	Short_t *ParSpuriList,
	Short_t *SkewCommonList,
	Short_t *SkewSpuriList
	)
{
 bool flaggo;
 Short_t	i, jexp, exphit, iHit,
		enne[MAXTRACKSPEREVENT][MAXSTTHITS];
 Short_t	emme;

 int index1, index2;

  for(jexp=0; jexp<nTracksFoundSoFar;jexp++){
	if(!keepit[jexp]) continue;
	nParalCommon[jexp]=0;
	nSkewCommon[jexp]=0;
	nMCParalAlone[jexp]=0;
	nMCSkewAlone[jexp]=0;
	nSpuriParinTrack[jexp]=0;
	nSpuriSkewinTrack[jexp]=0;

// --- parallel hits
	for(exphit=0; exphit<nHitsinTrack[jexp]; exphit++){
		index1 = jexp*MAXSTTHITSINTRACK + exphit;
		iHit = *(ListHitsinTrack+index1) ;
		enne[jexp][exphit] = (Short_t) ( info[iHit][6] + 0.01);
		if( enne[jexp][exphit] ==   daTrackFoundaTrackMC[jexp] ){
			index2 = jexp*MAXSTTHITSINTRACK +nParalCommon[jexp];
			*(ParalCommonList+index2) = iHit;
			nParalCommon[jexp]++;
		} else {
			index2 = jexp*MAXSTTHITSINTRACK +nSpuriParinTrack[jexp];
			*(ParSpuriList+index2) = iHit;
			nSpuriParinTrack[jexp]++;			
		}

	}
//--- ricerca degli hits non mecciati, della traccia MC associata a questa traccia trovata.
	for(i=0; i<ntotalHits; i++){
		emme = (Short_t) ( info[ i ][6] + 0.01);
		// escludo gli hits non paralleli oppure che non appartengono alla giusta
		// traccia MC
		if( info[i][5] > 2. || (emme != daTrackFoundaTrackMC[jexp]) ) continue;
		if( !InclusionListStt[i]) continue; // escludo gli hits con multiple hits
			flaggo=true;
			for(exphit=0; exphit<nHitsinTrack[jexp]; exphit++){
				index1 = jexp*MAXSTTHITSINTRACK + exphit;
				if(*(ListHitsinTrack+index1) == i){
					flaggo=false;
					break;
				}
			}
			if(flaggo){
				index2 = jexp*MAXSTTHITSINTRACK+nMCParalAlone[jexp];
				*(MCParalAloneList+index2) = i;
				nMCParalAlone[jexp]++;
			} // end of  if(flaggo)
	}  //  end of  for(i=0; i<ntotalHits; i++)

	nHitsInMCTrack[jexp] = nMCParalAlone[jexp]+nParalCommon[jexp];
// --- skew hits

	for(exphit=0; exphit<nSkewHitsinTrack[jexp]; exphit++){
		index1 = jexp*MAXSTTHITSINTRACK + exphit;
		iHit =  *(ListSkewHitsinTrack+index1);
		enne[jexp][exphit] = (Short_t) ( info[iHit][6] + 0.01);
		if( enne[jexp][exphit] ==   daTrackFoundaTrackMC[jexp] ){
			index2 = jexp*MAXSTTHITSINTRACK+nSkewCommon[jexp];
			*(SkewCommonList+index2) = iHit;
			nSkewCommon[jexp]++;
		} else {
			index2 = jexp*MAXSTTHITSINTRACK+nSpuriSkewinTrack[jexp];
			*(SkewSpuriList+index2) = iHit;
			nSpuriSkewinTrack[jexp]++;			
		}

	}
//--- ricerca degli hits non mecciati, della traccia MC associata a questa traccia trovata.
	for(i=0; i<ntotalHits; i++){
		emme = (Short_t) ( info[ i ][6] + 0.01);

		// considero solo le skew ( info[i][5]=99.) ed escludo quelle che
		//  non appartengono alla giusta traccia MC
		if( info[i][5] < 98. || (emme != daTrackFoundaTrackMC[jexp]) ) continue;
		if( !InclusionListStt[i]) continue; // escludo gli hits con multiple hits
			flaggo=true;
			for(exphit=0; exphit<nSkewHitsinTrack[jexp]; exphit++){
				index1 = jexp*MAXSTTHITSINTRACK + exphit;
				if(i == *(ListSkewHitsinTrack+index1) ){
					flaggo=false;
					break;
				}
			}
			if(flaggo){
				index2 = jexp*MAXSTTHITSINTRACK +nMCSkewAlone[jexp] ;
				*(MCSkewAloneList+index2) = i;
				nMCSkewAlone[jexp]++;
			}
	}

	nSkewHitsInMCTrack[jexp] = nMCSkewAlone[jexp]+nSkewCommon[jexp];


   }   //   end of  for(jexp=0; jexp<nTracksFoundSoFar;jexp++)



    return;
}


//------------------------- end of function  PndTrkComparisonMCtruth::SttMatchedSpurious




ClassImp(PndTrkComparisonMCtruth);
