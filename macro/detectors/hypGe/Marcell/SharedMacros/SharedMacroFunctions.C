// functions used in multiple macros

Double_t GetGeoFromFilename(TString Filename)
{
	Int_t IndexPreEnergy = Filename.Index("Geo",3,0,0);
	Int_t IndexPostEnergy = Filename.Index("_",1,IndexPreEnergy,0);
	TString EnergyFromFileName = Filename(IndexPreEnergy+3,IndexPostEnergy-IndexPreEnergy-3);
	return EnergyFromFileName.Atof();				// now in MeV
}

Double_t GetEnergyFromFilename(TString Filename)
{
	Int_t IndexPreEnergy = Filename.Index("E",1,4,0);
	Int_t IndexPostEnergy = Filename.Index("MeV",3,4,0);
	TString EnergyFromFileName = Filename(IndexPreEnergy+1,IndexPostEnergy-IndexPreEnergy-1);
	return EnergyFromFileName.Atof();				// now in MeV
}

Int_t GetNumberOfEventsInFileFromFilename(TString Filename)
{
	Int_t IndexPostEnergy = Filename.Index("MeV",3,4,0);
	Int_t IndexPreNEvts = Filename.Index("FileEvts",8,IndexPostEnergy,0);
	Int_t IndexPostNEvts = Filename.Index("_",1,IndexPreNEvts,0);
	TString NEvtsFromFileName = Filename(IndexPreNEvts+8,IndexPostNEvts-IndexPreNEvts-8);
	return NEvtsFromFileName.Atoi();//1000000;
}

Int_t GetNumberOfCombinedEventsFromFilename(TString Filename)
{
	Int_t IndexPostEnergy = Filename.Index("MeV",3,4,0);
	Int_t IndexPreNEvts = Filename.Index("Evts",4,IndexPostEnergy,0);
	Int_t IndexPostNEvts = Filename.Index("_",1,IndexPreNEvts,0);
	TString NEvtsFromFileName = Filename(IndexPreNEvts+4,IndexPostNEvts-IndexPreNEvts-4);
	return NEvtsFromFileName.Atoi();//1000000;
}

//not working properly
//Int_t GetNumberOfCombinedEventsFromFilenameNeutrons(TString Filename)
//{
//	Int_t IndexPostEnergy = Filename.Index("_",1,12,0);
//	Int_t IndexPreNEvts = Filename.Index("Evts",4,IndexPostEnergy,0);
//	Int_t IndexPostNEvts = Filename.Index("_",1,IndexPreNEvts,0);
//	TString NEvtsFromFileName = Filename(IndexPreNEvts+4,IndexPostNEvts-IndexPreNEvts-4);
//	return NEvtsFromFileName.Atoi();//1000000;
//}

Int_t GetIfTargetIsSimulated(TString Filename)
{
	Int_t IndexPostEnergy = Filename.Index("MeV",3,4,0);
	Int_t IndexPreNEvts = Filename.Index("_ST",3,IndexPostEnergy,0);
	Int_t IndexPostNEvts = Filename.Index(".root",5,IndexPreNEvts,0);
	TString NEvtsFromFileName = Filename(IndexPreNEvts+3,IndexPostNEvts-IndexPreNEvts-3);
	return NEvtsFromFileName.Atoi();
}

TString ComposeOutputFilename(char Type[4] ,Int_t Geometry,Double_t Energy,Int_t AllEvents, Int_t FileEvents, Int_t GenType,Int_t ST, Int_t JobNr,Bool_t OmegaMode=0,Double_t mu =0, Double_t Q = 0)
{
	char buf[200];
	if (OmegaMode)
		sprintf(buf,"%s_Geo%i_E%.4fMeV_Evts%i_FileEvts%i_Gen%i_ST%i_mu%.1lf,Q%.1lf__%i",Type,Geometry,Energy,AllEvents,FileEvents,GenType,ST,mu, Q,JobNr);
	else
		sprintf(buf,"%s_Geo%i_E%.3fMeV_Evts%i_FileEvts%i_Gen%i_ST%i__%i",Type,Geometry,Energy,AllEvents,FileEvents,GenType,ST,JobNr);
	return buf;
}
TString ComposeOutputFoldername(char Type[4] ,Int_t Geometry,Double_t Energy,Int_t AllEvents, Int_t FileEvents, Int_t GenType,Int_t ST,Bool_t OmegaMode=0,Double_t mu =0, Double_t Q = 0)
{
	char buf[200];
	if (OmegaMode)
		sprintf(buf,"%s_Geo%i_E%.4fMeV_Evts%i_FileEvts%i_Gen%i_ST%i_mu%.1lf,Q%.1lf",Type,Geometry,Energy,AllEvents,FileEvents,GenType,ST,mu, Q);
	else
		sprintf(buf,"%s_Geo%i_E%.3fMeV_Evts%i_FileEvts%i_Gen%i_ST%i",Type,Geometry,Energy,AllEvents,FileEvents,GenType,ST);
	return buf;
}

TString GeoFileChooser (Int_t Geometry)
{
	TString Geofilename;
	switch (Geometry)
	{
		case 2 	: GeoFilename = "hypGe_GeoMarcell_2er.root"; break;
		case 21 : GeoFilename = "hypGeGeoDoubleCluster_V3.root"; break;
		case 3 	: GeoFilename = "hypGe_GeoMarcell.root"; break;
		case 31 : GeoFilename = "hypGeGeoTripleCluster_V3.root"; break;
		case 32 : GeoFilename = "hypGeGeoTripleCluster_Straight.root"; break;
		case 33 : GeoFilename = "hypGeGeoTripleCluster_Ball40_Offset10.root"; break;
		case 34 : GeoFilename = "hypGeGeoTripleCluster_Ball40_Offset20.root"; break;
		case 35 : GeoFilename = "hypGeGeoTripleCluster_Ball40_Offset10_STTFitting.root"; break;
		case 36 : GeoFilename = "hypGeGeoTripleCluster_Ball40_Offset20_STTFitting.root"; break;
		case 41 : GeoFilename = "hypGeGeoDEGASstraight40_20.root"; break;
		case 42 : GeoFilename = "hypGeGeoBall40_6sym_Offset20.root"; break;
		case 43 : GeoFilename = "hypGeGeoDEGASstraight40_15.root"; break;
	}
	return GeoFilename;
}
