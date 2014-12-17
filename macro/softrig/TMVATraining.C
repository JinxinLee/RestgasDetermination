int gettype(TTree *t, TString varname)
{
	if (t->GetBranch(varname)==0) return -1;
	
	TString leaftype = t->GetLeaf(varname)->GetTypeName();
	
	if (leaftype=="Float_t")	 return 0; 
	else if (leaftype=="Int_t")	 return 1;  
	else if (leaftype=="Bool_t") return 2;
	
	return -1;
}


int SplitString(TString s, TString delim, TString *toks, int maxtoks)
{
	TObjArray *tok = s.Tokenize(delim);
	int N = tok->GetEntries();	
	for (int i=0;i<N;++i) 
		if (i<maxtoks) 
		{
			toks[i] = ((TObjString*)tok->At(i))->String();
			toks[i].ReplaceAll("\t","");
			toks[i] = toks[i].Strip(TString::kBoth);
		}
	return N;
}


void TMVATraining(TString fname, TString vars, TString treename="")
{
	TString sigcut = "tag&&mode%1000!=900";
	TString bkgcut = "tag&&mode%1000==900";
	
	TRegexp rntp("n[0-9][0-9][0-9]");
	if (treename=="") treename = fname(rntp);
	
	TFile *f = TFile::Open(fname);
	TTree *t =(TTree*) f->Get(treename);
	
	TFile* outputFile = TFile::Open( "TMVA_"+treename+".root", "RECREATE" );
	
	TMVA::Factory *factory = new TMVA::Factory( "MVAnalysis", outputFile,"!V:Transformations=I;N;D");

	TString toks[30];
	int N = SplitString(vars," ",toks,30);
	
	for (int i=0;i<N;++i)
	{
		int btype = gettype(t, toks[i]);
		if (btype==0 || btype==1)
			factory->AddVariable(toks[i], btype==0?'F':'I');
	}   
	
	factory->AddTree( t, "Signal",     1.0, sigcut.Data());
	factory->AddTree( t, "Background", 1.0, bkgcut.Data());
	
	int nsig = t->GetEntries(sigcut);
	int nbkg = t->GetEntries(bkgcut);
	
// 	factory->AddSignalTree ( (TTree*)input->Get("TreeS") );
// 	factory->AddBackgroundTree ( (TTree*)input->Get("TreeB") );

	factory->PrepareTrainingAndTestTree( "", "",
						TString::Format("nTrain_Signal=%d:nTrain_Background=%d:nTest_Signal=%d:nTest_Background=%d:!V",int(nsig*0.6),int(nbkg*0.6), int(nsig*0.35),int(nbkg*0.35)));

// 	factory->BookMethod( TMVA::Types::kLikelihood, "Likelihood",
// 	"!V:NAvEvtPerBin=50" );
// 
// 	factory->BookMethod( TMVA::Types::kMLP, "MLP", 
// 	"!V:NCycles=50:HiddenLayers=10,10:TestRate=5" );

	factory->BookMethod( TMVA::Types::kBDT, "BDT", 
	"!V:BoostType=Grad:nCuts=20:NNodesMax=15" );
	
	factory->TrainAllMethods();  
	factory->TestAllMethods();
	factory->EvaluateAllMethods();
	
	outputFile->Close();
	std::cout << "Caution, this was example code, please edit for real use!"<<std::endl;
	delete factory;
}
