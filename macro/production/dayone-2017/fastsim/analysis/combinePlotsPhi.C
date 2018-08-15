
// --------------------------------------------------------------------

void confgraph(TGraph *g, TString tit, int col=1, int marker=20, double shift=0.)
{
	if (tit!="") g->GetHistogram()->SetTitle(tit);
	g->GetHistogram()->SetMinimum(0);
	g->SetLineColor(col);
	g->SetMarkerColor(col);
	g->SetMarkerStyle(marker);
	g->SetMarkerSize(1.5);
	g->SetLineWidth(2);
	
	TF1 *f1 = g->GetFunction("f1");
	
	if (f1)
	{
		f1->SetLineColor(col);
		f1->SetLineStyle(2);
	}
	
	for (int i=0;i<g->GetN();++i)
	{
		double x,y;
		g->GetPoint(i,x,y);
		g->SetPoint(i,x+shift,y);
		
	//	cout <<x<<" "<<y<<endl;
	}
}

// --------------------------------------------------------------------

TH1F* createHistoGraph(TGraph *g, TString tit="", double xmin=0, double xmax=0)
{
	static int cnt=0;
	if (tit=="") tit=g->GetTitle();
	
	double ymax  = TMath::MaxElement(g->GetN(), g->GetY());
	double dymax = g->GetErrorY(TMath::LocMax(g->GetN(), g->GetEY()));
	
	if (xmin>=xmax)
	{
		xmin = TMath::MinElement(g->GetN(), g->GetX());
		xmax = TMath::MaxElement(g->GetN(), g->GetX());
	}	
	TH1F *h=new TH1F(Form("h%03d",cnt++),tit,g->GetN(),xmin-0.5*(xmax-xmin)/g->GetN(), xmax+0.5*(xmax-xmin)/g->GetN());
	
	h->GetXaxis()->SetLabelSize(0.05);
	h->GetXaxis()->SetTitleSize(0.05);
	h->GetYaxis()->SetLabelSize(0.05);
	h->GetYaxis()->SetTitleSize(0.05);
	h->GetYaxis()->SetTitleOffset(1.4);
	
	h->SetMaximum((ymax+dymax)*1.05);
	h->SetMinimum(0);
	
	h->SetStats(0);
	
	return h;
}

// --------------------------------------------------------------------

void combinePlotsPhi(TString fname="anaPhi.root")
{
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(0);
	
	TFile *f=new TFile(fname);
	
	TString plotnam[2]={"phi_A","phi_B"};
	TString lab[4] = {  "signal efficiency;EMC missing (polar #theta) [%];signal efficiency [%]", "background efficiency;EMC missing (polar #theta) [%];background efficiency [%]", "signal to noise;EMC missing (polar #theta) [%];S/B", "significance;EMC missing (polar #theta) [%];significance [#sigma]"};
	
	TGraphErrors *geffs[2];
	TGraphErrors *geffb[2];
	TGraphErrors *gston[2];
	TGraphErrors *gsign[2];

	TCanvas *c1=new TCanvas("c1","c1",10,10,1300,1000);
	c1->Divide(2,2,0.0001,0.0001);

	int colors[2] = {kBlue,kCyan-2};
	//int colors[2] = {2,kRed-7};

	TH1F *h[4]={0};

	for (int i=0;i<2;++i)
	{
		c1->cd(1);
		geffs[i]=(TGraphErrors*)f->Get(Form("effs_%s",plotnam[i].Data()));
		confgraph(geffs[i],"",colors[i],20+(i%2)*3,i);
		if (h[0]==0) h[0]=createHistoGraph(geffs[i],lab[0]);
		if (i==0) h[0]->Draw();
		geffs[i]->Draw("P same");
		
		c1->cd(2);	//gPad->SetLogy();
		geffb[i]=(TGraphErrors*)f->Get(Form("effb_%s",plotnam[i].Data()));
		confgraph(geffb[i],"",colors[i],20+(i%2)*3,i);
		if (h[1]==0) h[1]=createHistoGraph(geffb[i],lab[1]);
		//h[1]->SetMinimum(1e-8);
		//h[1]->SetMaximum(0.0002);
		if (i==0) h[1]->Draw();
		geffb[i]->Draw("P same");
		
		c1->cd(3);	
		gston[i]=(TGraphErrors*)f->Get(Form("sn_%s",plotnam[i].Data()));
		confgraph(gston[i],"",colors[i],20+(i%2)*3,i);
		if (h[2]==0) h[2]=createHistoGraph(gston[i],lab[2]);
		if (i==0) h[2]->Draw();
		gston[i]->Draw("P same");
		
		c1->cd(4);	
		gsign[i]=(TGraphErrors*)f->Get(Form("sign_%s",plotnam[i].Data()));	
		confgraph(gsign[i],"",colors[i],20+(i%2)*3,i);
		if (h[3]==0) h[3]=createHistoGraph(gsign[i],lab[3]);
		if (i==0) h[3]->Draw();
		gsign[i]->Draw("P same");
	}
	
	c1->cd(1);
	TLegend *leg1=new TLegend(0.16,0.18,0.5,0.34);
	leg1->AddEntry(geffs[0],"#phi#phi - Setup A","lep");
	leg1->AddEntry(geffs[1],"#phi#phi - Setup B","lep");
	leg1->Draw();

	c1->cd(2);
	TLegend *leg2=new TLegend(0.16,0.18,0.5,0.34);
	leg2->AddEntry(geffb[0],"#phi#phi - Setup A","lep");
	leg2->AddEntry(geffb[1],"#phi#phi - Setup B","lep");
	leg2->Draw();
	
	c1->cd(3);
	TLegend *leg3=new TLegend(0.16,0.18,0.5,0.34);
	leg3->AddEntry(gston[0],"#phi#phi - Setup A","lep");
	leg3->AddEntry(gston[1],"#phi#phi - Setup B","lep");
	leg3->Draw();
	
	c1->cd(4);
	TLegend *leg4=new TLegend(0.16,0.18,0.5,0.34);
	leg4->AddEntry(gsign[0],"#phi#phi - Setup A","lep");
	leg4->AddEntry(gsign[1],"#phi#phi - Setup B","lep");
	leg4->Draw();
	
	c1->SaveAs("fig/comb_phi.gif");
	
}
