{
  // Macro loads a file after reconstruction and plots difference between initial direction of particle and angular position of cluster
  //gROOT->Macro("style.C");
  gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Tools.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  //TFile* fsim = new TFile("data/sim_emc_g4_new_geom_m3.root"); // (GEANT 3) 29.06.09
  TFile* fsim = new TFile("data/fixed/sim_emc_g3_m3_1000eve_fixed.root"); // (GEANT 4) 1.07.09

  TTree *tsim=(TTree *) fsim->Get("pndsim") ;
  TClonesArray* mctrack_array=new TClonesArray("PndMCTrack");
  tsim->SetBranchAddress("MCTrack",&mctrack_array);

  // -----Point -----
  TClonesArray* point_array=new TClonesArray("PndEmcPoint");
  tsim->SetBranchAddress("EmcPoint",&point_array);
  PndEmcMapper *emcMap=PndEmcMapper::Instance(2);//,tsim);


  Double_t theta_mc, phi_mc, ene_mc;
  Double_t point_ene, point_theta, point_phi,
    x_p, y_p, z_p, module_p, crystal_p, row_p, copy_p,
    crystal_pid,row_pid,
    hit_ene, hit_theta, hit_phi,
    hit_id, hit_x, hit_y;
  Double_t z_cp1_p, z_cp2_p, z_cp3_p, z_cp4_p; // Z position for 4 copies of fwendcap
  Int_t id_p;

  TVector3 photon_momentum;
  TLorentzVector phot;

  int ndigi, npoint;
  double max_energy1=0;

  //----- x, y, z, id
  TH2F *hrow_crystal_p = new TH2F("row_crystal","row vs crystal (POINT)",72,-36,36,72,-36,36);
  TH2F *hx_y_p = new TH2F("x_y_p","x vs y (POINT)",200,-100,100,200,-100,100);
  TH1F *hid_p = new TH1F("id_p","id (POINT)",140000000,300000000,340000000);
  TH1F *hx_p = new TH1F("x_p","X (POINT)",100,-100,100);
  TH1F *hy_p = new TH1F("y_p","Y (POINT)",100,-100,100);
  TH1F *hz_p = new TH1F("z_p","Z (POINT)",100,200,300);
  TH1F *hmodule_p = new TH1F("module_p","Module (POINT)",5,1,6);

  TH2F *hcp_z_p = new TH2F("cp_z_p","Copy vs.Z pos (POINT)",4,1,5,80,200,240);
  TH1F *hz_cp1_p = new TH1F("z_cp1_p","z (POINT)",300,1,300);
  TH1F *hz_cp2_p = new TH1F("z_cp2_p","z (POINT)",300,1,300);
  TH1F *hz_cp3_p = new TH1F("z_cp3_p","z (POINT)",300,1,300);
  TH1F *hz_cp4_p = new TH1F("z_cp4_p","z (POINT)",300,1,300);

  TH2F *hxpad_x_p = new TH2F("xpad_x_p","x vs xpad (POINT)",200,-100,100,200,-100,100);
  TH2F *hypad_y_p = new TH2F("ypad_y_p","y vs ypad (POINT)",200,-100,100,200,-100,100);

  TH2F *hxpad_x_cp1_p = new TH2F("xpad_x_cp1_p","x vs xpad _cp1_ (POINT)",200,-100,100,200,-100,100);
  TH2F *hxpad_x_cp2_p = new TH2F("xpad_x_cp2_p","x vs xpad _cp2_ (POINT)",200,-100,100,200,-100,100);
  TH2F *hxpad_x_cp3_p = new TH2F("xpad_x_cp3_p","x vs xpad _cp3_ (POINT)",200,-100,100,200,-100,100);
  TH2F *hxpad_x_cp4_p = new TH2F("xpad_x_cp4_p","x vs xpad _cp4_ (POINT)",200,-100,100,200,-100,100);

  TH2F *hypad_y_cp1_p = new TH2F("ypad_y_cp1_p","y vs ypad _cp1_ (POINT)",200,-100,100,200,-100,100);
  TH2F *hypad_y_cp2_p = new TH2F("ypad_y_cp2_p","y vs ypad _cp2_ (POINT)",200,-100,100,200,-100,100);
  TH2F *hypad_y_cp3_p = new TH2F("ypad_y_cp3_p","y vs ypad _cp3_ (POINT)",200,-100,100,200,-100,100);
  TH2F *hypad_y_cp4_p = new TH2F("ypad_y_cp4_p","y vs ypad _cp4_ (POINT)",200,-100,100,200,-100,100);

  //--------- Energy: HITS & DIGITS ----------------------------------------------------------
  TH1F *hene_mc= new TH1F("hene_mc","MC energy (GeV)",100,0.0,1.05);

  TH1F *hene_p= new TH1F("hene_p","DATA energy (GeV)",1000,0.0,0.1);

  TH1F *hpoi_ene= new TH1F("hpoi_ene","SUM of Points energy (GeV)",100,0.0,1.05);
  TH1F *hhit_ene= new TH1F("hhit_ene","SUM of Hits energy (GeV)",100,0.0,1.05);

  //--------- Theta & Phi (reconstructed & truth): CLUSTER -----------------------------------
  TH1F *h10= new TH1F("h10","POINT Theta (MC)",180,0.,180.);
  TH1F *h20= new TH1F("h20","POINT Phi (MC)",360,-180.,180.);
  TH1F *h10d= new TH1F("h10d","POINT Theta (data)",180,0.,180.);
  TH1F *h20d= new TH1F("h20d","POINT Phi (data)",360,-180.,180.);

  TH1F *h1= new TH1F("h1","POINT Theta difference",100,-5.,5.);
  TH1F *h2= new TH1F("h2","POINT Phi difference",100,-5.,5.);

  TH1F *h1a= new TH1F("h1a","POINT Theta difference -changed id ",100,-5.,5.);
  TH1F *h2a= new TH1F("h2a","POINT Phi difference - changed id",100,-5.,5.);

  TH1F *h3= new TH1F("h3","HIT Theta difference",100,-5.,5.);
  TH1F *h4= new TH1F("h4","HIT Phi difference",100,-5.,5.);

  TH1F *h3a= new TH1F("h3a","HIT Theta difference -changed id",100,-5.,5.);
  TH1F *h4a= new TH1F("h4a","HIT Phi difference -changed id",100,-5.,5.);


  cout << "POINTs for new FwEndCap ==  " << tsim->GetEntriesFast()<< endl;

  Int_t ncounts = tsim->GetEntriesFast();
  cout << "No of events ==  " << ncounts<< endl;
  for(int j = 0; j < ncounts; j++){
    //for (Int_t j=1;j< 20; j++) {
    Float_t sum_point_ene=0;
    tsim->GetEntry(j);

    PndMCTrack *mctrack=(PndMCTrack *) mctrack_array->At(0); // 1st element in mctrack_array
    photon_momentum=mctrack->GetMomentum();
    theta_mc=photon_momentum.Theta()*(180./TMath::Pi());
    phi_mc=photon_momentum.Phi()*(180./TMath::Pi());//+360.;
    //phot=mctrack->Get4Momentum();

    Int_t npoints = point_array->GetEntries();
    cout << "POINT array ==  " << npoints<< endl;
    for (Int_t i=0; i<npoints; i++)
      {
	PndEmcPoint *point=(PndEmcPoint*)point_array->At(i);

	module_p = point->GetModule();

	if (module_p==3){
	  //hene_mc->Fill(phot.E());

	  point_ene=point->GetEnergyLoss();
	  hene_p->Fill(point_ene);

	  sum_point_ene+=point_ene;

	  TVector3 point_pos(point->GetX(),point->GetY(),point->GetZ());
	  point_theta=point_pos.Theta()*(180./TMath::Pi());
	  point_phi=point_pos.Phi()*(180./TMath::Pi());//+360.;


	  h10->Fill(theta_mc);
	  h20->Fill(phi_mc);

	  h10d->Fill(point_theta);
	  h20d->Fill(point_phi);

	  h1->Fill(point_theta-theta_mc);
	  h2->Fill(point_phi-phi_mc);

	  crystal_p = point->GetCrystal();
	  row_p     = point->GetRow();
	  copy_p    = point->GetCopy();

	  x_p = point->GetX();
	  y_p = point->GetY();
	  z_p = point->GetZ();

	  hz_p->Fill(z_p);
	  if (copy_p==1) hz_cp1_p->Fill(z_p);
	  if (copy_p==2) hz_cp2_p->Fill(z_p);
	  if (copy_p==3) hz_cp3_p->Fill(z_p);
	  if (copy_p==4) hz_cp4_p->Fill(z_p);

	  hxpad_x_p->Fill(point->GetXPad(),x_p);
	  hypad_y_p->Fill(point->GetYPad(),y_p);

	  if (copy_p==1){
	    hxpad_x_cp1_p->Fill(point->GetXPad(),x_p);
	    hypad_y_cp1_p->Fill(point->GetYPad(),y_p);
	  }
	  if (copy_p==2){
	    hxpad_x_cp2_p->Fill(point->GetXPad(),x_p);
	    hypad_y_cp2_p->Fill(point->GetYPad(),y_p);
	  }
	  if (copy_p==3){
	    hxpad_x_cp3_p->Fill(point->GetXPad(),x_p);
	    hypad_y_cp3_p->Fill(point->GetYPad(),y_p);
	  }
	  if (copy_p==4){
	    hxpad_x_cp4_p->Fill(point->GetXPad(),x_p);
	    hypad_y_cp4_p->Fill(point->GetYPad(),y_p);
	  }

	  id_p = point->GetDetectorID();

	  crystal_pid = (id_p%10000);
	  row_pid     = ((id_p/1000000)%100);

	  hmodule_p->Fill(module_p);
	  hcp_z_p->Fill(copy_p,z_p);
	  hrow_crystal_p->Fill(crystal_pid,row_pid);
	  hx_p->Fill(x_p);
	  hy_p->Fill(y_p);
	  hx_y_p->Fill(x_p,y_p);
	  hid_p->Fill(id_p);
	}
      }
    hpoi_ene->Fill(sum_point_ene);
  }

  TCanvas* c1 = new TCanvas("c1", "", 100, 100, 800, 800);
  c1->Divide(3,3);

  gStyle->SetPalette(1);

  c1->cd(1);
  hrow_crystal_p->Draw("colz");
  hrow_crystal_p->GetXaxis()->SetTitle("crystal number");
  hrow_crystal_p->GetYaxis()->SetTitle("row number");

  c1->cd(2);
  hx_y_p->Draw("colz");
  hx_y_p->GetXaxis()->SetTitle("X position");
  hx_y_p->GetYaxis()->SetTitle("Y position");

  c1->cd(3); // Z position in 4 quarters of fwendcap (POINTS)
  TLegend *copies = new TLegend(0.60,0.50,0.85,0.85);
  copies->SetTextSize(0.05);
  copies->SetFillColor(0);
  copies->SetBorderSize(0);
  copies->AddEntry(hz_p, "All copies","l");
  copies->AddEntry(hz_cp1_p, "copy_1","l");
  copies->AddEntry(hz_cp2_p, "copy_2","l");
  copies->AddEntry(hz_cp3_p, "copy_3","l");
  copies->AddEntry(hz_cp4_p, "copy_4","l");

  hz_p->Draw(); // all quarters (copies) together

  hz_cp1_p->SetLineColor(2);
  hz_cp1_p->Draw("same");

  hz_cp2_p->SetLineColor(3);
  hz_cp2_p->Draw("same");

  hz_cp3_p->SetLineColor(4);
  hz_cp3_p->Draw("same");

  hz_cp4_p->SetLineColor(6);
  hz_cp4_p->Draw("same");

  hz_p->GetXaxis()->SetTitle("Z position");
  hz_p->GetYaxis()->SetTitle("Counts");

  copies->Draw();

  c1->Modified();


  c1->cd(4);
  h10->SetLineColor(4); // theta MC (POINTS)
  h10->Draw();
  h10d->SetLineColor(1); // theta DATA (POINTS)
  h10d->Draw("same");
  h10->GetXaxis()->SetTitle("#theta (deg)");

  c1->cd(5);
  h20->SetLineColor(4); // phi MC (POINTS)
  h20->Draw();
  h20d->SetLineColor(1); //phi DATA (POINTS)
  h20d->Draw("same");
  h20->GetXaxis()->SetTitle("#phi (deg)");

  c1->cd(6);
  hxpad_x_p->Draw("colz");
  hxpad_x_p->GetXaxis()->SetTitle("EmcPoint::GetXPad()");
  hxpad_x_p->GetYaxis()->SetTitle("EmcPoint::GetX()");


  c1->cd(7);
  h1->Draw(); // point_theta-theta_monte_carlo
  h1->GetXaxis()->SetTitle("#theta_{POINT}-#theta_{MC}");
  h1->GetYaxis()->SetTitle("Counts");

  c1->cd(8);
  h2->Draw(); // point_phi-phi_monte_carlo
  h2->GetXaxis()->SetTitle("#phi_{POINT}-#phi_{MC}");
  h2->GetYaxis()->SetTitle("Counts");

  c1->cd(9);
  hypad_y_p->Draw("colz");
  hypad_y_p->GetXaxis()->SetTitle("EmcPoint::GetYPad()");
  hypad_y_p->GetYaxis()->SetTitle("EmcPoint::GetY()");

  TCanvas* c2 = new TCanvas("c2", "", 100, 100, 800, 800);
  c2->Divide(3,3);

  gStyle->SetPalette(1);

  c2->cd(1);
  hpoi_ene->SetLineColor(1);
  hpoi_ene->Draw();
  hpoi_ene->GetXaxis()->SetTitle("Energy (GeV)");

  c2->cd(2);
  hcp_z_p->Draw("colz");

  c2->cd(4);
  hx_p->SetLineColor(1);
  hx_p->Draw();
  hx_p->GetXaxis()->SetTitle("X position (cm)");

  c2->cd(5);
  hy_p->SetLineColor(1);
  hy_p->Draw();
  hy_p->GetXaxis()->SetTitle("Y position (cm)");

  c2->Update();

  TCanvas* c3a = new TCanvas("c3a", "", 100, 100, 800, 800);
  c3a->Divide(2,2);

  gStyle->SetPalette(1);

  c3a->cd(1);
  hxpad_x_cp1_p->Draw("colz");
  hxpad_x_cp1_p->GetXaxis()->SetTitle("EmcPoint::GetXPad()  copy =1");
  hxpad_x_cp1_p->GetYaxis()->SetTitle("EmcPoint::GetX()");

  c3a->cd(2);
  hxpad_x_cp2_p->Draw("colz");
  hxpad_x_cp2_p->GetXaxis()->SetTitle("EmcPoint::GetXPad()  copy =2");
  hxpad_x_cp2_p->GetYaxis()->SetTitle("EmcPoint::GetX()");

  c3a->cd(3);
  hxpad_x_cp3_p->Draw("colz");
  hxpad_x_cp3_p->GetXaxis()->SetTitle("EmcPoint::GetXPad()  copy =3");
  hxpad_x_cp3_p->GetYaxis()->SetTitle("EmcPoint::GetX()");

  c3a->cd(4);
  hxpad_x_cp4_p->Draw("colz");
  hxpad_x_cp4_p->GetXaxis()->SetTitle("EmcPoint::GetXPad()  copy =4");
  hxpad_x_cp4_p->GetYaxis()->SetTitle("EmcPoint::GetX()");

  TCanvas* c3b = new TCanvas("c3b", "", 100, 100, 800, 800);
  c3b->Divide(2,2);

  gStyle->SetPalette(1);

  c3b->cd(1);
  hypad_y_cp1_p->Draw("colz");
  hypad_y_cp1_p->GetXaxis()->SetTitle("EmcPoint::GetYPad()  copy =1");
  hypad_y_cp1_p->GetYaxis()->SetTitle("EmcPoint::GetY()");

  c3b->cd(2);
  hypad_y_cp2_p->Draw("colz");
  hypad_y_cp2_p->GetXaxis()->SetTitle("EmcPoint::GetYPad()  copy =2");
  hypad_y_cp2_p->GetYaxis()->SetTitle("EmcPoint::GetY()");

  c3b->cd(3);
  hypad_y_cp3_p->Draw("colz");
  hypad_y_cp3_p->GetXaxis()->SetTitle("EmcPoint::GetYPad()  copy =3");
  hypad_y_cp3_p->GetYaxis()->SetTitle("EmcPoint::GetY()");

  c3b->cd(4);
  hypad_y_cp4_p->Draw("colz");
  hypad_y_cp4_p->GetXaxis()->SetTitle("EmcPoint::GetYPad()  copy =4");
  hypad_y_cp4_p->GetYaxis()->SetTitle("EmcPoint::GetY()");
}
