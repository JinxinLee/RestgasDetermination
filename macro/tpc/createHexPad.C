{
double a=0.15; // Hexagon radius
double l=0.01; // margin around pad
a-=l;

TPolyLine* line=new TPolyLine(7);

std::cout<<"p 0 ";
for(int i=0; i<7; ++i){
  double al=(double)i*TMath::Pi()/3.;
  double x=a*TMath::Cos(al);
  double y=a*TMath::Sin(al);
  if(fabs(x)<1E-4)x=0;
  if(fabs(y)<1E-4)y=0;

  std::cout<<x<<" "<<y<<" ";
  line->SetPoint(i,x,y);
}
std::cout.flush();

TCanvas* c=new TCanvas("c","c",10,10,300,300);
TH2D* h=new TH2D("h","h",1,-0.3,0.3,1,-0.3,0.3);
h->Draw();
line->SetLineColor(kRed);
line->Draw();
}
