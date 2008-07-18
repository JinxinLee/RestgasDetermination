#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TString.h"

//! returns 0 if everything looks ok
int
plotDigiQA(TString filename, TString reffilename=""){
	TFile* infile=TFile::Open(filename);
	if(infile==0){
		std::cout<<filename<<" not found! Aborting"<<std::endl;
		return 1;
	}
	
	TFile* reffile=0;	
	if(reffilename!=""){
		reffile=TFile::Open(reffilename);
		if(reffile==0){
			std::cout<<"Reference file specified but not found: "
				<<reffilename<<std::endl;
		}
	}
	
	TCanvas* c=new TCanvas("TpcDigiQA","Tpc Digitization QA plots",10,10,1000,1000);
	
	c->Divide(3,3);

}