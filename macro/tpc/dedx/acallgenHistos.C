//it needs the directory where the raw or reco file lies and the JobSumName of this file
//FileExtension is ".raw.root" or ".reco.root"
bool acallgenHistos(string Dir, string JobSumName, string FileExtension, int i)
{
	gROOT->ProcessLine(".L macro/tpc/dedx/CChainGetter.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/PDGMap.C++");	
	gROOT->ProcessLine(".L macro/tpc/dedx/HistogramSet.C++");		
	gROOT->ProcessLine(".L macro/tpc/dedx/DEDXFitCreator.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFit.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/GausFitCenter.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/FitServices.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/PDGHistos.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/genHistos.C++");

	cout << "calling Histo gen..." << endl;

	bool ret=callgenHistos(Dir, JobSumName,"DEDX", 0., 0.6, FileExtension);
	return ret;
}