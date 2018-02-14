// --------------------------------------------------------------------

void confgraph(TGraph *g, TString tit, int col=1, int marker=20)
{
	g->GetHistogram()->SetTitle(tit);
	g->GetHistogram()->SetMinimum(0);
	g->SetLineColor(col);
	g->SetMarkerColor(col);
	g->SetMarkerStyle(marker);
	g->SetMarkerSize(1);
	g->SetLineWidth(2);
}

// --------------------------------------------------------------------

void confhist(TH1* h)
{
	h->GetXaxis()->SetLabelSize(0.05);
	h->GetXaxis()->SetTitleSize(0.05);
	h->GetYaxis()->SetLabelSize(0.05);
	h->GetYaxis()->SetTitleSize(0.05);
	h->GetYaxis()->SetTitleOffset(1.4);
	
	h->GetXaxis()->SetNdivisions(505);
	h->SetMinimum(0);
	h->SetStats(0);	
}

// --------------------------------------------------------------------

TH1F* createHistoGraph(TGraph *g, TString tit="", double xmin=0, double xmax=0, double ymaxf=1.05)
{
	static int cnt=0;
	if (tit=="") tit=g->GetTitle();
	
	double ymax  = TMath::MaxElement(g->GetN(), g->GetY());
	double dymax = g->GetErrorY(TMath::LocMax(g->GetN(), g->GetEY()));
	
	if (xmin==xmax)
	{
		xmin = TMath::MinElement(g->GetN(), g->GetX());
		xmax = TMath::MaxElement(g->GetN(), g->GetX());
	}	
	TH1F *h=new TH1F(Form("h%03d",cnt++),tit,g->GetN(),xmin-0.5*(xmax-xmin)/g->GetN(), xmax+0.5*(xmax-xmin)/g->GetN());

	confhist(h);	
	//h->GetXaxis()->SetLabelSize(0.05);
	//h->GetXaxis()->SetTitleSize(0.05);
	//h->GetYaxis()->SetLabelSize(0.05);
	//h->GetYaxis()->SetTitleSize(0.05);
	//h->GetYaxis()->SetTitleOffset(1.4);
	
	//h->GetXaxis()->SetNdivisions(505);
	//h->SetMinimum(0);
	//h->SetStats(0);
	
	h->SetMaximum((ymax+dymax)*ymaxf);
		
	return h;
}
// --------------------------------------------------------------------
// --------------------------------------------------------------------


