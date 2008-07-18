#include "statTreeAnalyzer.h"

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"

statTreeAnalyzer::statTreeAnalyzer(std::string treeName_source, std::string fileName_source, std::string fileName_target) : statTreeAnalyzer_auto(treeName_source,fileName_source) {
  outfileName = fileName_target;
}

statTreeAnalyzer::~statTreeAnalyzer() {
}

void statTreeAnalyzer::rangeSetHelper(double val, double& more, double& less) {
  if(val>0.) {
    less = 0.999;
    more = 1.001;
  }
  else {
    less = 1.001;
    more = 0.999;
  }
}

void statTreeAnalyzer::Loop() {

   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;

   std::cout << outfileName << std::endl;
   TFile outfile(outfileName.c_str(),"RECREATE");
   outfile.mkdir("pull_vs_resStart");
   outfile.mkdir("pull_vs_state");
   std::cout << outfileName << " 1" <<std::endl;

   fChain->GetEntry(0);
   const int dimension = state->GetNrows();

   //loop over all entries to look for minima and maxima for histo ranges
   double pull_min[dimension];
   double pull_max[dimension];
   double res_MCtruth_min[dimension];
   double res_MCtruth_max[dimension];
   double res_start_min[dimension];
   double res_start_max[dimension];
   double state_min[dimension];
   double state_max[dimension];
   double chi2_min;
   double chi2_max;
   //init all the values with first entry
   double less,more;

   rangeSetHelper(chi2,more,less);
   chi2_min = less * chi2;
   chi2_max = more * chi2;
   for(int i=0;i<dimension;i++) {
     rangeSetHelper((*residMCtruth)[i][0] / TMath::Sqrt( (*covFit)[i][i] ),more,less);
     pull_min[i] = less * (*residMCtruth)[i][0] / TMath::Sqrt( (*covFit)[i][i] );
     pull_max[i] = more * (*residMCtruth)[i][0] / TMath::Sqrt( (*covFit)[i][i] );

     rangeSetHelper((*residMCtruth)[i][0],more,less);
     res_MCtruth_min[i] = less * (*residMCtruth)[i][0];
     res_MCtruth_max[i] = more * (*residMCtruth)[i][0];

     rangeSetHelper((*residStart)[i][0],more,less);
     res_start_min[i] = less * (*residStart)[i][0];
     res_start_max[i] = more * (*residStart)[i][0];

     rangeSetHelper((*state)[i][0],more,less);
     state_min[i] = less * (*state)[i][0];
     state_max[i] = more * (*state)[i][0];
   }

   //do the loop finally
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
     Long64_t ientry = LoadTree(jentry);
     if (ientry < 0) break;
     nb = fChain->GetEntry(jentry);   nbytes += nb;
     // if (Cut(ientry) < 0) continue;
     for(int i=0;i<dimension;i++) {
       if( (*residMCtruth)[i][0] / TMath::Sqrt( (*covFit)[i][i] )< 1000. && (*residMCtruth)[i][0] / TMath::Sqrt( (*covFit)[i][i] ) > -1000. ) {
		 if((*residMCtruth)[i][0] / TMath::Sqrt( (*covFit)[i][i] ) <  pull_min[i]) {
		   pull_min[i] = (*residMCtruth)[i][0] / TMath::Sqrt( (*covFit)[i][i] );
		 }
		 if((*residMCtruth)[i][0] / TMath::Sqrt( (*covFit)[i][i] ) >  pull_max[i]) {
		   pull_max[i] = (*residMCtruth)[i][0] / TMath::Sqrt( (*covFit)[i][i] );
		 }
	   }
       if((*residMCtruth)[i][0] < 1000. && (*residMCtruth)[i][0] > -1000. ) {
		 if((*residMCtruth)[i][0] <  res_MCtruth_min[i]) {
		   res_MCtruth_min[i] = (*residMCtruth)[i][0];
		 }
		 if((*residMCtruth)[i][0] >  res_MCtruth_max[i]) {
		   res_MCtruth_max[i] = (*residMCtruth)[i][0];
		 }
	   }
       if((*residStart)[i][0] < 1.e5 && (*residStart)[i][0] > -1.e5 ) {
		 if((*residStart)[i][0] <  res_start_min[i]) {
		   res_start_min[i] = (*residStart)[i][0];
		 }
		 if((*residStart)[i][0] >  res_start_max[i]) {
		   res_start_max[i] = (*residStart)[i][0];
		 }
	   }
       if((*state)[i][0] < 1.e5 && (*state)[i][0] > -1.e5 ) {
		 if((*state)[i][0] <  state_min[i]) {
		   state_min[i] = (*state)[i][0];
		 }
		 if((*state)[i][0] >  state_max[i]) {
		   state_max[i] = (*state)[i][0];
		 }
	   }
     }
     if(chi2 > -1.e5 && chi2 < 1.e5) {
       if(chi2 <  chi2_min) {
		 chi2_min = chi2;
       }
       if(chi2 >  chi2_max) {
		 chi2_max = chi2;
       }
     }
     else {
	   //       std::cout << "statTreeAnalyzer trying to find range for chi2-histo: found chi2 value in tree: " 
	   // << chi2 << " -> skipping" << endl;
     }
   }


   //create and name the histos
   TH1D* pull_H[dimension];
   TH1D* resStart_H[dimension];
   TH1D* resMCtruth_H[dimension];
   TH1D* chi2_H;

   TH2D* pull_vs_resStart_H[dimension][dimension];
   TH2D* pull_vs_state_H[dimension][dimension];
   TH2D* chi2_vs_resStart_H[dimension];

   int nbins = 100;
   double margin = 0.1;

   double hist_minX,hist_maxX,par_minX,par_maxX, hist_minY,hist_maxY,par_minY,par_maxY;

   par_minX = chi2_min;
   par_maxX = chi2_max;
   hist_minX = par_minX - margin * (par_maxX-par_minX);
   hist_maxX = par_maxX + margin * (par_maxX-par_minX);
   chi2_H = new TH1D("chi2","chi2",nbins,hist_minX,hist_maxX);


   for(int i=0;i<dimension;i++) {
     char buf[50];

     sprintf(buf,"pull_par%i",i+1);
     par_minX = pull_min[i];
     par_maxX = pull_max[i];
     hist_minX = par_minX - margin * (par_maxX-par_minX);
     hist_maxX = par_maxX + margin * (par_maxX-par_minX);
     pull_H[i] = new TH1D(buf,buf,nbins,hist_minX,hist_maxX);

     sprintf(buf,"resMCtruth_par%i",i+1);
     par_minX = res_MCtruth_min[i];
     par_maxX = res_MCtruth_max[i];
     hist_minX = par_minX - margin * (par_maxX-par_minX);
     hist_maxX = par_maxX + margin * (par_maxX-par_minX);
     resMCtruth_H[i] = new TH1D(buf,buf,nbins,hist_minX,hist_maxX);

     sprintf(buf,"resStart_par%i",i+1);
     par_minX = res_start_min[i];
     par_maxX = res_start_max[i];
     hist_minX = par_minX - margin * (par_maxX-par_minX);
     hist_maxX = par_maxX + margin * (par_maxX-par_minX);
     resStart_H[i] = new TH1D(buf,buf,nbins,hist_minX,hist_maxX);

     for(int j=0;j<dimension;j++) {
       sprintf(buf,"pullPar%i_vs_resStartPar%i",i+1,j+1);
       par_minX = res_start_min[j];
       par_maxX = res_start_max[j];
       par_minY = pull_min[i];
       par_maxY = pull_max[i];
       hist_minX = par_minX - margin * (par_maxX-par_minX);
       hist_maxX = par_maxX + margin * (par_maxX-par_minX);
       hist_minY = par_minY - margin * (par_maxY-par_minY);
       hist_maxY = par_maxY + margin * (par_maxY-par_minY);
       pull_vs_resStart_H[i][j] = new TH2D(buf,buf,
					   nbins,hist_minX,hist_maxX,
					   nbins,hist_minY,hist_maxY
					   );
     }

     sprintf(buf,"chi2_vs_resStart_par%i",i+1);
     par_minX = res_start_min[i];
     par_maxX = res_start_max[i];
     par_minY = chi2_min;
     par_maxY = chi2_max;
     hist_minX = par_minX - margin * (par_maxX-par_minX);
     hist_maxX = par_maxX + margin * (par_maxX-par_minX);
     hist_minY = par_minY - margin * (par_maxY-par_minY);
     hist_maxY = par_maxY + margin * (par_maxY-par_minY);
     chi2_vs_resStart_H[i] = new TH2D(buf,buf,
				      nbins,hist_minX,hist_maxX,
				      nbins,hist_minY,hist_maxY
				      );

     for(int j=0;j<dimension;j++) {
       sprintf(buf,"pullPar%i_vs_statePar%i",i+1,j+1);
       par_minX = state_min[j];
       par_maxX = state_max[j];
       par_minY = pull_min[i];
       par_maxY = pull_max[i];
       hist_minX = par_minX - margin * (par_maxX-par_minX);
       hist_maxX = par_maxX + margin * (par_maxX-par_minX);
       hist_minY = par_minY - margin * (par_maxY-par_minY);
       hist_maxY = par_maxY + margin * (par_maxY-par_minY);
       pull_vs_state_H[i][j] = new TH2D(buf,buf,
										nbins,hist_minX,hist_maxX,
										nbins,hist_minY,hist_maxY
										);
     }

   }

   
   //in this loop the histos are filled
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      if(!TMath::IsNaN(chi2)) chi2_H->Fill(chi2);

      for(int i=0;i<dimension;i++) {
		double pullVal = (*residMCtruth)[i][0] / TMath::Sqrt( (*covFit)[i][i] );
		if(!TMath::IsNaN(pullVal)) pull_H[i]->Fill(pullVal);
		
		if(!(TMath::IsNaN((*residMCtruth)[i][0])) && 
		   (*residMCtruth)[i][0]>-1.e30 && 
		   (*residMCtruth)[i][0] < 1.e30) {
		  resMCtruth_H[i]->Fill( (*residMCtruth)[i][0] );
		  std::cout << (*residMCtruth)[i][0] << std::endl;
		  covFit->Print();
		}
		
		
		if(!TMath::IsNaN(   (*residStart)[i][0]   )) resStart_H[i]->Fill( (*residStart)[i][0] );
		for(int j=0;j<dimension;j++) {
		  if(!TMath::IsNaN(  (*residStart)[j][0]   ) && !TMath::IsNaN(  pullVal   )) pull_vs_resStart_H[i][j]->Fill((*residStart)[j][0],pullVal);
		  if(!TMath::IsNaN(  (*state)[j][0]   ) && !TMath::IsNaN(  pullVal   ))pull_vs_state_H[i][j]->Fill((*state)[j][0],pullVal);
		}
		if(!TMath::IsNaN(  (*residStart)[i][0]   ) && !TMath::IsNaN(  chi2   ))chi2_vs_resStart_H[i]->Fill((*residStart)[i][0],chi2);
      }
      
   }

   chi2_H->Write();
   delete chi2_H;
   for(int i=0;i<dimension;i++) {
     outfile.cd();
     pull_H[i]->Write();
     resMCtruth_H[i]->Write();
     resStart_H[i]->Write();
     chi2_vs_resStart_H[i]->Write();
     outfile.cd("pull_vs_resStart");
     for(int j=0;j<dimension;j++) {
       pull_vs_resStart_H[i][j]->Write();
     }
     outfile.cd("pull_vs_state");
     for(int j=0;j<dimension;j++) {
       pull_vs_state_H[i][j]->Write();
     }

     delete pull_H[i];
     delete resMCtruth_H[i];
     delete resStart_H[i];
     for(int j=0;j<dimension;j++) {
       delete pull_vs_resStart_H[i][j];
       delete pull_vs_state_H[i][j];
     }
     delete chi2_vs_resStart_H[i];
   }

   outfile.Close();
}



//this is the constructor that has to put into the
//statTreeAnalyzer_auto after the automatic generation.
//No worries, the statTreeAnalyzer_auto.o won't compile,
//because it calls this constructor. The default one will
//generate a compile time error
/*
statTreeAnalyzer_auto::statTreeAnalyzer_auto(std::string treeName,
					     std::string fileName)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
  TTree* tree = NULL;
  //TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("statTree.root");
  TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(fileName.c_str());
  if (!f) {
    //f = new TFile("statTree.root");
    f = new TFile(fileName.c_str());
  }
  tree = (TTree*)gDirectory->Get(treeName.c_str());
  if(tree == NULL) {
    std::cerr << "Error in statTreeAnalyzer-packet:" << std::endl 
	      << "tree\"" << treeName << "\" not found in file \""
	      << fileName << "\" -> abort" << std::endl;
    throw;
  }
  Init(tree);
}
*/
