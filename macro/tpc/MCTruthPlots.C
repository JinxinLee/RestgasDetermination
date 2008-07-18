#include <map>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TCut.h>
#include <TStyle.h>
#include <sstream>
#include "CbmMCTrack.h"

void Draw_Alt(TTree *pTree, TCanvas *pCanvas, int nStartCanvas, int nPdgCode, int m);
void Draw_ParticleDistribution(TTree *pTree, TCanvas *pCanvas, int nCanvas, int m);
map<Int_t, string> PDGMap;

void run(TString filename)
{	
	//________________________________________
	//---------insert Particles in the PDGMap
	//PDGMap[111]="Pi_0";	//TODO: kein Fehler bei nicht existierenden Teilchen
	PDGMap[-13]="Muon+";	
	PDGMap[-11]="e+";
	PDGMap[11]="e-";
	PDGMap[14]="Neutrino";
	PDGMap[22]="Gamma";
	PDGMap[211]="Pi+";
	PDGMap[0]="Summary";	//TODO: Summary 
	
	//________________________________________
	//--------Load the Tree
	//"/afs/e18/user/vmichael/data/test1.mc.root"
	TFile *pFile=new TFile(filename, "READ");
	assert(pFile);
	TTree *pTree = (TTree*)pFile->Get("cbmsim");
	assert(pTree);
	
	//________________________________________
	//--------Set Colors
	gStyle->SetPalette(1);

	//________________________________________
	//-------Paint a Canvas for every entry in the PDGMap
	map<Int_t, string>::const_iterator PDGMapCit;
	Int_t m=0;
	for(PDGMapCit=PDGMap.begin();PDGMapCit!=PDGMap.end();PDGMapCit++, m++)	
	{
		Int_t nPdgCode=PDGMapCit->first;
		string strParticleName=PDGMapCit->second;
		
		
		printf("Particle: %s (%d)\n\a", strParticleName.c_str(), m);
		
		//________________________________________
		//----------- Create the Canvas
		ostringstream CanvasName;
		CanvasName << "canvas" << m; 
		TCanvas *pCanvas = new TCanvas(CanvasName.str().c_str(), strParticleName.c_str(),30*m,30*m,1000,700);
		pCanvas->Divide(3,3);
		
		//________________________________________
		//---------- Draw the Particle Distribution in the 1. Subpad
		Draw_ParticleDistribution(pTree, pCanvas, 1, m);
		
		//________________________________________
		//---------- Create the histograms and set their names
		ostringstream MomentumHistogramName;
		MomentumHistogramName << "Momentum " << strParticleName.c_str();
		TH1F *pMomHist = new TH1F(MomentumHistogramName.str().c_str(), "|P|" , 100, -4.0, 4.0);
		pMomHist->GetXaxis()->SetTitle("|P|");
		
		ostringstream ThetaHistogramName;
		ThetaHistogramName << "Theta " << strParticleName.c_str();
		TH1F *pThetaHist = new TH1F(ThetaHistogramName.str().c_str(), "Theta" , 100, -4.0, 4.0);
		pThetaHist->GetXaxis()->SetTitle("Theta");
		
		ostringstream cosThetaHistogramName;
		cosThetaHistogramName << "cosTheta " << strParticleName.c_str();
		TH1F *pCosThetaHist = new TH1F(cosThetaHistogramName.str().c_str(), "cos(Theta)", 100, -4.0, 4.0);
		pCosThetaHist->GetXaxis()->SetTitle("cos(Theta)");
		
		ostringstream cosThetaMomHistogramName;
		cosThetaMomHistogramName << "cosThetaMom " << strParticleName.c_str();
		TH2F *pCosThetaMomHist = new TH2F(cosThetaMomHistogramName.str().c_str(),"|P|(cos(Theta))",100, -4, 4,100, -4, 4);
		pCosThetaMomHist->GetXaxis()->SetTitle("cos(Theta)");
		pCosThetaMomHist->GetYaxis()->SetTitle("|P|");
		pCosThetaMomHist->SetOption("COL");
		
		ostringstream ThetaMomHistogramName;
		ThetaMomHistogramName << "ThetaMom " << strParticleName.c_str();
		TH2F *pThetaMomHist = new TH2F(ThetaMomHistogramName.str().c_str(),"|P|(Theta)",100, -4, 4,100, -4, 4);
		pThetaMomHist->GetXaxis()->SetTitle("Theta");
		pThetaMomHist->GetYaxis()->SetTitle("|P|");
		pThetaMomHist->SetOption("COL");
		
		ostringstream PhiHistogramName;
		PhiHistogramName << "Phi " << strParticleName.c_str();
		TH1F *pPhiHist = new TH1F(PhiHistogramName.str().c_str(),"Phi", 100, -4.0, 4.0);
		pPhiHist->GetXaxis()->SetTitle("Phi");
		
		//________________________________________
		//----------- Read out the MCTrack Branch of the pTree and fill the data in the histograms
		TClonesArray *pClArray=NULL;
		pTree->SetBranchAddress("MCTrack", &pClArray);
		//pClArray->Inspect();
		CbmMCTrack *pTrack=NULL;
		Long64_t nTreeEntries=pTree->GetEntries();
		printf("nTreeEntries: %lld\n", nTreeEntries);
		
		for(int i=0; i<nTreeEntries; i++)
		{
			pTree->GetEntry(i);			
			//Int_t size=pClArray->GetSize();
			//printf("size: %d - ", size);
			Int_t nClArrayEntries=pClArray->GetEntries();
			//printf("nClArrayEntries: %d\n", nClArrayEntries);
			for(Int_t j=0; j < nClArrayEntries; j++)
			{
				pTrack=(CbmMCTrack*)pClArray->At(j);
				assert(pTrack);
				if( PDGMap.find( pTrack->GetPdgCode() ) != PDGMap.end() )	{
					if( PDGMap[pTrack->GetPdgCode()]==strParticleName.c_str() )	{
						Double_t mag=((TVector3)pTrack->GetMomentum()).Mag();
						pMomHist->Fill(mag);
						
						Double_t theta=((TVector3)pTrack->GetMomentum()).Theta();
						pThetaHist->Fill(theta);
						
						Double_t cosTheta=((TVector3)pTrack->GetMomentum()).CosTheta();
						pCosThetaHist->Fill(cosTheta);
						
						pCosThetaMomHist->Fill(cosTheta, mag);
						pThetaMomHist->Fill(theta, mag);
						
						Double_t phi=((TVector3)pTrack->GetMomentum()).Phi();
						pPhiHist->Fill(phi);
					}
				}
				if(!nPdgCode)	{	//TODO: Summary
						Double_t mag=((TVector3)pTrack->GetMomentum()).Mag();
						pMomHist->Fill(mag);
						
						Double_t theta=((TVector3)pTrack->GetMomentum()).Theta();
						pThetaHist->Fill(theta);
						
						Double_t cosTheta=((TVector3)pTrack->GetMomentum()).CosTheta();
						pCosThetaHist->Fill(cosTheta);	
						
						pCosThetaMomHist->Fill(cosTheta, mag);
						pThetaMomHist->Fill(theta, mag);
						
						Double_t phi=((TVector3)pTrack->GetMomentum()).Phi();
						pPhiHist->Fill(phi);
				}
			}
		}
		
		//________________________________________
		//-------------- Draw the histograms and set Ranges
		pCanvas->cd(2);
		pMomHist->Draw();
		pCanvas->cd(3);
		pThetaHist->Draw();
		pCanvas->cd(4);
		pCosThetaHist->Draw();
		pCanvas->cd(5);
		pPhiHist->Draw();
		pCanvas->cd(6);
		pCosThetaMomHist->GetXaxis()->SetRangeUser(-1.0, 1.0);
		pCosThetaMomHist->GetYaxis()->SetRangeUser(-0.0, 2.0);
		pCosThetaMomHist->Draw();
		pCanvas->cd(7);
		pThetaMomHist->GetXaxis()->SetRangeUser(0.0, 4.0);
		pThetaMomHist->GetYaxis()->SetRangeUser(0.0, 2.0);
		pThetaMomHist->Draw();	
		
		//Draw_Alt(pTree, pCanvas, 8, nPdgCode, m);
	}
}

