// created on 23.09.13 by Marcell Steinen steinen@kph.uni-mainz.de
// root macro to analyse the combined energy spectrum
//full chain description:
// root macro to create an energy spectrum of parts of simulations with a high number of events (memory leak somewhere :/); to be used with "script_Gamma_Ana_List_split.sh" on himster
// further analysis is done  via bash script "script_CombineAndAnalyzeGamma.sh" which combines the split up root output via "hadd" and calls root macro "AnalyseCombinedGammaFiles.C" to analyse the spectrum

#include<TH1D.h>
#include<TH2D.h>
#include<stdio.h>
#include<TString.h>
#include"TFile.h"
#include<iostream>
#include<fstream>
#include"TROOT.h"

using namespace std;

Double_t Compton(Double_t Energy, Double_t Th)
{
return Energy-Energy/(1+Energy/0.000511*(1-TMath::Cos(Th)));
}
Double_t invCompton(Double_t Energy, Double_t Eprime)
{
return TMath::ACos((Energy*Energy-Energy*Eprime-Eprime*0.000511)/(Energy*(Energy-Eprime)));
}
Double_t PeakFunc (Double_t *x, Double_t *par)
{
	return par[0]+x[0] * par[1]+x[0] * x[0] * par[2]+ par[3] * 1/(TMath::Sqrt(2 * TMath::Pi()) * par[5]) * TMath::Exp(- pow((x[0] -par[4]),2)/2/par[5]/par[5]); 
}
Double_t PoissonFunc(Double_t *x, Double_t *par)
{
	return par[1]*TMath::Poisson(x[0],par[0]);
}

