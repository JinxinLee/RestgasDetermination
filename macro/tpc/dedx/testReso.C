
void testReso()
{
	gROOT->ProcessLine(".L macro/tpc/dedx/Coordinates.C++");
	gROOT->ProcessLine(".L macro/tpc/dedx/Resolution.C++");
	Resolution reso("test");
	reso.Fill(1,2);
	reso.Fill(2,4);
	reso.Fill(3,6);
	reso.Fill(4,8);
	
	TCanvas *can1=new TCanvas("d1","d1");
	TGraph *pGraph=reso.CreateGraph();
	pGraph->Draw("ALP");
	
	
	Resolution reso2("test");
	reso2.Fill(1,2,1);
	reso2.Fill(2,4,1);
	reso2.Fill(3,6,1);
	reso2.Fill(4,8,1);
	reso2.Fill(1,3,2);
	reso2.Fill(2,5,2);
	reso2.Fill(3,7,2);
	reso2.Fill(4,9,2);	
	
	TCanvas *can2=new TCanvas("d2","d2");
	TGraph2D *pGraph2D=reso2.CreateGraph2D();
	pGraph2D->Draw("surf1");
	TCanvas *can3=new TCanvas("c3","c3");
	TGraph *pGraph2=reso2.CreateGraph();
	pGraph2->Draw("ALP");
	
}
