#include <sstream>
#include <fstream>
#include <map>
#include <iostream>

#include "TCanvas.h"
#include "TF1.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TH1F.h"
#include "TDirectory.h"
#include "TProfile.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TGraphErrors.h"

#include "PDGMap.h"
#include "DEDXDistribana.h"
#include "DEDXHelper.h"
#include "PndTpcDEDXFits.h"

using namespace std;

 DEDXDistribana::DEDXDistribana():m_MinEntries(15),m_SumBins(100),m_SumMin(0),m_SumMax(30)
 {
	p_PDGMap=new PDGMap; 
	SetMomenta();
	p_Fits=new PndTpcDEDXFits;
 }
 
  DEDXDistribana::~DEDXDistribana()
  {
  	delete p_PDGMap;
  }

void DEDXDistribana::PrepareHistogram(TH1F *pHisto, string strName, Double_t P_set)
{
	ostringstream HistoName;			
	HistoName << strName.c_str() << "_de_dx_" << P_set;
	pHisto->SetNameTitle(HistoName.str().c_str(), HistoName.str().c_str());
	pHisto->GetXaxis()->SetTitle("dE/dx");
	//cout << "PrepareHistogram " << HistoName.str().c_str() << endl; 
}

TH1F* DEDXDistribana::GetHistogram(std::string strName, Double_t P_set)
{
	//cout << "GetHistogram: " << strName << "_" << P_set << endl;
	ostringstream HistoName;				
	HistoName << strName.c_str() << "_de_dx_" << P_set;
	TH1F *pCurHisto=(TH1F*)gDirectory->Get(HistoName.str().c_str());
	return pCurHisto;
}

void DEDXDistribana::DeDxFromExistingHistogram(TH1F *pHisto, string strName, Double_t P)
{
	assert(pHisto);
	{
		PrepareHistogram(pHisto, strName, P);
		m_SumBins=pHisto->GetNbinsX();
		m_SumMin=pHisto->GetBinLowEdge(1);
		m_SumMax=pHisto->GetBinLowEdge(pHisto->GetNbinsX()) + pHisto->GetXaxis()->GetBinWidth(pHisto->GetNbinsX());
		//cout << "Got existing Histogram " << pHisto->GetName() <<" (" << m_SumBins << "," << m_SumMin << "," << m_SumMax << ")" << endl;
		CreateDeDxDistributionSummary(strName, P);
	}
}

void DEDXDistribana::CreateDeDxDistributionSummary(string strName, Double_t P)
{
	if(strName!=p_PDGMap->GetSummaryName() )	{ return; }
	TH1F *pHist = new TH1F("tmpdedxsummary","tmpdedxsummary", m_SumBins, m_SumMin,m_SumMax );
	//cout << "Created Summary Histogram (" << m_SumBins << "," << m_SumMin << "," << m_SumMax << ")" << endl;
	assert(pHist);
	PrepareHistogram(pHist, strName, P);
}

void DEDXDistribana::FillSummary(TH1F *pFill, Double_t P_set)
{
	TH1F *pHisto=GetHistogram(p_PDGMap->GetSummaryName(), P_set);
	if(pHisto)	{
		pHisto->Add(pFill);
	}
}

void DEDXDistribana::FillDEdxDistributions( )
{
	map<string, Double_t> Statistic;
	FillDEdxDistributions(Statistic);
}

void DEDXDistribana::FillDEdxDistributions( map<string, Double_t> &Statistic)
{
	int binTolerance=2;
	map<Int_t,string>::const_iterator cit;
	for(cit=p_PDGMap->Particles.begin(); cit!=p_PDGMap->Particles.end(); cit++)	
	{
		string ParticleName=cit->second;
		TH2F *h = (TH2F *) gROOT->FindObject((ParticleName).c_str());
		if(h)	{
			Int_t nbin = h->GetNbinsX();	//number of bins
			//cout << "nbin: " << nbin << endl;
			
			Statistic[ParticleName]=h->GetEntries();	
			for(Int_t i=0; i<=nbin;i++)	
			{	
				//das Histogram zu einem Teilchen wird für viele Impulswerte projeziert
				Int_t first_bin=i-binTolerance;
				Int_t last_bin=i+binTolerance;
				
				Double_t P=BinToMomentum(h,i);
				//cout << "Checking Momentum: " << P << endl;
				if(!IsInVector(P,vP_set))	{
					//cout << "Invalid Momentum!" << endl;
					continue;
				}
				
				//cout << "ProjectionY from bin number " << first_bin << " to " << last_bin << endl; 
				TH1F * pHisto=(TH1F*)h->ProjectionY("temp", first_bin,last_bin );  //z.b. bin 1 , 2, 3        //pHisto will get another name in DeDxFromExistingHistogram

				DeDxFromExistingHistogram(pHisto, ParticleName, P);			
				FillSummary(pHisto, P);

			}
		}
	}
}

