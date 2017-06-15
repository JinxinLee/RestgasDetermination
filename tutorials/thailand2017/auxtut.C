// **** some auxilliary functions in auxtut.C ****
// - FairRunAna* initrun(TString prefix, TString outfile, int min=-1, int max=-1) --> Init FairRunAna
// - plotmyhistos()               --> Plots all histograms in current TDirectory on a autosized canvas
// - writemyhistos()              --> Writes all histos in current TFile 
// - fillM(RhoCandList l, TH1* h) --> Fill mass histogram h with masses of candidates in l
// **** some auxilliary functions in auxtut.C ****

bool checkfile(TString fn)
{
	bool fileok=true;
	TFile fff(fn); 
	if (fff.IsZombie()) fileok=false;
	TTree *t=(TTree*)fff.Get("cbmsim");
	if (t==0x0) fileok=false;
	
	if (!fileok) cout <<"Skipping broken file '"<<fn<<"'"<<endl;
	return fileok;
}

FairRunAna* initrun(TString prefix, TString outfile, int min=-1, int max=-1)
{
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
	
	// *** setup parameter database 	
	FairParRootFileIo* parIO = new FairParRootFileIo();
	parIO->open(inParFile);
	FairParAsciiFileIo* parIOPid = new FairParAsciiFileIo();
	parIOPid->open(pidParFile.Data(),"in");
	
	rtdb->setFirstInput(parIO);
	rtdb->setSecondInput(parIOPid);
	rtdb->setOutput(parIO);  
	
	fRun->SetOutputFile(outfile);
	
	return fRun;
}

void plotmyhistos(std::vector<TH1*> h, int maxy=800, double asp = 1.1)
{
	int N = h.size();
	int nx=sqrt(N);
	int ny=nx; 
	while(nx*ny<N) nx++;

	int dim = maxy/ny;
	
	TCanvas *c=new TCanvas("c","c",10,10,dim*nx*asp, dim*ny);
	
	c->Divide(nx,ny, 0.0005,0.0005);
	
	for (int i=0;i<N;++i)
	{
		c->cd(i+1);
		h[i]->Draw();
	}
}

void plotmyhistos(int maxy=800, double asp = 1.1)
{
	TList *hl=gDirectory->GetList();
	std::vector<TH1*> h;
	for (int i=0;i<hl->GetSize();++i)
	{
		TString cn = hl->At(i)->ClassName();
		if (cn.BeginsWith("TH1") || cn.BeginsWith("TH2")) h.push_back((TH1*)hl->At(i));
	}

	int N = h.size();	
	if (N==0) return;
	int nx=sqrt(N);
	int ny=nx; 
	while(nx*ny<N) nx++;

	int dim = maxy/ny;
	if (dim>500) dim=500;
	
	TCanvas *c=new TCanvas("c","c",10,10,dim*nx*asp, dim*ny);
	
	c->Divide(nx,ny, 0.0005,0.0005);
	
	for (int i=0;i<N;++i)
	{
		c->cd(i+1);
		h[i]->Draw();
	}
}

int writemyhistos(int maxy=800, double asp = 1.1)
{
	TList *hl=gDirectory->GetList();
	std::vector<TH1*> h;
	for (int i=0;i<hl->GetSize();++i)
	{
		TString cn = hl->At(i)->ClassName();
		if (cn.BeginsWith("TH1") || cn.BeginsWith("TH2")) h.push_back((TH1*)hl->At(i));
	}

	int N = h.size();
	if (N==0) return 0;
	
	for (int i=0;i<N;++i) h[i]->Write();
	
	return N;
}

int fillM(RhoCandList &l, TH1* h)
{
	for (int i=0;i<l.GetLength();++i) h->Fill(l[i]->M());
	
	return l.GetLength();
}

int fillP(RhoCandList &l, TH1* h)
{
	for (int i=0;i<l.GetLength();++i) h->Fill(l[i]->P());
	
	return l.GetLength();
}
