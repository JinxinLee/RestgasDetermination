{
  gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Tools.C");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  TFile* fsim = new TFile("../../emc_complete_1GeVphoton_FwEndCap.root");
  
  TTree *tsim=(TTree *) fsim->Get("cbmsim") ;
  TClonesArray* mctrack_array=new TClonesArray("PndMCTrack");
  tsim->SetBranchAddress("MCTrack",&mctrack_array);

  // -----Point -----
  TClonesArray* point_array=new TClonesArray("PndEmcPoint");
  tsim->SetBranchAddress("EmcPoint",&point_array);

  // -----Hit -----
  TClonesArray* hit_array=new TClonesArray("PndEmcHit");
  tsim->SetBranchAddress("EmcHit",&hit_array);
    
  Double_t theta_mc, phi_mc, ene_mc;
  Double_t point_ene, point_theta, point_phi,
	   x_p, y_p, z_p, module_p,
	   hit_ene, hit_theta, hit_phi, hit_id, hit_x, hit_y;
  Int_t id_p, crystal_pid, row_pid;

  TVector3 photon_momentum;
  TLorentzVector p4mom;
  
  TH2F *hypad_xpad_p = new TH2F("ypad_xpad","crystal row vs col number (derived from POINT)",74,-37,37,74,-37,37); 
  TH2F *hx_y_p = new TH2F("x_y_p","x vs y (derived from POINT)",1000,-100,100,1000,-100,100); 
  TH1F *hid_p = new TH1F("id_p","id (derived from POINT)",140000000,300000000,340000000);
  TH1F *hx_p = new TH1F("x_p","X (derived from POINT)",200,-100,100);
  TH1F *hy_p = new TH1F("y_p","Y (derived from POINT)",200,-100,100);
  TH1F *hz_p = new TH1F("z_p","Z (derived from POINT)",100,200,300);
  TH1F *hmodule_p = new TH1F("module_p","Module (derived from POINT)",5,0.5,5.5);
  
  TH2F *hxpad_x_p = new TH2F("xpad_x_p","x vs xpad (derived from POINT)",2*37.5,-37.5,37.5,211,-105,105); 
  TH2F *hypad_y_p = new TH2F("ypad_y_p","y vs ypad (derived from POINT)",2*37.5,-37.5,37.5,211,-105,105); 

  //--------- Energy: HITS & DIGIS ----------------------------------------------------------
  TH1F *hene_mc= new TH1F("hene_mc","MC energy (GeV)",100,0.0,1.05); 
  TH1F *hene_p= new TH1F("hene_p","DATA energy (GeV)",1000,0.0,0.1); 

  TH1F *hpoi_ene= new TH1F("hpoi_ene","SUM of Point energies (GeV)",100,0.0,1.05); 
  TH1F *hpoi_ene11_12= new TH1F("hpoi_ene11_12","crystal [11,12] dep. energy (GeV)",100,0.0001,0.2); 
  TH1F *hpoi_ene12_12= new TH1F("hpoi_ene12_12","crystal [12,12] dep. energy (GeV)",100,0.0001,0.2); 
  TH1F *hpoi_ene13_12= new TH1F("hpoi_ene13_12","crystal [13,12] dep. energy (GeV)",100,0.0001,0.2); 
  TH1F *hpoi_ene14_12= new TH1F("hpoi_ene14_12","crystal [14,12] dep. energy (GeV)",100,0.0001,0.2); 
  
  
  //--------- Theta & Phi (reconstructed & real): CLUSTER -----------------------------------
  TH1F *h10= new TH1F("h10","POINT Theta (MC)",180,0.,180.);
  TH1F *h20= new TH1F("h20","POINT Phi (MC)",360,0.,360.);
  TH1F *h10d= new TH1F("h10d","POINT Theta (data)",180,0.,180.);
  TH1F *h20d= new TH1F("h20d","POINT Phi (data)",360,0.,360.);

  TH1F *h1= new TH1F("h1","POINT Theta difference",100,-2.,2.);
  TH1F *h2= new TH1F("h2","POINT Phi difference",100,-2.,2.);

  Int_t ncounts = tsim->GetEntriesFast();
  cout << "Number of events = " << ncounts<< endl;
  //ncounts=2000; //in case you want to analyse specific number of events
/*
  Double_t crystalDepEnergy[36][37];
  for (Int_t i=0; i<=35; i++) //presetting of variables
    for (Int_t j=0; j<=36; j++)
      crystalDepEnergy[i][j]=0;
  
  Int_t totalpoints=0;//counter on the total number of MC points in all events
  for(int j = 0; j < ncounts; j++){ //loop repeated per event
    tsim->GetEntry(j);
    Int_t npoints = point_array->GetEntries(); //number of points per event
    if (npoints>=1) //in case you want to analyse only the first interaction point per event
     {npoints = 1;    
         totalpoints+=npoints;
     }    
  }
  cout<<"totalpoints = "<<totalpoints<<"\n";

  Double_t x[totalpoints], xx[totalpoints], y[totalpoints], yy[totalpoints], zz[totalpoints];//here xx,yy,zz represent the point position WHILE x and y represent the column and row numbers, respectively
  //columns 3,7,11,15,19,23,27,31,35 are critical columns & rows 2,6,10,14,18,22,26,30,34 are critical rows:
  Double_t z3[totalpoints], z7[totalpoints], z11[totalpoints], z15[totalpoints], z19[totalpoints], z23[totalpoints], z27[totalpoints], z31[totalpoints], z35[totalpoints];//z-positions for the "x_point vs column number"
  Double_t z2[totalpoints], z6[totalpoints], z10[totalpoints], z14[totalpoints], z18[totalpoints], z22[totalpoints], z26[totalpoints], z30[totalpoints], z34[totalpoints];//z-positions for the "y_point vs row number"  
  for (Int_t k=0; k<totalpoints; k++) {//presetting variables
   z3[k]=z7[k]=z11[k]=z15[k]=z19[k]=z23[k]=z27[k]=z31[k]=z35[k]=0;
   z2[k]=z6[k]=z10[k]=z14[k]=z18[k]=z22[k]=z26[k]=z30[k]=z34[k]=0;
  }  
  Int_t ii=0; //counter on the number of points per event (increases up until "totalpoints")
  //Double_t x3[totalpoints]=0;
  //Double_t y3[totalpoints]=0;

  for(int j = 0; j < ncounts; j++){
    Double_t sum_point_ene=0;
    tsim->GetEntry(j);

    PndMCTrack *mctrack=(PndMCTrack *) mctrack_array->At(0); // 1st element in mctrack_array
    photon_momentum=mctrack->GetMomentum();
    theta_mc=photon_momentum.Theta()*(180./TMath::Pi());
    phi_mc=photon_momentum.Phi()*(180./TMath::Pi());
    //p4mom=mctrack->Get4Momentum();

    Int_t npoints = point_array->GetEntries();
    //cout << "point_array size= " << npoints << endl;
    if (npoints>=1) //in case you want to analyse only the first interaction point per event
      npoints = 1;

    for (Int_t i=0; i<npoints; i++) //loop repeated per MC point
      {
	PndEmcPoint *point=(PndEmcPoint*)point_array->At(i); // get the i-th point of the event   
	module_p = point->GetModule();

	if (module_p==3){
	  //hene_mc->Fill(p4mom.E());
	  point_ene=point->GetEnergyLoss();
	  hene_p->Fill(point_ene);
	  sum_point_ene+=point_ene;

	  TVector3 point_pos(point->GetX(),point->GetY(),point->GetZ());
	  point_theta=point_pos.Theta()*(180./TMath::Pi());
	  point_phi=point_pos.Phi()*(180./TMath::Pi());
	   
	  h10->Fill(theta_mc);
	  h20->Fill(phi_mc);
	  h10d->Fill(point_theta);
	  h20d->Fill(point_phi);    
	  h1->Fill(point_theta-theta_mc);
	  h2->Fill(point_phi-phi_mc);

	  id_p = point->GetDetectorID();
  	  crystal_pid = -(id_p%10000 - 36);         // crystal column number (see PndEmcPoint.cxx)
	  row_pid     = (id_p/1000000)%100 - 37;   // crystal row number

	  x_p = point->GetX();
	  y_p = point->GetY();
	  z_p = point->GetZ();

///////////////////////////////////////////////////////////////
	  zz[ii]=z_p;
	  xx[ii]=x_p;
	  yy[ii]=y_p;
	  if (point->GetXPad() < 0)
	    x[ii]=point->GetXPad() + 1;
	  else
	    x[ii]=point->GetXPad();
	  if (point->GetYPad() < 0)
	    y[ii]=point->GetYPad() + 1;
	  else
	    y[ii]=point->GetYPad();
	  
	  
	  if (x[ii]==3) // && fabs(y[ii])>8
	  {
	    z3[ii]=z_p;    
	    //x3[ii]=x_p;
	    //y3[ii]=y_p;
	  }
	  else if (x[ii]==7)
	    z7[ii]=z_p;
	  else if (x[ii]==11)
	    z11[ii]=z_p;
	  else if (x[ii]==15)
	    z15[ii]=z_p;
	  else if (x[ii]==19)
	    z19[ii]=z_p;
	  else if (x[ii]==23)
	    z23[ii]=z_p;
	  else if (x[ii]==27)
	    z27[ii]=z_p;
	  else if (x[ii]==31)
	    z31[ii]=z_p;
	   
	  if (y[ii]==2) // && fabs(x[ii])>15
	    z2[ii]=z_p;
	  else if (y[ii]==6)
	    z6[ii]=z_p;
	  else if (y[ii]==10)
	    z10[ii]=z_p;
	  else if (y[ii]==14)
	    z14[ii]=z_p;
	  else if (y[ii]==18)
	    z18[ii]=z_p;
	  else if (y[ii]==22)
	    z22[ii]=z_p;
	  else if (y[ii]==26)
	    z26[ii]=z_p;
	  else if (y[ii]==30)
	    z30[ii]=z_p;
	  else if (y[ii]==34)
	    z34[ii]=z_p;
	  
  	  ii+=1;
///////////////////////////////////////////////////////////////
	  hz_p->Fill(z_p);  
	  hxpad_x_p->Fill(point->GetXPad(),x_p);
	  hypad_y_p->Fill(point->GetYPad(),y_p);  
	  hypad_xpad_p->Fill(point->GetXPad(),point->GetYPad());
	  hmodule_p->Fill(module_p);
	  hx_p->Fill(x_p);
	  hy_p->Fill(y_p);
	  hx_y_p->Fill(x_p,y_p);  
	  hid_p->Fill(id_p);
	  
	  if (crystal_pid == 11 && row_pid == 12) // deposited energy per event in the crystal[col=11,row=12]
	    crystalDepEnergy[11][12]+=point_ene;
	  else if (crystal_pid == 12 && row_pid == 12)
	    crystalDepEnergy[12][12]+=point_ene;
	  else if (crystal_pid == 13 && row_pid == 12)
	    crystalDepEnergy[13][12]+=point_ene;
	  else if (crystal_pid == 14 && row_pid == 12)
	    crystalDepEnergy[14][12]+=point_ene;
	}
      }
    hpoi_ene->Fill(sum_point_ene); // deposited energy per event in the whole FwEndCap
    hpoi_ene11_12->Fill(crystalDepEnergy[11][12]);
    hpoi_ene12_12->Fill(crystalDepEnergy[12][12]);
    hpoi_ene13_12->Fill(crystalDepEnergy[13][12]);
    hpoi_ene14_12->Fill(crystalDepEnergy[14][12]);
    
    crystalDepEnergy[11][12]=crystalDepEnergy[12][12]=crystalDepEnergy[13][12]=crystalDepEnergy[14][12]=0;
  }
 */
////////////////////////////////////////////////////////////////////////// HIT ANALYSIS
   Int_t module_h, id_h, crystal_hid, row_hid;
   TH1F *HISTene_h= new TH1F("HISTene_h","energy (hit)",1000,0.0001,1.01);  // deposited energy per hit in the whole FwEndCap
   TH1F *HISThit_ene= new TH1F("HISThit_ene","FwEndCap energy (hit)",1000,0.0,1.05); // deposited energy per event in the whole FwEndCap
   TH1F *HISTz_h = new TH1F("HISTz_h","z (hit)",100,200,300);
   TH2F *HISTrow_x_h = new TH2F("HISTrow_x_h","row vs x (hit)",200,-100,100,2*38.5,-38.5,38.5); 
   TH2F *HISTrow_crystal_h = new TH2F("HISTrow_crystal_h","row vs column (hit)",2*38.5,-38.5,38.5,2*38.5,-38.5,38.5); 
   TH2F *HISTy_x_h = new TH2F("HISTy_x_h","y vs x (hit)",211,-105,105,211,-105,105);  // y- versus x-position of the center of the hit crystal
   TH1F *HISTtheta_h = new TH1F("HISTtheta_h","theta (hit)",180,0,180);  // theta of the 
   TH2I *HISTxpad_x_h = new TH2I("HISTxpad_x_h","",10500,-105,105,100*38.5,-38.5,38.5); 
   TH2I *HISTypad_y_h = new TH2I("HISTypad_y_h","",10500,-105,105,100*38.5,-38.5,38.5);
   
   //TH1F *HISTid_h = new TH1F("HISTid_h","id (HIT)",140000000,300000000,340000000);
  TH1F *HISTOhit_ene11_12= new TH1F("HISTOhit_ene11_12","crystal [11,12] dep. energy (GeV)",100,0.0001,1.01); 
  TH1F *HISTOhit_ene12_12= new TH1F("HISTOhit_ene12_12","crystal [12,12] dep. energy (GeV)",100,0.0001,1.01); 
  TH1F *HISTOhit_ene13_12= new TH1F("HISTOhit_ene13_12","crystal [13,12] dep. energy (GeV)",100,0.0001,1.01); 
  TH1F *HISTOhit_ene14_12= new TH1F("HISTOhit_ene14_12","crystal [14,12] dep. energy (GeV)",100,0.0001,1.01);
  
  
   Int_t totalhits=0;  //counter on the total number of hits (fired detectors) in all events  
   Int_t efficiencyCounter=0;
   Int_t atLeastOneHitPerEventCounter=0;
   for(int j = 0; j < ncounts; j++){
     Int_t efficiencyFlag=0;
     Int_t atLeastOneHitPerEventFlag=0;
     Double_t sum_hit_ene=0;

     tsim->GetEntry(j);

	PndMCTrack *mctrack=(PndMCTrack *) mctrack_array->At(0); 
	photon_momentum=mctrack->GetMomentum();
	theta_mc=photon_momentum.Theta()*(180./TMath::Pi());     
	phi_mc=photon_momentum.Phi()*(180./TMath::Pi());
	//p4mom=mctrack->Get4Momentum();
	
	Int_t nhits = hit_array->GetEntries();
	totalhits+=nhits;
	if (nhits!=0)
	  atLeastOneHitPerEventFlag = 1;
	
	for (Int_t i=0; i<nhits; i++)
	  {
	    PndEmcHit *hit=(PndEmcHit*)hit_array->At(i); // get the i-th hit (detector) of the event   
	    module_h = hit->GetModule();

	    if (module_h==3){
	      //hene_mc->Fill(p4mom.E());
	      hit_ene=hit->GetEnergy();
	      HISTene_h->Fill(hit_ene); 
	      if (hit_ene >= 0.010) // 10 MeV threshold used (here only) to calculate the efficiency of the FwEndCap geometry
		efficiencyFlag = 1;
		
		
	      //filling some histograms for a few crystals
	      if (hit->GetXPad()==11 && hit->GetYPad()==12)
		HISTOhit_ene11_12->Fill(hit_ene);
	      else if (hit->GetXPad()==12 && hit->GetYPad()==12)
		HISTOhit_ene12_12->Fill(hit_ene);
	      else if (hit->GetXPad()==13 && hit->GetYPad()==12)
		HISTOhit_ene13_12->Fill(hit_ene);
	      else if (hit->GetXPad()==14 && hit->GetYPad()==12)
		HISTOhit_ene14_12->Fill(hit_ene);
	      
	      sum_hit_ene+=hit_ene;

	      TVector3 hit_pos(hit->GetX(),hit->GetY(),hit->GetZ()); // position of the center of the hit crystal
	      hit_theta=hit_pos.Theta()*(180./TMath::Pi());
	      hit_phi=hit_pos.Phi()*(180./TMath::Pi());
	    
	      h10->Fill(theta_mc);
	      h20->Fill(phi_mc);    
	      h10d->Fill(hit_theta);
	      h20d->Fill(hit_phi);    
	      h1->Fill(hit_theta-theta_mc);
	      h2->Fill(hit_phi-phi_mc);    
	     
	      HISTz_h->Fill(hit->GetZ());
	      if (hit->GetXPad() >= 1)
	        HISTxpad_x_h->Fill(hit->GetX(),hit->GetXPad()-1);
	      else
		HISTxpad_x_h->Fill(hit->GetX(),hit->GetXPad());
	      
	      if (hit->GetYPad() >= 1)
	        HISTypad_y_h->Fill(hit->GetY(),hit->GetYPad()-1);  
	      else
		HISTypad_y_h->Fill(hit->GetY(),hit->GetYPad()); 
	      
	      if (hit->GetYPad() >= 1)
	        HISTrow_x_h->Fill(hit->GetX(),hit->GetYPad()-1);
	      else
		HISTrow_x_h->Fill(hit->GetX(),hit->GetYPad());
	      
	      id_h = hit->GetDetectorID();    
	      crystal_hid = -(id_h%10000 -36);     //crystal column number (the same as GetXPad())
	      row_hid = (id_h/1000000)%100 - 37;   //crystal row number (the same as GetYPad())

	      if (hit->GetXPad() <= -1 && hit->GetYPad() <= -1)
	        HISTrow_crystal_h->Fill(hit->GetXPad(),hit->GetYPad());
	      else if (hit->GetXPad() <= -1 && hit->GetYPad() >= 1)
		HISTrow_crystal_h->Fill(hit->GetXPad(),hit->GetYPad()-1);
	      else if (hit->GetXPad() >= 1 && hit->GetYPad() <= -1)
		HISTrow_crystal_h->Fill(hit->GetXPad()-1,hit->GetYPad());
	      else //if (hit->GetXPad() >= 1 && hit->GetYPad() >= 1)
		HISTrow_crystal_h->Fill(hit->GetXPad()-1,hit->GetYPad()-1);
		
	      HISTy_x_h->Fill(hit->GetX(),hit->GetY());  
	    }
	  }
	HISThit_ene->Fill(sum_hit_ene);     // deposited energy per event in all hits (whole FwEndCap)
	if (efficiencyFlag==1)
          efficiencyCounter+=1;
	if (atLeastOneHitPerEventFlag==1)
	  atLeastOneHitPerEventCounter+=1;
	
	if (j%1000 ==0)
	  cout<<"Evt. No. "<< j<<endl;
   }
   cout<<"totalhits = "<<totalhits<<"\n";
   cout<<"efficiencyCounter = "<<efficiencyCounter<<" , atLeastOneHitPerEventCounter = "<<atLeastOneHitPerEventCounter<<"\n";
   cout<<"FwEndCap efficiency (more than 10 MeV deposition in at least one crystal) = "<< (double)efficiencyCounter/atLeastOneHitPerEventCounter<<"\n";
	
////////////////////////////////////////////////////////////////////////// DRAWING
  gStyle->SetPalette(1);
 /* TCanvas* c1 = new TCanvas("c1","", 10, 20, 1200, 900);
  //c1->SetFillColor(10);
  c1->Divide(4,2);
  c1->cd(1);
  hypad_xpad_p->Draw("colz");
  hypad_xpad_p->GetXaxis()->SetTitle("column number");
  hypad_xpad_p->GetYaxis()->SetTitle("row number");

  c1->cd(2);
  hx_y_p->Draw("colz");
  hx_y_p->GetXaxis()->SetTitle("X position");
  hx_y_p->GetYaxis()->SetTitle("Y position");
  
    
    //TGraph2D* gr3D = new TGraph2D(totalpoints,x3,y3,z3);
    //gr3D->Draw("P");
    //gr3D->SetMarkerColor(2);
    //gr3D->SetMarkerStyle(8);
    //gr3D->SetMarkerSize(.2);
    
  
  c1->cd(3);
  h10->SetLineColor(4);  // theta MC (POINTS)
  h10->Draw();
  h10d->SetLineColor(1); // theta DATA (POINTS)
  h10d->Draw("same");
  h10->GetXaxis()->SetTitle("#theta (deg)");

  c1->cd(4);
  h20->SetLineColor(4);  // phi MC (POINTS)
  h20->Draw();
  h20d->SetLineColor(1); //phi DATA (POINTS)
  h20d->Draw("same");
  h20->GetXaxis()->SetTitle("#phi (deg)");

  c1->cd(5); 
  hxpad_x_p->Draw("colz");
  hxpad_x_p->GetXaxis()->SetTitle("crystal column number");
  hxpad_x_p->GetYaxis()->SetTitle("x_point");

  c1->cd(6);
  hypad_y_p->Draw("colz");
  hypad_y_p->GetXaxis()->SetTitle("crystal row number");
  hypad_y_p->GetYaxis()->SetTitle("y_point");

  c1->cd(7);
  h1->Draw(); // point_theta-theta_monte_carlo
  h1->GetXaxis()->SetTitle("#theta_{POINT}-#theta_{MC}");
  h1->GetYaxis()->SetTitle("Counts");

  c1->cd(8);
  h2->Draw(); // point_phi-phi_monte_carlo
  h2->GetXaxis()->SetTitle("#phi_{POINT}-#phi_{MC}");
  h2->GetYaxis()->SetTitle("Counts");
*/ 

/*
  TCanvas* c2 = new TCanvas("c2","", 100, 100, 800, 800);
  c2->Divide(3,2);  
  c2->cd(1);
  hpoi_ene11_12->Draw();
  c2->cd(2);
  hpoi_ene12_12->Draw();
  c2->cd(3);
  hpoi_ene13_12->Draw();  
  c2->cd(4);
  hpoi_ene14_12->Draw();
  c2->cd(6);
  hpoi_ene->SetLineColor(1);
  hpoi_ene->Draw();
  hpoi_ene->GetXaxis()->SetTitle("FwEndCap Deposited Energy (point analysis)");
  */

  
 /* TCanvas* c3 = new TCanvas("c3","", 100, 100, 800, 800);//x_point vs z_point for the critical columns (those proceeding after the inter-subunit gaps)
  c3->SetFillColor(10);
  c3->SetFrameFillColor(10); 
  c3->Divide(3,3);  
  c3->cd(1);
  gr = new TGraph(totalpoints,x,xx);
  gr->Draw("AP");
  gr->SetMarkerColor(2); 
  c3->cd(2);
  gr2 = new TGraph(totalpoints,z3,xx);
  gr2->Draw("AP");
  c3->cd(3);
  gr2 = new TGraph(totalpoints,z7,xx);
  gr2->Draw("AP");
  c3->cd(4);
  gr2 = new TGraph(totalpoints,z11,xx);
  gr2->Draw("AP");
  c3->cd(5);
  gr2 = new TGraph(totalpoints,z15,xx);
  gr2->Draw("AP");
  c3->cd(6);
  gr2 = new TGraph(totalpoints,z19,xx);
  gr2->Draw("AP");
  c3->cd(7);
  gr2 = new TGraph(totalpoints,z23,xx);
  gr2->Draw("AP");
  c3->cd(8);
  gr2 = new TGraph(totalpoints,z27,xx);
  gr2->Draw("AP");
  c3->cd(9);
  gr2 = new TGraph(totalpoints,z31,xx);
  gr2->Draw("AP");
 */ 
  
 /* 
  TCanvas* c4 = new TCanvas("c4","", 100, 100, 800, 800);//y_point vs z_point for the critical rows (those proceeding above the inter-subunit gaps)
  c4->SetFillColor(10);
  c4->SetFrameFillColor(10);   
  c4->Divide(3,3);  
  c4->cd(1);
  gr = new TGraph(totalpoints,y,yy);
  gr->Draw("AP");
  gr->SetMarkerColor(2); 
  c4->cd(2);
  gr2 = new TGraph(totalpoints,z2,yy);
  gr2->Draw("AP");
  c4->cd(3);
  gr2 = new TGraph(totalpoints,z6,yy);
  gr2->Draw("AP");
  c4->cd(4);
  gr2 = new TGraph(totalpoints,z10,yy);
  gr2->Draw("AP");
  c4->cd(5);
  gr2 = new TGraph(totalpoints,z14,yy);
  gr2->Draw("AP");
  c4->cd(6);
  gr2 = new TGraph(totalpoints,z18,yy);
  gr2->Draw("AP");
  c4->cd(7);
  gr2 = new TGraph(totalpoints,z22,yy);
  gr2->Draw("AP");
  c4->cd(8);
  gr2 = new TGraph(totalpoints,z26,yy);
  gr2->Draw("AP");
  c4->cd(9);
  gr2 = new TGraph(totalpoints,z30,yy);
  gr2->Draw("AP");
  */ 
   
 /*
  TCanvas* c5 = new TCanvas("c5","", 100, 100, 800, 800);
  c5->Divide(3,2);  
  c5->cd(1);
  HISTOhit_ene11_12->Draw();
  c5->cd(2);
  HISTOhit_ene12_12->Draw();
  c5->cd(3);
  HISTOhit_ene13_12->Draw();  
  c5->cd(4);
  HISTOhit_ene14_12->Draw();    
  c5->cd(5);
  HISTene_h->Draw();
  HISTene_h->GetXaxis()->SetTitle("FwEndCap Deposited Energy per hit (hit analysis)");   
  c5->cd(6);
  HISThit_ene->SetLineColor(1);
  HISThit_ene->Draw();
  HISThit_ene->GetXaxis()->SetTitle("FwEndCap Deposited Energy per event (hit analysis)");   
  */
 
 /*
  TCanvas* c6 = new TCanvas("c6","", 100, 100, 800, 800);
  c6->Divide(3,2);  
  c6->cd(1);
   HISTy_x_h->Draw("colz");
  c6->cd(2);
   HISTrow_crystal_h->Draw("colz");
  c6->cd(3);
   HISTxpad_x_h->Draw("colz"); 
  c6->cd(4);
   HISTypad_y_h->Draw("colz");
  c6->cd(5);
   HISTrow_x_h->Draw("colz");
*/
 
 TCanvas* c7 = new TCanvas("c7","", 100, 100, 1000, 700);
 c7->SetFillColor(10);
 gStyle->SetOptStat(0);
 c7->Divide(2,1);
 c7->cd(1)->SetRightMargin(0.02);
 HISTxpad_x_h->Draw("box");
 HISTxpad_x_h->GetYaxis()->SetTitle("crystal column number");
 HISTxpad_x_h->GetYaxis()->SetTitleSize(0.05);
 HISTxpad_x_h->GetYaxis()->CenterTitle();
 HISTxpad_x_h->GetYaxis()->SetTitleOffset(1.);
 HISTxpad_x_h->GetXaxis()->SetTitle("x [cm]");
 HISTxpad_x_h->GetXaxis()->SetTitleSize(0.07);
 HISTxpad_x_h->GetXaxis()->SetTitleOffset(0.6);
 HISTxpad_x_h->GetXaxis()->CenterTitle();
 c7->cd(2)->SetRightMargin(0.02);
 HISTypad_y_h->Draw("box");
 HISTypad_y_h->GetYaxis()->SetTitle("crystal row number");
 HISTypad_y_h->GetYaxis()->SetTitleSize(0.05);
 HISTypad_y_h->GetYaxis()->CenterTitle();
 HISTypad_y_h->GetYaxis()->SetTitleOffset(1.);
 HISTypad_y_h->GetXaxis()->SetTitle("y [cm]");
 HISTypad_y_h->GetXaxis()->SetTitleSize(0.07);
 HISTypad_y_h->GetXaxis()->SetTitleOffset(0.6);
 HISTypad_y_h->GetXaxis()->CenterTitle();
 
 //TString outfile= "./HISTxPad_x.root";
 //TFile* file_out = new TFile(outfile,"RECREATE");
 //HISTxpad_x_h->Write();
 //HISTypad_y_h->Write();
 //file_out->Close();
 
}
