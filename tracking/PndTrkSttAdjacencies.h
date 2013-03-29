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
		TClonesArray *SttTubeArray
			);

  ClassDef(PndTrkSttAdjacencies,1);

};

#endif
