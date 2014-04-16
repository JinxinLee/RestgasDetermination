/*
 * PndLmdCombinedDataReader.cxx
 *
 *  Created on: Aug 26, 2013
 *      Author: steve
 */

#include "PndLmdCombinedDataReader.h"
#include "PndLmdTrackQ.h"
#include "TVector3.h"
PndLmdCombinedDataReader::PndLmdCombinedDataReader() :
		rel_momentum_deviation_threshold(0.0003), data_tree("cbmsim"), filtered_track_array(
												    "PndLmdTrackQ", 100) , sigTh_threshold(10) {
	track_array = 0;
}

PndLmdCombinedDataReader::~PndLmdCombinedDataReader() {
}

unsigned int PndLmdCombinedDataReader::getEntries() const {
	return data_tree.GetEntries();
}

void PndLmdCombinedDataReader::initDataStream() {
	for (unsigned int i = 0; i < data_dirs.size(); i++)
		data_tree.Add(data_dirs[i] + "/Lumi_TrksQA*.root");

	track_array = new TClonesArray("PndLmdTrackQ");
	data_tree.SetBranchAddress("LMDTrackQ", &track_array);
	data_tree.SetBranchStatus("LMDTrackQ*", 1);
}

void PndLmdCombinedDataReader::clearDataStream() {
}

TClonesArray* PndLmdCombinedDataReader::getEntry(unsigned int i) {
	data_tree.GetEntry(i);
	filtered_track_array.Clear();
	unsigned int good_track_counter = 0;

	//loop over track array
	for (Int_t iN = 0; iN < track_array->GetEntries(); iN++) {
		PndLmdTrackQ *track = (PndLmdTrackQ*) track_array->At(iN);

		// // 	double thREC = 1e3*fabs(track->GetIPtheta());
	        if((track->GetSecondary())<0){
		  if(track->GetTrkRecStatus() == 0){

		  // // //X&Y cut ----------------------------------
		  // TVector3 LMDpoint;
		  // track->GetLMDpoint(LMDpoint);
		  // double thLMD = 1e3*track->GetLMDtheta();
		  // double Xref = -19.14+1.121*thLMD;
		  // double phLMD = 1e3*track->GetLMDphi();
		  // double Yref = 0.04149+0.04468*phLMD;
		  // double diffX = fabs(LMDpoint.X() - Xref);
		  // double diffY = fabs(LMDpoint.Y() - Yref);
		  //  // std::cout<<"thLMD = "<<thLMD<<" phLMD = "<<phLMD
		  //  // 	    <<" diffX = "<<diffX<<" diffY = "<<diffY<<std::endl;
		  // //  std::cout<<"Xref = "<<Xref<<" LMDpoint.X() = "<<LMDpoint.X()<<std::endl;
		  // //	  std::cout<<"Yref = "<<Yref<<" LMDpoint.Y() = "<<LMDpoint.Y()<<std::endl;

		  // //ToDo: simple Momentum cut 
		  // double momIP = track->GetIPmom();
		  // double diffMom = fabs(momIP-1.5);
		  // //  std::cout<<"diffMom = "<<diffMom<<std::endl;
		  // //  if(diffX<1.5 && diffY<2.1 && diffMom<1e-2)
		  // if(diffX<1.5 && diffY<2.1)
		  //   filtered_track_array[good_track_counter++] = track;
		  // else{
		  //   track->SetTrkRecStatus(-1);//track was lost
		  //   filtered_track_array[good_track_counter++] = track;
		  // }
		  // }
		  // else{
		  //   filtered_track_array[good_track_counter++] = track;
		  // }

		  // //[END] X&Y cut --------------------------------
		  
		//   // // //Filter tracks ------------------------------
		    
		    double thREC = 1e3*fabs(track->GetIPtheta());
		    double thMC = 1e3*fabs(track->GetMCtheta());
		    // if(thMC<8.){
		    //   filtered_track_array[good_track_counter++] = track;
		    // }
		    // else{
		    double diffthMCREC_real = fabs(thMC-thREC);
		    if(diffthMCREC_real<sigTh_threshold)
		      filtered_track_array[good_track_counter++] = track;
		    else{
		      track->SetTrkRecStatus(-1);//track was lost
		      filtered_track_array[good_track_counter++] = track;
		    }
		  }
		  else{
		    filtered_track_array[good_track_counter++] = track;
		  }
		  //   //[END] Filter tracks --------------------------------
		  //	  filtered_track_array[good_track_counter++] = track;
		}
	}
	return &filtered_track_array;
}
