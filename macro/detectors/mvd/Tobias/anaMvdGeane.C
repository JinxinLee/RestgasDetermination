{
gROOT->LoadMacro("Tools.C");
LoadPandaStyle();
TFile f("./data/Mvd_FlatG4Special_0G5_70k_Geane.root");
TTree* t = (TTree*)f->Get("cbmsim");
//gStyle->SetPalette(1);
TProfile2D pSdk1("pSdk1","MS on Sdk1", 100, -15,15, 100, -15,15);
TProfile2D pSdk2("pSdk2","MS on Sdk2", 100, -15,15, 100, -15,15);
TProfile2D pMdk1("pMdk1","MS on Mdk1", 100, -15,15, 100, -15,15);
TProfile2D pMdk2("pMdk2","MS on Mdk2", 100, -15,15, 100, -15,15);
TProfile2D pMdk3("pMdk3","MS on Mdk3", 100, -15,15, 100, -15,15);
TProfile2D pMdk4("pMdk4","MS on Mdk4", 100, -15,15, 100, -15,15);
TProfile2D pLdk1("pLdk1","MS on Ldk1", 100, -15,15, 100, -15,15);
TProfile2D pLdk2("pLdk2","MS on Ldk2", 100, -15,15, 100, -15,15);
TProfile2D pBl1("pBl1","MS on Bl1", 100, -1,1, 50, -TMath::Pi(),TMath::Pi());
TProfile2D pBl2("pBl2","MS on Bl2", 100, -1,1, 50, -TMath::Pi(),TMath::Pi());
TProfile2D pBl3("pBl3","MS on Bl3", 100, -1,1, 50, -TMath::Pi(),TMath::Pi());
TProfile2D pBl4("pBl4","MS on Bl4", 100, -1,1, 50, -TMath::Pi(),TMath::Pi());

TH1D hSdk1("hSdk1","MS on Sdk1", 1000, 0, 0.5);
TH1D hSdk2("hSdk2","MS on Sdk2", 1000, 0, 0.5);
TH1D hMdk1("hMdk1","MS on Mdk1", 1000, 0, 0.5);
TH1D hMdk2("hMdk2","MS on Mdk2", 1000, 0, 0.5);
TH1D hMdk3("hMdk3","MS on Mdk3", 1000, 0, 0.5);
TH1D hMdk4("hMdk4","MS on Mdk4", 1000, 0, 0.5);
TH1D hLdk1("hLdk1","MS on Ldk1", 1000, 0, 0.5);
TH1D hLdk2("hLdk2","MS on Ldk2", 1000, 0, 0.5);

TH1D hBl1("hBl1","MS on Bl1", 1000, 0, 0.5);
TH1D hBl2("hBl2","MS on Bl2", 1000, 0, 0.5);
TH1D hBl3("hBl3","MS on Bl3", 1000, 0, 0.5);
TH1D hBl4("hBl4","MS on Bl4", 1000, 0, 0.5);


TCanvas *myCan = new TCanvas("BarrelStructures");
TCanvas *myCan2 = new TCanvas("DiskStructures");
TCanvas *myCan3 = new TCanvas("BarrelStructures1D");
TCanvas* myCan4 = new TCanvas("DiskStructures1D");

myCan->Divide(2,2);

for (int i = 0; i < 4; i++){
	myCan->GetPad(i+1)->SetRightMargin(0.15);
}

myCan->cd(1);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ) : TMath::ATan2(GeaneTrackPar.fX, GeaneTrackPar.fY): (GeaneTrackPar.fZ/TMath::Sqrt(GeaneTrackPar.fX*GeaneTrackPar.fX + GeaneTrackPar.fY*GeaneTrackPar.fY + GeaneTrackPar.fZ*GeaneTrackPar.fZ)) >>pBl1","DetName.fString.Contains(\"Bl1\")","colz");
myCan->cd(2);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ) : TMath::ATan2(GeaneTrackPar.fX, GeaneTrackPar.fY): (GeaneTrackPar.fZ/TMath::Sqrt(GeaneTrackPar.fX*GeaneTrackPar.fX + GeaneTrackPar.fY*GeaneTrackPar.fY + GeaneTrackPar.fZ*GeaneTrackPar.fZ)) >>pBl2","DetName.fString.Contains(\"Bl2\")","colz");
myCan->cd(3);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ) : TMath::ATan2(GeaneTrackPar.fX, GeaneTrackPar.fY): (GeaneTrackPar.fZ/TMath::Sqrt(GeaneTrackPar.fX*GeaneTrackPar.fX + GeaneTrackPar.fY*GeaneTrackPar.fY + GeaneTrackPar.fZ*GeaneTrackPar.fZ)) >>pBl3","DetName.fString.Contains(\"Bl3\")","colz");
myCan->cd(4);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ) : TMath::ATan2(GeaneTrackPar.fX, GeaneTrackPar.fY): (GeaneTrackPar.fZ/TMath::Sqrt(GeaneTrackPar.fX*GeaneTrackPar.fX + GeaneTrackPar.fY*GeaneTrackPar.fY + GeaneTrackPar.fZ*GeaneTrackPar.fZ)) >>pBl4","DetName.fString.Contains(\"Bl4\")","colz");