void DEDXDistribana::CreateDeDxDistributionGausFits()
{
	vector<Double_t>::const_iterator cit;
	for(cit=vP_set.begin(); cit!=vP_set.end(); cit++)
	{
		map<string, TF1*> FitMap_setP;
		CreateDeDxDistributionGausFits(*cit, FitMap_setP);
		FitMap[*cit]=FitMap_setP;
	}
}

void DEDXDistribana::CreateDeDxDistributionGausFits(Double_t P_set, map<string, TF1*> &ParticleFits)
{
	map<Int_t,string>::const_iterator cit;
	for(cit=p_PDGMap->Particles.begin(); cit!=p_PDGMap->Particles.end(); cit++)
	{
		if(cit->first)	{
			TH1F *pCurHisto=GetHistogram(cit->second, P_set);
			if(!pCurHisto)	{
				//cout << "CreateGausFits - Skipping: " << cit->second << "_" << P_set << endl;
				continue;
			}
			if( pCurHisto->GetEntries() >= m_MinEntries )	{
				//cout << "CreateGausFits - Creating Gaus Fits for " << cit->second << "_" << P_set << endl;
				//cout << "CreateFits - Using upper fit limit: " << m_SumMax << endl;
				TF1 *fgaus = new TF1("fgaus","gaus",0 , m_SumMax+100);
				assert(fgaus);
				pCurHisto->Fit("fgaus","RNOLL");
				fgaus->SetLineColor(p_PDGMap->GetParticleColor(cit->second));
				ostringstream FunctionName;
				FunctionName << cit->second << "_" << P_set; 
				ParticleFits[FunctionName.str()]=fgaus;
			}
			else	{
				//cout << "CreateGausFits - Skipping: " << cit->second << "_" << P_set << endl;
				//cout << "Reason: Less than " << m_MinEntries << " Entries" << endl;
			}
		}
	}
}

void DEDXDistribana::DrawDeDxDistribution()
{
	vector<Double_t>::const_iterator cit;
	for(cit=vP_set.begin(); cit!=vP_set.end(); cit++)
	{ 
		DrawDeDxDistribution(*cit, FitMap[*cit]);
	}
}

void DEDXDistribana::DrawDeDxDistribution(Double_t P_set, map<string, TF1*> &ParticleFits)
{
	//cout << "DrawDeDxDistribution -Start" << endl;
	//________________________________________
	//Draw DeDx-Distributions for the momentum P_set in new Canvas
	ostringstream CanvasName;
	CanvasName << "dedxDistribution_Canvas" << P_set;
	ostringstream CanvasTitle;
	CanvasTitle << "dedx for set P value " << P_set << " GeV/c";
	TCanvas *pCanvas = new TCanvas(CanvasName.str().c_str(), CanvasTitle.str().c_str() , 30,30,500,350);
	assert(pCanvas);
	
	if( (p_PDGMap->Particles.size())%2 == 0  )	{	
		pCanvas->Divide(2,(p_PDGMap->Particles.size())/2);
	}
	else	{
		pCanvas->Divide(2,(p_PDGMap->Particles.size()+1)/2);
	}	
	
	Int_t nCanvas=1;
	map<Int_t,string>::const_iterator cit;
	for(cit=p_PDGMap->Particles.begin(); cit!=p_PDGMap->Particles.end(); cit++)	//draw in a subpad for every particle + summary
	{
		if(cit->first)	{
			pCanvas->cd(nCanvas++);
			TH1F *pCurHisto=GetHistogram(cit->second, P_set);
			if(pCurHisto) {
				pCurHisto->Draw();
				ostringstream FunctionName;
				FunctionName << (cit->second).c_str() << "_" << P_set;
				if(ParticleFits.find(FunctionName.str())!=ParticleFits.end())	{
					ParticleFits[FunctionName.str()]->Draw("SAME");
				}
			}
		}
		else	{	//the summary
			pCanvas->cd(nCanvas++);
			TH1F *pCurHisto=GetHistogram(cit->second, P_set);
			if(pCurHisto)	{
				pCurHisto->Draw();
				
				TLegend *legend=new TLegend(0.39,0.6,1.1,0.8);
				legend->SetTextFont(72);
				legend->SetTextSize(0.04);
	
				map<string, TF1*>::const_iterator cit_FM;
				for(cit_FM=ParticleFits.begin(); cit_FM!=ParticleFits.end(); cit_FM++)
				{				
					ostringstream FunctionName;
					FunctionName << (cit->second).c_str() << "_" << P_set;
					(cit_FM->second)->Draw("SAME");
					ostringstream legendText;
					legendText << (cit_FM->first).c_str() << " Mean: " << (cit_FM->second)->GetParameter("Mean") << ", Sigma: " << (cit_FM->second)->GetParameter("Sigma");
					legend->AddEntry( cit_FM->second, legendText.str().c_str() );
					
				}
				legend->Draw();
			}	
		}
	}
	//cout << "DrawDeDxDistribution -Ende" << endl;
}

