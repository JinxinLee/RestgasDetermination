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
		TClonesArray *SttTubeArray, // input; Stt tube position TClonesArray;
		int nParContigue[4542],  // output; number of contiguous straws (axial Stt);
		int ListParContiguous[4542][6]  // output list (axial Stt); maximum 6 adjacencies;
			);

  ClassDef(PndTrkSttAdjacencies,1);

};

#endif
