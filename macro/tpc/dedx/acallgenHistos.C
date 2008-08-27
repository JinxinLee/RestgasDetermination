//it needs the directory where the raw or reco file lies and the JobSumName of this file
//FileExtension is ".raw.root" or ".reco.root"
//e.g:  Dir=/nfs/data/panda-mc/nMCnDX/nMCnDX/nMCnDX/
// JobSumName=nMCnDX,  FileExtension=".reco.root"
bool acallgenHistos(string Dir, string JobSumName, string FileExtension, int i)
{
	gROOT->ProcessLine(".L macro/tpc/dedx/DEDXHistoProcessing.C");
	LoadClasses();

	cout << "calling Histo gen..." << endl;

	//bool ret=callgenHistos(Dir, JobSumName,"DEDX", 0.3, 0.7, FileExtension);
	//bool ret=callgenHistos(Dir, JobSumName,"DEDX", 0.3, 0.6, FileExtension);
	//bool ret=callgenHistos(Dir, JobSumName,"DEDX", 0.3, 0.8, FileExtension);
	//bool ret=callgenHistos(Dir, JobSumName,"DEDX", 0.2, 0.8, FileExtension);	
	bool ret=callgenHistos(Dir, JobSumName,"DEDX", 0., 0.6, FileExtension);
	//bool ret=callgenHistos(Dir, JobSumName,"DEDX", 0.1, 0.6, FileExtension);
	//bool ret=callgenHistos(Dir, JobSumName,"DEDX", 0.1, 0.7, FileExtension);
	//bool ret=callgenHistos(Dir, JobSumName,"DEDX", 0.1, 0.8, FileExtension);	
	//bool ret=callgenHistos(Dir, JobSumName,"DEDX", 0.1, 0.9, FileExtension);	

	return ret;
}