double CalcEventsPerYear(Int_t simEvents, Int_t interactionRate = 2*1E7, Double_t dutyCycle = 0.5)
{
	Int_t secondsPerYear = 60*60*24*365;
	return (double)interactionRate/simEvents * secondsPerYear * dutyCycle;
}


int drawMvdHistos(TString fileName, TString selector, Int_t columns = 2, Int_t mult = 1)
{
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/TransformHisto.C");
	gStyle->SetPalette(1);
//	gSystem->Load("libEve");
//	gROOT->LoadMacro("DrawEveHisto.C");

	TFile f(fileName);
	TGeoManager* fGeoMan = new TGeoManager("geoMan","geoMan");
	fGeoMan->Import("Mvd_GridUrqmdSim.root");
	PndGeoHandling gH(gGeoManager);

	TString id = gH->GetVolumeID(selector);

	//TEveManager::Create();

	TList* l = f.GetListOfKeys();
	TIter iter(l);
	TIter iter2(l);
	TObject* ob;
	int NHistos = 0;
	double max = 0, oldMax = 0;

	TString path;
	while(ob = iter()){
		if (TString(ob->GetName()).Contains(id)){
				NHistos++;
				TH2D* histo = (TH2D*)(f.Get(ob->GetName()));
				max = histo->GetMaximum();
				if (max > oldMax)
					oldMax = max;
		}
	}
	max = oldMax*1.1;

	TCanvas *c1 = new TCanvas();
	c1->Divide(columns, NHistos/columns);
	TH1D h1("h1",selector, 1000,0,max*CalcEventsPerYear(mult, 1E6));

	int i = 0;
	while(ob = iter2()){
		if (TString(ob->GetName()).Contains(id)){
			TH2D* histo = (TH2D*)(f.Get(ob->GetName()));
			TF1* f1 = new TF1("f1","1",-100,100);
			histo->Multiply(f1, CalcEventsPerYear(mult, 1E6));
			c1->cd(++i);
			histo->DrawCopy("colz");
			TH1D htemp(TransformHisto(histo,0,max*CalcEventsPerYear(mult,1E6)));
			h1.Add(&htemp);
		}
	}
	TCanvas *c2 = new TCanvas();
	c2->cd(1);
	h1.DrawCopy();
  return 0;

}