int AnalyseCombinedGammaFiles(TString Filepath, TString FileName,Int_t NoOfJobs = 10)//TString FileListname_ext,TString OutfileName_ext)
{
	
	TString Anapath = Filepath;
	Anapath.Remove(Anapath.Last('/'),Anapath.Length()-Anapath.Last('/'));
	cout << Anapath << endl;
	TString InputName =Filepath.Remove(0,Filepath.Last('/')+1);
	
	if (InputName.Contains("TripleBall40Offset10STT"))
		TString Geometry = "35";
	if (InputName.Contains("TripleBall40Offset20STT"))
		TString Geometry = "36";	
	if (InputName.Contains("SecTar"))
		Int_t Sektar = 1;
	else
		Int_t Sektar = 0;
	
	TFile *File = new TFile(FileName.Data());
	File->ls();
	TH1D *hGammaSpectrum;
	TH1D *hNoOfHits;
	TH1D *hParBuffer;
	File->GetObject("gamTde;1",hGammaSpectrum);
	File->GetObject("Number of Hits;1",hNoOfHits);
	File->GetObject("ParameterBuffer;1",hParBuffer);
	Double_t Energy = hParBuffer->GetBinContent(1)/NoOfJobs;
	Int_t nEvents = hParBuffer->GetBinContent(2)/NoOfJobs;
	Int_t ChannelResolution = hParBuffer->GetBinContent(3)/NoOfJobs;
	Double_t Resolution = hParBuffer->GetBinContent(4)/NoOfJobs;
	cout << Energy << "  " << nEvents << "   " << ChannelResolution << "   " << Resolution << endl;
	
	ofstream txtfile;
	txtfile.open("OutputCombinedAna.txt");
	txtfile << "File analysed:\t" << InputName << endl;
	txtfile<< "Number of Simulated Events:\t"<<nEvents<<endl;
	
	
	
	TCanvas* can3 = new TCanvas("can3","germanium detector",0,0,1000,1000);
	hGammaSpectrum->Draw();
  
  
  //Analysis of spectrum
  
	Double_t lowAngle = invCompton(0.001332,0.001040);			//get the angle from the values of co60 for comparison
	Double_t highAngle = invCompton(0.001332,0.001096);			//get the angle from the values of co60 for comparison
	Int_t npeaks = 1;
	Int_t PeakToLook = 1;
	
	TSpectrum *s = new TSpectrum(npeaks);
	s->Search(hGammaSpectrum,npeaks,"new",0.01);

	Float_t *xpeaks = s->GetPositionX();
	Float_t *ypeaks = s->GetPositionY();
	
		//Print Peaks
	for (Int_t i = 0; i < npeaks; ++i)
	{ 
		cout <<"("<< xpeaks[i]<<"," << ypeaks[i] << ") ";
	}
	cout << endl;
	
	Double_t PeakX = xpeaks[PeakToLook-1]*ChannelResolution;
	Double_t PeakY = ypeaks[PeakToLook-1];
	Double_t DPeakY = sqrt(PeakY); 
	Double_t SumPeak = 0;
	
	for(Int_t i = PeakX-Resolution*10*10/2.3548; i <= PeakX+Resolution*10*10/2.3548; i++)		//Peak = 10 * Resolution of 1 crystal --> takes multiple hits into account (faster than fitting and than doing it again, error is small)
		SumPeak += hGammaSpectrum->GetBinContent(i);
	Double_t DSumPeak = sqrt(SumPeak);
	cout <<"MaxX: "<< PeakX << "; MaxY: " << PeakY << " +- " << DPeakY<< endl;
	cout << "SumPeak: " << SumPeak << " +- " << DSumPeak << endl;
	txtfile <<"MaxX: "<< PeakX << "; MaxY: " << PeakY << " +- " << DPeakY<< endl;
	txtfile << "SumPeak: " << SumPeak << " +- " << DSumPeak << endl;
	
	Double_t ComptonEdge = Int_t(PeakX*(1-1/(1+2*PeakX/5110)))+1;
	cout <<"ComptonEdge @ Bin: " << ComptonEdge << endl;
	Double_t SumCompton=0, DSumCompton = 0; 
	
	
  for (Int_t i =Compton(Energy,lowAngle)*ChannelResolution; i <= Compton(Energy,highAngle)*ChannelResolution; i++)
  {
		SumCompton += hGammaSpectrum->GetBinContent(i);
	}
	DSumCompton = sqrt(SumCompton);
	
	cout << "lowEnChan: " << Compton(Energy,lowAngle)*ChannelResolution << endl << "highEnChan: "<< Compton(Energy,highAngle)*ChannelResolution << endl;
	txtfile << "lowEnChan: " << Compton(Energy,lowAngle)*ChannelResolution << endl << "highEnChan: "<< Compton(Energy,highAngle)*ChannelResolution << endl;
	cout <<"N. of Compton Events: "<< SumCompton<< " +- " << DSumCompton << endl;
	txtfile <<"N. of Compton Events: "<< SumCompton<< " +- " << DSumCompton << endl;
	Double_t PeakToCompton = 0;
	Double_t DPeakToCompton = 0;
	if (SumCompton != 0)
	{
		PeakToCompton = PeakY/SumCompton*(Compton(Energy,highAngle)-Compton(Energy,lowAngle))*ChannelResolution;
		DPeakToCompton = sqrt(pow(DPeakY/SumCompton,2)+ pow(PeakY*DSumCompton/SumCompton/SumCompton,2))*(Compton(Energy,highAngle)-Compton(Energy,lowAngle))*ChannelResolution;
		cout << "Peak/Compton (res): "<< PeakToCompton << " +- "<< DPeakToCompton << endl; 
		txtfile << "Peak/Compton (res):\t"<< PeakToCompton << " +- "<< DPeakToCompton << endl;
	}
	else
	{
		
		cout << "SumCompton = 0 -> no PeakToCompton possible!"<<endl;
		txtfile << "SumCompton = 0 -> no PeakToCompton possible!"<<endl;
	}
	Double_t AllEntries = hGammaSpectrum->GetEntries();
	cout << "Entries: " << AllEntries << endl;
	txtfile << "Entries: " << AllEntries << endl;
	
	
	//fitting the histograms
	
	TF1 *GausBG = new TF1("GausBG","gausn"/*(0)+pol1(3)" */,(PeakX-100)/ChannelResolution,(PeakX+100)/ChannelResolution);
		GausBG->SetParName(0,"Ampl");
		GausBG->SetParName(1,"x0");
		GausBG->SetParName(2,"sigma");
		GausBG->SetParameter(0,PeakY/10000);
		GausBG->SetParameter(1,PeakX/Double_t(ChannelResolution));
		GausBG->SetParameter(2,100./Double_t(ChannelResolution));
		GausBG->SetParLimits(0,0.0000001, 1000000);
		GausBG->SetParLimits(1,(PeakX-10)/Double_t(ChannelResolution),(PeakX+10)/Double_t(ChannelResolution));
		GausBG->SetParLimits(2,1./Double_t(ChannelResolution),100./Double_t(ChannelResolution));
		/*GausBG->SetParameter(0,1);
		GausBG->SetParameter(1,1);
		GausBG->SetParameter(2,0);
		GausBG->FixParameter(2,0);
		GausBG->FixParameter(1,0);
		GausBG->FixParameter(0,0);
		*/
		GausBG->SetNpx(100000);
		GausBG->SetLineColor(kRed);	
	hGammaSpectrum->Fit(GausBG,"R");
	
	txtfile << "Fitparameter Spectrum (model: gausn):" << endl;
	for (Int_t i = 0; i < 3; i++)
	{
		txtfile << GausBG->GetParName(i) << ":\t" << GausBG->GetParameter(i) << endl;
	}
	Double_t FWHM = GausBG->GetParameter(2)*2.3548200;
	txtfile << "FWHM[keV]:\t" <<  FWHM*1000000 << endl;
	cout << "FWHM[keV]:\t" <<  FWHM*1000000 << endl;
	
	/*TF1 *Poisson = new TF1("Poisson","gausn",0,15);//,2);			//maybe gausn? maybe no fit? //gausn --> no ,2);
		//Poisson->SetParameter(0,4);
		//Poisson->SetParameter(1,SumPeak);
		//Poisson->SetParLimits(0,0,12);
		Poisson->SetParName(0,"Ampl");//Poisson->SetParName(0,"lambda");
		Poisson->SetParName(1,"x0");//Poisson->SetParName(1,"Ampl");
		Poisson->SetParName(2,"sigma");
		//Poisson->SetParameter(1,1000);
		Poisson->SetNpx(100000);
		Poisson->SetLineColor(kRed);	
	//hNoHits->Fit(Poisson);*/
	
	//txtfile << "Fitparameter Number of Hits (model: Ampl.*Poisson):" << endl;
	//for (Int_t i = 0; i < 3; i++)			// if gausn fit --> < 2 -> < 3 , if no fit, comment it out!!!
	//{
	//	txtfile << Poisson->GetParName(i) << ":\t" << Poisson->GetParameter(i) << endl;
	//}
	
	//some make-up for the histograms
	Double_t FullEnergyPeakEff = double(int(GausBG->GetParameter(0)*ChannelResolution/(nEvents*2)*100*1000))/1000;
	Double_t FullEnergyPeakEffError = double(int(GausBG->GetParError(0)*ChannelResolution/(nEvents*2)*100*1000))/1000;
	
	cout << "Full-Energy-Peak-Eff. [%]: " << FullEnergyPeakEff << endl;; // *2 because only 2Pi of solid angle is simulated
	txtfile << "Full-Energy-Peak-Eff. [%]:\t" << FullEnergyPeakEff << endl; // *2 because only 2Pi of solid angle is simulated 
	// writing to files and closing
	cout << "Error ofFull-Energy-Peak-Eff. [%]: " <<  FullEnergyPeakEffError<< endl; // *2 because only 2Pi of solid angle is simulated
	txtfile << "Error of Full-Energy-Peak-Eff. [%]:\t" << FullEnergyPeakEffError << endl; // *2 because only 2Pi of solid angle is simulated 
	// writing to files and closing 
	TFile *Output = new TFile("OutputCombinedAna.root","RECREATE");
	hGammaSpectrum->Write();
	hNoOfHits->Write();
	
	txtfile.close();
	Output->Close();
	File->Close();
	
	TString CollectFileName = Anapath +"/CollectFEPF.txt";
	ofstream CollectFile(CollectFileName, ios::out|ios::app);			//collect name and Full Energy Eff of all Sims here
		CollectFile <<  Geometry << "\t" << Sektar <<  "\t"<< Energy << "\t"<< nEvents << "\t" << FullEnergyPeakEff << "\t" << FullEnergyPeakEffError << "\t" <<FWHM*1000000 << "\t"<< PeakToCompton<< "\t"<<InputName << endl;
	
	CollectFile.close();
	
	cout << "macro finished successfully" << endl;
	
  return 0;
}

