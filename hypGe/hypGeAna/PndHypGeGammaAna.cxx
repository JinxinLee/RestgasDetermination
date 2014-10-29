/******************************************************

Analysis Task created by M.Steinen steinen@kph.uni-mainz.de
Analysis of Gamma Simulation with hypGe detectors
*******************************************************/ 


#include "PndHypGeGammaAna.h"
#include <iostream>
#include <TMath.h>
#include "PndHypGePoint.h"
#include "PndMCTrack.h"
#include <time.h>
#include <TCanvas.h>
#include <TSpectrum.h>

//#include "RhoBase/TFactory.h"


using namespace std;


PndHypGeGammaAna::PndHypGeGammaAna()
	: FairTask("Panda HypGeGammaAna	 Task")
{
	
}
PndHypGeGammaAna::PndHypGeGammaAna(TString TxtFileName_Ext,Double_t GammaEnergy_Ext,Int_t nEvents, Int_t nPeaks_Ext,Int_t PeakToLook_Ext) 
	: FairTask("Panda HypGeGammaAna	 Task")
{
	TxtFileName=TxtFileName_Ext;
	GammaEnergy=GammaEnergy_Ext;
	NumberOfEvents= nEvents;
	nPeaks= nPeaks_Ext;
	PeakToLook = PeakToLook_Ext;
}

PndHypGeGammaAna::~PndHypGeGammaAna()
{
	delete fStorage;
	delete hGamEnergy;
	delete hNumberOfHits;
	delete Rng;
}


InitStatus PndHypGeGammaAna::Init()
{
	cout << "Hyp Ge Gamma Ana:\t" << "Starting Init of HypGe Gamma Ana" << endl;
	
	//----------get FairRootManager-------------------------------------
	FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndEmcHitProducer::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
   
	if (fVerbose == 3)
		cout << "Hyp Ge Gamma Ana:\t" << "Getting FairRootManager finished" << endl;
		
	//----------get input arrays---------------------------------------------
	fMcTr = (TClonesArray*) ioman->GetObject("MCTrack");
	//fMc = (TClonesArray*) ioman->GetObject("HypPoint");
	fGe = (TClonesArray*) ioman->GetObject("HypGePoint");
	
	if (fVerbose ==3)
		cout << "Hyp Ge Gamma Ana:\t" << "Getting input arrays finished" << endl;
		
	//-----------extracting file name--------------------------------------
	fName = ioman->GetInFile()->GetName();
	if (fVerbose == 3)
	{
		cout << "Hyp Ge Gamma Ana:\t" << "Full input file name with path: " << fName << endl;
		cout << "Hyp Ge Gamma Ana:\t" << "Chopping first " << fName.Last('/')+1 << " chars"<< endl;
	}
	fName.Remove(0,fName.Last('/')+1);
	if (fVerbose == 3)
		cout << "Hyp Ge Gamma Ana:\t" << "File name: " <<fName<< endl;

	if (fVerbose == 3)
		cout << "Hyp Ge Gamma Ana:\t" << "File name extraction finished" << endl;
		
	//----------create histograms------------------------------------------
	iHistoUpperLimit = GammaEnergy*1.2*1000;
	hGamEnergy = new TH1D("GamEnergy","Total gamma energy deposit, " + fName, 100000*iHistoUpperLimit,0.0,iHistoUpperLimit);
	
	hNumberOfHits= new TH1D("NumberOfHits", "Number of Hits, " + fName, 16,0,15);
	
	if (fVerbose == 3)
		cout << "Hyp Ge Gamma Ana:\t" << "Histogram creation finished" << endl;
	
	//----------set detector resolution---------------------------------------------
			//TO DO: more realistic resolution (smearing) modell 	M.Steinen
	Resolution = 0.000001* 2./2.3548;		// 2 keV FWHM in GeV and changed to gaussian sigma
	
	//----------initialize EvtCount----------------------------------------
	EvtCount = 0;
	
	//----------Get number of events from FairRootManager-----------------------
	
	if (NumberOfEvents== 0)
		NumberOfEvents= ioman->GetInChain()->GetEntries();
	cout << "Hyp Ge Gamma Ana:\tNumber of analized events: "<< NumberOfEvents << endl;
		
	//----------initialize random generator------------------------------------
	Rng = new TRandom(time(NULL));
	cout << "Hyp Ge Gamma Ana:\t" << "Init of HypGe Gamma Ana finished succesfully" << endl;
	
	//----------create storage in root file------------------------------------
	fStorage = new PndHypGeGammaAnaStorage("AnaData","Analys Data");
		fStorage-> SetAllEntries(NumberOfEvents);
						//fStorage-> SetInFile((ioman->GetInFile()->GetName()).Data());			// this line doen't work!!!!! why?!?!?!?!?!??!?!
	//----------open txt outputfile--------------------------------------------

	TxtFile.open(TxtFileName);
		TxtFile << "Hyp Ge Gamma Ana:\tFile to analize: \t"<< ioman->GetInFile()->GetName() << endl;
		TxtFile << "Hyp Ge Gamma Ana:\tNumber of analized events: \t"<< NumberOfEvents << endl;
}