void DEDXDistribana::PrintChi2()
{
	vector<Double_t>::const_iterator cit;
	for(cit=vP_set.begin(); cit!=vP_set.end(); cit++)
	{ 
		DrawDeDxDistribution(*cit, FitMap[*cit]);
		map<string, TF1*> ParticleFits=FitMap[*cit];
		map<Int_t,string>::const_iterator cjt;
		for(cjt=p_PDGMap->Particles.begin(); cjt!=p_PDGMap->Particles.end(); cjt++)
		{
			if(cjt->first)	{
				ostringstream FunctionName;
				FunctionName << (cjt->second).c_str() << "_" << *cit;
				if(ParticleFits.find(FunctionName.str())!=ParticleFits.end())	{
					Double_t chi2=ParticleFits[FunctionName.str()]->GetChisquare();
					cout << FunctionName.str().c_str() << "-> chi2: " << chi2 << endl;
				}
			}
		}
	}
}

void DEDXDistribana::PrintBBFitChi2()
{
	map<Int_t,string>::const_iterator cit;
	for(cit=p_PDGMap->Particles.begin(); cit!=p_PDGMap->Particles.end(); cit++)
	{
		TF1* fit=p_Fits->GetFunction(cit->second);
		if(fit)	{
			Double_t chi2=fit->GetChisquare();	
			cout << cit->second << "-> BB-Fit chi2: " << chi2 << endl;
		}
	}
}


void DEDXDistribana::GetFitMap(std::map<Double_t, std::map<std::string, TF1*> > &Map) const
{
	Map=FitMap;
}


Double_t DEDXDistribana::BinToMomentum(TH2F *h, Int_t bin) const
{
	Double_t momentum = h->GetBinLowEdge(bin);	
	return momentum;
}

bool DEDXDistribana::IsInVector(Double_t momentum, const vector<Double_t> &vP_set) const
{
	vector<Double_t>::const_iterator cit;
	for(cit=vP_set.begin();cit!=vP_set.end(); cit++)
	{
		if(fabs((*cit)-momentum)<0.0001)	{
			return true;
		}
	}
	return false;
} 

void DEDXDistribana::SetMomenta()
{
	//vP_set.push_back(0.1);
	vP_set.push_back(0.2);
	vP_set.push_back(0.25);	
	vP_set.push_back(0.3);
	vP_set.push_back(0.35);
	//vP_set.push_back(0.37);
	vP_set.push_back(0.4);
	vP_set.push_back(0.5);
	vP_set.push_back(0.6);
	vP_set.push_back(0.7);	
	vP_set.push_back(0.8);
	vP_set.push_back(0.9);	
	
	/*
	vector<Double_t>::const_iterator v;
	for(v=vP_set.begin(); v!=vP_set.end(); v++)
	{
		cout << "P: " << *v << " - ";
	}
	*/
}

void DEDXDistribana::SetFunctionNameAddOn(const std::string &AddOn)	
{
	 p_Fits->SetFunctionNameAddOn(AddOn);	
}

