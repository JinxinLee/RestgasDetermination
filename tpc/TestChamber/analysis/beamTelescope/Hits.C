#define Hits_cxx
#include "Hits.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

bool Hits::loadNext(){
  static int count(0);
  if (fChain == 0) return false;
   
  static Long64_t nentries = fChain->GetEntriesFast();
  if(count >= nentries) return false;
  
  //std::cout << "loading event " << count << std::endl;
  fChain->GetEntry(count++);
  return true;
}


void Hits::Loop()
{
//   In a ROOT session, you can do:
//      Root > .L Hits.C
//      Root > Hits t
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
      nb = fChain->GetEntry(jentry);  
      nbytes += nb;
      // if (Cut(ientry) < 0) continue;
   }
}
