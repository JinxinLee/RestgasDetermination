#include "statTree.h"
#include "Track.h"

#include<vector>
#include<string>
#include<iostream>

#include "TFile.h"
#include "TTree.h"
#include "TMatrixT.h"

statTreeMaker::~statTreeMaker() {
  outfile->Close();
  delete outfile;
  //delete tree;
}

statTreeMaker::statTreeMaker(std::string treeName, std::string fileName)
{

  outfile = new TFile(fileName.c_str(),"RECREATE");
  tree = new TTree(treeName.c_str(),
				   "tree created by statTreeMaker in genfit()");

  state_p = &state;
  residMCtruth_p = &residMCtruth;
  residStart_p = &residStart;
  covFit_p = &covFit;
  
  tree->Branch("state",       "TMatrixT<double>",&state_p,       32000,0);
  tree->Branch("residMCtruth","TMatrixT<double>",&residMCtruth_p,32000,0);
  tree->Branch("residStart",  "TMatrixT<double>",&residStart_p,  32000,0);
  tree->Branch("covFit",    "TMatrixT<double>",&covFit_p,    32000,0);

  tree->Branch("chi2",&chi2,"chi2/D");


}

void statTreeMaker::addTracks(const std::vector<Track*>& trackList,
							  int trackRepID)
{
  //loop over all tracks
  for(unsigned int i=0;i<trackList.size();++i){
    Track* trk = trackList[i];

	int _dim = trk->getTrackRep(trackRepID)->getDim();
	state.ResizeTo(_dim,1);
	residMCtruth.ResizeTo(_dim,1);
	residStart.ResizeTo(_dim,1);
	covFit.ResizeTo(_dim,_dim);

	//which hit to use?
	//	int k=trk->getNumHits()-1;
	//	if(k<=0) continue;
	//	double k_s = trk->getHit(k)->getS();

	trk->getTrackRep(trackRepID)->getFitParams()->getStateFin(0,state);

	TMatrixT<double> startState = trk->getTrackRep(trackRepID)->getFitParams()->getStartState();
	//	double startState_s = trk->getTrackRep(trackRepID)->getFitParams()->getStartState_s();
	TMatrixT<double> MCtruth = trk->getTrackRep(trackRepID)->getFitParams()->getMCtruth();
	//	double MCtruth_s = trk->getTrackRep(trackRepID)->getFitParams()->getMCtruth_s();

	/*
	TMatrixT<double> startState_at_k(_dim,1); 
	TMatrixT<double> MCtruth_at_k(_dim,1); 
	//	startState.Print();
	trk->getTrackRep(trackRepID)->extrapolate(startState_s,k_s,
											  startState,
											  startState_at_k);
	trk->getTrackRep(trackRepID)->extrapolate(MCtruth_s,k_s,
											  MCtruth,
											  MCtruth_at_k);

	TMatrixT<double> state_at_startS(_dim,1);
	trk->getTrackRep(trackRepID)->extrapolate(k_s,MCtruth_s,
											  state,
											  state_at_startS);
	*/

	chi2 = trk->getTrackRep(trackRepID)->getChiSqu();
	//    residMCtruth = state - MCtruth_at_k;
    residMCtruth = state - MCtruth;
	//    residMCtruth = state_at_startS - MCtruth;
	//    residStart = startState_at_k - MCtruth_at_k;
    residStart = startState - MCtruth;
    trk->getTrackRep(trackRepID)->getFitParams()->getCovFin(0,covFit);
    tree->Fill();

	//MCtruth_at_k.Print();
	//state.Print();
	//	(trk->getTrackRep(trackRepID)->getStartCov()).Print();
	//	(trk->getTrackRep(trackRepID)->getCov()).Print();
	//covFit.Print();

  }
  outfile->cd();
  tree->Write();  
}