void PndHypGeGammaAna::Exec(Option_t* opt)
{
	bufferEnergy =0;
	
	for (Int_t i=0; i < fGe->GetEntriesFast(); i++)
	{
		PndHypGePoint *hitgam=(PndHypGePoint*)fGe->At(i);
		PndMCTrack *mcgam = (PndMCTrack*)fMcTr->At(hitgam->GetTrackID());

		Double_t rande;
		if (hitgam->GetEnergyLoss() !=0)
			bufferEnergy +=Rng->Gaus(0,Resolution);
		bufferEnergy =bufferEnergy + (hitgam->GetEnergyLoss());
		
	}
	if (bufferEnergy > 0)
	{
		hGamEnergy->Fill(bufferEnergy*1000);
		if (fVerbose == 3)
			cout << "Hyp Ge Gamma Ana:\t" << "Energy " << bufferEnergy*1000 << endl;
	}

	if (TMath::Abs(bufferEnergy-GammaEnergy) < 10*Resolution) 			//Peak = 10 * Resolution of 1 crystal --> takes multiple hits into account (faster than fitting and than doing it again, error is small)
	{			
		hNumberOfHits->Fill(fGe->GetEntriesFast());		//Fill # of Hits -diagramm 
		if (fVerbose == 3)
			cout << "Hyp Ge Gamma Ana:\t" << "NoHFill: " << fGe->GetEntriesFast() << endl;
	}
	if (fVerbose == 3 && bufferEnergy != 0)
		cout << "Hyp Ge Gamma Ana:\t" << "Energy"<< bufferEnergy << endl;
	
	if (!((EvtCount++*10) % NumberOfEvents))
	{
		if (EvtCount == 1)
			cout << "Hyp Ge Gamma Ana:\t"<<"0  % done!" << endl;
		else
			cout << "Hyp Ge Gamma Ana:\t" << 100 * Double_t(EvtCount-1)/NumberOfEvents << " % done!" << endl;
	}
	if (fVerbose == 3)
		cout << "Hyp Ge Gamma Ana:\t" << "Event Number"<< EvtCount << endl;
}