void DEDXDistribana::CreateGraphs()
{
	map<Double_t, map<string, TF1*> >::const_iterator cit;
	map<Double_t, map<string, TF1*> >::const_iterator end=FitMap.end();
	
	map<Int_t,string>::const_iterator citParticle;
	int iPoint=0;
	TGraphErrors *pGraph=NULL;
	for(citParticle=p_PDGMap->Particles.begin(); citParticle!=p_PDGMap->Particles.end(); ++citParticle)
	{
		for(cit=FitMap.begin(); cit!=end; ++cit)	// try to fill a graph for citParticle
		{	// go through all momenta and pick out the right particle
			double x=cit->first;
			assert(x);
			map<string, TF1*> ParticleFits=cit->second;
			
			ostringstream FunctionName;
			FunctionName << (citParticle->second).c_str() << "_" << x;
			map<string, TF1*>::const_iterator pFit=ParticleFits.find(FunctionName.str());
			if(pFit!=ParticleFits.end())	{
				if(iPoint==0)	{	//found something => create a new TGraph and let it be managed by vGraph
					pGraph=new TGraphErrors;//(FitMap.size());
					vGraph[citParticle->second]=pGraph;
					cout << "vGraph Managing particle " << citParticle->second << endl;
				}
				TF1* Fit=pFit->second;
				double y=Fit->GetParameter("Mean");
				double error=Fit->GetParameter("Sigma");
				cout << "DEDXDistribana - CreateGraphs: Fill (" << x << "," << "y" << ")" << endl;
				pGraph->SetPoint(iPoint,x,y);
				pGraph->SetPointError(iPoint,0.01,error); //vielleicht einen noch sinnvolleren x-Fehler?
				++iPoint;
			}
		}
		iPoint=0;
		pGraph=NULL;
	}
}

void DEDXDistribana::CreateBBFits()
{
	std::map<std::string,TGraphErrors *>::const_iterator cit;
	std::map<std::string,TGraphErrors *>::const_iterator end=vGraph.end();
	for(cit=vGraph.begin();cit!=end;++cit)
	{
		p_Fits->CreateFitForGraph(cit->second, cit->first);
	}
}

//this is the same as the function of DEDXFitCreator
//except: the TProfiles are TGraphErrors
//except: i do not use a FitMap but the PndTpcDEDXFits to manage the fits
void DEDXDistribana::traceDrawing() const
{
	//________________________________________
	//Create a new Canvas, there is a histogram and a profile Subpad for every Particle
	ostringstream CanvasName;
	CanvasName << "trace_Canvas";
	TCanvas *pCanvas=new TCanvas(CanvasName.str().c_str(), "dedx and fits");
	if(p_PDGMap->Particles.find(0)!=p_PDGMap->Particles.end() )	{ 
		pCanvas->Divide(2,p_PDGMap->Particles.size()-1);
	}
	else	{
		pCanvas->Divide(2,p_PDGMap->Particles.size());
	}
		
	int nSubpad_dedx=1;		//left Pads
	int nSubpad_profile=2;		//right Pads
	map<Int_t,string>::const_iterator cit;
	for(cit=p_PDGMap->Particles.begin(); cit!=p_PDGMap->Particles.end(); cit++)
	{
		if(cit->first)	{
			//________________________________________
			//Draw the Histogram in left pad
			pCanvas->cd(nSubpad_dedx); 
			ostringstream HistoName;
			HistoName << cit->second;
			printf("HistoName: %s\n", HistoName.str().c_str());
			TH2F *pCurHisto=(TH2F*)gDirectory->Get(HistoName.str().c_str());
			if(!pCurHisto)	{
				continue;
			}
			pCurHisto->Draw();	
			
			//________________________________________
			//Check the fit associated with the current Histogram Name
			//and draw the corresponding fit function
			TF1 *fit=p_Fits->GetFunction(cit->second);
			if(fit)	{
				fit->SetNpx(100);
				fit->SetLineWidth(1);
				fit->SetLineColor(p_PDGMap->GetParticleColor(cit->second));
				fit->Draw("SAME");
			}
			else	{
				printf( "Tried to draw non-existing Fit for %s\n", (cit->second).c_str() );
				//continue;	//i want to draw the histogram even if i have no fit
			}
			
			
			//________________________________________
			//Draw the Graph in the right pad
			pCanvas->cd(nSubpad_profile);
			std::map<std::string,TGraphErrors *>::const_iterator found=vGraph.find(cit->second);
			if( found!=vGraph.end() )	{
				TGraphErrors *graph=found->second;
				graph->Draw("SAMEALP");
			}
			
			/*
			TProfile *pPro=pCurHisto->ProfileX("_pfx", -1, -1, "b");
			assert(pPro);
			pPro->Draw();
			*/
			//________________________________________
			// Draw the corresponding fit function again
			if(fit)	{
				fit->Draw("SAME");
			}
			
			nSubpad_dedx+=2;
			nSubpad_profile+=2;
		}
	}
}

