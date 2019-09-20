int anaMCTrackInfo(Int_t nEvents = 0, TString prefix="../data/evtcomplete")
{

	// Number of events to process
//  Int_t nEvents = 0;  // if 0 all the vents will be processed

    TString simFile = prefix + "_sim.root";
    TString mcInfoFile = prefix + "_mctrackinfo.root";
    TString histoOutFile = prefix + "_mctrackinfoplots.root";

    TFile *f = new TFile(mcInfoFile);
    TFile *fout = new TFile(histoOutFile, "RECREATE");
    TTree* t = (TTree*)f->Get("pndsim");
    t->AddFriend("pndsim", simFile);

    TH1* hThetaPrim = new TH1D("hThetaPrim","Theta Distribution Primaries", 360, -180, 180);
    TH1* hRangePrim = new TH1D("hRangePrim", "Range of Primaries", 999,1,1000);
    TH1* hTotalPoints = new TH1D("hTotalPoints","Total number of hit points",100,0,100);
    TH2* hMvdTheta  = new TH2D("hMvdTheta", "Mvd hits vs. theta", 360, -180, 180, 100, 0, 100);
    TH2* hSttTheta  = new TH2D("hSttTheta", "Stt hits vs. theta", 360, -180, 180, 100, 0, 100);
    TH2* hGemTheta  = new TH2D("hGemTheta", "Gem hits vs. theta", 360, -180, 180, 100, 0, 100);
    TH2* hFtsTheta  = new TH2D("hFtsTheta", "Fts hits vs. theta", 360, -180, 180, 100, 0, 100);
    TH2* hTotalTheta  = new TH2D("hTotalTheta", "Total hits vs. theta", 360, -180, 180, 100, 0, 100);


    TH1* hThetaHits = new TH1D("hThetaHits","Theta Distribution Primaries with > 4 hits", 360, -180, 180);
    TH1* hThetaEff = new TH1D("hThetaEff","Theta Distribution Primaries Efficiency", 360, -180, 180);

    TString thetaDeg = "TMath::RadToDeg()*(TMath::ATan2(TMath::Sqrt(MCTrack.fPx^2+MCTrack.fPy^2), MCTrack.fPz))";

    t->Draw(thetaDeg+">>hThetaPrim","MCTrackInfo.fStage==0","");
    t->Draw(thetaDeg+">>hThetaHits","MCTrackInfo.fStage==0&&MCTrackInfo.fTotalPoints>4","");
    t->Draw("MCTrackInfo.fDecayVertex.Mag()>>hRangePrim","MCTrackInfo.fStage==0");
    t->Draw("MCTrackInfo.fTotalPoints>>hTotalPoints","MCTrackInfo.fStage==0");

    t->Draw("MCTrackInfo.fMVDPoints:"+thetaDeg+">>hMvdTheta","MCTrackInfo.fStage==0","colz");
    t->Draw("MCTrackInfo.fSTTPoints:"+thetaDeg+">>hSttTheta","MCTrackInfo.fStage==0","colz");
    t->Draw("MCTrackInfo.fGEMPoints:"+thetaDeg+">>hGemTheta","MCTrackInfo.fStage==0","colz");
    t->Draw("MCTrackInfo.fFTSPoints:"+thetaDeg+">>hFtsTheta","MCTrackInfo.fStage==0","colz");
    t->Draw("MCTrackInfo.fTotalPoints:"+thetaDeg+">>hTotalTheta","MCTrackInfo.fStage==0","colz");

    hThetaEff->Divide(hThetaHits, hThetaPrim);
//
    TTreeReader reader (t);
    TTreeReaderArray<PndMCTrack> mcTracks(reader, "MCTrack");
    TTreeReaderArray<PndMCTrackInfo> mcTrackInfos(reader, "MCTrackInfo");

    while (reader.Next()){
        auto mcTrackInfo = mcTrackInfos.begin();
        for (auto mcTrack : mcTracks){
            std::cout << mcTrack;
            std::cout << *mcTrackInfo++ << std::endl;
        }
    }
    fout->cd();
    hThetaPrim->Write();
    hThetaHits->Write();
    hThetaEff->Write();
    hRangePrim->Write();
    hTotalPoints->Write();

    hMvdTheta->Write();
    hSttTheta->Write();
    hGemTheta->Write();
    hFtsTheta->Write();
    hTotalTheta->Write();

  return 0;
}
