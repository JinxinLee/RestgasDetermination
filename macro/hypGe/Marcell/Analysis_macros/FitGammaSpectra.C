using namespace std;

void FitGammaSpectra(TString Filename="Combined_Ana_Geo36_E1.000MeV_Evts10000000_FileEvts50000_Gen1_ST12.root")
{
	//gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	gROOT->LoadMacro("$VMCWORKDIR/macro/hypGe/Marcell/SharedMacros/SharedMacroFunctions.C");
	gSystem->Load("libHypGe");

	TString Path = getenv("SIMDATADIR");
	TString FullPath= Path+"/Gamma/Ana/CombinedData/";
	TString OutputFullPath= Path+"/Gamma/Ana/CombinedData/Fitted/";
	TString FilenameWithPath= FullPath+Filename;
	//TString TxtFilename = OutputFullPath +"FittedData.txt";
	//TString TxtFilename = OutputFullPath +"FittedDataLowEnergy.txt"; // changed output file for low energy tests (20.07.15 steinen)
	TString TxtFilename = OutputFullPath +"FittedDataNewGeometry.txt"; // changed output file for low energy tests (20.07.15 steinen)
	TString RootFilename = OutputFullPath+"Fitted_"+Filename;



	//get energy from filename
	Double_t energy = GetEnergyFromFilename(FilenameWithPath);
	Int_t CombinedEvents=GetNumberOfCombinedEventsFromFilename(Filename);
	Int_t Geo = GetGeoFromFilename(Filename);
	Int_t Sektar = GetIfTargetIsSimulated(Filename);
	Int_t nEvents = GetNumberOfCombinedEventsFromFilename(Filename);
	cout << nEvents << endl;
	cout << energy << endl;
	// get Spectrum from file
	TFile *file = new TFile(FilenameWithPath);
	TH1D* hEnergySpec;
	file ->GetObject("GamEnergy",hEnergySpec);

//	PndHypGeSpectrumAnalyser *Ana = new PndHypGeSpectrumAnalyser(hEnergySpec, Energies, 100);
//		Ana->SetOutputPath(OutputFullPath);
//		Ana->SetTxtFileOutputName(TxtFilename);
//		Ana->SetRootFileOutputName(RootFilename);
//		Ana->SetGaussianFitting();
//		Ana->SetInputHistogramResolution(0.0001);
//		Ana->SetNumberOfSimEvents(CombinedEvents, 1);

	int NPeaks;
	if (energy > 1.022)
	{
		NPeaks = 3;
	}
	else
		if (energy > 0.511)
		{
			NPeaks=2;
		}
		else
			NPeaks = 1;

	TF1 *Gausfunc[3];
	TF1 *fitfunc[3];
	Double_t FWHM[3];
	Double_t FWHMError[3];
	Double_t Efficiency[3];
	Efficiency[1] = 0;
	Efficiency[2] = 0;
	Double_t EfficiencyError[3];
	EfficiencyError[1]=0;
	EfficiencyError[2]=0;
	char buf[20];
	for(int i = 0; i<NPeaks;i++)
	{
		sprintf(buf, "gausfunc_%d",i);
		Gausfunc[i] = new TF1(buf, "gausn(0)", energy-i*0.511-0.02,energy-i*0.511+0.02);
		Gausfunc[i]-> SetParameter(0, 100);
		Gausfunc[i]-> SetParLimits(0,1,300);

		Gausfunc[i]-> SetParameter(1, energy-i*0.511);
		Gausfunc[i]-> SetParLimits(1,energy-i*0.511-0.01,energy-i*0.511+0.01);
		Gausfunc[i]-> SetParameter(2, 0.0004);
		Gausfunc[i]-> SetParLimits(2,0.00001,0.01);

		hEnergySpec->Fit(buf,"rN");
		sprintf(buf, "fitfunc_%d",i);
	fitfunc[i] = new TF1(buf, "gausn(0)+pol1(3)", energy-i*0.511-0.02,energy-i*0.511+0.02);
		fitfunc[i]-> SetParameter(0, Gausfunc[i]-> GetParameter(0));
		fitfunc[i]-> SetParLimits(0,Gausfunc[i]-> GetParameter(0)*0.9,Gausfunc[i]-> GetParameter(0)*1.1);
		//fitfunc[i]-> SetParLimits(1,10,300);
		fitfunc[i]-> SetParameter(1, Gausfunc[i]-> GetParameter(1));
		fitfunc[i]-> SetParLimits(1,Gausfunc[i]->GetParameter(1)*0.9,Gausfunc[i]-> GetParameter(1)*1.1);
		fitfunc[i]-> SetParameter(2, Gausfunc[i]-> GetParameter(2));
		fitfunc[i]-> SetParLimits(2,Gausfunc[i]->GetParameter(2)*0.9,Gausfunc[i]-> GetParameter(2)*1.1);

	hEnergySpec->Fit(buf,"r+");

		FWHM[i] = fitfunc[i]->GetParameter(2)*2.3548;
		FWHMError[i] = fitfunc[i]->GetParError(2)*2.3548;
		Efficiency[i] =fitfunc[i]->GetParameter(0)*1000000/2/nEvents;
		EfficiencyError[i] = fitfunc[i]->GetParError(0)*1E6/2/nEvents;
		//delete Ana;
		cout << "eff " << i << "\t" << Efficiency[i] << endl;
		cout << "effError " << i << "\t" << EfficiencyError[i] << endl;
	}
	Double_t EffInclSEP = Efficiency[0]+Efficiency[1];
	Double_t EffInclSEPError = sqrt(EfficiencyError[0]*EfficiencyError[0]+EfficiencyError[1]*EfficiencyError[1]);
	Double_t EffInclSEPDEP = Efficiency[0]+Efficiency[1]+Efficiency[2];
	Double_t EffInclSEPDEPError = sqrt(EfficiencyError[0]*EfficiencyError[0]+EfficiencyError[1]*EfficiencyError[1]+EfficiencyError[2]*EfficiencyError[2]);

	cout << "eff1 " <<   "\t" << Efficiency[0] << endl;
	cout << "eff2 " <<   "\t" << EffInclSEP << endl;
	cout << "eff3 " <<  "\t" << EffInclSEPDEP << endl;
		ofstream txtfile (TxtFilename.Data(),std::ofstream::out | std::ofstream::app);
		txtfile  <<  Geo <<"\t" << Sektar <<"\t"<< energy <<"\t"<< nEvents <<"\t"<<  Efficiency[0] <<"\t"<<  EfficiencyError[0] <<"\t"<<FWHM[0] <<"\t"<< FWHMError[0]<< "\t" << EffInclSEP <<"\t" << EffInclSEPError <<"\t" << EffInclSEPDEP <<"\t" << EffInclSEPDEPError <<endl;
		txtfile.close();
		cout << RootFilename.Data()<< endl;
//		TFile *fileOut = new TFile(RootFilename,"RECREATE",RootFilename);
//			hEnergySpec->Write();
//			fileOut->Close();
		file->Close();
}
