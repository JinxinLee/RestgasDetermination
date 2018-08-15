{ TChain *mychain = new TChain("pndsim");


  mychain->Add("run.root");
  mychain->Add("run_1.root");
  mychain->Add("run_2.root");


  gStyle->SetPalette(1);


TCut kaons     = "HypPoint.fpdgCode==-321 || HypPoint.fpdgCode == 321";
TCut pions     = "HypPoint.fpdgCode==-211 || HypPoint.fpdgCode == 211";
TCut protons   = "HypPoint.fpdgCode==2212 || HypPoint.fpdgCode == -2212";
TCut lambdas   = "HypPoint.fpdgCode==3122"  ;
TCut electrons = "HypPoint.fpdgCode==11   || HypPoint.fpdgCode == -11";
TCut ximin = "HypPoint.fpdgCode==3312";

TCut ximstop = "HypPoint.fpdgCode==3312 && HypPoint.fPLout==0.";
TCut Secstop = "HypSecTarPoint.fPLout==0.";




Double_t mom = "TMath::Sqrt(HypPoint.fPLout*HypPoint.fPLout)";
Double_t energy = "TMath::Sqrt(mom*mom + HypPoint.fmass*HypPoint.fmass)";

 //mychain->Draw("HypSegTarPoint.fdist:HypSegTarPoint.fZin>>histo(00,-77,-72,200,0.5,5)");
c1 = new TCanvas("stopping xi- rate momentum distribution");
//mychain->SetLineColor(4);
 mychain->Draw("TMath::Sqrt(HypPoint.fPxin*HypPoint.fPxin+HypPoint.fPyin*HypPoint.fPyin+HypPoint.fPzin*HypPoint.fPzin)>>his(200,-0.4,1.4)", ximin );

 cout<<"integral of Xi- entering"<< his->Integral()<<endl;
 //mychain->SetLineColor(2);
 mychain->Draw("TMath::Sqrt(HypSecTarPoint.fPxin*HypSecTarPoint.fPxin+HypSecTarPoint.fPyin*HypSecTarPoint.fPyin+HypSecTarPoint.fPzin*HypSecTarPoint.fPzin)>>hist(200,-0.4,1.4)","","same" );
 cout<<"integral of Xi- stopped"<< hist->Integral()<<endl;
 //hist->Scale(10);



}
