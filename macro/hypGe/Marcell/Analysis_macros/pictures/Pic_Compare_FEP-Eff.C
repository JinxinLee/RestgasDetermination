//#include <iostream>
//#include <fstream>

//#include "TStyle.h"
//#include "TROOT.h"
//#include "TApplication.h"
//#include "TCanvas.h"
//#Include "TGraph.h"
//#include "TLegend.h"
#include <map>


Double_t* MapsToArrays(std::map<Double_t, Double_t> Map,const int ArraySize, bool isFirst = 0, Double_t *x = 0, Double_t *Dx = 0)
{
	std::map<Double_t,Double_t>::iterator it;
	Double_t *Array = new Double_t[ArraySize];
	int i = 0;
	for (it=Map.begin(); it!=Map.end(); ++it)
	{
		//std::cout << it->first << " => " << it->second << '\n';
		if (isFirst)
		{
			x[i]=it->first;//*1000;
			Dx[i]=0*1000;
		}
		Array[i]= it->second;
		i++;
	}
	return Array;
}

void Pic_Compare_FEP_Eff(bool Plot20 = true,	bool Plot10SekTar = false,	bool Plot10 = false)
{

	bool useExpectedRange = 0;


	std::map<Double_t, Double_t> Triple4010sekTar;	//<Energy, FEP-Eff>
	std::map<Double_t, Double_t> Triple4010sekTarError;	//<Energy, FEP-EffError>
	std::map<Double_t, Double_t> Triple4010;
	std::map<Double_t, Double_t> Triple4010Error;
	std::map<Double_t, Double_t> Triple4020sekTar;
	std::map<Double_t, Double_t> Triple4020sekTarError;
	std::map<Double_t, Double_t> Triple4020sekTarSEP;
	std::map<Double_t, Double_t> Triple4020sekTarSEPError;
	std::map<Double_t, Double_t> Triple4020sekTarSEPDEP;
	std::map<Double_t, Double_t> Triple4020sekTarSEPDEPError;
	std::map<Double_t, Double_t> Triple4020sekTar1;
	std::map<Double_t, Double_t> Triple4020sekTar1Error;
	std::map<Double_t, Double_t> Triple4020sekTar1SEP;
	std::map<Double_t, Double_t> Triple4020sekTar1SEPError;
	std::map<Double_t, Double_t> Triple4020sekTar1SEPDEP;
	std::map<Double_t, Double_t> Triple4020sekTar1SEPDEPError;
	std::map<Double_t, Double_t> Triple4020;
	std::map<Double_t, Double_t> Triple4020Error;
	std::map<Double_t, Double_t> Triple4020SEP;
	std::map<Double_t, Double_t> Triple4020SEPError;
	std::map<Double_t, Double_t> Triple4020SEPDEP;
	std::map<Double_t, Double_t> Triple4020SEPDEPError;
	std::map<Double_t, Double_t> Triple4020sekTarFWHM;
	std::map<Double_t, Double_t> Triple4020sekTarFWHMError;
	std::map<Double_t, Double_t> Triple4020sekTar1FWHM;
	std::map<Double_t, Double_t> Triple4020sekTar1FWHMError;
	std::map<Double_t, Double_t> Triple4020FWHM;
	std::map<Double_t, Double_t> Triple4020FWHMError;
	std::map<Double_t,Double_t>::iterator it;
	TStyle *style = gROOT -> GetStyle("Default");
	style->SetFillColor(10);
	style->SetFrameFillColor(10);
	style->SetCanvasColor(10);
	style->SetPadColor(10);
	style->SetTitleFillColor(10);
	style->cd();
	//gStyle->SetFrameFillColor(10);
	

	//ifstream Inputfile("/data/work/kpha1/steinen/Gamma/Ana/CollectFEPF.txt");
	ifstream Inputfile("/data/work/kpha1/steinen/Gamma/Ana/CombinedData/Fitted/FittedData.txt");

	Int_t Geometry, Sektar, nEvents;
	Double_t Energy, FullEnergyPeakEff, FullEnergyPeakEffError, FWHM, FWHMError,FullEnergyPeakEffSEP, FullEnergyPeakEffSEPError,FullEnergyPeakEffSEPDEP, FullEnergyPeakEffSEPDEPError;
	char InputName[100];
	while(Inputfile  >>  Geometry  >> Sektar >> Energy >> nEvents >>  FullEnergyPeakEff >>  FullEnergyPeakEffError >>FWHM >> FWHMError>> FullEnergyPeakEffSEP >> FullEnergyPeakEffSEPError>>FullEnergyPeakEffSEPDEP >> FullEnergyPeakEffSEPDEPError  )
	{
		//cout <<"\t"<<  Geometry  <<"\t"<< Sektar <<"\t"<< Energy <<"\t"<< nEvents <<"\t"<<  FullEnergyPeakEff <<"\t"<<  FullEnergyPeakEffError <<"\t"<<FWHM <<"\t"<< FWHMError<<"\t"<< FullEnergyPeakEffSEP <<"\t"<< FullEnergyPeakEffSEPError<<"\t"<<FullEnergyPeakEffSEPDEP <<"\t"<< FullEnergyPeakEffSEPDEPError <<"\t"<<endl;

		FWHM = FWHM*1000;
		FWHMError = FWHMError*1000;
		if (nEvents == 10000000)
		{
			if (Geometry == 35)
				{
					if(Sektar)
					{
						Triple4010sekTar.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEff) );
						Triple4010sekTarError.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEffError) );
					}
					else
					{
						Triple4010.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEff) );
						Triple4010Error.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEffError) );
					}
				}
			if (Geometry == 36)
			{
				if(Sektar==3)
				{
					Triple4020sekTar.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEff) );
					Triple4020sekTarError.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEffError) );
					Triple4020sekTarSEP.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEffSEP) );
					Triple4020sekTarSEPError.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEffSEPError) );
					Triple4020sekTarSEPDEP.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEffSEPDEP) );
					Triple4020sekTarSEPDEPError.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEffSEPDEPError) );
					Triple4020sekTarFWHM.insert( std::pair<Double_t,Double_t>(Energy,FWHM) );
					Triple4020sekTarFWHMError.insert( std::pair<Double_t,Double_t>(Energy,FWHMError) );

				}
				if(Sektar==1)
				{
					Triple4020sekTar1.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEff) );
					Triple4020sekTar1Error.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEffError) );
					Triple4020sekTar1SEP.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEffSEP) );
					Triple4020sekTar1SEPError.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEffSEPError) );
					Triple4020sekTar1SEPDEP.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEffSEPDEP) );
					Triple4020sekTar1SEPDEPError.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEffSEPDEPError) );
					Triple4020sekTar1FWHM.insert( std::pair<Double_t,Double_t>(Energy,FWHM) );
					Triple4020sekTar1FWHMError.insert( std::pair<Double_t,Double_t>(Energy,FWHMError) );

				}
				if (Sektar == 0)
				{
					Triple4020.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEff) );
					Triple4020Error.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEffError) );
					Triple4020SEP.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEffSEP) );
					Triple4020SEPError.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEffSEPError) );
					Triple4020SEPDEP.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEffSEPDEP) );
					Triple4020SEPDEPError.insert( std::pair<Double_t,Double_t>(Energy,FullEnergyPeakEffSEPDEPError) );
					Triple4020FWHM.insert( std::pair<Double_t,Double_t>(Energy,FWHM) );
					Triple4020FWHMError.insert( std::pair<Double_t,Double_t>(Energy,FWHMError) );
				}
			}
		}
	}
	Inputfile.close();
	
	const int n = Triple4020sekTar.size();
	double x[n];
	double Dx[n];
	Double_t *yTriple4020sekTar = MapsToArrays(Triple4020sekTar,n,1,x,Dx);
	Double_t * yTriple4020sekTarError= MapsToArrays(Triple4020sekTarError,n);
	Double_t * yTriple4020sekTarSEP= MapsToArrays(Triple4020sekTarSEP,n);
	Double_t * yTriple4020sekTarSEPError= MapsToArrays(Triple4020sekTarSEPError,n);
	Double_t * yTriple4020sekTarSEPDEP= MapsToArrays(Triple4020sekTarSEPDEP,n);
	Double_t * yTriple4020sekTarSEPDEPError= MapsToArrays(Triple4020sekTarSEPDEPError,n);
	Double_t *yTriple4020sekTar1 = MapsToArrays(Triple4020sekTar1,n);
		Double_t * yTriple4020sekTar1Error= MapsToArrays(Triple4020sekTar1Error,n);
		Double_t * yTriple4020sekTar1SEP= MapsToArrays(Triple4020sekTar1SEP,n);
		Double_t * yTriple4020sekTar1SEPError= MapsToArrays(Triple4020sekTar1SEPError,n);
		Double_t * yTriple4020sekTar1SEPDEP= MapsToArrays(Triple4020sekTar1SEPDEP,n);
		Double_t * yTriple4020sekTar1SEPDEPError= MapsToArrays(Triple4020sekTar1SEPDEPError,n);
	Double_t * yTriple4020= MapsToArrays(Triple4020,n);
	Double_t * yTriple4020Error= MapsToArrays(Triple4020Error,n);
	Double_t * yTriple4010sekTar= MapsToArrays(Triple4010sekTar,n);
	Double_t * yTriple4010sekTarError= MapsToArrays(Triple4010sekTarError,n);
	Double_t * yTriple4010= MapsToArrays(Triple4010,n);
	Double_t * yTriple4010Error= MapsToArrays(Triple4010Error,n);
	Double_t * yTriple4020sekTarFWHM= MapsToArrays(Triple4020sekTarFWHM,n);
	Double_t * yTriple4020sekTarFWHMError= MapsToArrays(Triple4020sekTarFWHMError,n);
	Double_t * yTriple4020FWHM= MapsToArrays(Triple4020FWHM,n);
	Double_t * yTriple4020FWHMError= MapsToArrays(Triple4020FWHMError,n);
	
	Double_t yEffLoss4020[n];

	cout << "Arrays filled!"<<endl;
	//drawing
	//efficiency
	TCanvas *c1 = new TCanvas("c1","A Simple Graph with error bars",200,10,900,700);
	TPad *pad1 = new TPad("pad1","",0,0,1,1);
	TPad *pad2 = new TPad("pad2","",0,0,1,1);
	pad2->SetFillStyle(4000); //will be transparent
	pad1->Draw();
	pad1->cd();
	
	TGraphErrors *graphTr4020sekTar = new TGraphErrors(n,x,yTriple4020sekTar,0,yTriple4020sekTarError);
	TGraphErrors *graphTr4020sekTarSEP = new TGraphErrors(n,x,yTriple4020sekTarSEP,0,yTriple4020sekTarSEPError);
	TGraphErrors *graphTr4020sekTarSEPDEP = new TGraphErrors(n,x,yTriple4020sekTarSEPDEP,0,yTriple4020sekTarSEPDEPError);
	TGraphErrors *graphTr4020 = new TGraphErrors(n,x,yTriple4020,0, yTriple4020Error);
	TGraphErrors *graphTr4010sekTar = new TGraphErrors(n,x,yTriple4010sekTar,0, yTriple4010sekTarError);
	TGraphErrors *graphTr4010 = new TGraphErrors(n,x,yTriple4010,0,yTriple4010Error);
	TGraphErrors *graphTr4020sekTar1 = new TGraphErrors(n,x,yTriple4020sekTar1,0,yTriple4020sekTar1Error);


	for (Int_t i = 0; i<n; i++)
		yEffLoss4020[i]=  (yTriple4020[i]-yTriple4020sekTar[i])/ yTriple4020[i]*100;				
	TGraph *graphEffLoss4020 = new TGraphErrors(n,x,yEffLoss4020);
	
	Double_t TitleLabelSize = 0.05;
	graphTr4020sekTar->SetTitle("");//"Simulation of full-energy-peak-efficiency");
	graphTr4020sekTar->GetHistogram()->SetXTitle("Energy [MeV]");
	//graphTr4020sekTar->GetHistogram()->GetXaxis()->CenterTitle();
	graphTr4020sekTar->GetHistogram()->GetXaxis()->SetTitleSize(TitleLabelSize);
	graphTr4020sekTar->GetHistogram()->GetXaxis()->SetTitleOffset(0.9);
	graphTr4020sekTar->GetHistogram()->GetXaxis()->SetLabelSize(TitleLabelSize);
	graphTr4020sekTar->GetHistogram()->SetYTitle("Efficiency [%]");
	//graphTr4020sekTar->GetHistogram()->GetYaxis()->CenterTitle();
	graphTr4020sekTar->GetHistogram()->GetYaxis()->SetTitleSize(TitleLabelSize);
	graphTr4020sekTar->GetHistogram()->GetYaxis()->SetTitleOffset(0.9);
	graphTr4020sekTar->GetHistogram()->GetYaxis()->SetLabelSize(TitleLabelSize);
	//graphTr4020sekTar->GetHistogram()->GetYaxis()->SetRangeUser(0,9);

	Size_t dMarkerSize = 4;
	graphTr4020sekTar->SetMarkerColor(1);
	graphTr4020sekTar->SetMarkerSize(dMarkerSize/2);
	graphTr4020sekTar->SetMarkerStyle(20);
	graphTr4020sekTar->SetLineColor(1);
	graphTr4020sekTar->SetLineStyle(2);

	graphTr4020->SetMarkerColor(kRed);
	graphTr4020->SetMarkerSize(dMarkerSize/2);
	graphTr4020->SetMarkerStyle(20);
	graphTr4020->SetLineColor(kRed);
	graphTr4020sekTar->SetLineStyle(2);

	graphTr4010sekTar->SetMarkerColor(kGreen);
	graphTr4010sekTar->SetMarkerSize(dMarkerSize/2);
	graphTr4010sekTar->SetMarkerStyle(20);
	graphTr4010sekTar->SetLineColor(kGreen);
	graphTr4020sekTar->SetLineStyle(2);

	graphTr4010->SetMarkerColor(kBlue);
	graphTr4010->SetMarkerSize(dMarkerSize/2);
	graphTr4010->SetMarkerStyle(20);
	graphTr4010->SetLineColor(kBlue);
	graphTr4020sekTar->SetLineStyle(2);

	pad1->Modified();
	
	c1->cd();
 
	graphEffLoss4020->SetMarkerColor(kGreen+3);
	graphEffLoss4020->SetMarkerSize((dMarkerSize+1)/2);
	graphEffLoss4020->SetMarkerStyle(22);
	graphEffLoss4020->SetLineColor(kGreen+3);
	graphEffLoss4020->SetLineStyle(2);
	
	graphTr4020sekTar->GetYaxis()->SetRangeUser(0,9);
	graphEffLoss4020->GetYaxis()->SetRangeUser(0,20);
	graphEffLoss4020->GetHistogram()->SetMaximum(0);
	graphEffLoss4020->GetHistogram()->SetMaximum(11);
	//compute the pad range with suitable margins
	graphTr4020sekTar->GetHistogram()->SetMinimum(0);
	graphTr4020sekTar->GetHistogram()->SetMaximum(11);
	
	Double_t ymin = 0;//graphTr4020sekTar->GetYaxis()->GetXmin();
	Double_t ymax = graphEffLoss4020->GetYaxis()->GetXmax()*2;
	Double_t dy = (ymax-ymin)/0.8; //10 per cent margins top and bottom
	Double_t xmin = graphTr4020sekTar ->GetXaxis()->GetXmin();
	Double_t xmax = graphTr4020sekTar->GetXaxis()->GetXmax();
	Double_t dx = (xmax-xmin)/0.8; //10 per cent margins left and right
	pad2->Range(xmin-0.1*dx,ymin-0.1*dy,xmax+0.1*dx,ymax+0.1*dy);
	



	graphTr4020sekTar->Draw("APL");
	//graphTr4020->Draw("PL");
	//graphTr4010sekTar->Draw("PL");
	//graphTr4010->Draw("PL");

	
	TColor *HIMBlue = new TColor(1780,0,92./256,158./256);
	TBox *Box = new TBox(1,0,8,10);
	Box->SetFillColor(1780);
	Box->SetFillStyle(3004);
	//Box->SetLineColor(11);
	if (useExpectedRange) Box->Draw();
	
	TLatex *Text = new TLatex(0.00115*1000,0.7,"expected range of #gamma in experiment");
	Text->SetTextColor(1780);
	if (useExpectedRange)Text->Draw();
	
	graphTr4020sekTar->Draw("PL");
	TLegend *leg = new TLegend(0.5,0.9,0.85,0.65);
	leg->SetFillColor(kWhite);
	leg -> AddEntry(graphTr4020sekTar,"#splitline{with target and}{tracking system}","p");
	if (Plot20)
	{
		graphTr4020->Draw("PL");
		leg -> AddEntry(graphTr4020,"#splitline{without target and}{tracking system}","p");
	}
	if (Plot10SekTar)
	{
		leg -> AddEntry(graphTr4010sekTar,"#splitline{with target and}{tracking system}, offset 10","p");
		graphTr4010sekTar->Draw("PL");
	}
	if (Plot10)
	{
		leg -> AddEntry(graphTr4010,"#splitline{without target and}{tracking system}, offset 10","p");
		graphTr4010->Draw("PL");
	}
	pad2->Draw();
	pad2->cd();
	graphEffLoss4020->Draw("PL");
	pad2->Update();
	TGaxis *axis = new TGaxis(xmax,ymin,xmax,ymax,ymin,ymax,510,"+L");
	axis->SetTitle("Rel. loss of #gamma due to material [%]");
	axis->SetTitleSize(0.06);
	axis->SetTitleOffset(0.6);
	axis->SetLabelSize(0.045);
	axis->CenterTitle();
	axis->SetVertical();
	axis->SetLabelColor(kGreen+3);
	axis->Draw();
	TArrow *ArrowLoss = new TArrow(8.5*1000,3,10.75*1000.75,3,0.02,"|>");
	ArrowLoss->SetLineColor(kGreen+3);
	ArrowLoss->SetLineWidth(2);
	ArrowLoss->SetFillColor(kGreen+3);
	ArrowLoss->SetAngle(30);
	ArrowLoss->Draw();
	TArrow *ArrowFEPF = new TArrow(.25*1000,6.5,2.5*1000,6.5,0.02,"<|");
	ArrowFEPF->SetLineColor(kRed);
	ArrowFEPF->SetLineWidth(2);
	ArrowFEPF->SetFillColor(kRed);
	ArrowFEPF->SetAngle(30);
	ArrowFEPF->Draw();
	leg->Draw();
