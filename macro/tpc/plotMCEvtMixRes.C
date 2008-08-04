{
TCanvas* c=new TCanvas("c","residuals",10,10,800,800);

c->Divide(2,2);

TCut phys("_ID._mceventID==0 && _status==0");


c->cd(1);
cbmsim->Draw("DeconvTrkl._t0>>ht0(100,-300,300)",phys);
cbmsim->Draw("DeconvTrkl._t0>>ht0V0(100,-300,300)",phys && "_q==0","same");
ht0V0->SetLineColor(kRed);

c->cd(2);
cbmsim->Draw("DeconvTrkl._zpoca.X()>>hxy(100,-0.02,0.02)",phys && "abs(_t0)<600");

c->cd(3);
cbmsim->Draw("DeconvTrkl._V0res>>hV0res(100,0,2)",phys && "_q==0");

c->cd(4);
cbmsim->Draw("DeconvTrkl._mvdhits>>hmvd(100,0,10)",phys);









}
