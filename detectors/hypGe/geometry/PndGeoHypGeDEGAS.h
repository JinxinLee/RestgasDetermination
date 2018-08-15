// ******** Header of class to build HypGe Triple Cluster Geometry *************
//					by steinen@kph.uni-mainz.de

#ifndef PndGeoHypGeDEGAS_H
#define PndGeoHypGeDEGAS_H

#include "PndGeoHypGeCrystal.h"
#include "PndGeoHypGeCluster.h"
#include "TGeoArb8.h"
#include "TGeoXtru.h"

class  PndGeoHypGeDEGAS : public PndGeoHypGeCluster
{
private:
  Double_t 						TripleCrystalXOffset;
	Double_t 						TripleCrystalYOffset;
	Double_t 						TripleCrystalZOffset;
public:
  PndGeoHypGeDEGAS();			// do not use!
  PndGeoHypGeDEGAS(TGeoMedium *ExtGe, TGeoMedium *ExtAl,Int_t ExtClusterNumber);
  ~PndGeoHypGeDEGAS();

	void BuildCrystals(Int_t *CrystalNumber);
	void BuildCryostat();
	
// ---------------  Prints the nodes of a cluster -----------------
	void PrintNodes(Int_t nLevels );
  
  ClassDef(PndGeoHypGeDEGAS,0); // Class for GeoHypGeTripleCluster
};

#endif  /* !PndGeoHypGeDEGAS_H */
