// -------------------------------------------------------------------------
// -----                PndSttTrackFinderReal source file             -----
// -----                  Created 28/03/06  by V. Friese               -----
// -------------------------------------------------------------------------
#include "glpk.h"

// Pnd includes
#include "PndSttTrackFinderReal.h"

#include "PndSttHit.h"
#include "PndSttPoint.h"
#include  <cmath>
#include "FairMCPoint.h"
#include "FairRootManager.h"
#include "GFKalman.h"

// ROOT includes
#include "TClonesArray.h"
#include "TDatabasePDG.h"
#include "TRandom.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

// C++ includes
#include <iostream>
#include <map>

using std::cout;
using std::cin;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------
PndSttTrackFinderReal::PndSttTrackFinderReal()
{ 
  fVerbose      = 1;
  MINIMUMOUTERHITSPERTRACK=5;
                 Fimin=0.;     Fimax=2.*PI;
                 FI0min = 0.; FI0max = 2.*PI;
               stepD=(Dmax-Dmin)/nbinD;
               stepFi=(Fimax-Fimin)/nbinFi;
               stepR=(Rmax-Rmin)/nbinR;
               stepKAPPA=(KAPPAmax-KAPPAmin)/nbinKAPPA;
               stepFI0=(FI0max-FI0min)/nbinFI0;
               stepfineKAPPA=2.*DELTA_KAPPA/nbinKAPPA;
               stepfineFI0=2.*DELTA_FI0/nbinFI0;
               RminStrawSkewArea = RStrawDetectorMin*2./1.732051 + 18.*StrawRadius ; // delimitation of the skew area
               RmaxStrawSkewArea = RminStrawSkewArea + 8.*1.732051 *StrawRadius ;

}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndSttTrackFinderReal::PndSttTrackFinderReal(Int_t verbose) 
{ 
  fVerbose      = verbose;
  MINIMUMOUTERHITSPERTRACK=5;
                 Fimin=0.;     Fimax=2.*PI;
                 FI0min = 0.; FI0max = 2.*PI;
               stepD=(Dmax-Dmin)/nbinD;
               stepFi=(Fimax-Fimin)/nbinFi;
               stepR=(Rmax-Rmin)/nbinR;
               stepKAPPA=(KAPPAmax-KAPPAmin)/nbinKAPPA;
               stepFI0=(FI0max-FI0min)/nbinFI0;
               stepfineKAPPA=2.*DELTA_KAPPA/nbinKAPPA;
               stepfineFI0=2.*DELTA_FI0/nbinFI0;
               RminStrawSkewArea = RStrawDetectorMin*2./1.732051 + 18.*StrawRadius ; // delimitation of the skew area
               RmaxStrawSkewArea = RminStrawSkewArea + 8.*1.732051 *StrawRadius ;

}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndSttTrackFinderReal::~PndSttTrackFinderReal() 
{ 
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
void PndSttTrackFinderReal::Init() 
{

   UShort_t i, j, k;
   Double_t    r1, r2, A , rConfMin, rConfMax,
              tempRadiaConf[nRdivConformal];

   IVOLTE=0; ntimes = 0;


  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();

//    get   the MCTrack  array

  fMCTrackArray = (TClonesArray*) ioman->ActivateBranch("MCTrack");

//   -------------------------------------------------------------------


//   --------------------   initializations for the Kalman with Genfit later

/*
   fSttHitArray=(TClonesArray*) ioman->GetObject("STTHit");
   if(fSttHitArray==0){
     Error("PndSttKalmanTask2::Init","stt pattern recognition; hit-array not found!");
     return ;
   }

  // Build hit factory -----------------------------

  _theRecoHitFactory = new GFRecoHitFactory();
  _theRecoHitFactory->addProducer(3, new GFRecoHitProducer<PndSttHit,PndSttRecoHit>(fSttHitArray));   //
*/
// -------------------- end initializations for the Kalman with Genfit later










  if (!ioman) 
    {
      cout << "-E- PndSttTrackFinderReal::Init: "
	   << "RootManager not instantised!" << endl;
      return;
    }
 
//   calculate the boundaries of the Box in Conformal Space, see Gianluigi logbook on pag. 210-211

    radiaConf[0] = 1./RStrawDetectorMax; 
    r1 = RStrawDetectorMin;
//    A = (RStrawDetectorMax*RStrawDetectorMax - r1)/nRdivConformal;
    A = (RStrawDetectorMax - r1)/nRdivConformal;
    if ( nRdivConformal > 1 ) {
      for(i = 1; i< nRdivConformal ; i++){
        r2 = r1 + A;
//        tempRadiaConf[nRdivConformal-i] = 1./sqrt(r2);
        tempRadiaConf[nRdivConformal-i] = 1./r2;
        r1=r2;

      }
    }





      rConfMin = 1./RmaxStrawSkewArea;
      rConfMax = 1./RminStrawSkewArea;

//  now take into account the zone of the skew straws, which is 'empty' as far as the parallel straws is concerned

      for(i = 1; i< nRdivConformal  ; i++){

        radiaConf[i] = tempRadiaConf[i];
      }
      nRdivConformalEffective = nRdivConformal;




}
// -------------------------------------------------------------------------


// -----   Public method DoFind   ------------------------------------------
Int_t PndSttTrackFinderReal::DoFind(TClonesArray* trackArray) 
{
    UShort_t auxIndex[nmaxHits],
             OLDinfoparal[nmaxHits];
    UShort_t istep,inclination_type;

    Double_t aaa, ddd, delta, deltabis, deltaZ, mindis, distanza, X, Y, Z, ap1, ap2, ap3, cross1, cross2, cross3,
             info[nmaxHits][6],
             WDX, WDY, WDZ,
             auxRvalues[nmaxHits],
             inclinationversors[nmaxinclinationversors][3];

    inclinationversors[0][0]=inclinationversors[0][1]=0.,    inclinationversors[0][2]=1.;
    Int_t Ninclinations = 1, Ninclinate;
    Int_t Minclinations[nmaxinclinationversors];



  // Check pointers
  if (fHitCollectionList.GetEntries() == 0)
  {
      cout << "-E- PndSttTrackFinderReal::DoFind: "
	   << "No hit arrays present, call AddHitCollection() first (at least once)! " << endl;
      return -1;
  }

  if (fPointCollectionList.GetEntries() == 0)
  {
      cout << "-E- PndSttTrackFinderReal::DoFind: "
	   << "No point arrays present, call AddHitCollection() first (at least once)! " << endl;
      return -1;
  }

  if ( !trackArray ) 
    {
      cout << "-E- PndSttTrackFinderReal::DoFind: "
	   << "Track array missing! " << endl;
      return -1;
    }


   PndMCTrack*      pMCtr = NULL;
   nMCTracks = fMCTrackArray->GetEntriesFast(); // num. tracce/evento

   if(nMCTracks>MAXMCTRACKS){
    cout<<"from DoFind  : nMCTracks = "<<nMCTracks<<"  larger that MAXMCTRACKS = "
       <<MAXMCTRACKS
        <<",  skipping this event\n";
    return  -10;
  }

   for (Int_t iMCTrack = 0; iMCTrack < nMCTracks; iMCTrack++) 
   { 
       pMCtr = (PndMCTrack*) fMCTrackArray->At(iMCTrack);
       if ( ! pMCtr ) continue;



         Double_t R, D, Fi, Ox, Oy, Cx, Cy, Px, Py  ;
         Int_t icode;
         icode  = pMCtr->GetPdgCode() ;    //   PDG code of track
         Ox = pMCtr->GetStartVertex().X();    //   X of starting point track
         Oy = pMCtr->GetStartVertex().Y();    //   Y of starting point track
         Px = pMCtr->GetMomentum().X();
         Py = pMCtr->GetMomentum().Y();
         aaa = sqrt( Px*Px + Py*Py);
         R =   aaa*1000./(BFIELD*CVEL);    //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla

         if( icode > 0 ) {
           Cx = Ox  -Py*1000./(BFIELD*CVEL);    // MC truth X of center of circle of Helix trajectory
           Cy = Oy + Px*1000./(BFIELD*CVEL);    // MC truth Y of center of circle of Helix trajectory
         }  else {
           Cx = Ox + Py*1000./(BFIELD*CVEL);    // MC truth X of center of circle of Helix trajectory
           Cy = Oy - Px*1000./(BFIELD*CVEL);    // MC truth Y of center of circle of Helix trajectory

         }




         Fi = atan2(Cy, Cx);       // MC truth Fi angle of circle of Helix trajectory
         if(Fi<0.)  Fi += 2.*PI;
         D = sqrt( Cx*Cx+Cy*Cy) - R;



         CxMC[iMCTrack]= Cx;
         CyMC[iMCTrack]= Cy;
         R_MC[iMCTrack]= R;



         MCtruthTrkInfo[0][iMCTrack] = Ox;    //   X of starting point track
         MCtruthTrkInfo[1][iMCTrack] = Oy;    //   Y of starting point track
         MCtruthTrkInfo[2][iMCTrack] = pMCtr->GetStartVertex().Z();    //   Z of starting point track

         MCtruthTrkInfo[3][iMCTrack] = Px;    //   Px at starting point of track
         MCtruthTrkInfo[4][iMCTrack] = Py;    //   Py at starting point of track
         MCtruthTrkInfo[5][iMCTrack] = pMCtr->GetMomentum().Z();    //   Pz at starting point of track

         MCtruthTrkInfo[6][iMCTrack] = D;     //   D of Helix of track projected in XY plane
         MCtruthTrkInfo[7][iMCTrack] = Fi;    //   Fi of Helix of track projected in XY plane
         MCtruthTrkInfo[8][iMCTrack] = R;    //   R  of Helix of track projected in XY plane

         MCtruthTrkInfo[9][iMCTrack] = Cx;     //   Cx of Helix of track projected in XY plane
         MCtruthTrkInfo[10][iMCTrack] = Cy;    //   Cy of Helix of track projected in XY plane
         MCtruthTrkInfo[11][iMCTrack] = icode ;    //   PDG code of track
         MCtruthTrkInfo[12][iMCTrack] = 0.001*BFIELD*CVEL/MCtruthTrkInfo[5][iMCTrack] ;    //   KAPPA of Helix of track in cm*-1
        double tempoang = atan2(-MCtruthTrkInfo[10][iMCTrack]+MCtruthTrkInfo[1][iMCTrack],
                                 -MCtruthTrkInfo[9][iMCTrack]+MCtruthTrkInfo[0][iMCTrack]);
        if(tempoang<0.) tempoang+= 2.*PI;
         MCtruthTrkInfo[13][iMCTrack] = tempoang ;    //   FI0 of Helix of track
         nHitsInMCTrack[iMCTrack]  = 0;
         nSkewHitsInMCTrack[iMCTrack]  = 0;

   }




  IVOLTE++;

    
  // Initialise control counters
  Int_t nNoTrack     = 0;
  Int_t nNoSttPoint  = 0;
  Int_t nNoSttHit    = 0;

  // Create pointers to hit and SttPoint
  PndSttHit*       pMhit = NULL;
  FairMCPoint*      pMCpt = NULL;

  // Number of STT hits
  Int_t nHits = 0;
  
  for (Int_t hitListCounter = 0; hitListCounter < fHitCollectionList.GetEntries(); hitListCounter++)
  {
      nHits += ((TClonesArray *)fHitCollectionList.At(hitListCounter))->GetEntriesFast();
  }

  if(nHits > nmaxHits ) {
    cout<<"From DoFind  : nHits = "<<nHits<<"  larger that nmaxHits = "<<nmaxHits
        <<",  skipping this event\n";
    return  -10;
  }


  // Declare some variables outside the loops
  Int_t trackIndex   = 0;     // STTTrack index

  // Create STL map from MCtrack index to number of valid SttHits
  map<Int_t, map<Double_t, Int_t> >
    hitMap;



    for(Int_t j=0; j<nmaxinclinationversors;j++){
     Minclinations[j]=0;
    }

    Ninclinate=0;



  //   generated momenta and starting position of each track




  // Loop over hits
  for (Int_t iHit = 0; iHit < nHits; iHit++) 
    {
      // hit point
      pMhit = GetHitFromCollections(iHit);   // <== PndSttHit
      if (!pMhit) continue;
      
      // MC point
      Int_t ptIndex = pMhit->GetRefIndex();
      if (ptIndex < 0) continue;           // fake or background hit
      pMCpt = GetPointFromCollections(iHit); // <== FairMCPoint
      if (!pMCpt) continue;
      
      // real hit center of tube coordinates
      TVector3 center(pMhit->GetX(), pMhit->GetY(), pMhit->GetZ());

      // drift radius
      Double_t dradius = pMhit->GetIsochrone();

      // wire direction
      TVector3 wiredirection = pMhit->GetWireDirection();

      // "real" MC coordinates (in + out)/2.
      TVector3 mcpoint( ((PndSttPoint*)pMCpt)->GetXtot(), ((PndSttPoint*)pMCpt)->GetYtot(), ((PndSttPoint*)pMCpt)->GetZtot());

      if(wiredirection.Z() >=0.) {
       WDX = wiredirection.X();     WDY = wiredirection.Y(); WDZ = wiredirection.Z();
      }  else {
       WDX = -wiredirection.X();     WDY = -wiredirection.Y(); WDZ = -wiredirection.Z();
      }






      // stampe di controllo
      info[iHit][0]= pMhit->GetX();
      info[iHit][1]= pMhit->GetY();
      info[iHit][2]= pMhit->GetZ();
      info[iHit][3]= dradius;
      info[iHit][4]=pMhit->GetTubeHalfLength();

      if( fabs( WDX )< 0.00001 && fabs( WDY )< 0.00001 ){
          info[iHit][5]= 1.;
          infoparal[Minclinations[0]]= iHit ;
          Minclinations[0]++;
          ZCENTER_STRAIGHT = info[iHit][2];      //    this works because just few lines below there is the
          SEMILENGTH_STRAIGHT = info[iHit][4];   //    requirement that Minclinations[0] > 2 (= at least 3 parallel straws)
      } else {
       for (Int_t i=2; i<=Ninclinations;i++) {
        if (fabs( WDX-inclinationversors[i-1][0] )< 0.00001
                                &&
            fabs( WDY -inclinationversors[i-1][1])< 0.00001
                                &&
            fabs( WDZ -inclinationversors[i-1][2])< 0.00001
                                           ){
          info[iHit][5]= i;
          infoskew[Ninclinate]= iHit;
          Ninclinate++;
          Minclinations[i-1]++;
          goto jumpout;
        }
       }
       Ninclinations++;
       inclinationversors[Ninclinations-1][0]=(Double_t) WDX;
       inclinationversors[Ninclinations-1][1]=(Double_t) WDY;
       inclinationversors[Ninclinations-1][2]=(Double_t) WDZ;
       info[iHit][5]= Ninclinations;
          infoskew[Ninclinate]= iHit;
          Ninclinate++;
       Minclinations[Ninclinations-1]++;
jumpout: ;
      }        

      NSkewhits = Ninclinate;


//   calcoli validi solo per il MC   -----------------------------

      veritaMC[iHit][0]= ((PndSttPoint*)pMCpt)->GetXtot();
      veritaMC[iHit][1]= ((PndSttPoint*)pMCpt)->GetYtot();
      veritaMC[iHit][2]= ((PndSttPoint*)pMCpt)->GetZtot();



// association of this hit (in this part only if it is a 'parallel hit')to the MC track

    if(info[iHit][5]==1. ){
       deltabis = 9999999999.;
       FromHitToMCTrack[iHit] = 888888888;   //  just some large unrealistic number
       for( Int_t iMCTrack = 0; iMCTrack < nMCTracks; iMCTrack++){

         delta = fabs( sqrt(R_MC[iMCTrack]*R_MC[iMCTrack]+info[iHit][3]*info[iHit][3]-StrawRadius*StrawRadius )+
                info[iHit][3] - sqrt(  (info[iHit][0]-CxMC[iMCTrack])*(info[iHit][0]-CxMC[iMCTrack]) +
                     (info[iHit][1]-CyMC[iMCTrack])*(info[iHit][1]-CyMC[iMCTrack])   )     );

         if( delta < deltabis &&  delta < 4.*StrawRadius) {
            FromHitToMCTrack[iHit] =  iMCTrack;
            deltabis = delta;
         }
       }


     if(FromHitToMCTrack[iHit]<  MAXMCTRACKS){
      FromMCTrackToHit[ FromHitToMCTrack[iHit] ][ nHitsInMCTrack[ FromHitToMCTrack[iHit] ]] = iHit;
      nHitsInMCTrack[  FromHitToMCTrack[iHit]  ]++ ;
     }

//----------------------------------------------------------------

   }    // end of if(info[iHit][5]==1. )

    }   //   end  of  for (Int_t iHit = 0;


//------------------------------------------------------------------------------------------
//   associazione degli hits non paralleli
  // Loop over hits
  for (Int_t iHit = 0; iHit < nHits; iHit++) 
    {
       if(info[iHit][5]==1. )  continue;
       FromSkewHitToMCTrack[iHit] = 888888888;   //  just some large unrealistic number
       inclination_type = (UShort_t) info[iHit][5] -1;
       deltaZ = 2.*info[iHit][4]*inclinationversors[inclination_type][2]/2000.;
       for (Int_t iMCTrack = 0; iMCTrack < nMCTracks; iMCTrack++) 
       {
          // now the stepping
          for(istep=0, mindis=99999999999.; istep<2000; istep++){
            Z = istep*deltaZ + info[iHit][2] - info[iHit][4]*inclinationversors[inclination_type][2];
            X = MCtruthTrkInfo[9][iMCTrack] + MCtruthTrkInfo[8][iMCTrack]*cos( MCtruthTrkInfo[13][iMCTrack]+ MCtruthTrkInfo[12][iMCTrack]*Z);
            Y = MCtruthTrkInfo[10][iMCTrack] + MCtruthTrkInfo[8][iMCTrack]*sin( MCtruthTrkInfo[13][iMCTrack]+ MCtruthTrkInfo[12][iMCTrack]*Z);

            ap1 = info[iHit][0]-X;
            ap2 = info[iHit][1]-Y;
            ap3 = info[iHit][2]-Z;

            cross1 = ap2*inclinationversors[inclination_type][2]-ap3*inclinationversors[inclination_type][1];
            cross2 = ap3*inclinationversors[inclination_type][0]-ap1*inclinationversors[inclination_type][2];
            cross3 = ap1*inclinationversors[inclination_type][1]-ap2*inclinationversors[inclination_type][0];


            distanza = sqrt( cross1*cross1+cross2*cross2+cross3*cross3) ;
            if(distanza<mindis)  mindis = distanza;
          }   //  end of for(istep=0; istep<100; istep++)
          if(mindis<3.*StrawRadius) {
            FromSkewHitToMCTrack[iHit] =  iMCTrack;
            FromMCTrackToSkewHit[ iMCTrack ][ nSkewHitsInMCTrack[ iMCTrack ] ] = iHit;
            nSkewHitsInMCTrack[  iMCTrack  ]++ ;
          }
       }   //  end of for (Int_t iMCTrack = 0; iMCTrack < nMCTracks; iMCTrack++)

    }   //   end  of  for (Int_t iHit = 0;

//-------------------   fine associazione degli hits non paralleli











//   ordering the hits by INCREASING CONFORMAL RADIUS or equivalently, decreasing spatial radius.

    for (int j = 0; j< Minclinations[0]; j++){
      auxIndex[j]=j;
      auxRvalues[j]=
                    info[ infoparal[ j ]  ][0]*
                    info[ infoparal[ j ]  ][0]+
                    info[ infoparal[ j ]  ][1]*
                    info[ infoparal[ j ]  ][1];
      OLDinfoparal[j]=infoparal[j];
    }


    PndStt_Merge_Sort( Minclinations[0], auxRvalues, auxIndex);
    for (int j = 0; j< Minclinations[0]; j++){
      infoparal[ Minclinations[0]-1-j]  =  OLDinfoparal[ auxIndex[ j ]   ];
    }

//----------------------------









   if( nHits >0 &&  Minclinations[0] > 2 ) {
         PndSttTrkFinderPartial(nHits,info,Ninclinations,Minclinations,inclinationversors,
                                             Ninclinate,
              trackArray   //   this is the output, ie the TClonesArray *   od  PndSttTrack
                          //    classes containing the info for a found track, one class per each track
                                             );

   };


  Int_t nTracks = 1;
  return nTracks;
}

//-------------------------------------------------------------------------------------------------------------------

PndSttHit* PndSttTrackFinderReal::GetHitFromCollections(Int_t hitCounter)
{
    PndSttHit
	*retval = NULL;
 
    Int_t
	relativeCounter = hitCounter;

    for (Int_t collectionCounter = 0; collectionCounter < fHitCollectionList.GetEntries(); collectionCounter++)
    {
	Int_t
	    size = ((TClonesArray *)fHitCollectionList.At(collectionCounter))->GetEntriesFast();

	if (relativeCounter < size)
	{
	    retval = (PndSttHit*) ((TClonesArray *)fHitCollectionList.At(collectionCounter))->At(relativeCounter);
	    break;
	}
	else
	{
	    relativeCounter -= size;
	}
    }
    return retval;
}

//-------------------------------------------------------------------------------------------------------------------

FairMCPoint* PndSttTrackFinderReal::GetPointFromCollections(Int_t hitCounter)
{
    FairMCPoint
	*retval = NULL;
 
    Int_t
	relativeCounter = hitCounter;

    for (Int_t collectionCounter = 0; collectionCounter < fHitCollectionList.GetEntries(); collectionCounter++)
    {
	Int_t
	    size = ((TClonesArray *)fHitCollectionList.At(collectionCounter))->GetEntriesFast();

	if (relativeCounter < size)
	{
	    Int_t
		tmpHit = ((PndSttHit*) ((TClonesArray *)fHitCollectionList.At(collectionCounter))->At(relativeCounter))->GetRefIndex();
	    
	    retval = (FairMCPoint*) ((TClonesArray *)fPointCollectionList.At(collectionCounter))->At(tmpHit);
	    
	    break;
	}
	else
	{
	    relativeCounter -= size;
	}
    }
    return retval;
}


//-------------------------------------------------------------------------------------------------------------------

void PndSttTrackFinderReal::WriteHistograms(){

};






//-------------------------------------------------------------------------------------------------------------------

void PndSttTrackFinderReal::PndSttTrkFinderPartial(
                                 Int_t Nhits, Double_t info[][6],
                                 Int_t Nincl,Int_t Minclinations[],Double_t inclination[][3],
                                 Int_t Ninclinate,
                                 TClonesArray * trackArray   //  this is the TClonesArray *   of PndSttTrack classes in output
                                                    )
{

  bool    ExclusionList[nmaxHits],
          TypeConf[MAXTRACKSPEREVENT],   //  if TypeConf[]=false --> the track is a line in the Conformal space,
                                         //   if TypeConf[]=true it is a crf;
          TypeConfSkew[MAXTRACKSPEREVENT];


  UShort_t iExclude,
           imc,jexp,mchit,exphit,
           nTracksFoundSoFar,
           NN,
           Nouter,
           Naux,
           Nbaux,
           nTotalHits,
           iParHit,
           SeedParallelNumber,
           OLDnHitsinTrack,
           TemporarynSkewHitsinTrack,
           nParspuri,
           nSkewspuri,
           BigList[nmaxHits],
           TemporarySkewList[nmaxHits][2],
           nHitsinTrack[MAXTRACKSPEREVENT],
           nSkewHitsinTrack[MAXTRACKSPEREVENT],
           tempore[nmaxHits],
           auxListHitsinTrack[nmaxHits],
           ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHits],
           ListHitsinTrackinWhichToSearch[nmaxHits],
           OLDListHitsinTrack[nmaxHits],
           OutputListHitsinTrack[nmaxHits],
           OutputList2HitsinTrack[nmaxHits],
           ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxHits],
           nParalMax[MAXMCTRACKS],
           nParalCommon[MAXTRACKSPEREVENT],
           TempParalCommonList[MAXMCTRACKS][nmaxHits],
           ParalCommonList[MAXMCTRACKS][nmaxHits],
           nSkewMax[MAXMCTRACKS],
           nSkewCommon[MAXTRACKSPEREVENT],
           TempSkewCommonList[MAXMCTRACKS][nmaxHits],
           SkewCommonList[MAXMCTRACKS][nmaxHits],
           daMCTrackaParTrackFound[MAXMCTRACKS],
           daMCTrackaSkewTrackFound[MAXMCTRACKS],
           RConformalIndex[nmaxHits],  //  given a Hit number it gives its radial box number
           FiConformalIndex[nmaxHits],  //  given a Hit number it gives its azimuthal box number
           nBoxConformal[nRdivConformal][nFidivConformal],  //  first index -> radial divisions, 2nd index -> azimuthal divisions; n. of
                                                            //  hits falling in this cell
           HitsinBoxConformal[nRdivConformal][nFidivConformal][nmaxHits];  //  first index -> radial divisions, 2nd index -> azimuthal divisions;
                                                //  list of hit numbers of those falling in this cell
   Short_t Charge,
           Status[MAXTRACKSPEREVENT],
           daParTrackFoundaTrackMC[MAXTRACKSPEREVENT],
           daSkewTrackFoundaTrackMC[MAXTRACKSPEREVENT];

   Int_t status;



   Int_t  i, j,ii,jj,k,kk,n1,n2,n3,
          i1,j1,k1,imaxima, jmaxima,
          iofmax, jofmax, kofmax,
          index[nmaxHits], integer,
          Ncirc, nSkew1, nSkew2,
          n_K, n_FI0, n_R, n_D, n_Fi,STATUS,
          Nremaining, Nremaining2,
          NumberofMaximaDFiR,
          NumberofMaximaKFI0,
