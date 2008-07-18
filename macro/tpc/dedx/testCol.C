#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include <ostream>
#include <sstream>

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
#include "TClonesArray.h"
#include "TCanvas.h"

#include "PndTpcRawDEdxCollection.h"
#include "DEDX.h"

using namespace std;

void testTruncateAndMean()
{
	PndTpcRawDEdxCollection Col;
	//set 10 arbitrary values
	Col.SetRawDEdx(10.,2.);	//5
	Col.SetRawDEdx(14.,2.);	//7	//why does it skip this and not the 28?
	Col.SetRawDEdx(20.,1.);	//20 //
	Col.SetRawDEdx(12.,2.);	//6
	
	Col.SetRawDEdx(31.,1.);	//31	//
	Col.SetRawDEdx(9.,3.);	//3
	Col.SetRawDEdx(12.,2.);	//6
	Col.SetRawDEdx(28.,4.);	//7	
	
	Col.SetRawDEdx(12.,1.);	//12	//
	Col.SetRawDEdx(12.,2.);  //6
	Col.Close();
	double meanDEDX=Col.TruncateAndMean(0.,1.,2);
	cout << "sumDEDX: " << meanDEDX << endl;
	assert(meanDEDX==160./20.);
	double truncatedMean=Col.TruncateAndMean(0.,0.6,2);
	cout << "truncatedMean: " << truncatedMean << endl;
	assert(truncatedMean==83./15.);
}

void testTruncateAndMeanAlternative()
{
	PndTpcRawDEdxCollection Col;
	//set 10 arbitrary values
	Col.SetRawDEdx(10.,2.);	//5
	Col.SetRawDEdx(14.,2.);	//7	//why does it skip this and not the 28?
	Col.SetRawDEdx(20.,1.);	//20 //
	Col.SetRawDEdx(12.,2.);	//6
	
	Col.SetRawDEdx(31.,1.);	//31	//
	Col.SetRawDEdx(9.,3.);	//3
	Col.SetRawDEdx(12.,2.);	//6
	Col.SetRawDEdx(28.,4.);	//7	
	
	Col.SetRawDEdx(12.,1.);	//12	//
	Col.SetRawDEdx(12.,2.);  //6
	Col.Close();
	double meanDEDX=Col.TruncateAndMeanAlternative(0.,1.,2);
	cout << "sumDEDX: " << meanDEDX << endl;
	assert(meanDEDX==103./10.);
	double truncatedMean=Col.TruncateAndMeanAlternative(0.,0.6,2);
	cout << "truncatedMean: " << truncatedMean << endl;
	assert(truncatedMean==33./6.);	
}

void testSum()
{
	PndTpcRawDEdxCollection Col;
	//set 10 arbitrary values
	Col.SetRawDEdx(10.,2.);
	Col.SetRawDEdx(15.,2.);
	Col.SetRawDEdx(20.,1.);
	Col.SetRawDEdx(11.,2.);
	
	Col.SetRawDEdx(31.,1.);
	Col.SetRawDEdx(9.5,2.);
	Col.SetRawDEdx(11.2,0.5);
	Col.SetRawDEdx(31.,0.5);
	
	Col.SetRawDEdx(12.,1.);
	Col.SetRawDEdx(12.,2.);
	Col.Close();
		
	double sumDE=Col.GetSumDE();
	cout << "SumDE: " << sumDE << endl;
	assert(sumDE==162.7);
	double sumDx=Col.GetSumDx();
	cout << "SumDx: " << sumDx << endl;
	assert(sumDx==14);
	
		
}

void testTruncation()
{
	PndTpcRawDEdxCollection Col;
	
	//set 10 arbitrary values
	Col.SetRawDEdx(10.,3.);
	Col.SetRawDEdx(15.,3.);
	Col.SetRawDEdx(20.,3.);
	Col.SetRawDEdx(11.,3.);
	
	Col.SetRawDEdx(31.,3.);
	Col.SetRawDEdx(9.5,3.);
	Col.SetRawDEdx(11.2,3);
	Col.SetRawDEdx(31.,3.);
	
	Col.SetRawDEdx(12.,3.);
	Col.SetRawDEdx(12.,2.8);
	Col.Close();
	
	std::vector<DEDx> NotTruncated;
	Col.Truncate(NotTruncated,0,1);
	vector<DEDx>::const_iterator cit=max_element(NotTruncated.begin(), NotTruncated.end());
	if(cit!=NotTruncated.end())	{
		cout << "max energy:" << cit->fDE << endl;
		assert(cit->fDE==31.);
	}
	
	cout << "NotTruncated.size(): " << NotTruncated.size() << endl;
	assert(NotTruncated.size()==10);
	
	std::vector<DEDx> Truncated;
	Col.Truncate(Truncated,0,0.6);
	cit=max_element(Truncated.begin(), Truncated.end());	
	if(cit!=Truncated.end())	{
		cout << "max truncated energy:" << cit->fDE << endl;
		assert(cit->fDE==12.);
	}	
	
	cout << "Truncated.size(): " << Truncated.size() << endl;
	assert(Truncated.size()==6);	
}


