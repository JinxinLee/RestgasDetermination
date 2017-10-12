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
	if (channelNr == -1)			// -1 creates global histograms for all channels
		cut = "";
//	std::cout << "Cut: " << cutstream.str() << std::endl;

	std::stringstream hstream;
	hstream << "hToT_" << channelNr;
	TH1* tot = new TH1D(hstream.str().c_str(),"ToT Histogram ", 1025,-0.5,1024.5);
	std::stringstream tstream;
	tstream << "data.fToT >> " << hstream.str();
	t->Draw(tstream.str().c_str(), cut, "goff");
	histos.push_back(tot);

	std::stringstream hstream1;
	hstream1 << "hECoarse_" << channelNr;
	TH1* eCoarse = new TH1D(hstream1.str().c_str(),"Coarse Energy : EnergyBranch", 1025,-0.5,1024.5);
	std::stringstream tstream1;
	tstream1 << "data.fEnergyBranch.t_coarse >> " << hstream1.str();
	t->Draw(tstream1.str().c_str(), cut, "goff");
	histos.push_back(eCoarse);

	std::stringstream hstream2;
	hstream2 << "hTCoarse_" << channelNr;
	TH1* tCoarse = new TH1D(hstream2.str().c_str(),"Coarse Energy : TimeBranch", 1025,-0.5,1024.5);
	std::stringstream tstream2;
	tstream2 << "data.fTimeBranch.t_coarse >> " << hstream2.str();
	t->Draw(tstream2.str().c_str(), cut, "goff");
	histos.push_back(tCoarse);

	return histos;
}

TH1* CreateErrorHisto(TFile* f){
	RunSummary* summary = (RunSummary*)f->Get("RunSummary");
	TH1D* histo = new TH1D("hOverview","Overview",9,-0.5,8.5);
	histo->Fill((int)0, summary->fCrcMatchCount);
	histo->Fill(1, summary->fCrcErrorCount);
	histo->Fill(2, summary->fSingleWordFrames);
	histo->Fill(3, summary->fAllPartialResets);
	histo->Fill(4, summary->fWrongHitCount);
	histo->Fill(5, summary->fWrongFrameCount);
	histo->Fill(6, summary->fMissingFrames);
	histo->Fill(7, summary->fSuperFrameCount);

	return histo;
}

int timeStructurePasta(TString fileName)
{
	TFile* f = new TFile(fileName, "READ");

	PndFileNameCreator creator(fileName.Data());
	std::string output = creator.GetCustomFileName("histos");

	TFile* fOut = new TFile(output.c_str(),"RECREATE");

	TTree* t = (TTree*)f->Get("PastaTB");
	TClonesArray* pastadata = new TClonesArray("PndMvdPastaDigi");
	t->SetBranchAddress("data", &pastadata);

	TH1* hEvents = new TH1D("hEvents","Events per Channel", 65,-0.5,64.5);
	t->Draw("data.fTimeBranch.channelId >> hEvents","","goff");

	std::vector<ULong64_t> times;
	std::vector<int> stripNr;
	for (int i = 0; i < t->GetEntriesFast(); i++){
		t->GetEntry(i);
		for (int j = 0; j < pastadata->GetEntries(); j++){
			PndMvdPastaDigi* digi = (PndMvdPastaDigi*)pastadata->At(j);
			times.push_back(digi->GetTimeStamp());
			stripNr.push_back(digi->GetChannelId(PndMvdPastaDigi::ENERGY));
//			std::cout << "TimeStamp: " << digi->GetTimeStamp() << std::endl;
		}
	}

	for(int i = -1; i < 64; i++){
		std::stringstream channel;
		std::vector<TH1*> channelResult = CreateChannelHistos(i, t);
		for (int j = 0; j < channelResult.size(); j++){
			channelResult[j]->Write();
		}

	}

	TH1* errorHisto = CreateErrorHisto(f);
	errorHisto->Write();

	const int sizeArray = times.size();
//	std::cout << "SizeArray " << sizeArray << std::endl;

	TGraph* g = new TGraph(sizeArray);
	g->SetName("GraphTimeVsIndex");
	TGraph* g2 = new TGraph(sizeArray);
	g2->SetName("GraphStripVsIndex");
//	std::cout << "After creating graph" << std::endl;
	for (int n = 0; n < sizeArray; n++){
		g->SetPoint(n, n, times[n]);
		g2->SetPoint(n,n,stripNr[n]);
	}
	hEvents->Write();
	g->Write();
	g2->Write();

//	TCanvas* c1 = new TCanvas();
//	c1->Divide(2,1);
//	c1->cd(1);
//	g->Draw("AP");


//	c1->cd(2);
//	g2->Draw("AP");
//	std::cout << "Writing Channel Structure" << std::endl;


	f->Close();
	fOut->Close();

	std::cout << "Finished conversion" << std::endl;

	return 0;
}



