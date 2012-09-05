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

 void stampaMvdHits(
	char *fMvdPixelBranch,
	char *fMvdStripBranch,
	Short_t nMvdPixelHit,
	Short_t nMvdStripHit,
	Double_t *refindexMvdPixel,
	Double_t *refindexMvdStrip,
	Double_t *sigmaXMvdPixel,
	Double_t *sigmaXMvdStrip,
	Double_t *sigmaYMvdPixel,
	Double_t *sigmaYMvdStrip,
	Double_t *sigmaZMvdPixel,
	Double_t *sigmaZMvdStrip,
	Double_t *XMvdPixel,
	Double_t *XMvdStrip,
	Double_t *YMvdPixel,
	Double_t *YMvdStrip,
	Double_t *ZMvdPixel,
	Double_t *ZMvdStrip
	);




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


 void stampetta(
	int IVOLTE,
	bool *keepit,
	Short_t *ListMvdPixelHitsinTrack,
	Short_t *ListMvdStripHitsinTrack,
	Short_t *ListSttParHitsinTrack,
	Short_t *ListSttSkewHitsinTrack,
	Short_t *ListSciTilHitsinTrack,
	Short_t *nMvdPixelHitsinTrack,
	Short_t *nMvdStripHitsinTrack,
	Short_t *nSttParHitsinTrack,
	Short_t *nSttSkewHitsinTrack,
	Short_t *nSciTilHitsinTrack,
	Short_t nSttTrackCand,
	int MAXMVDPIXELHITSINTRACK,
	int MAXMVDSTRIPHITSINTRACK,
	int MAXSCITILHITSINTRACK,
	int MAXSTTHITSINTRACK,
	Double_t *R,
	Double_t *Ox,
	Double_t *Oy,
	Double_t *FI0,
	Double_t *KAPPA
	);



  ClassDef(PndTrkPrintouts,1);

};

#endif
