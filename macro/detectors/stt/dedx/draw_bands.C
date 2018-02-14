{

   TString fundedx = "[0] * (1./x)**2 + [1] * TMath::Log(x) + [2]";

   //   TString funsig = "pol4(3)";
  TString funsig = "([3] * (1./x)**2 + [4] * TMath::Log(x) + [5])";

  // ====================================

  TCanvas *c = new TCanvas("c", "dedx_p");
  c->Divide(3, 2);
  
  TFile infile("dedx_out.root", "READ");
  
  TH2F *h1 = (TH2F*) infile.Get("hdedx_p_e"); 
  TH2F *h2 = (TH2F*) infile.Get("hdedx_p_mu");
  TH2F *h3 = (TH2F*) infile.Get("hdedx_p_pi");
  TH2F *h4 = (TH2F*) infile.Get("hdedx_p_k"); 
  TH2F *h5 = (TH2F*) infile.Get("hdedx_p_p");
  
  gStyle->SetPalette(1, 0);
  c->cd(1);  h1->Draw("colz");
  c->cd(2);  h2->Draw("colz");
  c->cd(3);  h3->Draw("colz");
  c->cd(4);  h4->Draw("colz");
  c->cd(5);  h5->Draw("colz");
  c->cd(6);
  h1->Draw();
  h2->Draw("SAME");
  h3->Draw("SAME");
  h4->Draw("SAME");
  h5->Draw("SAME");

  TString funsum = fundedx + " + " + funsig;
  TString fundiff = fundedx +  " - " + funsig; 
  
  TF1 *fdedx1 = new TF1("fdedx1", fundedx, 0., 1.5);
  fdedx1->SetParameters(-0.0051344, 0.163231, 7.30319);
  TF1 *fsum1 = new TF1("fsum1", funsum, 0., 1.5);
  fsum1->SetParameters(-0.0051344, 0.163231, 7.30319, 0.00428134, -0.00359152, 0.655438);
  TF1 *fdiff1 = new TF1("fdiff1", fundiff, 0., 1.5);
  fdiff1->SetParameters(-0.0051344, 0.163231, 7.30319, 0.00428134, -0.00359152, 0.655438);
  c->cd(1); 
  fdedx1->SetLineColor(kOrange);
  fdedx1->Draw("SAME");
  fsum1->Draw("SAME");
  fdiff1->Draw("SAME");

  TF1 *fdedx2 = new TF1("fdedx2", fundedx, 0., 1.5);
  fdedx2->SetParameters(0.0737857, 0.643465, 5.42333);
  TF1 *fsum2 = new TF1("fsum2", funsum, 0., 1.5);
  fsum2->SetParameters(0.0737857, 0.643465, 5.42333,  0.0193587, -0.0174329, 0.539616);
  TF1 *fdiff2 = new TF1("fdiff2", fundiff, 0., 1.5);
  fdiff2->SetParameters(0.0737857, 0.643465, 5.42333,  0.0193587, -0.0174329, 0.539616);
  c->cd(2); 
  fdedx2->SetLineColor(kCyan);
  fdedx2->Draw("SAME");
  fsum2->Draw("SAME");
  fdiff2->Draw("SAME");

  TF1 *fdedx3 = new TF1("fdedx3", fundedx, 0., 1.5);
  fdedx3->SetParameters(0.124558, 0.585125, 5.20178);
  TF1 *fsum3 = new TF1("fsum3", funsum, 0., 1.5);
  fsum3->SetParameters(0.124558, 0.585125, 5.20178, 0.0395417, -0.0443629, 0.502671);
  TF1 *fdiff3 = new TF1("fdiff3", fundiff, 0., 1.5);
  fdiff3->SetParameters(0.124558, 0.585125, 5.20178, 0.0395417, -0.0443629, 0.502671);
  c->cd(3); 
  fdedx3->SetLineColor(kGreen + 3);
  fdedx3->Draw("SAME");
  fsum3->Draw("SAME");
  fdiff3->Draw("SAME");
 
  TF1 *fdedx4 = new TF1("fdedx4", fundedx, 0., 1.5);
  fdedx4->SetParameters(1.71069, 2.21786, 3.82597);
  TF1 *fsum4 = new TF1("fsum4", funsum, 0., 1.5);
  fsum4->SetParameters(1.71069, 2.21786, 3.82597, 0.260654, 0.385838, 0.307763);
  TF1 *fdiff4 = new TF1("fdiff4", fundiff, 0., 1.5);
  fdiff4->SetParameters(1.71069, 2.21786, 3.82597, 0.260654, 0.385838, 0.307763);
  c->cd(4); 
  fdedx4->SetLineColor(kViolet + 2);
  fdedx4->Draw("SAME");
  fsum4->Draw("SAME");
  fdiff4->Draw("SAME");

  TF1 *fdedx5 = new TF1("fdedx5", fundedx, 0., 1.5);
  fdedx5->SetParameters(4.26561, -0.152697, 4.03156);
  TF1 *fsum5 = new TF1("fsum5", funsum, 0., 1.5);
  fsum5->SetParameters(4.26561, -0.152697, 4.03156, 0.338277, -0.664876, 0.656844);
  TF1 *fdiff5 = new TF1("fdiff5", fundiff, 0., 1.5);
  fdiff5->SetParameters(4.26561, -0.152697, 4.03156, 0.338277, -0.664876, 0.656844);

  c->cd(5); 
  fdedx5->SetLineColor(kGray);
  fdedx5->Draw("SAME");
  fsum5->SetLineColor(kYellow);
  fsum5->Draw("SAME");
  fdiff5->SetLineColor(kYellow);
  fdiff5->Draw("SAME");

  c->cd(6); 
  fdedx1->Draw("SAME");
  fdedx2->Draw("SAME");
  fdedx3->Draw("SAME");
  fdedx4->Draw("SAME");
  fdedx5->Draw("SAME");
  
}
