#ifndef PndTrkPrintouts_H
#define PndTrkPrintouts_H 1

#include "FairMCPoint.h"
#include "PndSttTube.h"
// Root includes
#include "TROOT.h"


class PndTrkPrintouts : public TObject
{


 public:

 /** Default constructor **/
 PndTrkPrintouts(){};
 /** Destructor **/
 ~PndTrkPrintouts(){};

void stampaSttHits(
	Short_t iHit,
	Short_t ipunto,
	Double_t dradius,
	Double_t *WDX,
	Double_t *WDY,
	Double_t *WDZ,
	FairMCPoint* puntator,
	PndSttTube * pSttTube
	);





  ClassDef(PndTrkPrintouts,1);

};

#endif
