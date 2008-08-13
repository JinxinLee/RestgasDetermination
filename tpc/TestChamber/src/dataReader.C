#define dataReader_cxx
#include "dataReader.h"
#include "TH2.h"
#include "TStyle.h"
#include "TCanvas.h"

#include<iostream>

using namespace std;

void dataReader::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L dataReader.C
//      Root > dataReader t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
   }
}

TCevt* dataReader::readEvent() {
  Long64_t nentries = fChain->GetEntriesFast();
  TCevt* returnData;

  if(lastEvent<nentries) {
    
    returnData = new TCevt();
    
    Long64_t nb = fChain->GetEntry(lastEvent);
    lastEvent++;
    returnData->trigger = trigger;
    //returnData->runtime = runtime;
    //returnData->nchan = nchan;
    //returnData->mc_intercept_x = mc_intercept_x;
    //returnData->mc_slope_x = mc_slope_x;
    //returnData->mc_intercept_z = mc_intercept_z;
    //returnData->mc_slope_z = mc_slope_z;
    for(int i=0; i< 128; i++) {
      //returnData->nslice[i] = nslice[i];
      //returnData->chan[i] = chan[i];
      for(int j=0; j< 150; j++) {
	returnData->charge[i][j] = charge[i][j];
	//returnData->time[i][j] = time[i][j];
      }
    }
    if(mcFlag==1){
      returnData->MCTax=MCTax;
      returnData->MCTbx=MCTbx;
      returnData->MCTay=MCTay;
      returnData->MCTby=MCTby;
    }
    return returnData;
  }
  else {
    return NULL;
  }
  
}


void TCevt::Print(){
  cout << "====================================" << endl;
  cout << "trigger: " << trigger << endl;
  //cout << "nchan: " << nchan << endl;
  //cout << "mc_intercept_x: " << mc_intercept_x << endl;
  //cout << "mc_intercept_z: " << mc_intercept_z << endl;
  //cout << "mc_slope_x: " << mc_slope_x << endl;
  //cout << "mc_slope_z: " << mc_slope_z << endl;

  
	cout << "chan: ";
	for(int i=0;i<128;i++) {
	  //cout << chan << " ";
	}
	//cout << endl;
	
	cout << "nslice: ";
	for(int i=0;i<128;i++) {
	  //cout << nslice << " ";
	}
	//cout << endl;
	
	for(int i=0;i<128;i++) {
	  cout << "charge" << i << ": ";
	  for(int j=0;j<150;j++) {
		cout << charge[i][j] << " "; 
	  }
	  cout << endl;
	  //cout << "time" << i << ": ";
	  for(int j=0;j<150;j++) {
	    //cout << time[i][j] << " "; 
	  }
	  //cout << endl;
	}
  
}

void TCevt::Print(int pad){

  if(pad<0 || pad>127) throw;
  
  cout << "charge" << pad << ": ";
  for(int j=0;j<150;j++) {
	cout << charge[pad][j] << " "; 
  }
  cout << endl;
  //cout << "time" << pad << ": ";
  for(int j=0;j<150;j++) {
    //cout << time[pad][j] << " "; 
  }
  //cout << endl;
}

int TCevt::Nchan(std::map<int,double>& _ped_s) {
  int returnVal = 0;
  for(int iPad=0;iPad<128;++iPad) {
	bool isactive=false;
	for(int iTime=0;iTime<150;++iTime) {
	  if(charge[iPad][iTime] > 4.*_ped_s[iPad]) {
		isactive=true;
	  }	
	}
	if(isactive) returnVal++;
  }
  return returnVal;
}
 
 
