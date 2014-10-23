void plotMTrange(){
  double p1[2]={1.5,1.5};
  double t1[2]={0.05,1.5};
  TGraph *gr1 = new TGraph(2,t1,p1);
  gr1->SetMarkerStyle(20);
  gr1->SetMarkerSize(1.5);
  gr1->SetMarkerColor(kRed-9);
  gr1->SetLineColor(kRed-9);

 double p2[2]={1.6,1.6};
 double t2[2]={0.06,1.7};
 TGraph *gr2 = new TGraph(2,t2,p2);
  gr2->SetMarkerStyle(20);
  gr2->SetMarkerSize(1.5);
  gr2->SetMarkerColor(kRed-9);
  gr2->SetLineColor(kRed-9);

  double p3[2]={1.71,1.71};
  double t3[2]={0.07,1.85};
  TGraph *gr3 = new TGraph(2,t3,p3);
  gr3->SetMarkerStyle(20);
  gr3->SetMarkerSize(1.5);
  gr3->SetMarkerColor(kRed-9);
  gr3->SetLineColor(kRed-9);

  double p4[2]={1.78,1.78};
  double t4[2]={0.001,0.1};
  TGraph *gr4 = new TGraph(2,t4,p4);
  gr4->SetMarkerStyle(20);
  gr4->SetMarkerSize(1.5);
  gr4->SetMarkerColor(kRed-9);
  gr4->SetLineColor(kRed-9);

  double p5[2]={1.81,1.81};
  double t5[2]={0.07,2.05};
  TGraph *gr5 = new TGraph(2,t5,p5);
  gr5->SetMarkerStyle(20);
  gr5->SetMarkerSize(1.5);
  gr5->SetMarkerColor(kRed-9);
  gr5->SetLineColor(kRed-9);

  double p6[2]={1.86,1.86};
  double t6[2]={0.08,2.1};
  TGraph *gr6 = new TGraph(2,t6,p6);
  gr6->SetMarkerStyle(20);
  gr6->SetMarkerSize(1.5);
  gr6->SetMarkerColor(kRed-9);
  gr6->SetLineColor(kRed-9);

 double p7[2]={1.91,1.91};
 double t7[2]={0.07,2.2};
 TGraph *gr7 = new TGraph(2,t7,p7);
  gr7->SetMarkerStyle(20);
  gr7->SetMarkerSize(1.5);
  gr7->SetMarkerColor(kRed-9);
  gr7->SetLineColor(kRed-9);

  double p8[2]={2.01,2.01};
  double t8[2]={0.08,2.3};
  TGraph *gr8 = new TGraph(2,t8,p8);
  gr8->SetMarkerStyle(20);
  gr8->SetMarkerSize(1.5);
  gr8->SetMarkerColor(kRed-9);
  gr8->SetLineColor(kRed-9);

  double p9[2]={2.12,2.12};
  double t9[2]={0.1,2.5};
  TGraph *gr9 = new TGraph(2,t9,p9);
  gr9->SetMarkerStyle(20);
  gr9->SetMarkerSize(1.5);
  gr9->SetMarkerColor(kRed-9);
  gr9->SetLineColor(kRed-9);

  double p10[2]={2.23,2.23};
  double t10[2]={0.1,2.5};
  TGraph *gr10 = new TGraph(2,t10,p10);
  gr10->SetMarkerStyle(20);
  gr10->SetMarkerSize(1.5);
  gr10->SetMarkerColor(kRed-9);
  gr10->SetLineColor(kRed-9);

  double p11[2]={2.33,2.33};
  double t11[2]={0.1,2.6};
  TGraph *gr11 = new TGraph(2,t11,p11);
  gr11->SetMarkerStyle(20);
  gr11->SetMarkerSize(1.5);
  gr11->SetMarkerColor(kRed-9);
  gr11->SetLineColor(kRed-9);

  double p12[2]={2.33,2.33};
  double t12[2]={0.04,2.9};
  TGraph *gr12 = new TGraph(2,t12,p12);
  gr12->SetMarkerStyle(21);
  gr12->SetMarkerSize(1.5);
  gr12->SetMarkerColor(kRed+2);//used in DPM
  gr12->SetLineColor(kRed+2);

 double p13[2]={2.43,2.43};
 double t13[2]={ 0.11,2.6};
  TGraph *gr13 = new TGraph(2,t13,p13);
  gr13->SetMarkerStyle(20);
  gr13->SetMarkerSize(1.5);
  gr13->SetMarkerColor(kRed-9);
  gr13->SetLineColor(kRed-9);

  double p14[2]={2.607,2.607};
  double t14[2]={0.001,0.1};
  TGraph *gr14 = new TGraph(2,t14,p14);
  gr14->SetMarkerStyle(20);
  gr14->SetMarkerSize(1.5);
  gr14->SetMarkerColor(kRed-9);
  gr14->SetLineColor(kRed-9);

  double p15[2]={2.85,2.85};
  double t15[2]={0.04,3.7};
  TGraph *gr15 = new TGraph(2,t15,p15);
  gr15->SetMarkerStyle(21);
  gr15->SetMarkerSize(1.5);
  gr15->SetMarkerColor(kRed+2);//used in DPM
  gr15->SetLineColor(kRed+2);

  double p16[2]={3.0,3.0};
  double t16[2]={0.02,1.3};
  TGraph *gr16 = new TGraph(2,t16,p16);
  gr16->SetMarkerStyle(20);
  gr16->SetMarkerSize(1.5);
  gr16->SetMarkerColor(kRed-9);
  gr16->SetLineColor(kRed-9);

  double p17[2]={3.55,3.55};
  double t17[2]={0.16,3.9};
  TGraph *gr17 = new TGraph(2,t17,p17);
  gr17->SetMarkerStyle(21);
  gr17->SetMarkerSize(1.5);
  gr17->SetMarkerColor(kRed+2);//used in DPM
  gr17->SetLineColor(kRed+2);

 double p18[2]={3.65,3.65};
 double t18[2]={0.003,1.4};
 TGraph *gr18 = new TGraph(2,t18,p18);
 gr18->SetMarkerStyle(20);
 gr18->SetMarkerSize(1.5);
 gr18->SetMarkerColor(kRed-9);
 gr18->SetLineColor(kRed-9);

 double p19[2]={3.7,3.7};
 double t19[2]={0.001,0.016};
  TGraph *gr19 = new TGraph(2,t19,p19);
  gr19->SetMarkerStyle(22);
  gr19->SetMarkerSize(1.5);
  gr19->SetMarkerColor(kRed);//used in E760
  gr19->SetLineColor(kRed);

 double p20[2]={4.07,4.07};
 double t20[2]={0.001,0.016};
  TGraph *gr20 = new TGraph(2,t20,p20);
  gr20->SetMarkerStyle(22);
  gr20->SetMarkerSize(1.5);
  gr20->SetMarkerColor(kRed);//used in E760
  gr20->SetLineColor(kRed);

 double p21[2]={4.2,4.2};
 double t21[2]={0.001,0.019};
  TGraph *gr21 = new TGraph(2,t21,p21);
 gr21->SetMarkerStyle(20);
 gr21->SetMarkerSize(1.5);
 gr21->SetMarkerColor(kRed-9);
 gr21->SetLineColor(kRed-9);

 double p22[2]={5.0,5.0};
 double t22[2]={0.03,1.2};
 TGraph *gr22 = new TGraph(2,t22,p22);
 gr22->SetMarkerStyle(20);
 gr22->SetMarkerSize(1.5);
 gr22->SetMarkerColor(kRed-9);
 gr22->SetLineColor(kRed-9);

 double p23[2]={5.0,5.0};
 double t23[2]={0.16,7.6};
 TGraph *gr23 = new TGraph(2,t23,p23);
 gr23->SetMarkerStyle(20);
 gr23->SetMarkerSize(1.5);
 gr23->SetMarkerColor(kRed-9);
 gr23->SetLineColor(kRed-9);

 double p24[2]={5.6,5.6};
 double t24[2]={0.001,0.02};
 TGraph *gr24 = new TGraph(2,t24,p24);
 gr24->SetMarkerStyle(22);
 gr24->SetMarkerSize(1.5);
 gr24->SetMarkerColor(kRed);//used in E760
 gr24->SetLineColor(kRed);

 double p25[2]={5.7,5.7};
 double t25[2]={0.03,2.5};
 TGraph *gr25 = new TGraph(2,t25,p25);
 gr25->SetMarkerStyle(21);
 gr25->SetMarkerSize(1.5);
 gr25->SetMarkerColor(kRed+2);//used in DPM
 gr25->SetLineColor(kRed+2);

 double p26[2]={5.72,5.72};
 double t26[2]={0.0008,0.02};
 TGraph *gr26 = new TGraph(2,t26,p26);
 gr26->SetMarkerStyle(22);
 gr26->SetMarkerSize(1.5);
 gr26->SetMarkerColor(kRed);//used in E760
 gr26->SetLineColor(kRed);

 double p27[2]={5.94,5.94};
 double t27[2]={0.0008,0.02};
 TGraph *gr27 = new TGraph(2,t27,p27);
 gr27->SetMarkerStyle(22);
 gr27->SetMarkerSize(1.5);
 gr27->SetMarkerColor(kRed);//used in E760
 gr27->SetLineColor(kRed);

double p28[2]={6.0,6.0};
 double t28[2]={0.0015,0.035};
 TGraph *gr28 = new TGraph(2,t28,p28);
 gr28->SetMarkerStyle(20);
 gr28->SetMarkerSize(1.5);
 gr28->SetMarkerColor(kRed-9);
 gr28->SetLineColor(kRed-9);

 double p29[2]={6.2,6.2};
 double t29[2]={0.31,1};
 TGraph *gr29 = new TGraph(2,t29,p29);
 gr29->SetMarkerStyle(20);
 gr29->SetMarkerSize(1.5);
 gr29->SetMarkerColor(kRed-9);
 gr29->SetLineColor(kRed-9);

 double p30[2]={6.23,6.23};
 double t30[2]={0.001,0.02};
 TGraph *gr30 = new TGraph(2,t30,p30);
 gr30->SetMarkerStyle(22);
 gr30->SetMarkerSize(1.5);
 gr30->SetMarkerColor(kRed);//used in E760
 gr30->SetLineColor(kRed);

 double p31[2]={8.0,8.0};
 double t31[2]={0.05,0.86};
 TGraph *gr31 = new TGraph(2,t31,p31);
 gr31->SetMarkerStyle(20);
 gr31->SetMarkerSize(1.5);
 gr31->SetMarkerColor(kRed-9);
 gr31->SetLineColor(kRed-9);

 double p32[2]={8.0,8.0};
 double t32[2]={0.0018,0.08};
 TGraph *gr32 = new TGraph(2,t32,p32);
 gr32->SetMarkerStyle(20);
 gr32->SetMarkerSize(1.5);
 gr32->SetMarkerColor(kRed-9);
 gr32->SetLineColor(kRed-9);

 double p33[2]={8.0,8.0};
 double t33[2]={0.02,0.85};
 TGraph *gr33 = new TGraph(2,t33,p33);
 gr33->SetMarkerStyle(20);
 gr33->SetMarkerSize(1.5);
 gr33->SetMarkerColor(kRed-9);
 gr33->SetLineColor(kRed-9);

 double p34[2]={10.0,10.0};
 double t34[2]={0.0018,0.1};
 TGraph *gr34 = new TGraph(2,t34,p34);
 gr34->SetMarkerStyle(20);
 gr34->SetMarkerSize(1.5);
 gr34->SetMarkerColor(kRed-9);
 gr34->SetLineColor(kRed-9);

 double p35[2]={10.1,10.1};
 double t35[2]={0.2,3};
 TGraph *gr35 = new TGraph(2,t35,p35);
 gr35->SetMarkerStyle(20);
 gr35->SetMarkerSize(1.5);
 gr35->SetMarkerColor(kRed-9);
 gr35->SetLineColor(kRed-9);

 double p36[2]={10.4,10.4};
 double t36[2]={0.02,0.2};
 TGraph *gr36 = new TGraph(2,t36,p36);
 gr36->SetMarkerStyle(21);
 gr36->SetMarkerSize(1.5);
 gr36->SetMarkerColor(kRed+2);//used in DPM
 gr36->SetLineColor(kRed+2);

 double p37[2]={15.95,15.95};
 double t37[2]={0.11,1.3};
 TGraph *gr37 = new TGraph(2,t37,p37);
 gr37->SetMarkerStyle(21);
 gr37->SetMarkerSize(1.5);
 gr37->SetMarkerColor(kRed+2);//used in DPM
 gr37->SetLineColor(kRed+2);

double p38[2]={16.0,16.0};
 double t38[2]={0.085,1.2};
 TGraph *gr38 = new TGraph(2,t38,p38);
 gr38->SetMarkerStyle(20);
 gr38->SetMarkerSize(1.5);
 gr38->SetMarkerColor(kRed-9);
 gr38->SetLineColor(kRed-9);
  TMultiGraph *mgr = new TMultiGraph(); 
  mgr->Add(gr1,"PL");
  mgr->Add(gr2,"PL");
  mgr->Add(gr3,"PL");
  mgr->Add(gr4,"PL");
  mgr->Add(gr5,"PL");
  mgr->Add(gr6,"PL");
  mgr->Add(gr8,"PL");
  mgr->Add(gr9,"PL");
  mgr->Add(gr10,"PL");
  mgr->Add(gr11,"PL");

  mgr->Add(gr13,"PL");
  mgr->Add(gr14,"PL");

  mgr->Add(gr16,"PL");

  mgr->Add(gr18,"PL");


  mgr->Add(gr21,"PL");
  mgr->Add(gr22,"PL");
  mgr->Add(gr23,"PL");



  mgr->Add(gr28,"PL");
  mgr->Add(gr29,"PL");
 
  mgr->Add(gr31,"PL");
  mgr->Add(gr32,"PL");
  mgr->Add(gr33,"PL"); mgr->Add(gr34,"PL");  mgr->Add(gr35,"PL");
     mgr->Add(gr38,"PL");

  mgr->Add(gr12,"PL");   mgr->Add(gr15,"PL");   mgr->Add(gr17,"PL");   
  mgr->Add(gr25,"PL"); mgr->Add(gr36,"PL");  mgr->Add(gr37,"PL");
  mgr->Add(gr19,"PL");   mgr->Add(gr20,"PL");    mgr->Add(gr24,"PL");
  mgr->Add(gr26,"PL");     mgr->Add(gr27,"PL");   mgr->Add(gr30,"PL");


  TCanvas cRES("cRES","canvas",800,600);
  cRES.SetLogx();
  mgr->Draw("A");
  mgr->GetXaxis()->SetLimits(1e-4,1e2);
  mgr->GetXaxis()->SetTitle("|t|, (GeV/c)^{2}");
  mgr->GetYaxis()->SetTitle("P_{lab}, GeV/c");

TLegend *leg = new TLegend(0.7,0.7,0.97,0.95);
 leg->SetFillColor(0);
 leg->SetTextFont(42);
 leg->SetTextSize(0.05);
 leg->AddEntry(gr1,"not used","lp");
 leg->AddEntry(gr12,"used in DPM","lp");
 leg->AddEntry(gr19,"used in E760","lp");
 leg->Draw();
 cRES.SaveAs("t_ranges_prev_exp.pdf");
 cRES.SaveAs("t_ranges_prev_exp.root");
}
