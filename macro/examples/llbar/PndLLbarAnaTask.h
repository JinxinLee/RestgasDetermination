#ifndef PndLLbarAnaTask_H
#define PndLLbarAnaTask_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "TLorentzVector.h"

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;

class RhoMassParticleSelector;
class PndAnalysis;
class RhoCandList;
class RhoTuple;


class PndLLbarAnaTask : public FairTask
{

 public:
	
	// ** Default constructor   
	PndLLbarAnaTask();
	
	// ** Destructor 
	~PndLLbarAnaTask();	

	//int SelectTruePid(PndAnalysis *ana, RhoCandList &l);
	
	// ** Virtual method Init 
	virtual InitStatus Init();
	
	// ** Virtual method Exec 
	virtual void Exec(Option_t* opt);
	
	
	virtual void Finish();

 protected:
	
	
 private: 
	// *** event counter
	int nevts;	
	
	// *** mass selector for the J/psi
	//RhoMassParticleSelector *fJpsiMassSel;
	
	// *** a method 
	int  SelectTruePid(PndAnalysis *ana, RhoCandList &l);

	// *** declare some histograms

	// *** Real histograms

	TH1F *hprob_4c;
	TH1F *hchi2_4c;

	TH1F *hlam0_M_all;
	TH1F *hlam0_M_4c;

	TH1F *hlam0bar_M_all;
	TH1F *hlam0bar_M_4c;

	//Piplus truthmatch histos
	TH1F *hpiplus_tm_P;
	TH1F *hpiplus_tm_Pt;
	TH1F *hpiplus_tm_Theta;
	TH1F *hpiplus_tm_cosTheta;
	TH1F *hpiplus_tm_cm_cosTheta;

	//Piminus truthmatch histos
	TH1F *hpiminus_tm_P;
	TH1F *hpiminus_tm_Pt;
	TH1F *hpiminus_tm_Theta;
	TH1F *hpiminus_tm_cosTheta;
	TH1F *hpiminus_tm_cm_cosTheta;

	//Proton truthmatch histos
	TH1F *hproton_tm_P;
	TH1F *hproton_tm_Pt;
	TH1F *hproton_tm_Theta;
	TH1F *hproton_tm_cosTheta;
	TH1F *hproton_tm_cm_cosTheta;

	//AntiProton truthmatch histos
	TH1F *hantiproton_tm_P;
	TH1F *hantiproton_tm_Pt;
	TH1F *hantiproton_tm_Theta;
	TH1F *hantiproton_tm_cosTheta;
	TH1F *hantiproton_tm_cm_cosTheta;

	//Lambda truthmatch histos
	TH1F *hlam0_tm_M_all;
	TH1F *hlam0_tm_cosTheta_all;
	TH1F *hlam0_tm_chi2_vtx;
	TH1F *hlam0_tm_prob_vtx;
	TH1F *hlam0_tm_M_vtx;
	TH1F *hlam0_tm_R_vtx;
	TH1F *hlam0_tm_Z_vtx;
	TH1F *hlam0_tm_cm_cosTheta_all;

	//Lambdabar truthmatch histos
	TH1F *hlam0bar_tm_M_all;
	TH1F *hlam0bar_tm_cosTheta_all;
	TH1F *hlam0bar_tm_chi2_vtx;
	TH1F *hlam0bar_tm_prob_vtx;
	TH1F *hlam0bar_tm_M_vtx;
	TH1F *hlam0bar_tm_R_vtx;
	TH1F *hlam0bar_tm_Z_vtx;
	TH1F *hlam0bar_tm_cm_cosTheta_all;
	
	// *** --------------------- ***
	// *** True MC 1D histograms ***
	// *** --------------------- ***

	//Lambda CM frame
	TH1F *htruelam0_cm_cosTheta;
	//Lambdabar CM frame
	TH1F *htruelam0bar_cm_cosTheta;

	//Piplus
	TH1F *htruepiplus_cosTheta;
	//Piminus
	TH1F *htruepiminus_cosTheta;
	//Proton
	TH1F *htrueproton_cosTheta;
	//Antiproton
	TH1F *htrueantiproton_cosTheta;


	// *** ------------------------- ***
	// *** Create some 2D histograms ***
	// *** ------------------------- ***

	//Piplus truthmatch histos
	TH2F *hpiplus_tm_PvsTheta;
	TH2F *hpiplus_tm_PvsDP;
	TH2F *hpiplus_tm_ThetavsDTheta;

	//Piminus truthmatch histos
	TH2F *hpiminus_tm_PvsTheta;
	TH2F *hpiminus_tm_PvsDP;
	TH2F *hpiminus_tm_ThetavsDTheta;

	//Proton truthmatch histos
	TH2F *hproton_tm_PvsTheta;
	TH2F *hproton_tm_PvsDP;
	TH2F *hproton_tm_ThetavsDTheta;

	//AntiProton truthmatch histos
	TH2F *hantiproton_tm_PvsTheta;
	TH2F *hantiproton_tm_PvsDP;
	TH2F *hantiproton_tm_ThetavsDTheta;
	
	// *** the PndAnalysis object
	PndAnalysis *theAnalysis;

	// *** Get parameter containers
	virtual void SetParContainers();
		
	ClassDef(PndLLbarAnaTask,1);
  
};

#endif