void PndHypGeGammaAna::Finish()
{
	//-------------Analysis of the spectrum-------------------------------------
	
	//print number of entries in spectrum
	if (fVerbose == 2)
		cout << "Hyp Ge Gamma Ana:\tEntries in spetrum: " << hGamEnergy->GetEntries() <<endl;
	TxtFile << "Hyp Ge Gamma Ana:\tEntries in spetrum: \t" << hGamEnergy->GetEntries() <<endl;

	// get peak position, heigth, content from spectrum
		//get position of peak in spectrum

	TSpectrum *s = new TSpectrum(nPeaks);
	s->Search(hGamEnergy,0.01,"goffnodraw",0.01);

	Float_t *xpeaks = s->GetPositionX();
	Float_t *ypeaks = s->GetPositionY();

	if (fVerbose == 3)
	{
		for (Int_t i = 0; i < nPeaks; i++)
		{ 
			cout <<"Hyp Ge Gamma Ana:\tPeak @ ("<< xpeaks[i]<<"," << ypeaks[i] << ")" << endl;
		}
	}
	
	PeakX = xpeaks[PeakToLook-1]*100000;
	PeakY = ypeaks[PeakToLook-1];
	DPeakY = sqrt(PeakY); 
	SumPeak = 0;
	
		//get content of peak
	for(Int_t i = PeakX-Resolution*10*1000000*100; i <= PeakX+Resolution*10*1000000*100; i++)		//Peak = 10 * Resolution of 1 crystal --> takes multiple hits into account (faster than fitting and than doing it again, error is small)
		SumPeak += hGamEnergy->GetBinContent(i);
	DSumPeak = sqrt(SumPeak);
	if (fVerbose == 2)
	{
		cout <<"Hyp Ge Gamma Ana:\tPeak at bin "<< PeakX << endl;
		cout <<"Hyp Ge Gamma Ana:\tPeak bin height"<< PeakY << " ± " << DPeakY<< endl;
		cout <<"Hyp Ge Gamma Ana:\tSumPeak "<<SumPeak  << " ± " << DSumPeak << endl;
	}
	TxtFile <<"Hyp Ge Gamma Ana:\tPeak at bin \t"<< PeakX << endl;
	TxtFile <<"Hyp Ge Gamma Ana:\tPeak bin height\t"<< PeakY << " ± " << DPeakY<< endl;
	TxtFile <<"Hyp Ge Gamma Ana:\tSumPeak \t"<<SumPeak << " ± " << DSumPeak << endl;

	// get number of compton events
		// get limits for "compton range" from definition for ^{60}Co (between both compton edges)
	Double_t lowAngle = invCompton(0.001332,0.001040);
	Double_t highAngle = invCompton(0.001332,0.001096);
	for (Int_t i =Compton(GammaEnergy,lowAngle)*1000*100000; i <= Compton(GammaEnergy,highAngle)*1000*100000; i++)
  {
		SumCompton += hGamEnergy->GetBinContent(i);
	}
	DSumCompton = sqrt(SumCompton);
	
	if (fVerbose == 2)
	{
		cout <<"Hyp Ge Gamma Ana:\tCompton from bins [" << Compton(GammaEnergy,lowAngle)*1000*100000 << " to "<< Compton(GammaEnergy,highAngle)*1000*100000 << "]" << endl;
		cout <<"Hyp Ge Gamma Ana:\tNumber of compton events: "<< SumCompton<< " +- " << DSumCompton << endl;
	}
	TxtFile <<"Hyp Ge Gamma Ana:\tCompton range [bins] \t[" << Compton(GammaEnergy,lowAngle)*1000*100000 << " to "<< Compton(GammaEnergy,highAngle)*1000*100000 << "]" << endl;
	TxtFile <<"Hyp Ge Gamma Ana:\tNumber of compton events: \t"<< SumCompton<< " +- " << DSumCompton << endl;
	
	// calculate Peak/Compton ratio
	
	if (SumCompton != 0)
	{
		PeakToCompton = PeakY/SumCompton*(Compton(GammaEnergy,highAngle)-Compton(GammaEnergy,lowAngle))*1000*100000;
		DPeakToCompton = sqrt(pow(DPeakY/SumCompton,2)+ pow(PeakY*DSumCompton/SumCompton/SumCompton,2))*(Compton(GammaEnergy,highAngle)-Compton(GammaEnergy,lowAngle))*1000*100000;
		if (fVerbose == 2)
			cout << "Hyp Ge Gamma Ana:\tPeak/Compton : "<< PeakToCompton << " ± "<< DPeakToCompton << endl; 
		TxtFile << "Hyp Ge Gamma Ana:\tPeak/Compton : \t"<< PeakToCompton << " ± "<< DPeakToCompton << endl; 
	}
	else
	{
		if (fVerbose == 2)
			cout << "Hyp Ge Gamma Ana:\tSumCompton = 0 -> no PeakToCompton possible!"<<endl;
		TxtFile << "Hyp Ge Gamma Ana:\tSumCompton = 0 -> no PeakToCompton possible!"<<endl;
	}

	//fitting the spectrum						modell is not good!!!! M.Steinen
	GausBG = new TF1("GausBG","gausn",(PeakX-1000)/100000,(PeakX+1000)/100000);
		GausBG->SetParName(0,"Ampl");
		GausBG->SetParName(1,"x0");
		GausBG->SetParName(2,"sigma");
		GausBG->SetParameter(0,PeakY/10000);
		GausBG->SetParameter(1,PeakX/100000.);
		GausBG->SetParameter(2,100./100000.);
		GausBG->SetParLimits(0,0.000001, 1000000);
		GausBG->SetParLimits(1,(PeakX-100)/100000.,(PeakX+100)/100000.);
		GausBG->SetParLimits(2,1./100000.,1000./100000.);
		/*GausBG->SetParameter(0,1);
		GausBG->SetParameter(1,1);
		GausBG->SetParameter(2,0);
		GausBG->FixParameter(2,0);
		GausBG->FixParameter(1,0);
		GausBG->FixParameter(0,0);
		*/
		GausBG->SetNpx(100000);
		GausBG->SetLineColor(kRed);	
	hGamEnergy->Fit(GausBG,"R");
	cout << "Hyp Ge Gamma Ana:\tCHi²red of the fit \t"<<GausBG->GetChisquare() / GausBG->GetNDF() << endl;
	cout << "Hyp Ge Gamma Ana:\tCHi²red of the fit \t"<<GausBG->GetChisquare() <<"  "<< GausBG->GetNDF() << endl;
	TxtFile << "Hyp Ge Gamma Ana:\tCHi²red of the fit \t"<<GausBG->GetChisquare() / GausBG->GetNDF() << endl;
	TxtFile << "Hyp Ge Gamma Ana:\tFitparameter Spectrum (model: gausn):" << endl;
	for (Int_t i = 0; i < 3; i++)
	{
		TxtFile <<"Hyp Ge Gamma Ana:\t" << GausBG->GetParName(i) << ":\t" << GausBG->GetParameter(i) << endl;
	}
	if (fVerbose == 2)
	{
		cout << "Hyp Ge Gamma Ana:\tFitparameter Spectrum (model: gausn):" << endl;
		for (Int_t i = 0; i < 3; i++)
		{
			cout <<"Hyp Ge Gamma Ana:\t" << GausBG->GetParName(i) << ":\t" << GausBG->GetParameter(i) << endl;
		}
	}
	
	Double_t FWHM = GausBG->GetParameter(2)*2.3548200;
	if (fVerbose == 2)
		cout << "Hyp Ge Gamma Ana:\tFWHM[keV]: " <<  FWHM*1000 << endl;
	TxtFile << "Hyp Ge Gamma Ana:\tFWHM[keV]:\t" <<  FWHM*1000 << endl;
	
	if (fVerbose == 2)
		cout << "Hyp Ge Gamma Ana:\tFull-Energy-Peak-Eff. [%]: " << double(int(GausBG->GetParameter(0)*100000/(NumberOfEvents*2)*100*1000))/1000 << " ± " << double(int(GausBG->GetParError(0)*100000/(NumberOfEvents*2)*100*1000))/1000 << endl; // *2 because only 2Pi of solid angle is simulated
	TxtFile << "Hyp Ge Gamma Ana:\tFull-Energy-Peak-Eff. [%]: \t" << double(int(GausBG->GetParameter(0)*100000/(NumberOfEvents*2)*100*1000))/1000 << " ± " << double(int(GausBG->GetParError(0)*100000/(NumberOfEvents*2)*100*1000))/1000 << endl; // *2 because only 2Pi of solid angle is simulated 
	
		//--------------make up for the histograms----------------------------------
	hGamEnergy->SetXTitle("#gamma Energy [MeV]");
	hGamEnergy->SetYTitle("Counts");
	hGamEnergy->GetXaxis()->SetTitleSize(0.06);
	hGamEnergy->GetXaxis()->SetTitleOffset(0.7);
	hGamEnergy->GetXaxis()->SetLabelSize(0.045);
	//hGamEnergy->GetYaxis()->CenterTitle();
	hGamEnergy->GetYaxis()->SetTitleSize(0.06);
	hGamEnergy->GetYaxis()->SetTitleOffset(0.8);
	hGamEnergy->GetYaxis()->SetLabelSize(0.045);
	
	hNumberOfHits->SetXTitle(" Number of Crystals hit");
	hNumberOfHits->SetYTitle(" Counts");
	hNumberOfHits->GetXaxis()->SetTitleSize(0.06);
	hNumberOfHits->GetXaxis()->SetTitleOffset(0.7);
	hNumberOfHits->GetXaxis()->SetLabelSize(0.045);
	//hNumberOfHits->GetYaxis()->CenterTitle();
	hNumberOfHits->GetYaxis()->SetTitleSize(0.06);
	hNumberOfHits->GetYaxis()->SetTitleOffset(0.8);
	hNumberOfHits->GetYaxis()->SetLabelSize(0.045);
	
	//-------------writing and drawing the histograms-------------------------
	hGamEnergy->Write();
	hNumberOfHits->Write();
	TCanvas* CanGamEnergy = new TCanvas("CanGamEnergy", "Gamma Energy", 1000,600);
	CanGamEnergy->cd();
	hGamEnergy->Draw();
	TCanvas* CanNoH = new TCanvas("CanNoH", "Number of Hits", 1000,600);
	CanNoH->cd();
	hNumberOfHits->Draw();
	
	TCanvas* CanCombined = new TCanvas("CanCombined", "Combined Canvas", 1500,450);
	CanCombined->Divide(2,1);
	CanCombined->cd(1);
	hGamEnergy->Draw();
	CanCombined->cd(2);
	hNumberOfHits->Draw();
	CanCombined->Write();
	fStorage->Write();
	TxtFile.close();
	CanCombined->cd(1);
	cout << "Hyp Ge Gamma Ana:\tAnalysis finished succesfully" << endl;
}


Double_t PndHypGeGammaAna::Compton(Double_t E, Double_t Th)
{
return E-E/(1+E/0.000511*(1-TMath::Cos(Th)));
}
Double_t PndHypGeGammaAna::invCompton(Double_t E, Double_t Eprime)
{
return TMath::ACos((E*E-E*Eprime-Eprime*0.000511)/(E*(E-Eprime)));
}


ClassImp(PndHypGeGammaAna)
