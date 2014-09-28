

 Double_t mysBFunc(double s) {
   double sB_FTF = 15.65 + 700*TMath::Power(2.173-sqrt(s),2.5);
   if(sqrt(s)>2.172) sB_FTF = 34/sqrt(s);
   return sB_FTF;
   }

void FTF_DPM_cs(){
  double s= 5*5;
// DPM
  double sA = 51.6/sqrt(s)-58.8/s + 16.4/TMath::Power(s,1.5);
  double sB = 77.4/sqrt(s)-88.2/s + 24.6/TMath::Power(s,1.5);
  double sC = 93/s - 106/TMath::Power(s,1.5) + 30/(s*s);
  double sG = 18.6/TMath::Power(s,0.08) - 33.5/sqrt(s) + 30.8/s;
  double sD = 0;
  double sE = 0;
  double sF = 0;
  double sH = 0;
  TF1 *fsA = new TF1("fsA","51.6/sqrt(x)-58.8/x + 16.4/TMath::Power(x,1.5)",4,36);
  fsA->SetLineColor(1);
  fsA->SetLineWidth(3);
  fsA->SetLineStyle(2);
  TF1 *fsB = new TF1("fsB","77.4/sqrt(x)-88.2/x + 24.6/TMath::Power(x,1.5)",4,36);
  fsB->SetLineColor(2);
  fsB->SetLineWidth(3);
  fsB->SetLineStyle(2);

  TF1 *fsC = new TF1("fsC","93/x - 106/TMath::Power(x,1.5) + 30/(x*x)",4,36);
  fsC->SetLineColor(3);
  fsC->SetLineWidth(3);
  fsC->SetLineStyle(2);

  TF1 *fsE = new TF1("fsE","0*x",4,36);
  fsE->SetLineColor(4);
  fsE->SetLineWidth(3);
  fsE->SetLineStyle(2);

  TF1 *fsG = new TF1("fsG","18.6/TMath::Power(x,0.08) - 33.5/sqrt(x) + 30.8/x",4,36);
  fsG->SetLineColor(5);
  fsG->SetLineWidth(3);
  fsG->SetLineStyle(2);
  
  TF1 *fsSum = new TF1("fsSum","fsA+fsB+fsC+fsE+fsG",4,36);
  fsSum->SetLineColor(6);
  fsSum->SetLineWidth(3);
  fsSum->SetMinimum(0);
  fsSum->SetLineStyle(2);
  //  fsSum->Draw();
  //  fsG->Draw();
  fsC->SetMinimum(-2);
  fsC->SetMaximum(35);
  fsC->GetXaxis()->SetTitle("s, (GeV)^{2}");
  fsC->GetYaxis()->SetTitle("#sigma, mb");
  fsC->Draw();
  fsE->Draw("same");
  fsB->Draw("same");
  fsA->Draw("same");
  TLegend *leg = new TLegend(0.88,0.7,0.98,0.95);
  leg->SetHeader("DPM: ");
  //   leg->AddEntry("fsSum","#sigma_{tot. inel} DPM","l");
   leg->AddEntry("fsA","#sigma_{A}","l");
   leg->AddEntry("fsB","#sigma_{B}","l");
   leg->AddEntry("fsC","#sigma_{C}","l");
   leg->AddEntry("fsE","#sigma_{E}","l");
   // leg->AddEntry("fsG","#sigma_{G} DPM","l");
 


//     \sigma_{a} & = 51.6/\sqrt{s} - 58.8/s + 16.4/s^{1.5}\\
//       \sigma_{b} & = 77.4/\sqrt{s} - 88.2/s + 24.6/s^{1.5} \nonumber\\
//       \sigma_{c} & = 93/s - 106/s^{1.5} + 30/s^{2} \nonumber\\
//       \sigma_{g} & = 18.6/s^{0.08} - 33.5/\sqrt{s} + 30.8/s \nonumber\\
//       \sigma_{d} & = \sigma_{e} = \sigma_{f} = \sigma_{h} = 0 \nonumber

// FTF
  double m = 0.938; //proton mass
  double sA_FTF = 25/sqrt(s-4*m*m);
  double sB_FTF = 15.65 + 700*TMath::Power(2.173-sqrt(s),2.5);
  if(sqrt(s)>2.172) sB_FTF = 34/sqrt(s);
  double sC_FTF = (2/sqrt(s-4*m*m))*(TMath::Power(2*m/s,2));
  double sE_FTF = 140/s;

  TF1 *fsA_FTF = new TF1("fsA_FTF","25/sqrt(x-4*0.938*0.938)",4,36);
  fsA_FTF->SetLineColor(1);
  fsA_FTF->SetLineWidth(3);


  TF1 *fsB_FTF = new TF1("fsB_FTF","mysBFunc(x)",4,36);
  fsB_FTF->SetLineColor(2);
  fsB_FTF->SetLineWidth(3);
 
  TF1 *fsC_FTF = new TF1("fsC_FTF","(2/sqrt(x-4*0.938*0.938))*(TMath::Power(2*0.938/x,2))",4,36);
  fsC_FTF->SetLineColor(3);
  fsC_FTF->SetLineWidth(3);
 
  TF1 *fsE_FTF = new TF1("fsE_FTF","140/x",4,36);
  fsE_FTF->SetLineColor(4);
  fsE_FTF->SetLineWidth(3);
   
  TF1 *fsSum_FTF = new TF1("fsSum_FTF","fsA_FTF+fsB_FTF+fsC_FTF+fsE_FTF+fsG",4,36);
  fsSum_FTF->SetLineColor(6);
  fsSum_FTF->SetLineWidth(3);
  fsSum_FTF->SetMinimum(0);
  //  fsSum_FTF->Draw("same");
  fsE_FTF->Draw("same");
  fsC_FTF->Draw("same");
  fsB_FTF->Draw("same");
  fsA_FTF->Draw("same");
  //  leg->AddEntry("fsSum_FTF","#sigma_{tot. inel} FTF","l");
  TLegend *leg2 = new TLegend(0.88,0.4,0.98,0.65);
  leg2->SetHeader("FTF: ");
  leg2->AddEntry("fsA_FTF","#sigma_{A}","l");
  leg2->AddEntry("fsB_FTF","#sigma_{B}","l");
  leg2->AddEntry("fsC_FTF","#sigma_{C}","l");
  leg2->AddEntry("fsE_FTF","#sigma_{E}","l");
  leg->Draw();
  leg2->Draw();
  cout<<"DPM: "<<sA<<" "<<sB<<" "<<sC<<" "<<sE<<endl;
  cout<<"FTF: "<<sA_FTF<<" "<<sB_FTF<<" "<<sC_FTF<<" "<<sE_FTF<<endl;
 
}
