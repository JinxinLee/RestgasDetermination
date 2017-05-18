void confhist(TH1* h, int color=1, int style=1)
{
	h->SetStats(0);
	h->SetLineColor(color);
	h->SetMarkerColor(color);
	h->SetLineStyle(style);
	h->SetLineWidth(2);
}

void plotTMVADemoData()
{
	TFile *f = new TFile("demodata.root");
	TTree *t = (TTree*) f->Get("ntp");
	TString vars[6] = {"m","v1","v2","v3","v4","v5"};
	
	TH1F *htot[6], *hs[6], *hb[6];
	TH2F *hv24s = new TH2F("hv24s","v2:v4;v2;v4",100,t->GetMinimum("v2"),t->GetMaximum("v2"),100,t->GetMinimum("v4"),t->GetMaximum("v4"));
	TH2F *hv24b = new TH2F("hv24b","v2:v4;v2;v4",100,t->GetMinimum("v2"),t->GetMaximum("v2"),100,t->GetMinimum("v4"),t->GetMaximum("v4"));
	TH2F *hv53s = new TH2F("hv53s","v5:v3;v3;v5",100,t->GetMinimum("v5"),t->GetMaximum("v5"),100,t->GetMinimum("v3"),t->GetMaximum("v3"));
	TH2F *hv53b = new TH2F("hv53b","v5:v3;v3;v5",100,t->GetMinimum("v5"),t->GetMaximum("v5"),100,t->GetMinimum("v3"),t->GetMaximum("v3"));
	
	confhist(hv24s,kGreen+2);
	confhist(hv53s,kGreen+2);
	confhist(hv24b,kRed+1);
	confhist(hv53b,kRed+1);
	
	TCanvas *c1 = new TCanvas("c1","c1",20,20,1600,800);
	c1->Divide(4,2,0.001,0.001);
	
	for (int i=0; i<6;++i)
	{
		c1->cd(i+1);
		double xmin = t->GetMinimum(vars[i]);
		double xmax = t->GetMaximum(vars[i]);
		
		//cout <<i<<" "<<vars[i]<<" "<<xmin<<" "<<xmax<<endl;
		htot[i] = new TH1F("htot"+vars[i],vars[i]+";"+vars[i],100,xmin,xmax);
		hs[i]   = new TH1F("hs"+vars[i],vars[i]+";"+vars[i],100,xmin,xmax);
		hb[i]   = new TH1F("hb"+vars[i],vars[i]+";"+vars[i],100,xmin,xmax);
		
		confhist(htot[i],kGray+1,9);
		confhist(hs[i],kGreen+2);
		confhist(hb[i],kRed+1);
		
		t->Project("htot"+vars[i],vars[i]);
		t->Project("hs"+vars[i],vars[i],"signal>0");
		t->Project("hb"+vars[i],vars[i],"signal<1");
		
		htot[i]->Draw();
		hs[i]->Draw("same");
		hb[i]->Draw("same");
		
		c1->Update();
	}
	
	c1->cd(7);
	t->Project("hv24s","v4:v2","signal");
	t->Project("hv24b","v4:v2","signal<1");
	hv24b->Draw();
	hv24s->Draw("same");
	
	c1->cd(8);
	t->Project("hv53s","v3:v5","signal");
	t->Project("hv53b","v3:v5","signal<1");
	hv53b->Draw();
	hv53s->Draw("same");
	
	
}
