// Fit energy distribution with Novosibirsk function

int fit_resolution()
{
	Double_t energy0=1.; // GeV

	TF1 *f1 = new TF1("novosibirsk",novosibirsk_fcn,0.7*energy0,1.1*energy0,4);
	f1->SetParameters(1000,energy0,0.01*energy0,0.001);
	f1->SetParNames("constant","mu","sigma","tau");


 	TFile* f = new TFile("cluster_emc.root"); //file you want to analyse
 	TTree *t=(TTree *) f->Get("pndsim") ;
 	TClonesArray* cluster_array=new TClonesArray("PndEmcCluster");
 	t->SetBranchAddress("EmcCluster",&cluster_array);

	double cluster_energy;
 	TH1F *h1= new TH1F("h1","Cluster energy",200,0.7*energy0,1.1*energy0);

	// Cluster energy
 	for (Int_t j=0; j< t->GetEntriesFast(); j++)
 	{
 		t->GetEntry(j);
 		for (Int_t i=0; i<cluster_array->GetEntriesFast(); i++)
 		{
 			PndEmcCluster *cluster=(PndEmcCluster*)cluster_array->At(i);
 			cluster_energy=cluster->energy();
 			if ((cluster->NumberOfDigis()>1)&&(cluster_energy>0.02))
 				h1->Fill(cluster_energy);
 		}
 	}

 	TCanvas* c1 = new TCanvas("c1", "Cluster Energy", 100, 100, 800, 800);
 	h1->SetTitle("Cluster energy");
 	h1->GetXaxis()->SetTitle("Energy, GeV");
 	h1->Draw();
	h1->Fit(f1);

  return 0;
}

Double_t novosibirsk_fcn(Double_t *xp, Double_t *par)
{
  // from root (RooNovosibirsk.cxx)
  Double_t x=xp[0];
    Double_t a=par[0];
    Double_t mu=par[1];
    Double_t sigma=par[2];
    Double_t tau=par[3];

  if (TMath::Abs(tau) < 1.e-7) {
    return TMath::Exp( -0.5 * TMath::Power( ( (x - mu) / sigma), 2 ));
  }

  Double_t arg = 1.0 - ( x - mu ) * tau / sigma;

  if (arg < 1.e-7) {
    //Argument of logaritem negative. Real continuation -> function equals zero
    return 0.0;
  }

  Double_t log = TMath::Log(arg);
  static const Double_t xi = 2.3548200450309494; // 2 Sqrt( Ln(4) )

  Double_t width_zero = ( 2.0 / xi ) * TMath::ASinH( tau * xi * 0.5 );
  Double_t width_zero2 = width_zero * width_zero;
  Double_t exponent = ( -0.5 / (width_zero2) * log * log ) - ( width_zero2 * 0.5 );

  return a*TMath::Exp(exponent);
}
