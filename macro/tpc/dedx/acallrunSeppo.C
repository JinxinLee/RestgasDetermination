acallrunSeppo()
{	
	gROOT->ProcessLine(".L macro/tpc/dedx/CChainGetter.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/PDGMap.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/DEDXFitCreator.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/DEDXDistribana.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/Coordinates.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/SeparationPower.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/SepPoHandler.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/Resolution.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/ResoHandler.C+");
	gROOT->ProcessLine(".L macro/tpc/dedx/seppo.C+");	
}

rebuild()
{	
	gROOT->ProcessLine(".L macro/tpc/dedx/CChainGetter.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/PDGMap.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/DEDXFitCreator.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/DEDXDistribana.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/Coordinates.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/SeparationPower.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/SepPoHandler.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/Resolution.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/ResoHandler.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/seppo.C++");	
}

acallrunSeppo(string Dir, string JobSumName, string FileType)
{	
	acallrunSeppo();	
	callrunSeppo(Dir, JobSumName,"DEDX",0.,0.6,FileType);
	
}
