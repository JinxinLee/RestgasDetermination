// -------------------------------------------------------------------------
// -----                PndSttTrackFinderReal source file             -----
// -----                  Created 28/03/06  by V. Friese               -----
// -------------------------------------------------------------------------

// Pnd includes
#include "PndSttTrackFinderReal.h"

#include "PndSttHit.h"
#include "PndSttPoint.h"
#include "PndSttTrack.h"
#include  <cmath>
#include "FairMCPoint.h"
#include "FairRootManager.h"

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

  int IVOLTE=0, ntimes = 0;



//  Double_t SEMILENGTH_STRAIGHT = 75.;
  Double_t SEMILENGTH_STRAIGHT ;
  Double_t ZCENTER_STRAIGHT = 75.;

  const bool  iplotta = true , ianalizza = true ;
  const int istampa = 1 ;

  TH1F * hx;

   FILE * HANDLE ;


  Double_t veritaMC[nmaxHits][3];

//    UShort_t  BoxCXCYR[nbinCX][nbinCY][nbinR];
    UShort_t  BoxDFiR[nbinD][nbinFi][nbinR];
    UShort_t  BoxKFI0[nbinKAPPA][nbinFI0];



// -----   Default constructor   -------------------------------------------
PndSttTrackFinderReal::PndSttTrackFinderReal() 
{ 
  fVerbose      = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndSttTrackFinderReal::PndSttTrackFinderReal(Int_t verbose) 
{ 
  fVerbose      = verbose;
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
  // Get and check FairRootManager
  FairRootManager* ioman = FairRootManager::Instance();
  hx = new TH1F("hx", "Associated Z", 400, -200., 200.);

  if (!ioman) 
    {
      cout << "-E- PndSttTrackFinderReal::Init: "
	   << "RootManager not instantised!" << endl;
      return;
    }
 
}
// -------------------------------------------------------------------------


// -----   Public method DoFind   ------------------------------------------
Int_t PndSttTrackFinderReal::DoFind(TClonesArray* trackArray) 
{
    Double_t info[nmaxHits][6],
             WDX, WDY, WDZ,
             inclinationversors[nmaxinclinationversors][3];
    inclinationversors[0][0]=inclinationversors[0][1]=0.,    inclinationversors[0][2]=1.;
    Int_t Ninclinations = 1;
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


//------------------------------------ modifiche Gianluigi, 9-7-08
     IVOLTE++;
//   cout<<"\nGianluigi da DoFind IVOLTE "<<IVOLTE<<endl;
//------------------------------------ fine modifiche Gianluigi, 9-7-08


    
  // Initialise control counters
  Int_t nNoTrack     = 0;
  Int_t nNoSttPoint  = 0;
  Int_t nNoSttHit    = 0;

  // Create pointers to hit and SttPoint
  PndSttHit*       pMhit = NULL;
  FairMCPoint*      pMCpt = NULL;
  PndSttTrack*     pTrck = NULL;

  // Number of STT hits
  Int_t nHits = 0;
  
  for (Int_t hitListCounter = 0; hitListCounter < fHitCollectionList.GetEntries(); hitListCounter++)
  {
      nHits += ((TClonesArray *)fHitCollectionList.At(hitListCounter))->GetEntriesFast();
  }

  // Declare some variables outside the loops
  Int_t trackIndex   = 0;     // STTTrack index

  // Create STL map from MCtrack index to number of valid SttHits
  map<Int_t, map<Double_t, Int_t> >
    hitMap;



    for(Int_t j=0; j<nmaxinclinationversors;j++){
     Minclinations[j]=0;
    }

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

      veritaMC[iHit][0]= ((PndSttPoint*)pMCpt)->GetXtot();
      veritaMC[iHit][1]= ((PndSttPoint*)pMCpt)->GetYtot();
      veritaMC[iHit][2]= ((PndSttPoint*)pMCpt)->GetZtot();

      info[iHit][0]= pMhit->GetX();
      info[iHit][1]= pMhit->GetY();
      info[iHit][2]= pMhit->GetZ();
      info[iHit][3]= dradius;
      info[iHit][4]=pMhit->GetTubeHalfLength();

      if( fabs( WDX )< 0.00001 && fabs( WDY )< 0.00001 ){
        info[iHit][5]= 1.;
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
          Minclinations[i-1]++;
          goto jumpout;
        }
       }
       Ninclinations++;
       inclinationversors[Ninclinations-1][0]=(Double_t) WDX;
       inclinationversors[Ninclinations-1][1]=(Double_t) WDY;
       inclinationversors[Ninclinations-1][2]=(Double_t) WDZ;
       info[iHit][5]= Ninclinations;
       Minclinations[Ninclinations-1]++;
jumpout: ;
      }        
    }   //   end  of  for (Int_t iHit = 0;





   if( nHits >0 &&  Minclinations[0] > 2 ) {
     if(ianalizza)   PndSttTrkFinderPartial(nHits,info,Ninclinations,Minclinations,inclinationversors);
   };


  // la parte seguente andra' messa a posto per 
  // dare in output la traccia PndSttTrack
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

