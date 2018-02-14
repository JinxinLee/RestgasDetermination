{

  // === CHOOSE ==========================
  int ifile = 4;

  double mean_inf = 0.39;
  double mean_sup = 1.23;
  double sigma_inf = 0.39;
  double sigma_sup = 1.23;

  // y upper limit
  Double_t yup = 30.;

  // steps for gaussians (e canvas)
  double nsteps = 50;
  int cx = 5;
  int cy = 10;
  double limit = 100/nsteps;
  double flimit = floor(100/nsteps);
  if( limit != flimit){
    nsteps = 100/flimit;
    limit = flimit;
    cout << "changing nsteps to " << nsteps << endl;
  }

  TString fundedx = "[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]";


  // TString fundedx = "pol6(0)";
  int npardedx = 3;

//   TString funsig = "pol4(0)";
//   TString funsig_forsum = "pol4(3)";
//   int nparsig = 5;

  TString funsig = "[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]";
  TString funsig_forsum = "([3] * (1./x)**2 + [4] * TMath::Log(x) + [5])";
  int nparsig = 3;


  int npartot = npardedx + nparsig;
  // ====================================

  TCanvas *c = new TCanvas("c", "dedx_p");

  TFile infile("dedx_out.root", "READ");

  TH2F *h;
  switch(ifile) {
  case 0: h = (TH2F*) infile.Get("hdedx_p_e"); break;
  case 1: h = (TH2F*) infile.Get("hdedx_p_mu"); break;
  case 2: h = (TH2F*) infile.Get("hdedx_p_pi"); break;
  case 3: h = (TH2F*) infile.Get("hdedx_p_k"); break;
  case 4: h = (TH2F*) infile.Get("hdedx_p_p");
  }
  c->cd();  h->Draw();

  // procedure -------------------------------------------
 
 
  TH1D *htemp[nsteps];
  TF1 *fgaus[nsteps];
  Double_t x[nsteps]; 
  // array of mean/sigma
  Double_t mean[nsteps], sigma[nsteps], meanerr[nsteps], sigmaerr[nsteps];

  // gauss
  TCanvas *c2 = new TCanvas("c2", "slices");
  c2->Divide(cx, cy);
 
  for(int i = 0; i < nsteps; i++) {
    double stepsize = 1.5/nsteps;
    x[i] = (i + 0.5) * stepsize;
    
    TString name = "htemp"; name += i;
    TString title = "";
    TString fname = name; 
    TString fgname = name; 
    switch(ifile) {
    case 0: fname.Prepend("e_");  fgname.Prepend("fe_"); break;
    case 1: fname.Prepend("mu_"); fgname.Prepend("fmu_"); break;
    case 2: fname.Prepend("pi_"); fgname.Prepend("fpi_"); break;
    case 3: fname.Prepend("k_");  fgname.Prepend("fk_"); break;
    case 4: fname.Prepend("p_");  fgname.Prepend("fp_"); 
    }
    title = fname; title += "("; title += (x[i] - 0.5 * stepsize); title += ", "; title += (x[i] + 0.5 * stepsize); title += ") GeV"; title += ((i + 1) * limit - 1);

    htemp[i] = new TH1D(fname, title, 100, 0, yup);
    h->ProjectionY(fname, limit * i, (i + 1) * limit - 1,"o"); 
    c2->cd(i + 1); 
    htemp[i].Draw();


    double tmpmu = htemp[i]->GetMean() ;
    double tmpsig = htemp[i]->GetRMS();
    double gaus_inf = tmpmu - 3 * tmpsig;
    double gaus_sup = tmpmu + 3 * tmpsig;

    fgaus[i] = new TF1("fgname", "gaus(0)", gaus_inf, gaus_sup);
    fgaus[i]->SetParameters(1, tmpmu, tmpsig);
    htemp[i]->Fit("fgname", "R"); 
    mean[i] = fgaus[i]->GetParameter(1);
    sigma[i] = fgaus[i]->GetParameter(2);
    meanerr[i] = fgaus[i]->GetParError(1);
    sigmaerr[i] = fgaus[i]->GetParError(2);
}

  // GRAPHS ========================================================
  TCanvas *graphs = new TCanvas("graphs", "graphs");
  graphs->Divide(2, 1);
  
  Double_t masses[5] = {0.000510999,  0.1056584,  0.139570,  0.49368,  0.9382723 };

  Double_t xerr[nsteps];
  for(int istep = 0; istep < nsteps; istep++) xerr[istep] = 0;
  // ================= MEAN =================
  TGraphErrors *g = new TGraphErrors(nsteps, x, mean, xerr, meanerr);
  TF1 *fdedx = new TF1("fdedx", fundedx,  mean_inf, mean_sup);
  fdedx->SetParameters(1, 1, 1);
  graphs->cd(1); 
  g->Draw("A*P"); 
  g->GetYaxis()->SetRangeUser(0, 100);
  g->Fit("fdedx", "R");
  Double_t par[npardedx]; fdedx->GetParameters(&par[0]);
  // =========================================

  // ================= SIGMA =================
  for(int i = 0; i < nsteps; i++) sigma[i]  = fabs(sigma[i]); 
  TGraphErrors *gs = new TGraphErrors(nsteps, x, sigma, xerr, sigmaerr);
  TF1 *sfdedx = new TF1("sfdedx", funsig, sigma_inf, sigma_sup);
  graphs->cd(2); 
  gs->Draw("A*P"); 
  gs->GetYaxis()->SetRangeUser(0, 5);
  gs->Fit("sfdedx", "R");
  Double_t spar[nparsig]; sfdedx->GetParameters(&spar[0]);
  // =========================================

  // ==== DRAW ALL ON SCATTER PLOT ===========
  TF1 *fdedx2 = new TF1("fdedx2", fundedx, 0., 1.5);
  fdedx2->SetParameters(par);
  c->cd();  fdedx2->SetLineColor(kOrange);  fdedx2->Draw("SAME");

  double parcomp[npartot];
  for(int i = 0; i < npartot; i++) {
    if(i < npardedx)   parcomp[i] = par[i];
    else parcomp[i] = spar[i - npardedx];
  }
  TString funsum = fundedx + " + " + funsig_forsum;
  TF1 *fsum = new TF1("fsum", funsum, 0., 1.5);
  fsum->SetParameters(parcomp);
  TString fundiff = fundedx +  " - " + funsig_forsum; 
  TF1 *fdiff = new TF1("fdiff", fundiff, 0., 1.5);
  fdiff->SetParameters(parcomp);
  c->cd();
  fsum->Draw("SAME");
  fdiff->Draw("SAME");
  // =========================================
  
  // ==== PRINT OUTS =========================
  cout << "ifile " << ifile << endl;
  cout << " momentum sampled over " << nsteps << " with step width " << 1.5/nsteps << endl;
  cout << endl;
  cout << "MEAN DEDX PARAMETRIZATION" << endl;
  cout << "mom limits " << mean_inf << " " << mean_sup << endl;
  cout << "mu: "; for(int param = 0; param < npardedx; param++) cout << fdedx->GetParameter(param) << ", "; cout << endl;
  cout << endl;
  cout << "SIGMA DEDX PARAMETRIZATION" << endl;
  cout << "mom limits " << sigma_inf << " " << sigma_sup << endl;
  cout << "sig: "; for(int param = 0; param < nparsig; param++) cout << sfdedx->GetParameter(param) << ", " ; cout << endl;
 // =========================================


}