void Draw_ParticleDistribution(TTree *pTree, TCanvas *pCanvas, int nCanvas, int m)
{
	pCanvas->cd(nCanvas);
	(pCanvas->cd(nCanvas))->SetLogy();
	
	//________________________________________
	//----------- Get Histogram from Tree
	pTree->Draw("MCTrack.fPdgCode>>h(8000,-4000,+4000)");	//TODO: Histogramm nicht zeichnen, um es zu bekommen	
	TH1F *pHistogram=(TH1F*)pTree->GetHistogram();
	Double_t offset = pHistogram->GetBinLowEdge(1);
	
	//________________________________________
	//---------- Store content of Histogram
	Int_t nbin = pHistogram->GetNbinsX();	//number of bins
	map<Int_t, Double_t> PDGCode_Content;
	for(Int_t i=1; i<nbin; i++)
	{
		Double_t Content = pHistogram->GetBinContent(i);
		if(Content)	{
			Int_t PDGCode=i+(int)offset-1;
			//printf("bin %d: %f\t-\tPDGCode=%d\n", i, Content, PDGCode);
			PDGCode_Content[PDGCode]=Content;
		}
	}
	
	//________________________________________
	//-------- Create new Histogram
	Int_t num_of_bins=max((Int_t)PDGMap.size(),1);	
	if(PDGMap.find(0)!=PDGMap.end())	{//TODO: Summary	//Summary is not a real Particle
		num_of_bins--;	
	}
	ostringstream H1Name;
	H1Name << "h1_" << m;
	TH1F *pNewHist = new TH1F(H1Name.str().c_str(), "Particle Distribution", num_of_bins, 0.0, (Double_t)num_of_bins);
	
	//________________________________________
	//----------- Fill new Histogram with Content of the old one
	map<Int_t, Double_t>::const_iterator Mapcit;	
	Mapcit=PDGCode_Content.begin();
	for(Int_t i=1; Mapcit!=PDGCode_Content.end(); Mapcit++)
	{
		Int_t PDGCode=Mapcit->first;		
		if(PDGMap.find(PDGCode) != PDGMap.end() && PDGCode) {	//TODO: Summary
			Double_t Content=Mapcit->second;
			pNewHist->SetBinContent(i, Content);
			TString ParticleName=PDGMap[Mapcit->first];
			pNewHist->GetXaxis()->SetBinLabel(i,ParticleName);
			i++;
		}
		else	{
			printf("Found unmapped Particle: %d\n",Mapcit->first);
		}
	}
	
	//________________________________________
	//---------Draw Particle Distribution Histogramm
	//pNewHist->SetOption("B");
	pNewHist->Draw();
}