void PndSttTrackFinderReal::PndSttTrkFinderPartial(Int_t Nhits, Double_t info[][6],
                                 Int_t Nincl,Int_t Mincl[],Double_t inclination[][3])
{




   Int_t  i, j,ii,jj,k,kk,n1,n2,n3,
          i1,j1,k1,imaxima, jmaxima,
          iofmax, jofmax, kofmax,
          index[nmaxHits], integer,indexskew[nmaxHits],
          Ncirc, nSkew1, nSkew2, STATUS,
          n_K, n_FI0, n_R, n_D, n_Fi,
          Nstraight, Nslanted,
          Nremaining, Nremaining2,
          NumberofMaximaDFiR,
          NumberofMaximaKFI0,
          MaximaIndexesDFiR[MAXElementsOverThresholdinHough][3],
          MaximaIndexesKFI0[MAXElementsOverThresholdinHough][2];
   Int_t  nAssociatedParallelHits ;

   int   iimax,jjmax,  ncount;


   Double_t angle, max1, HoughR, HoughD, HoughFi, HoughKAPPA, HoughFI0,
              tempR, tempD,tempFi,tempKAPPA,tempFI0,
              Rlow, Rup, Dlow, Dup,Filow,Fiup, KAPPAlow, KAPPAup, FI0low, FI0up;


    Float_t RemainingR[MAXSTTINFO],
            RemainingFi[MAXSTTINFO],
            RemainingD[MAXSTTINFO],
            RemainingCX[MAXSTTINFO],
            RemainingCY[MAXSTTINFO],
            RemainingKAPPA[MAXSTTINFO],
            RemainingFI0[MAXSTTINFO];


   Double_t    D,Fi, skewinclination[nmaxHits][3];
   CalculatedCircles Result;
   CalculatedHelix   HelixResult;
   AssociatedHitsToHelix ResultAssociatedHits ;

   char nomef[100], nome[30],titolo[100];





      for(j=0, Nstraight=0, Nslanted=0; j< Nhits ;j++){      // sort out all hits of wires parallel to Z from those not parallel
        if( info[j][5] == 1. ){
          index[Nstraight]=j;
          Nstraight++;
        } else {
          indexskew[Nslanted]=j;
          integer = (Int_t) info[j][5];
          skewinclination[Nslanted][0]=inclination[ integer-1 ] [0];
          skewinclination[Nslanted][1]=inclination[ integer-1 ] [1];
          skewinclination[Nslanted][2]=inclination[ integer-1 ] [2];
          Nslanted++;
        }
      }


//     start the track finding procedure



          for(i=0; i<nbinD;i++){
           for(j=0; j<nbinFi;j++){
            for(k=0; k<nbinR; k++){
              BoxDFiR[i][j][k]=0;
            }
           }
          }



    Nremaining=0;   // for the R, D, Fi  maximux search

    for (i=0;   i<Mincl[0]-2 ; i++){
     for (j=i+1; j<Mincl[0]-1  ; j++){
      for (k=j+1; k<Mincl[0] ;   k++){

        Result = PndSttTrkFindCircles(
                                       info[index[i]][0], info[index[i]][1], info[index[i]][3],
                                       info[index[j]][0], info[index[j]][1], info[index[j]][3],
                                       info[index[k]][0], info[index[k]][1], info[index[k]][3]
                                     );


//------------------------------------








  for(Int_t itemp = 0; itemp<Result.Ncircles; itemp++){
     if(Result.R[itemp]<RMINIMUM) {
        continue;
     }


       D = sqrt(Result.CX[itemp]*Result.CX[itemp] + Result.CY[itemp]*Result.CY[itemp]) - Result.R[itemp];
       Fi = atan2(Result.CY[itemp],Result.CX[itemp]);
       if(Fi<0.)  Fi+= 2.*PI;




       n1=(Int_t)((D-Dmin)/stepD);
       n2=(Int_t)((Fi-Fimin)/stepFi);
       n3=(Int_t)((Result.R[itemp]-Rmin)/stepR);
       if (
            n1>=0  &&  n1<nbinD
                  &&
            n2>=0  &&  n2<nbinFi
                  &&
            n3>=0  &&  n3<nbinR
          ) {
            BoxDFiR[n1][n2][n3]++;
            RemainingR[Nremaining]=Result.R[itemp];
            RemainingFi[Nremaining]= Fi;
            RemainingD[Nremaining]= D;
            RemainingCX[Nremaining]= Result.CX[itemp];
            RemainingCY[Nremaining]= Result.CY[itemp];
            Nremaining++;
            if ( Nremaining > MAXSTTINFO) {
               cout<<"in EVENT n. "<<IVOLTE<<" number of infos exceeded dimension of RemainingR, RemainingFi etc."
                   <<", stopping the accumulation but continuing the analysis of this parallel combination\n";
               goto stoppp ;  // number of infos cannot exceed dimension of RemainingR, RemainingFi etc.
            }
        }

  }   //  end of  for(Int_t itemp = 0;




      }    //   end of      for (i=0;   i<Mincl[0]-2;
     }     //   end of      for (j=i+1; j<Mincl[0]-1;
    }      //   end of      for (k=j+1; k<Mincl[0];


stoppp:  ;



//   cout<<"Parallels of Event "<<IVOLTE<<" give Hough plot\n";

    

//  searching cell of maximum of histogram of D,Fi,R --------------


//------ new method

    findmaximaDFiR( BoxDFiR, MINIMUMCOUNTSDFiR, &NumberofMaximaDFiR,   MaximaIndexesDFiR,
                      &STATUS );
    if(STATUS<=0) {
      return;
    }
//---------------------------- end of new method




  for(imaxima=0; imaxima<NumberofMaximaDFiR;imaxima++){



    tempD = Dmin+(MaximaIndexesDFiR[imaxima][0]+0.5)*stepD;
    tempFi = Fimin+(MaximaIndexesDFiR[imaxima][1]+0.5)*stepFi;
    tempR = Rmin+(MaximaIndexesDFiR[imaxima][2]+0.5)*stepR;

    Rlow= tempR-DELTA_R;
    Rup= tempR+DELTA_R;
    Filow = tempFi-DELTA_Fi;
    Fiup = tempFi+DELTA_Fi;
    Dlow = tempD-DELTA_D;
    Dup = tempD+DELTA_D;

    HoughR = HoughD= HoughFi=0.;
    ncount=0;
    for(i=0;   i<Nremaining; i++){
      if( RemainingR[i]>Rlow && RemainingR[i]<Rup
                             &&
          RemainingD[i]>Dlow && RemainingD[i]<Dup
                             &&
          RemainingFi[i]>Filow && RemainingFi[i]<Fiup
        )  {
          HoughR += RemainingR[i];
          HoughD += RemainingD[i];
          HoughFi += RemainingFi[i];
          ncount++;
      }
    }
    HoughR /= ncount;
    HoughD /= ncount;
    HoughFi /= ncount;


    nAssociatedParallelHits =
                   PndSttTrkAssociatedParallelHitsToHelix(HoughD,HoughFi,HoughR,Nhits,info,Nincl,Mincl,inclination);


    if(nAssociatedParallelHits<minParallelAssociatedHits) {
cout<<"n. del massimo "<<imaxima+1
  <<"-----------------non sopravvive perche' non ha sufficienti hits paralleli associati :   "<<nAssociatedParallelHits<<"\n";
      continue;
    }


//----------------------------------------------------------------------------------------------------------------

//  resetting the matrix for the Hough plot
     Nremaining2=0;  // for the KAPPA, FI0 maximum search
     for(ii=0; ii<nbinFI0; ii++){
        for(jj=0; jj<nbinKAPPA; jj++){
           BoxKFI0[jj][ii]=0;
        }
     }

// cout<<"stampa  "<<Mincl[1] <<", "<<Mincl[2]<<", "<<Nslanted<<endl;

//     for(nSkew1=0; nSkew1<Mincl[1]+Mincl[2]-1 ; nSkew1++){
//      for(nSkew2=nSkew1+1; nSkew2<Mincl[1]+Mincl[2] ; nSkew2++){
     for(nSkew1=0; nSkew1<Nslanted-1 ; nSkew1++){
      for(nSkew2=nSkew1+1; nSkew2 < Nslanted ; nSkew2++){
        HelixResult = PndSttTrkFindHelix((HoughR-HoughD)*cos(HoughFi),(HoughR-HoughD)*sin(HoughFi),HoughR,    // parameters of the Circle in XY
   ZCENTER_STRAIGHT,ZCENTER_STRAIGHT,ZCENTER_STRAIGHT,    // Z center of the straight straws forming the Circle in XY
   SEMILENGTH_STRAIGHT,SEMILENGTH_STRAIGHT,SEMILENGTH_STRAIGHT,    // semilength of the straight straws forming the Circle in XY
   info[ indexskew[nSkew1] ][0], info[ indexskew[nSkew1] ][1],  info[ indexskew[nSkew1] ][2], // parameter of the first skew straw : C0x1, C0y1, C0z1;
   info[ indexskew[nSkew1] ][4],  // parameter of the first skew straw : semilength;
   info[ indexskew[nSkew1] ][3],skewinclination[nSkew1][0],skewinclination[nSkew1][1],skewinclination[nSkew1][2],//drift radius1; vx1,vy1,vz1=direction of wire
   info[ indexskew[nSkew2] ][0], info[ indexskew[nSkew2] ][1],  info[ indexskew[nSkew2] ][2], // parameter of the second skew straw : C0x2, C0y2,C0z2;
   info[ indexskew[nSkew2] ][4],  // parameter of the first skew straw : semilength;
   info[ indexskew[nSkew2] ][3],skewinclination[nSkew2][0],skewinclination[nSkew2][1],skewinclination[nSkew2][2],//drift radius2; vx2,vy2,vz2=direction of wire
   &STATUS
                                        );

   if ( STATUS <= 0)     continue;



//  filling the Hough plot

          for(ii=0; ii<3; ii++){
           for(jj=0; jj< HelixResult.Nhelix[ii]; jj++){

            HelixResult.FI0[ii][jj] = fmod(HelixResult.FI0[ii][jj], 2.*PI);
            if ( HelixResult.FI0[ii][jj] < 0.)  HelixResult.FI0[ii][jj] += 2.*PI; 


            if(HelixResult.KAPPA[ii][jj]< KAPPAmin || HelixResult.KAPPA[ii][jj]>= KAPPAmax) continue; 
            if(HelixResult.FI0[ii][jj]< FI0min || HelixResult.FI0[ii][jj]>= FI0max) continue; 
            n_K = (int) ( (HelixResult.KAPPA[ii][jj]-KAPPAmin)/stepKAPPA );
            n_FI0 = (int) ( (HelixResult.FI0[ii][jj]-FI0min)/stepFI0 );
            BoxKFI0[n_K][n_FI0]++;
            RemainingKAPPA[Nremaining2]=HelixResult.KAPPA[ii][jj];
            RemainingFI0[Nremaining2]=HelixResult.FI0[ii][jj];
            Nremaining2++;

           }   //   end of for(jj=0; jj< HelixResult.Nhelix[ii]; jj++)
         }    //   end of for(ii=0; ii<3; ii++)



      }   // end of for(nSkew=nSkew1+1;
     }    // end of for(nSkew1=0;


//-----------------------------------------------------------------------------------------------------------------------

//  searching the maxima of Hough histo of KAPPA and FI0, new method

    findmaximaKFI0( BoxKFI0, MINIMUMCOUNTSKAPPAFI0, &NumberofMaximaKFI0, MaximaIndexesKFI0,
                      &STATUS );
    if(STATUS<=0) {
      cout<<"Ricerca grossa con skew inclinate fallisce : STATUS = "<<STATUS<<",  n. massimo DFiR = "<<imaxima+1<<endl;
      return;
    }

//--------------------------------------------  end KAPPA, FI0 new method

//  searching cell of maximum of histogram of KAPPA,FI0 --------------


/*
  max1 = -1.;
  for(jj=0; jj<nbinKAPPA; jj++){
   for(ii=0; ii<nbinFI0; ii++){

       if(BoxKFI0[jj][ii] > max1 ){
         max1=BoxKFI0[jj][ii];
         jjmax = jj;
         iimax = ii;
       }

   }   //  end   for (ii=0;
  }   //  end   for (jj=0;


*/



  Int_t NSurvivingResults=0;
  Double_t SurvivingResults[2][MAXElementsOverThresholdinHough];


  for(jmaxima=0; jmaxima<NumberofMaximaKFI0;jmaxima++){


  tempFI0 = FI0min+(MaximaIndexesKFI0[jmaxima][1]+0.5)*stepFI0;
  tempKAPPA = KAPPAmin+(MaximaIndexesKFI0[jmaxima][0]+0.5)*stepKAPPA;

    KAPPAlow= tempKAPPA-DELTA_KAPPA;
    KAPPAup= tempKAPPA+DELTA_KAPPA;
    FI0low = tempFI0-DELTA_FI0;
    FI0up = tempFI0+DELTA_FI0;



//  finding the maximum in the Hough plot again for KAPPA, FI0 (finer granularity)

    for(ii=0; ii<nbinFI0; ii++){
      for(jj=0; jj<nbinKAPPA; jj++){
         BoxKFI0[jj][ii]=0;
      }
    }

    for(i=0;   i<Nremaining2; i++){
      if( RemainingKAPPA[i]>KAPPAlow && RemainingKAPPA[i]<KAPPAup
                             &&
          RemainingFI0[i]>FI0low && RemainingFI0[i]<FI0up
        )  {

            n_K = (int) ( (RemainingKAPPA[i]-KAPPAlow)/stepfineKAPPA );
            n_FI0 = (int) ( (RemainingFI0[i]-FI0low)/stepfineFI0 );
            BoxKFI0[n_K][n_FI0]++;
      }
    }

  max1 = -1;
  for(jj=0; jj<nbinKAPPA; jj++){
   for(ii=0; ii<nbinFI0; ii++){

       if(BoxKFI0[jj][ii] > max1 ){
         max1=BoxKFI0[jj][ii];
         jjmax = jj;
         iimax = ii;
       }

   }   //  end   for (ii=0;
  }   //  end   for (jj=0;

  HoughFI0 = FI0low+(iimax+0.5)*stepfineFI0;
  HoughKAPPA = KAPPAlow+(jjmax+0.5)*stepfineKAPPA;




  for ( ii=0; ii<NSurvivingResults; ii++){
      if( fabs( HoughFI0 - SurvivingResults[1][ii]) < 0.006 && fabs( HoughKAPPA - SurvivingResults[0][ii]) < 0.001 ) {
        cout<<"questo massimo in KAPPA e FI0 e' praticamente uguale ad uno dei precedenti e viene eliminato\n";
        goto fine ;
      }
  }
  SurvivingResults[0][NSurvivingResults] = HoughKAPPA;
  SurvivingResults[1][NSurvivingResults] = HoughFI0;
  NSurvivingResults++;



//------------------------------------------------------------------

  ResultAssociatedHits =
           PndSttTrkAssociatedHitsToHelix(HoughKAPPA,HoughFI0,HoughD,HoughFi,HoughR,Nhits,info,Nincl,Mincl,inclination,imaxima, jmaxima);



//------------------------------------  plots finali



  fine: ;

 }   //  end of  for(jmaxima=0; jmaxima<NumberofMaximaKFI0;jmaxima++)





   }   //  end of     for(imaxima=0; i<NumberofMaximaDFiR;i++)




 }; //----------------------------------------------------   end of function  PndSttTrackFinderReal::PndSttTrkFinderPartial