//          MaximaIndexesDFiR[MAXElementsOverThresholdinHough][3],
//          MaximaIndexesKFI0[MAXElementsOverThresholdinHough][2],
          nAssociatedParallelHits ;

   int   iimax,jjmax,  ncount;


   Double_t angle, max1, HoughR, HoughD, HoughFi, HoughKAPPA, HoughFI0,
              tempR, tempD,tempFi,tempKAPPA,tempFI0,
              Rlow, Rup, Dlow, Dup,Filow,Fiup, KAPPAlow, KAPPAup, FI0low, FI0up,
              gamma;

   Double_t    D,Fi, rotationangle, rotationsin, rotationcos,ptotal,
               m[MAXTRACKSPEREVENT],
               q[MAXTRACKSPEREVENT],
               ALFA[MAXTRACKSPEREVENT],
               BETA[MAXTRACKSPEREVENT],
               GAMMA[MAXTRACKSPEREVENT],
               R[MAXTRACKSPEREVENT],
               Ox[MAXTRACKSPEREVENT],
               Oy[MAXTRACKSPEREVENT],
               KAPPA[MAXTRACKSPEREVENT],
               FI0[MAXTRACKSPEREVENT],
               trajectory_vertex[3],
               infoparalConformal[nmaxHits][5],
               auxinfoparalConformal[nmaxHits][5],
               S[nmaxHits],
               Z[nmaxHits],
               ZDrift[nmaxHits],
               ZErrorafterTilt[nmaxHits];


    Float_t RemainingR[MAXSTTINFO],
            RemainingFi[MAXSTTINFO],
            RemainingD[MAXSTTINFO],
            RemainingCX[MAXSTTINFO],
            RemainingCY[MAXSTTINFO],
            RemainingKAPPA[MAXSTTINFO],
            RemainingFI0[MAXSTTINFO];
    bool    Goodflag[MAXSTTINFO],
            temporflag[8],
            IFLAG;


   CalculatedCircles Result;
   CalculatedHelix   HelixResult;
   AssociatedHitsToHelix ResultAssociatedHits ;

   char nomef[100], nome[30],titolo[100];






      for(i=0; i< Minclinations[0]; i++){
         ExclusionList[   infoparal[i]   ]= true ;


      }

      trajectory_vertex[0]=trajectory_vertex[1]=trajectory_vertex[2]=0.;

      PndSttFromXYtoConformal(trajectory_vertex,info, Minclinations[0],infoparalConformal, &status);



      PndSttBoxConformalFilling(infoparalConformal, Minclinations[0], nBoxConformal, HitsinBoxConformal,
                                RConformalIndex, FiConformalIndex);



//     start the track finding procedure


//----- loop over the parallel hits

  nTracksFoundSoFar=0;    // # tracks found

//   begins the first iteration with more severe cuts on the # hits in track candidate

  MINIMUMHITSPERTRACK=4;
  for(iParHit=0; iParHit<Minclinations[0] + 1 -  MINIMUMHITSPERTRACK ; iParHit++) {

      if( ! ExclusionList[    infoparal[iParHit]  ] )  continue;
      nHitsinTrack[nTracksFoundSoFar] = PndSttFindTrackPatterninBoxConformal(
                           1,   //  distance in R cells allowed
                           2,   //  distance in Fi cells allowed
                           Minclinations[0],
                           iParHit,
                           info,
                           ExclusionList,
                           RConformalIndex,
                           FiConformalIndex,
                           nBoxConformal,
                           HitsinBoxConformal,
                           &ListHitsinTrack[nTracksFoundSoFar][0]
                                                    );

      if( nHitsinTrack[nTracksFoundSoFar] < MINIMUMHITSPERTRACK) {
        cout<<"      nHitsinTrack = "<<nHitsinTrack[nTracksFoundSoFar]<<
  "  is < the MINIMUMHIITSPERTRACK ("<< MINIMUMHITSPERTRACK<<
  ");  skip this candidate track\n";
        continue;
      }


//-----------------------

//   find among the ListHitsinTrack  if there are at least a minimum # of hits belonging to the outer part
//   of the STT  system

      for(j=0, Nouter =0; j<nHitsinTrack[nTracksFoundSoFar]; j++){
         if(info[infoparal[ ListHitsinTrack[nTracksFoundSoFar][j] ]][0]* info[infoparal[ ListHitsinTrack[nTracksFoundSoFar][j] ]][0]+
            info[infoparal[ ListHitsinTrack[nTracksFoundSoFar][j] ]][1]* info[infoparal[ ListHitsinTrack[nTracksFoundSoFar][j] ]][1]
            < RmaxStrawSkewArea*RmaxStrawSkewArea  )   break;
         Nouter++;
      }

     if( Nouter >= MINIMUMOUTERHITSPERTRACK) {

      for(i=0; i< Nouter;i++){
          ListHitsinTrackinWhichToSearch[i]=ListHitsinTrack[nTracksFoundSoFar][i];
      }







      for(i=0; i< Nouter;i++){
        SeedParallelNumber = ListHitsinTrackinWhichToSearch[i];
        Naux =  PndSttFindTrackPatterninBoxConformalSpecial(
                           3,    // NRCELLDISTANCE
                           1,    // NFiCELLDISTANCE
                           Minclinations[0],
                           Nouter,
                           SeedParallelNumber,
                           ListHitsinTrackinWhichToSearch,
                           info,
                           ExclusionList,
                           RConformalIndex,
                           FiConformalIndex,
                           nBoxConformal,
                           HitsinBoxConformal,
                           OutputListHitsinTrack);

      if( Naux >= MINIMUMOUTERHITSPERTRACK && Naux > 0.7 * Nouter )  break;

      if( Naux >= MINIMUMOUTERHITSPERTRACK) {
//   further collection of hits in the inner region but this time strictly connected to the outer ones

//  first the list of non outer hits
         for(j=Nouter;j<nHitsinTrack[nTracksFoundSoFar]; j++){
            ListHitsinTrackinWhichToSearch[j-Nouter] = ListHitsinTrack[nTracksFoundSoFar][j];
         }


         Nbaux =  PndSttFindTrackStrictCollection(
                           1,    // NFiCELLDISTANCE
                           SeedParallelNumber,   //  seed hit
                           nHitsinTrack[nTracksFoundSoFar]-Nouter,
                           ListHitsinTrackinWhichToSearch,
                           ExclusionList,
                           FiConformalIndex,
                           OutputList2HitsinTrack
                                                );
//   add the new hits found to the list
         nHitsinTrack[nTracksFoundSoFar]=Naux+Nbaux;
         for(j=0;j<Naux;j++){
            ListHitsinTrack[nTracksFoundSoFar][j] = OutputListHitsinTrack[j];
         }
         for(j=0;j<Nbaux;j++){
            ListHitsinTrack[nTracksFoundSoFar][Naux+j] = OutputList2HitsinTrack[j];
         }
         break;
      }

      }   // end of for(i=0; i< Nouter;i++)
     }    // end of if( Nouter >= MINIMUMOUTERHITSPERTRACK)








//  finding the rotation angle for best utilization of the MILP procedure

      for(j=0, rotationcos=0., rotationsin=0.; j<nHitsinTrack[nTracksFoundSoFar]; j++){
         rotationcos += cos( (0.5+ FiConformalIndex[ infoparal[ ListHitsinTrack[nTracksFoundSoFar][j] ] ])
                                      *2.*PI/nFidivConformal) ;
         rotationsin += sin( (0.5+ FiConformalIndex[ infoparal[ ListHitsinTrack[nTracksFoundSoFar][j] ] ])
                                      *2.*PI/nFidivConformal) ;
      }
      rotationcos /=nHitsinTrack[nTracksFoundSoFar];
      rotationsin /=nHitsinTrack[nTracksFoundSoFar];
      rotationangle = atan2(rotationsin, rotationcos);



//  fitting with superfast MILP code

//  first redefine the center of the conformal plane; it will be centered in the center of the  hit in the
//  hit list with the LEAST drift radius; this hit has to be excluded from the fitting


      trajectory_vertex[0]=  0.;
      trajectory_vertex[1]=  0.;




  for(j=0; j<nHitsinTrack[nTracksFoundSoFar]; j++){
    for(i=0; i<5;i++){
      auxinfoparalConformal[j][i] = infoparalConformal[ ListHitsinTrack[nTracksFoundSoFar][j]  ][i];
    }
  }



      INTERO=1;

      Status[nTracksFoundSoFar] = PndSttFitHelixCylinder( 
                nHitsinTrack[nTracksFoundSoFar],
                auxinfoparalConformal,
                nTracksFoundSoFar,
                rotationangle,
                trajectory_vertex,
                NHITSINFIT,
                &m[nTracksFoundSoFar],
                &q[nTracksFoundSoFar],
                ALFA,
                BETA,
                GAMMA,
                TypeConf
                      );

      if(Status[nTracksFoundSoFar] < 0  ) {

        continue;
      }




//  this trasformation is valid even if the equation is a straight line from the fit

      Ox[nTracksFoundSoFar]= -0.5*ALFA[nTracksFoundSoFar];
      Oy[nTracksFoundSoFar]= -0.5*BETA[nTracksFoundSoFar];
      R[nTracksFoundSoFar]= Ox[nTracksFoundSoFar]*Ox[nTracksFoundSoFar]+Oy[nTracksFoundSoFar]*Oy[nTracksFoundSoFar]-
                            GAMMA[nTracksFoundSoFar];
      if( R[nTracksFoundSoFar] < 0. )   continue;
      R[nTracksFoundSoFar]= sqrt( R[nTracksFoundSoFar] );



//---------------------  better association of the hits in the track candidate

  ITRACCIA = nTracksFoundSoFar;



  NN =  PndSttTrkAssociatedParallelHitsToHelixTris(
                   m[nTracksFoundSoFar],
                   q[nTracksFoundSoFar],
                   Status[nTracksFoundSoFar],
                   nHitsinTrack[nTracksFoundSoFar],
                   &ListHitsinTrack[nTracksFoundSoFar][0],
                   Minclinations[0],
                   infoparalConformal,
                   RConformalIndex,
                   FiConformalIndex,
                   nBoxConformal,
                   HitsinBoxConformal,
                   auxListHitsinTrack              //  this is the output
                                                     );



   if( NN < MINIMUMHITSPERTRACK ) {
     continue;
   }
   nHitsinTrack[nTracksFoundSoFar]=NN;
   for(i=0; i<nHitsinTrack[nTracksFoundSoFar];i++){
     ListHitsinTrack[nTracksFoundSoFar][i]=auxListHitsinTrack[i];
   }





//  this trasformation is valid even if the equation is a straight line from the fit

      Ox[nTracksFoundSoFar]= -0.5*ALFA[nTracksFoundSoFar];
      Oy[nTracksFoundSoFar]= -0.5*BETA[nTracksFoundSoFar];
      R[nTracksFoundSoFar]= Ox[nTracksFoundSoFar]*Ox[nTracksFoundSoFar]+Oy[nTracksFoundSoFar]*Oy[nTracksFoundSoFar]-
                            GAMMA[nTracksFoundSoFar];
      if( R[nTracksFoundSoFar] < 0. )   continue;
      R[nTracksFoundSoFar]= sqrt( R[nTracksFoundSoFar] );



// --------  here the track and its hits were found, filling the exclusion list

   for(j=0; j<nHitsinTrack[nTracksFoundSoFar]; j++){
     ExclusionList[    infoparal[   ListHitsinTrack[nTracksFoundSoFar][j]  ] ] = false;
   }











   nTracksFoundSoFar++;

  }      // end  of   for(iParHit=0; iParHit<Minclinations[0] + 1 -  MINIMUMHITSPERTRACK; iParHit++)





