#ifndef PndTrkCategorizeStt_H
#define PndTrkCategorizeStt_H 1

#include "TClonesArray.h"


class PndTrkCategorizeStt
{

 public:

 /** Default constructor **/
 PndTrkCategorizeStt();

 /** Destructor **/
 virtual ~PndTrkCategorizeStt();

 void CategorizeStt(
	// in this geametry there are 4542 Stt Straws (axial and skew);
		const Short_t NUMBER_STRAWS,  // number of Stt Straws in total;
		TClonesArray *SttTubeArray, // input; array of the Stt tubes;
		Short_t &nAxialOuterRight,  // output; number of axial Stt, outer, on the right (looking into the beam);
		Short_t &nAxialInnerRight,  // output; number of axial Stt, inner, on the right (looking into the beam);
		Short_t &nAxialOuterLeft,  // output; number of axial Stt, outer, on the left (looking into the beam);
		Short_t &nAxialInnerLeft,  // output; number of axial Stt, inner, on the left (looking into the beam);

		Short_t *ListAxialOuterRight,  // output; list of axial Stt, outer, on the right (looking into the beam);
		Short_t *ListAxialInnerRight,  // output; list of axial Stt, inner, on the lright (looking into the beam);
		Short_t *ListAxialOuterLeft,  // output; list of axial Stt, outer, on the left (looking into the beam);
		Short_t *ListAxialInnerLeft,  // output; list of axial Stt, inner, on the left (looking into the beam);

		Short_t &nSkewRight,  // output; number of skew Stt, on the right (looking into the beam);
		Short_t &nSkewLeft,  // output; number of skew Stt, on the right (looking into the beam);
		Short_t *ListSkewRight,  // output; list of axial Stt, inner, on the lright (looking into the beam);
		Short_t *ListSkewLeft  // output; list of axial Stt, outer, on the left (looking into the beam);
			);



  ClassDef(PndTrkCategorizeStt,1);

};

#endif