CalculatedCircles PndSttTrackFinderReal::PndSttTrkFindCircles(Double_t x1,Double_t y1,Double_t r1,
                                                 Double_t x2,Double_t y2,Double_t r2,
                                                 Double_t x3,Double_t y3,Double_t r3)
{
  Double_t a,b,c,d,ap,bp,cp,dp,radix,solution,Radius, A,B,C,D,AAA,BBB,CCC,DELTA,S,T,X;
  Int_t Nsol = 0;
  Double_t R[8],CX[8],CY[8];
  CalculatedCircles Ris ;

//cout<<"---------------------------------- inizio printout da find_circles \n";


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

/*
cout<<"printout iniziale :\nx1="<<x1<<";\nx2="<<x2<<";\nx3="<<x3<<";\ny1="<<y1<<";\ny2="<<y2<<";\ny3="
       <<y3<<";\nr1="<<r1<<";\nr2="<<r2<<";\nr3="<<r3<<endl;
cout<<" a= "<<a<<";  b="<<b<<";  d="<<d<<endl;
cout<<" ap= "<<ap<<";  bp="<<bp<<";  dp="<<dp<<endl;
cout<<"fine printout iniziale\n";
*/

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



 CalculatedHelix PndSttTrackFinderReal::PndSttTrkFindHelix(
      Double_t Ox, Double_t Oy, Double_t R,
      Double_t Zcenter1,Double_t Zcenter2,Double_t Zcenter3,
      Double_t semilengthStraight1, Double_t semilengthStraight2, Double_t semilengthStraight3,
      Double_t C0x1, Double_t C0y1, Double_t C0z1, Double_t semilengthSkew1,
      Double_t r1, Double_t vx1, Double_t vy1, Double_t vz1,
      Double_t C0x2, Double_t C0y2, Double_t C0z2, Double_t semilengthSkew2,
      Double_t r2, Double_t vx2, Double_t vy2, Double_t vz2,
      Int_t *STATUS
                                                         )
{


//-------------------------------------------



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

*/

  Int_t  Nsolutions, NTOTAL,  Msol, i, j, i1, i2, k1, k2,  enne, jtemp, nfile;

  Double_t aaa, bbb, ccc, ddd, eee, fff,
           bp, cp, bs, cs,
           q1, q2pos, q2neg,
           Rx, Ry, SkewInclWithRespectToS, Aellipsis1, Bellipsis1, Aellipsis2, Bellipsis2, LL,
           alpha, beta, gamma, delta, epsilon,DELTA, A, B, C, D, cosalfa1, cosalfa2,
           alpha1, beta1, gamma1,
           x0, y0, fi1, fi2,
           SSS1, SSS2,
           xmin,xmax,ymin,ymax,
           x1,x2,y1,y2,
           POINTS1[6],POINTS2[6],distance1[2], distance2[2],
           M[4], Temp[4], Tiltdirection1[2], Tiltdirection2[2] ;

  const Double_t PI = 3.141592654;

  bool BAD1[2], BAD2[2] ;

  char nome[100];

  CalculatedHelix  Result;

//--------------------------



//       cout<<"Entra in PndSttTrkFindHelix.........\n";



 ntimes++;


 aaa = sqrt(vx1*vx1+vy1*vy1+vz1*vz1);
 vx1 /= aaa;
 vy1 /= aaa;
 vz1 /= aaa;
 aaa = sqrt(vx2*vx2+vy2*vy2+vz2*vz2);
 vx2 /= aaa;
 vy2 /= aaa;
 vz2 /= aaa;


 NTOTAL=0;
 Result.Nhelix[0] = Result.Nhelix[1] = Result.Nhelix[2] = 0;

//  calculation of the intersection points between skew straw axis and trajectory cylinder ----------------------------

 calculateintersections(Ox,Oy,R,C0x1,C0y1,C0z1,r1,vx1,vy1,vz1, STATUS,POINTS1);
 if(*STATUS < 0 ) { return Result;}

 for( i=0; i<2; i++){
  j=3*i;
  distance1[i] = sqrt(
                  (POINTS1[j]-C0x1)*(POINTS1[j]-C0x1) + 
                  (POINTS1[1+j]-C0y1)*(POINTS1[1+j]-C0y1) + 
                  (POINTS1[2+j]-C0z1)*(POINTS1[2+j]-C0z1) 
                 );
  if( distance1[i] >= semilengthSkew1 ){
     BAD1[i] = true ;
  } else {
     BAD1[i]= false ;
  }
 }

//----------------

  if ( BAD1[0]  && BAD1[1] ) {
    *STATUS = -4;
    return Result;
  }

//----------------

 calculateintersections(Ox,Oy,R,C0x2,C0y2,C0z2,r2,vx2,vy2,vz2, STATUS,POINTS2);
 if(*STATUS < 0 ) { return Result;}

//-------------------------
 for( i=0; i<2; i++){
  j=3*i;
  distance2[i] = sqrt(
                  (POINTS2[j]-C0x2)*(POINTS2[j]-C0x2) +
                  (POINTS2[1+j]-C0y2)*(POINTS2[1+j]-C0y2) + 
                  (POINTS2[2+j]-C0z2)*(POINTS2[2+j]-C0z2) 
                 );

  if( distance2[i] >= semilengthSkew2 ){
     BAD2[i] = true ;
  } else {
     BAD2[i]= false ;
  }
 }
//-------------------------

  if ( BAD2[0] && BAD2[1] ) {
    *STATUS = -5;
    return Result;
  }

//---------------------------------------------------------------------------------------------------------------------


for(k1=0; k1<2;k1++){
//              if( BAD1[k1] )  { cout<<"per k1 = "<<k1<<"  la intersezione e' BAD\n";}
 if( BAD1[k1] )  continue;
 i1 = 3*k1;

//  calculation of the approximate axis length of the ellipses projection of the skew straw on the plane tangent to the trajectory cylinder.
//  first skew straw

 Rx = POINTS1[i1]-Ox ;   //  x component Radial vector of cylinder of trajectory
 Ry = POINTS1[1+i1]-Oy ;   //  y direction Radial vector of cylinder of trajectory

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
 Aellipsis1 = r1*aaa/LL;


// checks that the projected ellipsis doesn't go out the boundaries of both the skew straw and the trajectory cylinder
// under the safe assumption that the ellipse has the major axis in Z direction.

 if(
    Aellipsis1 > semilengthStraight1-fabs(POINTS1[i1+2]-Zcenter1)   ||
    Aellipsis1 > semilengthStraight2-fabs(POINTS1[i1+2]-Zcenter2)   ||
    Aellipsis1 > semilengthStraight3-fabs(POINTS1[i1+2]-Zcenter3)   ||
    distance1[k1] + Aellipsis1 > semilengthSkew1        //  the ellipsis goes out of the boundaries of the skew straw
   )  continue;




  fi1 = atan2(POINTS1[i1+1]-Oy, POINTS1[i1]-Ox) ;  // atan2 returns radians in (-pi and +pi]
  if( fi1 < 0.) fi1 += 2.*PI;


 for(k2=0; k2<2;k2++){
  if(  BAD2[k2] )  continue;

  i2 = 3*k2;

//  calculation of the approximate axis length of the ellipses projection of the skew straw on the plane tangent to the trajectory cylinder.
//  first skew straw

  Rx = POINTS2[i2]-Ox ;   //  x direction along R of cylinder of trajectory
  Ry = POINTS2[1+i2]-Oy ;   //  y direction along R of cylinder of trajectory

  aaa = sqrt(Rx*Rx+Ry*Ry);
  SkewInclWithRespectToS = (-Ry*vx2 + Rx*vy2)/aaa ;
  SkewInclWithRespectToS /= R;
  bbb = sqrt( SkewInclWithRespectToS*SkewInclWithRespectToS + vz2*vz2);
//  the tilt direction of this ellipse is (1,0)  when major axis along Z direction
 if( bbb > 1.e-10){
   Tiltdirection2[0] = vz2/bbb;
   Tiltdirection2[1] = SkewInclWithRespectToS/bbb;
 } else {
   Tiltdirection2[0] = 1.;
   Tiltdirection2[1] = 0.;
 }
 LL = fabs(vx2*Rx + vy2*Ry);
 if(LL < 1.e-10) continue;
 Aellipsis2 = r2*aaa/LL;
// Bellipsis2 = r2;

// checks that the projected ellipsis doesn't go out the boundaries of both the skew straw and the trajectory cylinder

  if(
    Aellipsis2 > semilengthStraight1-fabs(POINTS2[i2+2]-Zcenter1)   ||  // the ellipsis goes out of the boundaries of the straight straws
    Aellipsis2 > semilengthStraight2-fabs(POINTS2[i2+2]-Zcenter2)   ||
    Aellipsis2 > semilengthStraight3-fabs(POINTS2[i2+2]-Zcenter3)   ||
    distance2[k2] + Aellipsis2 > semilengthSkew2        //  the ellipsis goes out of the boundaries of the skew straw
    )  continue;


//    end of boundaries checks, now find KAPPA and FI0





     fi2 = atan2(POINTS2[i2+1]-Oy, POINTS2[i2]-Ox) ;  // atan2 returns radians in (-pi and +pi]
     if ( fi2 < 0.)  fi2 += 2.*PI;





//   translation with the new variables ( those of the lateral surface of the trajectory cylinder and with the first ellipsis
//   positioned at 0,0


  for ( enne = -1; enne<2; enne++) {    //  enne  is the order of the solution


//   translation with the new variables ( those of the lateral surface of the trajectory cylinder and with the first ellipsis
//   positioned at 0,0



     Result.Nhelix[enne+1] = 4;

  for(i=0; i<2; i++){
   for(j=0; j<2; j++){

     x1 =  POINTS1[i1+2] + (1 - 2*j)*Aellipsis1*Tiltdirection1[0];
     y1 = fi1 + (1 - 2*j)*Aellipsis1*Tiltdirection1[1] ;
     x2 =  POINTS2[i2+2] + (1 - 2*i)*Aellipsis2*Tiltdirection2[0];
     y2 = fi2 + (1 - 2*i)*Aellipsis2*Tiltdirection2[1] + enne * 2. * PI  ;




     Nsolutions = j + 2*i;

     if ( x2-x1 != 0.){
       Result.KAPPA[enne+1][Nsolutions] =  (y2-y1) / (x2-x1) ;
       Result.FI0[enne+1][Nsolutions] = -Result.KAPPA[enne+1][Nsolutions] * x1 + y1;
     } else {
       Result.KAPPA[enne+1][Nsolutions] = 1.e14;
       Result.FI0[enne+1][Nsolutions] = x1;
     }

    }
   }      //  end of   for(i=0; i<2; i++)







// --------------------- inizio stampe diagnostiche e macros diagnostiche



// --------------------------------------------------------------------   stampa delle macro di controllo
// --------------------- fine stampe diagnostiche e macros diagnostiche




   }  //   end  for(enne=0; enne<2; enne++)

  }   // end  for(k2=0;
 }   // end  for(k1=0;

  NTOTAL=4;
 *STATUS = NTOTAL;
 
//----------------------------------------------------------------------------------------------------------------------

/*
cout<<"ultima stampa, N totale soluzioni = "<<NTOTAL<<"; N a 0 giri = "<<
     Result.Nhelix[1]<<"; N a 1 giro "<<Result.Nhelix[2]<<"; N ad un giro sotto "<<Result.Nhelix[0]<<endl;

cout<<"Esce da PndSttTrkFindHelix.........\n";
*/



 return Result;

}
//--------------------------------  end of function         PndSttTrackFinderReal::PndSttTrkFindHelix







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






  AssociatedHitsToHelix PndSttTrackFinderReal::PndSttTrkAssociatedHitsToHelix(
                   Double_t KAPPA,Double_t FI0,Double_t D,Double_t Fi,Double_t R,
                   Int_t Nhits, Double_t info[][6], Int_t Nincl, Int_t Mincl[], Double_t inclination[][3],
                   Int_t imaxima, Int_t nMaxima
                                                     )
{

    Int_t i, j, i1, ii, index, Kincl, nlow, nup, STATUS;

    Double_t xmin , xmax, ymin, ymax, Ox, Oy,
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

    AssociatedHitsToHelix ResultAssociatedHits;




    Ox = (D+R)*cos(Fi);
    Oy = (D+R)*sin(Fi);









//  association of hits, ambiguities, errors.

       ResultAssociatedHits.NAssociatedHits=0;
       ResultAssociatedHits.NAssociatedParallelHits=0;
       ResultAssociatedHits.NAssociatedSkewHits=0;

if(IVOLTE== 16) {   cout<<"evento 16, from PndSttTrkAssociatedHitsToHelix, Nhits "<<Nhits<<endl; }

       for( i=0; i< Nhits; i++) {

          if( ResultAssociatedHits.NAssociatedHits == nmaxAssociatedHits )  break;
          Kincl = (int) info[i][5] - 1;

// --------------------    association of the hits from parallel straws
         if( info[i][5] == 1. ) {     // parallel straws

          dx = -Ox+info[i][0];
          dy = -Oy+info[i][1];
          distance = sqrt(dx*dx+dy*dy);
          if( distance < 1.e-10)  continue;
          angle = atan2(dy,dx);

          if ( fabs(R - distance ) > nAdmittedRadia*StrawRadius )  continue;
          if(distance > R )  distance *= -1.;
             if( KAPPA == 0.) {
                ResultAssociatedHits.AssociatedHitsCoordinates[0][0][ResultAssociatedHits.NAssociatedHits]= info[i][0]+info[i][3]*dx/distance;  //  X of the hit
                ResultAssociatedHits.AssociatedHitsCoordinates[1][0][ResultAssociatedHits.NAssociatedHits]= info[i][1]+info[i][3]*dy/distance;  //  Y of the hit
                ResultAssociatedHits.AssociatedHitsCoordinates[2][0][ResultAssociatedHits.NAssociatedHits] = -1.e20;
                ResultAssociatedHits.mAmbiguities[ResultAssociatedHits.NAssociatedHits]=1;
                ResultAssociatedHits.AssociatedWireDirection[0][ResultAssociatedHits.NAssociatedHits]=inclination[Kincl][0];
                ResultAssociatedHits.AssociatedWireDirection[1][ResultAssociatedHits.NAssociatedHits]=inclination[Kincl][1];
                ResultAssociatedHits.AssociatedWireDirection[2][ResultAssociatedHits.NAssociatedHits]=inclination[Kincl][2];
                ResultAssociatedHits.AssociatedWireCenter[0][ResultAssociatedHits.NAssociatedHits]=info[i][0];
                ResultAssociatedHits.AssociatedWireCenter[1][ResultAssociatedHits.NAssociatedHits]=info[i][1];
                ResultAssociatedHits.AssociatedWireCenter[2][ResultAssociatedHits.NAssociatedHits]=info[i][2];
                ResultAssociatedHits.AssociatedWireDriftRadius[ResultAssociatedHits.NAssociatedHits]=info[i][3];
                ResultAssociatedHits.Hitnumber[ResultAssociatedHits.NAssociatedHits]=i;  // temporaneo!
                ResultAssociatedHits.NAssociatedHits++;
                ResultAssociatedHits.NAssociatedParallelHits++;
                continue;
             } else  {
                diff = FI0-angle;
                major = 0.5*(KAPPA*(info[i][2]+info[i][4])+diff)/PI ;
                minor = 0.5*(KAPPA*(info[i][2]-info[i][4])+diff)/PI ;
                if ( major < minor) {
                 aaa=minor;
                 minor=major;
                 major=aaa;
                }
                minor <= 0. ?  nlow = (int) minor    : nlow = ( (int) minor) + 1;
                major <  0. ?  nup = ((int) major)-1 : nup  = (int) major;

                if( nlow> nup)  continue;
                if( nup-nlow+1 > nmaxAmbiguities )  continue;

                step = fabs(2.*PI/KAPPA) ;
                aaa = -diff/KAPPA ;
                bbb = info[i][0]+info[i][3]*dx/distance;  //  X of the hit
                ccc = info[i][1]+info[i][3]*dy/distance;  //  Y of the hit

                for(ii=nlow; ii<=nup; ii++){
                  ResultAssociatedHits.AssociatedHitsCoordinates[0][ii-nlow][ResultAssociatedHits.NAssociatedHits]= bbb;  //  X of the hit
                  ResultAssociatedHits.AssociatedHitsCoordinates[1][ii-nlow][ResultAssociatedHits.NAssociatedHits]= ccc ;  //  Y of the hit
                  ResultAssociatedHits.AssociatedHitsCoordinates[2][ii-nlow][ResultAssociatedHits.NAssociatedHits] = aaa + ii*step;   //  Z of the hit
                }
                ResultAssociatedHits.mAmbiguities[ResultAssociatedHits.NAssociatedHits] = nup - nlow + 1;
                ResultAssociatedHits.AssociatedWireDirection[0][ResultAssociatedHits.NAssociatedHits]=inclination[Kincl][0];
                ResultAssociatedHits.AssociatedWireDirection[1][ResultAssociatedHits.NAssociatedHits]=inclination[Kincl][1];
                ResultAssociatedHits.AssociatedWireDirection[2][ResultAssociatedHits.NAssociatedHits]=inclination[Kincl][2];
                ResultAssociatedHits.AssociatedWireCenter[0][ResultAssociatedHits.NAssociatedHits]=info[i][0];
                ResultAssociatedHits.AssociatedWireCenter[1][ResultAssociatedHits.NAssociatedHits]=info[i][1];
                ResultAssociatedHits.AssociatedWireCenter[2][ResultAssociatedHits.NAssociatedHits]=info[i][2];
                ResultAssociatedHits.AssociatedWireDriftRadius[ResultAssociatedHits.NAssociatedHits]=info[i][3];

                ResultAssociatedHits.Hitnumber[ResultAssociatedHits.NAssociatedHits]=i;  // temporaneo!
                ResultAssociatedHits.NAssociatedHits++;
                ResultAssociatedHits.NAssociatedParallelHits++;
             }  //  end  if( KAPPA == 0.)


            }  else {

//------------------------------ starts here association of skew straw hits

       //  calculation of the intersection points between skew straw axis and trajectory cylinder


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


// checks that the projected ellipsis doesn't go out the boundaries of both the skew straw and the trajectory cylinder

             if(
                fabs(POINTS1[j+2]-ZCENTER_STRAIGHT) > SEMILENGTH_STRAIGHT- Aellipsis1 ||
                distance + bbb > info[i][4]        //  the ellipsis goes out of the boundaries of the skew straw
               )  continue;
//--------------------------

             fi1 = atan2(POINTS1[j+1]-Oy, POINTS1[j]-Ox) ;  // atan2 returns radians in (-pi and +pi]
             if( fi1 < 0.) fi1 += 2.*PI;


//   now calculate the distance (along the Z direction) from the edge of the ellipsis and the predicted Helix
//   trajectory
             if(fabs(KAPPA) > 1.e-10) {
                z1 = POINTS1[j+2] + Aellipsis1*Tiltdirection1[0];
                y1 = fi1 + Aellipsis1*Tiltdirection1[1] ;
                zpos1 = (y1 - FI0)/KAPPA;
                d1 = fabs(zpos1 - z1);
                z2 = POINTS1[j+2] - Aellipsis1*Tiltdirection1[0];
                y2 = fi1 - Aellipsis1*Tiltdirection1[1]   ;
                zpos2 = (y2 - FI0)/KAPPA;
                d2 = fabs(zpos2 - z2);
                if ( d2>d1 ) {
                    distance = d1;
                    if( distance < nAdmittedRadia * Aellipsis1 ) {
                      ResultAssociatedHits.AssociatedHitsCoordinates[0][0][ResultAssociatedHits.NAssociatedHits]= R*cos(y1) + Ox;  //  X of the hit
                      ResultAssociatedHits.AssociatedHitsCoordinates[1][0][ResultAssociatedHits.NAssociatedHits]= R*sin(y1) + Oy;  //  Y of the hit
                      ResultAssociatedHits.AssociatedHitsCoordinates[2][0][ResultAssociatedHits.NAssociatedHits] = z1;
                      ResultAssociatedHits.mAmbiguities[ResultAssociatedHits.NAssociatedHits]=1;
                      ResultAssociatedHits.AssociatedWireDirection[0][ResultAssociatedHits.NAssociatedHits]=inclination[Kincl][0];
                      ResultAssociatedHits.AssociatedWireDirection[1][ResultAssociatedHits.NAssociatedHits]=inclination[Kincl][1];
                      ResultAssociatedHits.AssociatedWireDirection[2][ResultAssociatedHits.NAssociatedHits]=inclination[Kincl][2];
                      ResultAssociatedHits.AssociatedWireCenter[0][ResultAssociatedHits.NAssociatedHits]=info[i][0];
                      ResultAssociatedHits.AssociatedWireCenter[1][ResultAssociatedHits.NAssociatedHits]=info[i][1];
                      ResultAssociatedHits.AssociatedWireCenter[2][ResultAssociatedHits.NAssociatedHits]=info[i][2];
                      ResultAssociatedHits.AssociatedWireDriftRadius[ResultAssociatedHits.NAssociatedHits]=info[i][3];
                      ResultAssociatedHits.Hitnumber[ResultAssociatedHits.NAssociatedHits]=-i;  // temporaneo!
                      ResultAssociatedHits.NAssociatedHits++;
                      ResultAssociatedHits.NAssociatedSkewHits++;
                    }

                } else {
                    distance = d2 ;
                    if( distance < nAdmittedRadia * Aellipsis1 ) {
                      ResultAssociatedHits.AssociatedHitsCoordinates[0][0][ResultAssociatedHits.NAssociatedHits]= R*cos(y2) + Ox;  //  X of the hit
                      ResultAssociatedHits.AssociatedHitsCoordinates[1][0][ResultAssociatedHits.NAssociatedHits]= R*sin(y2) + Oy;  //  Y of the hit
                      ResultAssociatedHits.AssociatedHitsCoordinates[2][0][ResultAssociatedHits.NAssociatedHits] = z2;
                      ResultAssociatedHits.mAmbiguities[ResultAssociatedHits.NAssociatedHits]=1;
                      ResultAssociatedHits.AssociatedWireDirection[0][ResultAssociatedHits.NAssociatedHits]=inclination[Kincl][0];
                      ResultAssociatedHits.AssociatedWireDirection[1][ResultAssociatedHits.NAssociatedHits]=inclination[Kincl][1];
                      ResultAssociatedHits.AssociatedWireDirection[2][ResultAssociatedHits.NAssociatedHits]=inclination[Kincl][2];
                      ResultAssociatedHits.AssociatedWireCenter[0][ResultAssociatedHits.NAssociatedHits]=info[i][0];
                      ResultAssociatedHits.AssociatedWireCenter[1][ResultAssociatedHits.NAssociatedHits]=info[i][1];
                      ResultAssociatedHits.AssociatedWireCenter[2][ResultAssociatedHits.NAssociatedHits]=info[i][2];
                      ResultAssociatedHits.AssociatedWireDriftRadius[ResultAssociatedHits.NAssociatedHits]=info[i][3];
                      ResultAssociatedHits.Hitnumber[ResultAssociatedHits.NAssociatedHits]=-i;  // temporaneo!
                      ResultAssociatedHits.NAssociatedHits++;
                      ResultAssociatedHits.NAssociatedSkewHits++;
                    }
                }


              }  else  {    //  case when fabs(KAPPA) < 1.e-10

//  pick up the hits whose ellipse is close to the staight line at constant FI0
                z1 = POINTS1[j+2] + Aellipsis1*Tiltdirection1[0];
                y1 = fi1 + Aellipsis1*Tiltdirection1[1] ;
                d1 = fabs(y1 - FI0);
                z2 = POINTS1[j+2] - Aellipsis1*Tiltdirection1[0];
                y2 = fi1 - Aellipsis1*Tiltdirection1[1]   ;
                d1 = fabs(y2 - FI0);
                if(d1<nAdmittedRadia *Bellipsis1 || d1<nAdmittedRadia *Bellipsis1) {
                 if( d1 < d2 ){
                    y1 > FI0 ? factor = -1. : factor = 1.;
                    ResultAssociatedHits.AssociatedHitsCoordinates[0][0][ResultAssociatedHits.NAssociatedHits]=
                            R*cos(y1+factor*Bellipsis1) + Ox;  //  X of the hit
                    ResultAssociatedHits.AssociatedHitsCoordinates[1][0][ResultAssociatedHits.NAssociatedHits]=
                            R*sin(y1+factor*Bellipsis1) + Oy;  //  Y of the hit
                  }  else {
                    y2 > FI0 ? factor = -1. : factor = 1.;
                    ResultAssociatedHits.AssociatedHitsCoordinates[0][0][ResultAssociatedHits.NAssociatedHits]=
                            R*cos(y2+factor*Bellipsis1) + Ox;  //  X of the hit
                    ResultAssociatedHits.AssociatedHitsCoordinates[1][0][ResultAssociatedHits.NAssociatedHits]=
                            R*sin(y2+factor*Bellipsis1) + Oy;  //  Y of the hit
                  }
                   
                  ResultAssociatedHits.AssociatedHitsCoordinates[2][0][ResultAssociatedHits.NAssociatedHits] = (z1+z2)/2.;
                  ResultAssociatedHits.mAmbiguities[ResultAssociatedHits.NAssociatedHits]=1;
                  ResultAssociatedHits.AssociatedWireDirection[0][ResultAssociatedHits.NAssociatedHits]=inclination[Kincl][0];
                  ResultAssociatedHits.AssociatedWireDirection[1][ResultAssociatedHits.NAssociatedHits]=inclination[Kincl][1];
                  ResultAssociatedHits.AssociatedWireDirection[2][ResultAssociatedHits.NAssociatedHits]=inclination[Kincl][2];
                  ResultAssociatedHits.AssociatedWireCenter[0][ResultAssociatedHits.NAssociatedHits]=info[i][0];
                  ResultAssociatedHits.AssociatedWireCenter[1][ResultAssociatedHits.NAssociatedHits]=info[i][1];
                  ResultAssociatedHits.AssociatedWireCenter[2][ResultAssociatedHits.NAssociatedHits]=info[i][2];
                  ResultAssociatedHits.AssociatedWireDriftRadius[ResultAssociatedHits.NAssociatedHits]=info[i][3];
                  ResultAssociatedHits.Hitnumber[ResultAssociatedHits.NAssociatedHits]=-i;  // temporaneo!
                  ResultAssociatedHits.NAssociatedHits++;
                  ResultAssociatedHits.NAssociatedSkewHits++;
                }

              }   //   end of  if(fabs(KAPPA) > 1.e-10)

             }    //  end of    for( ii=0; ii<2; ii++)



            }    //  end of   if( info[i][5] == 1 )  else


       }    //   end of   for( i=1; i< Nhits; i++)





    return ResultAssociatedHits;

}


