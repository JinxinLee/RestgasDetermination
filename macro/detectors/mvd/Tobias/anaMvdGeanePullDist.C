{
gROOT->LoadMacro("Tools.C");
LoadPandaStyle();
TFile f("./data/Mvd_FlatG4Special_0G5_70k_Geane.root");
TTree* t = (TTree*)f->Get("pndsim");
//gStyle->SetPalette(1);

TH1D hpdX("hpdX","Pull distribution in x", 1000, -5, 5);
TH1D hpdY("hpdY","Pull distribution in y", 1000, -5, 5);
TH1D hpdZ("hpdZ","Pull distribution in z", 1000, -5, 5);
TH1D hdiffX("hdiffX","Diff in x", 1000, -0.5, 0.5);
TH1D hdiffY("hdiffY","Diff in y", 1000, -0.5, 0.5);
TH1D hdiffZ("hdiffZ","Diff in z", 1000, -0.5, 0.5);


TCanvas *myCan = new TCanvas("BarrelStructures");


myCan->Divide(3,2);

myCan->cd(1);
t->Draw("(GeaneTrackFinal.fX-GeaneTrackPar.fX)/GeaneTrackPar.fDX >>hpdX","","");
myCan->cd(2);
t->Draw("(GeaneTrackFinal.fY-GeaneTrackPar.fY)/GeaneTrackPar.fDY >>hpdY","","");
myCan->cd(3);
t->Draw("(GeaneTrackFinal.fZ-GeaneTrackPar.fZ)/GeaneTrackPar.fDZ >>hpdZ","","");
myCan->cd(4);
t->Draw("(GeaneTrackFinal.fX-GeaneTrackPar.fX) >>hdiffX","","");
myCan->cd(5);
t->Draw("(GeaneTrackFinal.fY-GeaneTrackPar.fY) >>hdiffY","","");
myCan->cd(6);
t->Draw("(GeaneTrackFinal.fZ-GeaneTrackPar.fZ) >>hdiffZ","","");

}
