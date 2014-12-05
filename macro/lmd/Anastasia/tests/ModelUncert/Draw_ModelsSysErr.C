void Draw_ModelsSysErr(){
  const int Npoints = 30;
  double plab[Npoints]={1.5, 1.6, 1.71, 1.776, 1.81, 1.86, 1.91, 2.01, 2.12, 2.23, 2.33, 2.43, 2.607, 2.85, 3., 3.65, 3.7, 4.07, 4.2, 5., 5.6, 5.7, 5.72, 6., 6.23, 8, 10, 10.1, 10.4, 16};
  double dpm_lmd[Npoints]={1.03959, 0.779715, 0.269866, 0.0623551, 0.273208, 0.543643, 0.836084, 1.2628, 2.02268, 2.45285, 2.53406, 4.92003, 1.49486, 0.649123, 0.491151, 6.41349, 6.26635, 10.1506, 11.9318, 17.5594, 15.6334, 15.2044, 16.3522, 12.815, 14.1192, 13.3866, 0.246657, 4.89026, 3.41264, 0.386694};
  double err_dpm_lmd[Npoints]={0.351894, 0.236592, 0.0922625, 0.00647721, 0.101586, 0.222705, 0.328228, 0.63127, 1.05247, 1.53713, 0.580463, 2.14004, 0.586376, 1.41994, 2.13398, 3.95255, 2.54631, 2.97125, 3.76341, 6.32209, 3.87471, 6.04065, 3.80878, 3.72273, 4.35814, 5.40778, 1.67676, 12.4169, 2.59851, 8.89337};
  double e760_lmd[Npoints]={0.0164035, 0.00859962, 0.0366396, 0.0677868, 0.224522, 0.205474, 0.167627, 0.0453202, 0.551512, 0.534404, 0.808844, 1.68802, 0.541136, 1.08232, 0.840849, 2.2014, 1.10315, 0.589194, 0.0202727, 0.0289875, 1.07401, 3.46127, 0.548765, 3.79349, 2.05811, 1.49126, 3.69767, 9.70818, 4.99238, 29.1489};
  double err_e760_lmd[Npoints]={0.00429051, 0.0264213, 0.0761525, 0.026686, 0.141367, 0.168364, 0.193127, 0.215391, 0.389222, 0.458424, 0.20952, 0.495963, 0.274092, 0.360101, 0.697359, 1.1805, 0.361962, 0.398398, 1.03084, 1.79518, 0.461638, 1.13257, 0.453064, 1.2637, 0.524299, 1.47823, 0.697975, 2.0195, 0.556984, 2.11476};
  double dpm_data[Npoints]={34.7713, 36.452, 34.0324, 26.9488, 33.1666, 30.6284, 29.6379, 24.3254, 24.6541, 21.1481, 16.9331, 26.5292, 5.9241, 1.87314, 1.21759, 10.4662, 10.0124, 14.3052, 16.2565, 20.8855, 17.6129, 17.0144, 18.2751, 14.0924, 15.3565, 13.8961, 0.25118, 4.97704, 3.46764, 0.388114};
  double err_dpm_data[Npoints]={11.7699, 11.0608, 11.6351, 2.79934, 12.3322, 12.547, 11.6352, 12.1602, 12.8284, 13.2529, 3.87875, 11.5393, 2.3238, 4.09743, 5.29024, 6.45018, 4.0685, 4.1874, 5.12747, 7.51964, 4.36533, 6.75971, 4.25667, 4.09382, 4.74007, 5.6136, 1.70751, 12.6372, 2.64039, 8.92603};
  double e760_data[Npoints]={9.79746, 0.944998, 1.53623, 1.98501, 5.63106, 4.21117, 2.8752, 0.573628, 5.29295, 4.0597, 5.11112, 9.06667, 2.27407, 3.47455, 2.35595, 4.09496, 2.0062, 0.931632, 0.0308102, 0.0371459, 1.27666, 4.07451, 0.644784, 4.35267, 2.32277, 1.56356, 3.75515, 9.84958, 5.05159, 28.9257};
  double err_e760_data[Npoints]={2.56263, 2.9034, 3.19294, 0.781448, 3.54553, 3.4506, 3.31258, 2.72625, 3.73543, 3.4825, 1.32397, 2.66391, 1.15185, 1.15602, 1.95391, 2.19592, 0.658267, 0.629946, 1.56667, 2.30042, 0.548745,  1.33323, 0.532337, 1.44998, 0.591721, 1.54989, 0.708826, 2.04892, 0.56359, 2.09856};


TGraphErrors *gr_dpm_data = new TGraphErrors(Npoints, plab, dpm_data, 0, err_dpm_data);
gr_dpm_data->SetMarkerColor(kBlue+1);
gr_dpm_data->SetLineColor(kBlue+1);
gr_dpm_data->SetMarkerStyle(21);
TGraphErrors *gr_dpm_lmd = new TGraphErrors(Npoints, plab, dpm_lmd, 0, err_dpm_lmd);
gr_dpm_lmd->SetMarkerColor(kBlue+1);
gr_dpm_lmd->SetLineColor(kBlue+1);
gr_dpm_lmd->SetMarkerStyle(21);

TGraphErrors *gr_e760_data = new TGraphErrors(Npoints, plab, e760_data, 0, err_e760_data);
gr_e760_data->SetMarkerColor(kAzure+1);
gr_e760_data->SetLineColor(kAzure+1);
gr_e760_data->SetMarkerStyle(20);
TGraphErrors *gr_e760_lmd = new TGraphErrors(Npoints, plab, e760_lmd, 0, err_e760_lmd);
gr_e760_lmd->SetMarkerColor(kAzure+1);
gr_e760_lmd->SetLineColor(kAzure+1);
gr_e760_lmd->SetMarkerStyle(20);

 TLegend *leg_sigt = new TLegend(0.89,0.77,0.99,0.95);
  leg_sigt->SetFillColor(0);
  leg_sigt->SetTextFont(42);
  leg_sigt->SetTextSize(0.04);

TCanvas c1("c1","canvas",600,800);
 c1.Divide(1,2);
c1.cd(1);
gPad->SetLogy();
TMultiGraph *mg_data = new TMultiGraph();
mg_data->Add(gr_dpm_data);
mg_data->Add(gr_e760_data);
 leg_sigt->AddEntry(gr_dpm_data,"DPM","pel");
 leg_sigt->AddEntry(gr_e760_data,"E760","pel");
mg_data->Draw("AP");
mg_data->GetYaxis()->SetTitle("model system.err, %");
mg_data->GetXaxis()->SetTitle("P_{lab}, GeV/c");
 leg_sigt->Draw();
c1.cd(2);
gPad->SetLogy();
TMultiGraph *mg_lmd = new TMultiGraph();
mg_lmd->Add(gr_dpm_lmd);
mg_lmd->Add(gr_e760_lmd);
mg_lmd->Draw("AP");
mg_lmd->GetYaxis()->SetTitle("model system.err, %");
mg_lmd->GetXaxis()->SetTitle("P_{lab}, GeV/c");
 leg_sigt->Draw();
 c1.SaveAs("ModelsSysErr.pdf");
 c1.SaveAs("ModelsSysErr.root");
}