void FillTruncatedDE(TH1F *pHisto, PndTpcRawDEdxCollection *Col, double min, double max)
{
	std::vector<DEDx> Truncated;
	if(Col)	{
		Col->Truncate(Truncated,min,max);
	}
	std::vector<DEDx>::const_iterator cit;
	for(cit=Truncated.begin();cit!=Truncated.end();cit++)
	{
		pHisto->Fill(cit->fDE*1000*1000);
	}
}

void FillTruncatedDEDX(TH1F *pHisto, PndTpcRawDEdxCollection *Col, double min, double max, double factor=1.)
{
	std::vector<DEDx> Truncated;
	if(Col)	{
		Col->Truncate(Truncated,min,max);
	}
	std::vector<DEDx>::const_iterator cit;
	for(cit=Truncated.begin();cit!=Truncated.end();cit++)
	{
		if(cit->fDx)	{
			pHisto->Fill(cit->fDE*factor/cit->fDx);
		}
		else	{
			//cout << "zero dx skipped!" << endl;
		}
	}
}

void testWithTree(std::string filename)
{
	TTree *pTree=NULL;
	TFile *pFile=new TFile(filename.c_str(), "READ");
	if(pFile)	{
		pTree = (TTree*)pFile->Get("cbmsim");
	}
	if(!pTree)	{
		cout << "no tree, good bye!" << endl;
		return;
	}
	TClonesArray *pRawDEdxArray=NULL;
	pTree->SetBranchAddress("PndTpcRawDEdx", &pRawDEdxArray);
	
	TH1F *pDEHisto=new TH1F("raw", "raw", 200, 0, 40);	
	TH1F *pDEHisto2=new TH1F("raw2", "raw2", 200, 0, 40);	
	TH1F *pDEDXHisto=new TH1F("raw3", "raw3", 200, 0, 40);	
	TH1F *pDEDXHisto2=new TH1F("raw4", "raw4", 200, 0, 40);	
		
	Long64_t nTreeEntries=pTree->GetEntriesFast();
	printf("nTreeEntries: %lld\nPlease wait...\n", nTreeEntries);
	for(int i=0; i<nTreeEntries; i++)
	{
		pTree->GetEntry(i);
		if(!pRawDEdxArray)	{ cout << "No DEdxArray!" << endl; break;}
		PndTpcRawDEdxCollection *Col=(PndTpcRawDEdxCollection*)pRawDEdxArray->At(0);
		FillTruncatedDE(pDEHisto, Col, 0, 1);
		FillTruncatedDE(pDEHisto2, Col, 0, 0.6);
		FillTruncatedDEDX(pDEDXHisto,Col, 0, 1);
		FillTruncatedDEDX(pDEDXHisto2,Col, 0, 0.6);
	}
	TCanvas *pCanvasDE=new TCanvas("DE","DE");
	pCanvasDE->cd();
	pDEHisto->SetLineColor(kRed);
	pDEHisto->Draw();
	pDEHisto2->SetLineColor(kBlue);
	pDEHisto2->Draw("SAME");	
	
	TCanvas *pCanvasDEDX=new TCanvas("DEDX","DEDX");
	pCanvasDEDX->cd();
	pDEDXHisto->SetLineColor(kRed);
	pDEDXHisto->Draw();
	pDEDXHisto2->SetLineColor(kBlue);
	pDEDXHisto2->Draw("SAME");		
	
}

