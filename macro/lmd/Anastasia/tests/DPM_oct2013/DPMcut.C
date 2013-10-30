void DPMcut(){
  double Plab[5]={1.5,4.06,8.9,11.91,15};
  double thCut[30];
  thCut[0]=2e-2;
  for(int ith=1;ith<15;ith++){
    thCut[ith]=thCut[ith-1]+2e-2; //degree!
  }
  double CS_1_5[15]={1320.32959,348.836517,169.123718,106.102028,81.5195084,61.2443542,53.9445419,45.5504189,41.5222206,39.4303474,36.0226860,34.4050026,33.3348923,32.6022339,31.2131920};
  double CS_4_06[15]={155.734833,54.8221359,36.7322998,29.6530476,26.5662251,25.1666069,23.9947281,23.3945808,23.0912437,22.6343727,22.4072990,22.2762127,22.1385498,21.9681950,21.8791924};
  double CS_8_9[15]={42.1134033,20.4706917,16.3297157,15.0917206,14.3578768,14.0437069,13.8278847,13.7208920,13.5896463,13.5071402,13.4475269,13.3716459,13.3131437,13.2571535,13.2044640};
  double CS_11_91[15]={26.2160683,14.6451588,12.5770531,11.7729158,11.3963776,11.2288837,11.0782871,10.9958181,10.9065933,10.8369398,10.7789087,10.7115211,10.6470747,10.5816088,10.5164165};
  double CS_15[15]={18.8430214,11.8499861,10.5633678,10.1106071,9.89229965,9.76206112,9.66052914,9.5783596,9.51154518,9.43439007,9.36103916,9.28842354,9.20829582,9.12535954,9.03874302};

  TGraphErrors *gr_15 = new TGraphErrors(15,thCut,CS_15);
  gr_15->SetMarkerStyle(21);
  gr_15->SetMarkerColor(3);
  gr_15->SetMarkerSize(2.5);
  gr_15->GetYaxis()->SetTitle("#sigma_{EL}, mb");
  gr_15->GetXaxis()->SetTitle("#theta, ^{#circ}");
  gr_15->Draw("AP");
  TLine *l_csExp_15 = new TLine(thCut[0],11.59, thCut[14],11.59);
  l_csExp_15->SetLineWidth(2.);
  l_csExp_15->SetLineColor(2);
  l_csExp_15->Draw();

  TLatex txt_15;
  txt_15.SetTextSize(0.05);
  txt_15.SetTextAlign(13);  //align at top
  txt_15.DrawLatex(.1,15,"P_{lab}=15 GeV/c");
  // TGraphErrors *gr_1_5 = new TGraphErrors(15,thCut,CS_1_5);
  // gr_1_5->SetMarkerStyle(21);
  // gr_1_5->SetMarkerColor(3);
  // gr_1_5->SetMarkerSize(2.5);
  // gr_1_5->GetYaxis()->SetTitle("#sigma_{EL}, mb");
  // gr_1_5->GetXaxis()->SetTitle("#theta, ^{#circ}");
  // gr_1_5->Draw("AP");
  // TLine *l_csExp_1_5 = new TLine(thCut[0], 37.5, thCut[14], 37.5);
  // l_csExp_1_5->SetLineWidth(2.);
  // l_csExp_1_5->SetLineColor(2);
  // l_csExp_1_5->Draw();

  // TLatex txt_1_5;
  // txt_1_5.SetTextSize(0.05);
  // txt_1_5.SetTextAlign(13);  //align at top
  // txt_1_5.DrawLatex(.1,900,"P_{lab}=1.5 GeV/c");


  // TGraphErrors *gr_4_06 = new TGraphErrors(15,thCut,CS_4_06);
  // gr_4_06->SetMarkerStyle(21);
  // gr_4_06->SetMarkerColor(3);
  // gr_4_06->SetMarkerSize(2.5);
  // gr_4_06->GetYaxis()->SetTitle("#sigma_{EL}, mb");
  // gr_4_06->GetXaxis()->SetTitle("#theta, ^{#circ}");
  // gr_4_06->Draw("AP");
  // TLine *l_csExp_4_06 = new TLine(thCut[0],19.75, thCut[14], 19.75);
  // l_csExp_4_06->SetLineWidth(2.);
  // l_csExp_4_06->SetLineColor(2);
  // l_csExp_4_06->Draw();

  // TLatex txt_4_06;
  // txt_4_06.SetTextSize(0.05);
  // txt_4_06.SetTextAlign(13);  //align at top
  // txt_4_06.DrawLatex(.1,90,"P_{lab}=4.06 GeV/c");


  // TGraphErrors *gr_8_9 = new TGraphErrors(15,thCut,CS_8_9);
  // gr_8_9->SetMarkerStyle(21);
  // gr_8_9->SetMarkerColor(3);
  // gr_8_9->SetMarkerSize(2.5);
  // gr_8_9->GetYaxis()->SetTitle("#sigma_{EL}, mb");
  // gr_8_9->GetXaxis()->SetTitle("#theta, ^{#circ}");
  // gr_8_9->Draw("AP");
  // TLine *l_csExp_8_9 = new TLine(thCut[0],13.89, thCut[14],13.89);
  // l_csExp_8_9->SetLineWidth(2.);
  // l_csExp_8_9->SetLineColor(2);
  // l_csExp_8_9->Draw();

  // TLatex txt_8_9;
  // txt_8_9.SetTextSize(0.05);
  // txt_8_9.SetTextAlign(13);  //align at top
  // txt_8_9.DrawLatex(.1,39,"P_{lab}=8.9 GeV/c");

  // TGraphErrors *gr_11_91 = new TGraphErrors(15,thCut,CS_11_91);
  // gr_11_91->SetMarkerStyle(21);
  // gr_11_91->SetMarkerColor(3);
  // gr_11_91->SetMarkerSize(2.5);
  // gr_11_91->GetYaxis()->SetTitle("#sigma_{EL}, mb");
  // gr_11_91->GetXaxis()->SetTitle("#theta, ^{#circ}");
  // gr_11_91->Draw("AP");
  // TLine *l_csExp_11_91 = new TLine(thCut[0],11.59, thCut[14],11.59);
  // l_csExp_11_91->SetLineWidth(2.);
  // l_csExp_11_91->SetLineColor(2);
  // l_csExp_11_91->Draw();

  // TLatex txt_11_91;
  // txt_11_91.SetTextSize(0.05);
  // txt_11_91.SetTextAlign(13);  //align at top
  // txt_11_91.DrawLatex(.1,19,"P_{lab}=11.91 GeV/c");


}
