class RhoCandList;
class RhoCandidate;
class RhoTuple;
class PndRhoTupleQA;
class PndAnaPidSelector;
class PndAnaPidCombiner;
class PndAnalysis;

// *** routine to only keep PID matched candidates in list
int SelectTruePid(PndAnalysis *ana, RhoCandList &l)
{
	int removed = 0;

	for (int ii=l.GetLength()-1;ii>=0;--ii)
	{
		if ( !(ana->McTruthMatch(l[ii])) )
		{
			l.Remove(l[ii]);
			removed++;
		}
	}

	return removed;
}

void printCand(RhoCandidate *c)
{
	TLorentzVector lv=c->P4();

	cout <<c->PdgCode()<<" ("<<lv.X()<<"/"<<lv.Y()<<"/"<<lv.Z()<<"/"<<lv.E()<<")"<<endl;
}

void countDoubles(RhoCandList &l, int &n1, int &n2, int &n3)
{
	int n_smc  = 0;
	int n_strk = 0;
	int n_both = 0;
	double d = 0.00001;

	for (int i=0;i<l.GetLength()-1;++i)
	{
		for (int j=i+1;j<l.GetLength();++j)
		{
			TLorentzVector dl = l[i]->P4() - l[j]->P4();

			bool chkmc = (l[i]->GetMcTruth()==l[j]->GetMcTruth());
			bool chktrk = (fabs(dl.X())<d) && (fabs(dl.Y())<d) && (fabs(dl.Z())<d) && (fabs(dl.E())<d);
			if (chkmc) n_smc++;
			if (chktrk) n_strk++;
			if (chktrk && chkmc) n_both++;
		}
	}
	n1 = n_strk;
	n2 = n_smc;
	n3 = n_both;
}

