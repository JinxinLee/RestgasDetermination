{
gROOT->SetStyle("Plain");


double phi=126.6;
double theta=22.1;

Ltr->SetLineColor(kGreen);
Lst->SetLineColor(kRed);
Lst->SetLineColor(kBlue);
Mtr->SetMarkerStyle(24);
Mtr->SetMarkerSize(0.4);
Mtr->SetMarkerColor(kGreen);
Msm->SetMarkerStyle(24);
Msm->SetMarkerSize(0.4);
Msm->SetMarkerColor(kRed);


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



Msm->Draw();
Ltr->Draw("same");
Lst->Draw("same");
Mpl->Draw("same");
//LplU->Draw("same");
//LplV->Draw("same");

TLegend l(0.7,0.7,0.9,0.9);
l.AddEntry(Ltr,"1) true track","l");


l.AddEntry(Msm,"2) smeared space points","p");
l.AddEntry(Lst,"3) seed for fit","l");
l.AddEntry(Mpl,"4) 2D reference plane for pulls","p");
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
