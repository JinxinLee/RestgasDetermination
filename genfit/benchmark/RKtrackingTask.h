#ifndef RKTRACKINGTASK_H
#define RKTRACKINGTASK_H

// Pnd includes
#include "FairTask.h"

// Root includes
#include "TStopwatch.h"
#include "TRandom3.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "TTree.h"

#include "AbsBField.h"




class RKtrackingTask : public FairTask{

 public:
	
	/** Default constructor **/  
	RKtrackingTask();

	/** Constructor **/  
	RKtrackingTask(const char* name, Int_t iVerbose);
	
	/** Destructor **/
	virtual ~RKtrackingTask();
	
	/** Virtual method Init **/
	virtual InitStatus Init();

	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);
	
	void setField(AbsBField* f){field=f;}
	
	void writeFile(){
	  std::cout << "RKtrackingTask: writing output file" << std::endl;
	  tree->Write();
	  tree->Print();
	  file->Write();
	  file->Close();
	}
 private: 
     double momRe;
  double momTr;
  double momSi;
  double momPu;
  double xRe;
  double xTr;
  double yRe;
  double yTr;
  double xSi;
  double ySi;
  double xPu;
  double yPu;
  double xpRe;
  double xpTr;
  double ypRe;
  double ypTr;
  double xpSi;
  double ypSi;
  double xpPu;
  double ypPu;
  double chi2;

	TFile* file;
	TTree* tree;

	AbsBField* field;

	TClonesArray* fMvdPointArray;
	TClonesArray* fGemPointArray;
	TClonesArray* fDchPointArray;

	/** Get parameter containers **/
	virtual void SetParContainers();

	/** Virtual method Finish **/
	virtual void Finish();

  ClassDef(RKtrackingTask,1);
  
};

#endif
