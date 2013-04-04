#include "PndTrkSttAdjacencies.h"

#include "PndSttTube.h"

#include "TClonesArray.h"
#include "TVector3.h"

#include <stdlib.h>
#include <iostream>
#include <cmath>

using namespace std;


 /** Default constructor **/
 PndTrkSttAdjacencies::PndTrkSttAdjacencies(){};

 /** Destructor **/
 PndTrkSttAdjacencies::~PndTrkSttAdjacencies(){};

//-----------------------------------------------------------

 void PndTrkSttAdjacencies::CalculateAdjacentStt(
		const Short_t NUMBER_STRAWS,  // number of Stt Straws in total;
		TClonesArray *SttTubeArray, // input; array of the Stt tubes;
		Short_t *nParContiguous,  // output; number of contiguous straws (axial Stt);
				// NUMBER_STRAWS+1 because the numbering scheme for the Stt straws goes
				//  from 1 to NUMBER_STRAWS included;
		Short_t ListParContiguous[][6]  // output list (axial Stt);
						)
 {
	int i,j,n, ncontigue;

//	FILE *HAND = fopen("ListaSttStraw.lis","w");

	double
		dis2,
		x[NUMBER_STRAWS],
		Xwiredirection[NUMBER_STRAWS],
		y[NUMBER_STRAWS],
		Ywiredirection[NUMBER_STRAWS],
		z[NUMBER_STRAWS],
		Zwiredirection[NUMBER_STRAWS];

	PndSttTube *pSttTube;

	// the total n. of STT straws is NUMBER_STRAWS;
	for (i=1;i<= NUMBER_STRAWS; i++){
		pSttTube = (PndSttTube *) SttTubeArray->At(i);
		x[i-1] = pSttTube->GetPosition().X();
		y[i-1] = pSttTube->GetPosition().Y();
		z[i-1] = pSttTube->GetPosition().Z();
		Xwiredirection[i-1] = pSttTube->GetWireDirection().X();
		Ywiredirection[i-1] = pSttTube->GetWireDirection().Y();
		Zwiredirection[i-1] = pSttTube->GetWireDirection().Z();
	}


	for (i=0;i< NUMBER_STRAWS; i++){
		if( fabs(Zwiredirection[i] -1.) > 1.e-5) continue; // for now I don't consider the skews;

		nParContiguous[i+1]= 0;
		for(j=0;j<NUMBER_STRAWS; j++){
			if(j==i) continue;
			if( fabs(Zwiredirection[j] -1.) > 1.e-5) continue; // for now I don't consider the skews;
			dis2 = ( x[i]-x[j])*( x[i]-x[j]) +
				(y[i]-y[j])*(y[i]-y[j]);
			if(dis2>1.1) continue;
			ListParContiguous[i+1][nParContiguous[i+1]] = j+1;
			nParContiguous[i+1]++;
			if( nParContiguous[i+1]>6 ) { cout<<"Errore ! N contigue Par > 6!!! Exit.\n"; exit(-1);}
		}


/*
		fprintf(HAND,"Stt straw n. %d ",i+1);
		fprintf(HAND,", X %g, Y %g, Z %g ",
		x[i],y[i],z[i]);

		if(Zwiredirection[i] <0.) {
			fprintf(HAND," dirX %g , dirY %g , dirZ %g ; straw contigue = %d ;",
			-Xwiredirection[i],-Ywiredirection[i],Zwiredirection[i],
			nParContiguous[i+1]);
		}else{
			fprintf(HAND," dirX %g , dirY %g , dirZ %g ; straw contigue = %d ;",
			Xwiredirection[i],Ywiredirection[i],Zwiredirection[i],
			nParContiguous[i+1]);
		}


		if( nParContiguous[i+1]>0 ) {
			fprintf(HAND," lista :");
			for(n=0;n<nParContiguous[i+1];n++){
				fprintf(HAND," %d",ListParContiguous[i+1][n]);
			}
			fprintf(HAND,";\n");
		} else {
			fprintf(HAND,"\n");
		}
*/

	}

//	fclose (HAND);

 }
//------------------ end function  PndTrkSttAdjacencies::CalculateAdjacentStt

ClassImp(PndTrkSttAdjacencies)