void analyse_phi2(double t=2, double sigsmin=500, double sigsmax=500,  int Nrep=1000, int steps = 25, double emin = 2.25, double emax = 2.35, double m=2.3, double G=0.015)
{
	//gStyle->SetStatX(0.5);
	//gStyle->SetStatY(0.40);
	gStyle->SetStatW(0.20);
	gStyle->SetStatH(0.12);
	
	double sigs=(sigsmax+sigsmin)*0.5;
	
	// C. Evangelista et al., Phys. Rev. D57, 5370 (1998).
	double x[25]  = {2.219, 2.221, 2.221, 2.222, 2.224, 2.226, 2.226, 2.228, 2.229, 2.231, 2.231, 2.233, 2.235, 2.236, 2.242, 2.247, 2.254,  2.255, 2.272, 2.289, 2.307, 2.333, 2.360, 2.404, 2.430};
	double dx[25] = {0.0};
	
	double y[25]  = {4.15,  4.16,  3.87,  3.94,  4.40,  3.90,  3.68,  3.00,  3.20,  3.73,  2.97,  4.20,  3.52,  2.48,  3.38,  2.68,  2.63,   3.01,  2.25,  2.15,  1.97,  2.04,  2.04,  1.61,  1.93 };
	double dy[25] = {0.34,  0.36,  0.54,  0.35,  0.36,  0.34,  0.34,  0.30,  0.32,  0.35,  0.33,  0.35,  0.34,  0.30,  0.45,  0.39,  0.35,   0.16,  0.17,  0.21,  0.24,  0.14,  0.12,  0.18,  0.11 };


	TGraphErrors *gphi = new TGraphErrors(25,x,y,dx,dy);
	
	confgraph(gphi,"#phi#phi cross section [JetSet];E_{cm} [GeV];#sigma [#mub]");
	TH1F *hphi = createHistoGraph(gphi,"#phi#phi cross section [JetSet];E_{cm} [GeV];#sigma [#mub]",2.21,2.435);
	
	//TF1 *f1=new TF1("f1","expo",2.21,2.435);
	TF1 *f1=new TF1("f1","0.5*[0]*(1.0-TMath::Erf((x-[1])/[2]))+[3]",2.21,2.435);
	f1->SetParameters(3,2.23,0.01,1);	

	// inelasic cross section in mb
	TF1 *finel=new TF1("finel","[0]+[1]/x^[2]-[3]/x^[4]");
	finel->SetParameters(32,82,0.67,45,0.85);
	finel->SetRange(1.,100.);
	
	TCanvas *c1=new TCanvas("c1","c1",20,20,1000,1000);
	c1->Divide(2,2,0.0001,0.0001);
	
	c1->cd(1);
	hphi->Draw();
	gphi->Fit("f1","R");
	gphi->Draw("Psame");

	TLine l;
	l.SetLineColor(4);
	l.SetLineStyle(9);
	l.SetLineWidth(2);
	l.DrawLine(emin,0,emin,hphi->GetMaximum()*0.5);	
	l.DrawLine(emax,0,emax,hphi->GetMaximum()*0.5);	
	
	
	double effs   = 22463./100000.;     // signal efficiency: cut = abs(summ4-2.15)<0.15&&abs(f4cxd0m-1.02)<0.012&&abs(f4cxd1m-1.02)<0.012&chi24c<50	
	//double sigs   = 100.;               // signal cross section [nb]
	double fBR    = 0.489*0.489;        // BR(phi->K+ K-)^2
	
	double effg   = 77./840.5e6;        // generic background efficiency
	
	double sigphi = f1->Eval(2.3)*1000; // phi phi cross section [nb]
	double sigg   = 60e6;               // generic cross section [nb]
	
	//double t      = 0.5;                // [d]
	double Lint   = 788;               // [1./(nb*d)]
	
	double S      = sigs*effs*fBR*t*Lint;    // expected number of signals
	double Bphi   = sigphi*effs*fBR*t*Lint;  // expected number of non-res phi phi       
	double Bg     = sigg*effg*t*Lint;        // expected number of 
	
	cout <<"S="<<S<<"  B_phi(sig ="<<sigphi<<" nb)="<<Bphi<<"  B_g="<<Bg<<endl;


	// signal cross section (BW)
	TF1 *fbw = new TF1("fbw","[0]*[3]*TMath::BreitWigner(x,[1],[2])");
	fbw->SetParNames("#sigma_{S}","m","#Gamma","f_{scale}");

	// background function
	TF1 *fbg=new TF1("fbg","pol3",2.21,2.435);

	// combined fit function
	TF1 *ffit = new TF1("ffit","[0]*TMath::BreitWigner(x,[1],[2]) + pol3(3)");
	ffit->SetParNames("A","m","#Gamma","a_{0}","a_{1}","a_{2}","a_{3}");
	
	
	
	
	double stsize = (emax-emin)/(steps-1);
	
	TGraphErrors *gscan = new TGraphErrors(steps);
	TGraphErrors *gscansig = new TGraphErrors(steps);
	TGraphErrors *gscanbg = new TGraphErrors(steps);
	confgraph(gscan,Form("#phi#phi scan (t=%.2fd);E_{cm} [GeV];counts",t));
	confgraph(gscanbg,Form("#phi#phi scan (t=%.2fd);E_{cm} [GeV];counts",t),16);
	confgraph(gscansig,Form("#phi#phi scan (t=%.2fd);E_{cm} [GeV];counts",t));
	
	double st_t = t/steps;
	
	TH1F *hsigs = new TH1F("hsigs",Form("#sigma_{S}, %d fits",Nrep),100,0,sigs*3); 
	confhist(hsigs);
	hsigs->SetLineWidth(2);
	
	TH1F *hscan=0, *hscansig=0;	
	
	for (int j=0;j<Nrep;++j)
	{
		fbw->SetParameters(1,m,G,1);
		fbw->FixParameter(3,1);

		for (int i=0;i<steps;++i)
		{
			double ecm = emin + i*stsize;
			
			double st_sigs   = fbw->Eval(ecm)/fbw->Eval(m)*sigs;
			double st_sigphi = f1->Eval(ecm)*1000; // mub -> nb
			double st_sigg   = finel->Eval(ecm)*1e6; // mub -> nb
			
			double yieldS    = gRandom->Poisson(st_sigs * effs * fBR * st_t * Lint);
			double yieldBphi = gRandom->Poisson(st_sigphi * effs * fBR * st_t * Lint);
			double yieldBg   = gRandom->Poisson(st_sigg * effg * st_t * Lint);
			
			//printf("(%2d) Ecm = %5.3f GeV : sig_S = %.2f nb (S = %6.0f)   sig_phi = %.2f nb (B = %6.0f)    sig_gen = %.2f mub (Bg = %5.0f)\n", i, ecm,  st_sigs, yieldS, st_sigphi, yieldBphi, st_sigg/1000, yieldBg);

			double yield     = (yieldS + yieldBg + yieldBphi);
			
			gscan->SetPoint(i,ecm, yield);
			gscan->SetPointError(i, 0, sqrt(yield));
			
			gscanbg->SetPoint(i,ecm, gRandom->Poisson(yieldBg));
			gscanbg->SetPointError(i, 0, sqrt(yieldBg));
		}	
			
		ffit->SetParameters(sigs,m,G,1,1,1);
		ffit->FixParameter(1,m);
		ffit->FixParameter(2,G);
		
		gscan->Fit("ffit","0q");
		ffit->SetParLimits(1,m*0.7,m*1.3);
		ffit->SetParLimits(2,G*0.5,G*1.5);
		gscan->Fit("ffit","qm");
		
		c1->cd(2);	
		if (hscan==0) hscan = createHistoGraph(gscan,Form("#phi#phi scan (t = %.2fd, #sigma = %.0fnb);E_{cm} [GeV];counts",t,sigs),0,0,1.3);
		hscan->Draw();	
		gscan->Draw("Psame");	
		gscanbg->Draw("Psame");
		
		fbg->SetParameters(ffit->GetParameter(3),ffit->GetParameter(4),ffit->GetParameter(5),ffit->GetParameter(6));

		
		for (int i=0;i<steps;++i)
		{
			double x,y,dx,dy;
			gscan->GetPoint(i,x,y);
			gscansig->SetPoint(i,x,y-fbg->Eval(x));
			gscansig->SetPointError(i,gscan->GetErrorX(i),gscan->GetErrorY(i));
			
			//cout <<x<<" "<<fbg->Eval(x)<<endl;
		}
		
		fbw->FixParameter(3,1);	
		fbw->SetParLimits(1,m*0.7,m*1.3);
		fbw->SetParLimits(2,G*0.6,G*1.5);
		gscansig->Fit("fbw","q");
		fbw->SetParameter(0,1);
		//cout<<fbw->Eval(fbw->GetParameter(1))<<"  "<<fbw->GetParameter(0)<<"  "<<fbw->GetParameter(1)<<"  "<<fbw->GetParameter(2)<<"  "<<fbw->GetParameter(3)<<endl;
		fbw->FixParameter(3, 1./fbw->Eval(fbw->GetParameter(1))*(effs*fBR*st_t*Lint) );
		fbw->SetParameter(0,TMath::MaxElement(gscansig->GetN(),gscansig->GetY())/(effs*fBR*st_t*Lint));
		gscansig->Fit("fbw","q");
		
		c1->cd(3);
		if (hscansig==0) hscansig = createHistoGraph(gscansig,Form("#phi#phi scan (t = %.2fd, #sigma = %.0fnb);E_{cm} [GeV];counts",t,sigs),0,0,1.3);
		hscansig->SetMinimum(-100);
		hscansig->Draw();
		gscansig->Draw("Psame");
		
		//fbw->Print();
		double ss = fbw->GetParameter(0);
		hsigs->Fill(ss);	
		
		
		//cout <<"sig_in = "<<sigs<<"   sig_out = "<< ss<<"   "<<TMath::MaxElement(gscansig->GetN(),gscansig->GetY())<<"  "<<fbw->GetParameter(0)<<"  "<<fbw->GetParameter(1)<<"  "<<fbw->GetParameter(2)<<"  "<<fbw->GetParameter(3)<<endl;		
		
		c1->cd(4);
		hsigs->Draw();
		
		if (j%100==0) c1->Update();
		
		//if (ss>880) return;
		//sleep(1);
	}
	
	//c1->cd(2);	
	//TH1F *hscan = createHistoGraph(gscan,Form("#phi#phi scan (t = %.2fd, #sigma = %.0fnb);E_{cm} [GeV];counts",t,sigs),0,0,1.3);
	//hscan->Draw();	
	//gscan->Draw("Psame");	
	//gscanbg->Draw("Psame");
	
	//c1->cd(3);
	//TH1F *hscansig = createHistoGraph(gscansig,Form("#phi#phi scan (t = %.2fd, #sigma = %.0fnb);E_{cm} [GeV];counts",t,sigs),0,0,1.3);
	//hscansig->SetMinimum(-100);
	//hscansig->Draw();
	//gscansig->Draw("Psame");

	c1->cd(4);
	hsigs->Draw("e");
	hsigs->SetStats(1);
	hsigs->Fit("gaus");
	
	//fbw->FixParameter(3,1);	
	//fbw->SetParLimits(1,m*0.7,m*1.3);
	//fbw->SetParLimits(2,G*0.5,G*1.5);
	//gscansig->Fit("fbw");
	//fbw->SetParameter(0,1);
	//fbw->FixParameter(3, 1./fbw->GetMaximum(emin,emax) );
	//fbw->SetParameter(0,TMath::MaxElement(gscansig->GetN(),gscansig->GetY()));
	//gscansig->Fit("fbw");
	
	
	c1->Update();
	TPaveStats *st = (TPaveStats *)gscan->GetListOfFunctions()->FindObject("stats");
	if (st)
	{
		cout <<"good"<<endl;
		st->SetX1NDC(0.5); //new x start position
		st->SetX2NDC(0.5); //new x end position
		c1->Modified();
		c1->Update();
	}
	else cout <<"***"<<endl;
		
}

