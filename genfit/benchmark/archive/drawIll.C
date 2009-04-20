{
gROOT->SetStyle("Plain");



double phi=126.6;
double theta=22.1;

Lfit->SetLineColor(kGreen);
Lst->SetLineColor(kRed);
Lst->SetLineStyle(3);
LfitBehind->SetLineColor(kGreen);
LstBehind->SetLineColor(kRed);
LstBehind->SetLineStyle(3);

//Mtr->SetMarkerStyle(24);
//Mtr->SetMarkerSize(0.4);
//Mtr->SetMarkerColor(kGreen);
Msm->SetMarkerStyle(24);
Msm->SetMarkerSize(0.6);
Msm->SetMarkerColor(kRed);
Mpl->SetMarkerStyle(20);
Mpl->SetMarkerSize(1.7);
//Mpl->SetMarkerColor(17);

/*
TCanvas c1("c1","c1",600,600);
c1.SetPhi(phi);
c1.SetTheta(theta);

Ltr->Draw();





TCanvas c2("c2","c2",600,600);
c2.SetPhi(phi);
c2.SetTheta(theta);

Mtr->Draw();

TCanvas c3("c3","c3",600,600);
c3.SetPhi(phi);
c3.SetTheta(theta);
Msm->Draw();
Ltr->Draw("same");

*/

TCanvas c4("c4","c4",600,600);
c4.SetPhi(phi);
c4.SetTheta(theta);




//LtrBehind->Draw("same");
LstBehind->Draw("same");
LfitBehind->Draw("same");
Mpl->Draw("same");
Lpl->SetLineStyle(1);
Lpl->SetLineWidth(2);Lpl->SetLineColor(15);
Lpl->Draw("same");



//Ltr->Draw("same");
Lst->Draw("same");
Msm->Draw();
Lfit->Draw("same");

//LplU->Draw("same");
//LplV->Draw("same");

TLegend l(0.7,0.7,0.9,0.9);



l.AddEntry(Msm,"1) smeared space points","p");
l.AddEntry(Lst,"2) seed for fit","l");

l.AddEntry(Lfit,"3) fitted track","l");
l.AddEntry(Mpl,"4) 2D reference plane for pull","p");
l.SetTextFont(102);
l.SetFillStyle(0);
l.SetLineWidth(1);
l.SetBorderSize(0);
l.Draw("same");


/*
TCanvas c3("c3","c3",600,600);
c3.SetPhi(phi);
c3.SetTheta(theta);
Msm->SetMarkerStyle(24);
Msm->SetMarkerSize(0.5);
Msm->SetMarkerColor(kBlue);
Msm->Draw();
Lst->Draw("same");
*/





}