//----------end of function PndSttTrackFinderReal::PndSttTrkAssociatedHitsToHelix









  Int_t PndSttTrackFinderReal::PndSttTrkAssociatedParallelHitsToHelix(
                   Double_t D,Double_t Fi,Double_t R,
                   Int_t Nhits, Double_t info[][6], Int_t Nincl, Int_t Mincl[], Double_t inclination[][3]
                                                     )
{

    Int_t i,Kincl;
    Int_t Nassociatedhits;

    Double_t Ox, Oy,dx, dy,distance, angle;


    Ox = (D+R)*cos(Fi);
    Oy = (D+R)*sin(Fi);



//  association of hits in the XY plane

       Nassociatedhits=0;

       for( i=0; i< Nhits; i++) {


          Kincl = (int) info[i][5] - 1;

// --------------------    association of the hits from parallel straws
         if( info[i][5] == 1. ) {     // parallel straws

          dx = -Ox+info[i][0];
          dy = -Oy+info[i][1];
          distance = sqrt(dx*dx+dy*dy);
          if( distance < 1.e-10)  continue;
          angle = atan2(dy,dx);

          if ( fabs(R - distance ) > nAdmittedRadia*StrawRadius )  continue;

          Nassociatedhits++;

         }    //  end of   if( info[i][5] == 1 )  else


       }    //   end of   for( i=1; i< Nhits; i++)






    return Nassociatedhits;

}


