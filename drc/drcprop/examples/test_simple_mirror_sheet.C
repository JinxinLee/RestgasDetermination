{
   gROOT->Reset();
   TCanvas *c1 = new TCanvas("c1","c1",700,900);
   //c1->Divide(1,2);
   //c1->cd(1);
   TFile f("test_simple_mirror_sheet.root"); //file produced by tutorial hsimple.C
   TNtuple *ntuple = (TNtuple*)f.Get("ntuple");
   //solution 1: use a scatter plot with different colors
   //corresponding to one variable (here i%8)
   Int_t col[10] = {1,793,632,797,400,416,600,880,922,920};
   
   TString cut1 = "iside_refl%10==";
   ntuple->SetMarkerColor(1);
   ntuple->SetMarkerStyle(20);
   ntuple->SetMarkerSize(1);
   //TAxis *xaxis = ntuple->GetXaxis();
   for (Int_t i=0;i<10;i++) {
      ntuple->SetMarkerColor(col[i]);
      TString cut2 = Form("%d",i);//TString(itoa(i));
      cout<<cut2<<endl;
      
      TString selection=cut1+cut2;
      if (i==0) ntuple->Draw("x:y:t>>h(10,15,35,100,-300,300,100,-300,300)",selection);
      else        ntuple->Draw("x:y:t",selection,"same");
   }
   //solution 2: use a surface plot using the 4th dimension
   //as the weight for each cell in the 3-d histogram
   //c1->cd(2);
   //ntuple->SetFillColor(38);
   //ntuple->Draw("px:py:pz","random","iso");
}
