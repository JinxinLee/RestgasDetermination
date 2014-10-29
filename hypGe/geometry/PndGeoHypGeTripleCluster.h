// ******** Header of class to build HypGe Triple Cluster Geometry *************
//					by steinen@kph.uni-mainz.de

#ifndef PNDGEOHYPGETRIPLECLUSTER_H
#define PNDGEOHYPGETRIPLECLUSTER_H

#include "PndGeoHypGeCrystal.h"
#include "PndGeoHypGeCluster.h"
#include "TGeoArb8.h"
#include "TGeoXtru.h"

class  PndGeoHypGeTripleCluster : public PndGeoHypGeCluster
{
private:
  Double_t 						TripleCrystalXOffset;
	Double_t 						TripleCrystalYOffset;
	Double_t 						TripleCrystalZOffset;
public:
  PndGeoHypGeTripleCluster();			// do not use!
  PndGeoHypGeTripleCluster(TGeoMedium *ExtGe, TGeoMedium *ExtAl,Int_t ExtClusterNumber);
  ~PndGeoHypGeTripleCluster();

	void BuildCrystals(Int_t *CrystalNumber);
	void BuildCryostat();
	
// ---------------  Prints the nodes of a cluster -----------------
	void PrintNodes(Int_t nLevels );
  
  ClassDef(PndGeoHypGeTripleCluster,0); // Class for GeoHypGeTripleCluster
};

#endif  /* !PNDGEOHYPGETRIPLECLUSTER_H */