void Truncation(TTree *pTree, bool bReco, double min, double max, double &resolution, double &sigma, double &chi2)
{
	TClonesArray *pRawDEdxArray=NULL;
	TH1F *pDEDXHisto=NULL;
	TH1F *pDEDXHisto2=NULL;
	TH1F *pDEDXMeans=NULL;
	double factor=1.;
	double r_mean_min;
	double r_mean_max;
	ostringstream Add_On_stream;
	Add_On_stream << "_" << min << "_" << max;
	string Add_On=Add_On_stream.str();
	if(bReco)	{
		pTree->SetBranchAddress("PndTpcRawDEdxReco", &pRawDEdxArray);
		r_mean_min=0;
		r_mean_max=1700000;
		string name="trdedx"+Add_On;
		pDEDXHisto=new TH1F(name.c_str(), name.c_str(), 2000, 0, 1900000);
		name="dedx"+Add_On;
		pDEDXHisto2=new TH1F(name.c_str(), name.c_str(), 2000, 0, 1900000);		
		name="dedxmeans"+Add_On;
		pDEDXMeans=new TH1F(name.c_str(),name.c_str(), 2000,r_mean_min,r_mean_max);
	}
	else	{
		r_mean_min=0;
		r_mean_max=10;	
		pTree->SetBranchAddress("PndTpcRawDEdx", &pRawDEdxArray);
		string name="trdedx"+Add_On;		
		pDEDXHisto=new TH1F(name.c_str(), name.c_str(), 200, 0, 40);
		name="dedx"+Add_On;
		pDEDXHisto2=new TH1F(name.c_str(),name.c_str(), 200, 0, 40);	
		name="dedxmeans"+Add_On;	
		pDEDXMeans=new TH1F(name.c_str(),name.c_str(), 200, r_mean_min,r_mean_max);
		factor=1000.*1000.;
	}
	Long64_t nTreeEntries=pTree->GetEntriesFast();
	//ab hier in eine Funktion, die den tree übergeben bekommt + die entries + min + max

	
	printf("nTreeEntries: %lld\nPlease wait...\n", nTreeEntries);
	for(int i=0; i<nTreeEntries; i++)
	{
		pTree->GetEntry(i);
		if(!pRawDEdxArray)	{ cout << "No DEdxArray!" << endl; break;}
		PndTpcRawDEdxCollection *Col=(PndTpcRawDEdxCollection*)pRawDEdxArray->At(0);
		FillTruncatedDEDX(pDEDXHisto,Col, min, max, factor);
		FillTruncatedDEDX(pDEDXHisto2,Col, 0., 1., factor);
		pDEDXMeans->Fill(Col->TruncateAndMean(min,max)*factor);
		
	}
	string name="DEDX"+Add_On;
	TCanvas *pCanvasDEDX=new TCanvas(name.c_str(),name.c_str());
	pCanvasDEDX->cd();
	pDEDXHisto->SetLineColor(kRed);
	pDEDXHisto->Draw();
	pDEDXHisto2->SetLineColor(kBlue);
	pDEDXHisto2->Draw("SAME");
	
	name="DEDX_Means"+Add_On;
	TCanvas *pCanvasDEDXMeans=new TCanvas(name.c_str(),name.c_str());
	pCanvasDEDXMeans->Divide(1,1);
	pCanvasDEDXMeans->cd(0);	
	pDEDXMeans->Draw();
	
	string fnc_name="fgaus"+Add_On;
	TF1 *fgaus = new TF1(fnc_name.c_str(),"gaus",r_mean_min , r_mean_max);
	pDEDXMeans->Fit(fnc_name.c_str(),"RNOLL");
	fgaus->SetLineWidth(1);
	fgaus->SetLineColor(kBlue);
	fgaus->Draw("SAME");
	double mean=fgaus->GetParameter("Mean");
	sigma=fgaus->GetParameter("Sigma");
	chi2=fgaus->GetChisquare();
	resolution=sigma/mean;
	cout << "Resolution: " << sigma/mean << " - chi2: " << chi2 << endl;
	
}

void testTruncation(std::string filename, bool bReco=true)
{
	TTree *pTree=NULL;
	TFile *pFile=new TFile(filename.c_str(), "READ");
	if(pFile)	{
		pTree = (TTree*)pFile->Get("cbmsim");
	}
	if(!pTree)	{
		cout << "no tree, good bye!" << endl;
		return;
	}
	ostringstream output;
	double chi2, resolution, sigma;
	float min_first=0.;
	float min_last=0.5;
	float max_first=0.5;
	float max_last=0.9;
	for(float i=max_first; i<=max_last; i+=0.1)
	{
		for(float j=min_first; j<=min_last; j+=0.1)
		{
			Truncation(pTree, bReco, j,i,resolution,sigma, chi2);
			output << j << "-" << i << "R: " << resolution << " sigma: " << sigma
			<< " --- chi2: " << chi2 << endl;
		}
	}
	

	
	cout << output.str() << endl;
	
}
