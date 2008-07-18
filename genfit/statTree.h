#ifndef FITTER_MAKESTATTREE_H
#define FITTER_MAKESTATTREE_H

#include "TMatrixT.h"
#include "TTree.h"
#include "TFile.h"
#include<string>
#include<vector>

class Track;

class statTreeMaker {
 private:
  TFile* outfile;
  TTree* tree;

  TMatrixT<double> state,residMCtruth,residStart,covFit;
  TMatrixT<double> *state_p,*residMCtruth_p,*residStart_p,*covFit_p;
  double chi2;

  int _dim;

 public:
  statTreeMaker(std::string treeName, std::string fileName);
  virtual ~statTreeMaker();

  void addTracks(const std::vector<Track*>& trackList,
				 int trackRepID);

};
#endif // FITTER_MAKESTATTREE_H