//----------end of function PndSttTrackFinderReal::PndSttTrkAssociatedParallelHitsToHelix





//----------start of function PndSttTrackFinderReal::plottamentiParalleleGenerali

 void  PndSttTrackFinderReal::plottamentiParalleleGenerali(
                      Int_t Nremaining, Float_t * RemainingR, Float_t * RemainingD,
                      Float_t * RemainingFi, Float_t * RemainingCX, Float_t * RemainingCY  
                       )
{


   int itemp, i, j, ii, jj;

   Double_t  D, Fi;

   char nome[100],titolo[100];



   sprintf(nome,"HoughGeneralPlotsEvent%d.root",IVOLTE);
   TFile  hfile(nome,"RECREATE", "STT pattern recognition");

//----------
       sprintf(titolo,"Cxofcircle");
       TH1F hCX(titolo,titolo,nbinCX,CXmin,CXmax);
//----------
       sprintf(titolo,"Cyofcircle");
       TH1F hCY(titolo,titolo,nbinCY,CYmin,CYmax);
//-----
       sprintf(titolo,"Radiusofcircle");
       TH1F hR(titolo,titolo,nbinR,Rmin,Rmax);
//-----
       sprintf(titolo,"CxabscissavsCy");
       TH2F hCX_CY(titolo,titolo,nbinCX,CXmin,CXmax,nbinCY,CYmin,CYmax);
//-----
       sprintf(titolo,"CxabscissavsRadius");
       TH2F hCX_R(titolo,titolo,nbinCX,CXmin,CXmax,nbinR,Rmin,Rmax);
//-----
       sprintf(titolo,"CyabscissavsRadius");
       TH2F hCY_R(titolo,titolo,nbinCY,CYmin,CYmax,nbinR,Rmin,Rmax);
//-----
       sprintf(titolo,"CxabscissavsCyordinatevsRadius");
       TH3F hCX_CY_R(titolo,titolo,nbinCX,CXmin,CXmax,nbinCY,CYmin,CYmax,nbinR,Rmin,Rmax);
//-----
       sprintf(titolo,"Distanceofclosestapproachofcircle");
       TH1F hD(titolo,titolo,nbinD,Dmin,Dmax);
//----------
       sprintf(titolo,"Firadofcircle");
       TH1F hFi(titolo,titolo,nbinFi,Fimin,Fimax);
//-----
       sprintf(titolo,"DabscissavsFi");
       TH2F hD_Fi(titolo,titolo,nbinD,Dmin,Dmax,nbinFi,Fimin,Fimax);
//-----
       sprintf(titolo,"DabscissavsRadius");
       TH2F hD_R(titolo,titolo,nbinD,Dmin,Dmax,nbinR,Rmin,Rmax);
//-----
       sprintf(titolo,"FiabscissavsRadius");
       TH2F hFi_R(titolo,titolo,nbinFi,Fimin,Fimax,nbinR,Rmin,Rmax);
//-----
       sprintf(titolo,"DabscissavsFiordinatevsRadius");
       TH3F hD_Fi_R(titolo,titolo,nbinD,Dmin,Dmax,nbinFi,Fimin,Fimax,nbinR,Rmin,Rmax);
//-----


  for(Int_t itemp = 0; itemp<Nremaining; itemp++){

       D = RemainingD[itemp];
       Fi = RemainingFi[itemp];


       hCX.Fill(RemainingCX[itemp]);
       hCY.Fill(RemainingCY[itemp]);
       hR.Fill(RemainingR[itemp]);
       hCX_CY.Fill(RemainingCX[itemp],RemainingCY[itemp]);
       hCX_R.Fill(RemainingCX[itemp],RemainingR[itemp]);
       hCY_R.Fill(RemainingCY[itemp],RemainingR[itemp]);
       hCX_CY_R.Fill(RemainingCX[itemp],RemainingCY[itemp],RemainingR[itemp]);

       hD.Fill(D);
       hFi.Fill(Fi);
       hD_Fi.Fill(D,Fi);
       hD_R.Fill(D,RemainingR[itemp]);
       hFi_R.Fill(Fi,RemainingR[itemp]);
       hD_Fi_R.Fill(D,Fi,RemainingR[itemp]);



   }  //  end of    for(Int_t itemp = 0; itemp<Nremaining; itemp++)





   hfile.Write(nome);
   hfile.Close();

 }

//----------end of function PndSttTrackFinderReal::plottamentiParalleleGenerali


//----------start of function PndSttTrackFinderReal::plottamentiParalleleconMassimo

 void  PndSttTrackFinderReal::plottamentiParalleleconMassimo(
                       char * tipo,
                      Int_t nMaxima,
                      Int_t Nremaining, Float_t * RemainingR, Float_t * RemainingD,
                      Float_t * RemainingFi, Float_t * RemainingCX, Float_t * RemainingCY,  
                Double_t Rup, Double_t Rlow, Double_t Dup, Double_t Dlow, Double_t Fiup, Double_t Filow
                                                            )
 {


   int itemp, i, j, ii, jj;

   Double_t  D, Fi;

   char nome[100],titolo[100];



   sprintf(nome,"HoughParallele%sMaximumN%devent%d.root",tipo,nMaxima,IVOLTE);
   TFile  hfile(nome,"RECREATE", "STT pattern recognition");


//----------
       sprintf(titolo,"CxofcircleSelected");
       TH1F hCXsel(titolo,titolo,nbinCX,CXmin,CXmax);
//-----
       sprintf(titolo,"CyofcircleSelected");
       TH1F hCYsel(titolo,titolo,nbinCY,CYmin,CYmax);
//-----
       sprintf(titolo,"RadiusofcircleSelected");
       TH1F hRsel(titolo,titolo,nbinR,Rlow,Rup);
//-----
       sprintf(titolo,"CxabscissavsCyordinatevsRadiusSelected");
       TH3F hCX_CY_Rsel(titolo,titolo,nbinCX,CXmin,CXmax,nbinCY,CYmin,CYmax,nbinR,Rlow,Rup);
//----------
       sprintf(titolo,"DistanceofclosestapproachofcircleSelected");
       TH1F hDsel(titolo,titolo,nbinD,Dlow,Dup);
//-----
       sprintf(titolo,"FiradofcircleSselected");
       TH1F hFisel(titolo,titolo,nbinFi,Filow,Fiup);
//-----
       sprintf(titolo,"DabscissavsFiSelected");
       TH2F hD_Fisel(titolo,titolo,nbinD,Dlow,Dup,nbinFi,Filow,Fiup);
//-----
       sprintf(titolo,"DabscissavsRadiusSelected");
       TH2F hD_Rsel(titolo,titolo,nbinD,Dlow,Dup,nbinR,Rlow,Rup);
//-----
       sprintf(titolo,"FiabscissavsRadiusSelected");
       TH2F hFi_Rsel(titolo,titolo,nbinFi,Filow,Fiup,nbinR,Rlow,Rup);
//-----
       sprintf(titolo,"DabscissavsFiordinatevsRadiusSelected");
       TH3F hD_Fi_Rsel(titolo,titolo,nbinD,Dlow,Dup,nbinFi,Filow,Fiup,nbinR,Rlow,Rup);




  for(Int_t itemp = 0; itemp<Nremaining; itemp++){

       D = RemainingD[itemp];
       Fi = RemainingFi[itemp];


      if( RemainingR[itemp]>Rlow && RemainingR[itemp]<Rup
                             &&
          RemainingD[itemp]>Dlow && RemainingD[itemp]<Dup
                             &&
          RemainingFi[itemp]>Filow && RemainingFi[itemp]<Fiup
        )  {
           hRsel.Fill(RemainingR[itemp]);
           hFisel.Fill(Fi);
           hDsel.Fill(D);
           hCXsel.Fill(RemainingCX[itemp]);
           hCYsel.Fill(RemainingCY[itemp]);
           hD_Fisel.Fill(D,Fi);
           hD_Rsel.Fill(D,RemainingR[itemp]);
           hFi_Rsel.Fill(Fi,RemainingR[itemp]);
           hD_Fi_Rsel.Fill(D,Fi,RemainingR[itemp]);
           hCX_CY_Rsel.Fill(RemainingCX[itemp],RemainingCY[itemp],RemainingR[itemp]);
      }

   }  //  end of    for(Int_t itemp = 0; itemp<Nremaining; itemp++)






   hfile.Write(nome);
   hfile.Close();

 }

//----------end of function PndSttTrackFinderReal::plottamentiParalleleconMassimo