c1->Update();

c1->SaveAs("~/pictures/Compare_FEP-Eff.png");

TCanvas *c2 = new TCanvas("c2","A Simple Graph with error bars",200,10,900,700);

	
	TGraphErrors *graphTr4020sekTarFWHM = new TGraphErrors(n,x,yTriple4020sekTarFWHM,0, yTriple4020sekTarFWHMError);
	TGraphErrors *graphTr4020FWHM = new TGraphErrors(n,x,yTriple4020FWHM, 0, yTriple4020FWHMError);
	
	graphTr4020sekTarFWHM->SetTitle("Simulation of resolution");
	graphTr4020sekTarFWHM->GetHistogram()->SetXTitle("#gamma energy [MeV]");
	graphTr4020sekTarFWHM->GetHistogram()->GetXaxis()->CenterTitle();
	graphTr4020sekTarFWHM->GetHistogram()->GetXaxis()->SetTitleSize(0.045);
	graphTr4020sekTarFWHM->GetHistogram()->GetXaxis()->SetTitleOffset(0.75);
	graphTr4020sekTarFWHM->GetHistogram()->GetXaxis()->SetLabelSize(0.045);
	graphTr4020sekTarFWHM->GetHistogram()->SetYTitle("FWHM [keV]");
	graphTr4020sekTarFWHM->GetHistogram()->GetYaxis()->CenterTitle();
	graphTr4020sekTarFWHM->GetHistogram()->GetYaxis()->SetTitleSize(0.045);
	graphTr4020sekTarFWHM->GetHistogram()->GetYaxis()->SetTitleOffset(0.6);
	graphTr4020sekTarFWHM->GetHistogram()->GetYaxis()->SetLabelSize(0.045);   

	Size_t dMarkerSize = 2;
	graphTr4020sekTarFWHM->SetMarkerColor(1);
	graphTr4020sekTarFWHM->SetMarkerSize(dMarkerSize/2);
	graphTr4020sekTarFWHM->SetMarkerStyle(20);
	graphTr4020sekTarFWHM->SetLineColor(1);
	graphTr4020sekTarFWHM->SetLineStyle(2);

	graphTr4020FWHM->SetMarkerColor(kRed);
	graphTr4020FWHM->SetMarkerSize(dMarkerSize/2);
	graphTr4020FWHM->SetMarkerStyle(20);
	graphTr4020FWHM->SetLineColor(kRed);
	graphTr4020sekTarFWHM->SetLineStyle(2);
	
	graphTr4020sekTarFWHM->Draw("APL");
	
	TLegend *leg2 = new TLegend(0.15,0.9,0.5,0.65);
	leg2->SetFillColor(kWhite);
	leg2 -> AddEntry(graphTr4020sekTarFWHM,"#splitline{with target and}{tracking system}","p");
	if (Plot20)
	{
		graphTr4020FWHM->Draw("PL");
		leg2 -> AddEntry(graphTr4020FWHM,"#splitline{without target and}{tracking system}","p");
	}
	leg2->Draw();
	c2->SaveAs("~/pictures/Compare_FWHM.png");
	andi::setCustomStyle();
	// fep with added single and double escape peaks
	TCanvas *c3 = new TCanvas("c3","A Simple Graph with error bars",200,10,900,700);
	c3->SetGrid(1,1);
	c3->SetTicks();
	graphTr4020sekTarSEP->SetMarkerColor(2);
	graphTr4020sekTarSEP->SetMarkerSize(dMarkerSize/2);
	graphTr4020sekTarSEP->SetMarkerStyle(20);
	graphTr4020sekTarSEP->SetLineColor(1);
	graphTr4020sekTarSEP->SetLineStyle(2);
	graphTr4020sekTarSEPDEP->SetMarkerColor(4);
	graphTr4020sekTarSEPDEP->SetMarkerSize(dMarkerSize/2);
	graphTr4020sekTarSEPDEP->SetMarkerStyle(20);
	graphTr4020sekTarSEPDEP->SetLineColor(1);
	graphTr4020sekTarSEPDEP->SetLineStyle(2);
	//andi::setCustomStyle();
	graphTr4020sekTar->GetYaxis()->SetRangeUser(0,9);
	graphTr4020sekTar->Draw("ALP");
	//graphTr4020sekTarSEP->Draw("PL");
	//graphTr4020sekTarSEPDEP->Draw("PL");
	TLegend *leg3 = new TLegend(0.55,0.9,0.9,0.65);
	leg3->SetFillColor(kWhite);
	leg3 -> AddEntry(graphTr4020sekTar,"full energy","p");
	leg3 -> AddEntry(graphTr4020sekTarSEP,"+ single escape","p");
	leg3 -> AddEntry(graphTr4020sekTarSEPDEP,"+ double escape","p");

	//leg3->Draw();
	c3->SaveAs("~/pictures/Compare_FWHMSEPDEP.png");

	// fep tar1 vs tar3
		TCanvas *c4 = new TCanvas("c4","A Simple Graph with error bars",200,10,900,700);
		c4->SetGrid(1,1);
		graphTr4020sekTar1->SetMarkerColor(2);
		graphTr4020sekTar1->SetMarkerSize(dMarkerSize/2);
		graphTr4020sekTar1->SetMarkerStyle(20);
		graphTr4020sekTar1->SetLineColor(1);
		graphTr4020sekTar1->SetLineStyle(2);


		graphTr4020sekTar->Draw("ALP");
		graphTr4020sekTar1->Draw("PL");
		TLegend *leg4 = new TLegend(0.65,0.9,0.9,0.65);
		leg4->SetFillColor(kWhite);
		leg4 -> AddEntry(graphTr4020sekTar1,"old geometry","p");
		leg4 -> AddEntry(graphTr4020sekTar,"improved geometry","p");


		leg4->Draw();
		c4->SaveAs("~/pictures/Compare_geo1geo3.png");
}
