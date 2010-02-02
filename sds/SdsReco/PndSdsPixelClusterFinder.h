#ifndef PNDSDSPIXELCLUSTERFINDER_H
#define PNDSDSPIXELCLUSTERFINDER_H

#include <vector>
#include "PndSdsDigiPixel.h"

//! Base class for cluster finding algorithms
/** @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
* As input it gets all digis to be analyzed and a set of parameters vector<Double_t>
* The output is a vector< vector <Int_t> > which contains the position of the hits in the TClonesArray containing the Digis
*/

class PndSdsPixelClusterFinder
{
public :
  PndSdsPixelClusterFinder(){};
  PndSdsPixelClusterFinder(std::vector<Double_t> params, std::vector<PndSdsDigiPixel> hits)
  {
        fParams = params;
        fHits   = hits;
        fVerbose = 0;
  };
//  virtual ~PndSdsPixelClusterFinder();
  virtual std::vector< std::vector < Int_t > > GetClusters() = 0;
  void Print();
  void Print(std::vector<PndSdsDigiPixel> hits);
  void PrintResult(std::vector<std::vector< Int_t> > clusters);
  void SetVerbose(Int_t level){fVerbose = level;};

  std::vector<PndSdsDigiPixel> fHits;
  Int_t fVerbose;
  std::vector<Double_t> fParams;

  ClassDef(PndSdsPixelClusterFinder, 2);
};

#endif


