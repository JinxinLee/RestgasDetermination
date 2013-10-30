void PDG_hystory(){
  int date[]={1970, 1974, 1976, 1978, 1980, 1982, 
	      1984, 1986, 1988, 1990, 1992, 1994, 1996, 1998, 
	      2000, 2002, 2004, 2006, 2008, 2010, 2012};
  int pages[]={127, 198, 246, 250, 286, 294, 
	       304, 350, 486, 515, 574, 650, 720, 794, 
	       878, 974, 1109, 1232, 1340, 1422, 1528};

  // since 1996 avaliable on the web and 'Review of Particle Properties' -> 'Review of Particle Physics' 

  TGraph *grPDGhy = new TGraph(21,date,pages);
  grPDGhy->SetMarkerStyle(22);
  grPDGhy->SetMarkerColor(8);
  grPDGhy->SetMarkerSize(3.5);
  grPDGhy->GetYaxis()->SetTitle("width, pages");
  grPDGhy->GetXaxis()->SetTitle("year");
  grPDGhy->Draw("AP");


}
