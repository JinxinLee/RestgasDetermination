int Pic_FED-Eff_lEE.C()
{
Double_t x[10],y[10],yE[];

ifstream Inputfile("/data/work/kpha1/steinen/Gamma/Ana/CombinedData/Fitted/FittedDataLowEnergy.txt");

	Int_t Geometry, Sektar, nEvents;
	Double_t Energy, FullEnergyPeakEff, FullEnergyPeakEffError, FWHM, FWHMError,FullEnergyPeakEffSEP, FullEnergyPeakEffSEPError,FullEnergyPeakEffSEPDEP, FullEnergyPeakEffSEPDEPError;
	char InputName[100];
	int counter = 0;
	while(Inputfile  >>  Geometry  >> Sektar >> Energy >> nEvents >>  FullEnergyPeakEff >>  FullEnergyPeakEffError >>FWHM >> FWHMError>> FullEnergyPeakEffSEP >> FullEnergyPeakEffSEPError>>FullEnergyPeakEffSEPDEP >> FullEnergyPeakEffSEPDEPError  )
	{
	x[counter] = Energy;
	y[counter] = FullEnergyPeakEff;
	yE[counter] = FullEnergyPeakEffError;
	cout <<x[counter] <<" " <<y[counter] << " " <<yE[counter]<<endl;
	}
	
  return 0;
}