//-----------------------  doing the fit with the skew hits for each XY plane track found
  UShort_t ipinco;
  for(i=0,  ipinco=0; i<nTracksFoundSoFar;i++){


//-----  finding the skew hits intersecting this XY trajectory circle


  TemporarynSkewHitsinTrack = AssociateSkewHitsToXYTrack(
                   Ox[i],   //  input : X of center of XY plane circle
                   Oy[i],   //  input : Y of center of XY plane circle
                   R[i],   //  input : Radius of XY plane circle
                   info,
                   inclination,
                   TemporarySkewList, // output,  list of selected skew hits (in skew numbering)
                   S,       //  output,  S coordinate of selected Skew hit
                   Z,       //  output,  Z coordinate of selected Skew hit
                   ZDrift,   //  output,  drift distance IN Z DIRECTION only, of selected Skew hit
                   ZErrorafterTilt   //  output,  Radius taking into account the tilt, IN Z DIRECTION only, of selected Skew hit
                                                     );

    nSkewHitsinTrack[i]=TemporarynSkewHitsinTrack;
  if( TemporarynSkewHitsinTrack ==0) {
    continue;
  }




//
      Status[i] = PndSttFitSZspace( 
                TemporarynSkewHitsinTrack,
                S,
                Z,
                ZDrift,
                i,   //   index of the track in XY plane
                1.571,   //  rotationangle always 90 degrees
                NHITSINFIT,
                &KAPPA[i],
                &FI0[i]
                      );

      if(Status[i] < 0  ) {
        continue;
      }





    for(j=0;j<TemporarynSkewHitsinTrack;j++){
       ListSkewHitsinTrack[i][j]=TemporarySkewList[j][0];
    }
    nSkewHitsinTrack[i]=TemporarynSkewHitsinTrack;




//  }    //   end of   for(i=0; i<nTracksFoundSoFar;i++)




    HoughR = R[i];
    HoughD   = sqrt(Ox[i]*Ox[i]+
                                    Oy[i]*Oy[i]) -R[i];
    HoughFi = atan2(Oy[i],Ox[i]);
    if(HoughFi<0.)  HoughFi += 2.*PI;








//    ordering the parallel and skew hits; determining the charge of this track


       PndSttOrdering(
                             Ox[i],
                             Oy[i],
                             info,
                             nHitsinTrack[i],
                             &ListHitsinTrack[i][0],
                             nSkewHitsinTrack[i],
                             &ListSkewHitsinTrack[i][0],
                             S,
                             infoparal,
                             infoskew,
                             &nTotalHits,
                             BigList,
                             &Charge
                       );


//----      start the better track parameter determination and spurious (mainly skew) hits rejection with a Kalman fit.

       Double_t dista=sqrt( Ox[i]*Ox[i]+Oy[i]*Oy[i] );
       Double_t Ptras = R[i]*0.003*BFIELD;
       Double_t Pzini = 0.003*BFIELD/KAPPA[i];
       Double_t Pxini = Ptras*Oy[i]/dista;
       Double_t Pyini = -Ptras*Ox[i]/dista;



//----      end of the better track parameter determination and spurious (mainly skew) hits rejection with a Kalman fit.





//   loading the hits found and associates to a track in a  PndSttTrack  class; a class per each track

/*
         new((*trackArray)[ipinco])  PndSttTrack;
         PndSttTrack *pt = (PndSttTrack*) (*trackArray)[ipinco];
         ipinco++;
         pt->GetParamLast()->SetTx(R[i]);
         Double_t inv=R[i]*KAPPA[i];
         if(fabs(inv) > 1.e-10){
            pt->GetParamLast()->SetTy(1./inv);
         }else{
            pt->GetParamLast()->SetTy(1.e10);
         }

         ptotal = sqrt(Pxini*Pxini+Pyini*Pyini+Pzini*Pzini);
         if( fabs(ptotal)  > 1.e-10){
             pt->GetParamLast()->SetQp(Charge/ptotal);
         } else {
             pt->GetParamLast()->SetQp(Charge/1.e-10);
         }

*/

   }   //  end of     for(i=0; i<nTracksFoundSoFar;i++)








  return;


 }; //----------------------------------------------------   end of function  PndSttTrackFinderReal::PndSttTrkFinderPartial








CalculatedCircles PndSttTrackFinderReal::PndSttTrkFindCircles(Double_t x1,Double_t y1,Double_t r1,
                                                 Double_t x2,Double_t y2,Double_t r2,
                                                 Double_t x3,Double_t y3,Double_t r3)
{
  Double_t a,b,c,d,ap,bp,cp,dp,radix,solution,Radius, A,B,C,D,AAA,BBB,CCC,DELTA,S,T,X;
  Int_t Nsol = 0;
  Double_t R[8],CX[8],CY[8];
  CalculatedCircles Ris ;



/*
  x1 = X center crf 1;
  y1 = Y center crf 1;
  r1 = radius crf 1;
  x2 = X center crf 2;
  y2 = Y center crf 2;
  r2 = radius crf 2;
  x3 = X center crf 3;
  y3 = Y center crf 3;
  r3 = radius crf 3;
*/

  a=2.*(x1-x2);
  b=2.*(y1-y2);
  d= x1*x1+y1*y1-r1*r1-x2*x2-y2*y2+r2*r2;
  ap=2.*(x1-x3);
  bp=2.*(y1-y3);
  dp= x1*x1+y1*y1-r1*r1-x3*x3-y3*y3+r3*r3;


  Double_t aaa= a*bp-b*ap;
//cout<<"aaa  : "<<aaa<<endl;
  if ( aaa == 0.) {
   for(Int_t i1=-1;i1<=1;i1+=2){
    for(Int_t i2=-1;i2<=1;i2+=2){
      c=-2.*(i1*r1-i2*r2);
      for(Int_t i3=-1;i3<=1;i3+=2){
        cp=-2.*(i1*r1-i3*r3);
        if( bp*c-b*cp == 0. && b == 0. && bp == 0. && a != 0. && ap != 0. && a != ap && ap*c-a*cp != 0.){
           Radius = (a*dp-ap*d)/(ap*c-a*cp);
           if ( Radius >0.){
             S = d/a;
             T = -c/a;
             BBB = y1;
             CCC = (S-T*Radius)*(S-T*Radius)-Radius*Radius-2.*x1*(S-T*Radius)-2.*i1*Radius*r1+x1*x1+y1*y1-r1*r1;
             DELTA = BBB*BBB-CCC;
             if(DELTA<0.) {
              continue;
             } else if (DELTA ==0.){
              Nsol++;
              R[Nsol-1] = Radius ;
              CX[Nsol-1] = S-T*Radius;
              CY[Nsol-1] = -BBB;
             } else{
              Nsol+=2;
              R[Nsol-2] = Radius ;
              CX[Nsol-2] = S-T*Radius;
              CY[Nsol-2] = -BBB+sqrt(DELTA);
              R[Nsol-1] = Radius ;
              CX[Nsol-1] = S-T*Radius;
              CY[Nsol-1] = -BBB-sqrt(DELTA);
             }

           }  //  end  if ( Radius >0.)
        }  else if(bp*c-b*cp != 0.) {

         Radius = (-bp*d+b*dp)/(bp*c-b*cp) ;
         if( Radius > 0. && b != 0. ){
           S = (d+c*Radius)/b;
           T = a/b;
           AAA = 1+T*T;
           BBB = -(T*S+x1-T*y1);
           CCC = S*S-Radius*Radius-2.*y1*S-2.*i1*r1*Radius+x1*x1+y1*y1-r1*r1;
           DELTA= BBB*BBB-AAA*CCC;
//cout<<"AAA ="<<AAA<<",  BBB= "<<BBB<<", CCC ="<<CCC<<", DELTA= "<<DELTA<<endl;
//cout<<"S = "<<S<<", T= "<<T<<endl<<"DELTA "<<DELTA<<endl;
           if(DELTA<0.) {
            continue;
           } else if (DELTA ==0.){
             Nsol++;
             R[Nsol-1] = Radius ;
             CX[Nsol-1] = -BBB/AAA;
             CY[Nsol-1] = S-T*CX[Nsol-1];
// cout<<"CX[Nsol-1] ="<<CX[Nsol-1]<<",  CY[Nsol-1]= "<<CY[Nsol-1]<<endl;
           } else {
             Nsol+=2;
             R[Nsol-2] = Radius ;
             CX[Nsol-2] = (-BBB+sqrt(DELTA))/AAA;
             CY[Nsol-2] = S-T*CX[Nsol-2];
             R[Nsol-1] = Radius ;
             CX[Nsol-1] = (-BBB-sqrt(DELTA))/AAA;
             CY[Nsol-1] = S-T*CX[Nsol-1];
//cout<<"CX[Nsol-2] ="<<CX[Nsol-2]<<",  CY[Nsol-2]= "<<CY[Nsol-2]<<endl;
//cout<<"CX[Nsol-1] ="<<CX[Nsol-1]<<",  CY[Nsol-1]= "<<CY[Nsol-1]<<endl;
           }  // end  if(DELTA<0.)
         }   //  end  if( Radius > 0. && b != 0. )
       }    //   end if( bp*c-b*cp == 0.) 
      }    //  end for (Int_t i3
    }    //  end for(Int_t i2
   }   //  end for(Int_t i1

  } else {   // of if(aaa == 0. )

   A=(bp*d-b*dp)/aaa;
   C=(-ap*d+a*dp)/aaa;
// cout<<"aaa "<<aaa<<endl<<"A "<<A<<endl<<"C "<<C<<endl;
   for(Int_t i1=-1;i1<=1;i1+=2){
    for(Int_t i2=-1;i2<=1;i2+=2){
      c=-2.*(i1*r1-i2*r2);
      for(Int_t i3=-1;i3<=1;i3+=2){
        cp=-2.*(i1*r1-i3*r3);
// cout<<"----------------------------------------\n";
// cout<<"c="<<c<<";  cp="<<cp<<endl;

        B=(bp*c-b*cp)/aaa;
        D=(-ap*c+a*cp)/aaa;

        AAA = B*B+D*D-1.;
        BBB = A*B+C*D-B*x1-D*y1-i1*r1;
        CCC = A*A+C*C-2.*x1*A-2.*y1*C+x1*x1+y1*y1-r1*r1;
        DELTA = BBB*BBB - CCC*AAA;
//        cout<<"caso con i1= "<<i1<<", i2, i3 = "<<" "<<i2<<" "<<i3<<endl
//              <<"A "<<A<<";  B = "<<B<<"; C= "<<C<<"; D = "<<D<<endl;
//        cout<<"AAA = "<<AAA<<";  BBB = "<<BBB<<" DELTA "<<DELTA<<endl;
//        cout<<"sqrt(DELTA) = "<<sqrt(DELTA)<<endl;

        if (DELTA <0.) { continue; }
        else if (DELTA==0.){
         if( BBB==0. ){
          continue;
         } else {
           solution=-BBB/AAA;
           if(solution > 0.){
           Nsol++;
           R[Nsol-1] = solution;
             CX[Nsol-1] = A+B*R[Nsol-1];
             CY[Nsol-1] = C+D*R[Nsol-1];
           }
         continue;
        }
       }

//  this is the case when DELTA > 0.

        radix = sqrt(DELTA);

        if (AAA == 0.) {
          cout<<"AAA== 0.";
          continue;
        } else {

          solution=(-BBB+radix)/AAA;
//cout<<"-BBB[j]  radix  AAA[j] : "<<-BBB <<";  "<< radix <<"; "<< AAA<<"; solution=+ :"<<solution<<endl;
          if(solution > 0.){
           Nsol++;
           R[Nsol-1] = solution;
           CX[Nsol-1] = A+B*R[Nsol-1];
           CY[Nsol-1] = C+D*R[Nsol-1];
          }


          solution=(-BBB-radix)/AAA;
//cout<<"-BBB[j]  radix  AAA[j] : "<<-BBB <<";  "<< radix <<"; "<< AAA<<"; solution=- :"<<solution<<endl;
          if(solution > 0.){
           Nsol++;
           R[Nsol-1] = solution;
           CX[Nsol-1] = A+B*R[Nsol-1];
           CY[Nsol-1] = C+D*R[Nsol-1];
          }



        }     //  end of   if(AAA == 0.)


      }    //  end  for(i3
    }     //  end  for(i2
   }     //  end  for(i1
    


  }   // end if ( aaa == 0.)




//cout<<"----------------------------------------\n";
//  cout<<"n. soluzioni : "<<Nsol<<endl;
 Ris.Ncircles=Nsol;
 for(Int_t i=0; i<Nsol;i++){
//  cout<<"Soluzione n. "<<i+1<<"; R = "<<R[i]<<", CX = "<<CX[i]<<", CY = "<<CY[i]<<endl;
  Ris.CX[i]=CX[i];
  Ris.CY[i]=CY[i];
  Ris.R[i] =R[i];
 }



 return Ris;  

}

//----------------------------------------------------   end of function  PndSttTrackFinderReal::PndSttTrkFindCircles








//----------------------------------------------------   begin of function  PndSttTrackFinderReal::calculateintersections

 void PndSttTrackFinderReal::calculateintersections(Double_t Ox,Double_t Oy,Double_t R,Double_t C0x,Double_t C0y,
                   Double_t C0z,Double_t r,Double_t vx,Double_t vy,Double_t vz,
                   Int_t *STATUS, Double_t* POINTS)
{


//-------------------------------------------


  Double_t P1x, P1y, P1z, P2x, P2y, P2z;

  Double_t AAA, DELTA, ax, ay, aaa;




/*
 INPUTS :

  Ox, Oy        = abscissa and ordinate of the center of the circular trajectory of
                  the particle;
  R             = radius of such trajectory;
  C0x, C0y, Coz = x, y, z coordinates of a point belonging to the axis of the
                  skewed straw;
  r  = radius of equidrift of such skewed straw;
  vx, vy, vz    =  versor of the direction along which the skewed straw lies.


 OUTPUTS :

  P1x, P1y, P1z  =  x, y, z coordinates of the point intersection between the
                    particle trajectory circle and the equidrift cylinder of
                    the skewed straw calculated as a function of theta (first
                    solution);
  P2x, P2y, P2z  =  x, y, z coordinates of the point intersection between the
                    particle trajectory circle and the equidrift cylinder of
                    the skewed straw calculated as a function of theta (second
                    solution);

 P1x, P1y, .... , P2z  are stored in the array POINTS[0-5];  the status is stored
 in *STATUS.

*/




//--------------------------------------------------------------------------


// from the resolving formula on page 23 of Gianluigi's notes, setting  r=0.


  ax = C0x - Ox;
  ay = C0y - Oy;
  DELTA = R*R*(vx*vx+vy*vy) - (vx*ay - vy*ax)*(vx*ay - vy*ax);
  AAA = vx*vx+vy*vy;


  if ( DELTA < 0. ) {
      *STATUS=-2;
  } else if (AAA == 0.){
      *STATUS=-3;
  } else if (DELTA == 0.){
      *STATUS=1;
      POINTS[0] = C0x - vx*(vx*ax + vy*ay)/AAA;
      POINTS[1] = C0y - vy*(vx*ax + vy*ay)/AAA;
      POINTS[2] = C0z - vz*(vx*ax + vy*ay)/AAA;
  }else {
      *STATUS=0;
      DELTA = sqrt(DELTA);
      POINTS[0] = C0x - vx*(vx*ax + vy*ay - DELTA)/AAA;
      POINTS[1] = C0y - vy*(vx*ax + vy*ay - DELTA)/AAA;
      POINTS[2] = C0z - vz*(vx*ax + vy*ay - DELTA)/AAA;
      POINTS[3] = C0x - vx*(vx*ax + vy*ay + DELTA)/AAA;
      POINTS[4] = C0y - vy*(vx*ax + vy*ay + DELTA)/AAA;
      POINTS[5] = C0z - vz*(vx*ax + vy*ay + DELTA)/AAA;
  }

   return;


}



//----------end of function PndSttTrackFinderReal::calculateintersections























  bool  PndSttTrackFinderReal::iscontiguous(
                int ncomponents, UShort_t * vec1, UShort_t *vec2)
{

    for(int i=0; i<ncomponents;i++){
      if( vec1[i]-vec2[i] >3 || vec1[i]-vec2[i]<-3) return false;
    }

    return true;

}



//----------end of function PndSttTrackFinderReal::iscontiguous





//----------begin of function PndSttTrackFinderReal::PndSttfromXYtoConformal

 void PndSttTrackFinderReal::PndSttFromXYtoConformal(Double_t trajectory_vertex[3],
                            Double_t info[][6],
                            Int_t Nparal,Double_t infoparalConformal[][5],
                            Int_t *status )
{


//   do the transformation in the conformal space :  u= x/(x**2+y**2), v= y/(x**2+y**2) for each hit from parallel
//   straws;  also the equidrift radius changes.

//

    Double_t gamma, x, y, r;

    for(int i=0; i<Nparal; i++){
            x = info[infoparal[i]][0]-trajectory_vertex[0];
            y = info[infoparal[i]][1]-trajectory_vertex[1];
            r = info[infoparal[i]][3];
            gamma = x*x + y*y - r*r;
            if(fabs( gamma ) < 1.e-10) {
              *status = -1;
              continue;
            }
            infoparalConformal[i][0] = x / gamma;
            infoparalConformal[i][1] = y / gamma;
            infoparalConformal[i][2] = r/fabs(gamma);
            infoparalConformal[i][3] = infoparal[i] ;      //  n. of the Hit (in the original order)
            infoparalConformal[i][4] = StrawRadius/fabs(gamma);

    }


   *status=0;
   return;
}

//----------end of function PndSttTrackFinderReal::PndSttfromXYtoConformal


























