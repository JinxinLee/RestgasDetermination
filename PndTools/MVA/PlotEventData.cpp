// C/C++ includes
#include <iostream>
#include <vector>
#include <string>
#include <omp.h>

// Root includes
#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TString.h"
#include "TCanvas.h"

void PlotEventData(const char* EventTreeFile,
		   const char* HistOutFile)
{
  
  int id, nthreads;
  
  TFile *EvTrees = new TFile(EventTreeFile);
  
  TTree *El = ( TTree* )EvTrees->Get("El");
  TTree *Pi = ( TTree* )EvTrees->Get("Pion");
  TTree *Ka = ( TTree* )EvTrees->Get("Kaon");
  TTree *Mu = ( TTree* )EvTrees->Get("Muon");
  
  TH2F* el = new TH2F("el", "el", 100, 0,0.9, 100, 0,2.5);
  TH2F* pi = new TH2F("pi", "pi", 100, 0,0.9, 100, 0,2.5);
  TH2F* ka = new TH2F("ka", "ka", 100, 0,0.9, 100, 0,2.5);
  TH2F* mu = new TH2F("mu", "mu", 100, 0,0.9, 100, 0,2.5);
  
  el->SetLineColor(kMagenta);
  el->SetMarkerColor(kMagenta);
  
  pi->SetLineColor(kBlue);
  pi->SetMarkerColor(kBlue);
  
  ka->SetLineColor(kRed);
  ka->SetMarkerColor(kRed);
  
  mu->SetLineColor(kBlack);
  mu->SetMarkerColor(kBlack);
  
  float p,s,m,tpc,mvd;
  p = s = m = tpc = mvd = 0.0;
  id = 0;
  
  //omp_set_dynamic(0);
  //omp_set_num_threads(2);
#pragma omp parallel private(id,p,s,m,tpc,mvd)
  {
    id = omp_get_thread_num();
#pragma omp sections nowait
    {
#pragma omp section //1
      {
	//Fill Electron hist
	El->SetBranchAddress("p1",&p);
	El->SetBranchAddress("s1",&s);
	El->SetBranchAddress("mm",&m);
	El->SetBranchAddress("tpc1",&tpc);
	El->SetBranchAddress("mvd1",&mvd);
	
	// #pragma omp for ordered
	for(unsigned int j = 0; j < El->GetEntriesFast(); j++){
	  El->GetEntry(j);
	  el->Fill(m,tpc);
	}
      }//Section 1
#pragma omp section //2
      {
	//Fill Pion hist
	p = s = m = tpc = mvd = 0.0;
	Pi->SetBranchAddress("p1",&p);
	Pi->SetBranchAddress("s1",&s);
	Pi->SetBranchAddress("mm",&m);
	Pi->SetBranchAddress("tpc1",&tpc);
	Pi->SetBranchAddress("mvd1",&mvd);
	
	for(unsigned int j = 0; j < Pi->GetEntriesFast(); j++){
	  Pi->GetEntry(j);
	  pi->Fill(m,tpc);
	}
      }// End of Section 2
#pragma omp section //3
      {
	//Fill Kaon hist
	p = s = m = tpc = mvd = 0.0;
	Ka->SetBranchAddress("p1",&p);
	Ka->SetBranchAddress("s1",&s);
	Ka->SetBranchAddress("mm",&m);
	Ka->SetBranchAddress("tpc1",&tpc);
	Ka->SetBranchAddress("mvd1",&mvd);

	for(unsigned int j = 0; j < Ka->GetEntriesFast(); j++){
	  Ka->GetEntry(j);
	  ka->Fill(m,tpc);
	}
      }//End of Section 3
#pragma omp section //4
      {
	//Fill Muon hist
	p = s = m = tpc = mvd = 0.0;
	Mu->SetBranchAddress("p1",&p);
	Mu->SetBranchAddress("s1",&s);
	Mu->SetBranchAddress("mm",&m);
	Mu->SetBranchAddress("tpc1",&tpc);
	Mu->SetBranchAddress("mvd1",&mvd);
	
	for(unsigned int j = 0; j < Mu->GetEntriesFast(); j++){
	  Mu->GetEntry(j);
	  mu->Fill(m,tpc);
	}
      }//End of Section 4
    }// End of sections 
    
    /* 
     * We need to wait for all running threads. The first one will
     * create and save the output histogram file 
     */
#pragma omp barrier
    if ( id == 0 ){
      nthreads = omp_get_num_threads();
      TCanvas *c1 = new TCanvas("c1", "c1",11,34,700,500);
      c1->SetGridx();
      c1->SetGridy();
      el->Draw();
      pi->Draw("same");
      ka->Draw("same");
      mu->Draw("same");
      c1->SaveAs(HistOutFile);
      
      std::cout<< "There were "<< nthreads << " threads"<<std::endl;
      
      delete el;
      delete pi;
      delete ka;
      delete mu;
      delete c1;
    }
  }// End of Parallel
}

int main(int argc, char** argv)
{
  if(argc != 3){
    std::cout << 
      "You need to specify the input and output filenames."
	      << std::endl;
    exit(1);
  }
  const char* fname    = argv[1];
  const char* Outfname = argv[2];
  PlotEventData(fname,Outfname);
  return 0;
}

/*
  TObjArray* bla = El->GetListOfBranches();
  std::cout << "Number of entries is " 
  << bla->GetEntriesFast() << std::endl;
  
  std::vector<std::string*> names;
  
  for (int i = 0; i< bla->GetEntries(); i++){
  std::cout << bla->At(i)->GetName() << std::endl;
  std::string *b = new std::string(bla->At(i)->GetName());
  names.push_back(b);
  }
  
  for (unsigned int i = 0 ; i < names.size(); i++){
  std::string *b = names.at(i);
  std::cout << "names["<< i <<"] = "<< *b << std::endl;
  }
*/
