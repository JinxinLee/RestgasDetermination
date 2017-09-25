/*
 * timeStructurePasta.C
 *
 *  Created on: 24.09.2017
 *      Author: Stockmanns
 */

int timeStructurePasta(TString fileName)
{
	TFile* f = new TFile(fileName, "READ");

	TTree* t = (TTree*)f->Get("PastaTB");
	TClonesArray* pastadata = new TClonesArray("PndMvdPastaDigi");
	t->SetBranchAddress("data", &pastadata);

	std::vector<ULong64_t> times;
	std::vector<int> stripNr;
	for (int i = 0; i < t->GetEntriesFast(); i++){
		t->GetEntry(i);
		for (int j = 0; j < pastadata->GetEntries(); j++){
			PndMvdPastaDigi* digi = (PndMvdPastaDigi*)pastadata->At(j);
			times.push_back(digi->GetTimeStamp());
			stripNr.push_back(digi->GetChannelId(PndMvdPastaDigi::ENERGY));
			std::cout << "TimeStamp: " << digi->GetTimeStamp() << std::endl;
		}
	}
	int y[times.size()];
	int x[times.size()];
	int channelId[times.size()];

	for (int n = 0; n < times.size(); n++){
		if ((int)times[n] > 0)
			y[n] = times[n];
		else y[n] = 0;
		channelId[n] = stripNr[n];
		x[n] = n;
	}
	TCanvas* c1 = new TCanvas();
	c1->Divide(2,1);
	c1->cd(1);
	TGraph* g = new TGraph(times.size(), x, y);
	g->Draw("AP");

	c1->cd(2);
	TGraph* g2 = new TGraph(times.size(), x, channelId);
	g2->Draw("AP");

	f->Close();

	return 0;
}



