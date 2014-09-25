//draw multiplicity  of events versus time stamp

void DrawMulti(){
  //	gROOT->Macro("$VMCWORKDIR/macro/lmd/Anastasia/test_macros/rootlogon.C");
	TFile *f = new TFile("/home/karavdina/soft/pandaroot/macro/lmd/DPM_el_inel_1_5/Lumi_digi_0.root","READ");
	TTree *trec = (TTree*)f->Get("cbmsim");
	TH2D *hmlt = new TH2D("hmlt","; time stamp, ns; multiplicity, trks/ev",60,0,1500,25,0,25);
	TH1D *hml = new TH1D("hml","; multiplicity, trks/ev",25,0,25);
       	double nEv =trec->Project("hmlt","LMDPixelDigis.GetNIndices():LMDPixelDigis.fTimeStamp");
	//	trec->Project("hml","LMDPixelDigis.GetNIndices()","LMDPixelDigis.fTimeStamp<39.","");
	trec->Project("hml","LMDPixelDigis.GetNIndices()","LMDPixelDigis.fTimeStamp<39.");
	//	nEv *=1e-2; 
	hmlt->SetZTitle("Number of events, %");
	double scl = 100/hmlt->Integral();
	hmlt->Scale(scl);
	hmlt->GetZaxis()->SetRangeUser(1e-6,100);
	hmlt->SetContour(1e4);
	cout<<"Scale: "<<scl<<endl;
	hmlt->Draw("colz");
	hml->Scale(100/hml->Integral());
	//	hml->Draw();
}