//----------begin of function PndSttTrackFinderReal::PndSttBoxConformalFilling

 void  PndSttTrackFinderReal::PndSttBoxConformalFilling(
                                                        Double_t infoparalConformal[][5],Int_t Nparal,
                                                        UShort_t nBoxConformal[nRdivConformal][nFidivConformal],
                                                        UShort_t HitsinBoxConformal[nRdivConformal][nFidivConformal][nmaxHits],
                                                        UShort_t  RConformalIndex[nmaxHits],
                                                        UShort_t  FiConformalIndex[nmaxHits]   )
{

    Short_t iR, iFi, i, j;
    Double_t Fi;



      for(i = 0; i< nRdivConformalEffective ; i++){
        for(j = 0; j< nFidivConformal ; j++){
           nBoxConformal[i][j]= 0;
        }
      }

      for(i = 0; i< Nparal ; i++){

         Fi =  atan2(infoparalConformal[i][1],infoparalConformal[i][0]) ;
         if ( Fi < 0. ) Fi += 2.*PI;
         iFi =  (Short_t) (0.5*nFidivConformal*Fi/PI);
         if(iFi > nFidivConformal ) {
            iFi = nFidivConformal;
         } else if (iFi<0) {
            iFi = 0;
         }


         Double_t RRR = sqrt(infoparalConformal[i][0]*infoparalConformal[i][0]+infoparalConformal[i][1]*infoparalConformal[i][1]);
//   cout<<"from Fillng : hit parallel n. "<<i<<"  con raggio conforme "<<RRR<<endl;

         for(j=nRdivConformalEffective-1, iR=0; j>0; j--){
           if( RRR> radiaConf[j] ){
              iR = j;
              break;
           }
         }
         HitsinBoxConformal[iR][iFi][ nBoxConformal[iR][iFi] ]=(UShort_t) i;
         nBoxConformal[iR][iFi]++;
         RConformalIndex[ infoparal[i] ]  =  iR;
         FiConformalIndex[ infoparal[i] ]  =  iFi;
      }



    return;

}
//----------end of function PndSttTrackFinderReal::PndSttBoxConformalFilling



//----------begin of function PndSttTrackFinderReal::PndStt_Merge_Sort



void PndSttTrackFinderReal::PndStt_Merge_Sort(UShort_t n_ele, Double_t *array, UShort_t *ind)
{

  UShort_t nr, nl, middle, i,
           ind_left[n_ele], ind_right[n_ele];

  Double_t left[n_ele], right[n_ele], result[n_ele];

   if( n_ele <= 1)  return;

   middle = n_ele/2 ;
   for(i=0; i<middle; i++){
     left[i]=array[i];
     ind_left[i]= ind[i];
   }
   for(i=middle; i<n_ele; i++){
     right[i-middle]=array[i];
     ind_right[i-middle]= ind[i];
   }

   PndStt_Merge_Sort( middle,  left, ind_left);
   PndStt_Merge_Sort(n_ele-middle, right, ind_right);

   if( left[middle-1] > right[0]) {
     PndStt_Merge(middle, left,ind_left, n_ele-middle, right, ind_right, array, ind);
   }  else {
     //  do the appending
     for(i=0; i<middle; i++){
       array[i]=left[i];
       ind[i]=ind_left[i];
       
     }
     for(i=middle; i<n_ele; i++){
       array[i]=right[i-middle];
       ind[i]=ind_right[i-middle];
     }
   }


}


//----------end of function PndSttTrackFinderReal::PndStt_Merge_Sort


//----------begin of function PndSttTrackFinderReal::PndStt_Merge




void PndSttTrackFinderReal::PndStt_Merge(UShort_t nl, Double_t *left, UShort_t *ind_left, UShort_t nr,
                                         Double_t *right, UShort_t *ind_right,  Double_t *result, UShort_t *ind)
{
   UShort_t i =0, j, nl_curr=0, nr_curr=0;

   while( nl > 0 && nr >0){
     if( left[nl_curr] <= right[nr_curr]){
      result[i] =  left[nl_curr];
      ind[i] = ind_left[nl_curr];
      nl--;
      nl_curr++;
     } else {
      result[i] =  right [nr_curr];
      ind[i] =  ind_right [nr_curr];
      nr--;
      nr_curr++;
     }
    i++;
   }
//--------------------
   if( nl ==0) {
     for(j=0; j<nr; j++){
      result[i+j]= right[nr_curr+j];
      ind[i+j]= ind_right[nr_curr+j];
     }
   }   else {
     for(j=0; j<nl; j++){
      result[i+j]= left[nl_curr+j];
      ind[i+j]= ind_left[nl_curr+j];
     }
   }





}

//----------end of function PndSttTrackFinderReal::PndStt_Merge





//----------begin of function PndSttTrackFinderReal::PndSttFindTrackPatterninBoxConformal


  Short_t PndSttTrackFinderReal::PndSttFindTrackPatterninBoxConformal(
                                                  UShort_t NRCELLDISTANCE,
                                                  UShort_t NFiCELLDISTANCE,
                                                  UShort_t Nparal,
                                                  UShort_t ihit,
                                                  Double_t info[][6],
                                                  bool ExclusionList[nmaxHits],
                                                  UShort_t RConformalIndex[nmaxHits],
                                                  UShort_t FiConformalIndex[nmaxHits],
                                                  UShort_t nBoxConformal[nRdivConformal][nFidivConformal],
                                                  UShort_t HitsinBoxConformal[nRdivConformal][nFidivConformal][nmaxHits],
                                                  UShort_t *ListHitsinTrack
                                                                      )
{





     bool TemporaryExclusionList[nmaxHits];

     UShort_t i, j, iR, iFi, nRmin, nRmax, nRemainingHits, nRcell, nFicell, nHitsinTrack,
                 Remaining[nmaxHits],
                 auxIndex[nmaxHits];

     Short_t iFi2;

     Double_t auxRvalues[nmaxHits];

//   ihit        is the hit number in the PARALLEL number scheme




     for(i=0, nRemainingHits=0; i<Nparal; i++){
        if( i != ihit && ExclusionList[  infoparal[i]   ] ) {   //  exclusion of the parallel hit straws already used in other tracks
                                                           //  remember the index of ExclusionList is in the ORIGINAL scheme of hits
            TemporaryExclusionList[ infoparal[i]  ]= true;
            Remaining[nRemainingHits]= i;   //  index of the PARALLEL hit
            nRemainingHits++;
        } else {
            TemporaryExclusionList[ infoparal[i]  ]= false;
        }

     }


     if( nRemainingHits < MINIMUMHITSPERTRACK )    return 0;



//  cells of the seed hit

    nHitsinTrack=1;
    ListHitsinTrack[0]=  ihit ;

   i = 0;
   while( nRemainingHits > 0 &&  i < nHitsinTrack) {

    nRcell = RConformalIndex[   infoparal[  ListHitsinTrack[i] ]   ];
    nFicell = FiConformalIndex[  infoparal[  ListHitsinTrack[i]  ]  ];

//---------------

    if (nRcell - NRCELLDISTANCE < 0 ) {
      nRmin = 0;
    }  else {
      nRmin = nRcell - NRCELLDISTANCE;
    }
    if (nRcell + NRCELLDISTANCE >= nRdivConformalEffective ) {
      nRmax = nRdivConformalEffective-1;
    }  else {
      nRmax = nRcell + NRCELLDISTANCE;
    }


    for( iR= nRmin ; iR<= nRmax ; iR++){
      for( iFi2 = nFicell - NFiCELLDISTANCE ; iFi2<= nFicell + NFiCELLDISTANCE ; iFi2++){
          if ( iFi2 < 0 )  {
            iFi = nFidivConformal + iFi2;
          } else if ( iFi2 >= nFidivConformal) {
            iFi = iFi2  - nFidivConformal;
          }  else {
            iFi = iFi2;
          }
         for (j = 0; j< nBoxConformal[iR][iFi]; j++){
          if( ExclusionList[  infoparal[  HitsinBoxConformal[iR][iFi][j]  ]  ]
                                      &&
              TemporaryExclusionList[   infoparal[  HitsinBoxConformal[iR][iFi][j]  ]   ]) {
            ListHitsinTrack[nHitsinTrack]=HitsinBoxConformal[iR][iFi][j] ;   //  hit number in the PARALLEL straws scheme

            nHitsinTrack++;
            TemporaryExclusionList[ infoparal[  HitsinBoxConformal[iR][iFi][j]  ]  ]= false;
            nRemainingHits--;
          }
         }
      }
    }
//----------------
    i++;


   }    //  end      while ( nRemainingHits > 0 && i < nHitsinTrack)



//   ordering the hits by INCREASING CONFORMAL RADIUS (decreasing space radius)



    return nHitsinTrack;

}


//----------end of function PndSttTrackFinderReal::PndSttFindTrackPatterninBoxConformal


//----------begin of function PndSttTrackFinderReal::PndSttFindTrackPatterninBoxConformalSpecial


  Short_t PndSttTrackFinderReal::PndSttFindTrackPatterninBoxConformalSpecial(
                                                  UShort_t NRCELLDISTANCE,
                                                  UShort_t NFiCELLDISTANCE,                                                  
                                                  UShort_t Nparal,
                                                  UShort_t NparallelToSearch,
                                                  UShort_t iSeed,
                                                  UShort_t *ListHitsinTrackinWhichToSearch,
                                                  Double_t info[][6],
                                                  bool ExclusionList[nmaxHits],
                                                  UShort_t RConformalIndex[nmaxHits],
                                                  UShort_t FiConformalIndex[nmaxHits],
                                                  UShort_t nBoxConformal[nRdivConformal][nFidivConformal],
                                                  UShort_t HitsinBoxConformal[nRdivConformal][nFidivConformal][nmaxHits],
                                                  UShort_t  *OutputListHitsinTrack
                                                                      )
{





     bool TemporaryExclusionList[nmaxHits];

     UShort_t i, i2, j, iR, iFi, nRmin, nRmax, nRemainingHits, nRcell, nFicell, nHitsinTrack,
                 Remaining[nmaxHits],
                 auxIndex[nmaxHits];

     Short_t iFi2;

     Double_t auxRvalues[nmaxHits];

//   iSeed        is the hit number in the PARALLEL number scheme

//cout<<" Nparal "<<Nparal<<",  NparallelToSearch "<<NparallelToSearch<<
//", iSeed   "<<iSeed<< endl;
//--------    the following initialization is essential for the algorithm to work
     for(i=0; i<Nparal; i++){
       TemporaryExclusionList[ infoparal[i]  ]= false;
     }
//-------------

     for(i2=0, nRemainingHits=0; i2<NparallelToSearch; i2++){
        i=ListHitsinTrackinWhichToSearch[i2];
        if( i != iSeed && ExclusionList[  infoparal[i]   ] ) {   //  exclusion of the parallel hit straws already used in other tracks
                                                           //  remember the index of ExclusionList is in the ORIGINAL scheme of hits
            TemporaryExclusionList[ infoparal[i]  ]= true;
            Remaining[nRemainingHits]= i;   //  index of the PARALLEL hit
            nRemainingHits++;
        } 

     }


     if( nRemainingHits < MINIMUMHITSPERTRACK )    return 0;

//  cells of the seed hit

    nHitsinTrack=1;
    OutputListHitsinTrack[0]=  iSeed ;
//cout<<"From PatterninBoxConformal -------------------- hit seed n. (orig.) "<<OutputListHitsinTrack[0]<<endl;
   i = 0;
   while( nRemainingHits > 0 &&  i < nHitsinTrack) {

    nRcell = RConformalIndex[   infoparal[  OutputListHitsinTrack[i] ]   ];
    nFicell = FiConformalIndex[  infoparal[  OutputListHitsinTrack[i]  ]  ];
//cout<<"From PatterninBoxConformal HIT collezionatore ora ha nRcell e nFicell "<<nRcell<<",  "<< nFicell<<endl;

//---------------

    if (nRcell - NRCELLDISTANCE < 0 ) {
      nRmin = 0;
    }  else {
      nRmin = nRcell - NRCELLDISTANCE;
    }
    if (nRcell + NRCELLDISTANCE >= nRdivConformalEffective ) {
      nRmax = nRdivConformalEffective-1;
    }  else {
      nRmax = nRcell + NRCELLDISTANCE;
    }


    for( iR= nRmin ; iR<= nRmax ; iR++){
      for( iFi2 = nFicell - NFiCELLDISTANCE ; iFi2<= nFicell + NFiCELLDISTANCE ; iFi2++){
          if ( iFi2 < 0 )  {
            iFi = nFidivConformal + iFi2;
          } else if ( iFi2 >= nFidivConformal) {
            iFi = iFi2  - nFidivConformal;
          }  else {
            iFi = iFi2;
          }
         for (j = 0; j< nBoxConformal[iR][iFi]; j++){
          if( ExclusionList[  infoparal[  HitsinBoxConformal[iR][iFi][j]  ]  ]
                                      &&
              TemporaryExclusionList[   infoparal[  HitsinBoxConformal[iR][iFi][j]  ]   ]) {
            OutputListHitsinTrack[nHitsinTrack]=HitsinBoxConformal[iR][iFi][j] ;   //  hit number in the PARALLEL straws scheme
            nHitsinTrack++;
            TemporaryExclusionList[ infoparal[  HitsinBoxConformal[iR][iFi][j]  ]  ]= false;
            nRemainingHits--;
          }
         }
      }
    }
//----------------
    i++;

   }    //  end      while ( nRemainingHits > 0 && i < nHitsinTrack)






    return nHitsinTrack;

}


//----------end of function PndSttTrackFinderReal::PndSttFindTrackPatterninBoxConformalSpecial






//----------begin of function PndSttTrackFinderReal::PndSttFindTrackStrictCollection


  Short_t PndSttTrackFinderReal::PndSttFindTrackStrictCollection(
                                                  UShort_t NFiCELLDISTANCE,                                                  
                                                  UShort_t iSeed,   //  seed track (parallel notation) as fa as the Fi angle is concerned
                                                  UShort_t NParallelToSearch,    //  n. of hits to search in ListHitsinTrackinWhichToSearch
                                                  UShort_t *ListHitsinTrackinWhichToSearch,
                                                  bool ExclusionList[nmaxHits],
                                                  UShort_t FiConformalIndex[nmaxHits],
                                                  UShort_t  *OutputListHitsinTrack
                                                                      )
{






     UShort_t i,  j, iR, iFi,  iFiseed, nHitsinTrack;


     Double_t auxRvalues[nmaxHits];




//   iSeed        is the hit number in the PARALLEL number scheme

     iFiseed = FiConformalIndex[ infoparal[  iSeed ] ];

     nHitsinTrack=0;
     for(i=0; i<NParallelToSearch; i++){
        if( ExclusionList[  infoparal[ ListHitsinTrackinWhichToSearch[i] ]   ] ) {   //  exclusion of the parallel hit straws already used in other tracks
                                                           //  remember the index of ExclusionList is in the ORIGINAL scheme of hits

          iFi = FiConformalIndex[ infoparal[  ListHitsinTrackinWhichToSearch[i] ] ];
          if( iFi == iFiseed ) {
            OutputListHitsinTrack[nHitsinTrack]=ListHitsinTrackinWhichToSearch[i];
            nHitsinTrack++;
          } else if ( iFi < iFiseed ) {
            if( iFiseed - iFi <= NFiCELLDISTANCE ) {
              OutputListHitsinTrack[nHitsinTrack]=ListHitsinTrackinWhichToSearch[i];
              nHitsinTrack++;
            }  else {
               if( iFi + nFidivConformal - iFiseed<= NFiCELLDISTANCE ) {
                 OutputListHitsinTrack[nHitsinTrack]=ListHitsinTrackinWhichToSearch[i];
                 nHitsinTrack++;
               }
            }
          }   else {   //  iFi > iFiseed
            if( -iFiseed + iFi <= NFiCELLDISTANCE ) {
              OutputListHitsinTrack[nHitsinTrack]=ListHitsinTrackinWhichToSearch[i];
              nHitsinTrack++;
            }  else {
               if( -iFi + nFidivConformal + iFiseed<= NFiCELLDISTANCE ) {
                 OutputListHitsinTrack[nHitsinTrack]=ListHitsinTrackinWhichToSearch[i];
                 nHitsinTrack++;
               }
            }

          }   //  end of   if( iFi == iFiseed )

        }    //  end of    if( ExclusionList[  infoparal[ ListHitsinTrackinWhichToSearch[i] ]   ] )

     }   //  end of        for(i=0; i<NparallelToSearch; i++)



    return nHitsinTrack;

}


//----------end of function PndSttTrackFinderReal::PndSttFindTrackStrictCollection








