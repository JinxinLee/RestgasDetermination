#ifndef PndTrkSttAdjacencies_H
#define PndTrkSttAdjacencies_H 1

#include "TClonesArray.h"


class PndTrkSttAdjacencies
{

 public:

 /** Default constructor **/
 PndTrkSttAdjacencies();

 /** Destructor **/
 ~PndTrkSttAdjacencies();

 void CalculateAdjacentStt(
	// in this geametry there are 4542 Stt Straws (axial and skew);
		const Short_t NUMBER_STRAWS,  // number of Stt Straws in total;
		TClonesArray *SttTubeArray, // input; array of the Stt tubes;
		Short_t *nParContiguous,  // output; number of contiguous straws (axial Stt);
				// NUMBER_STRAWS+1 because the numbering scheme for the Stt straws goes
				//  from 1 to NUMBER_STRAWS included;
		Short_t ListParContiguous[][6]  // output list (axial Stt);
			);




 void CalculateAdjacentStt2(
	// in this geametry there are 4542 Stt Straws (axial and skew);
		const Short_t NUMBER_STRAWS,  // number of Stt Straws in total;
		TClonesArray *SttTubeArray, // input; array of the Stt tubes;
		Short_t *nParContiguous,  // output; number of contiguous straws (axial Stt);
				// NUMBER_STRAWS+1 because the numbering scheme for the Stt straws goes
				//  from 1 to NUMBER_STRAWS included;
		Short_t ListParContiguous[][6],  // output list (axial Stt);
		Double_t *x,	// X position center of tube;
		Double_t *y,	// Y position center of tube;
		Double_t *z,	// Z position center of tube;
		Double_t *rxy2	// X*X+Y*Y position center of tube;
			);

  ClassDef(PndTrkSttAdjacencies,1);

};

#endif