//----------start of function PndSttTrackFinderReal::plottamentiSkewconMassimo

 void  PndSttTrackFinderReal::plottamentiSkewconMassimo(
                      Int_t nMaxima, Int_t jmaxima,
                      Int_t Nremaining2, Float_t * RemainingKAPPA, Float_t *RemainingFI0,
                      Double_t KAPPAlow, Double_t KAPPAup, Double_t FI0low, Double_t FI0up,
                      AssociatedHitsToHelix  ResultAssociatedHits,
                      Double_t HoughR, Double_t HoughD, Double_t HoughFi) {


   int itemp, i, j, ii, jj;

   Double_t  D, Fi;

   char nome[100],titolo[100];





   sprintf(nome,"HoughMaximumPMaxN%dSMaxN%devent%d.root",nMaxima,jmaxima,IVOLTE);
   TFile  hfile(nome,"RECREATE", "STT pattern recognition");



//----------
       sprintf(titolo,"KAPPAofHelix");
       TH1F hKAPPA(titolo,titolo,nbinKAPPA,KAPPAmin,KAPPAmax);
//----------
       sprintf(titolo,"KAPPAofHelixSelected");
       TH1F hKAPPAsel(titolo,titolo,nbinKAPPA,KAPPAlow,KAPPAup);
//----------
       sprintf(titolo,"FI0radofHelix");
       TH1F hFI0(titolo,titolo,nbinFI0,FI0min,FI0max);
//----------
       sprintf(titolo,"FI0radofHelixSelected");
       TH1F hFI0sel(titolo,titolo,nbinFI0,FI0low,FI0up);
//----------
       sprintf(titolo,"hits parallel straws : Xfound - Xtrue");
       TH1F hXdiffparallel(titolo,titolo,1000,-5.,5.);
//----------
       sprintf(titolo,"hits parallel straws : Yfound - Ytrue");
       TH1F hYdiffparallel(titolo,titolo,1000,-5.,5.);
//----------
       sprintf(titolo,"hits parallel straws : Zfound - Ztrue");
       TH1F hZdiffparallel(titolo,titolo,1000,-5.,5.);
//----------
       sprintf(titolo,"hits skew straws : Xfound - Xtrue");
       TH1F hXdiffskew(titolo,titolo,1000,-5.,5.);
//----------
       sprintf(titolo,"hits skew straws : Yfound - Ytrue");
       TH1F hYdiffskew(titolo,titolo,1000,-5.,5.);
//----------
       sprintf(titolo,"hits skew straws : Zfound - Ztrue");
       TH1F hZdiffskew(titolo,titolo,1000,-5.,5.);



//-------------
      sprintf(titolo,"FI0abscissavsKAPPA");
       TH2F hFI0_KAPPA(titolo,titolo,nbinFI0,FI0min,FI0max,nbinKAPPA,KAPPAmin,KAPPAmax);
//-------------
      sprintf(titolo,"FI0abscissavsKAPPASelected");
       TH2F hFI0_KAPPAsel(titolo,titolo,nbinFI0,FI0low,FI0up,nbinKAPPA,KAPPAlow,KAPPAup);
//-----
      sprintf(titolo,"FI0abscissavsRadius");
       TH2F hFI0_R(titolo,titolo,nbinFI0,FI0min,FI0max,nbinR,Rmin,Rmax);
//-----
      sprintf(titolo,"FI0abscissavsCX");
       TH2F hFI0_CX(titolo,titolo,nbinFI0,FI0min,FI0max,nbinCX,CXmin,CXmax);
//-----
      sprintf(titolo,"FI0abscissavsCY");
       TH2F hFI0_CY(titolo,titolo,nbinFI0,FI0min,FI0max,nbinCY,CYmin,CYmax);
//-----
      sprintf(titolo,"KAPPAHelixabscissavsRadius");
       TH2F hKAPPA_R(titolo,titolo,nbinKAPPA,KAPPAmin,KAPPAmax,nbinR,Rmin,Rmax);
//-----
      sprintf(titolo,"KAPPAHelixabscissavsCX");
       TH2F hKAPPA_CX(titolo,titolo,nbinKAPPA,KAPPAmin,KAPPAmax,nbinCX,CXmin,CXmax);
//-----
      sprintf(titolo,"KAPPAHelixabscissavsCY");
       TH2F hKAPPA_CY(titolo,titolo,nbinKAPPA,KAPPAmin,KAPPAmax,nbinCY,CYmin,CYmax);
//-----


 for(ii=0; ii<Nremaining2; ii++){


   hKAPPA.Fill(RemainingKAPPA[ii]);
   hFI0.Fill(RemainingFI0[ii]);
   hFI0_KAPPA.Fill(RemainingFI0[ii],RemainingKAPPA[ii]);
   hFI0_R.Fill(RemainingFI0[ii],HoughR);
   hFI0_CX.Fill(RemainingFI0[ii],(HoughR-HoughD)*cos(HoughFi));
   hFI0_CY.Fill(RemainingFI0[ii],(HoughR-HoughD)*sin(HoughFi));
   hKAPPA_R.Fill(RemainingKAPPA[ii],HoughR);
   hKAPPA_CX.Fill(RemainingKAPPA[ii],(HoughR-HoughD)*cos(HoughFi));
   hKAPPA_CY.Fill(RemainingKAPPA[ii],(HoughR-HoughD)*sin(HoughFi));

      if( RemainingKAPPA[ii]>KAPPAlow && RemainingKAPPA[ii]<KAPPAup
                             &&
          RemainingFI0[ii]>FI0low && RemainingFI0[ii]<FI0up
        )  {
           hKAPPAsel.Fill(RemainingKAPPA[ii]);
           hFI0sel.Fill(RemainingFI0[ii]);
           hFI0_KAPPAsel.Fill(RemainingFI0[ii],RemainingKAPPA[ii]);
      }
 }





    for (i=0; i< ResultAssociatedHits.NAssociatedHits ; i++){
       for(j=0; j< ResultAssociatedHits.mAmbiguities[i] ; j++){


         if( ResultAssociatedHits.Hitnumber[i] >= 0){
           hXdiffparallel.Fill(ResultAssociatedHits.AssociatedHitsCoordinates[0][j][i]-veritaMC[ResultAssociatedHits.Hitnumber[i]][0]);
           hYdiffparallel.Fill(ResultAssociatedHits.AssociatedHitsCoordinates[1][j][i]-veritaMC[ResultAssociatedHits.Hitnumber[i]][1]);
           hZdiffparallel.Fill(ResultAssociatedHits.AssociatedHitsCoordinates[2][j][i]-veritaMC[ResultAssociatedHits.Hitnumber[i]][2]);
         } else {
           hXdiffskew.Fill(ResultAssociatedHits.AssociatedHitsCoordinates[0][j][i]-veritaMC[-ResultAssociatedHits.Hitnumber[i]][0]);
           hYdiffskew.Fill(ResultAssociatedHits.AssociatedHitsCoordinates[1][j][i]-veritaMC[-ResultAssociatedHits.Hitnumber[i]][1]);
           hZdiffskew.Fill(ResultAssociatedHits.AssociatedHitsCoordinates[2][j][i]-veritaMC[-ResultAssociatedHits.Hitnumber[i]][2]);
         }

       }
    }





   hfile.Write(nome);
   hfile.Close();

 }

//----------end of function PndSttTrackFinderReal::plottamentiSkewconMassimo



  void PndSttTrackFinderReal::findmaximaDFiR( UShort_t BoxDFiR[nbinD][nbinFi][nbinR],
                                    Int_t MINIMUMCOUNTS,
                                    Int_t * NumberofMaximaDFiR, Int_t  MaximaIndexesDFiR[][3],
                                    Int_t * STATUS)
{


     Int_t      i, j, iD, iFi, iR, icount,
                ntotClusters,
                NinCluster,
                nClusterElementsFound,
                nRemai,
                nRemainingElements,
                max,
                nElementsinCluster[MAXElementsOverThresholdinHough];

     UShort_t   found[MAXElementsOverThresholdinHough][MAXElementsOverThresholdinHough][3],
                auxDFiRIndex[MAXElementsOverThresholdinHough][3],
                Remai[MAXElementsOverThresholdinHough][3],
                RemainingElements[MAXElementsOverThresholdinHough][3],
                Cluster[MAXElementsOverThresholdinHough][3],
                ClusterElementsFound[MAXElementsOverThresholdinHough][3];

     icount=0;
     for(iD=0; iD<nbinD; iD++){
      for(iFi=0; iFi<nbinFi; iFi++){
       for(iR=0; iR<nbinR; iR++){
         if(
              BoxDFiR[iD][iFi][iR] > MINIMUMCOUNTS
           ) {
             auxDFiRIndex[icount][0]=iD;
             auxDFiRIndex[icount][1]=iFi;
             auxDFiRIndex[icount][2]=iR;
             icount++;
             if(icount == MAXElementsOverThresholdinHough){
                *STATUS=-1;
cout<<"Temporary printout from findmaximaDFiR  : too many cells (>= "<<MAXElementsOverThresholdinHough<<")  above MINIMUMCOUNTS (= "<<
     MINIMUMCOUNTS<<")"<<endl;
                return ;
             }
         }

       }
      }
     }






    if ( icount == 0) {
       * NumberofMaximaDFiR = 0;
       *STATUS=1;
       return;
    } else if (icount == 1 ) {
       ntotClusters=1;
       nElementsinCluster[0]=1;
       for(i=0; i<3; i++){
         MaximaIndexesDFiR[0][i] =auxDFiRIndex[0][i];
       }
       * NumberofMaximaDFiR = 1;
       *STATUS=1;
       return;
    }

//   case with more that 1 elements over threshold ---------------------------------------------------------------


    ntotClusters=0;

    for(i=0; i<3; i++){
       Cluster[0][i] = auxDFiRIndex[0][i];
    }


    for(i=1; i<icount; i++){
     for(j=0; j<3; j++){
       Remai[i-1][j] = auxDFiRIndex[i][j];
     }
    }
    nRemai = icount-1;
    NinCluster=1;

    while(1){
      i=0;
      while(i<NinCluster && nRemai >0) {
        clustering3(
           (UShort_t *) (&Cluster[i][0]),  //   inputs to function clustering3
           nRemai, Remai,               //   inputs to function clustering3
           nClusterElementsFound, ClusterElementsFound,  //   ouputs from function clustering3
           nRemainingElements, RemainingElements       //   ouputs from function clustering3
                   );
        for(j=0; j<nClusterElementsFound; j++){
           Cluster[NinCluster+j][0] = ClusterElementsFound[j][0];
           Cluster[NinCluster+j][1] = ClusterElementsFound[j][1];
           Cluster[NinCluster+j][2] = ClusterElementsFound[j][2];
        }
        NinCluster += nClusterElementsFound;
        nRemai=nRemainingElements;
        for(j=0; j<nRemainingElements; j++){
           Remai[j][0] = RemainingElements[j][0];
           Remai[j][1] = RemainingElements[j][1];
           Remai[j][2] = RemainingElements[j][2];
        }
        i++;
      }   //  end of  while(i<NinCluster && nRemai >0)

      nElementsinCluster[ntotClusters]= NinCluster;
      for( j=0; j<NinCluster; j++) {
       for(i=0; i<3; i++){
          found[ntotClusters][j][i] = Cluster[j][i];
       }
      }
      ntotClusters ++;

      if(nRemai>1){
        NinCluster=1;
        for(i=0; i<3; i++){
          Cluster[0][i] = Remai[0][i];
        }

        nRemai--;
        for(j=0;j<nRemai;j++){
          for(i=0; i<3; i++){
            Remai[j][i]=Remai[j+1][i];
          }
        }


      } else if (nRemai==1) {
        nElementsinCluster[ntotClusters]= 1;
        for(i=0; i<3; i++){
            found[ntotClusters][0][i] = Remai[0][i];
        }
        ntotClusters ++;
        break;
      } else {  // this is the case when nRemai == 0
        break;
      }  //  endo of  if(nRemai>1)

    }   //   end   of   while(1)



//  now find the indeces for the maxima

    *NumberofMaximaDFiR = ntotClusters;


    for(i=0; i<ntotClusters; i++){
       for(j=0, max=-1;j<nElementsinCluster[i];j++){
          if(max < BoxDFiR[found[i][j][0]][found[i][j][1]][found[i][j][2]]) {
            max=BoxDFiR[found[i][j][0]][found[i][j][1]][found[i][j][2]];
            MaximaIndexesDFiR[i][0]=found[i][j][0];
            MaximaIndexesDFiR[i][1]=found[i][j][1];
            MaximaIndexesDFiR[i][2]=found[i][j][2];
          }
       }
    }


    *STATUS=1;

}