//----------begin of function PndSttTrackFinderReal::PndSttFitHelixCylinder

      Short_t PndSttTrackFinderReal::PndSttFitHelixCylinder( UShort_t nHitsinTrack,
                                                     Double_t auxinfoparalConformal[][5],
                                                     UShort_t  nTracksFoundSoFar,
                                                     Double_t rotationangle,
                                                     Double_t * trajectory_vertex,
                                                     UShort_t NMAX,
                                                     Double_t *emme,
                                                     Double_t *qu,
                                                     Double_t *ALFA,
                                                     Double_t *BETA,
                                                     Double_t *GAMMA,
                                                     bool *TypeConf
                                                            )
{

    //   definition of variables for the glpsol  solver
   //    ROWS (for read_rows  function)
   //
   UShort_t  NpointsInFit = nHitsinTrack-NMAX <0 ?  nHitsinTrack :  NMAX;
   int    nRows= NpointsInFit*9 +1;
   int typeRows[nRows];
   char * nameRows[nRows];
   char  auxnameRows[nRows][20];
//-------  end ROWS information
//--------begin COLUMNS information
      int  NStructVar=5+NpointsInFit*4;  //  number of  structural variables
      int  NStructRows = 8*NpointsInFit ;  //  maximum number of ROWS in which a structural variable can be found
      double final_values[NStructVar];
      int  NRowsInWhichStructVarArePresent[NStructVar];
      char *StructVarName[NStructVar];
      char auxStructVarName[NStructVar][20];
      char *NameRowsInWhichStructVarArePresent[NStructVar*NStructRows];
      char aux[NStructVar*NStructRows][20];
//      double Coefficients[NStructVar][NStructRows];
      double Coefficients[NStructVar*NStructRows];
//--------end COLUMNS information
//--------begin RHS information
      double ValueB[9*NpointsInFit];
//--------end RHS information
//--------begin RANGES information
      int nRanges = NpointsInFit;
      double ValueRanges[nRanges];
      char *NameRanges[nRanges];
      char auxNameRanges[nRanges][20];
//--------end RANGES information
//--------start BOUNDS information
      int nBounds=2*NpointsInFit+1;
      double BoundValue[nBounds];
      char *BoundStructVarName[nBounds];
      char auxBoundStructVarName[nBounds][20];
      char *TypeofBound[nBounds];
//--------end BOUNDS information




     Double_t M = 1.,
              m_result,
              q_result,
              A,
              alfetta,
              angle,
              offsety,
              Delta[nmaxHits],
              Ox[nmaxHits],
              Oy[nmaxHits];

     UShort_t  i, ii;
     Short_t Status;

     char nome[100], stringa[100], stringa2[100];

     FILE * MACRO ;

     float m1_result,m2_result, q1_result,q2_result, A1_result, A2_result;

// --

     if( nHitsinTrack < MINIMUMHITSPERTRACK) {
        return -1;
     }

//  use the trick of increasing the rotation angle by 10 degrees in order to obtain always a positive m
      rotationangle -= PI/18.;

      Double_t cose = cos(rotationangle), sine = sin(rotationangle);
      for(i=0;i<nHitsinTrack; i++){
       Ox[i] = auxinfoparalConformal[ i ][0] *cose +
               auxinfoparalConformal[ i ][1]*sine;
       Oy[i] = -auxinfoparalConformal[ i ][0] *sine +
               auxinfoparalConformal[ i ][1]*cose;







         Delta[i] = 3.*auxinfoparalConformal[ i ][2];

      }

//      sprintf(nome,"GeneralParallelHitsConformeTraccia%dEvent%d.mcs",(nTracksFoundSoFar), IVOLTE);
//      MACRO = fopen(nome,"w");
//-----------------  write the ROWS  section


//--------
      nameRows[0]="OBJECT";
      typeRows[0]=GLP_FR;
      for(i=0 ; i< NpointsInFit ; i++) {
       ii=9*i;

       typeRows[1+ii]=GLP_UP;typeRows[2+ii]=GLP_UP;typeRows[3+ii]=GLP_UP;typeRows[4+ii]=GLP_UP;
       typeRows[5+ii]=GLP_UP;typeRows[6+ii]=GLP_UP;typeRows[7+ii]=GLP_UP;typeRows[8+ii]=GLP_UP;
       typeRows[9+ii]=GLP_LO;

       sprintf(&(auxnameRows[1+ii][0]),"Ap%d",i);  nameRows[1+ii]=&auxnameRows[1+ii][0];
       sprintf(&(auxnameRows[2+ii][0]),"Bp%d",i);  nameRows[2+ii]=&auxnameRows[2+ii][0];
       sprintf(&(auxnameRows[3+ii][0]),"Cp%d",i);  nameRows[3+ii]=&auxnameRows[3+ii][0];
       sprintf(&(auxnameRows[4+ii][0]),"Dp%d",i);  nameRows[4+ii]=&auxnameRows[4+ii][0];
       sprintf(&(auxnameRows[5+ii][0]),"Am%d",i);  nameRows[5+ii]=&auxnameRows[5+ii][0];
       sprintf(&(auxnameRows[6+ii][0]),"Bm%d",i);  nameRows[6+ii]=&auxnameRows[6+ii][0];
       sprintf(&(auxnameRows[7+ii][0]),"Cm%d",i);  nameRows[7+ii]=&auxnameRows[7+ii][0];
       sprintf(&(auxnameRows[8+ii][0]),"Dm%d",i);  nameRows[8+ii]=&auxnameRows[8+ii][0];
       sprintf(&(auxnameRows[9+ii][0]),"LAMBDA%d",i);  nameRows[9+ii]=&auxnameRows[9+ii][0];
      }




//-----------------  write the COLUMNS  section


//  Column variable  m1
      for(i=0, ii=0 ; i< NpointsInFit ; i++) {
       ii++;
        Coefficients[i*4]=    Ox[i];
        Coefficients[i*4+1]=  Ox[i];
        Coefficients[i*4+2]= -Ox[i];
        Coefficients[i*4+3]= -Ox[i];
      }

//  Column variable  m2
      for(i=0; i< NpointsInFit ; i++) {
        Coefficients[NStructRows+i*4]=   -Ox[i];
        Coefficients[NStructRows+i*4+1]= -Ox[i];
        Coefficients[NStructRows+i*4+2]= Ox[i];
        Coefficients[NStructRows+i*4+3]= Ox[i];

      }

//  Column variable  q1
      for(i=0 ; i< NpointsInFit ; i++) {
        Coefficients[2*NStructRows+i*4]=    1.;
        Coefficients[2*NStructRows+i*4+1]=  1.;
        Coefficients[2*NStructRows+i*4+2]= -1.;
        Coefficients[2*NStructRows+i*4+3]= -1.;
      }

//  Column variable  q2
      for(i=0 ; i< NpointsInFit ; i++) {
        Coefficients[3*NStructRows+i*4]=   -1.;
        Coefficients[3*NStructRows+i*4+1]= -1.;
        Coefficients[3*NStructRows+i*4+2]=  1.;
        Coefficients[3*NStructRows+i*4+3]=  1.;
      }

//  Column variable  lambdap(i)
      for(i=0 ; i< NpointsInFit ; i++) {
        Coefficients[(4+i)*NStructRows+0]= -M;
        Coefficients[(4+i)*NStructRows+1]= -M;
        Coefficients[(4+i)*NStructRows+2]= -M;
        Coefficients[(4+i)*NStructRows+3]=  M;
        Coefficients[(4+i)*NStructRows+4]=  1.;
      }
//  Column variable  lambdam(i)
      for(i=0 ; i< NpointsInFit ; i++) {
        Coefficients[(4+i+NpointsInFit)*NStructRows+0]= -M;
        Coefficients[(4+i+NpointsInFit)*NStructRows+1]= -M;
        Coefficients[(4+i+NpointsInFit)*NStructRows+2]= -M;
        Coefficients[(4+i+NpointsInFit)*NStructRows+3]=  M;
        Coefficients[(4+i+NpointsInFit)*NStructRows+4]=  1.;
      }
//  Column variable  sigmap(i)
      for(i=0; i< NpointsInFit ; i++) {

        Coefficients[(4+i+2*NpointsInFit)*NStructRows+0]=  1./Delta[i];
        Coefficients[(4+i+2*NpointsInFit)*NStructRows+1]= -1.;
        Coefficients[(4+i+2*NpointsInFit)*NStructRows+2]= -1.;
        Coefficients[(4+i+2*NpointsInFit)*NStructRows+3]=  1.;
        Coefficients[(4+i+2*NpointsInFit)*NStructRows+4]= -1.;
      }
//  Column variable  sigmam(i)
      for(i=0 ; i< NpointsInFit ; i++) {
        Coefficients[(4+i+3*NpointsInFit)*NStructRows+0]=  1./Delta[i];
        Coefficients[(4+i+3*NpointsInFit)*NStructRows+1]= -1.;
        Coefficients[(4+i+3*NpointsInFit)*NStructRows+2]= -1.;
        Coefficients[(4+i+3*NpointsInFit)*NStructRows+3]=  1.;
        Coefficients[(4+i+3*NpointsInFit)*NStructRows+4]= -1.;
      }

//  Column variable  DUMMY
      for(i=0 ; i< NStructRows ; i++) {
        Coefficients[(4+4*NpointsInFit)*NStructRows+i]= 1.;
      }
//--------------------
      StructVarName[0]="m1";
      NRowsInWhichStructVarArePresent[0]= 4*NpointsInFit;
      StructVarName[1]="m2";
      NRowsInWhichStructVarArePresent[1]= 4*NpointsInFit;
      StructVarName[2]="q1";
      NRowsInWhichStructVarArePresent[2]= 4*NpointsInFit;
      StructVarName[3]="q2";
      NRowsInWhichStructVarArePresent[3]= 4*NpointsInFit;
      for(i=0; i< NpointsInFit ; i++) {
          sprintf(&auxStructVarName[3+i+1][0],"lamp%d",i);
          StructVarName[4+i] = &auxStructVarName[4+i][0];
          NRowsInWhichStructVarArePresent[4+i]= 5;

          sprintf(&auxStructVarName[4+NpointsInFit+i][0],"lamm%d",i);
          StructVarName[4+NpointsInFit+i] = &auxStructVarName[4+NpointsInFit+i][0];
          NRowsInWhichStructVarArePresent[4+NpointsInFit+i]= 5;

          sprintf(&auxStructVarName[4+2*NpointsInFit+i][0],"sigmap%d",i);
          StructVarName[4+2*NpointsInFit+i] = &auxStructVarName[4+2*NpointsInFit+i][0];
          NRowsInWhichStructVarArePresent[4+2*NpointsInFit+i]= 5;

          sprintf(&auxStructVarName[4+3*NpointsInFit+i][0],"sigmam%d",i);
          StructVarName[4+3*NpointsInFit+i] = &auxStructVarName[4+3*NpointsInFit+i][0];
          NRowsInWhichStructVarArePresent[4+3*NpointsInFit+i]= 5;

      }
      StructVarName[4+4*NpointsInFit]="DUMMY";
      NRowsInWhichStructVarArePresent[4+4*NpointsInFit]= NStructRows;


//  for m1, m2, q1, q2
      for(i=0; i< 4; i++){
        for(ii=0; ii< NpointsInFit;ii++){
         sprintf(&aux[i*NStructRows+ii*4][0],"Ap%d",ii);
         NameRowsInWhichStructVarArePresent[i*NStructRows+ii*4]=&aux[i*NStructRows+ii*4][0];
         sprintf(&aux[i*NStructRows+ii*4+1][0],"Am%d",ii);
         NameRowsInWhichStructVarArePresent[i*NStructRows+ii*4+1]=&aux[i*NStructRows+ii*4+1][0];
         sprintf(&aux[i*NStructRows+ii*4+2][0],"Bp%d",ii);
         NameRowsInWhichStructVarArePresent[i*NStructRows+ii*4+2]=&aux[i*NStructRows+ii*4+2][0];
         sprintf(&aux[i*NStructRows+ii*4+3][0],"Bm%d",ii);
         NameRowsInWhichStructVarArePresent[i*NStructRows+ii*4+3]=&aux[i*NStructRows+ii*4+3][0];
        }
      }

//  now for the    lamp*   variables
      for(i=0; i< NpointsInFit;i++){
         sprintf(&aux[(i+4)*NStructRows+0][0],"Ap%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRows+0]= &aux[(i+4)*NStructRows+0][0];
         sprintf(&aux[(i+4)*NStructRows+1][0],"Bp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRows+1]= &aux[(i+4)*NStructRows+1][0];
         sprintf(&aux[(i+4)*NStructRows+2][0],"Cp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRows+2]= &aux[(i+4)*NStructRows+2][0];
         sprintf(&aux[(i+4)*NStructRows+3][0],"Dp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRows+3]= &aux[(i+4)*NStructRows+3][0];
         sprintf(&aux[(i+4)*NStructRows+4][0],"LAMBDA%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRows+4]= &aux[(i+4)*NStructRows+4][0];
      }

//  now for the    lamm*   variables
      for(i=0; i< NpointsInFit;i++){
         sprintf(&aux[(i+4+NpointsInFit)*NStructRows+0][0],"Am%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit)*NStructRows+0]= &aux[(i+4+NpointsInFit)*NStructRows+0][0];
         sprintf(&aux[(i+4+NpointsInFit)*NStructRows+1][0],"Bm%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit)*NStructRows+1]= &aux[(i+4+NpointsInFit)*NStructRows+1][0];
         sprintf(&aux[(i+4+NpointsInFit)*NStructRows+2][0],"Cm%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit)*NStructRows+2]= &aux[(i+4+NpointsInFit)*NStructRows+2][0];
         sprintf(&aux[(i+4+NpointsInFit)*NStructRows+3][0],"Dm%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit)*NStructRows+3]= &aux[(i+4+NpointsInFit)*NStructRows+3][0];
         sprintf(&aux[(i+4+NpointsInFit)*NStructRows+4][0],"LAMBDA%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit)*NStructRows+4]= &aux[(i+4+NpointsInFit)*NStructRows+4][0];
      }

//  now for the    sigmap*   variables
      for(i=0; i< NpointsInFit;i++){
         sprintf(&aux[(i+4+2*NpointsInFit)*NStructRows+0][0],"OBJECT",i);
         NameRowsInWhichStructVarArePresent[(i+4+2*NpointsInFit)*NStructRows+0]= &aux[(i+4+2*NpointsInFit)*NStructRows+0][0];
         sprintf(&aux[(i+4+2*NpointsInFit)*NStructRows+1][0],"Ap%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+2*NpointsInFit)*NStructRows+1]= &aux[(i+4+2*NpointsInFit)*NStructRows+1][0];
         sprintf(&aux[(i+4+2*NpointsInFit)*NStructRows+2][0],"Bp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+2*NpointsInFit)*NStructRows+2]= &aux[(i+4+2*NpointsInFit)*NStructRows+2][0];
         sprintf(&aux[(i+4+2*NpointsInFit)*NStructRows+3][0],"Cp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+2*NpointsInFit)*NStructRows+3]= &aux[(i+4+2*NpointsInFit)*NStructRows+3][0];
         sprintf(&aux[(i+4+2*NpointsInFit)*NStructRows+4][0],"Dp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+2*NpointsInFit)*NStructRows+4]= &aux[(i+4+2*NpointsInFit)*NStructRows+4][0];
      }

//  now for the    sigmam*   variables
      for(i=0; i< NpointsInFit;i++){
         sprintf(&aux[(i+4+3*NpointsInFit)*NStructRows+0][0],"OBJECT",i);
         NameRowsInWhichStructVarArePresent[(i+4+3*NpointsInFit)*NStructRows+0]= &aux[(i+4+3*NpointsInFit)*NStructRows+0][0];
         sprintf(&aux[(i+4+3*NpointsInFit)*NStructRows+1][0],"Am%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+3*NpointsInFit)*NStructRows+1]= &aux[(i+4+3*NpointsInFit)*NStructRows+1][0];
         sprintf(&aux[(i+4+3*NpointsInFit)*NStructRows+2][0],"Bm%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+3*NpointsInFit)*NStructRows+2]= &aux[(i+4+3*NpointsInFit)*NStructRows+2][0];
         sprintf(&aux[(i+4+3*NpointsInFit)*NStructRows+3][0],"Cm%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+3*NpointsInFit)*NStructRows+3]= &aux[(i+4+3*NpointsInFit)*NStructRows+3][0];
         sprintf(&aux[(i+4+3*NpointsInFit)*NStructRows+4][0],"Dm%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+3*NpointsInFit)*NStructRows+4]= &aux[(i+4+3*NpointsInFit)*NStructRows+4][0];
      }

//  now for the    DUMMY   variable
      for(i=0; i< NpointsInFit;i++){
         sprintf(&aux[(4+4*NpointsInFit)*NStructRows  +8*i][0],"Ap%d",i);
         NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+i*8  ]= &aux[(4+4*NpointsInFit)*NStructRows  +8*i][0];

         sprintf(&aux[(4+4*NpointsInFit)*NStructRows+1+8*i][0],"Am%d",i);
         NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+1+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+1+8*i][0];
         sprintf(&aux[(4+4*NpointsInFit)*NStructRows+2+8*i][0],"Bp%d",i);
         NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+2+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+2+8*i][0];
         sprintf(&aux[(4+4*NpointsInFit)*NStructRows+3+8*i][0],"Bm%d",i);
         NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+3+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+3+8*i][0];
         sprintf(&aux[(4+4*NpointsInFit)*NStructRows+4+8*i][0],"Cp%d",i);
         NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+4+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+4+8*i][0];
         sprintf(&aux[(4+4*NpointsInFit)*NStructRows+5+8*i][0],"Cm%d",i);
         NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+5+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+5+8*i][0];
         sprintf(&aux[(4+4*NpointsInFit)*NStructRows+6+8*i][0],"Dp%d",i);
         NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+6+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+6+8*i][0];
         sprintf(&aux[(4+4*NpointsInFit)*NStructRows+7+8*i][0],"Dm%d",i);
         NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+7+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+7+8*i][0];
      }



//-----------------  write the RHS  section

      for(i=0 ; i< NpointsInFit ; i++) {
          ValueB[i*9]  =  Oy[i]+auxinfoparalConformal[ i ][2]+2.*M;
          ValueB[i*9+1]= -Oy[i]-auxinfoparalConformal[ i ][2]+2.*M;
          ValueB[i*9+2]= Delta[i]+2.*M;
          ValueB[i*9+3]= M-Delta[i]+2.*M;


          ValueB[i*9+4]=  Oy[i]-auxinfoparalConformal[ i ][2]+2.*M;
          ValueB[i*9+5]= -Oy[i]+auxinfoparalConformal[ i ][2]+2.*M;
          ValueB[i*9+6]= Delta[i]+2.*M;
          ValueB[i*9+7]= M-Delta[i]+2.*M;
          ValueB[i*9+8]= 1.;


      }


//-----------------  write the RANGES  section

      for(i=0 ; i< NpointsInFit ; i++) {
        ValueRanges[i]=1.;
        sprintf(&auxNameRanges[i][0],"LAMBDA%d",i);
        NameRanges[i]=&auxNameRanges[i][0];
      }

//-----------------  write the BOUNDS  section


      for(i=0 ; i< NpointsInFit ; i++) {
          TypeofBound[i]="BV";
          sprintf(&auxBoundStructVarName[i][0],"lamp%d",i);
          BoundStructVarName[i]=&auxBoundStructVarName[i][0];
          BoundValue[i]=0.;
      }

      for(i=0 ; i< NpointsInFit ; i++) {
          TypeofBound[i+NpointsInFit]="BV";
          sprintf(&auxBoundStructVarName[i+NpointsInFit][0],"lamm%d",i);
          BoundStructVarName[i+NpointsInFit]=&auxBoundStructVarName[i+NpointsInFit][0];
          BoundValue[i+NpointsInFit]=0.;
      }

          TypeofBound[2*NpointsInFit]="FX";
          BoundStructVarName[2*NpointsInFit]="DUMMY";
          BoundValue[2*NpointsInFit]=2.;
//-----

//      fclose(MACRO);



      int status= glp_main(
            nRows,nameRows,typeRows, //  ROWS info
            NStructVar, NStructRows, NRowsInWhichStructVarArePresent,  //  COLUMNS info
      StructVarName, NameRowsInWhichStructVarArePresent,  //  COLUMNS info
      Coefficients,  //  COLUMNS info
      ValueB,  // RHS  info
      nRanges, ValueRanges, NameRanges, //  RANGES  info
      nBounds, BoundValue, BoundStructVarName, TypeofBound //  BOUNDS info
      ,final_values
       );










     m1_result = final_values[0];
     m2_result = final_values[1];
     q1_result = final_values[2];
     q2_result = final_values[3];




//------------------------  transformation of the result in terms of ALFA, BETA, GAMMA


     *qu = q1_result - q2_result;
     *emme = m1_result-m2_result ;

    GAMMA[nTracksFoundSoFar] = 0.;
    if( fabs( *qu ) > 1.e-10) {    //  trajectory is a circle in XY space
     ALFA[nTracksFoundSoFar] = *emme/(*qu);
     BETA[nTracksFoundSoFar] = -1./(*qu);
     TypeConf[nTracksFoundSoFar]=true;
//  now take into account the rotation and correct; the only affected quantities are ALFA and BETA
      alfetta = ALFA[nTracksFoundSoFar];
      ALFA[nTracksFoundSoFar] = ALFA[nTracksFoundSoFar]*cose - BETA[nTracksFoundSoFar]*sine;
      BETA[nTracksFoundSoFar] = alfetta*sine + BETA[nTracksFoundSoFar]*cose;
    }  else if(fabs(*emme)> 1.e-10)  {    //  trajectory is a straight line in XY space of equation y= m*x
       //  the rotation first
       angle = atan(*emme) + rotationangle;
       if( fabs(cos(angle)) > 1.e-10 ) {
         ALFA[nTracksFoundSoFar] = 999999.;
         BETA[nTracksFoundSoFar] = -ALFA[nTracksFoundSoFar]/tan(angle);

       } else {  //  in this case the equation is y = 0.
         ALFA[nTracksFoundSoFar] = 999999.;
         BETA[nTracksFoundSoFar] = 0.;
         TypeConf[nTracksFoundSoFar]=false;
       }
    }  else {   //  in this case also the equation in XY plane is  y = 0.
         ALFA[nTracksFoundSoFar] = 999999.;
         BETA[nTracksFoundSoFar] = 0.;
         TypeConf[nTracksFoundSoFar]=false;
    }


// now take into account the displacement and correct
      GAMMA[nTracksFoundSoFar] += (trajectory_vertex[0]*trajectory_vertex[0]+ trajectory_vertex[1]*trajectory_vertex[1]
                                  -ALFA[nTracksFoundSoFar]*trajectory_vertex[0]-BETA[nTracksFoundSoFar]*trajectory_vertex[1]);
      ALFA[nTracksFoundSoFar] -=  2.*trajectory_vertex[0];
      BETA[nTracksFoundSoFar] -=  2.*trajectory_vertex[1];


//------------------------ end of transformation of the result in terms of ALFA, BETA, GAMMA



//--------   end of taking into account the traslation that was performed and undoing that


// taking into account the rotation that was performed and calculate emme and qu in the normal conformal plane

      if(fabs(cose-*emme*sine)> 1.e-10) {
        *qu=*qu/(cose-*emme*sine);
        *emme=(*emme*cose+sine)/(cose-*emme*sine);
        return 1;
      } else {    //  in this case the equation is   0 = x+*qu .
        if(fabs(sine+*emme*cose) < 1.e-10)  {
  cout<<" From PndSttFitHelixCylinder, situation impossible in principle! Returning -1"
                    <<endl;
           return -1;
        }

        *emme=1.;
        *qu = *qu/(sine+*emme*cose);
        return 99;    //  in this case the equation is   0 = x+*qu .
      }




}




//----------end of function PndSttTrackFinderReal::PndSttFitHelixCylinder





















//----------begin of function PndSttTrackFinderReal::PndSttTrkAssociatedParallelHitsToHelixTris
  UShort_t PndSttTrackFinderReal::PndSttTrkAssociatedParallelHitsToHelixTris(
                   Double_t m,
                   Double_t q,
                   Short_t Status,
                   UShort_t nHitsinTrack,
                   UShort_t *ListHitsinTrack,
                   Int_t NhitsParallel,
                   Double_t infoparalConformal[][5],
                   UShort_t *RConformalIndex,
                   UShort_t *FiConformalIndex,
                   UShort_t nBoxConformal[nRdivConformal][nFidivConformal],
                   UShort_t HitsinBoxConformal[nRdivConformal][nFidivConformal][nmaxHits],
                   UShort_t *auxListHitsinTrack
                                                     )
{
  bool passamin,passamax,
       Unselected[nmaxHits];

  Short_t i, i2, j, k, l,  l2, l3, itemp, kstart, kend,
          iFi0;
  UShort_t Nextra=8,
           nFi,
           Fi,
           FFimin, FFimax,
           nR,
           nAssociatedHits;
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
           fi2;

  nAssociatedHits=0;
  for(i=0; i<NhitsParallel;i++){
   Unselected[i]= true;
  }

//   find the range in Fi spanned  by the candidate track

  FFimin = 10000;
  FFimax = 0;
  for(j=0; j<nHitsinTrack; j++){
    i = (UShort_t)  infoparalConformal[ ListHitsinTrack[j] ][3];
  }
    if( FiConformalIndex[i] <  FFimin ) FFimin = FiConformalIndex[i];
    if( FiConformalIndex[i] >  FFimax ) FFimax = FiConformalIndex[i];


  if( FFimax > 3.*nFidivConformal/4. && FFimin < nFidivConformal/4.) {
     FFimin = 10000;
     FFimax =  0;
     for(j=0; j<nHitsinTrack; j++){
       i = (UShort_t)  infoparalConformal[ ListHitsinTrack[j] ][3];
       Fi = FiConformalIndex[i];
       if( Fi < nFidivConformal/4. ) Fi = FiConformalIndex[i]+nFidivConformal;
       if( Fi <  FFimin ) FFimin = Fi;
       if( Fi >  FFimax ) FFimax = Fi;
     }
  }


//  finding the boundaries in the Conformal plane. The basic assumption is that the range
// in Fi is much less that 180 degrees.
  
  FFimin -= (Short_t) nFidivConformal/Nextra;
  FFimax +=  (Short_t) nFidivConformal/Nextra;
  if( FFimax - FFimin > nFidivConformal/2 ) {
    cout<<"something fishy is going on in PndSttTrkAssociatedParallelHitsToHelixTris!"
      <<"Range in Fi (rad) is  "<<(FFimax - FFimin)*2.*PI/nFidivConformal<<endl;
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
          i += nFidivConformal;
        } else if (i>=nFidivConformal){
          i -=  nFidivConformal*( i/nFidivConformal );
        }
        angle = (i+0.5)*2.*PI/nFidivConformal;
        aaa = cos(angle);
        if( fabs(cos(angle)) <1.e-10)  continue;
        r = -q/aaa;
        if(r< radiaConf[0] || r>= 1./RStrawDetectorMin)  continue;
        for(j=nRdivConformalEffective-1; j>=0;j--){
          if( r>= radiaConf[j] ){
           nR = j;
           break;
          }
        }


        for(l=-DELTAnR; l<DELTAnR+1;l++){
          l2 = nR+l;
          if(  l2<0 || l2 >= nRdivConformalEffective )  continue;
              for( k=0;k<nBoxConformal[l2][i];k++){
                xx=infoparalConformal[  HitsinBoxConformal[l2][i][k]  ][0];
                dist = fabs( xx +q );
                if(  PndSttAcceptHitsConformal(  dist,
                                                 infoparalConformal[  HitsinBoxConformal[l2][i][k]  ][2],
                                                 infoparalConformal[  HitsinBoxConformal[l2][i][k]  ][4]
                                                      ) )  {
                    auxListHitsinTrack[nAssociatedHits]= HitsinBoxConformal[l2][i][k];
                    nAssociatedHits++; 
                }
              }
        }   //   end of for(l=0; l<3;l++)

        // ------- special cases
           if((nR == nRdivConformalEffective-1 && passamin && ! passamax) ||  (nR==0 && passamax && !passamin)  ) {   //  do the last two Fi columns
            if(nR == nRdivConformalEffective-1)  passamax=true;
            if(nR == 0)   passamin=true;

            for(l2=1;l2<3;l2++){
             i2 = i+l2;
             if(i2>=nFidivConformal) i2 -=  nFidivConformal;
            for(l=-2; l<3;l++){
             l3 = nR+l;
             if(  l3<0 || l3 >= nRdivConformalEffective )  continue;
              for( k=0;k<nBoxConformal[l3][i2];k++){
                xx=infoparalConformal[  HitsinBoxConformal[l3][i2][k]  ][0];
                dist = fabs( xx +q );
                if(  PndSttAcceptHitsConformal(  dist,
                                                 infoparalConformal[  HitsinBoxConformal[l3][i2][k]  ][2],
                                                 infoparalConformal[  HitsinBoxConformal[l3][i2][k]  ][4]
                                                      ) )  {
                    auxListHitsinTrack[nAssociatedHits]= HitsinBoxConformal[l3][i2][k];
                    nAssociatedHits++; 
                }
              }
            }   //   end of for(l=-2; l<3;l++)
            }   //   end of for(l2=0;l2<2;l2++)
            return  nAssociatedHits;
           } else if ((nR == nRdivConformalEffective-1 && ! passamin && !passamax) || (nR==0 && !passamax && !passamin)){
             if(nR == nRdivConformalEffective-1)  passamax=true;
             if(nR == 0)   passamin=true;

            for(l2=1;l2<3;l2++){
             i2 = i-l2;
             if(i2<nFidivConformal) i2 += nFidivConformal;
            for(l=-2; l<3;l++){
             l3 = nR+l;
             if(  l3<0 || l3 >= nRdivConformalEffective )  continue;
              for( k=0;k<nBoxConformal[l3][i2];k++){
                xx=infoparalConformal[  HitsinBoxConformal[l3][i2][k]  ][0];
                dist = fabs( xx +q );
                if(  PndSttAcceptHitsConformal(  dist,
                                                 infoparalConformal[  HitsinBoxConformal[l3][i2][k]  ][2],
                                                 infoparalConformal[  HitsinBoxConformal[l3][i2][k]  ][4]
                                                      ) )  {
                    auxListHitsinTrack[nAssociatedHits]= HitsinBoxConformal[l3][i2][k];
                    nAssociatedHits++; 
                }
              }
            }   //   end of for(l=-2; l<3;l++)
            }   //   end of for(l2=0;l2<2;l2++)
           }    //   end of if((nR == nRdivConformalEffective-1 && passamin) ||  (nR==0 && passamax)  )



      }   //  end of     for(itemp=Fimin; itemp<=FFimax;itemp++)


    } else {  //  q=0 --> x=0



      if( FFimax > nRdivConformal/4 && Fimin < nRdivConformal/4 ) {
        iFi0 =  (Short_t)  (nRdivConformal/4 );
      } else if ( FFimax > 3*nRdivConformal/4 && Fimin < 3*nRdivConformal/4 ){
        iFi0 =  (Short_t)  (3*nRdivConformal/4 );
      }  else {
                cout <<"From PndSttTrackFinderReal::PndSttTrkAssociatedParallelHitsToHelixTris  :"
                      <<"  inconsistency, 0 associated hits to this track candidate\n";
        return 0;
     }

      for(itemp=iFi0-5; itemp<=iFi0+5;itemp++){
        i=itemp;
        if( i< 0 ) {
          i += nFidivConformal;
        } else if (i>=nFidivConformal){
          i -=  nFidivConformal*( i/nFidivConformal );
        }
        for(l=0; l<nRdivConformalEffective;l++){
              for( k=0;k<nBoxConformal[l][i];k++){
                xx=infoparalConformal[  HitsinBoxConformal[l][i][k]  ][0];
                dist = fabs( xx  );

                if(  PndSttAcceptHitsConformal(  dist,
                                                 infoparalConformal[  HitsinBoxConformal[l][i][k]  ][2],
                                                 infoparalConformal[  HitsinBoxConformal[l][i][k]  ][4]
                                                      ) )  {
                    auxListHitsinTrack[nAssociatedHits]= HitsinBoxConformal[l][i][k];
                    nAssociatedHits++;
                }
              }  //  end of for( k=0;k<nBoxConformal[l][i];k++)
        }  //  end of for(l=0; l<nRdivConformalEffective;l++)
      }   //  end of for(itemp=iFi0-5; itemp<=iFi0+5;itemp++)




      }    //   end  of if(fabs(q) > 1.e-10 )


  }  else if( fabs(q)> 1.e-10) {   //   second part of    if( Status ==99),  in this case y = m*x +q


        Fi0 = atan2(q, -m*q);
        if(Fi0<0.)  { Fi0 += PI; if (Fi0 <0. ) Fi0 =0.; };

        ddd= fabs(q)/sqrt(1.+m*m);

        for(itemp=FFimin; itemp<=FFimax;itemp++){
         i=itemp;
         if( i< 0 ) {
            i += nFidivConformal;
           } else if (i>=nFidivConformal){
            i -=  nFidivConformal*( i/nFidivConformal );
           }


              fi1 = i*2.*(PI/nFidivConformal);
              if( fabs(sin(fi1)-m*cos(fi1))>1.e-10) {
                 erre1 = q/(sin(fi1)-m*cos(fi1));
              }  else {
                 erre1 = 99999999999.;
              }

              fi2 = (i+1)*2.*(PI/nFidivConformal);
              if( fabs(sin(fi2)-m*cos(fi2))>1.e-10) {
                 erre2 = q/(sin(fi2)-m*cos(fi2));
              }  else {
                 erre2 = 99999999999.;
              }





         for(j=0; j<nRdivConformal; j++){
              Rin = radiaConf[j];
              if(j!=nRdivConformal-1) {
                 Rout =  radiaConf[j+1];
              }  else {
                 Rout = 1./RStrawDetectorMin;
              }

//  note that the following algorithm works also for negative erre1  and   erre2


              if(erre1<-1.e-10 ){
                if(erre2< 0. || erre2 > Rout ){
                     continue;
                } 
              } else if(fabs(erre1) < 1.e-10){
                if( Fi0 > fi2 || Fi0 < fi1)  continue;
              } else if ( erre1<Rin) {
                if( erre2< Rin )  continue;
              }   else if (erre1> Rout  &&  erre2 > Rout && !( fi1<=Fi0 && Fi0<=fi2 && ddd<=Rout )
                ) {
                   continue;
             }

              for(l=itemp-2; l<=itemp+2; l++){
                if( l< 0 ) {
                  l2 = l+nFidivConformal;
                } else if (l>=nFidivConformal){
                  l2 = l- nFidivConformal*( i/nFidivConformal );
               } else {
                 l2 = l;
               }
                if( j-1<0) { 
                  kstart=0;
                }  else {
                  kstart = j-1;
                }
                if ( j+1 >=  nRdivConformal ) {
                  kend = nRdivConformal;
                } else {
                  kend = j+2;
                }

                for(k=kstart;k<kend;k++){

                 for( l3=0;l3<nBoxConformal[k][l2];l3++){
                   if( ! Unselected[HitsinBoxConformal[k][l2][l3] ] )  continue;
                   xx=infoparalConformal[  HitsinBoxConformal[k][l2][l3]  ][0];
                   yy=infoparalConformal[  HitsinBoxConformal[k][l2][l3]  ][1];
                   dist = fabs( -yy+ m*xx +q )/sqrt(m*m+1.);
                   if(  PndSttAcceptHitsConformal(  dist,
                                                 infoparalConformal[  HitsinBoxConformal[k][l2][l3]  ][2],
                                                 infoparalConformal[  HitsinBoxConformal[k][l2][l3]  ][4]
                                                      ) )  {

                    auxListHitsinTrack[nAssociatedHits]= HitsinBoxConformal[k][l2][l3];
                    Unselected[HitsinBoxConformal[k][l2][l3]]= false;
                    nAssociatedHits++;
                   }

                 }   //   end of  for( l3=0;l3<nBoxConformal[k][l2];l3++)

                }   //   end of  for(k=j-1;k<j+2;k++)
              }     //   end of  for(l=itemp-1; l<itemp+2; l++)


         }   //  end of for(j=0; j<nRdivConformal; j++)
        }   //   end of    for(itemp=FFimin; itemp<=FFimax;itemp++)





  } else {  //  case in which    y= m*x ,  m can be zero    ,  third part of if( Status ==99)

      iFi0 =  (Short_t)  (atan(m)*nRdivConformal/(2.*PI) );
      for(itemp=iFi0-5; itemp<=iFi0+5;itemp++){
         i=itemp;
         if( i< 0 ) {
          i += nFidivConformal;
         } else if (i>=nFidivConformal){
          i -=  nFidivConformal*( i/nFidivConformal );
//          i -= nFidivConformal;
         }
        for(l=0; l<nRdivConformalEffective;l++){
              for( k=0;k<nBoxConformal[l][i];k++){
                xx=infoparalConformal[  HitsinBoxConformal[l][i][k]  ][0];
                yy=infoparalConformal[  HitsinBoxConformal[l][i][k]  ][1];
                dist = fabs( m*xx-yy  )/sqrt( m*m+1.);
                if(  PndSttAcceptHitsConformal(  dist,
                                                 infoparalConformal[  HitsinBoxConformal[l][i][k]  ][2],
                                                 infoparalConformal[  HitsinBoxConformal[l][i][k]  ][4]
                                                      ) )  {
                    auxListHitsinTrack[nAssociatedHits]= HitsinBoxConformal[l][i][k];
                    nAssociatedHits++;
                }
              }  //  end of for( k=0;k<nBoxConformal[l][i];k++)
        }
      }   //  end of for(itemp=FFimin; itemp<=FFimax;itemp++)





  }   //   end of if ( Status ==99)






 return nAssociatedHits;

}



//----------end of function PndSttTrackFinderReal::PndSttTrkAssociatedParallelHitsToHelixTris







//----------begin of function PndSttTrackFinderReal::PndSttAcceptHitsConformal

bool  PndSttTrackFinderReal::PndSttAcceptHitsConformal(  Double_t  distance,
                                                         Double_t  DriftConfR, //drift radius in conformal space
                                                         Double_t  StrawConfR  // straw radius in conformal space
                                                      )
{
//if( IVOLTE==2 && ITRACCIA ==8) cout <<" fabs(distance-DriftConfR)  "<<fabs(distance-DriftConfR)<<
//       ",   2.*StrawConfR  "<< 2.*StrawConfR<<endl;
     if(  fabs(distance-DriftConfR)  <   2.*StrawConfR )   return true;
     return false;


}




//----------end of function PndSttTrackFinderReal::PndSttAcceptHitsConformal






//----------begin of function PndSttTrackFinderReal::AssociateSkewHitsToXYTrack

  UShort_t PndSttTrackFinderReal::AssociateSkewHitsToXYTrack(
                   Double_t Ox,
                   Double_t Oy,
                   Double_t R,
                   Double_t info[][6],
                   Double_t inclination[][3],
                   UShort_t SkewList[nmaxHits][2], // output,  list of selected skew hits (in skew numbering)
                   Double_t *S,       //  output,  S coordinate of selected Skew hit
                   Double_t *Z,       //  output,  Z coordinate of selected Skew hit
                   Double_t *ZDrift,   //  output,  drift distance IN Z DIRECTION only, of selected Skew hit
                   Double_t *ZErrorafterTilt   //  output,  Radius taking into account the tilt, IN Z DIRECTION only, of selected Skew hit
                                                     )
 {



    Int_t i, j, i1, ii, iii, NAssociated, Kincl, nlow, nup, STATUS, Nmin, Nmax;

    Double_t xmin , xmax, ymin, ymax,
           dx, dy, diff, d1, d2,
           delta, deltax, deltay, deltaz, deltaS,
           factor,
           zmin, zmax, Smin, Smax, S1, S2,
           z1, z2, y1, y2,
           vx1, vy1, vz1, C0x1, C0y1, C0z1,
           aaa, bbb, ccc, angle, minor, major,
           distance, Rx, Ry, LL,
           Aellipsis1, Bellipsis1,fi1,
           fmin, fmax, offset, step,
           SkewInclWithRespectToS, zpos, zpos1, zpos2,
           Tiltdirection1[2],
           zl[200],zu[200],
           POINTS1[6];




      NAssociated=0;

       for( iii=0; iii< NSkewhits; iii++) {
         i = infoskew[iii];


         Kincl = (int) info[i][5] - 1;


         aaa = sqrt(inclination[Kincl][0]*inclination[Kincl][0]+inclination[Kincl][1]*inclination[Kincl][1]+
                  inclination[Kincl][2]*inclination[Kincl][2]);
         vx1 = inclination[Kincl][0]/aaa;
         vy1 = inclination[Kincl][1]/aaa;
         vz1 = inclination[Kincl][2]/aaa;
         C0x1 = info[i][0];
         C0y1 = info[i][1];
         C0z1 = info[i][2];

       calculateintersections(Ox,Oy,R,C0x1,C0y1,C0z1,info[i][3],
                              vx1,vy1,vz1,
                              &STATUS,POINTS1);

       if(STATUS < 0 ) continue ;


       for( ii=0; ii<2; ii++){

        j=3*ii;
        distance = sqrt(
                  (POINTS1[j]-C0x1)*(POINTS1[j]-C0x1) + 
                  (POINTS1[1+j]-C0y1)*(POINTS1[1+j]-C0y1) + 
                  (POINTS1[2+j]-C0z1)*(POINTS1[2+j]-C0z1) 
                            );
        if( distance >= info[i][4] ) continue;


        Rx = POINTS1[j]-Ox ;   //  x component Radial vector of cylinder of trajectory
        Ry = POINTS1[1+j]-Oy ;   //  y direction Radial vector of cylinder of trajectory

        aaa = sqrt(Rx*Rx+Ry*Ry);
        SkewInclWithRespectToS = (-Ry*vx1 + Rx*vy1)/aaa ;
        SkewInclWithRespectToS /= R;
        bbb = sqrt( SkewInclWithRespectToS*SkewInclWithRespectToS + vz1*vz1);
        //  the tilt direction of this ellipse is (1,0)  when major axis along Z direction
        if( bbb > 1.e-10){
           Tiltdirection1[0] = vz1/bbb;
           Tiltdirection1[1] = SkewInclWithRespectToS/bbb;
        } else {
           Tiltdirection1[0] = 1.;
           Tiltdirection1[1] = 0.;
        }

        LL = fabs(vx1*Rx + vy1*Ry);
        if( LL < 1.e-10) continue;
        Aellipsis1 = info[i][3]*aaa/LL;

        Bellipsis1 = info[i][3]/R;





        S[NAssociated] = atan2(POINTS1[j+1]-Oy, POINTS1[j]-Ox) ;  // atan2 returns radians in (-pi and +pi]
        if( S[NAssociated] < 0.) S[NAssociated] += 2.*PI;
        Z[NAssociated] = POINTS1[j+2];
        ZDrift[NAssociated] = Aellipsis1*Tiltdirection1[0];
        ZErrorafterTilt[NAssociated] = StrawDriftError*aaa*Tiltdirection1[0]/LL;
        SkewList[NAssociated][0] = iii;  // n. skew hit in skew hit numbering
        SkewList[NAssociated][1] = ii;  //  solution 0 or solution 1 were accepted
        NAssociated++;

   }    //  end of    for( ii=0; ii<2; ii++)

  }   //   for( iii=0; iii< NSkewhits; iii++)



  return NAssociated;



 }

//----------end of function PndSttTrackFinderReal::AssociateSkewHitsToXYTrack



















//----------begin of function PndSttTrackFinderReal::PndSttFitSZspace

     Short_t PndSttTrackFinderReal::PndSttFitSZspace(
                                                     UShort_t nHitsinTrack,
                                                     Double_t *S,
                                                     Double_t *Z,
                                                     Double_t *DriftRadius,
                                                     UShort_t  nParallelTrack,
                                                     Double_t rotationangle,
                                                     UShort_t NMAX,
                                                     Double_t *emme,
                                                     Double_t *qu
                                                            )
{







    //   definition of variables for the glpsol  solver
   //    ROWS (for read_rows  function)
   //
   UShort_t  NpointsInFit = nHitsinTrack-NMAX <0 ?  nHitsinTrack :  NMAX;
   int    nRows= NpointsInFit*9 +1;
   int typeRows[nRows];
   char * nameRows[nRows];
   char  auxnameRows[nRows][20];
//-------  end ROWS information
//--------begin COLUMNS information
      int  NStructVar=5+NpointsInFit*4;  //  number of  structural variables
      int  NStructRows = 8*NpointsInFit ;  //  maximum number of ROWS in which a structural variable can be found
      double final_values[NStructVar];
      int  NRowsInWhichStructVarArePresent[NStructVar];
      char *StructVarName[NStructVar];
      char auxStructVarName[NStructVar][20];
      char *NameRowsInWhichStructVarArePresent[NStructVar*NStructRows];
      char aux[NStructVar*NStructRows][20];
//      double Coefficients[NStructVar][NStructRows];
      double Coefficients[NStructVar*NStructRows];
//--------end COLUMNS information
//--------begin RHS information
      double ValueB[9*NpointsInFit];
//--------end RHS information
//--------begin RANGES information
      int nRanges = NpointsInFit;
      double ValueRanges[nRanges];
      char *NameRanges[nRanges];
      char auxNameRanges[nRanges][20];
//--------end RANGES information
//--------start BOUNDS information
      int nBounds=2*NpointsInFit+1;
      double BoundValue[nBounds];
      char *BoundStructVarName[nBounds];
      char auxBoundStructVarName[nBounds][20];
      char *TypeofBound[nBounds];
//--------end BOUNDS information


//----------------------------------------------------







     Double_t M = 50.,
              m_result,
              q_result,
              A,
              alfetta,
              angle,
              offsety,
              Ox[nmaxHits],
              Oy[nmaxHits],
              Delta[nmaxHits];

     UShort_t  i, ii;
     Short_t Status;

     char nome[100], stringa[100], stringa2[100];

//     FILE * MACRO ;

     float m1_result,m2_result, q1_result,q2_result, A1_result, A2_result;

// --

     if( nHitsinTrack < MINIMUMHITSPERTRACK) {
        return -1;
     }


//  use the trick of increasing the rotation angle by 10 degrees in order to obtain always a positive m
//      rotationangle -= PI/18.;

      Double_t cose = cos(rotationangle), sine = sin(rotationangle);
      for(i=0;i<nHitsinTrack; i++){
       Ox[i] = Z[ i ] *cose +
               S[ i ]*sine;
       Oy[i] = -Z[ i ] *sine +
               S[ i ]*cose;






         Delta[i] = 2.;

      }

//-----------------  write the ROWS  section



//--------
      nameRows[0]="OBJECT";
      typeRows[0]=GLP_FR;
      for(i=0 ; i< NpointsInFit ; i++) {
       ii=9*i;
       typeRows[1+ii]=GLP_UP;typeRows[2+ii]=GLP_UP;typeRows[3+ii]=GLP_UP;typeRows[4+ii]=GLP_UP;
       typeRows[5+ii]=GLP_UP;typeRows[6+ii]=GLP_UP;typeRows[7+ii]=GLP_UP;typeRows[8+ii]=GLP_UP;
       typeRows[9+ii]=GLP_LO;

       sprintf(&(auxnameRows[1+ii][0]),"Ap%d",i);  nameRows[1+ii]=&auxnameRows[1+ii][0];
       sprintf(&(auxnameRows[2+ii][0]),"Bp%d",i);  nameRows[2+ii]=&auxnameRows[2+ii][0];
       sprintf(&(auxnameRows[3+ii][0]),"Cp%d",i);  nameRows[3+ii]=&auxnameRows[3+ii][0];
       sprintf(&(auxnameRows[4+ii][0]),"Dp%d",i);  nameRows[4+ii]=&auxnameRows[4+ii][0];
       sprintf(&(auxnameRows[5+ii][0]),"Am%d",i);  nameRows[5+ii]=&auxnameRows[5+ii][0];
       sprintf(&(auxnameRows[6+ii][0]),"Bm%d",i);  nameRows[6+ii]=&auxnameRows[6+ii][0];
       sprintf(&(auxnameRows[7+ii][0]),"Cm%d",i);  nameRows[7+ii]=&auxnameRows[7+ii][0];
       sprintf(&(auxnameRows[8+ii][0]),"Dm%d",i);  nameRows[8+ii]=&auxnameRows[8+ii][0];
       sprintf(&(auxnameRows[9+ii][0]),"LAMBDA%d",i);  nameRows[9+ii]=&auxnameRows[9+ii][0];
      }





//-----------------  write the COLUMNS  section







//  Column variable  m1
      for(i=0, ii=0 ; i< NpointsInFit ; i++) {
       ii++;
        Coefficients[i*4]=    Ox[i];
        Coefficients[i*4+1]=  Ox[i];
        Coefficients[i*4+2]= -Ox[i];
        Coefficients[i*4+3]= -Ox[i];
      }

//  Column variable  m2
      for(i=0; i< NpointsInFit ; i++) {
        Coefficients[NStructRows+i*4]=   -Ox[i];
        Coefficients[NStructRows+i*4+1]= -Ox[i];
        Coefficients[NStructRows+i*4+2]= Ox[i];
        Coefficients[NStructRows+i*4+3]= Ox[i];

      }

//  Column variable  q1
      for(i=0 ; i< NpointsInFit ; i++) {
        Coefficients[2*NStructRows+i*4]=    1.;
        Coefficients[2*NStructRows+i*4+1]=  1.;
        Coefficients[2*NStructRows+i*4+2]= -1.;
        Coefficients[2*NStructRows+i*4+3]= -1.;
      }

//  Column variable  q2
      for(i=0 ; i< NpointsInFit ; i++) {
        Coefficients[3*NStructRows+i*4]=   -1.;
        Coefficients[3*NStructRows+i*4+1]= -1.;
        Coefficients[3*NStructRows+i*4+2]=  1.;
        Coefficients[3*NStructRows+i*4+3]=  1.;
      }

//  Column variable  lambdap(i)
      for(i=0 ; i< NpointsInFit ; i++) {
        Coefficients[(4+i)*NStructRows+0]= -M;
        Coefficients[(4+i)*NStructRows+1]= -M;
        Coefficients[(4+i)*NStructRows+2]= -M;
        Coefficients[(4+i)*NStructRows+3]=  M;
        Coefficients[(4+i)*NStructRows+4]=  1.;
      }
//  Column variable  lambdam(i)
      for(i=0 ; i< NpointsInFit ; i++) {
        Coefficients[(4+i+NpointsInFit)*NStructRows+0]= -M;
        Coefficients[(4+i+NpointsInFit)*NStructRows+1]= -M;
        Coefficients[(4+i+NpointsInFit)*NStructRows+2]= -M;
        Coefficients[(4+i+NpointsInFit)*NStructRows+3]=  M;
        Coefficients[(4+i+NpointsInFit)*NStructRows+4]=  1.;
      }
//  Column variable  sigmap(i)
      for(i=0; i< NpointsInFit ; i++) {

        Coefficients[(4+i+2*NpointsInFit)*NStructRows+0]=  1./Delta[i];
        Coefficients[(4+i+2*NpointsInFit)*NStructRows+1]= -1.;
        Coefficients[(4+i+2*NpointsInFit)*NStructRows+2]= -1.;
        Coefficients[(4+i+2*NpointsInFit)*NStructRows+3]=  1.;
        Coefficients[(4+i+2*NpointsInFit)*NStructRows+4]= -1.;
      }
//  Column variable  sigmam(i)
      for(i=0 ; i< NpointsInFit ; i++) {
        Coefficients[(4+i+3*NpointsInFit)*NStructRows+0]=  1./Delta[i];
        Coefficients[(4+i+3*NpointsInFit)*NStructRows+1]= -1.;
        Coefficients[(4+i+3*NpointsInFit)*NStructRows+2]= -1.;
        Coefficients[(4+i+3*NpointsInFit)*NStructRows+3]=  1.;
        Coefficients[(4+i+3*NpointsInFit)*NStructRows+4]= -1.;
      }

//  Column variable  DUMMY
      for(i=0 ; i< NStructRows ; i++) {
        Coefficients[(4+4*NpointsInFit)*NStructRows+i]= 1.;
      }
//--------------------
      StructVarName[0]="m1";
      NRowsInWhichStructVarArePresent[0]= 4*NpointsInFit;
      StructVarName[1]="m2";
      NRowsInWhichStructVarArePresent[1]= 4*NpointsInFit;
      StructVarName[2]="q1";
      NRowsInWhichStructVarArePresent[2]= 4*NpointsInFit;
      StructVarName[3]="q2";
      NRowsInWhichStructVarArePresent[3]= 4*NpointsInFit;
      for(i=0; i< NpointsInFit ; i++) {
          sprintf(&auxStructVarName[3+i+1][0],"lamp%d",i);
          StructVarName[4+i] = &auxStructVarName[4+i][0];
          NRowsInWhichStructVarArePresent[4+i]= 5;

          sprintf(&auxStructVarName[4+NpointsInFit+i][0],"lamm%d",i);
          StructVarName[4+NpointsInFit+i] = &auxStructVarName[4+NpointsInFit+i][0];
          NRowsInWhichStructVarArePresent[4+NpointsInFit+i]= 5;

          sprintf(&auxStructVarName[4+2*NpointsInFit+i][0],"sigmap%d",i);
          StructVarName[4+2*NpointsInFit+i] = &auxStructVarName[4+2*NpointsInFit+i][0];
          NRowsInWhichStructVarArePresent[4+2*NpointsInFit+i]= 5;

          sprintf(&auxStructVarName[4+3*NpointsInFit+i][0],"sigmam%d",i);
          StructVarName[4+3*NpointsInFit+i] = &auxStructVarName[4+3*NpointsInFit+i][0];
          NRowsInWhichStructVarArePresent[4+3*NpointsInFit+i]= 5;

      }
      StructVarName[4+4*NpointsInFit]="DUMMY";
      NRowsInWhichStructVarArePresent[4+4*NpointsInFit]= NStructRows;


//  for m1, m2, q1, q2
      for(i=0; i< 4; i++){
        for(ii=0; ii< NpointsInFit;ii++){
         sprintf(&aux[i*NStructRows+ii*4][0],"Ap%d",ii);
         NameRowsInWhichStructVarArePresent[i*NStructRows+ii*4]=&aux[i*NStructRows+ii*4][0];
         sprintf(&aux[i*NStructRows+ii*4+1][0],"Am%d",ii);
         NameRowsInWhichStructVarArePresent[i*NStructRows+ii*4+1]=&aux[i*NStructRows+ii*4+1][0];
         sprintf(&aux[i*NStructRows+ii*4+2][0],"Bp%d",ii);
         NameRowsInWhichStructVarArePresent[i*NStructRows+ii*4+2]=&aux[i*NStructRows+ii*4+2][0];
         sprintf(&aux[i*NStructRows+ii*4+3][0],"Bm%d",ii);
         NameRowsInWhichStructVarArePresent[i*NStructRows+ii*4+3]=&aux[i*NStructRows+ii*4+3][0];
        }
      }

//  now for the    lamp*   variables
      for(i=0; i< NpointsInFit;i++){
         sprintf(&aux[(i+4)*NStructRows+0][0],"Ap%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRows+0]= &aux[(i+4)*NStructRows+0][0];
         sprintf(&aux[(i+4)*NStructRows+1][0],"Bp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRows+1]= &aux[(i+4)*NStructRows+1][0];
         sprintf(&aux[(i+4)*NStructRows+2][0],"Cp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRows+2]= &aux[(i+4)*NStructRows+2][0];
         sprintf(&aux[(i+4)*NStructRows+3][0],"Dp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRows+3]= &aux[(i+4)*NStructRows+3][0];
         sprintf(&aux[(i+4)*NStructRows+4][0],"LAMBDA%d",i);
         NameRowsInWhichStructVarArePresent[(i+4)*NStructRows+4]= &aux[(i+4)*NStructRows+4][0];
      }

//  now for the    lamm*   variables
      for(i=0; i< NpointsInFit;i++){
         sprintf(&aux[(i+4+NpointsInFit)*NStructRows+0][0],"Am%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit)*NStructRows+0]= &aux[(i+4+NpointsInFit)*NStructRows+0][0];
         sprintf(&aux[(i+4+NpointsInFit)*NStructRows+1][0],"Bm%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit)*NStructRows+1]= &aux[(i+4+NpointsInFit)*NStructRows+1][0];
         sprintf(&aux[(i+4+NpointsInFit)*NStructRows+2][0],"Cm%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit)*NStructRows+2]= &aux[(i+4+NpointsInFit)*NStructRows+2][0];
         sprintf(&aux[(i+4+NpointsInFit)*NStructRows+3][0],"Dm%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit)*NStructRows+3]= &aux[(i+4+NpointsInFit)*NStructRows+3][0];
         sprintf(&aux[(i+4+NpointsInFit)*NStructRows+4][0],"LAMBDA%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+NpointsInFit)*NStructRows+4]= &aux[(i+4+NpointsInFit)*NStructRows+4][0];
      }

//  now for the    sigmap*   variables
      for(i=0; i< NpointsInFit;i++){
         sprintf(&aux[(i+4+2*NpointsInFit)*NStructRows+0][0],"OBJECT",i);
         NameRowsInWhichStructVarArePresent[(i+4+2*NpointsInFit)*NStructRows+0]= &aux[(i+4+2*NpointsInFit)*NStructRows+0][0];
         sprintf(&aux[(i+4+2*NpointsInFit)*NStructRows+1][0],"Ap%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+2*NpointsInFit)*NStructRows+1]= &aux[(i+4+2*NpointsInFit)*NStructRows+1][0];
         sprintf(&aux[(i+4+2*NpointsInFit)*NStructRows+2][0],"Bp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+2*NpointsInFit)*NStructRows+2]= &aux[(i+4+2*NpointsInFit)*NStructRows+2][0];
         sprintf(&aux[(i+4+2*NpointsInFit)*NStructRows+3][0],"Cp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+2*NpointsInFit)*NStructRows+3]= &aux[(i+4+2*NpointsInFit)*NStructRows+3][0];
         sprintf(&aux[(i+4+2*NpointsInFit)*NStructRows+4][0],"Dp%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+2*NpointsInFit)*NStructRows+4]= &aux[(i+4+2*NpointsInFit)*NStructRows+4][0];
      }

//  now for the    sigmam*   variables
      for(i=0; i< NpointsInFit;i++){
         sprintf(&aux[(i+4+3*NpointsInFit)*NStructRows+0][0],"OBJECT",i);
         NameRowsInWhichStructVarArePresent[(i+4+3*NpointsInFit)*NStructRows+0]= &aux[(i+4+3*NpointsInFit)*NStructRows+0][0];
         sprintf(&aux[(i+4+3*NpointsInFit)*NStructRows+1][0],"Am%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+3*NpointsInFit)*NStructRows+1]= &aux[(i+4+3*NpointsInFit)*NStructRows+1][0];
         sprintf(&aux[(i+4+3*NpointsInFit)*NStructRows+2][0],"Bm%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+3*NpointsInFit)*NStructRows+2]= &aux[(i+4+3*NpointsInFit)*NStructRows+2][0];
         sprintf(&aux[(i+4+3*NpointsInFit)*NStructRows+3][0],"Cm%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+3*NpointsInFit)*NStructRows+3]= &aux[(i+4+3*NpointsInFit)*NStructRows+3][0];
         sprintf(&aux[(i+4+3*NpointsInFit)*NStructRows+4][0],"Dm%d",i);
         NameRowsInWhichStructVarArePresent[(i+4+3*NpointsInFit)*NStructRows+4]= &aux[(i+4+3*NpointsInFit)*NStructRows+4][0];
      }

//  now for the    DUMMY   variable
      for(i=0; i< NpointsInFit;i++){
         sprintf(&aux[(4+4*NpointsInFit)*NStructRows  +8*i][0],"Ap%d",i);
         NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+i*8  ]= &aux[(4+4*NpointsInFit)*NStructRows  +8*i][0];

         sprintf(&aux[(4+4*NpointsInFit)*NStructRows+1+8*i][0],"Am%d",i);
         NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+1+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+1+8*i][0];
         sprintf(&aux[(4+4*NpointsInFit)*NStructRows+2+8*i][0],"Bp%d",i);
         NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+2+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+2+8*i][0];
         sprintf(&aux[(4+4*NpointsInFit)*NStructRows+3+8*i][0],"Bm%d",i);
         NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+3+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+3+8*i][0];
         sprintf(&aux[(4+4*NpointsInFit)*NStructRows+4+8*i][0],"Cp%d",i);
         NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+4+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+4+8*i][0];
         sprintf(&aux[(4+4*NpointsInFit)*NStructRows+5+8*i][0],"Cm%d",i);
         NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+5+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+5+8*i][0];
         sprintf(&aux[(4+4*NpointsInFit)*NStructRows+6+8*i][0],"Dp%d",i);
         NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+6+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+6+8*i][0];
         sprintf(&aux[(4+4*NpointsInFit)*NStructRows+7+8*i][0],"Dm%d",i);
         NameRowsInWhichStructVarArePresent[(4+4*NpointsInFit)*NStructRows+7+8*i]= &aux[(4+4*NpointsInFit)*NStructRows+7+8*i][0];
      }




//-----------------  write the RHS  section


      for(i=0 ; i< NpointsInFit ; i++) {
          ValueB[i*9]  =  Oy[i]+DriftRadius[ i ]+2.*M;
          ValueB[i*9+1]= -Oy[i]-DriftRadius[ i ]+2.*M;
          ValueB[i*9+2]= Delta[i]+2.*M;
          ValueB[i*9+3]= M-Delta[i]+2.*M;
          ValueB[i*9+4]=  Oy[i]-DriftRadius[ i ]+2.*M;
          ValueB[i*9+5]= -Oy[i]+DriftRadius[ i ]+2.*M;
          ValueB[i*9+6]= Delta[i]+2.*M;
          ValueB[i*9+7]= M-Delta[i]+2.*M;
          ValueB[i*9+8]= 1.;


      }




//-----------------  write the RANGES  section

      for(i=0 ; i< NpointsInFit ; i++) {
        ValueRanges[i]=1.;
        sprintf(&auxNameRanges[i][0],"LAMBDA%d",i);
        NameRanges[i]=&auxNameRanges[i][0];
      }


//-----------------  write the BOUNDS  section


      for(i=0 ; i< NpointsInFit ; i++) {
          TypeofBound[i]="BV";
          sprintf(&auxBoundStructVarName[i][0],"lamp%d",i);
          BoundStructVarName[i]=&auxBoundStructVarName[i][0];
          BoundValue[i]=0.;
      }

      for(i=0 ; i< NpointsInFit ; i++) {
          TypeofBound[i+NpointsInFit]="BV";
          sprintf(&auxBoundStructVarName[i+NpointsInFit][0],"lamm%d",i);
          BoundStructVarName[i+NpointsInFit]=&auxBoundStructVarName[i+NpointsInFit][0];
          BoundValue[i+NpointsInFit]=0.;
      }

          TypeofBound[2*NpointsInFit]="FX";
          BoundStructVarName[2*NpointsInFit]="DUMMY";
          BoundValue[2*NpointsInFit]=2.*M;




//-----






//----------------------  calling the minimizer


      int status= glp_main(
            nRows,nameRows,typeRows, //  ROWS info
            NStructVar, NStructRows, NRowsInWhichStructVarArePresent,  //  COLUMNS info
      StructVarName, NameRowsInWhichStructVarArePresent,  //  COLUMNS info
      Coefficients,  //  COLUMNS info
      ValueB,  // RHS  info
      nRanges, ValueRanges, NameRanges, //  RANGES  info
      nBounds, BoundValue, BoundStructVarName, TypeofBound //  BOUNDS info
      ,final_values
       );



//------------------------------------------

     m1_result=final_values[0];
     m2_result=final_values[1];
     q1_result=final_values[2];
     q2_result=final_values[3];


//------------------------  transformation of the result in terms of ALFA, BETA, GAMMA


     *qu = q1_result - q2_result;
     *emme = m1_result-m2_result ;



// taking into account the rotation that was performed and calculate emme and qu in the normal conformal plane

      if(fabs(cose-*emme*sine)> 1.e-10) {
        *qu=*qu/(cose-*emme*sine);
        *emme=(*emme*cose+sine)/(cose-*emme*sine);
        return 1;
      } else {    //  in this case the equation is   0 = x+*qu .
        if(fabs(sine+*emme*cose) < 1.e-10)  {
  cout<<" From PndSttFitSZspace, situation impossible in principle! Returning -1"
                    <<endl;
           return -1;
        }

        *emme=1.;
        *qu = *qu/(sine+*emme*cose);
        return 99;    //  in this case the equation is   0 = x+*qu .
      }





}




//----------end of function PndSttTrackFinderReal::PndSttFitSZspace




//----------begin of function PndSttTrackFinderReal::PndSttFitwithKalman

      void   PndSttTrackFinderReal::PndSttFitwithKalman(
                                                     Double_t oX,
                                                     Double_t oY,
                                                     Double_t Pxini,
                                                     Double_t Pyini,
                                                     Double_t Pzini,
                                                     Double_t Ptras,
                                                     Double_t info[][6],
                                                     UShort_t nParallelHits,
                                                     UShort_t *ListParallelHits,
                                                     UShort_t nSkewHits,
                                                     UShort_t *ListSkewHits,
                                                     Double_t *S,
                                                     UShort_t *Infoparal,
                                                     UShort_t *Infoskew
                                                       )
{


      UShort_t i,j,flag,
               nTotal = nParallelHits+nSkewHits,
               BigList[nTotal];
      Double_t old,
               auxFivalues[nmaxHits],
               auxFiSkewvalues[nmaxHits],
               auxRvalues[nmaxHits],
               BigListFi[nTotal];



//  here there is the ordering of the hits

//   ordering of the parallel hits first

    for (j = 0; j< nParallelHits; j++){
      auxRvalues[j]=
                    info[ infoparal[ ListParallelHits[j] ]  ][0]*
                    info[ infoparal[ ListParallelHits[j] ]  ][0]+
                    info[ infoparal[ ListParallelHits[j] ]  ][1]*
                    info[ infoparal[ ListParallelHits[j] ]  ][1];
    }

    PndStt_Merge_Sort( nParallelHits, auxRvalues, ListParallelHits);

    for (j = 0; j< nParallelHits; j++){              
      auxFivalues[j] = atan2( info[ infoparal[ ListParallelHits[j] ]  ][1]-oY,
                              info[ infoparal[ ListParallelHits[j] ]  ][0]-oX);
      if( auxFivalues[j] < 0. ) auxFivalues[j] += 2.*PI;
    }

//  fixing possible discontinuity between fi<2*PI and fi>0.
   for (old=auxFivalues[0],flag=0, j = 1; j< nParallelHits; j++){
      if( fabs(old - auxFivalues[j]) > PI ) {
        flag=1;
        break;
      } else {
        old=auxFivalues[j];
      }
   }
   if( flag==1) {
      for (j = 0; j< nParallelHits; j++){
        if( auxFivalues[j] < PI) auxFivalues[j] += 2.*PI;
      }
   }
//     now ordering of the skew hits

   if( flag==1) {
      for (j = 0; j< nSkewHits; j++){
       if( S[j] < PI) {
            auxFiSkewvalues[j] = S[j] + 2.*PI;
       } else {
            auxFiSkewvalues[j] = S[j];
       }
      }
   } else {
      for (j = 0; j< nSkewHits; j++){
       auxFiSkewvalues[j] = S[j];
      }
   }
 
    PndStt_Merge_Sort( nSkewHits, auxFiSkewvalues, ListSkewHits);
//    merge the parallel and skew hits
     for(j = 0;j< nParallelHits; j++){
       BigListFi[j]=auxFivalues[j];
       BigList [j] = Infoparal[  ListParallelHits[i]  ] ;        
     }
      for(j=0; j<nSkewHits; j++){
       BigListFi[j]=auxFiSkewvalues[j+nParallelHits];
       BigList [i+nParallelHits] = Infoskew[  ListSkewHits[i]  ] ;
      }

    PndStt_Merge_Sort(nTotal, BigListFi, BigList);

//  now decide if track ran clockwise or anticlockwise


    if( auxFivalues[0] > auxFivalues[nParallelHits]) {
      for(j=0; j<nTotal; j++){
       auxFivalues[j]=BigListFi[nTotal-1-j];
      }
      for(j=0; j<nTotal; j++){
       BigListFi[j]=auxFivalues[j];
      }
    }



//---------   end ordering




      PndSttTrack*      track = NULL;

      // STARTING VERTEX ERRORS
//      TVector3 StartPos    = *recovtx;
      TVector3 StartPos    = TVector3(0.,0.,0.);
      Double_t pterr, plerr;
      Double_t errxp, erryp, errzp;
      errxp = 10 * 0.02;                      // 200 micron * 10
      erryp = errxp;
      errzp = 10 * 0.15;                      // 1.5 mm     * 10
      TVector3 StartPosErr = TVector3(errxp, erryp, errzp);
      TVector3 StartMom    = TVector3(Pxini,Pyini,Pzini);
//      TVector3 mcStartMom = mctrack->GetMomentum();
      Double_t momerr;                        // = 4% of mc mom 
//      momerr = 0.04 *  mcStartMom.Mag();
      momerr = 0.04 *  Ptras;
      pterr = momerr;
      plerr = momerr;
      TVector3 StartMomErr = TVector3(pterr, pterr, plerr); 

//      Int_t pdg = mctrack->GetPdgCode();
      Int_t pdg = 13;

      TDatabasePDG *fdbPDG= TDatabasePDG::Instance();
      TParticlePDG *fParticle= fdbPDG->GetParticle(pdg);
      Double_t  fCharge= fParticle->Charge()/3.;
      
      TVector3 u(0.,1.,0.);
      TVector3 v(0.,0.,1.);
      GFDetPlane pl(StartPos,u,v);

      GFAbsTrackRep* rep = 0;
      GeaneTrackRep *grep = new GeaneTrackRep(fPro,pl,StartMom,StartPosErr,StartMomErr,fCharge,pdg);
      grep->setPropDir(1); // propagate in flight direction!
      rep=grep;

      GFTrack* trk = new GFTrack(rep);
      GFTrackCand *cand = new GFTrackCand();
      int detId;
      for(int iPoint = 0; iPoint < nTotal; iPoint++)
 	{

          PndSttHit * currenthit = (PndSttHit*) GetHitFromCollections(BigList[iPoint]);
	  if(!currenthit) continue;
	  

  
	  detId = currenthit->GetDetectorID() ;

	  cand->addHit(detId, BigList[iPoint]);

      	}   //  end of       for(int iPoint = 0; iPoint < nTotal; iPoint++)



       trk->setCandidate(*cand); // here the candidate is copied! 

//----  now the Kalman
	trk->addHitVector(_theRecoHitFactory->createMany(trk->getCand()));
	GFKalman k;
        k.setLazy(1);
	k.setNumIterations(1);
	k.processTrack(trk);
//------ estrazione delle info dal Kalman secondo Lia
    TVector3 dum = trk->getCardinalRep()->getMom();
//------------
 
    delete grep;
    delete trk;
    delete cand;

 return; 

}
//----------end of function PndSttTrackFinderReal::PndSttFitwithKalman








//----------begin of function PndSttTrackFinderReal::PndSttOrdering

      void   PndSttTrackFinderReal::PndSttOrdering(
                                                     Double_t oX,
                                                     Double_t oY,
                                                     Double_t info[][6],
                                                     UShort_t nParallelHits,
                                                     UShort_t *ListParallelHits,
                                                     UShort_t nSkewHits,
                                                     UShort_t *ListSkewHits,
                                                     Double_t *S,
                                                     UShort_t *Infoparal,
                                                     UShort_t *Infoskew,
                                                     UShort_t *nTotal,
                                                     UShort_t *BigList,
                                                     Short_t  * Charge
                                                       )
{

      *nTotal = nParallelHits+nSkewHits;

      UShort_t i,j,flag,
               aux[*nTotal];
      Double_t old,
               auxFivalues[nmaxHits],
               auxFiSkewvalues[nmaxHits],
               auxRvalues[nmaxHits],
               BigListFi[*nTotal];



//  here there is the ordering of the hits

//   ordering of the parallel hits first

    for (j = 0; j< nParallelHits; j++){
      auxRvalues[j]=
                    info[ infoparal[ ListParallelHits[j] ]  ][0]*
                    info[ infoparal[ ListParallelHits[j] ]  ][0]+
                    info[ infoparal[ ListParallelHits[j] ]  ][1]*
                    info[ infoparal[ ListParallelHits[j] ]  ][1];
    }

    PndStt_Merge_Sort( nParallelHits, auxRvalues, ListParallelHits);

    for (j = 0; j< nParallelHits; j++){              
      auxFivalues[j] = atan2( info[ infoparal[ ListParallelHits[j] ]  ][1]-oY,
                              info[ infoparal[ ListParallelHits[j] ]  ][0]-oX);
      if( auxFivalues[j] < 0. ) auxFivalues[j] += 2.*PI;
    }

//  fixing possible discontinuity between fi<2*PI and fi>0.
   for (old=auxFivalues[0],flag=0, j = 1; j< nParallelHits; j++){
      if( fabs(old - auxFivalues[j]) > PI ) {
        flag=1;
        break;
      } else {
        old=auxFivalues[j];
      }
   }
   if( flag==1) {
      for (j = 0; j< nParallelHits; j++){
        if( auxFivalues[j] < PI) auxFivalues[j] += 2.*PI;
      }
   }

//   finding the charge of the track

    if( auxFivalues[0] > auxFivalues[nParallelHits-1]) {
      *Charge =  1;
    }  else {
     *Charge =  -1;
    }




//     now ordering of the skew hits

   if( flag==1) {
      for (j = 0; j< nSkewHits; j++){
       if( S[j] < PI) {
            auxFiSkewvalues[j] = S[j] + 2.*PI;
       } else {
            auxFiSkewvalues[j] = S[j];
       }
      }
   } else {
      for (j = 0; j< nSkewHits; j++){
       auxFiSkewvalues[j] = S[j];
      }
   }
 
    PndStt_Merge_Sort( nSkewHits, auxFiSkewvalues, ListSkewHits);

//    merge the parallel and skew hits
     for(j = 0;j< nParallelHits; j++){
       BigListFi[j]=auxFivalues[j];
       BigList[j] = Infoparal[  ListParallelHits[j]  ] ;        
// cout<<"from Ordering, Parallel; j= "<<j<<", n. Hit // in original numbering = "<<BigList[j]<<endl;
     }
      for(j=0; j<nSkewHits; j++){
       BigListFi[j+nParallelHits]=auxFiSkewvalues[j];
       BigList [j+nParallelHits] = Infoskew[  ListSkewHits[j]  ] ;
// cout<<"from Ordering, Skew; j= "<<j<<", n. Hit skew in original numbering = "<<BigList[j+nParallelHits]<<endl;
      }

    PndStt_Merge_Sort(*nTotal, BigListFi, BigList);






// for POSITIVE charged tracks, the ordering based on the Fi angle must be reversed because hits at smaller
// distance from center have geatest Fi

     if(*Charge ==1) {
      for(j=0; j<*nTotal; j++){
       auxFivalues[j]=BigListFi[(*nTotal)-1-j];
       aux[j]=BigList[(*nTotal)-1-j];
      }
      for(j=0; j<*nTotal; j++){
       BigListFi[j]=auxFivalues[j];
       BigList[j]=aux[j];
      }
     }


/*
    cout<<"Printout di prova, dopo ordinamento globale, carica traccia = "<<*Charge<<endl;
    for(j=0; j<*nTotal; j++){
cout<<"    j= "<<j<<", n. Hit in original numbering = "<<BigList[j]<<" e suo FI "<<BigListFi[j]<<endl;
    }
*/

//--------------






//---------   end ordering



 return; 

}
//----------end of function PndSttTrackFinderReal::PndSttOrdering









ClassImp(PndSttTrackFinderReal)

    

