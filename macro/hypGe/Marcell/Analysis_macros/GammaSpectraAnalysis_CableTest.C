// root macro to analyze the simulation output
//void convertMCPoints()

#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TH1D.h"
#include "TF1.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TTree.h"
#include <fstream>
#include <string>
#include "TVector3.h"

Double_t Compton(Double_t E, Double_t Th)
{
return E-E/(1+E/0.000511*(1-TMath::Cos(Th)));
}
Double_t invCompton(Double_t E, Double_t Eprime)
{
return TMath::ACos((E*E-E*Eprime-Eprime*0.000511)/(E*(E-Eprime)));
}

Double_t PeakFunc (Double_t *x, Double_t *par)
{
	return par[0]+x[0] * par[1]+x[0] * x[0] * par[2]+ par[3] * 1/(TMath::Sqrt(2 * TMath::Pi()) * par[5]) * TMath::Exp(- pow((x[0] -par[4]),2)/2/par[5]/par[5]); 
}

Double_t PoissonFunc(Double_t *x, Double_t *par)
{
	return par[1]*TMath::Poisson(x[0],par[0]);
}
int GammaSpectraAnalysis_CableTest(TString Filename)//, Double_t Energy) 
{

  // -----  Load libraries   ------------------------------------------------
//``gSystem->Load("fstream.h");
   gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  gSystem->Load("libHyp");
   gSystem->Load("libHypGe");
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

	//Get energy from filename
	
	Int_t IndexPreEnergy = Filename.Index("_",1,0,TString::kExact);
	Int_t IndexPostEnergy = Filename.Index("MeV",3,0,TString::kExact);
	TString EnergyFromFileName = Filename(IndexPreEnergy+1,IndexPostEnergy-IndexPreEnergy-1);
	//cout << EnergyFromFileName << endl;
	Double_t Energy = EnergyFromFileName.Atof()/1000;
	cout << "Gamma energy: " << Energy << endl;
	// the sim file you want to analyse
	//string Filename = "TripleBall40Offset10_1MeV_10000Evts"; //without File Type ending!!!
	//Double_t Energy = 0.001;
	if(Filename.EndsWith(".root"))
	{
		Filename.ReplaceAll(".root","");
		cout << "Filename ending chopped!" << endl;
	}
	TString CompleteFilename = "$SIMDATADIR/GeantTest/"+Filename+".root";
	TFile* g = new TFile(CompleteFilename);
	
//Output Files
	TString Path = getenv("SIMDATADIR");
	TString outfile= Path+"/GeantTest/Ana/Cable/Ana";
	outfile += Filename;
	TString txtfileName =  outfile;
	outfile +=".root";
	txtfileName += ".txt";
	TFile* fi = new TFile(outfile,"RECREATE");

	//cout << "Path" << Path << endl;
	//TString txtfileName = Path +"/Data_Marcell/"+Filename+"_Spectrum.txt";
	//cout << txtfileName << endl;
	ofstream txtfile;
	txtfile.open(txtfileName);
	txtfile << "File read:" << CompleteFilename << endl;
   //photons from hyp electromag. decay
  TTree *b=(TTree *) g->Get("cbmsim") ;
  TClonesArray* hit_bar=new TClonesArray("PndHypGePoint");
  b->SetBranchAddress("HypGePoint",&hit_bar);//Branch names
  TClonesArray* mc_bar=new TClonesArray("PndMCTrack");
  b->SetBranchAddress("MCTrack",&mc_bar);//Branch names


      
  
  //****photons from hyp elect. decay
	Double_t HistoUpperThreshold = Energy*1.2;
	TString Name = "total gam energy deposit, " + Filename;
	TH1D* gamTde = new TH1D("gamTde",Name.Data(),10000000*HistoUpperThreshold,0.0000,HistoUpperThreshold);
 
  
	bool verbose = false;
	Int_t MotherId,Motherpdg;
	
	TH1D *hNoHits = new TH1D("Number of Hits", "Number of Hits", 10,0,10);	
	
	TVector3 vecs , pos;
	int mcpdg = -1,ev;
	Double_t mult,En,Eng,Enth;
	
	//vector<int> event;
	int count;
	set<int> SetOfCrystalHit;
	set<int>::iterator it;
	
	Int_t nEvents = b->GetEntriesFast();
	cout<< "Number of Simulated Events: "<<nEvents<<endl;
	txtfile<< "Number of Simulated Events: "<<nEvents<<endl;
	
	Double_t Resolution = 2.;	//keV
	for (Int_t k=0; k<nEvents; k++)
	{ 
			//cout << k << endl;
		Eng=0.;
		b->GetEntry(k);
		if (!((k*10)% nEvents))
		{
			cout << k << endl;
		}
	    //if(verbose) cout<<"Event No "<<j<<endl;
		for (Int_t i=0; i<hit_bar->GetEntriesFast(); i++)
		{ 
			//cout << hit_bar->GetEntriesFast()<<endl;
			PndHypGePoint *hitgam=(PndHypGePoint*)hit_bar->At(i);
		
			PndMCTrack *mcgam = (PndMCTrack*)mc_bar->At(hitgam->GetTrackID());
			Eng +=gRandom->Gaus(0,0.000001*Resolution/2.3548);
			Eng =Eng + (hitgam->GetEnergyLoss());
			SetOfCrystalHit.insert(hitgam->GetDetectorID());
			//cout <<"DetID" << hitgam->GetDetectorID()<< endl;
		}//end for i (points in event)
	    //count =0;
			//cout <<Eng<<endl;
			//cout << TMath::Abs(Eng-Energy) << "\t" << 3*0.000001*Resolution/2.3548 << endl;
		if (TMath::Abs(Eng-Energy) < 10*0.000001*Resolution/2.3548) 			//Peak = 10 * Resolution of 1 crystal --> takes multiple hits into account (faster than fitting and than doing it again, error is small)
		{			
			//std::cout << "SetOfCrystalHit contains:";
			//for (it=SetOfCrystalHit.begin(); it!=SetOfCrystalHit.end(); ++it)
			//{
				//std::cout << ' ' << *it;
			//}
			//std::cout << " and is " << SetOfCrystalHit.size() << " long\n";
			hNoHits->Fill(SetOfCrystalHit.size());
			//hNoHits->Fill(hit_bar->GetEntriesFast());		//Fill # of Hits -diagramm with events inside 3 sigma of the peak
			//cout << hit_bar->GetEntriesFast()<<endl;
		}
		if(Eng>0)
		{
			gamTde->Fill(Eng);		//Fill spectrum
		}
	    
		SetOfCrystalHit.clear();
	}// end for j (events)
  cout << "event loop finished"<< endl;

  
	TCanvas* can3 = new TCanvas("can3","germanium detector",0,0,1000,1000);
 
  //gamTde->Draw();
  
  //Analysis of spectrum
  
	Double_t lowAngle = invCompton(0.001332,0.001040);
	Double_t highAngle = invCompton(0.001332,0.001096);
	Int_t npeaks = 1;
	Int_t PeakToLook = 1;
	Double_t E = Energy;
	
	TSpectrum *s = new TSpectrum(npeaks);
	s->Search(gamTde,npeaks,"new",0.01);

	Float_t *xpeaks = s->GetPositionX();
	Float_t *ypeaks = s->GetPositionY();
	
		//Print Peaks
	for (Int_t i = 0; i < npeaks; ++i)
	{ 
		cout <<"("<< xpeaks[i]<<"," << ypeaks[i] << ") ";
	}
	cout << endl;
	
	Double_t PeakX = xpeaks[PeakToLook-1]*10000000;
	Double_t PeakY = ypeaks[PeakToLook-1];
	Double_t DPeakY = sqrt(PeakY); 
	Double_t SumPeak = 0;
	
	for(Int_t i = PeakX-Resolution*10*10/2.3548; i <= PeakX+Resolution*10*10/2.3548; i++)		//Peak = 10 * Resolution of 1 crystal --> takes multiple hits into account (faster than fitting and than doing it again, error is small)
		SumPeak += gamTde->GetBinContent(i);
	Double_t DSumPeak = sqrt(SumPeak);
	cout <<"MaxX: "<< PeakX << "; MaxY: " << PeakY << " +- " << DPeakY<< endl;
	cout << "SumPeak: " << SumPeak << " +- " << DSumPeak << endl;
	txtfile <<"MaxX: "<< PeakX << "; MaxY: " << PeakY << " +- " << DPeakY<< endl;
	txtfile << "SumPeak: " << SumPeak << " +- " << DSumPeak << endl;
	
	Double_t ComptonEdge = Int_t(PeakX*(1-1/(1+2*PeakX/5110)))+1;
	cout <<"ComptonEdge @ Bin: " << ComptonEdge << endl;
	Double_t SumCompton=0, DSumCompton = 0; 
	
	
  for (Int_t i =Compton(E,lowAngle)*10000000; i <= Compton(E,highAngle)*10000000; i++)
  {
		SumCompton += gamTde->GetBinContent(i);
	}
	DSumCompton = sqrt(SumCompton);
	
	cout << "lowEnChan: " << Compton(E,lowAngle)*10000000 << endl << "highEnChan: "<< Compton(E,highAngle)*10000000 << endl;
	txtfile << "lowEnChan: " << Compton(E,lowAngle)*100000000 << endl << "highEnChan: "<< Compton(E,highAngle)*10000000 << endl;
	cout <<"N. of Compton Events: "<< SumCompton<< " +- " << DSumCompton << endl;
	txtfile <<"N. of Compton Events: "<< SumCompton<< " +- " << DSumCompton << endl;
	Double_t PeakToCompton = 0;
	Double_t DPeakToCompton = 0;
	if (SumCompton != 0)
	{
		PeakToCompton = PeakY/SumCompton*(Compton(E,highAngle)-Compton(E,lowAngle))*10000000;
		DPeakToCompton = sqrt(pow(DPeakY/SumCompton,2)+ pow(PeakY*DSumCompton/SumCompton/SumCompton,2))*(Compton(E,highAngle)-Compton(E,lowAngle))*10000000;
		cout << "Peak/Compton (res): "<< PeakToCompton << " +- "<< DPeakToCompton << endl; 
		txtfile << "Peak/Compton (res): "<< PeakToCompton << " +- "<< DPeakToCompton << endl;
	}
	else
	{
		
		cout << "SumCompton = 0 -> no PeakToCompton possible!"<<endl;
		txtfile << "SumCompton = 0 -> no PeakToCompton possible!"<<endl;
	}
	Double_t AllEntries = gamTde->GetEntries();
	cout << "Entries: " << AllEntries << endl;
	txtfile << "Entries: " << AllEntries << endl;
	
	
	//fitting the histograms
	
	TF1 *GausBG = new TF1("GausBG","gausn"/*(0)+pol1(3)" */,(PeakX-100)/10000000,(PeakX+100)/10000000);
		GausBG->SetParName(0,"Ampl");
		GausBG->SetParName(1,"x0");
		GausBG->SetParName(2,"sigma");
		GausBG->SetParameter(0,PeakY/10000);
		GausBG->SetParameter(1,PeakX/10000000.);
		GausBG->SetParameter(2,100./10000000.);
		GausBG->SetParLimits(0,0.0000001, 1000000);
		GausBG->SetParLimits(1,(PeakX-10)/10000000.,(PeakX+10)/10000000.);
		GausBG->SetParLimits(2,1./10000000.,100./10000000.);
		/*GausBG->SetParameter(0,1);
		GausBG->SetParameter(1,1);
		GausBG->SetParameter(2,0);
		GausBG->FixParameter(2,0);
		GausBG->FixParameter(1,0);
		GausBG->FixParameter(0,0);
		*/
		GausBG->SetNpx(100000);
		GausBG->SetLineColor(kRed);	
	gamTde->Fit(GausBG,"R");
	
	txtfile << "Fitparameter Spectrum (model: gausn):" << endl;
	for (Int_t i = 0; i < 3; i++)
	{
		txtfile << GausBG->GetParName(i) << ":\t" << GausBG->GetParameter(i) << endl;
	}
	Double_t FWHM = GausBG->GetParameter(2)*2.3548200;
	txtfile << "FWHM[keV]:\t" <<  FWHM*1000000 << endl;
	cout << "FWHM[keV]:\t" <<  FWHM*1000000 << endl;
	
	TF1 *Poisson = new TF1("Poisson","gausn",0,15);//,2);			//maybe gausn? maybe no fit? //gausn --> no ,2);
		//Poisson->SetParameter(0,4);
		//Poisson->SetParameter(1,SumPeak);
		//Poisson->SetParLimits(0,0,12);
		Poisson->SetParName(0,"Ampl");//Poisson->SetParName(0,"lambda");
		Poisson->SetParName(1,"x0");//Poisson->SetParName(1,"Ampl");
		Poisson->SetParName(2,"sigma");
		//Poisson->SetParameter(1,1000);
		Poisson->SetNpx(100000);
		Poisson->SetLineColor(kRed);	
	//hNoHits->Fit(Poisson);
	
	//txtfile << "Fitparameter Number of Hits (model: Ampl.*Poisson):" << endl;
	//for (Int_t i = 0; i < 3; i++)			// if gausn fit --> < 2 -> < 3 , if no fit, comment it out!!!
	//{
	//	txtfile << Poisson->GetParName(i) << ":\t" << Poisson->GetParameter(i) << endl;
	//}
	
	//some make-up for the histograms
	gamTde->SetXTitle("Energy [GeV]");
	gamTde->SetYTitle("Counts");
	gamTde->GetYaxis()->SetTitleOffset(1.35);
	hNoHits->SetXTitle("Number of Hits");
	hNoHits->SetYTitle("Counts");
	hNoHits->GetYaxis()->SetTitleOffset(1.1);

	cout << "Full-Energy-Peak-Eff. [%]: " << double(int(GausBG->GetParameter(0)*10000000/(nEvents*2)*100*1000))/1000 << endl; // *2 because only 2Pi of solid angle is simulated
	txtfile << "Full-Energy-Peak-Eff. [%]: " << double(int(GausBG->GetParameter(0)*10000000/(nEvents*2)*100*1000))/1000 << endl; // *2 because only 2Pi of solid angle is simulated 
	// writing to files and closing
	cout << "Error ofFull-Energy-Peak-Eff. [%]: " << double(int(GausBG->GetParError(0)*10000000/(nEvents*2)*100*1000))/1000 << endl; // *2 because only 2Pi of solid angle is simulated
	txtfile << "Error of Full-Energy-Peak-Eff. [%]: " << double(int(GausBG->GetParError(0)*10000000/(nEvents*2)*100*1000))/1000 << endl; // *2 because only 2Pi of solid angle is simulated 
	// writing to files and closing 
	gamTde->Write();
	hNoHits->Write();
	GausBG->Write();
	Poisson->Write();
	fi->Close();
	txtfile.close();



	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is "    << outfile << endl;
	cout << "Parameter file is " << txtfileName << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------
	
  return 0;
}
