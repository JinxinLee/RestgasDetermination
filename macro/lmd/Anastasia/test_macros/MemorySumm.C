void MemorySumm(TString pathName="/panda/curpandaroot/macro/lmd/testPixel/memory/mom_1_5/")
{
  TString fcomname = pathName;
  fcomname+="FairRunInfo_Lumi_";
  TString fMC = fcomname;
  fMC += "MC_0.root";
  TFile *fileMC = new TFile(fMC,"READ");
  TH1F* hVmemMC= (TH1F*)fileMC->Get("VirtualMemoryVsEvent");
  //fileMC->Close();
  
  TString fdigi = fcomname;
  fdigi += "digi_0.root";
  TFile *filedigi = new TFile(fdigi,"READ");
  TH1F* hVmemDigi = (TH1F*)filedigi->Get("VirtualMemoryVsEvent");
  //filedigi->Close();


  TString freco = fcomname;
  freco +="reco_0.root";
  TFile *filereco = new TFile(freco,"READ");
  TH1F* hVmemReco = (TH1F*)filereco->Get("VirtualMemoryVsEvent");
  //filereco->Close();

  TString frecoMerg = fcomname;
  frecoMerg +="recoMerged_0.root";
  TFile *filerecoMerg = new TFile(frecoMerg,"READ");
  TH1F* hVmemRecoMerg = (TH1F*)filerecoMerg->Get("VirtualMemoryVsEvent");
  //filerecoMerg->Close();

  TString fTrkCA = fcomname;
  fTrkCA+="TCand_CA_0.root";
  TFile *fileTrkCA = new TFile(fTrkCA,"READ");
  TH1F* hVmemTrkCA = (TH1F*)fileTrkCA->Get("VirtualMemoryVsEvent");
  //fileTrkCA->Close();

  TString fTrkF = fcomname;
  fTrkF +="TCand_Follow_0.root";
  TFile *fileTrkF = new TFile(fTrkF,"READ");
  TH1F* hVmemTrkF = (TH1F*)fileTrkF->Get("VirtualMemoryVsEvent");
  //fileTrkF->Close();

  TString fMinuit = fcomname;
  fMinuit +="Track_Minuit_0.root";
  TFile *fileMinuit = new TFile(fMinuit,"READ");
  TH1F* hVmemMinuit = (TH1F*)fileMinuit->Get("VirtualMemoryVsEvent");
  // fileMinuit->Close();

  TString fKalmanG = fcomname;
  fKalmanG+="Track_GEANE_0.root";
  TFile *fileKalmanG = new TFile(fKalmanG,"READ");
  TH1F* hVmemKalmanG = (TH1F*)fileKalmanG->Get("VirtualMemoryVsEvent");
  //fileKalmanG->Close();

  TString fKalmanRK = fcomname;
  fKalmanRK +="Track_RK_0.root";
  TFile *fileKalmanRK = new TFile(fKalmanRK,"READ");
  TH1F* hVmemKalmanRK = (TH1F*)fileKalmanRK->Get("VirtualMemoryVsEvent");
  //fileKalmanRK->Close();

  TString fBPG = fcomname;
  fBPG += "BP_Geane_0.root";
  TFile *fileBPG = new TFile(fBPG,"READ");
  TH1F* hVmemBPG = (TH1F*)fileBPG->Get("VirtualMemoryVsEvent");
  // fileBPG->Close();

  TString fBPRK = fcomname;
  fBPRK+="BP_RK_0.root";
  TFile *fileBPRK = new TFile(fBPRK,"READ");
  TH1F* hVmemBPRK = (TH1F*)fileBPRK->Get("VirtualMemoryVsEvent");
  // fileBPRK->Close();

  hVmemMC->SetLineColor(2);
  hVmemDigi->SetLineColor(43);
  hVmemReco->SetLineColor(13);
  hVmemRecoMerg->SetLineColor(17);
  hVmemTrkCA->SetLineColor(8);
  hVmemTrkF->SetLineColor(32);
  hVmemMinuit->SetLineColor(9);
  hVmemKalmanG->SetLineColor(4);
  hVmemKalmanRK->SetLineColor(41);
  hVmemBPG->SetLineColor(7);
  hVmemBPRK->SetLineColor(46);

  TString StepName[11]={"MC","digi","hit rec","hit merge","Trk Search CA","Trk Search F","Trk Fit Minuit","Trk Fit Kalman (G)","Trk Fit Kalman (RK)",
			"back propag (G)", "back propag (RK)"};
  TLegend *leg = new TLegend(0.7,0.65,0.98,0.945);
  leg->SetFillColor(0);
  leg->AddEntry(hVmemMC,StepName[0],"l");
  leg->AddEntry(hVmemDigi,StepName[1],"l");
  leg->AddEntry(hVmemReco,StepName[2],"l");
  leg->AddEntry(hVmemRecoMerg,StepName[3],"l");
  leg->AddEntry(hVmemTrkCA,StepName[4],"l");
  leg->AddEntry(hVmemTrkF,StepName[5],"l");
  leg->AddEntry(hVmemMinuit,StepName[6],"l");
  leg->AddEntry(hVmemKalmanG,StepName[7],"l");
  leg->AddEntry(hVmemKalmanRK,StepName[8],"l");
  leg->AddEntry(hVmemBPG,StepName[9],"l");
  leg->AddEntry(hVmemBPRK,StepName[10],"l");


  TCanvas c1;
  //  c1.PaintDate();
  hVmemMC->Draw();
  hVmemMC->SetMinimum(300);
  hVmemMC->SetMaximum(8000);
  hVmemDigi->Draw("same");
  hVmemReco->Draw("same");
  hVmemRecoMerg->Draw("same");
  hVmemTrkCA->Draw("same");
  hVmemTrkF->Draw("same");
  hVmemMinuit->Draw("same");
  hVmemKalmanG->Draw("same");
  hVmemKalmanRK->Draw("same");
  hVmemBPRK->Draw("same");
  hVmemBPG->Draw("same");
  leg->Draw();

  c1.Print("VMEM.root");
  c1.Print("VMEM.pdf");
  c1.Print("VMEM.jpg");
}
