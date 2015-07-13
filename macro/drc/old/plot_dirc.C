{
gROOT->Reset();

TFile f("./testrun.root");
f.ls();
f.cd();





TTree* tree = (TTree*)gDirectory->Get("cbmsim");

Double_t mom = "TMath::Sqrt(fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out)";
Double_t energy = "TMath::Sqrt(mom*mom + fmass*fmass)"; 
Double_t thetaC = "TMath::ACos(1/(1.47*(mom/energy)))";

TCut pions     = "fPdgCode==-211 || fPdgCode == 211";
TCut protons   = "fPdgCode==2212 || fPdgCode == -2212";
TCut muons     = "fPdgCode==13   || fPdgCode == -13";
TCut electrons = "fPdgCode==11   || fPdgCode == -11";


TCut ppos = "TMath::Sqrt(fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out)>0.";
TCut pinf = "TMath::Sqrt(fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out)<2.7";
TCut lenti = "TMath::Sqrt(fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out)<1.";
//TCut lenti = "TMath::Sqrt(fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out)<1.";


tree->SetMarkerSize(.3);
tree->SetMarkerStyle(20);

tree->SetMarkerColor(2);

c1 = new TCanvas("dE/dx vs momentum - Drc barrel");
tree->Draw("fELoss:TMath::Sqrt(fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out)", ppos && "fELoss < 0.04");


c2 = new TCanvas("X vs Y vs Z - Drc barrel");
tree->SetMarkerColor(2);
tree->Draw("fX_out:fY_out:fZ_out", ppos && pions);

tree->SetMarkerColor(3);
tree->Draw("fX_out:fY_out:fZ_out", ppos && protons , "same");

tree->SetMarkerColor(4);
tree->Draw("fX_out:fY_out:fZ_out", ppos && electrons , "same");


c3 = new TCanvas("momentum");
c3->Divide(2, 2);

c3->cd(1);
tree->SetMarkerColor(2);
tree->Draw("TMath::Sqrt(fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out)", pions  && ppos && pinf); // pions

c3->cd(2);
tree->SetMarkerColor(3);
tree->Draw("TMath::Sqrt(fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out)", muons && ppos && pinf); // muons

c3->cd(3);
tree->SetMarkerColor(4);
tree->Draw("TMath::Sqrt(fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out)", electrons && ppos && pinf); // electrons

c3->cd(4);
tree->SetMarkerColor(5);
tree->Draw("TMath::Sqrt(fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out)", protons &&  ppos && pinf); // protons



c4 = new TCanvas("Eloss - Drc barrel");
tree->Draw("fELoss", ppos && pinf && "fELoss < 0.04" && "fELoss > 0.005" ); 


c5 = new TCanvas("ThetaC vs momentum - Drc barrel");
tree->SetMarkerColor(2);
tree->Draw("TMath::ACos(1/(1.47*(TMath::Sqrt(fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out)/TMath::Sqrt((fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out) + fmass*fmass)))):TMath::Sqrt(fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out)", ppos && pions && "fELoss < 0.04"); //pions

tree->SetMarkerColor(3);
tree->Draw("TMath::ACos(1/(1.47*(TMath::Sqrt(fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out)/TMath::Sqrt((fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out) + fmass*fmass)))):TMath::Sqrt(fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out)", electrons && ppos &&"fELoss < 0.04", "same"); //electrons

tree->SetMarkerColor(1);
tree->Draw("TMath::ACos(1/(1.47*(TMath::Sqrt(fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out)/TMath::Sqrt((fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out) + fmass*fmass)))):TMath::Sqrt(fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out)", muons && ppos &&"fELoss < 0.04", "same"); //muons

tree->SetMarkerColor(4);
tree->Draw("TMath::ACos(1/(1.47*(TMath::Sqrt(fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out)/TMath::Sqrt((fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out) + fmass*fmass)))):TMath::Sqrt(fPx_out*fPx_out+ fPy_out*fPy_out+ fPz_out*fPz_out)", protons && ppos &&"fELoss < 0.04", "same"); //protons

/*
// treat PDGCode
TH1F *h1 = new TH1F("h1","pdg",90000000, -10000000, 80000000);

c6 = new TCanvas("PDG Code - Drc barrel");
tree->Draw("fPdgCode>>h1");
Int_t n;
Int_t b;
for(Int_t i = 0; i < 90000000; i=i+1){

  if (h1->GetBinContent(i)!=0)
    {
      n = h1->GetBinContent(i);
      b = i -10000001;
      cout << "PDGId = " << b << "->  entries:" << n << endl;
    }
}
*/
}

