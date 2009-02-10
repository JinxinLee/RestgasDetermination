//root macro for analysing big piles of reco data for the panda collab-meeting in dec 08
//and overhaul later :)

#include "recotasks/TrackFitStat.h"
#include "TFile.h"
#include "TError.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TH1D.h"
#include "TString.h"
#include "TF1.h"
#include "TCanvas.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>



void darmstadt_cluster_analysis_tool(int angle)
{

  //create list of valid filenames -----------------------------------------------
  
  int reference[17] = {15,20,25,30,35,40,50,60,70,80,90,100,110,120,130,140,150};
  

  int ang=-1;  //i-th angle of reference
  for (int i=0; i<17; i++)
    if(reference[i]==angle) {
      ang=i;
      std::cout<<"\n\nAngle "<<reference[i]<<" found at "<<i<<"th position of reference table"<<std::endl;
      break;
    }
  if(ang==-1)
    Fatal("cluster_analysis_tool", "invalid anglpassed to script");
  

  std::string basedir = "/lustre/e18/user/fboehmer/DATA/darmstadt_overhaul/150cm/";
  std::string infilename = basedir+"Namelist_overhaul.txt";
  
  //read in list of filenames
  std::cout<<"\n\nreading in ASCII file containing the file names to process: "
	   <<infilename.c_str()<<std::endl;
  std::ifstream infile(infilename.c_str(), std::fstream::in);
  std::vector<std::string> namelist;
  

  //TODO: build in EOF check
  for(int line=0; line<119; line++) {
    char name[100];
    infile.getline(name,100);
    if(line%17==ang)
      namelist.push_back(std::string(name));
  }
  int size = namelist.size();
  
  std::cout<<"\n\nDebug info: "<<std::endl;
  for(int i=0; i<namelist.size(); i++)
    std::cout<<(namelist[i]).c_str()<<std::endl;
  
  std::cout<<std::endl;
  
  //-------------------------------------------------------------------------------
  
  std::stringstream stream;
  stream<<angle;
  std::string angle_st=stream.str();
  std::string outfilename=basedir+"analysis/analysis_150_overhaul_"+angle_st.c_str()+".txt";
  std::string outrootfilename=basedir+"analysis/analysis_150_overhaul_"+angle_st.c_str()+".root";
  std::cout<<"Writing data to outfile "<<outfilename.c_str()<<std::endl;
  std::cout<<"Writing fit histograms to ROOT file "<<outrootfilename.c_str()<<std::endl;
  
  
  std::ofstream outfile(outfilename.c_str(), std::fstream::out);
  int errCount=0;
  

 //read in all data files, process, fit and store fit results --------------------

  std::string recodir = basedir+"reco/";
  std::vector<TH1D*> hists;

  for(int file=0; file<size; file++) {
    
    int nClust=0;
    std::string filename = recodir+(namelist[file]).c_str()+".reco.root";

    std::string mom = (namelist[file]).substr(6,3);
    std::cout<<"mom value from string: "<<mom.c_str()<<std::endl;
    double P=atof(mom.c_str());

    double lowBin=0.75*P;
    double highBin=1.25*P;

    double RMS_factor=1.;  //multiplier for range lookup - for better fits
    int NBins=250;
    
//     //if(file<29)
//       //NBins=1000;
//     if(file<60) {
//       //highBin=1.0;
//       RMS_factor=2.0; }
//     //if(file>74)
//       //lowBin=0.8;

    std::string histname = mom.c_str();
    histname.append("GeV_"); //gives errors with + operand
    histname.append(angle_st);
    histname.append("_deg_");
    
    std::cout<<"\ncreated histogram "<<histname.c_str()<<std::endl;
    hists.push_back(new TH1D(histname.c_str(),histname.c_str(),NBins,lowBin,highBin));

    int nh = hists.size();
    TH1D* histo = hists[nh-1]; //pointer to the current histogram

    TFile* inFile = new TFile(filename.c_str());
    TTree* tree = (TTree*) inFile->Get("cbmsim");
    //TrackFitStat* fitstat = new TrackFitStat();
    TClonesArray* fitstatarr = new TClonesArray("TrackFitStat");
    TClonesArray* clustarr = new TClonesArray("PndTpcCluster");
    
    int nE=0;
    if(tree!=0) {
      tree->SetBranchAddress("TrackFitStat", &fitstatarr);
      tree->SetBranchAddress("PndTpcCluster", &clustarr);
      nE = tree->GetEntries();
    }
    std::cout<<"found "<<nE<<" entries in tree"<<std::endl;

    for(int ev=0; ev<nE; ev++) {
      tree->GetEvent(ev);
      int nArr = fitstatarr->GetEntriesFast();				
      nClust += clustarr->GetEntriesFast();
      //std::cout<<"  found "<<nArr<<" entries in TClonesArray"<<std::endl;
      for(int arr=0; arr<nArr; arr++) {
	double p = ((TrackFitStat*)fitstatarr->At(arr))->GetP();
       	histo->Fill(p);
      }
    }
    
    inFile->Close();
    delete inFile;

    int entries=0;
    if(nE>0)
      entries = histo->GetEntries();
    double mean_p, low_p, high_p;
    double RMS = histo->GetRMS();
   
    if(entries!=0) {
      int b=histo->GetMaximumBin();
      mean_p = b*histo->GetBinWidth(b)+lowBin;
      low_p = mean_p*0.85;
      high_p = mean_p*1.15;
    
//   //     //improved fitting:
// //       double binWidth = histo->GetBinWidth(1);
// //       //test
// //       double max = lowBin + NBins*binWidth;
// //       if(fabs(max-highBin)>0.001)
// // 	errCount++;
// //       bool found=false;
// //       int step = 1;
// //       int meanBin = (mean_p-lowBin)/binWidth; 
// //       double cutValue=histo->GetMaximum()*0.05;
// //       int RMSbins = RMS/binWidth;
	
// //       while(found==false) {
// // 	double right=histo->GetBinContent(meanBin+step+RMSbins);
// // 	double left=histo->GetBinContent(meanBin-step-RMSbins);
// // 	if(right<cutValue && left<cutValue) {
// // 	  found=true;
// // 	  low_p=left;
// // 	  high_p=right;
// // 	}
// // 	step++; 
// //       } 
    }  
        
    if(entries>0) {
      //automatic fitting
      char fname[20];
      sprintf(fname,"fit%i",file);

      TF1* fitfunc = new TF1(fname, "gaus", low_p, high_p);
      fitfunc->SetLineColor(kRed);
      //fitfunc->SetParameters(mean_p,1);
      double* params;
      if (entries!=0) {
	histo->Fit(fitfunc,"","",low_p,high_p);
	//histo->DrawClone();
	//fitfunc->DrawClone("same");
	//histo->Write();
	params = fitfunc->GetParameters();
	std::cout<<"Fit parameters : "<<params[0]<<",   "<<params[1]<<",   "<<params[2]<<std::endl; 
	outfile<<mom<<"   "<<angle<<"   "<<params[0]<<"   "<<params[1]<<"   "
	       <<params[2]<<"   "<<nClust/10000<<std::endl;
      }
    }
    else
      outfile<<mom<<"   "<<angle<<"   "<<"0"<<"   "<<"0"<<"   "<<"0"<<"   0"<<std::endl;

  }
  std::cout<<"\n\nFinished... BinWidth stuff went wrong "<<errCount<<" times."<<std::endl;
  TFile* root_out = new TFile(outrootfilename.c_str(), "recreate");
  for(int i=0; i<hists.size(); i++)
    (hists[i])->Write();
  
}
  
  