void DEDXDistribana::DrawDEDXSummary() const
{
	if(p_PDGMap->Particles.find(0)==p_PDGMap->Particles.end())	{
		return;
	}
	
	//________________________________________
	//Draw Summary in new Canvas
	ostringstream CanvasName;
	CanvasName << "dedx_Summary_Canvas";
	ostringstream CanvasTitle;
	CanvasTitle << "dedx Summary";
	TCanvas *pCanvas = new TCanvas(CanvasName.str().c_str(), CanvasTitle.str().c_str() , 30,30,500,350);
	pCanvas->cd(1);
	
	//________________________________________
	//Draw the Summary Histogram and all fits stored in vFits
	ostringstream HistoName;
	HistoName << "Summary";
	TH2F *pCurHisto=(TH2F*)gDirectory->Get(HistoName.str().c_str());	//the Summary histogram is filled with all Particles
	if(pCurHisto) {
		pCurHisto->Draw("COLZ");
		map<Int_t,string>::const_iterator cit;
		for(cit=p_PDGMap->Particles.begin(); cit!=p_PDGMap->Particles.end(); cit++)
		{
			TF1 *fit=p_Fits->GetFunction(cit->second);
			if(fit)	{
				fit->SetNpx(100);
				fit->SetLineWidth(1);
				fit->SetLineColor(p_PDGMap->GetParticleColor(cit->second));
				fit->Draw("SAME");
			}
		}
		/*
		vector<TF1*>::const_iterator citFitFcn;		//vFits is filled with all fit-functions
		for(citFitFcn=vFits.begin(); citFitFcn!=vFits.end(); citFitFcn++)
		{
			(*citFitFcn)->Draw("SAME");
		}
		*/
	}
}

#if 0
void DEDXDistribana::Fill(string strName, Double_t input, Double_t P, const vector<Double_t> vP_set, Double_t tolerance)
{
	vector<Double_t>::const_iterator cit;
	bool bFilledIn=0;
	for(cit=vP_set.begin(); cit!=vP_set.end(); cit++)
	{
		bFilledIn=Fill(strName, input, P,*cit , tolerance);
		if(bFilledIn)	{
			break;
		}
	}
	if(!bFilledIn)	{
	//	dedxlog << "Momentum " << P << " is not filled in!" << endl;
	}
}

bool DEDXDistribana::Fill(string strName, Double_t input, Double_t P,const Double_t P_set, Double_t tolerance)
{
	if( !(P >= P_set-tolerance && P <= P_set+tolerance) )	{ 
		return 0;	
	} 
	//Fill the distribution for one Particle (strName)
	{
		TH1F *pHisto=GetHistogram(strName, P_set);
		if(pHisto)	{
			pHisto->Fill( input );
			return 1;
		}
	}
	
	//Fill the summary, too
	{
		TH1F *pHisto=GetHistogram("Summary", P_set);
		if(pHisto)	{
			pHisto->Fill( input );
			return 1;
		}
	}
	
	return 0;
}
#endif

#if 0
void DEDXDistribana::CreateSummaries()
{
	vector<Double_t>::const_iterator end=vP_set.end();
	for(vector<Double_t>::const_iterator cit=vP_set.begin(); cit!=end; ++cit)
	{
		CreateDeDxDistributionSummary("Summary", *cit);
	}
}
#endif

 #if 0
void DEDXDistribana::CreateDeDxDistribution(const vector<Double_t> &vP_set)
{
	vector<Double_t>::const_iterator cit;	//TODO
	for(cit=vP_set.begin(); cit!=vP_set.end(); cit++)
	{
		CreateDeDxDistribution(*cit);
	}
}

void DEDXDistribana::CreateDeDxDistribution(Double_t P_set)
{
	//________________________________________
	//----------Create a Histogram for every Particle and a Summary histograms if wanted
	map<Int_t,string>::const_iterator cit;
	for(cit=p_PDGMap->Particles.begin(); cit!=p_PDGMap->Particles.end(); cit++)
	{
		cout << "Creating Histogram " << cit->second << "_" << P_set << endl;
		TH1F *pHist = new TH1F("tempdedxhisto", "tempdedxhisto", m_SumBins, m_SumMin,m_SumMax );
		assert(pHist);
		PrepareHistogram(pHist,cit->second,P_set);
	}
}
#endif