//An alternative way of drawing the histograms
void Draw_Alt(TTree *pTree, TCanvas *pCanvas, int nStartCanvas, int nPdgCode, int m)
{
	//------ Set Particle Selection
	ostringstream ParticleSelection;
	if(nPdgCode)	{
		ParticleSelection << "MCTrack.fPdgCode==" << nPdgCode;
	}
	
	//--------- Impulsbetrag
	ostringstream P;
	P << "sqrt(MCTrack.fPx*MCTrack.fPx + MCTrack.fPy*MCTrack.fPy + MCTrack.fPz*MCTrack.fPz)";
	
	//----------
	ostringstream R;
	R << "sqrt(MCTrack.fPy*MCTrack.fPy + MCTrack.fPx*MCTrack.fPx + MCTrack.fPz*MCTrack.fPz)";
	//R << "sqrt(MCTrack.fPy*MCTrack.fPy + MCTrack.fPx*MCTrack.fPx)";

	
	ostringstream py;
	py << "MCTrack.fPy";
	ostringstream pz;
	pz << "MCTrack.fPz";
	ostringstream px;
	px << "MCTrack.fPx";
	//---------- Theta
	ostringstream Theta;
	Theta << "(" << "acos(" << pz.str().c_str() << "/" << R.str().c_str() << ")" << ")"; 
	//cos(Theta)
	ostringstream cosTheta;
	cosTheta << "cos" << "(" << Theta.str().c_str() << ")";
	//----------- Phi
	ostringstream Phi;
	Phi << "atan(" << py.str().c_str() << "/" << px.str().c_str() << ")";
	
	//---------------2. Subpad
	pCanvas->cd(nStartCanvas++);
	ostringstream PSelection;
	ostringstream H2Name;
	H2Name << "h2_" << m;
	PSelection << P.str().c_str() << ">>" << H2Name.str().c_str();
	//printf("cd(2): %s\n", PSelection.str().c_str() );
	pTree->Draw(PSelection.str().c_str(),ParticleSelection.str().c_str());
	((TH1F*)pTree->GetHistogram())->GetXaxis()->SetTitle("|P|");
	((TH1F*)pTree->GetHistogram())->SetTitle("Impulsbetrag");
	/*
	Double_t dMin = ((TH1F*)pTree->GetHistogram())->GetBinLowEdge(1);
	Double_t dMax = ((TH1F*)pTree->GetHistogram())->GetBinLowEdge(((TH1F*)pTree->GetHistogram())->GetNbinsX());
	((TH1F*)pTree->GetHistogram())->GetXaxis()->Set(25, dMin, dMax);
	*/
	((TH1F*)pTree->GetHistogram())->GetXaxis()->Set(100, -4.0, +4.0);
	pTree->Draw(PSelection.str().c_str(),ParticleSelection.str().c_str());
	
	//--------------- 3. Subpad
	pCanvas->cd(nStartCanvas++);
	ostringstream H3Name;
	H3Name << "h3_" << m;
	ostringstream ThetaSelection;
	ThetaSelection << cosTheta.str().c_str() << ">>" << H3Name.str().c_str() << "(100,-4,4)";
	//printf("cd(3): %s\n", ThetaSelection.str().c_str() );
	pTree->Draw(ThetaSelection.str().c_str(), ParticleSelection.str().c_str());
	((TH1F*)pTree->GetHistogram())->GetXaxis()->SetTitle("Theta");
	((TH1F*)pTree->GetHistogram())->SetTitle("Theta");

	//---------------- 4. Subpad
	pCanvas->cd(nStartCanvas++);
	ostringstream H4Name;
	H4Name << "h4_" << m;	
	ostringstream P_over_Theta;
	P_over_Theta << P.str().c_str() << ":" << cosTheta.str().c_str();
	ostringstream Selection;
	Selection << P_over_Theta.str().c_str() << ">>" << H4Name.str().c_str() << "(100, -4, 4, 100, -4, 4)";
	//printf("cd(4): %s\n", Selection.str().c_str());
	pTree->Draw(Selection.str().c_str(), ParticleSelection.str().c_str());
	((TH2F*)pTree->GetHistogram())->GetYaxis()->SetTitle("|P|");
	((TH2F*)pTree->GetHistogram())->GetXaxis()->SetTitle("cos(Theta)");
	((TH2F*)pTree->GetHistogram())->SetTitle("|P|(cos(Theta))");
	((TH2F*)gDirectory->Get(H4Name.str().c_str()))->Draw();
	
	
	//------------------- 5. Subpad
	pCanvas->cd(nStartCanvas++);
	ostringstream H5Name;
	H5Name << "h5_" << m;
	ostringstream PhiSelection;
	PhiSelection << Phi.str().c_str() << ">>" << H5Name.str().c_str() << "(100,-4,4)";
	//printf("cd(5): %s\n", PhiSelection.str().c_str());
	pTree->Draw(PhiSelection.str().c_str(), ParticleSelection.str().c_str());
	((TH1F*)pTree->GetHistogram())->GetXaxis()->SetTitle("Phi");
	((TH1F*)pTree->GetHistogram())->SetTitle("Phi");
}

