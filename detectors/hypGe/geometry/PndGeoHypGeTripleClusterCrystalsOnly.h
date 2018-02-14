// ******** Header of class to build HypGe Triple Cluster Geometry *************
//					by steinen@kph.uni-mainz.de

#ifndef PNDGEOHYPGETRIPLECLUSTERCRYSTALSONLY_H
#define PNDGEOHYPGETRIPLECLUSTERCRYSTALSONLY_H

#include "PndGeoHypGeCrystalWithoutCapsule.h"
#include "PndGeoHypGeCluster.h"
#include "TGeoArb8.h"
#include "TGeoXtru.h"

class  PndGeoHypGeTripleClusterCrystalsOnly : public PndGeoHypGeCluster
{
private:
  Double_t 						TripleCrystalXOffset;
	Double_t 						TripleCrystalYOffset;
	Double_t 						TripleCrystalZOffset;
public:
  PndGeoHypGeTripleClusterCrystalsOnly();			// do not use!
  PndGeoHypGeTripleClusterCrystalsOnly(TGeoMedium *ExtGe, TGeoMedium *ExtAl,Int_t ExtClusterNumber);
  ~PndGeoHypGeTripleClusterCrystalsOnly();

	void BuildCrystals(Int_t *CrystalNumber);
	void BuildCryostat();
	
// ---------------  Prints the nodes of a cluster -----------------
	void PrintNodes(Int_t nLevels );
  
  ClassDef(PndGeoHypGeTripleClusterCrystalsOnly,0); // Class for GeoHypGeTripleCluster
};

#endif  /* !PNDGEOHYPGETRIPLECLUSTER_H */