//----------end of function PndSttTrackFinderReal::findmaximaDFiR



  void PndSttTrackFinderReal::findmaximaKFI0(
                    UShort_t BoxKFI0[nbinKAPPA][nbinFI0],
                    Int_t MINIMUMCOUNTS,
                    Int_t *NumberofMaximaKFI0,
                    Int_t  MaximaIndexesKFI0[][2],
                    Int_t * STATUS)
{


     Int_t      i, j, iK, iFI0, icount,
                ntotClusters,
                NinCluster,
                nClusterElementsFound,
                nRemai,
                nRemainingElements,
                max,
                nElementsinCluster[MAXElementsOverThresholdinHough];

     UShort_t   found[MAXElementsOverThresholdinHough][MAXElementsOverThresholdinHough][2],
                auxKFI0Index[MAXElementsOverThresholdinHough][2],
                Remai[MAXElementsOverThresholdinHough][2],
                RemainingElements[MAXElementsOverThresholdinHough][2],
                Cluster[MAXElementsOverThresholdinHough][2],
                ClusterElementsFound[MAXElementsOverThresholdinHough][2];


     icount=0;
     for(iK=0; iK<nbinKAPPA; iK++){
      for(iFI0=0; iFI0<nbinFI0; iFI0++){
// cout<<"MINIMUMCOUNTS = "<<MINIMUMCOUNTS<<";  Box = "<<BoxKFI0[iK][iFI0]<< endl;
         if(
              BoxKFI0[iK][iFI0] > MINIMUMCOUNTS
           ) {
             auxKFI0Index[icount][0]=iK;
             auxKFI0Index[icount][1]=iFI0;
             icount++;
             if(icount == MAXElementsOverThresholdinHough){
                *STATUS=-1;
                return ;
             }
         }

      }
     }







    if ( icount == 0) {
       *NumberofMaximaKFI0 = 0;
       *STATUS=1;
       return;
    } else if (icount == 1 ) {
       ntotClusters=1;
       nElementsinCluster[0]=1;
       for(i=0; i<2; i++){
         MaximaIndexesKFI0[0][i] =auxKFI0Index[0][i];
       }
       *STATUS=1;
       return;
    }

//   case with more that 1 elements over threshold ---------------------------------------------------------------


    ntotClusters=0;

    for(i=0; i<2; i++){
       Cluster[0][i] = auxKFI0Index[0][i];
    }


    for(i=1; i<icount; i++){
     for(j=0; j<2; j++){
       Remai[i-1][j] = auxKFI0Index[i][j];
     }
    }
    nRemai = icount-1;
    NinCluster=1;

    while(1){
      i=0;
      while(i<NinCluster && nRemai >0) {
        clustering2(
           (UShort_t *) (&Cluster[i][0]),  //   inputs to function clustering2
           nRemai, Remai,               //   inputs to function clustering2
           nClusterElementsFound, ClusterElementsFound,  //   ouputs from function clustering2
           nRemainingElements, RemainingElements       //   ouputs from function clustering2
                   );
        for(j=0; j<nClusterElementsFound; j++){
           Cluster[NinCluster+j][0] = ClusterElementsFound[j][0];
           Cluster[NinCluster+j][1] = ClusterElementsFound[j][1];
        }
        NinCluster += nClusterElementsFound;
        nRemai=nRemainingElements;
        for(j=0; j<nRemainingElements; j++){
           Remai[j][0] = RemainingElements[j][0];
           Remai[j][1] = RemainingElements[j][1];
        }
        i++;
      }   //  end of  while(i<NinCluster && nRemai >0)

      nElementsinCluster[ntotClusters]= NinCluster;
      for( j=0; j<NinCluster; j++) {
       for(i=0; i<2; i++){
          found[ntotClusters][j][i] = Cluster[j][i];
       }
      }
      ntotClusters ++;

      if(nRemai>1){
        NinCluster=1;
        for(i=0; i<2; i++){
          Cluster[0][i] = Remai[0][i];
        }

        nRemai--;
        for(j=0;j<nRemai;j++){
          for(i=0; i<2; i++){
            Remai[j][i]=Remai[j+1][i];
          }
        }


      } else if (nRemai==1) {
        nElementsinCluster[ntotClusters]= 1;
        for(i=0; i<2; i++){
            found[ntotClusters][0][i] = Remai[0][i];
        }
        ntotClusters ++;
        break;
      } else {  // this is the case when nRemai == 0
        break;
      }  //  endo of  if(nRemai>1)

    }   //   end   of   while(1)



//  now find the indeces for the maxima

    *NumberofMaximaKFI0 = ntotClusters;


    for(i=0; i<ntotClusters; i++){

       for(j=0, max=-1;j<nElementsinCluster[i];j++){
          if(max < BoxKFI0[found[i][j][0]][found[i][j][1]]) {
            max=BoxKFI0[found[i][j][0]][found[i][j][1]];
            MaximaIndexesKFI0[i][0]=found[i][j][0];
            MaximaIndexesKFI0[i][1]=found[i][j][1];
          }
       }
    }


    *STATUS=1;

}


//----------end of function PndSttTrackFinderReal::findmaximaKFI0



  bool  PndSttTrackFinderReal::iscontiguous(
                int ncomponents, UShort_t * vec1, UShort_t *vec2)
{

    for(int i=0; i<ncomponents;i++){
      if( vec1[i]-vec2[i] >3 || vec1[i]-vec2[i]<-3) return false;
    }

    return true;

}



//----------end of function PndSttTrackFinderReal::iscontiguous

void PndSttTrackFinderReal::clustering2(
          UShort_t vec1[2],                                       // input
          int nListElements, UShort_t List[][2],                 // input
          int & nClusterElementsFound, UShort_t ClusterElementsFound[][2],  // output
          int & nRemainingElements, UShort_t  RemainingElements[][2]     // output
                )
{
   int i;
   UShort_t  vec2[2];

   nClusterElementsFound=0;
   nRemainingElements=0;
   for(i=0; i<nListElements; i++){
      vec2[0]=List[i][0];
      vec2[1]=List[i][1];
      if( iscontiguous(2, vec1, vec2) ){
         ClusterElementsFound[nClusterElementsFound][0] = vec2[0];
         ClusterElementsFound[nClusterElementsFound][1] = vec2[1];
         nClusterElementsFound++;
      } else {
         RemainingElements[nRemainingElements][0] = vec2[0];
         RemainingElements[nRemainingElements][1] = vec2[1];
         nRemainingElements++;
      }
   }

    return;
}



//----------end of function PndSttTrackFinderReal::clustering2

void PndSttTrackFinderReal::clustering3 (
          UShort_t vec1[3],                                       // input
          int nListElements, UShort_t List[][3],                 // input
          int & nClusterElementsFound, UShort_t ClusterElementsFound[][3],  // output
          int & nRemainingElements, UShort_t  RemainingElements[][3]     // output
                )
{
   int i;
   UShort_t  vec2[3];

   nClusterElementsFound=0;
   nRemainingElements=0;
   for(i=0; i<nListElements; i++){
      vec2[0]=List[i][0];
      vec2[1]=List[i][1];
      vec2[2]=List[i][2];
      if( iscontiguous(3, vec1, vec2) ){
         ClusterElementsFound[nClusterElementsFound][0] = vec2[0];
         ClusterElementsFound[nClusterElementsFound][1] = vec2[1];
         ClusterElementsFound[nClusterElementsFound][2] = vec2[2];
         nClusterElementsFound++;
      } else {
         RemainingElements[nRemainingElements][0] = vec2[0];
         RemainingElements[nRemainingElements][1] = vec2[1];
         RemainingElements[nRemainingElements][2] = vec2[2];
         nRemainingElements++;
      }
   }

    return;
}



//----------end of function PndSttTrackFinderReal::clustering3



void PndSttTrackFinderReal::WriteHistograms(){

//     TFile* file = FairRootManager::Instance()->GetOutFile();
          TFile* file = FairRootManager::Instance()->GetOutFile();
 	  file->cd();
 	  file->mkdir("PndSttTrackFinderReal");
 	  file->cd("PndSttTrackFinderReal");

          hx->Write();
 	  delete hx;

}

//----------end of function PndSttTrackFinderReal::WriteHistograms




//----------start of function PndSttTrackFinderReal::WriteMacroParallelHitsGeneral

  void PndSttTrackFinderReal::WriteMacroParallelHitsGeneral(
                   Int_t Nhits, Double_t info[][6], Int_t Nincl, Int_t Mincl[], Double_t inclination[][3]
                                                     )
{

    Int_t i, j, i1, ii, index, Kincl, nlow, nup, STATUS;

    Double_t xmin , xmax, ymin, ymax, Ox, Oy,
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





//    Ox = (D+R)*cos(Fi);
//    Oy = (D+R)*sin(Fi);




//---------- parallel straws Macro now
      char nome[50], nome2[50];
      sprintf(nome,"MacroGeneralParallelHitsEvent%d", IVOLTE);
      sprintf(nome2,"%s.C",nome);
      FILE * MACRO = fopen(nome2,"w");
      fprintf(MACRO,"void %s()\n{\n",nome);
      xmin=1.e20;
      xmax=-1.e20;
      ymin=1.e20;
      ymax=-1.e20;
       for( i=1; i< Nhits; i++) {
         if( info[i][5] == 1 ) {     // parallel straws
            if (info[i][0]-info[i][3] < xmin)   xmin = info[i][0]-info[i][3];
            if (info[i][0]+info[i][3] > xmax)   xmax = info[i][0]+info[i][3];
            if (info[i][1]-info[i][3] < ymin)   ymin = info[i][1]-info[i][3];
            if (info[i][1]+info[i][3] > ymax)   ymax = info[i][1]+info[i][3];
          }
       }

       if( xmin > 0. ) xmin = 0.;
       if( xmax < 0.)  xmax = 0.;
       if( ymin > 0. ) ymin = 0.;
       if( ymax < 0.)  ymax = 0.;

       deltax = xmax-xmin;
       deltay = ymax - ymin;

       if( deltax > deltay) {
         ymin -=  0.5*(deltax-deltay);
         ymax = ymin+ deltax;
         delta = deltax;
       }  else  {
         xmin -=  0.5*(deltay-deltax);
         xmax = xmin+ deltay;
         delta= deltay;
       }

       xmax = xmax + delta*0.05;
       xmin = xmin - delta*0.05;

       ymax = ymax + delta*0.05;
       ymin = ymin - delta*0.05;


       fprintf(MACRO,"TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n",xmin,ymin,xmax,ymax);

//       fprintf(MACRO,"TEllipse* TC = new TEllipse(%f,%f,%f,%f,0.,360.);\n",Ox,Oy,R,R);
//       fprintf(MACRO,"TC->SetLineColor(4);\nTC->Draw();\n");

       fprintf(MACRO,"TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n",xmin,0.,xmax,0.,xmin,xmax);
       fprintf(MACRO,"Assex->Draw();\n");
       fprintf(MACRO,"TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", 0.,ymin,0.,ymax,ymin,ymax);
       fprintf(MACRO,"Assey->Draw();\n");


       for( i=0; i< Nhits; i++) {
         if( info[i][5] == 1 ) {     // parallel straws
            fprintf(MACRO,"TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nE%d->Draw();\n",
                     i,info[i][0],info[i][1],info[i][3],info[i][3],i);
          }
       }

      fprintf(MACRO,"}\n");
      fclose(MACRO);
       

    return ;

}