int ana_multikalman(int nevts=0, TString  prefix = "../data/evtcomplete")
{
    //-----User Settings:------------------------------------------------------
  TString  parAsciiFile   = "all.par";

  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
  TString  output         = "ana";
  TString  friend1        = "reco";
  TString  friend2        = "pid";

//  TString  prefix         = "evtcomplete";
//  TString  input          = "psi2s_Jpsi2pi_Jpsi_mumu.dec";
//  TString  output         = "ana";
//  TString  friend1        = "reco_single";
//  TString  friend2        = "pid_single";

  TString  friend3        = "";
  TString  friend4        = "";

  //	// *** some variables
  	int i=0,j=0, k=0, l=0;
  	gStyle->SetOptFit(1011);

  // -----   Initial Settings   --------------------------------------------
//  PndMasterRunAna *fRun= new PndMasterRunAna();
//  //  fRun->SetInput(input);
//    fRun->SetInput(friend2);
//  fRun->SetOutput(output);
////  fRun->SetFriend1(friend1);
////  fRun->SetFriend2(friend2);
////  fRun->SetFriend3(friend3);
////  fRun->SetFriend4(friend4);
//  fRun->SetParamAsciiFile(parAsciiFile);
//  fRun->Setup(prefix);
//
//
//	fRun->Init();
//







	// *** the output file for FairRunAna
	TString OutFile="out_dummy.root";

	// *** the files coming from the simulation
	TString inPidFile  = prefix+"_pid.root";    // this file contains the PndPidCandidates and McTruth
	TString inParFile  = prefix+"_par.root";

	// *** PID table with selection thresholds; can be modified by the user
	TString pidParFile = TString(gSystem->Getenv("VMCWORKDIR"))+"/macro/params/all.par";

	// *** initialization
	FairLogger::GetLogger()->SetLogToFile(kFALSE);
	FairRunAna* fRun = new FairRunAna();
	FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
	fRun->SetSource(new FairFileSource(inPidFile));
	fRun->SetUseFairLinks(kTRUE);

	// *** setup parameter database
	FairParRootFileIo* parIO = new FairParRootFileIo();
	parIO->open(inParFile);
	FairParAsciiFileIo* parIOPid = new FairParAsciiFileIo();
	parIOPid->open(pidParFile.Data(),"in");

	rtdb->setFirstInput(parIO);
	rtdb->setSecondInput(parIOPid);
	rtdb->setOutput(parIO);

	fRun->SetOutputFile(OutFile);
	fRun->Init();










		// *** create rhotuples
	RhoTuple * fEPlus = new RhoTuple("EPlus","EPlus Info");
	RhoTuple * fEMinus = new RhoTuple("EMinus","EMinus Info");
	RhoTuple * fMuPlus = new RhoTuple("MuPlus","MuPlus Info");
	RhoTuple * fMuMinus = new RhoTuple("MuMinus","MuMinus Info");
	RhoTuple * fPiPlus = new RhoTuple("PiPlus","PiPlus Info");
	RhoTuple * fPiMinus = new RhoTuple("PiMinus","PiMinus Info");
	RhoTuple * fKPlus = new RhoTuple("KPlus","KPlus Info");
	RhoTuple * fKMinus = new RhoTuple("KMinus","KMinus Info");
	RhoTuple * fPPlus = new RhoTuple("PPlus","PPlus Info");
	RhoTuple * fPMinus = new RhoTuple("PMinus","PMinus Info");

	RhoTuple * fMcTruth = new RhoTuple("McTruth","McTruth Info");

        // *** create an output file for all histograms
	TFile *out = TFile::Open(prefix+"_output_ana.root","RECREATE");

	//
	// Now the analysis stuff comes...
	//


	// *** the data reader object
	//Set true for multikalman, false (or not set at all) for standard single kalman
	PndAnalysis* theAnalysis = new PndAnalysis();
	if (nevts==0) nevts= theAnalysis->GetEntries();

	// *** RhoCandLists for the analysis
	RhoCandList muplus, muminus, piplus, piminus,eplus,eminus,kplus,kminus,pplus,pminus, mctruth;

	// *** Mass selector for the jpsi cands
	double m0_jpsi = TDatabasePDG::Instance()->GetParticle("J/psi")->Mass();   // Get nominal PDG mass of the J/psi
	RhoMassParticleSelector *jpsiMassSel=new RhoMassParticleSelector("jpsi",m0_jpsi,1.0);

	// *** the lorentz vector of the initial psi(2S)
	TLorentzVector ini(0, 0, 6.231552, 7.240065);

	// ***
	// the event loop
	// ***

	int cntdbltrk=0, cntdblmc=0, cntdblboth=0, cnttrk=0, cnt_dbl_jpsi=0, cnt_dbl_psip=0;

	while (theAnalysis->GetEvent() && i++<nevts)
	{
		if ((i%100)==0) cout<<"evt " << i << endl;
		PndRhoTupleQA qa(theAnalysis, ini.Z());

		theAnalysis->FillList(mctruth, "McTruth");

		theAnalysis->FillList(eplus,  "PionAllPlus","PidAlgoStt;PidAlgoMvd;PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc");
		theAnalysis->FillList(eminus, "PionAllMinus","PidAlgoStt;PidAlgoMvd;PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc");
		theAnalysis->FillList(muplus,  "MuonAllPlus","PidAlgoMdtHardCuts");
		theAnalysis->FillList(muminus, "MuonAllMinus","PidAlgoMdtHardCuts");
		theAnalysis->FillList(piplus,  "PionAllPlus","PidAlgoStt;PidAlgoMvd;PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc");
		theAnalysis->FillList(piminus, "PionAllMinus","PidAlgoStt;PidAlgoMvd;PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc");
		theAnalysis->FillList(kplus,  "PionAllPlus","PidAlgoStt;PidAlgoMvd;PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc");
		theAnalysis->FillList(kminus, "PionAllMinus","PidAlgoStt;PidAlgoMvd;PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc");
		theAnalysis->FillList(pplus,  "PionAllPlus","PidAlgoStt;PidAlgoMvd;PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc");
		theAnalysis->FillList(pminus, "PionAllMinus","PidAlgoStt;PidAlgoMvd;PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc");

		for (j=0;j<mctruth.GetLength();++j)
		{
			qa.qaCand("",mctruth[j],fMcTruth);
			fMcTruth->DumpData();
		}

		// *** momentum and theta histograms
		for (j=0;j<eplus.GetLength();++j)
		{
			fEPlus->Column("mct",theAnalysis->McTruthMatch(eplus[j]));
			fEPlus->Column("recocandfitstatus",eplus[j]->GetRecoCandidate()->GetFitStatus());
			qa.qaCand("",eplus[j],fEPlus);
			qa.qaMc("",eplus[j],fEPlus,false);
			qa.qaMcDiff("",eplus[j],fEPlus,false);
      qa.qaPid("",eplus[j],fEPlus);
      qa.qaTrk("",eplus[j],fEPlus);
			fEPlus->DumpData();
		}
		for (j=0;j<eminus.GetLength();++j)
		{
			fEMinus->Column("mct",theAnalysis->McTruthMatch(eminus[j]));
			fEMinus->Column("recocandfitstatus",eminus[j]->GetRecoCandidate()->GetFitStatus());
			qa.qaCand("",eminus[j],fEMinus);
			qa.qaMc("",eminus[j],fEMinus,false);
			qa.qaMcDiff("",eminus[j],fEMinus,false);
      qa.qaPid("",eminus[j],fEMinus);
      qa.qaTrk("",eminus[j],fEMinus);
			fEMinus->DumpData();
		}
		for (j=0;j<muplus.GetLength();++j)
		{
			fMuPlus->Column("mct",theAnalysis->McTruthMatch(muplus[j]));
			fMuPlus->Column("recocandfitstatus",muplus[j]->GetRecoCandidate()->GetFitStatus());
			qa.qaCand("",muplus[j],fMuPlus);
			qa.qaMc("",muplus[j],fMuPlus,false);
			qa.qaMcDiff("",muplus[j],fMuPlus,false);
      qa.qaPid("",muplus[j],fMuPlus);
      qa.qaTrk("",muplus[j],fMuPlus);
			fMuPlus->DumpData();
		}
		for (j=0;j<muminus.GetLength();++j)
		{
			fMuMinus->Column("mct",theAnalysis->McTruthMatch(muminus[j]));
			fMuMinus->Column("recocandfitstatus",muminus[j]->GetRecoCandidate()->GetFitStatus());
			qa.qaCand("",muminus[j],fMuMinus);
			qa.qaMc("",muminus[j],fMuMinus,false);
			qa.qaMcDiff("",muminus[j],fMuMinus,false);
      qa.qaPid("",muminus[j],fMuMinus);
      qa.qaTrk("",muminus[j],fMuMinus);
			fMuMinus->DumpData();
		}
		for (j=0;j<piplus.GetLength();++j)
		{
			fPiPlus->Column("mct",theAnalysis->McTruthMatch(piplus[j]));
			fPiPlus->Column("recocandfitstatus",piplus[j]->GetRecoCandidate()->GetFitStatus());
			qa.qaCand("",piplus[j],fPiPlus);
			qa.qaMc("",piplus[j],fPiPlus,false);
			qa.qaMcDiff("",piplus[j],fPiPlus,false);
      qa.qaPid("",piplus[j],fPiPlus);
      qa.qaTrk("",piplus[j],fPiPlus);
			fPiPlus->DumpData();
		}
		for (j=0;j<piminus.GetLength();++j)
		{
			fPiMinus->Column("mct",theAnalysis->McTruthMatch(piminus[j]));
			fPiMinus->Column("recocandfitstatus",piminus[j]->GetRecoCandidate()->GetFitStatus());
			qa.qaCand("",piminus[j],fPiMinus);
			qa.qaMc("",piminus[j],fPiMinus,false);
			qa.qaMcDiff("",piminus[j],fPiMinus,false);
      qa.qaPid("",piminus[j],fPiMinus);
      qa.qaTrk("",piminus[j],fPiMinus);
			fPiMinus->DumpData();
		}
		for (j=0;j<kplus.GetLength();++j)
		{
			fKPlus->Column("mct",theAnalysis->McTruthMatch(kplus[j]));
			fKPlus->Column("recocandfitstatus",kplus[j]->GetRecoCandidate()->GetFitStatus());
			qa.qaCand("",kplus[j],fKPlus);
			qa.qaMc("",kplus[j],fKPlus,false);
			qa.qaMcDiff("",kplus[j],fKPlus,false);
      qa.qaPid("",kplus[j],fKPlus);
      qa.qaTrk("",kplus[j],fKPlus);
			fKPlus->DumpData();
		}
		for (j=0;j<kminus.GetLength();++j)
		{
			fKMinus->Column("mct",theAnalysis->McTruthMatch(kminus[j]));
			fKMinus->Column("recocandfitstatus",kminus[j]->GetRecoCandidate()->GetFitStatus());
			qa.qaCand("",kminus[j],fKMinus);
			qa.qaMc("",kminus[j],fKMinus,false);
			qa.qaMcDiff("",kminus[j],fKMinus,false);
      qa.qaPid("",kminus[j],fKMinus);
      qa.qaTrk("",kminus[j],fKMinus);
			fKMinus->DumpData();
		}
		for (j=0;j<pplus.GetLength();++j)
		{
			fPPlus->Column("mct",theAnalysis->McTruthMatch(pplus[j]));
			fPPlus->Column("recocandfitstatus",pplus[j]->GetRecoCandidate()->GetFitStatus());
			qa.qaCand("",pplus[j],fPPlus);
			qa.qaMc("",pplus[j],fPPlus,false);
			qa.qaMcDiff("",pplus[j],fPPlus,false);
      qa.qaPid("",pplus[j],fPPlus);
      qa.qaTrk("",pplus[j],fPPlus);
			fPPlus->DumpData();
		}
		for (j=0;j<pminus.GetLength();++j)
		{
			fPMinus->Column("mct",theAnalysis->McTruthMatch(pminus[j]));
			fPMinus->Column("recocandfitstatus",pminus[j]->GetRecoCandidate()->GetFitStatus());
			qa.qaCand("",pminus[j],fPMinus);
			qa.qaMc("",pminus[j],fPMinus,false);
			qa.qaMcDiff("",pminus[j],fPMinus,false);
      qa.qaPid("",pminus[j],fPMinus);
      qa.qaTrk("",pminus[j],fPMinus);
			fPMinus->DumpData();
		}

	}

	// *** write out all the histos
	out->cd();

	fEPlus->GetInternalTree()->Write();
	fEMinus->GetInternalTree()->Write();
	fMuPlus->GetInternalTree()->Write();
	fMuMinus->GetInternalTree()->Write();
	fPiPlus->GetInternalTree()->Write();
	fPiMinus->GetInternalTree()->Write();
	fKPlus->GetInternalTree()->Write();
	fKMinus->GetInternalTree()->Write();
	fPPlus->GetInternalTree()->Write();
	fPMinus->GetInternalTree()->Write();
	fMcTruth->GetInternalTree()->Write();

	out->Save();

  if (gROOT->GetVersionInt() >= 60602) {
    gGeoManager->GetListOfVolumes()->Delete();
    gGeoManager->GetListOfShapes()->Delete();
    delete gGeoManager;
  }
  return 0;

}
