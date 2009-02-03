plot_par()
{
  TFile* m_InPutF = new TFile("part_tree_clean.root","READ");
  TTree *t1 = (TTree*) m_InPutF->Get("pion");
  TTree *t2 = (TTree*) m_InPutF->Get("electron");
  float stt1,mvd1,tof1,thetaC1,emc1,p1;
  float stt2,mvd2,tof2,thetaC2,emc2,p2;
  
  TH1F *f0 = new TH1F("f0","f0",100,-1,6);
  TH1F *f1 = new TH1F("f1","f1",100,-1,6);
  TH1F *f2 = new TH1F("f2","f2",1000,5,25);
  TH1F *f3 = new TH1F("f3","f3",1000,5,25);
  TH1F *f6 = new TH1F("f6","f6",1000,0,0.014);
  TH1F *f7 = new TH1F("f7","f7",1000,0,0.014);
  
  TH1F *f8 = new TH1F("f8","f8",100,-1,14);
  TH1F *f9 = new TH1F("f9","f9",100,-1,14);
  
  TH2F *f4 = new TH2F("f4","f4",100,0,0.014,100,5,25);
  TH2F *f5 = new TH2F("f5","f5",100,0,0.014,100,5,25);
  
  TH2F *f10 = new TH2F("f10","f10",100,-1,6,100,-1,6);
  TH2F *f11 = new TH2F("f11","f11",100,-1,6,100,-1,6);
  
  TH1F *f12 = new TH1F("f12","f12",1000,0,2);
  TH1F *f13 = new TH1F("f13","f13",1000,0,2);
  TH2F *f14 = new TH2F("f14","f14",1000,5,25,1000,0,4);
  TH2F *f15 = new TH2F("f15","f15",1000,5,25,1000,0,4);
  
  TH1F *f16 = new TH1F("f16","f16",1000,1,2);
  TH1F *f17 = new TH1F("f17","f17",1000,1,2);
  
  f0->SetFillColor(2);
  f1->SetFillColor(4);
  f2->SetFillColor(2);
  f3->SetFillColor(4);
  f4->SetMarkerColor(2);
  f5->SetMarkerColor(4);
  f10->SetMarkerColor(2);
  f11->SetMarkerColor(4);
  
  f6->SetFillColor(2);
  f7->SetFillColor(4);
  f8->SetFillColor(2);
  f9->SetFillColor(4);
  f11->SetFillColor(2);
  f12->SetFillColor(2);
  f13->SetFillColor(4);
  f14->SetMarkerColor(2);
  f15->SetMarkerColor(4);
  f16->SetFillColor(2);
  f17->SetFillColor(4);
  
  t1->SetBranchAddress("stt",&stt1);
  t1->SetBranchAddress("tof",&tof1);
  t1->SetBranchAddress("mvd",&mvd1);
  t1->SetBranchAddress("thetaC",&thetaC1);
  t1->SetBranchAddress("emc",&emc1);
  t1->SetBranchAddress("p",&p1);
  
  t2->SetBranchAddress("stt",&stt2);
  t2->SetBranchAddress("tof",&tof2);
  t2->SetBranchAddress("mvd",&mvd2);
  t2->SetBranchAddress("thetaC",&thetaC2);
  t2->SetBranchAddress("emc",&emc2);
  t2->SetBranchAddress("p",&p2);
  
  for (int i=0; i < t1->GetEntries(); i++ ){
    t1->GetEntry(i);
    f0->Fill(tof1);
    f2->Fill(stt1);
    f4->Fill(mvd1,stt1);
    if ( mvd1 > 0 ) f6->Fill(mvd1);
    f8->Fill(thetaC1);
    f10->Fill(tof1,thetaC1);
    f12->Fill(emc1);
    f14->Fill(stt1,emc1);
    f16->Fill(p1);
    //cout<<p1<<endl;
  }
  
  for (int i=0; i < t2->GetEntries(); i++ ){
    t2->GetEntry(i);
    f1->Fill(tof2);
    f3->Fill(stt2);
    f5->Fill(mvd2,stt2);
    if (mvd2 > 0) f7->Fill(mvd2);
    f9->Fill(thetaC2);
    f11->Fill(tof2,thetaC2);
    f13->Fill(emc2);
    f15->Fill(stt2,emc2);
    f17->Fill(p2);
  }
  //TCanvas c1;
  TCanvas *c1 = new TCanvas("c1","c1",100,100,800,600);
  c1->Divide(2,4);
  c1->cd(1);
  f0->Draw();
  f1->Draw("same");
  f0->Draw("same");
  c1->cd(2);
  f2->Draw();
  f3->Draw("same");
  
  c1->cd(3);
  f6->Draw();
  f7->Draw("same");
  f6->Draw("same");
  c1->cd(4);
  f8->Draw();
  f9->Draw("same");
  c1->cd(5);
  f4->Draw();
  f5->Draw("same");
  c1->cd(6);
  f10->Draw();
  f11->Draw("same");
  c1->cd(7);
  f12->Draw();
  f13->Draw("same");
  c1->cd(8);
  f14->Draw();
  f15->Draw("same");
}