//----------end of function PndSttTrackFinderReal::WriteMacroParallelHitsGeneral




//----------start of function PndSttTrackFinderReal::WriteMacroParallelAssociatedHits

  void PndSttTrackFinderReal::WriteMacroParallelAssociatedHits(
                   Double_t D,Double_t Fi,Double_t R,
                   Int_t Nhits, Double_t info[][6], Int_t Nincl, Int_t Mincl[], Double_t inclination[][3],
                   Int_t imaxima
                                                     )
{

    Int_t i, j, i1, ii, index, Kincl, nlow, nup, STATUS;

    Double_t xmin , xmax, ymin, ymax, Ox, Oy,
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





    Ox = (D+R)*cos(Fi);
    Oy = (D+R)*sin(Fi);




//---------- parallel straws Macro now
      char nome[50], nome2[50];
      sprintf(nome,"MacroPMaxN%dParallelHitsSummary%d",imaxima, IVOLTE);
      sprintf(nome2,"%s.C",nome);
      FILE * MACRO = fopen(nome2,"w");
      fprintf(MACRO,"void %s()\n{\n",nome);
      xmin=1.e20;
      xmax=-1.e20;
      ymin=1.e20;
      ymax=-1.e20;
       for( i=1; i< Nhits; i++) {
         if( info[i][5] == 1 ) {     // parallel straws
            if (info[i][0]-info[i][3] < xmin)   xmin = info[i][0]-info[i][3];
            if (info[i][0]+info[i][3] > xmax)   xmax = info[i][0]+info[i][3];
            if (info[i][1]-info[i][3] < ymin)   ymin = info[i][1]-info[i][3];
            if (info[i][1]+info[i][3] > ymax)   ymax = info[i][1]+info[i][3];
          }
       }

       if( xmin > 0. ) xmin = 0.;
       if( xmax < 0.)  xmax = 0.;
       if( ymin > 0. ) ymin = 0.;
       if( ymax < 0.)  ymax = 0.;

       deltax = xmax-xmin;
       deltay = ymax - ymin;

       if( deltax > deltay) {
         ymin -=  0.5*(deltax-deltay);
         ymax = ymin+ deltax;
         delta = deltax;
       }  else  {
         xmin -=  0.5*(deltay-deltax);
         xmax = xmin+ deltay;
         delta= deltay;
       }

       xmax = xmax + delta*0.05;
       xmin = xmin - delta*0.05;

       ymax = ymax + delta*0.05;
       ymin = ymin - delta*0.05;


       fprintf(MACRO,"TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n",xmin,ymin,xmax,ymax);

       fprintf(MACRO,"TEllipse* TC = new TEllipse(%f,%f,%f,%f,0.,360.);\n",Ox,Oy,R,R);
       fprintf(MACRO,"TC->SetLineColor(4);\nTC->Draw();\n");

       fprintf(MACRO,"TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n",xmin,0.,xmax,0.,xmin,xmax);
       fprintf(MACRO,"Assex->Draw();\n");
       fprintf(MACRO,"TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", 0.,ymin,0.,ymax,ymin,ymax);
       fprintf(MACRO,"Assey->Draw();\n");


       for( i=0; i< Nhits; i++) {
         if( info[i][5] == 1 ) {     // parallel straws
            fprintf(MACRO,"TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nE%d->Draw();\n",
                     i,info[i][0],info[i][1],info[i][3],info[i][3],i);
          }
       }

      fprintf(MACRO,"}\n");
      fclose(MACRO);
       

    return ;

}


//----------end of function PndSttTrackFinderReal::WriteMacroParallelAssociatedHits



  void PndSttTrackFinderReal::WriteMacroSkewAssociatedHits(
                   Double_t KAPPA,Double_t FI0,Double_t D,Double_t Fi,Double_t R,
                   Int_t Nhits, Double_t info[][6], Int_t Nincl, Int_t Mincl[], Double_t inclination[][3],
                   Int_t imaxima, Int_t nMaxima 
                                                     )
 {



    Int_t i, j, i1, ii, index, Kincl, nlow, nup, STATUS;

    Double_t xmin , xmax, ymin, ymax, Ox, Oy,
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





//-------------------  skew straws hits Macro now

      char  nome2[50];
      FILE *MACRO;
      sprintf(nome2,  "MacroPMaxN%dSMaxN%dSkewHitsSummary%d.C",imaxima+1,nMaxima+1, IVOLTE);
      MACRO = fopen(nome2,"w");
      sprintf(nome2,  "MacroPMaxN%dSMaxN%dSkewHitsSummary%d",imaxima+1,nMaxima+1, IVOLTE);
      fprintf(MACRO,"void %s()\n{\n",nome2);

//KAPPA = 1./166.67 ;  FI0 = 1.5*PI;

      Smin=zmin = 1.e10;
      Smax=zmax = -zmin;
      index=0;

       for( i=1; i< Nhits; i++) {
         if( info[i][5] == 1. )   continue;     // exclude parallel straws

         Kincl = (int) info[i][5] - 1;


         aaa = sqrt(inclination[Kincl][0]*inclination[Kincl][0]+inclination[Kincl][1]*inclination[Kincl][1]+
                  inclination[Kincl][2]*inclination[Kincl][2]);
         vx1 = inclination[Kincl][0]/aaa;
         vy1 = inclination[Kincl][1]/aaa;
         vz1 = inclination[Kincl][2]/aaa;
         C0x1 = info[i][0];
         C0y1 = info[i][1];
         C0z1 = info[i][2];
         Ox = (R+D)*cos(Fi);
         Oy = (R+D)*sin(Fi);
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



// checks that the projected ellipsis doesn't go out the boundaries of both the skew straw and the trajectory cylinder

        if(
          fabs(POINTS1[j+2]-ZCENTER_STRAIGHT) > SEMILENGTH_STRAIGHT- Aellipsis1 ||
          distance + bbb > info[i][4]        //  the ellipsis goes out of the boundaries of the skew straw
          )  continue;
//--------------------------


        fi1 = atan2(POINTS1[j+1]-Oy, POINTS1[j]-Ox) ;  // atan2 returns radians in (-pi and +pi]
        if( fi1 < 0.) fi1 += 2.*PI;

        if( zmin > POINTS1[j+2] - Aellipsis1 ) zmin = POINTS1[j+2] - Aellipsis1;
        if( zmax < POINTS1[j+2] + Aellipsis1 ) zmax = POINTS1[j+2] + Aellipsis1;

        if( Smin > fi1 - Bellipsis1 ) Smin = fi1 - Bellipsis1;
        if( Smax < fi1 + Bellipsis1 ) Smax = fi1 + Bellipsis1;


        Double_t rotation1 = 180.*atan2(Tiltdirection1[1],Tiltdirection1[0])/PI;
        fprintf(MACRO,"TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.,%f);\n",
                     index,POINTS1[j+2],fi1,Aellipsis1,Bellipsis1,rotation1);
        index++;

   }    //  end of    for( ii=0; ii<2; ii++)

  }   //   end of  for( i=1; i< Nhits; i++)


  if(index==0) goto nohits ;
  if( zmax < zmin ) goto nohits ;
  if( Smax < Smin ) goto nohits;
  aaa = Smax-Smin;
  Smin -= aaa*0.05;
  Smax += aaa*0.05;

  aaa = zmax-zmin;
  zmin -= aaa*0.05;
  zmax += aaa*0.05;

  if(Smax > 2.*PI) Smax = 2.*PI;
  if( Smin < 0.) Smin = 0.;

  if ( KAPPA >= 0.) {
     fmin = KAPPA*zmin + FI0;
     fmax = KAPPA*zmax + FI0;
  }  else {
     fmax = KAPPA*zmin + FI0;
     fmin = KAPPA*zmax + FI0;
  }

  j = (int) (0.5*fmax/ PI);
  i = (int) (0.5*fmin/PI);
  fmin -= i*2.*PI;
  if(fmin < 0.) {
    fmin += 2.*PI;
    fmax -= (i-1)*2.*PI;
    offset = (i-1)*2.*PI;
    j -= (i-1) ;
  } else {
    fmax -= i*2.*PI;
    offset = i*2.*PI;
    j -= i;
  }

  if ( j == 0){
    if( Smax < fmax)   Smax = fmax; 
    if( Smin > fmin)   Smin = fmin; 
  } else if (j > 0) {
    Smax = 2.*PI;
    Smin = 0.;
  }




  fprintf(MACRO,"TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n",zmin,Smin,zmax,Smax);
  for( ii=0; ii< index; ii++) {
       fprintf(MACRO,"E%d->Draw();\n",ii);
  }






  if( j == 0 ) {


          fprintf(MACRO,"TLine* FOUND = new TLine(%f,%f,%f,%f);\nFOUND->SetLineColor(2);\nFOUND->Draw();\n",
                 zmin,KAPPA*zmin+FI0-offset,zmax, KAPPA*zmax+FI0-offset);

  } else {

          zl[0]=zmin;
          zu[j]=zmax;
          for( ii = 0; ii < j ; ii++){
          zu[ii]=zl[ii+1]= ((ii+1)*2.*PI + offset - FI0)/KAPPA;
  }
    for( ii = 0; ii < j+1 ; ii++){
          fprintf(MACRO,"TLine* FOUND%d = new TLine(%f,%f,%f,%f);\nFOUND%d->SetLineColor(2);\nFOUND%d->Draw();\n",
           ii, zl[ii] , KAPPA*zl[ii]+FI0-offset-ii*2.*PI , zu[ii], KAPPA*zu[ii]+FI0-offset-ii*2.*PI ,ii,ii);
  }


 }    //   end of    if(j == 0 )


   deltaz = zmax-zmin;
   deltaS = Smax-Smin;
   fprintf(MACRO,"TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n",
        zmin+0.05*deltaz,Smin+0.05*deltaS,zmax-0.05*deltaz,Smin+0.05*deltaS,zmin+0.05*deltaz,zmax-0.05*deltaz);
   fprintf(MACRO,"Assex->Draw();\n");
   fprintf(MACRO,"TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n",
        zmin+0.05*deltaz,Smin+0.05*deltaS,zmin+0.05*deltaz,Smax-0.05*deltaS,Smin+0.05*deltaS,Smax-0.05*deltaS);
   fprintf(MACRO,"Assey->Draw();\n");


nohits: ;

      fprintf(MACRO,"}\n");
      fclose(MACRO);





 }

//----------end of function PndSttTrackFinderReal::WriteMacroSkewAssociatedHits



ClassImp(PndSttTrackFinderReal)

    

