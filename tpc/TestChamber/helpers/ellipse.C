{


  double x[2]={-1.,1.};
  double y[2]={-1.,1.};

  TGraph *g = new TGraph(2,x,y);

  TCanvas c("c2","",600,600);

  g->Draw("A*");

  TEllipse *e = new TEllipse(0.,0.,0.7,0.5,0.,360.,45.);
  e->Draw();



}
