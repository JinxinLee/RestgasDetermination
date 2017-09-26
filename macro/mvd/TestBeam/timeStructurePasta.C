/*
 * timeStructurePasta.C
 *
 *  Created on: 24.09.2017
 *      Author: Stockmanns
 */

std::vector<TH1*> CreateChannelHistos(int channelNr, TTree* t){
	std::vector<TH1*> histos;

	std::stringstream cutstream;
	cutstream << "data.fTimeBranch.channelId == " << channelNr;
	TCut cut = cutstream.str().c_str();
	std::cout << "Cut: " << cutstream.str() << std::endl;

	std::stringstream hstream;
	hstream << "hToT" << channelNr;
	TH1* tot = new TH1D(hstream.str().c_str(),"ToT Histogram ", 1025,-0.5,1024.5);
	std::stringstream tstream;
	tstream << "data.fToT >> " << hstream.str();
	t->Draw(tstream.str().c_str(), cut, "goff");
	histos.push_back(tot);

	std::stringstream hstream1;
	hstream1 << "hECoarse" << channelNr;
	TH1* eCoarse = new TH1D(hstream1.str().c_str(),"Coarse Energy : EnergyBranch", 1025,-0.5,1024.5);
	std::stringstream tstream1;
	tstream1 << "data.fEnergyBranch.t_coarse >> " << hstream1.str();
	t->Draw(tstream1.str().c_str(), cut, "goff");
	histos.push_back(eCoarse);

	std::stringstream hstream2;
	hstream2 << "hTCoarse" << channelNr;
	TH1* tCoarse = new TH1D(hstream2.str().c_str(),"Coarse Energy : TimeBranch", 1025,-0.5,1024.5);
	std::stringstream tstream2;
	tstream2 << "data.fTimeBranch.t_coarse >> " << hstream2.str();
	t->Draw(tstream2.str().c_str(), cut, "goff");
	histos.push_back(tCoarse);

	return histos;
}

int timeStructurePasta(TString fileName)
{
	TFile* f = new TFile(fileName, "READ");

	TFile* fOut = new TFile("output.root","RECREATE");

	TTree* t = (TTree*)f->Get("PastaTB");
	TClonesArray* pastadata = new TClonesArray("PndMvdPastaDigi");
	t->SetBranchAddress("data", &pastadata);

	TH1* hEvents = new TH1D("hEvents","Events per Channel", 65,-0.5,64.5);
	t->Draw("data.fTimeBranch.channelId >> hEvents");

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

	std::vector<TCanvas*> canvasses;
	for(int i = 0; i < 64; i++){
		std::stringstream channel;
		channel << "Channel: " << i;
		TCanvas* c2 = new TCanvas(channel.str().c_str());
		c2->Divide(2,2);
		canvasses.push_back(c2);
		std::vector<TH1*> channelResult = CreateChannelHistos(i, t);
		for (int j = 0; j < channelResult.size(); j++){
			c2->cd(j+1);
//			channelResult[j]->Draw();
			channelResult[j]->Write();
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
	g->Write();

	c1->cd(2);
	TGraph* g2 = new TGraph(times.size(), x, channelId);
	g2->Draw("AP");
	g2->Write();

	hEvents->Write();

	f->Close();
	fOut->Close();

	return 0;
}



