{
TFile* infile=new TFile("Riemann.root");
TCanvas *c1 = new TCanvas("c1","Riemann Sphere",0,0,600,600);
TView *view = new TView(1);
view->SetRange(-0.6,-0.6,-0.1,0.6,0.6,1.1);
view->ShowAxis();

rieTrack->Draw();
arrow->Draw();
}