myCan2->Divide(4,2);

for (int i = 0; i < 8; i++){
	myCan2->GetPad(i+1)->SetRightMargin(0.15);
}

myCan2->cd(1);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ) : GeaneTrackPar.fY : GeaneTrackPar.fX >>pSdk1","DetName.fString.Contains(\"Sdk-v2-NEW_1\")","colz");
myCan2->cd(2);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ) : GeaneTrackPar.fY : GeaneTrackPar.fX >>pSdk2","DetName.fString.Contains(\"Sdk-v2-NEW_2\")","colz");
myCan2->cd(3);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ) : GeaneTrackPar.fY : GeaneTrackPar.fX >>pMdk1","DetName.fString.Contains(\"Mdkov1-NEW_1\")","colz");
myCan2->cd(4);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ) : GeaneTrackPar.fY : GeaneTrackPar.fX >>pMdk2","DetName.fString.Contains(\"Mdkov1-NEW_2\")","colz");
myCan2->cd(5);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ) : GeaneTrackPar.fY : GeaneTrackPar.fX >>pMdk3","DetName.fString.Contains(\"Mdkov1-NEW_3\")","colz");
myCan2->cd(6);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ) : GeaneTrackPar.fY : GeaneTrackPar.fX >>pMdk4","DetName.fString.Contains(\"Mdkov1-NEW_3\")","colz");
myCan2->cd(7);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ) : GeaneTrackPar.fY : GeaneTrackPar.fX >>pLdk1","DetName.fString.Contains(\"Ldk_1\")","colz");
myCan2->cd(8);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ) : GeaneTrackPar.fY : GeaneTrackPar.fX >>pLdk2","DetName.fString.Contains(\"Ldk_2\")","colz");

myCan3->Divide(2,2);
myCan3->cd(1);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ)>>hBl1","DetName.fString.Contains(\"Bl1\")","");
myCan3->cd(2);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ)>>hBl2","DetName.fString.Contains(\"Bl2\")","");
myCan3->cd(3);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ)>>hBl3","DetName.fString.Contains(\"Bl3\")","");
myCan3->cd(4);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ)>>hBl4","DetName.fString.Contains(\"Bl4\")","");

myCan4->Divide(4,2);
myCan4->cd(1);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ)>>hSdk1","DetName.fString.Contains(\"Sdk-v2-NEW_1\")","");
myCan4->cd(2);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ)>>hSdk2","DetName.fString.Contains(\"Sdk-v2-NEW_2\")","");
myCan4->cd(3);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ)>>hMdk1","DetName.fString.Contains(\"Mdkov1-NEW_1\")","");
myCan4->cd(4);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ)>>hMdk2","DetName.fString.Contains(\"Mdkov1-NEW_2\")","");
myCan4->cd(5);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ)>>hMdk3","DetName.fString.Contains(\"Mdkov1-NEW_3\")","");
myCan4->cd(6);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ)>>hMdk4","DetName.fString.Contains(\"Mdkov1-NEW_4\")","");
myCan4->cd(7);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ)>>hLdk1","DetName.fString.Contains(\"Ldk_1\")","");
myCan4->cd(8);
t->Draw("TMath::Sqrt(GeaneTrackPar.fDX*GeaneTrackPar.fDX + GeaneTrackPar.fDY*GeaneTrackPar.fDY + GeaneTrackPar.fDZ*GeaneTrackPar.fDZ)>>hLdk2","DetName.fString.Contains(\"Ldk_2\")","");


}
