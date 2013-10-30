void SetMaxCA_ForAlign(){
  const int ndt=4;
  const int nda=4;
  int vdt[ndt] = {0, 200, 600, 800};
  int vda[nda] = {0, 3, 6, 9};
  int psiCA[nda][ndt];
 TMultiGraph *mgr_CAlim = new TMultiGraph("CAlim","");
 TLegend *leg = new TLegend(0.15,0.7,0.39,0.98);
 TGraph *grpsi[nda];
  for(int ida=0; ida<nda; ida++){
    int da = vda[ida];
    for(int idt=0; idt<ndt; idt++){
      int dt = vdt[idt];
      TString path = "/panda/pandaroot/macro/lmd/testAlignmentLMDpixel/tmp_align_";
      path+=dt;
      path+="mkm_";
      path+=da;
      path+="mrad_mom_15_sample00/Lumi_TCand_0.root";
      TFile *fin = new TFile(path.Data(),"READ");
      TH1* hcospsi = (TH1*)fin->Get("hcosPSI");
      double IntTot = hcospsi->Integral();
      double EntTot = hcospsi->GetEntries();
      //  cout<<"IntTot = "<<IntTot<<" EntTot = "<<EntTot<<endl;
      double currInt = 0;
      for(int i=0;i<1e4;i++){
	currInt = hcospsi->Integral(0,i);
	//	cout<<"currInt = "<<currInt<<endl;
	//	double eff = 100.*currInt/EntTot;
	double eff = 100.*currInt/IntTot;
	if(eff>95.){ 
	  psiCA[ida][idt] = i;
	  break;
	}
	else{
	  if(i>9998)  psiCA[ida][idt] = 0;
	}
      }
    }
    grpsi[ida]= new TGraph(ndt,vdt,psiCA[ida]);
    grpsi[ida]->SetMarkerStyle(20+ida); 
    grpsi[ida]->SetMarkerColor(kOrange+ida); 
    grpsi[ida]->SetMarkerSize(2.5); 
    mgr_CAlim->Add(grpsi[ida]);
    TString leAng = "#Delta_{#alpha}~";
    leAng += da;
    leAng += " mrad";
    leg->AddEntry(grpsi[ida],leAng,"P");
  }

  mgr_CAlim->Draw("APL");
  mgr_CAlim->GetXaxis()->SetTitle("#delta_{t}, #mum");
  mgr_CAlim->GetYaxis()->SetTitle("CA limit to accept 90% of neighboring cells, 10^{-5}");
  leg->SetFillColor(0);
  leg->Draw();
  //  hcospsi->Draw();
}
