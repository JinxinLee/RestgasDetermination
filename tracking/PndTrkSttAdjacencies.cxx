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
			TClonesArray *SttTubeArray, // input; array of the Stt tubes;
			int nParContigue[4542],  // output; number of contiguous straws (axial Stt);
			int ListParContiguous[4542][6]  // output list (axial Stt);
						)
 {
	int i,j,n, ncontigue;

	int	nParContigue[4542],
		ListParContiguous[4542][6]; // al massimo ci sono 6 contigue;
	FILE *HAND = fopen("ListaSttStraw.lis","w");

	double dis2, x[4542], y[4542], z[4542];
	TVector3 center[4542], wiredirection[4542];
	PndSttTube *pSttTube;

	// the total n. of STT straws is 4542;
	for (i=1;i<= 4542; i++){
		pSttTube = (PndSttTube *) SttTubeArray->At(i);
		x[i-1] = pSttTube->GetPosition().X();
		y[i-1] = pSttTube->GetPosition().Y();
		z[i-1] = pSttTube->GetPosition().Z();
		wiredirection[i-1] = pSttTube->GetWireDirection();
	}


	for (i=0;i< 4542; i++){
		if( fabs(wiredirection[i].Z() -1.) > 1.e-5) continue; // non considero le skew;

		nParContigue[i]= 0;
		for(j=0;j<4542; j++){
			if(j==i) continue;
			if( fabs(wiredirection[j].Z() -1.) > 1.e-5) continue; // non considero le skew;
			dis2 = ( x[i]-x[j])*( x[i]-x[j]) +
				(y[i]-y[j])*(y[i]-y[j]);
			if(dis2>1.1) continue;
			ListParContiguous[i][nParContigue[i]] = j+1;
			nParContigue[i]++;
			if( nParContigue[i]>6 ) { cout<<"Errore ! N contigue Par > 6!!! Exit.\n"; exit(-1);}
		}

		fprintf(HAND,"Stt straw n. %d ",i+1);
		fprintf(HAND,", X %g, Y %g, Z %g ",
		x[i],y[i],z[i]);
		if(wiredirection[i].Z() <0.) {
			fprintf(HAND," dirX %g , dirY %g , dirZ %g ; straw contigue = %d ;",
			-wiredirection[i].X(),-wiredirection[i].Y(),-wiredirection[i].Z(),
			nParContigue[i]);
		}else{
			fprintf(HAND," dirX %g , dirY %g , dirZ %g ; straw contigue = %d ;",
			wiredirection[i].X(),wiredirection[i].Y(),wiredirection[i].Z(),
			nParContigue[i]);
		}

		if( nParContigue[i]>0 ) {
			fprintf(HAND," lista :");
			for(n=0;n<nParContigue[i];n++){
				fprintf(HAND," %d",ListParContiguous[i][n]);
			}
			fprintf(HAND,";\n");
		} else {
			fprintf(HAND,"\n");
		}
	}

	fclose (HAND);
 }
//------------------ end function  PndTrkSttAdjacencies::CalculateAdjacentStt

ClassImp(PndTrkSttAdjacencies)

