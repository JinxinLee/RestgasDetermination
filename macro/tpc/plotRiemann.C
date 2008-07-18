{

//gROOT->ProcessLine(".x tpc/TestChamber/macro/christian_style.C");


TCanvas* c1=new TCanvas("Riemann","Riemann",10,10,600,600);
TCanvas* c2=new TCanvas("Riemann2","Riemann2",20,20,600,600);
TFile* infile=new TFile("RiemannFinder.root");
if(infile==NULL){std::cout<<"File not found"<<std::endl;return;}
infile->Print();

c1->cd();
TF3* plane=new TF3("plane","x*[0]+y*[1]+z*[2]+[3]",-1,1,-1,1,0,1.2);
plane->SetNpz(20);
plane->SetTitle("Planes");

TView3D* view=new TView3D(1,0,0);
view->SetRange(-1,-1,0,1,1,2);

for(int i=0;i<100;++i){
  char name[200];
  sprintf(name,"plane%i",i);
  TLorentzVector* par=(TLorentzVector*)infile->Get(name);
  if(par==NULL)continue;
  std::cout<<"Plane "<<i<<std::endl;
  TVector3 p(par->X(),par->Y(),par->Z());
  p.Print();
  p.SetMag(1);
  TPolyLine3D* normal=new TPolyLine3D(2);
  normal->SetPoint(0,0,0,0);
  normal->SetPoint(1,p.X(),p.Y(),p.Z());
  normal->SetLineColor(kMagenta+i);
  normal->Draw();
  p.Print();
  plane->SetParameters(par->X(),par->Y(),par->Z(),par->T());
  plane->SetLineColor(kMagenta+i+100);
  //if(i==1)plane->DrawClone();
  //else plane->DrawClone("same"); 
}

std::cout<<"Drawn planes"<<std::endl;


//TF3 sphere("sphere","sqrt(x*x+y*y+(z-0.5)*(z-0.5))-0.499",-1,1,-1,1,0,1.2);
//sphere.SetFillStyle(0);
//sphere.SetLineColor();
//sphere.SetNpz(30);
//sphere.SetTitle("Riemann Sphere");
//sphere.SetLineColor(17);
//sphere.Draw("same");

c2->cd();
//sphere.DrawClone();

//TCanvas* c=new TCanvas("Tracks","Tracks",10,10,600,600);

//return;
for(int i=0;i<100;++i){
  char name[200];
  sprintf(name,"track%i",i);
  TPolyMarker3D* marker=(TPolyMarker3D*)infile->Get(name);
  if(marker==NULL){std::cout<<"Marker not found"<<std::endl;continue;}
  sprintf(name,"clusters%i",i);
  TPolyMarker3D* clust=(TPolyMarker3D*)infile->Get(name);
  if(clust==NULL){std::cout<<"Clust not found"<<std::endl;continue;}
  
  marker->SetMarkerColor(kMagenta+i);marker->SetMarkerStyle(20);
  marker->SetMarkerSize(0.75);
  clust->SetMarkerColor(1);clust->SetMarkerStyle(5);clust->SetMarkerSize(0.5);
  
  //if(i==0)marker->Draw();
  c1->cd();
  marker->Draw("same");
  //clust->Draw("same");

  c2->cd();
  //marker->DrawClone("same");
  clust->DrawClone("same");

  //c->cd();
  //clust->DrawClone("AP");


}
//delete c;
//c1->SaveAs("riemann2.eps");
//c2->SaveAs("riemann1.eps");

}

